#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""COEX driver for Dronepoint"""

import serial
import os, sys
import json
from enum import Enum, unique
import threading
import rospy
import rospkg

from socket import *
import socket

from std_msgs.msg import Header
from cs_driver_srvs.srv import DriverCommand, DriverCommandProxy, DriverCommandProxyRequest, \
    DriverCommandProxyResponse, DriverCommandRequest, DriverCommandResponse
from cs_driver_msgs.msg import DriverState

from cs_driver_software.CsAction import CsAction, CsActionSequenceExecutor
import dronepoint_scenarios


udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
udp_socket.settimeout(0.150)
host = '0.0.0.0'
port = 7111
addr = (host,port)
COMMCON = ''
state_buffer = ''  # Buffer for the incoming JSON state message

udp_socket.bind(addr)


STATE_BUFFER_LIMIT = 4096  # JSON state message buffer limit

# Prevents subscriber's thread to create new sequence executors during node shutdown
node_shutdown_event = threading.Event()

error_event = threading.Event()  # Charging station sequence failure event

# JSON state which came from Arduino is being saved here
current_hardware_state = {}

# Hardware state override disables driver state updates during the action sequence
# HINT: None - enable driver state updates
hardware_state_override = None

# For synchronisation between threads for: current_hardware_state, hardware_state_override
hardware_state_lock = threading.Lock()

# DriverState publisher object (will be created during initialisation)
state_publisher = None

# Arduino serial port (will be created during initialisation)
arduino_port = None

# Arduino executor class (will be initialised on the first charging station action sequence)
# WARNING: On each action sequence that will be a new object
action_sequence_executor = None

# Lock for sequence executor variable
# HINT: Executors change each other on new commands. It's possible multiple thread access from
#   the main thread and the command listener.
action_sequence_executor_lock = threading.Lock()

# Dict with cells states which is loaded from file and keeps info about payload ids and empty cells
cells_state = None

# Overall system state which will be published to ROS-topic and than to Mavlink
# HINT: It's better to report to MAVLink charging station node only status updates
# (will be initialised on the first Arduino state heartbeat) (str)
dronepoint_publishing_state = None
# Dronepoint metastate lock (multiple thread accessible)
dronepoint_publishing_state_lock = threading.Lock()


def sendUDP():
    global COMMCON
    while True:
      try:
          #recvfrom - получает UDP сообщения
          COMMCON, addr = udp_socket.recvfrom(1024)
          #sendto - передача сообщения JSON по UDP
          udp_socket.sendto((state_buffer.encode()), addr)
          UDPex()
          break
      except socket.timeout:
          break

def UDPex():
        global COMMCON
        if   COMMCON == 'HDO'+ '\n':
             params = {}
             X=0
             Y=0
             Z=0
             nkr=0
             cmmd='hatch_delivery_open'

             params = {'x': X, 'y': Y, 'z': Z, 'nkr': nkr}
             arduino_send_command(cmmd, params)
             COMMCON = ''
             return DriverCommandResponse(
                    DriverCommandResponse.SUCCESS, 'Command was proxied to Dronepoint'
                )

        elif COMMCON == 'HDC'+ '\n':
             params = {}
             X=0
             Y=0
             Z=0
             nkr=0
             cmmd='hatch_delivery_close'

             params = {'x': X, 'y': Y, 'z': Z, 'nkr': nkr}
             arduino_send_command(cmmd, params)
             COMMCON = ''
             return DriverCommandResponse(
                    DriverCommandResponse.SUCCESS, 'Command was proxied to Dronepoint'
                )




def set_and_pub_dronepoint_state(new_state, msg='', update_status_text=False):
    """
    Sets and publishes to ROS topic new overall dronepoint state
    """
    global dronepoint_publishing_state
    with dronepoint_publishing_state_lock:
        if dronepoint_publishing_state != new_state or update_status_text:
            dronepoint_publishing_state = new_state
            # Publish state into driver's state ROS topic
            state_publisher.publish(header=Header(stamp=rospy.Time.now()), state=new_state, status_string=msg)
            rospy.loginfo('New publishing dronepoint state: %s (%s)',
                            new_state, [key for key, val in DriverState.__dict__.items() if val == new_state][0])

def get_dronepoint_publishing_state():
    """
    Returns a dronepoint metastate in a thread-safe manner.
    WARNING: Asynchronous call (CsActionExecutor)!
    :return: An actual dronepoint metastate (str or unicode)
    """
    with dronepoint_publishing_state_lock:  # Synchronising using a lock
        return dronepoint_publishing_state


class ArduinoPlatformDriverState:
    """
    Namespace for Platform states.
    """
    GOING_TO_CELL = 'going_to_cell'
    ON_CELL = 'on_cell'
    LOADING_PAYLOAD_CELL = 'loading_payload_cell'
    UNLOADING_PAYLOAD_CELL = 'unloading_payload_cell'
    LOADING_CHARGING_CELL = 'loading_charging_cell'
    UNLOADING_CHARGING_CELL = 'unloading_charging_cell'
    GOING_TO_HATCH = 'going_to_hatch'
    OPENING_HATCH = 'opening_hatch'
    IN_HATCH = 'in_hatch'
    CLOSING_HATCH = 'closing_hatch'
    UNLOADING_TO_USER = 'unloading_to_user'
    GETTING_FROM_USER = 'getting_from_user'
    STANDBY = 'standby'
    PARKING = 'parking'
    PARKED = 'parked'
    STOPPED = 'stopped'
    UNKNOWN = 'unknown'


ArduinoZAxisState = {
    0: "Z0",
    1: "Z1",
    2: "Z2",
    3: "Z3",
    4: "Z4",
    5: "Z5",
    6: "Z6",
    7: "Z7",
    8: "Z8",
    9: "Z9",
    10: "Z10",
    11: "Z11",
}

ArduinoXAxisState = {
    0: "X0",
    1: "X1",
    2: "X2",
    3: "X3",
    4: "X4",
    5: "Xkr0", # Charging cells
    6: "Xkr1",
}

ArduinoYFlagState = {
    1: "1",
    2: "2",
    3: "3",
}

ArduinoYAxisState = {
    1: "Y1zad",
    2: "Y2center",
    3: "Y3pered",
}


class ArduinoPlatformPayloadDriverState:
    """
    Namespace for payload detector states.
    """
    LOADED = 'loaded'
    EMPTY = 'free'


class ArduinoTopHatchDriverState:
    """
    Namespace for top (drone) hatch states.
    """
    OPEN = 'open'
    CLOSED = 'close'


class ArduinoBottomHatchDriverState:
    """
    Namespace for bottom (user) hatch states.
    """
    OPEN = 'open'
    CLOSED = 'close'


class ArduinoLockDriverState(object):
    """
    Namespace for drone locking mechanizm states.
    """
    RELEASED = 'uncenter'
    LOCKED = 'center'
    RELEASING = 'moveuncenter'
    LOCKING = 'movecenter'
    UNKNOWN = 'unknown'

class ArduinoKRposDriverState(object):
    """
    Namespace 
    ArduinoDriverStateKey.KRPOS: ArduinoKRposDriverState.KR5
    """
    KR0 = 'KR0'
    KR1 = 'KR1'
    KR2 = 'KR2'
    KR3 = 'KR3'
    KR4 = 'KR4'
    KR5 = 'KR5'
    KR6 = 'KR6'


class ArduinoDNposDriverState(object):
    """
    #{"pDN":dn} {"pOV":none}
    Namespace 
    ArduinoDriverStateKey.PDN: ArduinoDNposDriverState.DN
    """
    DN = 'dn'
    NONE = 'none'

class ArduinoOVposDriverState(object):
    """
    #{"pDN":none} {"pOV":ov}
    Namespace 
    ArduinoDriverStateKey.POV: ArduinoOVposDriverState.NONE
    """
    OV = 'ov'
    NONE = 'none'

class ArduinoSTldDriverState(object):
    """
    #{"Pld":none} {"Pld":compl}
    Namespace 
    ArduinoDriverStateKey.PLD: ArduinoSTldDriverState.COMPL
    """
    COMPL = 'compl'
    NONE  = 'none'

class ArduinoSTudDriverState(object):
    """
    #{"Pud":none} {"Pud":compl}
    Namespace 
    ArduinoDriverStateKey.PUD: ArduinoSTudDriverState.COMPL
    """
    COMPL = 'compl'
    NONE  = 'none'

class ArduinoSTuntuDriverState(object):
    """
    #{"Puntu":none} {"Puntu":compl}
    Namespace 
    ArduinoDriverStateKey.PUNTU: ArduinoSTuntuDriverState.COMPL
    """
    COMPL = 'compl'
    NONE  = 'none'

class ArduinoSTgetfuDriverState(object):
    """
    #{"Pgetfu":none} {"Pgetfu":compl}
    Namespace 
    ArduinoDriverStateKey.PGETFU: ArduinoSTgetfuDriverState.COMPL
    """
    COMPL = 'compl'
    NONE  = 'none'

class ArduinoSTvOpenDriverState(object):
    """
    #{"Popn":"none"} {"Popn":"compl"}
    Namespace 
    ArduinoDriverStateKey.PVOPN: ArduinoSTvOpenDriverState.COMPL
    """
    COMPL = 'compl'
    NONE  = 'none'

class ArduinoSTvCloseDriverState(object):
    """
    #{"Pcls":"none"} {"Pcls":"compl"}
    Namespace 
    ArduinoDriverStateKey.PVCLS: ArduinoSTvCloseDriverState.COMPL
    """
    COMPL = 'compl'
    NONE  = 'none'

class ArduinoSTchanDriverState(object):
    """
    #{"Pchan":"none"} {"Pchan":"compl"}
    Namespace 
    ArduinoDriverStateKey.PCHAN: ArduinoSTchanDriverState.COMPL
    """
    COMPL = 'compl'
    NONE  = 'none'
   

class ChargingCellState:
    """
    Namespace for charging cell states
    """
    EMPTY = 'free'
    LOADED = 'loaded'


class ArduinoCommand(object):
    """
    Namespace for the Arduino commands.
    """
    # Lid commands
    LID_OPEN = 'open'  # Open charging station lid
    LID_CLOSE = 'close'  # Close charging station lid
    LID_STOP = 'stop'  # Stop lid motor

    # Lock commands
    LOCK_RELEASE = 'uncentr'  # Release charging station lock
    LOCK_LOCK = 'centr'  # Lock charging station lock
    LOCK_STOP = 'stopcentr'  # Stop lock motor

    # Platform commands
    OPEN_TOP_HATCH = 'top_hatch_state_removing'
    CLOSE_TOP_HATCH = 'top_hatch_state_install'
    GOTO_TOP_HATCH = 'goto_top_hatch'
    GOTO_CELL = 'goto_cell'
    LOAD_CHARGING_CELL = 'chargingKR1_cover_install'
    UNLOAD_CHARGING_CELL = 'chargingKR1_cover_removing'
    LOAD_PAYLOAD_CELL = 'loadingCELL'
    UNLOAD_PAYLOAD_CELL = 'unloadingCELL'
    GET_FROM_USER = 'get_from_user'
    UNLOAD_TO_USER = 'unload_to_user'
    PARK_PLATFORM = 'park_platform'
    CMO_LD = 'cmOpld' # internal instruction LOAD_DRONE
    CMO_UD = 'cmOpud' # internal instruction UNLOAD_DRONE
    CMO_UNTU = 'cmOpuntu' # internal instruction UNLOAD_TO_USER
    CMO_GETFU = 'cmOpgetfu'  # internal instruction GET_FROM_USER
    CMO_OPEN = 'cmOopn' # internal instruction OPEN
    CMO_CLOSE = 'cmOcls'  # internal instruction CLOSE
    CMO_CHAN = 'cmOchan'  # internal instruction CHANGING_BATTERY
    OV = 'posOV' #{'x': 2, 'z': 1, 'y': 3} otsek vydachi
    DN = 'posDN' #{'x': 2, 'z': 5, 'y': 2} positsia operatsiy c dronom
    KR0 = 'posKR0' #{'x': 0, 'z': 5, 'y': 2} KR0
    KR1 = 'posKR1' #{'x': 1, 'z': 5, 'y': 2} KR1
    KR2 = 'posKR2' #{'x': 3, 'z': 5, 'y': 2} KR2
    KR3 = 'posKR3' #{'x': 4, 'z': 5, 'y': 2} KR3
    KR4 = 'posKR4' #{'x': 0, 'z': 5, 'y': 2} KR4
    KR5 = 'posKR5' #{'x': 1, 'z': 5, 'y': 2} KR5
    KR6 = 'posKR6' #{'x': 3, 'z': 5, 'y': 2} KR6
    KR7 = 'posKR7' #{'x': 4, 'z': 5, 'y': 2} KR7
    STOP = 'stopAll'

    # Drone and payload
    GET_PAYLOAD_FROM_DRONE = 'container_removing'
    INSERT_PAYLOAD_INTO_DRONE = 'container_insert'

    # Platform payload lock commands
    LOCK_PAYLOAD = 'lock_payload'
    RELEASE_PAYLOAD = 'release_payload'

    # Charging cell commands
    LOCK_CHARGING_CELL_LOCK = 'lock_charging_cell_lock'
    RELEASE_CHARGING_CELL_LOCK = 'release_charging_cell_lock'

    # Top hatch locking mechanizm commands
    LOCK_TOP_HATCH_LOCK = 'lock_top_hatch_lock'
    RELEASE_TOP_HATCH_LOCK = 'release_top_hatch_lock'

    # Bottom hatch commands
    OPEN_BOTTOM_HATCH = 'hatch_delivery_open'
    CLOSE_BOTTOM_HATCH = 'hatch_delivery_close'

    # User cell locking mechanizm commands
    LOCK_USER_CELL_LOCK = 'lock_user_cell_lock'
    RELEASE_USER_CELL_LOCK = 'release_user_cell_lock'

# Array for SERVICE scenario which will pass the code of one of
# the following operations
proxying_commands_codes = [
    ArduinoCommand.LOCK_RELEASE, #0
    ArduinoCommand.LOCK_LOCK, #1
    ArduinoCommand.LOCK_STOP, #2

    # Platform commands
    ArduinoCommand.OPEN_TOP_HATCH, #3
    ArduinoCommand.CLOSE_TOP_HATCH, #4
    ArduinoCommand.GOTO_CELL, #5
    ArduinoCommand.LOAD_CHARGING_CELL, #6
    ArduinoCommand.UNLOAD_CHARGING_CELL, #7
    ArduinoCommand.LOAD_PAYLOAD_CELL, #8
    ArduinoCommand.UNLOAD_PAYLOAD_CELL, #9
    ArduinoCommand.GET_FROM_USER, #10
    ArduinoCommand.UNLOAD_TO_USER, #11
    ArduinoCommand.STOP, #12

    # Drone and payload
    ArduinoCommand.GET_PAYLOAD_FROM_DRONE, #13
    ArduinoCommand.INSERT_PAYLOAD_INTO_DRONE, #14

    # Platform payload lock commands
    ArduinoCommand.LOCK_PAYLOAD, #15
    ArduinoCommand.RELEASE_PAYLOAD, #16

    # Charging cell commands
    ArduinoCommand.LOCK_CHARGING_CELL_LOCK, #17
    ArduinoCommand.RELEASE_CHARGING_CELL_LOCK, #18

    # Top hatch locking mechanizm commands
    ArduinoCommand.LOCK_TOP_HATCH_LOCK, #19
    ArduinoCommand.RELEASE_TOP_HATCH_LOCK, #20

    # Bottom hatch commands
    ArduinoCommand.OPEN_BOTTOM_HATCH, #21
    ArduinoCommand.CLOSE_BOTTOM_HATCH, #22

    # User cell locking mechanizm commands
    ArduinoCommand.LOCK_USER_CELL_LOCK, #23
    ArduinoCommand.RELEASE_USER_CELL_LOCK, #24

    ArduinoCommand.GOTO_CELL, #25 - this is for going to charging cell - 
    # bacause we have another parameter set for it (Xkr coordinate instead of X)
]


class ArduinoDriverStateKey(object):
    """
    Namespace for the status message keys which came from Arduino UART.
    """
    LID = 'statelid'  # DriverState message charging station lid state key
    LOCK = 'statelock'  # DriverState message charging station lock state key
    PLATFORM = 'platform_state'  # Main platform state
    KRPOS = 'cover_position'  # ArduinoDriverStateKey.KRPOS: ArduinoKRposDriverState.KR5
    #{"pDN":dn} {"pOV":none}
    POV = 'pOV' # positsia otseka vydachi ArduinoDriverStateKey.POV: ArduinoOVposDriverState.NONE
    PDN = 'pDN' # positsia operatsiy s dronom ArduinoDriverStateKey.PDN: ArduinoDNposDriverState.DN
    PLD = 'Pld' # pinding LOAD_DRONE    ArduinoDriverStateKey.PLD: ArduinoSTldDriverState.COMPL
    PUD = 'Pud' # pinding UNLOAD_DRONE  ArduinoDriverStateKey.PUD: ArduinoSTudDriverState.COMPL
    PUNTU = 'Puntu' # pinding UNLOAD_TO_USER  ArduinoDriverStateKey.PUNTU: ArduinoSTuntuDriverState.COMPL
    PGETFU = 'Pgetfu' # pinding GET_FROM_USER  ArduinoDriverStateKey.PGETFU: ArduinoSTgetfuDriverState.COMPL
    PVOPN = 'Popn'    # pinding OPEN ArduinoDriverStateKey.PVOPN: ArduinoSTvOpenDriverState.COMPL
    PVCLS = 'Pcls'    # pinding CLOSE ArduinoDriverStateKey.PVCLS: ArduinoSTvCloseDriverState.COMPL
    PCHAN = 'Pchan'   # pinding CHANGING_BATTERY  ArduinoDriverStateKey.PCHAN: ArduinoSTchanDriverState.COMPL

    Z_AXIS_STATE = 'StatusZ' # Axises states
    X_AXIS_STATE = 'StatusX'
    Y_FLAG_STATE = 'YFlag' # StatusY will not be actually changed during "GOTO_CELL" platform moving
    Y_AXIS_STATE = 'StatusY' # Actual Y position of the platform

    # State of platform payload detectors (whether payload is on the platform or not)
    PLATFORM_PAYLOAD = 'platform_payload_state'
    # State of magnets which hold the payload on platform
    PLATFORM_PAYLOAD_LOCK = "platform_payload_lock_state"
    TOP_HATCH = 'top_hatch_state'  # Top hatch (drone loading) state
    BOTTOM_HATCH = 'bottom_hatch_state'  # Bottom hatch (user loading) state
    # State of lock which holds container while user interoperates with payload
    USER_CELL_LOCK = 'user_cell_lock_state'
    CHARGING_CELL = 'chargingKR1_cells_states'  # State of charging cell (just one at the moment)
    SCALES = 'scales_state'  # Weight measuring instrument state
    LAST_COMMAND = 'last_command'  # Feedback from the Dronepoint about the last command


# A namespace for scenarios
class ScenariosHandler(object):
    
    # Here will be saved parameters depending on what came wrom mavlink
    current_scenario_parameters = None

    # Flag to determine if scenario is being executed at the moment
    scenario_executing = False
    scenario_executing_lock = threading.Lock()

    @classmethod
    def set_scenario_executing(cls, value):
        with cls.scenario_executing_lock:
            cls.scenario_executing = value

    @classmethod
    def is_scenario_executing(cls):
        with cls.scenario_executing_lock:
            return cls.scenario_executing

    # Pending states for current executable scenario command
    current_pending_states = None
    # Event for waiting current pending states
    pending_states_came_event = threading.Event()

    @classmethod
    def check_pending_states(current_class, command):
        """
        Check current executable scenario command's pending states
        in JSON which last came from UART
        """
        global current_hardware_state, hardware_state_lock

        pending_states = command['pending_states']

        # This hard-to-understand place is needed to win the X and Xkr parameters
        if command["command"] ==  ArduinoCommand.GOTO_CELL  and "cell_coordinates" in command:
            pending_states = {
                ArduinoDriverStateKey.X_AXIS_STATE: ArduinoXAxisState[command["cell_coordinates"]["x"]] if "x" in command["cell_coordinates"] else command["cell_coordinates"]["Xkr"],
                ArduinoDriverStateKey.Y_FLAG_STATE: ArduinoYFlagState[command["cell_coordinates"]["y"]],
                ArduinoDriverStateKey.Z_AXIS_STATE: ArduinoZAxisState[command["cell_coordinates"]["z"]],
            }
        elif command["command"] ==  ArduinoCommand.GOTO_CELL:
            if command['cell_type'] == dronepoint_scenarios.CellTypes.PAYLOAD:
                pending_states = {
                    ArduinoDriverStateKey.X_AXIS_STATE: ArduinoXAxisState[current_class.current_scenario_parameters["payload_cell_coordinates"]["x"]],
                    ArduinoDriverStateKey.Y_FLAG_STATE: ArduinoYFlagState[current_class.current_scenario_parameters["payload_cell_coordinates"]["y"]],
                    ArduinoDriverStateKey.Z_AXIS_STATE: ArduinoZAxisState[current_class.current_scenario_parameters["payload_cell_coordinates"]["z"]],
                }
            elif command['cell_type'] == dronepoint_scenarios.CellTypes.CHARGING:
                pending_states = {
                    ArduinoDriverStateKey.X_AXIS_STATE: "Xkr" + str(current_class.current_scenario_parameters["charging_cell_coordinates"]["Xkr"]),
                    ArduinoDriverStateKey.Y_FLAG_STATE: ArduinoYFlagState[current_class.current_scenario_parameters["charging_cell_coordinates"]["y"]],
                    ArduinoDriverStateKey.Z_AXIS_STATE: ArduinoZAxisState[current_class.current_scenario_parameters["charging_cell_coordinates"]["z"]],
                }

        rospy.logdebug("Pending states: %s", pending_states)

        for key, value in pending_states.items():
            with hardware_state_lock:
                if key in current_hardware_state:
                    if current_hardware_state[key] != value:
                        return False
                else:
                    rospy.logwarn('check_pending_states: key "%s" no found in current_hardware_state!', key)
                    return False
        return True

    @classmethod
    def check_current_pending_states(cls):
        if cls.current_pending_states is not None:
            return cls.check_pending_states(cls.current_pending_states)
        else:
            return False

    @classmethod
    def run_uart_command(current_class, command):
        """
        Runs scenario command with SEND_UART type (sends command to Arduino and waits for some states)
        """
        rospy.loginfo("Executing command %s...", command['command'])

        params = ''
        # If we have cell coordinates in command structure - use it
        if command["command"] ==  ArduinoCommand.GOTO_CELL and "cell_coordinates" in command:
            params = command["cell_coordinates"]
        elif command["command"] ==  ArduinoCommand.GOTO_CELL:
            # If we have not coordinates in command structure - take it from 
            # mavlink command's parameters
            if command['cell_type'] == dronepoint_scenarios.CellTypes.PAYLOAD:
                params = current_class.current_scenario_parameters["payload_cell_coordinates"]
            elif command['cell_type'] == dronepoint_scenarios.CellTypes.CHARGING:
                params = current_class.current_scenario_parameters["charging_cell_coordinates"]

        # Sending next command from scenario to serial port
        arduino_send_command(command['command'], params)
        # Time point to detect timeout
        command_start_time = rospy.get_time()

        # Let's wait until command will be done or timed out (or we get a "shutdown" signal)
        while not rospy.is_shutdown() and current_class.check_pending_states(command) == False and rospy.get_time() - command_start_time <= command['timeout']:
            rospy.sleep(1)

        if current_class.check_pending_states(command) == True:
            return dronepoint_scenarios.CommandExitCodes.SUCCESS
        elif rospy.is_shutdown(): # If we have shutdown signal - exit immediatly
            return dronepoint_scenarios.CommandExitCodes.SHUTDOWN
        else:
            return dronepoint_scenarios.CommandExitCodes.TIMEOUT
        # TODO: Error state handling

    @classmethod
    def run_sleep_command(current_class, command):
        """
        Just sleep for command['timeout'] seconds
        """

        rospy.loginfo("Sleeping for %s seconds...", command['timeout'])

        # Time point to detect timeout
        command_start_time = rospy.get_time()

        # Let's wait until the time will out (or we get a "shutdown" signal)
        while not rospy.is_shutdown() and rospy.get_time() - command_start_time <= command['timeout']:
            rospy.sleep(1)

        if rospy.is_shutdown(): # If we have shutdown signal - exit immediatly
            return dronepoint_scenarios.CommandExitCodes.SHUTDOWN
        else:
            return dronepoint_scenarios.CommandExitCodes.SUCCESS
        # TODO: Error state handling

    @classmethod
    def run_get_info_command(current_class, command):
        pass

    @classmethod
    def dronepoint_command_callback(current_class, req):
        """
        MAVLink charging station node command callback.
        WARNING: Asynchronous callback (ROS service server)!
        :param req: A request from the MAVLink charging station node (DriverCommandRequest)
        :return: None
        """
        
        if req.command == DriverCommandRequest.STOP:
            rospy.loginfo(
                'Stop command requested. Executing STOP sequence immediatly!')
            arduino_stop()
            return DriverCommandResponse(DriverCommandResponse.SUCCESS, '')
        elif get_dronepoint_publishing_state() == DriverState.ERROR and req.command == DriverCommandRequest.RESTORE:
            rospy.loginfo('Restoring from error mode')
            set_and_pub_dronepoint_state(DriverState.STANDBY)
            return DriverCommandResponse(DriverCommandResponse.SUCCESS, 'Restoring from error mode')
        
        elif req.command == DriverCommandRequest.SERVICE:
            command_code = req.param1
            params = {}
            if command_code < len(proxying_commands_codes):
                if command_code == 5: # GOTO payload CELL
                    params = {'x': req.param2, 'y': req.param3, 'z': req.param4}
                elif command_code == 25: # GOTO charging CELL
                    params = {'Xkr': req.param2, 'y': req.param3, 'z': req.param4}

                #def arduino_send_command(command, params={}):
                #arduino_port.write(json.dumps(
                #  {"command": command, "params": params}) + '\n')
                arduino_send_command(proxying_commands_codes[command_code], params)
                return DriverCommandResponse(
                    DriverCommandResponse.SUCCESS, 'Command was proxied to Dronepoint'
                )
            else:
                return DriverCommandResponse(
                    DriverCommandResponse.ERROR,
                    'Wrong proxying command ID'
                )
        elif get_dronepoint_publishing_state() == DriverState.ERROR:
            rospy.logwarn('Attempt to run scenario in error mode!')
            return DriverCommandResponse(
                DriverCommandResponse.ERROR,
                'Can\'t run new scenario in ERROR mode!'
            )
        elif req.command == DriverCommandRequest.RESTORE:
            rospy.logwarn('Not in error mode, nothing to restore')
            return DriverCommandResponse(
                DriverCommandResponse.ERROR,
                'Not in error mode, nothing to restore'
            )
        elif current_class.is_scenario_executing():
            rospy.loginfo(
                'Can\'t run new scenario while another is in process')
            return DriverCommandResponse(
                DriverCommandResponse.BUSY,
                'Another scenario executing is in process'
            )
        elif req.command == DriverCommandRequest.LOAD_DRONE:
        #vynet iz iacheyki snimet kryshku zagruzit v dron    
            if req.param1 < 10 and req.param2 < 10 and req.param3 < 10 and req.param4 < 10:
                cmmd = 'prm_ld'
                params = {}
                params = {'x': req.param1, 'y': req.param2, 'z': req.param3, 'nkr': req.param4}
                #def arduino_send_command(command, params={}):
                #arduino_port.write(json.dumps(
                #  {"command": command, "params": params}) + '\n')
                #{"command": "prm_ld", "params": {"x": 2, "y": 2, "z": 1, "nkr": 1}}
                arduino_send_command(cmmd, params)
                rospy.sleep(2)
                set_and_pub_dronepoint_state(DriverState.LOADING_DRONE)
                return current_class.run_scenario(dronepoint_scenarios.LoadDroneScenario)
            else:
                return DriverCommandResponse(
                    DriverCommandResponse.ERROR,
                    'Wrong payload or charging cell numbers'
                )
        elif req.command == DriverCommandRequest.UNLOAD_DRONE:
        #snimet s drona vstavit krishku zagruzut v iacheyku 
            if req.param1 < 10 and req.param2 < 10 and req.param3 < 10 and req.param4 < 10:
                cmmd = 'prm_ud'
                params = {}
                params = {'x': req.param1, 'y': req.param2, 'z': req.param3, 'nkr': req.param4}
                #def arduino_send_command(command, params={}):
                #arduino_port.write(json.dumps(
                #  {"command": command, "params": params}) + '\n')
                #{"command": "prm_ud", "params": {"x": 2, "y": 2, "z": 1, "nkr": 1}}
                arduino_send_command(cmmd, params)
                rospy.sleep(2)
                set_and_pub_dronepoint_state(DriverState.UNLOADING_DRONE)
                return current_class.run_scenario(dronepoint_scenarios.UnloadDroneScenario)
            else:
                return DriverCommandResponse(
                    DriverCommandResponse.ERROR,
                    'Wrong payload or charging cell numbers'
                )
        elif req.command == DriverCommandRequest.UNLOAD_TO_USER:
            #vynet iz iacheyki i vygruzit polzavatelu
            if req.param1 < 10 and req.param2 < 10 and req.param3 < 10 and req.param4 < 10:
                cmmd = 'prm_untu'
                params = {}
                params = {'x': req.param1, 'y': req.param2, 'z': req.param3}
                #def arduino_send_command(command, params={}):
                #arduino_port.write(json.dumps(
                #  {"command": command, "params": params}) + '\n')
                #{"command": "prm_untu", "params": {"x": 2, "y": 2, "z": 1}}
                arduino_send_command(cmmd, params)
                rospy.sleep(2)
                set_and_pub_dronepoint_state(DriverState.UNLOADING_TO_USER)
                return current_class.run_scenario(dronepoint_scenarios.UnloadToUserScenario)
            else:
                return DriverCommandResponse(
                    DriverCommandResponse.ERROR,
                    'Wrong payload cell number'
                )
        elif req.command == DriverCommandRequest.GET_FROM_USER:
            #primet u polzovatelai i vstavit v iacheyku
            if req.param1 < 10 and req.param2 < 10 and req.param3 < 10 and req.param4 < 10:
                cmmd = 'prm_getfu'
                params = {}
                params = {'x': req.param1, 'y': req.param2, 'z': req.param3}
                #def arduino_send_command(command, params={}):
                #arduino_port.write(json.dumps(
                #  {"command": command, "params": params}) + '\n')
                #{"command": "prm_getfu", "params": {"x": 2, "y": 2, "z": 1}}
                arduino_send_command(cmmd, params)
                rospy.sleep(2)
                set_and_pub_dronepoint_state(DriverState.GETTING_FROM_USER)
                return current_class.run_scenario(dronepoint_scenarios.GetFromUserScenario)
            else:
                return DriverCommandResponse(
                    DriverCommandResponse.ERROR,
                    'Wrong payload cell number'
                )
        elif req.command == DriverCommandRequest.OPEN: 
             #vozmet u polsovatelia, snimet kryshku zagruzit v dron
             #ArduinoCommand.CMO_OPEN
             if req.param1 < 10 and req.param2 < 10 and req.param3 < 10 and req.param4 < 10:
                cmmd = 'prm_ld'
                params = {}
                params = {'x': req.param1, 'y': req.param2, 'z': req.param3, 'nkr': req.param4}
                arduino_send_command(cmmd, params)
                rospy.sleep(2)
                set_and_pub_dronepoint_state(DriverState.OPENING)
                return current_class.run_scenario(dronepoint_scenarios.cmOopn)
             else:
                return DriverCommandResponse(
                    DriverCommandResponse.ERROR,
                    'Wrong payload cell number'
                )
        elif req.command == DriverCommandRequest.CLOSE: 
             #snimet s drona vstavit krishku, vydast polsovatelu
             #ArduinoCommand.CMO_CLOSE
             if req.param1 < 10 and req.param2 < 10 and req.param3 < 10 and req.param4 < 10:
                cmmd = 'prm_ud'
                params = {}
                params = {'x': req.param1, 'y': req.param2, 'z': req.param3, 'nkr': req.param4}
                arduino_send_command(cmmd, params)
                rospy.sleep(2)
                set_and_pub_dronepoint_state(DriverState.CLOSING)
                return current_class.run_scenario(dronepoint_scenarios.cmOcls)
             else:
                return DriverCommandResponse(
                    DriverCommandResponse.ERROR,
                    'Wrong payload cell number'
                )
        elif req.command == 9: 
             #scenariy zamena AKB CHANGING_BATTERY
             #ArduinoCommand.CMO_CHAN
             if req.param1 < 10 and req.param2 < 10 and req.param3 < 10 and req.param4 < 10:
                cmmd = 'prm_ld'
                params = {}
                params = {'x': req.param1, 'y': req.param2, 'z': req.param3, 'nkr': req.param4}
                arduino_send_command(cmmd, params)
                rospy.sleep(2)
                set_and_pub_dronepoint_state(9)
                return current_class.run_scenario(dronepoint_scenarios.cmOchan)
             else:
                return DriverCommandResponse(
                    DriverCommandResponse.ERROR,
                    'Wrong payload cell number'
                )
        elif req.command <= 12:
            rospy.logwarn('Requested scenario is not implemented yet')
            return DriverCommandResponse(
                DriverCommandResponse.ERROR, 'Requested scenario is not implemented yet'
            )
        else:
            rospy.logerr('Unknown scenario: %s', req.command)
            return DriverCommandResponse(
                DriverCommandResponse.ERROR,
                'Unknown scenario'
            )

    @classmethod
    def run_scenario(current_class, scenario, params = None):
        """
        Starts separate thread with scenario
        """
        current_class.set_scenario_executing(True)
        # Starting thread which will handle incoming "metacommands" aka "scenarios"
        scenario_executor_thread = threading.Thread(target=ScenariosHandler.execute_scenario, kwargs={'scenario': scenario})
        scenario_executor_thread.start()
        return DriverCommandResponse(DriverCommandResponse.SUCCESS, 'Scenario started')

    @classmethod
    def execute_scenario(current_class, scenario, params = None):
        """
        Executes scenario sequence in a separate thread. Does prechecks and than runs commands one by one.
        """
        global current_hardware_state

        rospy.logdebug('Starting scenario executor thread...')
        rospy.loginfo('Running %s scenario', scenario['name'])

        # Scenario execution starts here
        # Lets's do prechecks
        prechecks_passed = True
        for key, value in scenario['prechecks'][dronepoint_scenarios.PrecheckTypes.UART_STATE].items():
            with hardware_state_lock:
                rospy.logdebug('Precheck "%s": needed - "%s", current - "%s"',
                                key, value, current_hardware_state[key])
                if current_hardware_state[key] != value:
                    rospy.logerr("Precheck for scenario %s failed! '%s' shuld be '%s' but it is '%s'!",
                                    scenario['name'], key, value, current_hardware_state[key])
                    prechecks_passed = False
                    current_class.set_scenario_executing(False)
                    # Updating metastate
                    set_and_pub_dronepoint_state(DriverState.ERROR, "Precheck failed! '{}' is '{}'".format(key, current_hardware_state[key]))
                    break

        if prechecks_passed:
            # Start looping over commands
            for command in scenario['commands']:
                command_result = None
                if "type" in command:
                    if command["type"] == dronepoint_scenarios.ScenarioCommandTypes.SEND_UART:
                        command_result = current_class.run_uart_command(command)
                    elif command["type"] == dronepoint_scenarios.ScenarioCommandTypes.SLEEP:
                        command_result = current_class.run_sleep_command(command)
                    elif command["type"] == dronepoint_scenarios.ScenarioCommandTypes.GET_INFO:
                        command_result = current_class.run_get_info_command(command)
                    else:
                        rospy.logerr("Unknown command type!")   
                        set_and_pub_dronepoint_state(DriverState.ERROR, "Unknown command type! '{}'".format(command["command"]))
                        break
                else:
                    rospy.logerr("Unknown command type!")
                    set_and_pub_dronepoint_state(DriverState.ERROR, "Unknown command type! '{}'".format(command["command"]))
                    break
                
                if command_result == dronepoint_scenarios.CommandExitCodes.SUCCESS:
                    # If command executed successfully
                    rospy.loginfo("Command '%s' successfully executed.", command['command'])
                elif command_result == dronepoint_scenarios.CommandExitCodes.TIMEOUT:
                    rospy.logerr('Command "%s" timed out!',
                                    command['command'])
                    current_class.set_scenario_executing(False)
                    set_and_pub_dronepoint_state(DriverState.ERROR, "Command '{}' timed out!".format(command["command"]))
                    # Exitting command loop
                    break
                elif command_result == dronepoint_scenarios.CommandExitCodes.SHUTDOWN:
                    break
                elif command_result == dronepoint_scenarios.CommandExitCodes.ERROR:
                    rospy.logerr('Command "%s" returned error!',
                                    command['command'])
                    current_class.set_scenario_executing(False)
                    set_and_pub_dronepoint_state(DriverState.ERROR, "Command '{}' returned error!".format(command["command"]))
                    # Exitting command loop
                    break
                else:
                    set_and_pub_dronepoint_state(DriverState.ERROR, "Unknown error from command '{}'!".format(command["command"]))
                    # Exitting command loop
                    break

        if get_dronepoint_publishing_state() != DriverState.ERROR:
            set_and_pub_dronepoint_state(DriverState.STANDBY, msg="Scenario '{}' finished".format(scenario['name']))
            rospy.loginfo('Scenario %s finished', scenario['name'])
        else:
            rospy.logerr('Scenario %s interrupted with error!', scenario['name'])

        current_class.set_scenario_executing(False)

        rospy.logdebug('Stopping scenario executor thread...')


# A lock for thread-safe command sending
command_sending_lock = threading.Lock()


def arduino_send_command(command, params={}):
    """
    Send a command to Arduino using serial port.
    :param command: A command to send (str).
    :param params: Optional params for command
    :return: None
    """
    with command_sending_lock:
        if not isinstance(command, str):
            raise TypeError('command has to be a string')

        # Send a JSON-serialised command to Arduino, add \n to the end
        arduino_port.write(json.dumps(
            {"command": command, "params": params}) + '\n')
        rospy.logdebug(
            'Arduino command sent: "%s", params: "%s"', command, params)

        set_and_pub_dronepoint_state(get_dronepoint_publishing_state(), 
            msg="Running command '{}'".format(command), update_status_text=True)
        


# A command sequence to stop charging station mechanics (tuple(str))
# WARNING: It's not an action sequence!
ARDUINO_STOP_SEQUENCE = ArduinoCommand.LID_STOP, ArduinoCommand.LOCK_STOP, \
    ArduinoCommand.STOP


def arduino_stop():
    """
    Stop Arduino mechanics using serial port.
    WARNING: Asynchronous call (CsActionExecutor)!
    :return: None
    """
    for command in ARDUINO_STOP_SEQUENCE:  # For every command from the command sequence
        arduino_send_command(command)  # Send a command using serial port


def cs_state_message_callback(msg):
    """
    Status message callback from Arduino.
    WARNING: Synchronous callback (main)!
    :param msg: A message from the charging station hardware (dict(str: str))
    :return: None
    """
    global current_hardware_state

    rospy.logdebug("Incoming state message from Arduino: '%s'", msg)

    with hardware_state_lock:
        # If the hardware state was changed from the last time
        if isinstance(msg, dict):
            # HINT: There is no write operation to the current hardware state except the one below

            # current_hardware_state = msg  # Update state value

            # If we have several different states with different keys
            for key, value in msg.items():
                current_hardware_state.update({key: value})

            # print(current_hardware_state)         


def dronepoint_command_proxy_callback(req):
    """
    Direct proxy any command to Arduino. "Service mode" for Tronev-like staff.
    :param req: A request from the user
    :return: command status
    """
    rospy.logdebug("Proxying command: %s with params: %s",
                   req.command, req.params)

    if req.params == '':
        req.params = '{}'

    arduino_send_command(req.command, json.loads(req.params))

    return DriverCommandProxyResponse(DriverCommandProxyResponse.SUCCESS, 'Approved by drone-o-mother')


# Main function
if __name__ == '__main__':
    # Initialize the node, name it, set log verbosity level
    rospy.init_node('dronepoint_driver', log_level=rospy.DEBUG)
    rospy.logdebug('Node initialized')

    node_prefix = rospy.get_param(
        "~prefix", default='')  # Extracting topic prefix

    if not isinstance(node_prefix, str):
        raise EnvironmentError(
            'Node topic prefix parameter value has to be a string!')

    rospy.logdebug('Node prefix: %s', node_prefix)

    # Creating a topic publisher for commands addressed to actuation driver with latching
    state_publisher = rospy.Publisher('{0}/state'.format(node_prefix),
                                      DriverState, queue_size=10, latch=True)
    rospy.logdebug('Publisher for a state topic ready: %s', state_publisher)

    # Create a command service server
    rospy.Service('{0}/command'.format(node_prefix),
                  DriverCommand, ScenariosHandler.dronepoint_command_callback)
    rospy.logdebug('Created a driver command service server')

    # Direct proxy any command to Arduino
    rospy.Service('{0}/command_proxy'.format(node_prefix),
                  DriverCommandProxy, dronepoint_command_proxy_callback)
    rospy.logdebug('Created a driver command proxy service server')

    lock_always_released = rospy.get_param(
        "~lock_always_released", default=False)
    rospy.logdebug('Lock always released: %s', lock_always_released)

    try:
        # Retrieving serial port path
        serial_port = rospy.get_param('~serial_port')
    except KeyError:
        raise EnvironmentError('No serial port presented!')

    if not isinstance(serial_port, str):
        raise EnvironmentError(
            'Serial port parameter value has to be a string!')

    rospy.logdebug('Serial port: "%s"', serial_port)

    serial_baudrate = rospy.get_param('~serial_baudrate', 115200)

    if not isinstance(serial_baudrate, int):
        raise EnvironmentError(
            'Serial port baudrate value has to be an integer!')

    rospy.logdebug('Serial port baudrate: %s', serial_baudrate)

    serial_bytesize = rospy.get_param('~serial_bytesize', 8)

    if not isinstance(serial_bytesize, int):
        raise EnvironmentError(
            'Serial port bytesize value has to be an integer!')

    rospy.logdebug('Serial port bytesize: %s', serial_bytesize)

    serial_parity = rospy.get_param('~serial_parity', 'N')

    if not isinstance(serial_parity, str):
        raise EnvironmentError('Serial port parity value has to be a string!')

    rospy.logdebug('Serial port parity: "%s"', serial_parity)

    serial_stopbits = rospy.get_param('~serial_stopbits', 1)

    if not isinstance(serial_stopbits, (int, float)):
        raise EnvironmentError(
            'Serial port stop bits number has to be an integer or a float!')

    rospy.logdebug('Serial port stop bits: "%s"', serial_stopbits)

    serial_xonxoff = rospy.get_param('~serial_xonxoff', False)

    if not isinstance(serial_xonxoff, bool):
        raise EnvironmentError('Serial port XON/XOFF value has to be a bool!')

    rospy.logdebug('Serial port XON/XOFF: %s', serial_xonxoff)

    serial_rtscts = rospy.get_param('~serial_rtscts', False)

    if not isinstance(serial_rtscts, bool):
        raise EnvironmentError('Serial port RTS/CTS value has to be a bool!')

    rospy.logdebug('Serial port RTS/CTS: %s', serial_rtscts)

    read_timeout = rospy.get_param('~read_timeout', 1.0)

    if not isinstance(read_timeout, float):
        raise EnvironmentError('Read timeout value has to be a bool!')

    rospy.logdebug('Read timeout: "%s"', read_timeout)

    arduino_port = serial.Serial(
        port=serial_port,
        baudrate=serial_baudrate,
        bytesize=serial_bytesize,
        parity=serial_parity,
        stopbits=serial_stopbits,
        xonxoff=serial_xonxoff,
        rtscts=serial_rtscts,
        # Blocking mode
        # WARNING: We have to keep a timeout for the correct node shutdown!
        timeout=read_timeout
    )
    rospy.logdebug('Arduino serial port ready: %s', arduino_port)

    # No need in these variables from this moment
    del serial_port
    del serial_baudrate
    del serial_bytesize
    del serial_parity
    del serial_stopbits
    del serial_xonxoff
    del serial_rtscts
    del read_timeout

    #state_buffer = ''  # Buffer for the incoming JSON state message

    serial_byte = None  # Storage for a serial port byte

    # Loading cells state from file
    try:
        rospack = rospkg.RosPack()

        with open(os.path.join(rospack.get_path('dronepoint_driver'), 'scripts/dronepoint_driver/cells_states.json')) as cells_states_file:
            cells_state = json.load(cells_states_file)
    except ValueError as e:  # Incorrect JSON file
        rospy.logerr('Failed to parse cells state JSON file "%s": %s',
                    cells_state, e)

    # Lets start with "STANDBY" state
    # TODO: startup checks
    set_and_pub_dronepoint_state(DriverState.STANDBY)

    try:
        while not rospy.is_shutdown():  # While the ROS is up
            try:
                # Get a byte from the serial port
                # HINT: It's a blocking routine
                serial_byte = arduino_port.read()

                # rospy.logdebug('Got byte from Arduino: "%s"',
                #         serial_byte)

                # No data in the serial port buffer (read timeout)
                if not serial_byte:
                    pass  # Just ignore
                elif serial_byte == '\n':  # End of the JSON state message
                    if len(state_buffer):  # Not an empty JSON state message
                        # rospy.logdebug('Got JSON state message from Arduino: "%s"',
                        # state_buffer)

                        try:
                            cs_state = json.loads(state_buffer)  # Decode JSON
                            # rospy.logdebug("Decoded state message from Arduino: '%s'",
                            #    cs_state)
                            #передача сообщения JSON по UDP
                            #nc -u 127.0.0.1 7111
                            sendUDP()
                            # JSON loads method correctly parses any JSON value without
                            # a parameter ('123', '0.2', '"ABC"'). We need an additional type
                            # check.
                            if not isinstance(cs_state, dict):  # We expect to get dict
                                rospy.logerr(
                                    'Invalid state message: %s', cs_state)
                            else:
                                # Passing to the callback
                                cs_state_message_callback(cs_state)
                        except ValueError as e:  # Incorrect JSON message
                            # rospy.logerr('Failed to parse JSON message "%s": %s',
                            #              state_buffer, e)
                            pass
                    else:
                        rospy.logerr('Empty JSON state message!')

                    state_buffer = ''  # Clean the JSON state message buffer
                else:  # Not an end of the JSON state message
                    # JSON status massage is too big (abnormal Arduino firmware?)
                    if len(state_buffer) > STATE_BUFFER_LIMIT:
                        rospy.logerr(
                            'Too big JSON state message: "%s"', state_buffer)

                        state_buffer = ''  # Clean the JSON state message buffer

                        # TODO: It's better to set a flag after buffer overflow and ignore
                        # TODO:     new data until the message end ('\n'). But in fact it
                        # TODO:     doesn't matter what we do with the rest of this message.

                    # Collect a character inside the state message buffer
                    state_buffer += serial_byte
            except serial.serialutil.SerialException as e:
                # TODO: When the ROS is down by Ctrl-C we will get an exception
                # TODO:     caused by EINTR (signal interrupt). The better way to avoid this is
                # TODO:     to place read operation in the separate thread.

                # Check if it is an interrupted system call
                # TODO: That's an ugly solution... Do we have other options with PySerial?
                if str(e).find("Interrupted system call") == -1:
                    rospy.logerr(
                        'Failed to read from the serial device: %s', e)
                else:
                    rospy.logdebug('Interrupted read')

                break  # Stop the ROS node
    finally:  # Cleaning up
        node_shutdown_event.set()  # All new command callbacks will be cancel after this point

        with action_sequence_executor_lock:  # Synchronise with the subscriber's thread
            # Action sequence executor exists and it's running
            if (action_sequence_executor is not None) \
                    and (action_sequence_executor.is_alive()):
                rospy.logdebug('The sequence execution is in progress!')

                # Requesting sequence execution stop
                action_sequence_executor.stop_executor()
                rospy.logdebug('Stopping the sequence execution!')

                # Wait until the executor stops
                action_sequence_executor.join()
                rospy.logdebug('The sequence execution has stopped!')

        arduino_port.close()  # Closing serial port
        rospy.logdebug('Arduino serial port closed')