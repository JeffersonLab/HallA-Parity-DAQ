///////////////////////////////////////////////
//  vqwk_config.c                           //
//  interface between configuration socket   //
//  server and the vqwk adcs       //
//  cap, oct 2015                            //
///////////////////////////////////////////////

#include "VQWK_cf_commands.h"

void taskVQWK_CF(long* command, long *par1, long *par2, long *par3)
{
  int junk;
  int index;
  switch (*command)
    {  
    case VQWK_GET_DATA:
      switch (*par1)
	{
        case VQWK_GD:
          //later want to change
	  //int vqwkDumpStatus(UINT16 index, UINT32 *data)
	  // vqwk_get_u8(ldata2,3)
	  *par2 = vqwkGetGateDelay(0);
	  break;
	case VQWK_SPB:
	  //later want to change
	  //index=2;
	  //int vqwkDumpStatus(UINT16 index, UINT32 *data)
	  //vqwk_get_u16(ldata1,0)
	  *par2 = vqwkGetSamplesPerBlock(0);
	  break;
	case VQWK_NB:
	  //later want to change
      	  *par2 = vqwkGetNumberOfBlocks(0); 
	  break;

	}
      break;
    case VQWK_SET_DATA:
      switch (*par1)
	{
        case VQWK_GD:
          //index=0;//later want something like index=vqwkGetIndex()
	  *command = vqwkSetGateDelay(*par3,*par2);
	  break;
        case VQWK_SPB:
          //index=0;//later want something like index=vqwkGetIndex()
	  *command = vqwkSetSamplesPerBlock(*par3,*par2);
	  break;
        case VQWK_NB:
          //index=0;//later want something like index=vqwkGetIndex()
	  *command = vqwkSetNumberOfBlocks(*par3,*par2);
	  break;

	}
      break;
    }
}






