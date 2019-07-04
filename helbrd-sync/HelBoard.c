/************************************************************************
 *  HelBoard.c                                                          *
 *   subroutines which are used to interface with the                   *
 *   Helicity Board built by Roger Flood                                *
 *                                                                      *
 *   Feb 13, 2013       R. Michaels                                     *
 *                                                                      *
 *                                                                      *
 ************************************************************************/

#define OK        0

#define TSETTLE   0
#define TSTABLE   1
#define DELAY     2
#define PATTERN   3

#include <stdio.h>
#include <math.h>
#include <string.h>


int debug=1;


/* Base address of board */
long base_address = 0xa00000;  
/* note: to test with scaler, need 0xab2000 and Write32, Read32, etc*/

/* register masks */
#define TSETTLE_MASK		0x1f	/* 5 bit register */
#define TSTABLE_MASK		0x1f	/* 5 bit register */
#define DELAY_MASK		0x0f	/* 4 bit register */
#define PATTERN_MASK		0x07	/* 3 bit register */
#define CLOCK_REG_MASK  0xf   /* offset 15 4 bit register */

static unsigned int iRegMasks[5] = 
{
	TSETTLE_MASK,
	TSTABLE_MASK,
	DELAY_MASK,
	PATTERN_MASK,
  CLOCK_REG_MASK
};

typedef struct HELBOARD {
     char  skip[6];     /* 00 - 06     */
     short registers[5];   /* tsettle, tstable, delay, pattern, clock */
  } HBST;


static struct HELBOARD *hBd;


/***************************************************************************************
 *  Write a a quantity from the helicity board
 * 
 *      arguments      : 
 *           1. quantity to write
 *
 *                 0 = tsettle,  1 = tstable,  2 = delay,  3 = pattern,  4 = clock
 * 
 *           2. value to write
 *                              
 */


int WriteHelBoard(long quantity, long value)
{
  int res;
  long laddr;
  long vwrite;

  if (quantity<0 || quantity>4) {
    printf("WriteHelBoard::ERROR: Invalid quantity\n");
    return ERROR;
  }

  res = sysBusToLocalAdrs(0x39, base_address, &laddr);
  printf("sysBusToLocalAdrs result = %d \n",res);
  if (res != 0) {
       printf("-------- HelBoard: ERROR:  sysBusToLocalAdrs address %x\n",laddr);
       return ERROR;  
  } else {
    if (debug) printf("--------- Helicity Board address %x\n", laddr);
  }  

  hBd = (struct HELBOARD *)laddr;
  vwrite=(value&iRegMasks[quantity]);
  if (debug) printf("Qty %d  Value to write = %d = 0x%x \n",quantity,vwrite,vwrite);
  printf("stuff 0x%x \n",hBd->registers[quantity]);
  hBd->registers[quantity] = vwrite;
  printf("all done \n");
  return OK;
 }


/***************************************************************************************
 *  Read the "quantity" from the helicity board.
 *
 *      argument:  quantity to write
 *
 *         0 = tsettle,  1 = tstable,  2 = delay,  3 = pattern,  4 = clock
 */
int ReadHelBoard(long quantity)
{
  long board_register, laddr;
  int res;

  if (quantity<0 || quantity>4) {
    printf("ReadHelBoard::ERROR: Invalid quantity\n");
    return ERROR;
  }

  res = sysBusToLocalAdrs(0x39, base_address, &laddr);
  if (res != 0) {
       printf("-------- HelBoard: ERROR:  sysBusToLocalAdrs address %x\n",laddr);
       return ERROR;  
  } else {
      if(debug) printf("--------- Helicity Board address %x\n", laddr);
  }  

  hBd = (struct HELBOARD *) laddr;
  board_register = hBd->registers[quantity];
  if (debug) printf("+++++++++++ Helicity Board register = 0x%x  ++++++++++\n",board_register);

  return board_register;
 }

