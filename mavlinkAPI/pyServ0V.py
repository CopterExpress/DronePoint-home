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
port = 756
addr = (host,port)
DT = "OTVETDATA:)"
#bind - связывает адрес и порт с сокетом
udp_socket.bind(addr)
#udp_socket.listen(5)
print('wait data...')


def sendUDP():
    while True:
      try:
          #recvfrom - получает UDP сообщения
          conn, addr = udp_socket.recvfrom(1024)
          print('client addr: ', addr)
          print('content: ', conn)
          #conn = "OTVET:"+conn

          #sendto - передача сообщения UDP
          udp_socket.sendto((DT.encode()), addr)
          print(DT)
          break
      except socket.timeout:
          print("Didn't receive data! [Timeout 0.150s]")
          break



# Main function
if __name__ == '__main__':
 #Бесконечный цикл работы программы
 while True:
    sendUDP()
    print("Pass")


#udp_socket.close()
#sudo netstat -tulpn | grep 796
#nc -u 127.0.0.1 796
