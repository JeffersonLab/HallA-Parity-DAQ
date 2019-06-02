'''
Green Monster GUI Revamp
Containing VXWorks Tab
Code Commissioned 2019-01-16
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import utils as u


class VXWorks(tk.Frame):
  def __init__(self, tab):
    self.vx_frame = tk.Frame(tab, bg=u.green_color)
    tk.Button(self.vx_frame, text='Kill VXWorks Server, CH', bg=u.green_color, width=30).grid(
        row=0, column=0, padx=2, pady=2)
    tk.Button(self.vx_frame, text='Kill VXWorks Server, Inj', bg=u.green_color, width=30).grid(
        row=1, column=0, padx=2, pady=2)
    tk.Button(self.vx_frame, text='Kill VXWorks Server, LftSpec', bg=u.green_color, width=30).grid(
        row=2, column=0, padx=2, pady=2)
    tk.Button(self.vx_frame, text='Kill VXWorks Server, RtSpec', bg=u.green_color, width=30).grid(
        row=3, column=0, padx=2, pady=2)
    self.vx_frame.place(relx=0.5, rely=0.5, anchor=tk.CENTER)
