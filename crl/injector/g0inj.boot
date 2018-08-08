#
# ROC31  g0inj	
# CPU: Motorola MVME6100-0163 - MPC 7457.  Processor #0
# 129.57.206.93
# 
# contact : Jeong Han Lee
#         : jhlee@jlab.org 

# date    : Friday, May 20 11:54:03 EDT 2011
# last modified : 

#
# old boot parameter for qweakl3 and CODA 2.5
#
# boot device          : geisc
# unit number          : 0 
# processor number     : 0 
# host name            : qweakl3.jlab.org
# file name            : /usr/local/coda/kern/5.5/vx6100_v4.1big
# inet on ethernet (e) : 129.57.206.93:fffffc00
# host inet (h)        : 129.57.168.63
# gateway inet (g)     : 129.57.204.1
# user (u)             : gzero
# flags (f)            : 0x20 
# target name (tn)     : g0inj
# startup script (s)   : /home/gzero/qweak/coda25/boot/g0inj_6100.boot

#
# current parameter for cdaql4 and CODA 2.6.1
#
# boot device          : geisc
# unit number          : 0 
# processor number     : 0 
# host name            : cdaql4
# file name            : /usr/local/coda/kern/5.5/vx6100_v4.1big
# inet on ethernet (e) : 129.57.206.93:fffffc00
# host inet (h)        : 129.57.168.44
# gateway inet (g)     : 129.57.204.1
# user (u)             : cdaq
# flags (f)            : 0x20 
# target name (tn)     : g0inj
# startup script (s)   : /home/cdaq/qweak/coda26/boot/g0inj.boot


shellPromptSet("qweak g0inj -> ");


< /home/cdaq/qweak/coda26/boot/g0inj_config.boot
< /home/cdaq/qweak/coda26/boot/g0inj_coda261_ppc.boot
< /home/cdaq/qweak/coda26/boot/g0inj_MVME6100.universeDma.boot
< /home/cdaq/qweak/coda26/boot/roc_261.boot



#  load bmw routines
#
ld < /home/cdaq/qweak/libraries/cfSocket/bmw/bmwClient.o
ld < /home/cdaq/qweak/libraries/cfSocket/bmw/bmw_config.o

#
#  load scan routines
#
ld < /home/cdaq/qweak/libraries/cfSocket/scan/SCAN_util.o
ld < /home/cdaq/qweak/libraries/cfSocket/scan/SCAN_config.o

#
#  load cfSock server
#
ld < /home/cdaq/qweak/libraries/cfSocket/cfSock/cfSockSer.o
#
#  to start, use command:
#sp cfSockSer
taskSpawn("CFSOCKSER", 120, 8, 20000, cfSockSer)
#
###  Download Scaler Libraries
ld < /home/cdaq/qweak/libraries/vme/sis3801IntLib/scalIntLib_longcall.o

#    Load the library for the STR7200
ld < /home/cdaq/qweak/libraries/vme/STR7200/STR7200_longcall.o

###  Download VQWK libraries
ld < /home/cdaq/qweak/libraries/vme/vqwk/vqwk_32_longcall.o

#    Load the library for the FlexIO
ld < /home/cdaq/qweak/libraries/vme/flexio/flexioLib_longcall.o


taskSpawn("ROC",200,8,300000,coda_roc,"-s","cdaq","-objects","ROC31 ROC")
# Booting successful :-)
