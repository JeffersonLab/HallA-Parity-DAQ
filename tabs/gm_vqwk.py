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

GM_VQWK		=7000
GM_VQWK_GET	=7001
GM_VQWK_SET	=7002
VQWK_GET_DATA	=7001
VQWK_SET_DATA	=7002
VQWK_SPB	=701
VQWK_GD		=702
VQWK_NB		=703
PATH      ="/adaqfs/home/apar/devices/crl/injector/g0inj.flags"

class VQWK(tk.Frame):
  def __init__(self, tab):
    self.ch_frame = tk.LabelFrame(tab, text='CH', background=u.green_color, width=500)
    self.inj_frame = tk.LabelFrame(tab, text='Inj', background=u.green_color, width=500)
    self.lft_spec_frame = tk.LabelFrame(tab, text='LftSpec', background=u.green_color, width=500)
    self.rt_spec_frame = tk.LabelFrame(tab, text='RtSpec', background=u.green_color, width=500)
    self.defaults_frame = tk.LabelFrame(tab, text='Defaults', background=u.green_color, width=1000)

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

    self.samples_lft_spec_l = tk.Label(self.lft_spec_frame, text='Samples Per Block', background=u.green_color)
    self.gate_lft_spec_l = tk.Label(self.lft_spec_frame, text='Gate Delay', background=u.green_color)
    self.blocks_lft_spec_l = tk.Label(self.lft_spec_frame, text='Number of Blocks', background=u.green_color)
    self.samples_lft_spec_e = tk.Entry(self.lft_spec_frame)
    self.gate_lft_spec_e = tk.Entry(self.lft_spec_frame)
    self.blocks_lft_spec_e = tk.Entry(self.lft_spec_frame)
    self.fill_lft_spec_frame()
    self.lft_spec_frame.grid(row=1, column=0, padx=20, pady=10)

    self.samples_rt_spec_l = tk.Label(self.rt_spec_frame, text='Samples Per Block', background=u.green_color)
    self.gate_rt_spec_l = tk.Label(self.rt_spec_frame, text='Gate Delay', background=u.green_color)
    self.blocks_rt_spec_l = tk.Label(self.rt_spec_frame, text='Number of Blocks', background=u.green_color)
    self.samples_rt_spec_e = tk.Entry(self.rt_spec_frame)
    self.gate_rt_spec_e = tk.Entry(self.rt_spec_frame)
    self.blocks_rt_spec_e = tk.Entry(self.rt_spec_frame)
    self.fill_rt_spec_frame()
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
    global samplesCH
    global gateCH
    global blocksCH
    global samplesINJ
    global gateINJ
    global blocksINJ
    global samplesRHRS
    global gateRHRS
    global blocksRHRS
    global samplesLHRS
    global gateLHRS
    global blocksLHRS

    infile = open(PATH,'r')
    for line in infile:
      if (line[0] == ';'):
        continue
      else:
        samplesCH = int(line[(line.index("CHvqwksamples=") + 14):line.index(", CHvqwkdelay=")])
        gateCH = int(line[(line.index("CHvqwkdelay=") + 12):line.index(", CHvqwkblocks=")])
        blocksCH = int(line[(line.index("CHvqwkblocks=") + 13):line.index(", CHvqwkperiod=")])

        samplesINJ = int(line[(line.index("INJvqwksamples=") + 14):line.index(", INJvqwkdelay=")])
        gateINJ = int(line[(line.index("INJvqwkdelay=") + 12):line.index(", INJvqwkblocks=")])
        blocksINJ = int(line[(line.index("INJvqwkblocks=") + 13):line.index(", INJvqwkperiod=")])

        samplesRHRS = int(line[(line.index("RHRSvqwksamples=") + 14):line.index(", RHRSvqwkdelay=")])
        gateRHRS = int(line[(line.index("RHRSvqwkdelay=") + 12):line.index(", RHRSvqwkblocks=")])
        blocksRHRS = int(line[(line.index("RHRSvqwkblocks=") + 13):line.index(", RHRSvqwkperiod=")])

        samplesLHRS = int(line[(line.index("LHRSvqwksamples=") + 14):line.index(", LHRSvqwkdelay=")])
        gateLHRS = int(line[(line.index("LHRSvqwkdelay=") + 12):line.index(", LHRSvqwkblocks=")])
        blocksLHRS = int(line[(line.index("LHRSvqwkblocks=") + 13):line.index(", LHRSvqwkperiod=")])
    infile.close()

    self.samples_ch_e.delete(0, tk.END)
    self.samples_ch_e.insert(0, str(samplesCH))

    self.gate_ch_e.delete(0, tk.END)
    self.gate_ch_e.insert(0, str(gateCH))

    self.blocks_ch_e.delete(0, tk.END)
    self.blocks_ch_e.insert(0, str(blocksCH))

    self.samples_inj_e.delete(0, tk.END)
    self.samples_inj_e.insert(0, str(samplesINJ))

    self.gate_inj_e.delete(0, tk.END)
    self.gate_inj_e.insert(0, str(gateINJ))

    self.blocks_inj_e.delete(0, tk.END)
    self.blocks_inj_e.insert(0, str(blocksINJ))

    self.samples_rt_spec_e.delete(0, tk.END)
    self.samples_rt_spec_e.insert(0, str(samplesRHRS))

    self.gate_rt_spec_e.delete(0, tk.END)
    self.gate_rt_spec_e.insert(0, str(gateRHRS))

    self.blocks_rt_spec_e.delete(0, tk.END)
    self.blocks_rt_spec_e.insert(0, str(blocksRHRS))

    self.samples_lft_spec_e.delete(0, tk.END)
    self.samples_lft_spec_e.insert(0, str(samplesLHRS))

    self.gate_lft_spec_e.delete(0, tk.END)
    self.gate_lft_spec_e.insert(0, str(gateLHRS))

    self.blocks_lft_spec_e.delete(0, tk.END)
    self.blocks_lft_spec_e.insert(0, str(blocksLHRS))

  def set_defaults(self):
    samplesCH = int(self.samples_ch_e.get())
    gateCH = int(self.gate_ch_e.get())
    blocksCH = int(self.blocks_ch_e.get())

    samplesINJ = int(self.samples_inj_e.get())
    gateINJ = int(self.gate_inj_e.get())
    blocksINJ = int(self.blocks_inj_e.get())

    samplesRHRS = int(self.samples_rt_spec_e.get())
    gateRHRS = int(self.gate_rt_spec_e.get())
    blocksRHRS = int(self.blocks_rt_spec_e.get())

    samplesLHRS = int(self.samples_lft_spec_e.get())
    gateLHRS = int(self.gate_lft_spec_e.get())
    blocksLHRS = int(self.blocks_lft_spec_e.get())

    newdefaultstringCH = "CHcrateheader=3, CHbadc=0x80, CHnadc=10, CHvqwksamples="+str(samplesCH)+", CHvqwkdelay="+str(gateCH)+", CHvqwkblocks="+str(blocksCH)+", CHvqwkperiod=0, CHvqwkinternal=2, CHnscaler=1, "
    newdefaultstringINJ = "INJcrateheader=3, INJbadc=0x80, INJnadc=10, INJvqwksamples="+str(samplesINJ)+", INJvqwkdelay="+str(gateINJ)+", INJvqwkblocks="+str(blocksINJ)+", INJvqwkperiod=0, INJvqwkinternal=2, INJnscaler=1, "
    newdefaultstringRHRS = "RHRScrateheader=3, RHRSbadc=0x80, RHRSnadc=10, RHRSvqwksamples="+str(samplesRHRS)+", RHRSvqwkdelay="+str(gateRHRS)+", RHRSvqwkblocks="+str(blocksRHRS)+", RHRSvqwkperiod=0, RHRSvqwkinternal=2, RHRSnscaler=1, "
    newdefaultstringLHRS = "LHRScrateheader=3, LHRSbadc=0x80, LHRSnadc=10, LHRSvqwksamples="+str(samplesLHRS)+", LHRSvqwkdelay="+str(gateLHRS)+", LHRSvqwkblocks="+str(blocksLHRS)+", LHRSvqwkperiod=0, LHRSvqwkinternal=2, LHRSnscaler=1\n"

    buff = []
    i = 0
    infile = open(PATH,'r')
    for line in infile:
      buff.append(line)
      if (line[0] != ';'):
        buff[i] = (";" + line)
      i+=1
    infile.close()

    buff.append(newdefaultstringCH + newdefaultstringINJ + newdefaultstringRHRS + newdefaultstringLHRS)

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

  def fill_ch_frame(self):
    self.samples_ch_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.gate_ch_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.blocks_ch_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.samples_ch_e, '0').grid(row=0, column=1)
    u.set_text(self.gate_ch_e, '0').grid(row=1, column=1)
    u.set_text(self.blocks_ch_e, '0').grid(row=2, column=1)
    tk.Button(self.ch_frame, text='Get Settings', background=u.green_color, command=self.check_values_ch).grid(
        row=3, column=0, pady=10)
    tk.Button(self.ch_frame, text='Apply Settings', background=u.green_color, command=self.set_values_ch).grid(
        row=3, column=1, pady=10)

  def fill_inj_frame(self):
    self.samples_inj_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.gate_inj_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.blocks_inj_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.samples_inj_e, '0').grid(row=0, column=1)
    u.set_text(self.gate_inj_e, '0').grid(row=1, column=1)
    u.set_text(self.blocks_inj_e, '0').grid(row=2, column=1)
    tk.Button(self.inj_frame, text='Get Settings', background=u.green_color, command=self.check_values_inj).grid(
        row=3, column=0, pady=10)
    tk.Button(self.inj_frame, text='Apply Settings', background=u.green_color, command=self.set_values_inj).grid(
        row=3, column=1, pady=10)

  def fill_lft_spec_frame(self):
    self.samples_lft_spec_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.gate_lft_spec_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.blocks_lft_spec_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.samples_lft_spec_e, '0').grid(row=0, column=1)
    u.set_text(self.gate_lft_spec_e, '0').grid(row=1, column=1)
    u.set_text(self.blocks_lft_spec_e, '0').grid(row=2, column=1)
    tk.Button(self.lft_spec_frame, text='Get Settings', background=u.green_color, command=self.check_values_lft_spec).grid(
        row=3, column=0, pady=10)
    tk.Button(self.lft_spec_frame, text='Apply Settings', background=u.green_color, command=self.set_values_lft_spec).grid(
        row=3, column=1, pady=10)

  def fill_rt_spec_frame(self):
    self.samples_rt_spec_l.grid(row=0, column=0, padx=10, pady=5, sticky='W')
    self.gate_rt_spec_l.grid(row=1, column=0, padx=10, pady=5, sticky='W')
    self.blocks_rt_spec_l.grid(row=2, column=0, padx=10, pady=5, sticky='W')
    u.set_text(self.samples_rt_spec_e, '0').grid(row=0, column=1)
    u.set_text(self.gate_rt_spec_e, '0').grid(row=1, column=1)
    u.set_text(self.blocks_rt_spec_e, '0').grid(row=2, column=1)
    tk.Button(self.rt_spec_frame, text='Get Settings', background=u.green_color, command=self.check_values_rt_spec).grid(
        row=3, column=0, pady=10)
    tk.Button(self.rt_spec_frame, text='Apply Settings', background=u.green_color, command=self.set_values_rt_spec).grid(
        row=3, column=1, pady=10)

  def check_values_ch(self):
    packet1 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_SPB, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_CH, packet1)

    print("COMMAND_VQWK is " + str(u.COMMAND_VQWK))
    print("cfSockCommand returned :  " + str(err_flag))
    
    if err_flag == u.SOCK_OK:
      CurrentSPB = int(reply1[3])
      self.samples_ch_e.delete(0, tk.END)
      self.samples_ch_e.insert(0, str(CurrentSPB))

    else:
      print("ERROR, Could not access socket.")
      return

    packet2 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_GD, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_CH, packet2)
    
    if err_flag == u.SOCK_OK:
      CurrentGD = int(reply2[3])
      self.gate_ch_e.delete(0, tk.END)
      self.gate_ch_e.insert(0, str(CurrentGD))

    else:
      print("ERROR, Could not access socket.")
      return

    packet3 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_NB, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_CH, packet3)
    
    if err_flag == u.SOCK_OK:
      CurrentNB = int(reply3[3])
      self.blocks_ch_e.delete(0, tk.END)
      self.blocks_ch_e.insert(0, str(CurrentNB))

    else:
      print("ERROR, Could not access socket.")
      return

  def check_values_inj(self):
    packet1 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_SPB, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_INJ, packet1)

    print("COMMAND_VQWK is " + str(u.COMMAND_VQWK))
    print("cfSockCommand returned :  " + str(err_flag))
    
    if err_flag == u.SOCK_OK:
      CurrentSPB = int(reply1[3])
      self.samples_inj_e.delete(0, tk.END)
      self.samples_inj_e.insert(0, str(CurrentSPB))

    else:
      print("ERROR, Could not access socket.")
      return

    packet2 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_GD, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_INJ, packet2)
    
    if err_flag == u.SOCK_OK:
      CurrentGD = int(reply2[3])
      self.gate_inj_e.delete(0, tk.END)
      self.gate_inj_e.insert(0, str(CurrentGD))

    else:
      print("ERROR, Could not access socket.")
      return

    packet3 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_NB, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_INJ, packet3)
    
    if err_flag == u.SOCK_OK:
      CurrentNB = int(reply3[3])
      self.blocks_inj_e.delete(0, tk.END)
      self.blocks_inj_e.insert(0, str(CurrentNB))

    else:
      print("ERROR, Could not access socket.")
      return

  def check_values_lft_spec(self):
    packet1 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_SPB, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_LHRS, packet1)

    print("COMMAND_VQWK is " + str(u.COMMAND_VQWK))
    print("cfSockCommand returned :  " + str(err_flag))
    
    if err_flag == u.SOCK_OK:
      CurrentSPB = int(reply1[3])
      self.samples_lft_spec_e.delete(0, tk.END)
      self.samples_lft_spec_e.insert(0, str(CurrentSPB))

    else:
      print("ERROR, Could not access socket.")
      return

    packet2 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_GD, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_LHRS, packet2)
    
    if err_flag == u.SOCK_OK:
      CurrentGD = int(reply2[3])
      self.gate_lft_spec_e.delete(0, tk.END)
      self.gate_lft_spec_e.insert(0, str(CurrentGD))

    else:
      print("ERROR, Could not access socket.")
      return

    packet3 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_NB, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_LHRS, packet3)
    
    if err_flag == u.SOCK_OK:
      CurrentNB = int(reply3[3])
      self.blocks_lft_spec_e.delete(0, tk.END)
      self.blocks_lft_spec_e.insert(0, str(CurrentNB))

    else:
      print("ERROR, Could not access socket.")
      return

  def check_values_rt_spec(self):
    packet1 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_SPB, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply1 = u.send_command(u.Crate_RHRS, packet1)

    print("COMMAND_VQWK is " + str(u.COMMAND_VQWK))
    print("cfSockCommand returned :  " + str(err_flag))
    
    if err_flag == u.SOCK_OK:
      CurrentSPB = int(reply1[3])
      self.samples_rt_spec_e.delete(0, tk.END)
      self.samples_rt_spec_e.insert(0, str(CurrentSPB))

    else:
      print("ERROR, Could not access socket.")
      return

    packet2 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_GD, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply2 = u.send_command(u.Crate_RHRS, packet2)
    
    if err_flag == u.SOCK_OK:
      CurrentGD = int(reply2[3])
      self.gate_rt_spec_e.delete(0, tk.END)
      self.gate_rt_spec_e.insert(0, str(CurrentGD))

    else:
      print("ERROR, Could not access socket.")
      return

    packet3 = [u.COMMAND_VQWK, VQWK_GET_DATA, VQWK_NB, 0, 0, "VQWK Get Data", "Y"]
    err_flag, reply3 = u.send_command(u.Crate_RHRS, packet3)
    
    if err_flag == u.SOCK_OK:
      CurrentNB = int(reply3[3])
      self.blocks_rt_spec_e.delete(0, tk.END)
      self.blocks_rt_spec_e.insert(0, str(CurrentNB))

    else:
      print("ERROR, Could not access socket.")
      return

  def set_values_ch(self):

    value1 = int(self.samples_ch_e.get())
    value2 = int(self.gate_ch_e.get())
    value3 = int(self.blocks_ch_e.get())

    i = 0
    while (i <= 10):
      packet1 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_SPB, value1, i, "VQWK Set Data", "Y"]
      err_flag, reply1 = u.send_command(u.Crate_CH, packet1)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply1[1] != 1:
          if reply1[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply1[1]))
            othererror = 1
        else:
          if reply1[2] != VQWK_SPB:
            print("Server replied with wrong VQWK number: " +str(reply1[2])+ " instead of " +str(VQWK_SPB))
            othererror = 1
          if reply1[3] != value1:
            print("Server replied with wrong VQWK set value: " +str(reply1[3])+ " instead of " +str(value1))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1

    i = 0
    while (i <= 10):
      packet2 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_GD, value2, i, "VQWK Set Data", "Y"]
      err_flag, reply2 = u.send_command(u.Crate_CH, packet2)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply2[1] != 1:
          if reply2[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply2[1]))
            othererror = 1
        else:
          if reply2[2] != VQWK_GD:
            print("Server replied with wrong VQWK number: " +str(reply2[2])+ " instead of " +str(VQWK_GD))
            othererror = 1
          if reply2[3] != value2:
            print("Server replied with wrong VQWK set value: " +str(reply2[3])+ " instead of " +str(value2))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1
    
    i = 0
    while (i <= 10):
      packet3 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_NB, value3, 0, "VQWK Set Data", "Y"]
      err_flag, reply3 = u.send_command(u.Crate_CH, packet3)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply3[1] != 1:
          if reply3[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply3[1]))
            othererror = 1
        else:
          if reply3[2] != VQWK_NB:
            print("Server replied with wrong VQWK number: " +str(reply3[2])+ " instead of " +str(VQWK_NB))
            othererror = 1
          if reply3[3] != value3:
            print("Server replied with wrong VQWK set value: " +str(reply3[3])+ " instead of " +str(value3))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1
    
    self.check_values_ch()

  def set_values_inj(self):

    value1 = int(self.samples_inj_e.get())
    value2 = int(self.gate_inj_e.get())
    value3 = int(self.blocks_inj_e.get())

    i = 0
    while (i <= 10):
      packet1 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_SPB, value1, i, "VQWK Set Data", "Y"]
      err_flag, reply1 = u.send_command(u.Crate_INJ, packet1)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply1[1] != 1:
          if reply1[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply1[1]))
            othererror = 1
        else:
          if reply1[2] != VQWK_SPB:
            print("Server replied with wrong VQWK number: " +str(reply1[2])+ " instead of " +str(VQWK_SPB))
            othererror = 1
          if reply1[3] != value1:
            print("Server replied with wrong VQWK set value: " +str(reply1[3])+ " instead of " +str(value1))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1

    i = 0
    while (i <= 10):
      packet2 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_GD, value2, i, "VQWK Set Data", "Y"]
      err_flag, reply2 = u.send_command(u.Crate_INJ, packet2)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply2[1] != 1:
          if reply2[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply2[1]))
            othererror = 1
        else:
          if reply2[2] != VQWK_GD:
            print("Server replied with wrong VQWK number: " +str(reply2[2])+ " instead of " +str(VQWK_GD))
            othererror = 1
          if reply2[3] != value2:
            print("Server replied with wrong VQWK set value: " +str(reply2[3])+ " instead of " +str(value2))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1
    
    i = 0
    while (i <= 10):
      packet3 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_NB, value3, 0, "VQWK Set Data", "Y"]
      err_flag, reply3 = u.send_command(u.Crate_INJ, packet3)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply3[1] != 1:
          if reply3[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply3[1]))
            othererror = 1
        else:
          if reply3[2] != VQWK_NB:
            print("Server replied with wrong VQWK number: " +str(reply3[2])+ " instead of " +str(VQWK_NB))
            othererror = 1
          if reply3[3] != value3:
            print("Server replied with wrong VQWK set value: " +str(reply3[3])+ " instead of " +str(value3))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1
    
    self.check_values_inj()

  def set_values_lft_spec(self):

    value1 = int(self.samples_lft_spec_e.get())
    value2 = int(self.gate_lft_spec_e.get())
    value3 = int(self.blocks_lft_spec_e.get())

    i = 0
    while (i <= 10):
      packet1 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_SPB, value1, i, "VQWK Set Data", "Y"]
      err_flag, reply1 = u.send_command(u.Crate_LHRS, packet1)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply1[1] != 1:
          if reply1[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply1[1]))
            othererror = 1
        else:
          if reply1[2] != VQWK_SPB:
            print("Server replied with wrong VQWK number: " +str(reply1[2])+ " instead of " +str(VQWK_SPB))
            othererror = 1
          if reply1[3] != value1:
            print("Server replied with wrong VQWK set value: " +str(reply1[3])+ " instead of " +str(value1))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1

    i = 0
    while (i <= 10):
      packet2 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_GD, value2, i, "VQWK Set Data", "Y"]
      err_flag, reply2 = u.send_command(u.Crate_LHRS, packet2)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply2[1] != 1:
          if reply2[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply2[1]))
            othererror = 1
        else:
          if reply2[2] != VQWK_GD:
            print("Server replied with wrong VQWK number: " +str(reply2[2])+ " instead of " +str(VQWK_GD))
            othererror = 1
          if reply2[3] != value2:
            print("Server replied with wrong VQWK set value: " +str(reply2[3])+ " instead of " +str(value2))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1
    
    i = 0
    while (i <= 10):
      packet3 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_NB, value3, 0, "VQWK Set Data", "Y"]
      err_flag, reply3 = u.send_command(u.Crate_LHRS, packet3)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply3[1] != 1:
          if reply3[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply3[1]))
            othererror = 1
        else:
          if reply3[2] != VQWK_NB:
            print("Server replied with wrong VQWK number: " +str(reply3[2])+ " instead of " +str(VQWK_NB))
            othererror = 1
          if reply3[3] != value3:
            print("Server replied with wrong VQWK set value: " +str(reply3[3])+ " instead of " +str(value3))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1
    
    self.check_values_lft_spec()

  def set_values_rt_spec(self):

    value1 = int(self.samples_rt_spec_e.get())
    value2 = int(self.gate_rt_spec_e.get())
    value3 = int(self.blocks_rt_spec_e.get())

    i = 0
    while (i <= 10):
      packet1 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_SPB, value1, i, "VQWK Set Data", "Y"]
      err_flag, reply1 = u.send_command(u.Crate_RHRS, packet1)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply1[1] != 1:
          if reply1[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply1[1]))
            othererror = 1
        else:
          if reply1[2] != VQWK_SPB:
            print("Server replied with wrong VQWK number: " +str(reply1[2])+ " instead of " +str(VQWK_SPB))
            othererror = 1
          if reply1[3] != value1:
            print("Server replied with wrong VQWK set value: " +str(reply1[3])+ " instead of " +str(value1))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1

    i = 0
    while (i <= 10):
      packet2 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_GD, value2, i, "VQWK Set Data", "Y"]
      err_flag, reply2 = u.send_command(u.Crate_RHRS, packet2)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply2[1] != 1:
          if reply2[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply2[1]))
            othererror = 1
        else:
          if reply2[2] != VQWK_GD:
            print("Server replied with wrong VQWK number: " +str(reply2[2])+ " instead of " +str(VQWK_GD))
            othererror = 1
          if reply2[3] != value2:
            print("Server replied with wrong VQWK set value: " +str(reply2[3])+ " instead of " +str(value2))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1
    
    i = 0
    while (i <= 10):
      packet3 = [u.COMMAND_VQWK, VQWK_SET_DATA, VQWK_NB, value3, 0, "VQWK Set Data", "Y"]
      err_flag, reply3 = u.send_command(u.Crate_RHRS, packet3)
      
      othererror = 0
      if err_flag == u.SOCK_OK:
        if reply3[1] != 1:
          if reply3[1] == -2:
            print("Cannot set parameter, CODA run in progress!")
          else:
            print("Error:Server replied with VQWK error code: " + str(reply3[1]))
            othererror = 1
        else:
          if reply3[2] != VQWK_NB:
            print("Server replied with wrong VQWK number: " +str(reply3[2])+ " instead of " +str(VQWK_NB))
            othererror = 1
          if reply3[3] != value3:
            print("Server replied with wrong VQWK set value: " +str(reply3[3])+ " instead of " +str(value3))
            othererror = 1
      else:
        print(" check_status: ERROR, Could not access socket.")
      if othererror == 1:
        print("Unknown error, cannot set VQWK parameter")
      i += 1
    
    self.check_values_rt_spec()
