'''
Green Monster GUI Revamp
Containing VQWK Tab
Code Commissioned 2019-01-16
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import utils as u
from ctypes import cdll

class VQWK(tk.Frame):
  def __init__(self, tab):
    self.lib = cdll.LoadLibrary('./libs/libGVQWK.so')
    self.obj = self.lib.init(4141, 10, 4, 496, 10, 4)

    self.ch_frame = tk.LabelFrame(tab, text='CH', background=u.green_color, width=500)
    self.inj_frame = tk.LabelFrame(tab, text='Inj', background=u.green_color, width=500)
    self.samples_ch_l = tk.Label(self.ch_frame, text='Samples Per Block', background=u.green_color)
    self.gate_ch_l = tk.Label(self.ch_frame, text='Gate Delay', background=u.green_color)
    self.blocks_ch_l = tk.Label(self.ch_frame, text='Number of Blocks', background=u.green_color)
    self.samples_ch_e = tk.Entry(self.ch_frame)
    self.gate_ch_e = tk.Entry(self.ch_frame)
    self.blocks_ch_e = tk.Entry(self.ch_frame)
    self.fill_ch_frame()
    self.ch_frame.grid(row=0, column=0, padx=20, pady=10)

    self.samples_inj_l = tk.Label(self.inj_frame, text='Samples Per Block', background=u.green_color)
    self.gate_inj_l = tk.Label(self.inj_frame, text='Gate Delay', background=u.green_color)
    self.blocks_inj_l = tk.Label(self.inj_frame, text='Number of Blocks', background=u.green_color)
    self.samples_inj_e = tk.Entry(self.inj_frame)
    self.gate_inj_e = tk.Entry(self.inj_frame)
    self.blocks_inj_e = tk.Entry(self.inj_frame)
    self.fill_inj_frame()
    self.inj_frame.grid(row=0, column=1, padx=20, pady=10)

  def fill_ch_frame(self):
    self.samples_ch_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.gate_ch_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.blocks_ch_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.samples_ch_e, '4141').grid(row=0, column=1)
    u.set_text(self.gate_ch_e, '10').grid(row=1, column=1)
    u.set_text(self.blocks_ch_e, '4').grid(row=2, column=1)
    tk.Button(self.ch_frame, text='Get Settings', background=u.green_color, command=self.get_settings_ch).grid(
        row=3, column=0, pady=10)
    tk.Button(self.ch_frame, text='Apply Settings', background=u.green_color, command=self.set_settings_ch).grid(
        row=3, column=1, pady=10)

  def fill_inj_frame(self):
    self.samples_inj_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.gate_inj_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.blocks_inj_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.samples_inj_e, '496').grid(row=0, column=1)
    u.set_text(self.gate_inj_e, '10').grid(row=1, column=1)
    u.set_text(self.blocks_inj_e, '4').grid(row=2, column=1)
    tk.Button(self.inj_frame, text='Get Settings', background=u.green_color, command=self.get_settings_inj).grid(
        row=3, column=0, pady=10)
    tk.Button(self.inj_frame, text='Apply Settings', background=u.green_color, command=self.get_settings_inj).grid(
        row=3, column=1, pady=10)

  def get_samp_ch(self):
    new_samp = self.lib.getSampVal1(self.obj)
    self.samples_ch_e = u.set_text(self.samples_ch_e, new_samp)

  def set_samp_ch(self):
    self.lib.setSampVal1(self.obj, int(self.samples_ch_e.get()))

  def get_gate_ch(self):
    new_gate = self.lib.getGateVal1(self.obj)
    self.gate_ch_e = u.set_text(self.gate_ch_e, new_gate)

  def set_gate_ch(self):
    self.lib.setGateVal1(self.obj, int(self.gate_ch_e.get()))

  def get_blocks_ch(self):
    new_blok = self.lib.getBlokVal1(self.obj)
    self.blocks_ch_e = u.set_text(self.blocks_ch_e, new_blok)

  def set_blocks_ch(self):
    self.lib.setBlokVal1(self.obj, int(self.blocks_ch_e.get()))

  def get_samp_inj(self):
    new_samp = self.lib.getSampVal2(self.obj)
    self.samples_inj_e = u.set_text(self.samples_inj_e, new_samp)

  def set_samp_inj(self):
    self.lib.setSampVal2(self.obj, int(self.samples_inj_e.get()))

  def get_gate_inj(self):
    new_gate = self.lib.getGateVal2(self.obj)
    self.gate_inj_e = u.set_text(self.gate_inj_e, new_gate)

  def set_gate_inj(self):
    self.lib.setGateVal2(self.obj, int(self.gate_inj_e.get()))

  def get_blocks_inj(self):
    new_blok = self.lib.getBlokVal2(self.obj)
    self.blocks_inj_e = u.set_text(self.blocks_inj_e, new_blok)

  def set_blocks_inj(self):
    self.lib.setBlokVal2(self.obj, int(self.blocks_inj_e.get()))

  def get_settings_ch(self):
    self.get_samp_ch()
    self.get_gate_ch()
    self.get_blocks_ch()

  def set_settings_ch(self):
    self.set_samp_ch()
    self.set_gate_ch()
    self.set_blocks_ch()

  def get_settings_inj(self):
    self.get_samp_inj()
    self.get_gate_inj()
    self.get_blocks_inj()

  def set_settings_inj(self):
    self.set_samp_inj()
    self.set_gate_inj()
    self.set_blocks_inj()
    
