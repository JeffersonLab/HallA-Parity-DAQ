# Bootfile for MVME 6100 loading the Tempe DMA library
# 


# Load Tempe DMA Library
ld< /site/coda/2.6.2/extensions/tempeDma/usrTempeDma.o
# Setup Address and data modes for transfers
#
#  usrVmeDmaConfig(addrType, dataType, sstMode);
#
#  addrType = 0 (A16)    1 (A24)    2 (A32)
#  dataType = 0 (D16)    1 (D32)    2 (BLK32) 3 (MBLK) 4 (2eVME) 5 (2eSST)
#  sstMode  = 0 (SST160) 1 (SST267) 2 (SST320)
#
usrVmeDmaConfig(2,2,0)

# Printout Tempe DMA engine status
sysVmeDmaShow()

