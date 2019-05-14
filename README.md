# PyGreenMonster
Green Monster GUI created without ROOT

To compile, run the following commands:
gcc -c -fPIC -o cfSock/cfSockCli.o cfSock/cfSockCli.c
gcc -shared -o cfSock/libcfSockCli.so cfSock/cfSockCli.o

Main file is GreenMonster.py

Running:
python3 GreenMonster.py

It will require extensive use of python's cdll library.
The GUI is constructed with python GUI module TKinter and its sister module ttk.

2019-05-14: Only tab with correct functionality is gm_bmw.py.
