'''
Green Monster GUI Revamp
Containing ADC18s Tab
Code Commissioned 2019-01-16
Code by A.J. Zec
'''

import tkinter as tk
from tkinter import ttk
import utils as u

ADC18_GET_NUMADC	=1001
ADC18_GET_LABEL		=1002
ADC18_GET_CSR		=1003
ADC18_SET_CONV		=1004
ADC18_SET_INT		=1005
ADC18_SET_PED		=1006
ADC18_GET_CONV		=1007
ADC18_GET_INT		=1008
ADC18_GET_PED		=1009
ADC18_SET_DAC		=1010
ADC18_GET_DAC		=1011
ADC18_SET_SAMP		=1012
ADC18_GET_SAMP		=1013
GA_MAXADC		=20
DACRADIO18		=100
GM_ADC_GET		=101
GM_ADC_SET		=201
DACTRI			=0
DACSAW			=1
DACCONST		=2
DACOFF18		=3

class ADC18(tk.Frame):
  def __init__(self, tab):
    global numADC
    numADC = self.get_num_adc()
    ADClabels = []

    i = 0
    while i < numADC:
      ADClabels.append(self.get_label_adc(i))
      i += 1

    self.rt_spec_frame = tk.LabelFrame(tab, text='RHRS', background=u.green_color)
    self.adc_ls = []
    self.int_es = []
    self.conv_es = []
    self.dac_settings = []
    self.sample_settings = []

    i = 0
    while i < numADC:
      self.adc_ls.append(tk.Label(self.rt_spec_frame, text='ADC '+str(ADClabels[i]), background=u.green_color))
      self.int_es.append(tk.Entry(self.rt_spec_frame, width=3))
      self.conv_es.append(tk.Entry(self.rt_spec_frame, width=3))
      self.dac_settings.append(tk.StringVar())
      self.sample_settings.append(tk.IntVar())
      i += 1
      
    labels = ['Label', 'Int', 'Conv', '-----', 'DAC', 'Settings', '-----', 'Sample by:']
    for i, label in enumerate(labels):
      tk.Label(self.rt_spec_frame, text=label, background=u.green_color).grid(
          row=0, column=i, padx=8, pady=10, sticky='W')
    
    self.create_table(numADC)
    self.check_values()

  def get_num_adc(self):
    packet = [u.COMMAND_ADC18, ADC18_GET_NUMADC, 0, 0, 0, "ADC Get Number", "Y"]
    err_flag, reply = u.send_command(u.Crate_RHRS, packet)

    if err_flag == u.SOCK_OK:
      return int(reply[3])

    else:
      print("ERROR, Could not access socket.")
      return -1

  def get_label_adc(self, index):
    packet = [u.COMMAND_ADC18, ADC18_GET_LABEL, index, 0, 0, "ADC Get Label", "Y"]
    err_flag, reply = u.send_command(u.Crate_RHRS, packet)
    
    if err_flag == u.SOCK_OK:
      return int(reply[3])

    else:
      print("ERROR, Could not access socket.")
      return -1
    
    
  def create_table(self, value):
    for i in range(1, value+1):
      self.adc_ls[i-1].grid(row=i, column=0, padx=10, pady=10, sticky='W')
      u.set_text(self.int_es[i-1], '3').grid(row=i, column=1, padx=10, pady=10)
      u.set_text(self.conv_es[i-1], '0').grid(row=i, column=2, padx=10, pady=10)
      setting = self.dac_settings[i-1]
      settings = ['Tri', 'Saw', 'Const', 'Off']
      setting.set('Tri')
      for j,s in enumerate(settings):
        tk.Radiobutton(self.rt_spec_frame, text=s, variable=setting, value=s, background=u.green_color).grid(
          row=i, column=j+3, padx=5, pady=10, sticky='W')
      sample_by = self.sample_settings[i-1]
      sample_by.set(1)
      tk.OptionMenu(self.rt_spec_frame, sample_by, 1, 2, 4, 8).grid(row=i, column=7)
    tk.Button(self.rt_spec_frame, text='Get Settings', background=u.green_color, command=self.check_values).grid(
        row=6, column=1, columnspan=2, pady=50, sticky='S')
    tk.Button(self.rt_spec_frame, text='Apply Settings', background=u.green_color, command=self.set_values).grid(
        row=6, column=3, columnspan=2, pady=50, sticky='S')
    tk.Button(self.rt_spec_frame, text='Cancel', background=u.green_color, command=self.check_values).grid(
        row=6, column=5, pady=50, sticky='S')
    self.rt_spec_frame.pack(padx=20, pady=20)

  def check_values(self):
    fSample = []
    fIntGain = []
    fConvGain = []
    fDAC = []
    value = numADC

    i = 0
    while i < value:
      packet = [u.COMMAND_ADC18, ADC18_GET_SAMP, i, 0, 0, "ADC18 Get Sample", "Y"]
      err_flag, reply = u.send_command(u.Crate_RHRS, packet)
    
      if err_flag == u.SOCK_OK:
        fSample.append(reply[3])
        self.sample_settings[i].set(reply[3])

      else:
        print("ERROR, Could not access socket.")
        return -1

      packet = [u.COMMAND_ADC18, ADC18_GET_INT, i, 0, 0, "ADC18 Get Int", "Y"]
      err_flag, reply = u.send_command(u.Crate_RHRS, packet)
    
      if err_flag == u.SOCK_OK:
        fIntGain.append(reply[3])
        self.int_es[i].delete(0, tk.END)
        self.int_es[i].insert(0, str(reply[3]))

      else:
        print("ERROR, Could not access socket.")
        return -1

      packet = [u.COMMAND_ADC18, ADC18_GET_CONV, i, 0, 0, "ADC18 Get Conv", "Y"]
      err_flag, reply = u.send_command(u.Crate_RHRS, packet)
    
      if err_flag == u.SOCK_OK:
        fConvGain.append(reply[3])
        self.conv_es[i].delete(0, tk.END)
        self.conv_es[i].insert(0, str(reply[3]))

      else:
        print("ERROR, Could not access socket.")
        return -1

      packet = [u.COMMAND_ADC18, ADC18_GET_DAC, i, 0, 0, "ADC18 Get DAC", "Y"]
      err_flag, reply = u.send_command(u.Crate_RHRS, packet)
    
      if err_flag == u.SOCK_OK:
        fDAC.append(reply[3])
        if reply[3] == DACSAW:
          self.dac_settings[i].set('Saw')
        elif reply[3] == DACCONST:
          self.dac_settings[i].set('Const')
        elif reply[3] == DACTRI:
          self.dac_settings[i].set('Tri')
        else:
          self.dac_settings[i].set('Off')

      else:
        print("ERROR, Could not access socket.")
        return -1
      i += 1

  def set_values(self):
    fSample = []
    fIntGain = []
    fConvGain = []
    fDAC = []
    value = numADC

    i = 0
    while i < value:
      fIntGain.append(int(self.int_es[i].get()))

      if fIntGain[i] < 0 or fIntGain[i] > 3:
        print("ERROR: Int Value is out of range! Try (0-3)...")
      else:
        packet = [u.COMMAND_ADC18, ADC18_SET_INT, i, fIntGain[i], 0, "ADC18 Set Int", "Y"]
        err_flag, reply = u.send_command(u.Crate_RHRS, packet)
      
        if err_flag == u.SOCK_OK:
          pass
        else:
          print("ERROR, Could not access socket.")
          return -1

      fConvGain.append(int(self.conv_es[i].get()))

      if fConvGain[i] < 0 or fConvGain[i] > 15:
        print("ERROR: Conv Value is out of range! Try (0-15)...")
      else:
        packet = [u.COMMAND_ADC18, ADC18_SET_CONV, i, fConvGain[i], 0, "ADC18 Set Conv", "Y"]
        err_flag, reply = u.send_command(u.Crate_RHRS, packet)
       
        if err_flag == u.SOCK_OK:
          pass
        else:
          print("ERROR, Could not access socket.")
          return -1
      
      fDAC.append(self.dac_settings[i].get())

      if fDAC[i]=='Tri':
        dacflag = DACTRI
      elif fDAC[i]=='Saw':
        dacflag = DACSAW
      elif fDAC[i] == 'Const':
        dacflag = DACCONST
      else:
        dacflag = DACOFF18

      packet = [u.COMMAND_ADC18, ADC18_SET_DAC, i, dacflag, 0, "ADC18 Set DAC", "Y"]
      err_flag, reply = u.send_command(u.Crate_RHRS, packet)
       
      if err_flag == u.SOCK_OK:
        pass
      else:
        print("ERROR, Could not access socket.")
        return -1

      fSample.append(int(self.sample_settings[i].get()))

      packet = [u.COMMAND_ADC18, ADC18_SET_SAMP, i, fSample[i], 0, "ADC18 Set Sample", "Y"]
      err_flag, reply = u.send_command(u.Crate_RHRS, packet)
      
      if err_flag == u.SOCK_OK:
        pass
      else:
        print("ERROR, Could not access socket.")
        return -1
      i += 1

    self.check_values()
