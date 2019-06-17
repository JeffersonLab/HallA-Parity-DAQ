'''
Green Monster GUI Revamp
Containing ScanUtil Tab
Code Commissioned 2019-01-16
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import utils as u


class ScanUtil(tk.Frame):
  def __init__(self, tab):
    self.util_frame = tk.LabelFrame(tab, text='SCAN UTILITY', bg=u.green_color)
    self.options = ['CLEAN', 'NOT CLEAN']
    self.clean_setting = tk.StringVar()
    self.clean_setting.set(self.options[0])
    self.inj_frame = tk.LabelFrame(self.util_frame, text='Inj', bg=u.green_color)
    self.inj_labels = [tk.Label(self.inj_frame, text='Set Point 1', bg=u.green_color),
                       tk.Label(self.inj_frame, text='Set Point 2', bg=u.green_color),
                       tk.Label(self.inj_frame, text='Set Point 3', bg=u.green_color),
                       tk.Label(self.inj_frame, text='Set Point 4', bg=u.green_color)]
    self.inj_entries = [tk.Entry(self.inj_frame),
                        tk.Entry(self.inj_frame),
                        tk.Entry(self.inj_frame),
                        tk.Entry(self.inj_frame)]
    for i, op in enumerate(self.options):
      tk.Radiobutton(self.util_frame, text=op, variable=self.clean_setting,
          value=op, bg=u.green_color).grid(row=0, column=i, padx=10, pady=10, sticky='W')
    self.fill_inj_frame()
    self.inj_frame.grid(row=1, column=0, columnspan=2, padx=10, pady=10, sticky='W')
    tk.Button(self.util_frame, text='Check Status', bg=u.green_color).grid(row=2, column=0, padx=10, pady=10)
    tk.Button(self.util_frame, text='Set Values', bg=u.green_color).grid(row=2, column=1, padx=10, pady=10)
    self.util_frame.pack(padx=20, pady=20, anchor='w')

  def fill_inj_frame(self):
    for r in range(0, 4):
      self.inj_labels[r].grid(row=r, column=0, padx=15, pady=10, sticky='E')
      u.set_text(self.inj_entries[r], '0').grid(row=r, column=1, padx=10, pady=10, sticky='W')
