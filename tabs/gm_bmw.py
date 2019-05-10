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

class BMW(tk.Frame):
  def __init__(self, tab):
    self.lib = cdll.LoadLibrary('./libGBMW.so')
    self.obj = self.lib.init()

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

  def script_frame_layout(self):
    self.ks_bm_l.grid(row=0, column=0, padx=10, pady=10, sticky='W')
    tk.Button(self.script_frame, text='Check Status', background=u.green_color, command=self.check_status).grid(
        row=0, column=1, padx=10, pady=10, sticky='W')
    self.status_bm_l.grid(row=0, column=2, padx=10, pady=10, sticky='W')
    self.bm_button = tk.Button(self.script_frame, text='Start Beam Modulation', background=u.green_color, command=self.change_status)
    self.bm_button.grid(row=1, column=0, padx=10, pady=10, sticky='W')
    self.script_bm_l.grid(row=1, column=1, padx=10, pady=10, sticky='W')

  def test_frame_layout(self):
    tk.Button(self.test_frame, text='Enable BMW Test', background=u.green_color, command=self.start_test).grid(
        row=0, column=0, columnspan=2, padx=10, pady=10)
    tk.Button(self.test_frame, text='Toggle Kill Switch', background=u.green_color, command=self.kill_switch).grid(
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

  def get_KS_text(self):
    new_ks = self.lib.getKSText(self.obj)
    self.ks_bm_l['text'] = str(new_ks)

  def get_ST_vtext(self):
    new_st = self.lib.getStatText(self.obj)
    self.status_bm_l['text'] = str(new_st)

  def get_ACT_text(self):
    new_act = self.lib.getActText(self.obj)
    self.script_bm_l['text'] = str(new_act)

  def get_button_text(self):
    new_button_text = self.lib.getButtonText(self.obj)
    self.bm_button['text'] = str(new_button_text)

  def get_test_type(self):
    new_test_type = self.lib.getTestType(self.obj)
    self.bm_test_index.set(int(new_test_type))

  def set_test_type(self):
    self.lib.setTestType(self.obj, self.bm_test_index.get())

  def get_set_point(self):
    new_set_point = self.lib.getSetPoint(self.obj)
    self.test_e = u.set_text(self.test_e, str(new_set_point))

  def set_set_point(self):
    self.lib.setSetPoint(self.obj, int(self.test_e.get()))

  def get_all_values(self):
    print("Kill switch?")
    self.get_KS_text()
    print("Yes. Status?")
    self.get_ST_text()
    print("Yes. Active?")
    self.get_ACT_text()
    print("Yes. Button text?")
    self.get_button_text()
    print("Yes. Test type?")
    self.get_test_type()
    print("Yes. Set point?")
    self.get_set_point()
    print("Yes. No more problems, chief.")

  def check_status(self):
    self.lib.BMWCheckStatus(self.obj)
    self.lib.BMWActiveProbe(self.obj)
    print("Starting get values...")
    self.get_all_values()

  def change_status(self):
    self.lib.BMWChangeStatus(self.obj)
    self.get_all_values()

  def start_test(self):
    self.set_test_type()
    self.set_set_point()
    self.lib.BMWStartTest(self.obj)
    self.get_all_values()

  def kill_switch(self):
    self.lib.BMWSetKill(self.obj)
    self.get_all_values()
    
  
