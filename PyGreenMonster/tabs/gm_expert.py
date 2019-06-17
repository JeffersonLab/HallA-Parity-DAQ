'''
Green Monster GUI Revamp
Module Created: 2019-03-05
Author: AJZ
'''

import tkinter as tk
from tkinter import ttk
from ctypes import cdll

import gm_timeboard as tmbd
import gm_vqwk as vqwk
import gm_adc18 as adc18s
import gm_vxworks as vxworks
import utils as u

class Expert(tk.Frame):
  def __init__(self, tab):
    expert_style = ttk.Style()
    expert_style.configure('My.TButton', foreground=u.green_color)
    expert_style.configure('My.TFrame', background=u.green_color)
    
    tab_control = ttk.Notebook(tab)
    tab_titles = ['TimeBoard', 'VQWK ADCs', 'ADC18s, CH', 'VXWorks Server']
    
    for title in tab_titles:
        sub_tab = ttk.Frame(tab_control, width=800, height=600, style="My.TFrame")
        tab_control.add(sub_tab, text=title)
        
        if 'BLAH' in title: pass
        #elif 'Time' in title: tmbd.Timeboard(tab)
        elif 'VQWK' in title: vqwk.VQWK(tab)
        elif 'ADC18s' in title: adc18s.ADC18(tab)
        elif 'VXWorks' in title: vxworks.VXWorks(tab)
        else: tk.Label(sub_tab, text='TODD IN THE SHADOWS', bg=u.green_color).pack(padx=20, pady=20, anchor='center')
    tab_control.grid(row=0, column=0, columnspan=2)
    
    
