'''
Green Monster GUI Revamp
Utilities for the whole program
Code Commissioned 2019-01-16
Code by A.J. Zec
'''
import tkinter as tk
from ctypes import cdll

green_color = '#3C8373'

COMMAND_HAPTB = 1000; COMMAND_BMW = 2000
COMMAND_FDBK = 3000; COMMAND_HAPADC = 4000
COMMAND_SCAN = 5000; COMMAND_ADC18 = 6000
COMMAND_VQWK = 7000

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
  
pass_params_to_c()
