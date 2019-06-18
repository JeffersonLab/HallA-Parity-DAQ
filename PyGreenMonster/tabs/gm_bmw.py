'''
Green Monster GUI Revamp
Containing BMW Tab
Code Commissioned 2019-01-16
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import utils as u
from ctypes import cdll
import time

KILL          = 2001
GET_STATUS    = 2002
START         = 2003
CHECK_ALIVE   = 2004
TEST_START    = 2005
TEST_SET_DATA = 2006
UNKILL        = 2007

tab_title = '(BMW tab)'

class BMW(tk.Frame):
  def __init__(self, tab):
    self.bm_frame = tk.LabelFrame(tab, text='Beam Modulation', background=u.green_color)
    self.script_frame = tk.LabelFrame(self.bm_frame, text='Beam Modulation Script', background=u.green_color)
    self.script_frame.grid(row=0, column=0, pady=10, sticky='W')
    self.test_frame = tk.LabelFrame(self.bm_frame, text='Beam Modulation - TEST', background=u.green_color)
    self.test_frame.grid(row=1, column=0, pady=10, sticky='W')

    self.button_titles = ['MHF1C01H', 'MHF1C02H', 'MHF1C03V', 'MHF1C08H',
                     'MHF1C08V', 'MHF1C10H', 'MHF1C10V', 'SL Zone 20']

    self.ks_bm_l = tk.Label(self.script_frame, text='Kill Switch is OFF', background=u.green_color)
    self.status_bm_l = tk.Label(self.script_frame, text='********', background=u.green_color)
    self.script_bm_l = tk.Label(self.script_frame, text='Beam Modulation script is ****', background=u.green_color)
    self.test_e = tk.Entry(self.test_frame, width=12)
    self.bm_test_setting = tk.StringVar(); self.bm_test_index = tk.IntVar()
    self.script_frame_layout()
    self.test_frame_layout()
    self.bm_frame.pack(padx=20, pady=20, anchor='w')
    self.check_status_button()

  def script_frame_layout(self):
    self.ks_bm_l.grid(row=0, column=0, padx=10, pady=10, sticky='W')
    tk.Button(self.script_frame, text='Check Status', background=u.green_color, command=self.check_status_button).grid(
        row=0, column=1, padx=10, pady=10, sticky='W')
    self.status_bm_l.grid(row=0, column=2, padx=10, pady=10, sticky='W')
    self.bm_button = tk.Button(self.script_frame, text='Start Beam Modulation', background=u.green_color, command=self.change_status)
    self.bm_button.grid(row=1, column=0, padx=10, pady=10, sticky='W')
    self.script_bm_l.grid(row=1, column=1, padx=10, pady=10, sticky='W')

  def test_frame_layout(self):
    tk.Button(self.test_frame, text='Enable BMW Test', background=u.green_color, command=self.start_test).grid(
        row=0, column=0, columnspan=2, padx=10, pady=10)
    tk.Button(self.test_frame, text='Toggle Kill Switch', background=u.green_color, command=self.set_kill).grid(
        row=0, column=2, columnspan=2, padx=10, pady=10)
    
    self.bm_test_index.set(0)
    self.bm_test_setting.set(self.button_titles[self.bm_test_index.get()])
    for r in range(0, 2):
      for c in range(0, 4):
        tk.Radiobutton(self.test_frame, text=self.button_titles[r*4+c], variable=self.bm_test_index,
            value=r*4+c, background=u.green_color).grid( row=r+1, column=c, padx=2, pady=2, sticky='W')
        u.set_text(self.test_e, '0').grid(row=3, column=0, padx=3, pady=3, sticky='W')
        tk.Label(self.test_frame, text='Set Point\n(mA or keV)', bg=u.green_color).grid(
            row=3, column=1, padx=5, pady=5, sticky='W')

  def start_test(self):
    packet = [u.COMMAND_BMW, START, 0, 0, 0, "BMW Start Test", "Y"]
    err_flag, reply = u.send_command(u.Crate_CH, packet)

    if err_flag == u.SOCK_OK:
      packet[0] = reply[0]
      self.test_step()
    else:
      print(tab_title + " start_test: ERROR, Could not access socket.")
    
  def test_step(self):
    value = int(self.test_e.get())
    print("  " + tab_title + " Writing new set point: " + self.test_e.get() + " to " + self.button_titles[self.bm_test_index.get()])
    
    packet = [u.COMMAND_BMW, TEST_SET_DATA, self.bm_test_index.get(), value, 0, "BMW Start Test Data", "Y"]
    err_flag, reply = u.send_command(u.Crate_CH, packet)
    
    if err_flag == u.SOCK_OK: 
      kill_switch = reply[2]
      if not kill_switch:
        self.start_time = time.perf_counter()
    else: 
      print(tab_title + " test_step: ERROR, Could not access socket.")

  def check_status(self):
    packet = [u.COMMAND_BMW, GET_STATUS, 0, 0, 0, "BMW status check", "Y"]
    err_flag, reply = u.send_command(u.Crate_CH, packet)
    
    if err_flag == u.SOCK_OK:
      bmw_running = bool(reply[2])
      print(tab_title + " check_status: bmw_running = " + str(bmw_running))
      kill_switch = bool(reply[3])
      print(tab_title + " check_status: kill_switch = " + str(kill_switch))

      if kill_switch: self.ks_bm_l['text'] = 'Kill Switch is ON'
      else: self.ks_bm_l['text'] = 'Kill Switch is OFF'

      if bmw_running: 
        self.status_bm_l['text'] = 'Beam Modulation is ON'
        self.bm_button['text'] = 'Set Kill Switch'
      else: 
        self.status_bm_l['text'] = 'Beam Modulation is OFF'
        self.bm_button['text'] = 'Start Beam Modulation'
      return bmw_running, kill_switch
    else:
      print(tab_title + " check_status: ERROR, Could not access socket.")
      return False, False

  def change_status(self):
    bmw_running, kill_switch = self.check_status()
    packet = []
    if bmw_running: packet = [u.COMMAND_BMW, KILL, 0, 0, 0, "BMW Status Change", "Y"]
    else: packet = [u.COMMAND_BMW, START, 0, 0, 0, "BMW Status Change", "Y"]
    print(tab_title + " Changing status of BMW client...")
    err_flag, reply = u.send_command(u.Crate_CH, packet)

    if err_flag == u.SOCK_OK:
      print(tab_title + ' BMW status change call is complete')
      bmw_running, kill_switch = self.check_status()
      print(tab_title + ' change_status: command = ' + str(reply[1]))
      print(tab_title + ' change_status: bmw_running')
      print(tab_title + ' change_status: Exiting...')
    else:
      print(tab_title + " change_status: ERROR, Could not access socket.")

  def set_kill(self):
    bmw_running, kill_switch = self.check_status()
    
    if not kill_switch:
      packet = [u.COMMAND_BMW, KILL, 0, 0, 0, 'BMW set kill', 'Y']
      err_flag, reply = u.send_command(u.Crate_CH, packet)
      if err_flag == u.SOCK_OK: print(tab_title + ' set_kill: BMW kill switch call is complete.')
      else: print(tab_title + ' set_kill: ERROR, Could not access socket.')
      self.check_status()
    else:
      packet = [u.COMMAND_BMW, UNKILL, 0, 0, 0, 'BMW lift kill', 'Y']
      err_flag, reply = u.send_command(u.Crate_CH, packet)
      if err_flag == u.SOCK_OK: print(tab_title + ' set_kill: BMW unkill switch call is complete.')
      else: print(tab_title + ' set_kill: ERROR, Could not access socket.')
      self.check_status()
      
  def check_active_flag(self):
    active = False
    packet = [u.COMMAND_BMW, CHECK_ALIVE, 0, 0, 0, 'BMW status check', 'Y']
    err_flag, reply = u.send_command(u.Crate_CH, packet)

    if err_flag == u.SOCK_OK:
      active = bool(reply[3])
      if active: self.script_bm_l['text'] = 'Beam Modulation script is ON'
      else: self.script_bm_l['text'] = 'Beam Modulation script is OFF'
    else: print(tab_title + ' check_active_flag: ERROR, Could not access socket.')

  def check_status_button(self):
    self.check_status()
    self.check_active_flag()
