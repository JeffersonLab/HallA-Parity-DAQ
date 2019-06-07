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

GM_HAPTB	=1000
GM_TB_GET	=1001
GM_TB_SET	=1002
HAPTB_GET_DATA	=1001
HAPTB_SET_DATA	=1002
HAPTB_RD	=201
HAPTB_IT	=202
HAPTB_OS	=221
PATH      ="/adaqfs/home/apar/devices/crl/vqwkTiming.flags"

class Timeboard(tk.Frame):
  def __init__(self, tab):
    self.ch_frame = tk.LabelFrame(tab, text='CH', background=u.green_color, width=500)
    self.inj_frame = tk.LabelFrame(tab, text='Inj', background=u.green_color, width=500)
    self.lft_spec_frame = tk.LabelFrame(tab, text='LftSpec', background=u.green_color, width=500)
    self.rt_spec_frame = tk.LabelFrame(tab, text='RtSpec', background=u.green_color, width=500)
    self.defaults_frame = tk.LabelFrame(tab, text='Defaults', background=u.green_color, width=1000)

    self.ramp_delay_l = tk.Label(self.ch_frame, text='Ramp Delay', background=u.green_color)
    self.int_time_l = tk.Label(self.ch_frame, text='Integrate Time', background=u.green_color)
    self.oversamp_l = tk.Label(self.ch_frame, text='Oversampling', background=u.green_color)
    self.ramp_delay_e = tk.Entry(self.ch_frame)
    self.int_time_e = tk.Entry(self.ch_frame)
    self.oversamp_e = tk.Entry(self.ch_frame)

    self.ramp_delay_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.int_time_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.oversamp_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.ramp_delay_e, '0').grid(row=0, column=1, padx=10, pady=5, sticky='W')
    u.set_text(self.int_time_e, '0').grid(row=1, column=1, padx=10, pady=5, sticky='W')
    u.set_text(self.oversamp_e, '0').grid(row=2, column=1, padx=10, pady=5, sticky='W')
    tk.Button(self.ch_frame, text='Get Settings', background=u.green_color, command=self.check_values_ch).grid(
        row=3, column=0, pady=10)
    tk.Button(self.ch_frame, text='Apply Settings', background=u.green_color, command=self.set_values_ch).grid(
        row=3, column=1, pady=10)
    self.ch_frame.grid(row=0, column=0, padx=20, pady=10)

    self.inj_ramp_delay_l = tk.Label(self.inj_frame, text='Ramp Delay', background=u.green_color)
    self.inj_int_time_l = tk.Label(self.inj_frame, text='Integrate Time', background=u.green_color)
    self.inj_oversamp_l = tk.Label(self.inj_frame, text='Oversampling', background=u.green_color)
    self.inj_ramp_delay_e = tk.Entry(self.inj_frame)
    self.inj_int_time_e = tk.Entry(self.inj_frame)
    self.inj_oversamp_e = tk.Entry(self.inj_frame)

    self.inj_ramp_delay_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.inj_int_time_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.inj_oversamp_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.inj_ramp_delay_e, '0').grid(row=0, column=1, padx=10, pady=5, sticky='W')
    u.set_text(self.inj_int_time_e, '0').grid(row=1, column=1, padx=10, pady=5, sticky='W')
    u.set_text(self.inj_oversamp_e, '0').grid(row=2, column=1, padx=10, pady=5, sticky='W')
    tk.Button(self.inj_frame, text='Get Settings', background=u.green_color, command=self.check_values_inj).grid(
        row=3, column=0, pady=10)
    tk.Button(self.inj_frame, text='Apply Settings', background=u.green_color, command=self.set_values_inj).grid(
        row=3, column=1, pady=10)
    self.inj_frame.grid(row=0, column=1, padx=20, pady=10)

    self.lft_spec_ramp_delay_l = tk.Label(self.lft_spec_frame, text='Ramp Delay', background=u.green_color)
    self.lft_spec_int_time_l = tk.Label(self.lft_spec_frame, text='Integrate Time', background=u.green_color)
    self.lft_spec_oversamp_l = tk.Label(self.lft_spec_frame, text='Oversampling', background=u.green_color)
    self.lft_spec_ramp_delay_e = tk.Entry(self.lft_spec_frame)
    self.lft_spec_int_time_e = tk.Entry(self.lft_spec_frame)
    self.lft_spec_oversamp_e = tk.Entry(self.lft_spec_frame)

    self.lft_spec_ramp_delay_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.lft_spec_int_time_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.lft_spec_oversamp_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.lft_spec_ramp_delay_e, '0').grid(row=0, column=1, padx=10, pady=5, sticky='W')
    u.set_text(self.lft_spec_int_time_e, '0').grid(row=1, column=1, padx=10, pady=5, sticky='W')
    u.set_text(self.lft_spec_oversamp_e, '0').grid(row=2, column=1, padx=10, pady=5, sticky='W')
    tk.Button(self.lft_spec_frame, text='Get Settings', background=u.green_color, command=self.check_values_lft_spec).grid(
        row=3, column=0, pady=10)
    tk.Button(self.lft_spec_frame, text='Apply Settings', background=u.green_color, command=self.set_values_lft_spec).grid(
        row=3, column=1, pady=10)
    self.lft_spec_frame.grid(row=1, column=0, padx=20, pady=10)

    self.rt_spec_ramp_delay_l = tk.Label(self.rt_spec_frame, text='Ramp Delay', background=u.green_color)
    self.rt_spec_int_time_l = tk.Label(self.rt_spec_frame, text='Integrate Time', background=u.green_color)
    self.rt_spec_oversamp_l = tk.Label(self.rt_spec_frame, text='Oversampling', background=u.green_color)
    self.rt_spec_ramp_delay_e = tk.Entry(self.rt_spec_frame)
    self.rt_spec_int_time_e = tk.Entry(self.rt_spec_frame)
    self.rt_spec_oversamp_e = tk.Entry(self.rt_spec_frame)

    self.rt_spec_ramp_delay_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.rt_spec_int_time_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.rt_spec_oversamp_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.rt_spec_ramp_delay_e, '0').grid(row=0, column=1, padx=10, pady=5, sticky='W')
    u.set_text(self.rt_spec_int_time_e, '0').grid(row=1, column=1, padx=10, pady=5, sticky='W')
    u.set_text(self.rt_spec_oversamp_e, '0').grid(row=2, column=1, padx=10, pady=5, sticky='W')
    tk.Button(self.rt_spec_frame, text='Get Settings', background=u.green_color, command=self.check_values_rt_spec).grid(
        row=3, column=0, pady=10)
    tk.Button(self.rt_spec_frame, text='Apply Settings', background=u.green_color, command=self.set_values_rt_spec).grid(
        row=3, column=1, pady=10)
    self.rt_spec_frame.grid(row=1, column=1, padx=20, pady=10)

    tk.Button(self.defaults_frame, text='Get All', background=u.green_color, command=self.check_all).grid(
        row=0, column=0, padx=10, pady=10)
    tk.Button(self.defaults_frame, text='Set All', background=u.green_color, command=self.set_all).grid(
        row=0, column=1, padx=10, pady=10)
    tk.Button(self.defaults_frame, text='Get Defaults', background=u.green_color, command=self.read_defaults).grid(
        row=1, column=0, padx=10, pady=10)
    tk.Button(self.defaults_frame, text='Set Defaults', background=u.green_color, command=self.set_defaults).grid(
        row=1, column=1, padx=10, pady=10)

    self.defaults_frame.grid(row=2, column=0, padx=20, pady=10, columnspan=2)

    self.read_defaults()
    self.set_all()
    self.check_all()

  def read_defaults(self):
    global delayCH
    global inttimeCH
    global delayINJ
    global inttimeINJ
    global delayRHRS
    global inttimeRHRS
    global delayLHRS
    global inttimeLHRS

    infile = open(PATH,'r')
    for line in infile:
      if (line[0] == ';'):
        continue
      else:
        delayCH = int(line[(line.index("HAPTB_delay_CH=") + 15):line.index(",HAPTB_int_time_CH=")])
        inttimeCH = int(line[(line.index("HAPTB_int_time_CH=") + 18):line.index(",HAPTB_delay_INJ=")])

        delayINJ = int(line[(line.index("HAPTB_delay_INJ=") + 16):line.index(",HAPTB_int_time_INJ=")])
        inttimeINJ = int(line[(line.index("HAPTB_int_time_INJ=") + 19):line.index(",HAPTB_delay_RHRS=")])

        delayRHRS = int(line[(line.index("HAPTB_delay_RHRS=") + 17):line.index(",HAPTB_int_time_RHRS=")])
        inttimeRHRS = int(line[(line.index("HAPTB_int_time_RHRS=") + 20):line.index(",HAPTB_delay_LHRS=")])

        delayLHRS = int(line[(line.index("HAPTB_delay_LHRS=") + 17):line.index(",HAPTB_int_time_LHRS=")])
        inttimeLHRS = int(line[(line.index("HAPTB_int_time_LHRS=") + 20):line.index("\n")])
    infile.close()

    self.ramp_delay_e.delete(0, tk.END)
    self.ramp_delay_e.insert(0, str(delayCH))

    self.int_time_e.delete(0, tk.END)
    self.int_time_e.insert(0, str(inttimeCH))

    self.inj_ramp_delay_e.delete(0, tk.END)
    self.inj_ramp_delay_e.insert(0, str(delayINJ))

    self.inj_int_time_e.delete(0, tk.END)
    self.inj_int_time_e.insert(0, str(inttimeINJ))

    self.lft_spec_ramp_delay_e.delete(0, tk.END)
    self.lft_spec_ramp_delay_e.insert(0, str(delayLHRS))

    self.lft_spec_int_time_e.delete(0, tk.END)
    self.lft_spec_int_time_e.insert(0, str(inttimeLHRS))

    self.rt_spec_ramp_delay_e.delete(0, tk.END)
    self.rt_spec_ramp_delay_e.insert(0, str(delayRHRS))

    self.rt_spec_int_time_e.delete(0, tk.END)
    self.rt_spec_int_time_e.insert(0, str(inttimeRHRS))

  def set_defaults(self):
    delayCH = int(self.ramp_delay_e.get())
    inttimeCH = int(self.int_time_e.get())
    value3 = int(self.oversamp_e.get())

    delayINJ = int(self.inj_ramp_delay_e.get())
    inttimeINJ = int(self.inj_int_time_e.get())
    value3 = int(self.inj_oversamp_e.get())

    delayRHRS = int(self.rt_spec_ramp_delay_e.get())
    inttimeRHRS = int(self.rt_spec_int_time_e.get())
    value3 = int(self.rt_spec_oversamp_e.get())

    delayLHRS = int(self.lft_spec_ramp_delay_e.get())
    inttimeLHRS = int(self.lft_spec_int_time_e.get())
    value3 = int(self.lft_spec_oversamp_e.get())

    newdefaultstring = "AUTO_GENERATED_CONTENT=1,HAPTB_delay_CH="+str(delayCH)+",HAPTB_int_time_CH="+str(inttimeCH)+",HAPTB_delay_INJ="+str(delayINJ)+",HAPTB_int_time_INJ="+str(inttimeINJ)+",HAPTB_delay_RHRS="+str(delayRHRS)+",HAPTB_int_time_RHRS="+str(inttimeRHRS)+",HAPTB_delay_LHRS="+str(delayLHRS)+",HAPTB_int_time_LHRS="+str(inttimeLHRS)+"\n"

    buff = []
    i = 0
    infile = open(PATH,'r')
    for line in infile:
      buff.append(line)
      if (line[0] != ';'):
        buff[i] = (";" + line)
      i+=1
    infile.close()

    buff.append(newdefaultstring)

    outfile = open(PATH,'w')
    for i in range(len(buff)):
      outfile.write(buff[i])
    outfile.close()


  def check_all(self):
    self.check_values_ch()
    self.check_values_inj()
    self.check_values_lft_spec()
    self.check_values_rt_spec()

  def set_all(self):
    self.set_values_ch()
    self.set_values_inj()
    self.set_values_lft_spec()
    self.set_values_rt_spec()

  def check_values_ch(self):
    packet1 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_RD, 0, 0, "TB Get Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_CH, packet1)
    
    print("I am here where you thought I was")
    print("COMMAND_HAPTB is " + str(u.COMMAND_HAPTB))
    print("cfSockCommand returned :  " + str(err_flag))
    
    if err_flag == u.SOCK_OK:
      CurrentRD = int(reply1[3])
      self.ramp_delay_e.delete(0, tk.END)
      self.ramp_delay_e.insert(0, str(CurrentRD))
      print("Ramp delay is " + str(CurrentRD))

    else:
      print("ERROR, Could not access socket.")
      return

    packet2 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_IT, 0, 0, "TB Get Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_CH, packet2)
    
    if err_flag == u.SOCK_OK:
      CurrentIT = int(reply2[3])
      self.int_time_e.delete(0, tk.END)
      self.int_time_e.insert(0, str(CurrentIT))
      print("Integration time is " + str(CurrentIT))

    else:
      print("ERROR, Could not access socket.")
      return

    packet3 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_OS, 0, 0, "TB Get Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_CH, packet3)
    
    if err_flag == u.SOCK_OK:
      CurrentOS = int(reply3[3])
      self.oversamp_e.delete(0, tk.END)
      self.oversamp_e.insert(0, str(CurrentOS))
      print("Oversampling is " + str(CurrentOS))

    else:
      print("ERROR, Could not access socket.")
      return

  def check_values_rt_spec(self):
    packet1 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_RD, 0, 0, "TB Get Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_RHRS, packet1)
    
    print("I am here where you thought I was")
    print("COMMAND_HAPTB is " + str(u.COMMAND_HAPTB))
    print("cfSockCommand returned :  " + str(err_flag))
    
    if err_flag == u.SOCK_OK:
      CurrentRD = int(reply1[3])
      self.rt_spec_ramp_delay_e.delete(0, tk.END)
      self.rt_spec_ramp_delay_e.insert(0, str(CurrentRD))
      print("Ramp delay is " + str(CurrentRD))

    else:
      print("ERROR, Could not access socket.")
      return

    packet2 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_IT, 0, 0, "TB Get Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_RHRS, packet2)
    
    if err_flag == u.SOCK_OK:
      CurrentIT = int(reply2[3])
      self.rt_spec_int_time_e.delete(0, tk.END)
      self.rt_spec_int_time_e.insert(0, str(CurrentIT))
      print("Integration time is " + str(CurrentIT))

    else:
      print("ERROR, Could not access socket.")
      return

    packet3 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_OS, 0, 0, "TB Get Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_RHRS, packet3)
    
    if err_flag == u.SOCK_OK:
      CurrentOS = int(reply3[3])
      self.rt_spec_oversamp_e.delete(0, tk.END)
      self.rt_spec_oversamp_e.insert(0, str(CurrentOS))
      print("Oversampling is " + str(CurrentOS))

    else:
      print("ERROR, Could not access socket.")
      return

  def check_values_lft_spec(self):
    packet1 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_RD, 0, 0, "TB Get Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_LHRS, packet1)
    
    print("I am here where you thought I was")
    print("COMMAND_HAPTB is " + str(u.COMMAND_HAPTB))
    print("cfSockCommand returned :  " + str(err_flag))
    
    if err_flag == u.SOCK_OK:
      CurrentRD = int(reply1[3])
      self.lft_spec_ramp_delay_e.delete(0, tk.END)
      self.lft_spec_ramp_delay_e.insert(0, str(CurrentRD))
      print("Ramp delay is " + str(CurrentRD))

    else:
      print("ERROR, Could not access socket.")
      return

    packet2 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_IT, 0, 0, "TB Get Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_LHRS, packet2)
    
    if err_flag == u.SOCK_OK:
      CurrentIT = int(reply2[3])
      self.lft_spec_int_time_e.delete(0, tk.END)
      self.lft_spec_int_time_e.insert(0, str(CurrentIT))
      print("Integration time is " + str(CurrentIT))

    else:
      print("ERROR, Could not access socket.")
      return

    packet3 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_OS, 0, 0, "TB Get Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_LHRS, packet3)
    
    if err_flag == u.SOCK_OK:
      CurrentOS = int(reply3[3])
      self.lft_spec_oversamp_e.delete(0, tk.END)
      self.lft_spec_oversamp_e.insert(0, str(CurrentOS))
      print("Oversampling is " + str(CurrentOS))

    else:
      print("ERROR, Could not access socket.")
      return

  def check_values_inj(self):
    packet1 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_RD, 0, 0, "TB Get Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_INJ, packet1)
    
    print("I am here where you thought I was")
    print("COMMAND_HAPTB is " + str(u.COMMAND_HAPTB))
    print("cfSockCommand returned :  " + str(err_flag))
    
    if err_flag == u.SOCK_OK:
      CurrentRD = int(reply1[3])
      self.inj_ramp_delay_e.delete(0, tk.END)
      self.inj_ramp_delay_e.insert(0, str(CurrentRD))
      print("Ramp delay is " + str(CurrentRD))

    else:
      print("ERROR, Could not access socket.")
      return

    packet2 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_IT, 0, 0, "TB Get Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_INJ, packet2)
    
    if err_flag == u.SOCK_OK:
      CurrentIT = int(reply2[3])
      self.inj_int_time_e.delete(0, tk.END)
      self.inj_int_time_e.insert(0, str(CurrentIT))
      print("Integration time is " + str(CurrentIT))

    else:
      print("ERROR, Could not access socket.")
      return

    packet3 = [u.COMMAND_HAPTB, HAPTB_GET_DATA, HAPTB_OS, 0, 0, "TB Get Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_INJ, packet3)
    
    if err_flag == u.SOCK_OK:
      CurrentOS = int(reply3[3])
      self.inj_oversamp_e.delete(0, tk.END)
      self.inj_oversamp_e.insert(0, str(CurrentOS))
      print("Oversampling is " + str(CurrentOS))

    else:
      print("ERROR, Could not access socket.")
      return

  def set_values_ch(self):

    value1 = int(self.ramp_delay_e.get())
    value2 = int(self.int_time_e.get())
    value3 = int(self.oversamp_e.get())

    packet1 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_RD, value1, 0, "TB Set Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_CH, packet1)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply1[1] != 1:
        if reply1[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply1[1]))
          othererror = 1
      else:
        if reply1[2] != HAPTB_RD:
          print("Server replied with wrong TB number: " +str(reply1[2])+ " instead of " +str(HAPTB_RD))
          othererror = 1
        if reply1[3] != value1:
          print("Server replied with wrong TB set value: " +str(reply1[3])+ " instead of " +str(value1))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    packet2 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_IT, value2, 0, "TB Set Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_CH, packet2)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply2[1] != 1:
        if reply2[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply2[1]))
          othererror = 1
      else:
        if reply2[2] != HAPTB_IT:
          print("Server replied with wrong TB number: " +str(reply2[2])+ " instead of " +str(HAPTB_IT))
          othererror = 1
        if reply2[3] != value2:
          print("Server replied with wrong TB set value: " +str(reply2[3])+ " instead of " +str(value2))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    packet3 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_OS, value3, 0, "TB Set Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_CH, packet3)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply3[1] != 1:
        if reply3[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply3[1]))
          othererror = 1
      else:
        if reply3[2] != HAPTB_OS:
          print("Server replied with wrong TB number: " +str(reply3[2])+ " instead of " +str(HAPTB_OS))
          othererror = 1
        if reply3[3] != value3:
          print("Server replied with wrong TB set value: " +str(reply3[3])+ " instead of " +str(value3))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    self.check_values_ch()

  def set_values_rt_spec(self):

    value1 = int(self.rt_spec_ramp_delay_e.get())
    value2 = int(self.rt_spec_int_time_e.get())
    value3 = int(self.rt_spec_oversamp_e.get())

    packet1 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_RD, value1, 0, "TB Set Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_RHRS, packet1)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply1[1] != 1:
        if reply1[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply1[1]))
          othererror = 1
      else:
        if reply1[2] != HAPTB_RD:
          print("Server replied with wrong TB number: " +str(reply1[2])+ " instead of " +str(HAPTB_RD))
          othererror = 1
        if reply1[3] != value1:
          print("Server replied with wrong TB set value: " +str(reply1[3])+ " instead of " +str(value1))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    packet2 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_IT, value2, 0, "TB Set Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_RHRS, packet2)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply2[1] != 1:
        if reply2[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply2[1]))
          othererror = 1
      else:
        if reply2[2] != HAPTB_IT:
          print("Server replied with wrong TB number: " +str(reply2[2])+ " instead of " +str(HAPTB_IT))
          othererror = 1
        if reply2[3] != value2:
          print("Server replied with wrong TB set value: " +str(reply2[3])+ " instead of " +str(value2))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    packet3 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_OS, value3, 0, "TB Set Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_RHRS, packet3)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply3[1] != 1:
        if reply3[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply3[1]))
          othererror = 1
      else:
        if reply3[2] != HAPTB_OS:
          print("Server replied with wrong TB number: " +str(reply3[2])+ " instead of " +str(HAPTB_OS))
          othererror = 1
        if reply3[3] != value3:
          print("Server replied with wrong TB set value: " +str(reply3[3])+ " instead of " +str(value3))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    self.check_values_rt_spec()

  def set_values_lft_spec(self):

    value1 = int(self.lft_spec_ramp_delay_e.get())
    value2 = int(self.lft_spec_int_time_e.get())
    value3 = int(self.lft_spec_oversamp_e.get())

    packet1 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_RD, value1, 0, "TB Set Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_LHRS, packet1)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply1[1] != 1:
        if reply1[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply1[1]))
          othererror = 1
      else:
        if reply1[2] != HAPTB_RD:
          print("Server replied with wrong TB number: " +str(reply1[2])+ " instead of " +str(HAPTB_RD))
          othererror = 1
        if reply1[3] != value1:
          print("Server replied with wrong TB set value: " +str(reply1[3])+ " instead of " +str(value1))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    packet2 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_IT, value2, 0, "TB Set Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_LHRS, packet2)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply2[1] != 1:
        if reply2[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply2[1]))
          othererror = 1
      else:
        if reply2[2] != HAPTB_IT:
          print("Server replied with wrong TB number: " +str(reply2[2])+ " instead of " +str(HAPTB_IT))
          othererror = 1
        if reply2[3] != value2:
          print("Server replied with wrong TB set value: " +str(reply2[3])+ " instead of " +str(value2))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    packet3 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_OS, value3, 0, "TB Set Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_LHRS, packet3)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply3[1] != 1:
        if reply3[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply3[1]))
          othererror = 1
      else:
        if reply3[2] != HAPTB_OS:
          print("Server replied with wrong TB number: " +str(reply3[2])+ " instead of " +str(HAPTB_OS))
          othererror = 1
        if reply3[3] != value3:
          print("Server replied with wrong TB set value: " +str(reply3[3])+ " instead of " +str(value3))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    self.check_values_lft_spec()

  def set_values_inj(self):

    value1 = int(self.inj_ramp_delay_e.get())
    value2 = int(self.inj_int_time_e.get())
    value3 = int(self.inj_oversamp_e.get())

    packet1 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_RD, value1, 0, "TB Set Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_INJ, packet1)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply1[1] != 1:
        if reply1[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply1[1]))
          othererror = 1
      else:
        if reply1[2] != HAPTB_RD:
          print("Server replied with wrong TB number: " +str(reply1[2])+ " instead of " +str(HAPTB_RD))
          othererror = 1
        if reply1[3] != value1:
          print("Server replied with wrong TB set value: " +str(reply1[3])+ " instead of " +str(value1))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    packet2 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_IT, value2, 0, "TB Set Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_INJ, packet2)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply2[1] != 1:
        if reply2[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply2[1]))
          othererror = 1
      else:
        if reply2[2] != HAPTB_IT:
          print("Server replied with wrong TB number: " +str(reply2[2])+ " instead of " +str(HAPTB_IT))
          othererror = 1
        if reply2[3] != value2:
          print("Server replied with wrong TB set value: " +str(reply2[3])+ " instead of " +str(value2))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    packet3 = [u.COMMAND_HAPTB, HAPTB_SET_DATA, HAPTB_OS, value3, 0, "TB Set Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_INJ, packet3)
    
    othererror = 0
    if err_flag == u.SOCK_OK:
      if reply3[1] != 1:
        if reply3[1] == -2:
          print("Cannot set parameter, CODA run in progress!")
        else:
          print("Error:Server replied with TB error code: " + str(reply3[1]))
          othererror = 1
      else:
        if reply3[2] != HAPTB_OS:
          print("Server replied with wrong TB number: " +str(reply3[2])+ " instead of " +str(HAPTB_OS))
          othererror = 1
        if reply3[3] != value3:
          print("Server replied with wrong TB set value: " +str(reply3[3])+ " instead of " +str(value3))
          othererror = 1
    else:
      print(" check_status: ERROR, Could not access socket.")
    if othererror == 1:
      print("Unknown error, cannot set TB parameter")

    self.check_values_inj()

