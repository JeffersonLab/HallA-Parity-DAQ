'''
Green Monster GUI Revamp
Code Commissioned 2019-01-04
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import os
#from PIL import Image, ImageTk

import gm_bmw as bmw
import gm_scan as scan_util
import gm_timeboard as tmbd
import gm_vqwk as vqwk
import gm_adc18 as adc18s
import gm_vxworks as vxworks
import utils as u

class GreenMonster:

    def __init__(self):
        os.environ["TCL_LIBRARY"] = "/lib64/tcl8.5/"
        self.win = tk.Tk()
        self.win.title("Green Monster")
        self.win.configure(background=u.green_color)
        self.adc_options_vars = [tk.StringVar(), tk.StringVar(), tk.StringVar(), tk.StringVar()]
        self.bm_test_setting = tk.StringVar()
        self.clean_setting = tk.StringVar()
        self.get_green_monster_style()
        self.create_widgets()

    def get_green_monster_style(self):
        style = ttk.Style()
        style.theme_create("green_monster", parent="alt", settings={
            "TNotebook": {"configure": {"background": u.green_color}},
            "TNotebook.Tab": {"configure": {"background": u.green_color}}})
        style.theme_use("green_monster")

    def quit(self):
        self.win.quit()
        self.win.destroy()
        exit()

    def expert_tab(self, expt_tab):
        tab_control = ttk.Notebook(expt_tab)
        tab_titles = ['TimeBoard', 'VQWK ADCs', 'ADC18s, CH', 'VXWorks Server']
    
        for title in tab_titles:
            sub_tab = ttk.Frame(tab_control, width=800, height=600, style="My.TFrame")
            tab_control.add(sub_tab, text=title)
        
            if 'Time' in title: tmbd.Timeboard(sub_tab)
            elif 'VQWK' in title: vqwk.VQWK(sub_tab)
            elif 'ADC18s' in title: adc18s.ADC18(sub_tab)
            elif 'VXWorks' in title: vxworks.VXWorks(sub_tab)
            else: tk.Label(sub_tab, text='Default', bg=u.green_color).pack(padx=20, pady=20, anchor='center')
        tab_control.grid(row=0, column=0, columnspan=2)

    def create_widgets(self):
        gui_style = ttk.Style()
        gui_style.configure('My.TButton', foreground=u.green_color)
        gui_style.configure('My.TFrame', background=u.green_color)

        tab_control = ttk.Notebook(self.win)
        tab_titles = ['BMW', 'ScanUtil', 'Expert']
        for title in tab_titles:
            tab = ttk.Frame(tab_control, width=800, height=600, style="My.TFrame")
            tab_control.add(tab, text=title)
            if 'BMW' in title:
                bmw.BMW(tab)
            elif 'Scan' in title:
                scan_util.ScanUtil(tab)
            elif 'Expert' in title:
                self.expert_tab(tab)
        tab_control.grid(row=0, column=0, columnspan=2)
        #fenway = ImageTk.PhotoImage(Image.open('Green_Monster.jpg'))
        fenway_pahk = tk.Label(self.win, text='GREEN MONSTER', background=u.green_color)
        #fenway_pahk.image = fenway
        fenway_pahk.grid(row=1, column=0, padx=5, pady=10, sticky='SW')
        tk.Button(self.win, text='QUIT', command=quit, background=u.green_color, width=20, height=4).grid(
            row=1, column=1, padx=15, pady=5, sticky='SE')

green_monster = GreenMonster()
green_monster.win.mainloop()
