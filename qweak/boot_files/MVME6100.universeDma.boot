# Load Tempe DMA Library for MVME 6100
# This is for the qwvme1 VME
#

ld< /adaqfs/home/apar/devices/qweak/boot_files/usrTempeDma.o

# Setup Address and data modes for transfers
#
#  usrVmeDmaConfig(addrType, dataType, sstMode);
#
#  addrType = 0 (A16)    1 (A24)    2 (A32)
#  dataType = 0 (D16)    1 (D32)    2 (BLK32) 3 (MBLK) 4 (2eVME) 5 (2eSST)
#  sstMode  = 0 (SST160) 1 (SST267) 2 (SST320)
#
#  * if dataType not_eqaul_to 5, third arg has no meaning.
#    see usrTempeDma.c line 280.
#  * usrVmeDmaInit() function returs OK, so for 6100, we don't
#    need to execute usrVmeDmaInit()
#    Thursday, June  9 11:51:44 EDT 2011, jhlee

usrVmeDmaConfig(2,2,0)
sysVmeDmaShow()
