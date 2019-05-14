'''
Green Monster GUI Revamp
Containing Timeboard Tab
Code Commissioned 2019-01-16
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import utils as u
from ctypes import cdll

class Timeboard(tk.Frame):
  def __init__(self, tab):
    self.lib = cdll.LoadLibrary('./libs/libGTimeboard.so')
    self.obj = self.lib.init(40, 13200, 0)

    self.ch_frame = tk.LabelFrame(tab, text='CH', background=u.green_color, width=500)
    self.ramp_delay_l = tk.Label(self.ch_frame, text='Ramp Delay', background=u.green_color)
    self.int_time_l = tk.Label(self.ch_frame, text='Integrate Time', background=u.green_color)
    self.oversamp_l = tk.Label(self.ch_frame, text='Oversampling', background=u.green_color)
    self.ramp_delay_e = tk.Entry(self.ch_frame)
    self.int_time_e = tk.Entry(self.ch_frame)
    self.oversamp_e = tk.Entry(self.ch_frame)

    self.ramp_delay_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.int_time_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.oversamp_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.ramp_delay_e, '40').grid(row=0, column=1)
    u.set_text(self.int_time_e, '13200').grid(row=1, column=1)
    u.set_text(self.oversamp_e, '0').grid(row=2, column=1)
    tk.Button(self.ch_frame, text='Get Settings', background=u.green_color, command=self.get_all_values).grid(
        row=3, column=0, pady=10)
    tk.Button(self.ch_frame, text='Apply Settings', background=u.green_color, command=self.set_all_values).grid(
        row=3, column=1, pady=10)
    self.ch_frame.pack(padx=20, pady=10, anchor='w')
  
  def get_RD_value(self):
    new_rd = self.lib.getRDvalue(self.obj)
    self.ramp_delay_e = u.set_text(self.ramp_delay_e, str(new_rd))

  def set_RD_value(self):
    self.lib.setRDvalue(self.obj, int(self.ramp_delay_e.get()))

  def get_IS_value(self):
    new_is = self.lib.getISvalue(self.obj)
    self.int_time_e = u.set_text(self.int_time_e, str(new_is))

  def set_IS_value(self):
    self.lib.setISvalue(self.obj, int(self.int_time_e.get()))

  def get_OS_value(self):
    new_os = self.lib.getOSvalue(self.obj)
    self.oversamp_e = u.set_text(self.oversamp_e, str(new_os))

  def set_OS_value(self):
    self.lib.setOSvalue(self.obj, int(self.oversamp_e.get()))

  def pull_from_board(self):
    self.lib.getValsTB(self.obj)

  def push_to_board(self):
    self.lib.setValsTB(self.obj)

  def get_all_values(self):
    self.pull_from_board()
    self.get_RD_value()
    self.get_IS_value()
    self.get_OS_value()

  def set_all_values(self):
    self.set_RD_value()
    self.set_IS_value()
    self.set_OS_value()
    self.push_to_board()
