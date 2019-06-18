'''
Green Monster GUI Revamp
Containing VXWorks Tab
Code Commissioned 2019-01-16
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import utils as u

KILL_SERVER_1	=5001
KILL_SERVER_2	=5002
KILL_SERVER_3	=5003
KILL_SERVER_4	=5004
KILL_SERVER_5	=5005

class VXWorks(tk.Frame):
  def __init__(self, tab):
    self.vx_frame = tk.Frame(tab, bg=u.green_color)
    self.crateStrings = ["Kill VXWorks Server, CH","Kill VXWorks Server, INJ","Kill VXWorks Server, LftSpec","Kill VXWorks Server, RtSpec"]
    self.crateNumbers = [u.Crate_CH,u.Crate_INJ,u.Crate_LHRS,u.Crate_RHRS]
    self.buttonsList = []
    for counter in range(0,len(self.crateStrings)):
      self.buttonsList.append(tk.Button(self.vx_frame, text=self.crateStrings[counter], bg=u.green_color, width=30, command=self.kill_server))
      self.buttonsList[counter].crateN = self.crateNumbers[counter]
      self.buttonsList[counter].crateName = self.crateStrings[counter]
      self.buttonsList[counter].config(command = lambda but=self.buttonsList[counter]: self.kill_server(but))

      self.buttonsList[counter].grid(row=counter, column=0, padx=2, pady=2)
    self.vx_frame.place(relx=0.5, rely=0.5, anchor=tk.CENTER)

  def kill_server(self,but):
    #packet = [10, 10, 1, 2, 3, "Kill Server", "N"]
    #packet = [10, 10, 1, 2, 3, "Y", "Q"]
    #packet = [10, 10, 1, 2, 3, "Y", "Kill Server"]
    #packet = [10, 10, 1, 2, 3, "Kill Server", "Y"]
    #packet = [10, 10, 1, 2, 3, 1, tmpVar]
    packet = [10, 10, 1, 2, 3, "Not Q", "N"]
    err_flag, reply = u.send_command(but.crateN, packet)
    print("Reply = {}".format(reply))
    
    if err_flag == u.SOCK_OK:
      print("{}".format(but.crateName))

    else:
      print("ERROR, Could not access socket.")
      return -1
