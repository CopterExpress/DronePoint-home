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
   try:  #CONTAINER_UNLOADING CUSTOM_MODE операция меняет стэйт
         #контролируемое перемещение каретки для операций замены АКБ
         #выполняется перед операциями COVER_REMOVAL или COVER_INSTALLATION
         #с указанием координат для выгрузки контейнера
         #значение параметров X, Y, Z    X в этом дронпоинте всегда 0, если Y=3 перед, Y=1 зад
         print("CONTAINER_UNLOADING X0Y3Z2")
         master.mav.command_long_send(master.target_system, master.target_component,
                                  mavutil.mavlink.MAV_CMD_DO_SET_MODE, 1,
                                  mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                                  CUSTOM_MODE_CONTAINER_UNLOADING, #9 стэйт
                                  0, 3, 2, 0, 0) #выгружает отсюда
                                 #X, Y, Z
         time.sleep(5)
         while CCSM!=12:
           pass
         time.sleep(7)
         
         #COVER REMOVAL CUSTOM_MODE операция меняет стэйт
         #перед этой командой должна выполняться команда CONTAINER_UNLOADING
         #после ее выполнения, если контейнер без крышки стоит на каретки
         #производит СНЯТИЕ крышки с зарядки всегда Kr0
         #возвращает контейнер С КРЫШКОЙ в заданную ячейку
         #т.е. по внешним параметрам команды
         print("COVER REMOVAL X0Y3Z2Kr0")
         master.mav.command_long_send(master.target_system, master.target_component,
                                  mavutil.mavlink.MAV_CMD_DO_SET_MODE, 1,
                                  mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                                  CUSTOM_MODE_COVER_REMOVAL, #4 стэйт
                                  0, 3, 2, 0, 0) #возвращает контейнер сюда
                                 #X, Y, Z, Kr0
         time.sleep(5)
         while CCSM!=12:
           pass
         time.sleep(7)


         print("CONTAINER_UNLOADING X0Y3Z0") #установка координат выгрузки контейнера
         master.mav.command_long_send(master.target_system, master.target_component,
                                  mavutil.mavlink.MAV_CMD_DO_SET_MODE, 1,
                                  mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                                  CUSTOM_MODE_CONTAINER_UNLOADING, #9 стэйт
                                  0, 3, 0, 0, 0) #выгружает отсюда
         time.sleep(5)
         while CCSM!=12:
           pass
         time.sleep(7)


         print("CONTAINER_UNLOADING X0Y3Z2") #установка координат выгрузки контейнера
         master.mav.command_long_send(master.target_system, master.target_component,
                                  mavutil.mavlink.MAV_CMD_DO_SET_MODE, 1,
                                  mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                                  CUSTOM_MODE_CONTAINER_UNLOADING, #9 стэйт
                                  0, 3, 2, 0, 0) #выгружает отсюда
         time.sleep(5)
         while CCSM!=12:
           pass
         time.sleep(7)

         #COVER INSTALLATION CUSTOM_MODE операция меняет стэйт
         #перед этой командой должна выполняться команда CONTAINER_UNLOADING
         #после ее выполнения, если контейнер с крышкой стоит на каретки
         #производит УСТАНОВКУ крышки на зарядку всегда Kr0
         #возвращает контейнер БЕЗ КРЫШКИ в заданную ячейку
         #т.е. по внешним параметрам команды
         print("COVER INSTALLATION X0Y3Z2Kr0")
         master.mav.command_long_send(master.target_system, master.target_component,
                                  mavutil.mavlink.MAV_CMD_DO_SET_MODE, 1,
                                  mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                                  CUSTOM_MODE_COVER_INSTALLATION, #2 стэйт
                                  0, 3, 2, 0,  0) #возвращает контейнер сюда
                                 #X, Y, Z, Kr0
         time.sleep(5)
         while CCSM!=12:
           pass
         time.sleep(7)

   except BaseException as e:
         print(e)

time.sleep(3)
print('OK')

