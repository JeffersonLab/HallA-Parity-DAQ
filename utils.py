'''
Green Monster GUI Revamp
Utilities for the whole program
Code Commissioned 2019-01-16
Code by A.J. Zec
'''
import tkinter as tk
from ctypes import cdll
import subprocess, os

green_color = '#3C8373'

SOCK_OK = 0; SOCK_ERROR = -1

COMMAND_HAPTB = 1000; COMMAND_BMW = 2000
COMMAND_FDBK = 3000; COMMAND_HAPADC = 4000
COMMAND_SCAN = 5000; COMMAND_ADC18 = 6000
COMMAND_VQWK = 7000

#Crate_CH   = 0; Crate_INJ  = 1
#Crate_LHRS = 2; Crate_RHRS = 3
#Crate_Test = 4

Crate_CH   = 0; Crate_LHRS = 1
Crate_RHRS = 2; Crate_INJ  = 3
Crate_Test = 4

def set_text(entry, text):
  entry.delete(0, tk.END)
  entry.insert(0, str(text))
  return entry

def pass_params_to_c():
  f = open('transfer.txt', 'w+')
  f.write('This is the message\n')
  f.write('And the reply')
  f.close()

  lib = cdll.LoadLibrary('./libs/libTestClass.so')
  obj = lib.init(2, 4000, 4006, 423, 5493, 585)
  lib.print_params(obj)
  
def send_command(crate_num, packet):
  lib = cdll.LoadLibrary('cfSock/libcfSockCli.so')

  f = open('transfer.txt', 'w+')
  for i in  range(5, 7):
    f.write(str(packet[i]) + '\n')
  f.close()

  err_flag = lib.GMSockCommand(crate_num, packet[0], packet[1], packet[2], packet[3], packet[4])

  replyBool = "Y"
  ind = 0; reply = []
  fin = open('reply.txt')
  if (len(packet)>6):
    replyBool = packet[6]
    #print("Reply status declared = {}".format(packet[6]))
  if replyBool == "Y" or replyBool == 'Y':
    #print("Reply requested")
    for line in fin.readlines():
      if ind < 5: 
        reply += [int(line)]
      else: 
        reply += [str(line)]
      ind += 1
  else:
    #print("Reply not requested")
    reply = "No Reply"
  fin.close()

  os.remove('reply.txt')

  return err_flag, reply 
  
