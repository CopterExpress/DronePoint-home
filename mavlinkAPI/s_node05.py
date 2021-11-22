#!/usr/bin/env python
# -*- coding: utf-8 -*-
from socket import *
import socket


udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
udp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
udp_socket.settimeout(0.150)
host = '0.0.0.0'
port = 9567
addr = (host,port)
state_buffer = "FF"
COMMCON = ""
#bind - связывает адрес и порт с сокетом
udp_socket.bind(addr)
#udp_socket.listen(5)
print('wait data...')


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
        params = {}
        SCOM = []
        X=0
        Y=0
        Z=0
        nkr=0
        global COMMCON
        SCOM = COMMCON.split(':')
        if   COMMCON == 'HDO'+ '\n':
             cmmd='hatch_delivery_open'
             #grocery = 'Milk:Chicken:Bread'
             #sgrocery = []
             #sgrocery = grocery.split(':')
             #print(sgrocery[0])

             params = {'x': X, 'y': Y, 'z': Z, 'nkr': nkr}
             print(params)
             print(cmmd)

             #arduino_send_command(cmmd, params)
             COMMCON = ''
             #return DriverCommandResponse(
             #       DriverCommandResponse.SUCCESS, 'Command was proxied to Dronepoint'
             #   )

        elif SCOM[0] == 'KM': #KM:0:3:2:0
             cmmd='hatch_delivery_close'
            
             try:
                X=int(SCOM[1])
                Y=int(SCOM[2])
                Z=int(SCOM[3])
                nkr=int(SCOM[4])
                params = {'x': X, 'y': Y, 'z': Z, 'nkr': nkr}
                print(params)
                print(cmmd)
                #arduino_send_command(cmmd, params)
                #return DriverCommandResponse(
                #       DriverCommandResponse.SUCCESS, 'Command was proxied to Dronepoint'
                #   )

             except:
                X=0
                Y=0
                Z=0
                nkr=0
            
             COMMCON = ''


        elif COMMCON == 'HDC'+ '\n':
             cmmd='hatch_delivery_close'

             params = {'x': X, 'y': Y, 'z': Z, 'nkr': nkr}
             print(params)
             print(cmmd)

             #arduino_send_command(cmmd, params)
             COMMCON = ''
             #return DriverCommandResponse(
             #       DriverCommandResponse.SUCCESS, 'Command was proxied to Dronepoint'
             #   )



# Main function
if __name__ == '__main__':
 #Бесконечный цикл работы программы
 while True:
    sendUDP()
    #print("Pass")


#udp_socket.close()
#sudo netstat -tulpn | grep 796
#nc -u 127.0.0.1 796
