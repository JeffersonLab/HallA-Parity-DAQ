'''
Green Monster GUI Revamp
Containing ADC18s Tab
Code Commissioned 2019-01-16
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import utils as u

class ADC18(tk.Frame):
  def __init__(self, tab):
    self.ch_frame = tk.LabelFrame(tab, text='CH', background=u.green_color)
    self.adc_ls = [tk.Label(self.ch_frame, text='ADC 8', background=u.green_color),
                   tk.Label(self.ch_frame, text='ADC 10', background=u.green_color),
                   tk.Label(self.ch_frame, text='ADC 10', background=u.green_color),
                   tk.Label(self.ch_frame, text='ADC 11', background=u.green_color)]
    self.int_es = [tk.Entry(self.ch_frame, width=3), 
                   tk.Entry(self.ch_frame, width=3),
                   tk.Entry(self.ch_frame, width=3),
                   tk.Entry(self.ch_frame, width=3)]
    self.conv_es = [tk.Entry(self.ch_frame, width=3), 
                    tk.Entry(self.ch_frame, width=3),
                    tk.Entry(self.ch_frame, width=3),
                    tk.Entry(self.ch_frame, width=3)]
    self.dac_settings = [tk.StringVar(), tk.StringVar(), tk.StringVar(), tk.StringVar()]
    self.sample_settings = [tk.IntVar(), tk.IntVar(), tk.IntVar(), tk.IntVar()]
    labels = ['Label', 'Int', 'Conv', '-----', 'DAC', 'Settings', '-----', 'Sample by:']
    for i, label in enumerate(labels):
      tk.Label(self.ch_frame, text=label, background=u.green_color).grid(
          row=0, column=i, padx=8, pady=10, sticky='W')
    self.create_table()
    
  def create_table(self):
    for i in range(1, 5):
      self.adc_ls[i-1].grid(row=i, column=0, padx=10, pady=10, sticky='W')
      u.set_text(self.int_es[i-1], '3').grid(row=i, column=1, padx=10, pady=10)
      u.set_text(self.conv_es[i-1], '0').grid(row=i, column=2, padx=10, pady=10)
      setting = self.dac_settings[i-1]
      settings = ['Tri', 'Saw', 'Const', 'Off']
      setting.set('Tri')
      for j,s in enumerate(settings):
        tk.Radiobutton(self.ch_frame, text=s, variable=setting, value=s, background=u.green_color).grid(
          row=i, column=j+3, padx=5, pady=10, sticky='W')
      sample_by = self.sample_settings[i-1]
      sample_by.set(1)
      tk.OptionMenu(self.ch_frame, sample_by, 1, 2, 4, 8).grid(row=i, column=7)
    tk.Button(self.ch_frame, text='Get Settings', background=u.green_color).grid(
        row=6, column=1, columnspan=2, pady=50, sticky='S')
    tk.Button(self.ch_frame, text='Apply Settings', background=u.green_color).grid(
        row=6, column=3, columnspan=2, pady=50, sticky='S')
    tk.Button(self.ch_frame, text='Cancel', background=u.green_color).grid(
        row=6, column=5, pady=50, sticky='S')
    self.ch_frame.pack(padx=20, pady=20)
