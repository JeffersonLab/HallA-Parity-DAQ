/******************************************************************************
 *
 *  flexioLib.h   -  Driver library header file for readout of a TJNAF
 *                   Flexible Input/Output Module using a VxWorks 5.2
 *                   or later based single board computer.
 *
 *  Author: Paul King
 *          University of Maryland / TJNAF G0 Collaboration
 *          March 2001
 *
 *  Revision 1.0 - Initial revision.
 */

/* Define a Structure for access to the FlexIO module */
typedef struct flexio_struct {
  /*  register name         offset         description            */
  unsigned short csr1      ; /* 0        - Port 1 CSR             */
  unsigned short data1     ; /* 2        - Port 1 Data register   */
  unsigned short csr2      ; /* 4        - Port 2 CSR             */
  unsigned short data2     ; /* 4        - Port 2 Data register   */
  unsigned short interrupt ; /* 6        - VME Interrupt register */
} FlexIOReg;

/* Define default interrupt register value. */
#define FLEXIO_INT_REG      0x66

/* Define CSR masks. */
#define FLEXIO_CSR_IN_STROBE       0x0001
#define FLEXIO_CSR_IN_LATCH        0x0002
#define FLEXIO_CSR_IN_EN_INT       0x0004
#define FLEXIO_CSR_IN_DATA         0x0010
#define FLEXIO_CSR_IN_INT_REQ      0x0020
#define FLEXIO_CSR_IN_ACT_STR      0x0100

#define FLEXIO_CSR_OUT_MODE_MASK   0x0003
#define FLEXIO_CSR_OUT_EXT_STR     0x0004
#define FLEXIO_CSR_OUT_PULSE       0x0100


#define FLEXIO_CSR_ID_MASK         0x00c0
#define FLEXIO_CSR_ID_OFFSET       6

#define FLEXIO_CSR_RESET           0x8000
#define FLEXIO_CSR_READONLY        0x80f0


#define FLEXIO_ID_OUTPUT       1
#define FLEXIO_ID_INPUT        2
#define FLEXIO_ID_EMPTY        3



/* Function Prototypes */
STATUS flexioInit (UINT32 addr);
void flexioReset();
void flexioPrintStatus();
void flexioPrintID();

STATUS flexioCheckID(int portnum, int porttype);
int flexioReadData(int portnum);
int flexioWriteData(int portnum, int writedata);

int flexioWriteMaskCard(int portnum, int value, int bitpattern);
int flexioSetDataMask(int portnum, int bitpattern);
int flexioUnsetDataMask(int portnum, int bitpattern);
int flexioToggleData(int portnum, int bitpattern);

int flexioWriteChan(int value, int bitposition);

STATUS flexioSetOutputMode(int portnum, int outputmode);
STATUS flexioEnableInputLatch(int portnum);
STATUS flexioIsExtLatch(int portnum);
STATUS flexioCheckCSRMask(int portnum, int csrmask);
STATUS flexioSetCSRMask(int portnum, int csrmask);

/******************************************************************************
 */
