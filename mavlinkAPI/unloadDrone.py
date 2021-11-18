#!/usr/bin/env python
# -*- coding: utf-8 -*-
import time, os, sys

from pymavlink import mavutil
import threading
import json


#CUSTOM_MODE
CUSTOM_MODE_UNKNOWN=0
CUSTOM_MODE_COVER_INSTALLATION=2
CUSTOM_MODE_COVER_REMOVAL=4
CUSTOM_MODE_LOADING_DRONE = 5
CUSTOM_MODE_UNLOADING_DRONE = 6
CUSTOM_MODE_GETTING_FROM_USER = 7
CUSTOM_MODE_UNLOADING_TO_USER = 8
CUSTOM_MODE_CONTAINER_UNLOADING = 9
SERVICE = 10
RESET = 11
STANDBY = 12
ERROR = 13

#CUSTOM_SUBMODE
LOCK_RELEASE = 0
LOCK_LOCK = 1
LOCK_STOP = 2
OPEN_TOP_HATCH = 3
CLOSE_TOP_HATCH = 4
GOTO_CELL = 5
LOAD_CHARGING_CELL = 6
UNLOAD_CHARGING_CELL = 7
LOAD_PAYLOAD_CELL = 8
UNLOAD_PAYLOAD_CELL = 9
GET_FROM_USER = 10
UNLOAD_TO_USER = 11
STOP = 12
GET_PAYLOAD_FROM_DRONE = 13
INSERT_PAYLOAD_INTO_DRONE = 14
LOCK_PAYLOAD = 15
RELEASE_PAYLOAD = 16
LOCK_CHARGING_CELL_LOCK = 17
RELEASE_CHARGING_CELL_LOCK = 18
LOCK_TOP_HATCH_LOCK = 19
RELEASE_TOP_HATCH_LOCK = 20
OPEN_BOTTOM_HATCH = 21
CLOSE_BOTTOM_HATCH = 22
LOCK_USER_CELL_LOCK = 23
RELEASE_USER_CELL_LOCK = 24
GOTO_CHARGING_CELL = 25
CCSM=12
NUMO=0


master = mavutil.mavlink_connection('udpout:127.0.0.1:14590')

def telemet():
    #show incoming mavlink messages
    global CCSM
    while True:
        msg =  master.recv_match(type = 'HEARTBEAT', blocking = False)
        if not msg:
            continue
        else:
            #print(msg)
            try:
              state = msg.to_dict()
              #print(state)
              if state.get("type")==31:
                #print(state)
                CCSM = state.get("custom_mode")
                print("CUSTOM_MODE =", CCSM)
            except ValueError as e:  # Incorrect message
              print(e)

def sendheard():
      '''heartbeat bistro'''
      while True:
            try:
              master.mav.heartbeat_send(0,0,0,0,0)
              time.sleep(1)
            except ValueError as e:  # nemogu message send
              print(e)


task1 = threading.Thread(target=telemet)
task2 = threading.Thread(target=sendheard)
task1.start()
task2.start()


while True:
   try:  #LOAD_DRONE CUSTOM_MODE операция меняет стэйт
         #vynet iz iacheyki snimet kryshku, zagruzit v dron
         print("LOAD_DRONE X0Y3Z2Kr0")
         master.mav.command_long_send(master.target_system, master.target_component,
                                  mavutil.mavlink.MAV_CMD_DO_SET_MODE, 1,
                                  mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                                  CUSTOM_MODE_LOADING_DRONE, #5 стэйт
                                  0, 3, 2, 0, 0) 
                                 #X, Y, Z, Kr0
         time.sleep(5)
         while CCSM!=12:
           pass
         time.sleep(7)
         #UNLOAD_DRONE CUSTOM_MODE операция меняет стэйт
         #snimet s drona vstavit krishku, zagruzut v iacheyku
         print("UNLOAD_DRONE X0Y3Z2Kr0")
         master.mav.command_long_send(master.target_system, master.target_component,
                                  mavutil.mavlink.MAV_CMD_DO_SET_MODE, 1,
                                  mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                                  CUSTOM_MODE_UNLOADING_DRONE, #6 стэйт
                                  0, 3, 2, 0, 0) 
         time.sleep(5)
         while CCSM!=12:
           pass
         time.sleep(7)

   except BaseException as e:
         print(e)

time.sleep(3)
print('OK')

