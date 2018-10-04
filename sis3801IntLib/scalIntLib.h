/******************************************************************************
*
*  scalIntLib.h  -  Driver library header file for readout of a Struck 7201 
*                   Multi-scaler or SIS 3801 Multi-scaler using a VxWorks 5.2 or
*                   later based single board computer.
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          January 1999
*
*  Modifications:
*  2000oct19  pmk  Corrected the CSR bit patterns for bits 16-23.
*/

/* Define a Structure for access to  Scaler */
struct fifo_scaler {
  volatile unsigned long csr;
  volatile unsigned long irq;
  volatile unsigned long blank1;
  volatile unsigned long cdr;
  volatile unsigned long wfifo;
  volatile unsigned long blank2[3];
  volatile unsigned long clear;
  volatile unsigned long next;
  volatile unsigned long enable;
  volatile unsigned long disable;
  volatile unsigned long Bclear;
  volatile unsigned long Bnext;
  volatile unsigned long Benable;
  volatile unsigned long Bdisable;
  volatile unsigned long blank3[8];
  volatile unsigned long reset;
  volatile unsigned long blank4;
  volatile unsigned long test;
  volatile unsigned long blank5[37];
  volatile unsigned long fifo[64];
};

/* Define default interrupt vector/level */
#define SCAL_INT_VEC      0xda
#define SCAL_VME_INT_LEVEL   5

/* Define CSR Register Bits */
#define SCAL_CSR_LED                         0x1
#define SCAL_CSR_FIFO_TEST                   0x2
#define SCAL_CSR_INPUT_BIT0                  0x4
#define SCAL_CSR_INPUT_BIT1                  0x8
#define SCAL_CSR_25MHZ_TEST                 0x10
#define SCAL_CSR_INPUT_TEST                 0x20
#define SCAL_CSR_BROADCAST_MODE             0x40
#define SCAL_CSR_HANDSHAKE_MODE             0x80
/* The bits from 0x100 through 0x8000 disable
   the corresponding bits above.              */
#define SCAL_CSR_EXT_NEXT                0x10000
#define SCAL_CSR_EXT_CLEAR               0x20000
#define SCAL_CSR_EXT_DISABLE             0x40000
#define SCAL_CSR_SOFT_DISABLE            0x80000
#define SCAL_CSR_IRQ_CIP                0x100000
#define SCAL_CSR_IRQ_FIFO_ALMOST_EMPTY  0x200000
#define SCAL_CSR_IRQ_FIFO_HALF_FULL     0x400000
#define SCAL_CSR_IRQ_FIFO_FULL          0x800000
/* The bits from 0x1000000 through 0x80000000
   disable the corresponding bits above.      */

/* Define CSR Status bits */
#define SCAL_STATUS_TEST_MODE              0x20
#define SCAL_STATUS_FIFO_EMPTY            0x100
#define SCAL_STATUS_FIFO_ALMOST_EMPTY     0x200
#define SCAL_STATUS_FIFO_HALF_FULL        0x400
#define SCAL_STATUS_FIFO_ALMOST_FULL      0x800
#define SCAL_STATUS_FIFO_FULL            0x1000
#define SCAL_STATUS_ENABLE_NEXT          0x8000


#define SCAL_CLEAR(a)       (a<<8)

/* Define Masks */
#define SCAL_INPUT_MODE_MASK          0xc
#define SCAL_FIFO_STATUS_MASK      0x1f00
#define SCAL_VERSION_MASK          0xf000
#define SCAL_ENABLE_IRQ_MASK     0xf00000
#define SCAL_DISABLE_IRQ_MASK  0xf0000000
#define SCAL_INT_VEC_MASK            0xff
#define SCAL_INT_LEVEL_MASK         0x700
#define SCAL_INT_ENABLED_MASK       0x800
#define SCAL_MODULE_ID_MASK    0xffff0000

