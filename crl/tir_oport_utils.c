#ifndef _TIR_OPORT_INCLUDED
#define _TIR_OPORT_INCLUDED
/* tir_oport.c
 *
 * Utilities to allow easy control over the Trigger Interface
 * output port.
 * Before these are used, the TIR must be initialized (usually done
 * at the download event).
 *
 *     int clear_tir_oport()       - Turns OFF all bits.
 *                                   Returns the final output port 
 *                                   state.
 *
 *     int read_tir_oport()        - Reads the output port and sets
 *                                   the internal variable to match
 *                                   the current state.
 *                                   Returns the final output port 
 *                                   state.
 *
 *     int set_tir_oport(int mask) - Turns ON the bits in the
 *                                   mask pattern which are on.
 *                                   Other bits are unchanged.
 *                                   Returns the final output port 
 *                                   state.
 *
 *     int unset_tir_oport(int mask) - Turns OFF the bits in the
 *                                   mask pattern which are on.
 *                                   Other bits are unchanged.
 *                                   Returns the final output port 
 *                                   state.
 *
 *     int toggle_tir_oport(int mask) - Toggles the bits in the
 *                                   mask pattern which are on.
 *                                   Other bits are unchanged.
 *                                   Returns the final output port 
 *                                   state.
 *
 * ========================================
 * P.King, 2009jul12:  Initial version.
 *
 */

int tir_oport_state;   /* output port state (global variable) */

int clear_tir_oport(){
  tir_oport_state = 0;
  tir[1]->tir_oport = tir_oport_state;
  /*  logMsg("set_tir_oport: mask=%x, final=%x", mask, tir_oport_state); */
  return tir_oport_state;
};

int read_tir_oport(){
  tir_oport_state = (tir[1]->tir_oport) & 0xFF;
  /*  logMsg("set_tir_oport: mask=%x, final=%x", mask, tir_oport_state); */
  return tir_oport_state;
};

int set_tir_oport(int mask){
  tir_oport_state |= mask;
  tir[1]->tir_oport = tir_oport_state;
  /*  logMsg("set_tir_oport: mask=%x, final=%x", mask, tir_oport_state); */
  return tir_oport_state;
};

int unset_tir_oport(int mask){
  tir_oport_state |= mask;
  tir_oport_state ^= mask;  
  tir[1]->tir_oport = tir_oport_state;
  /*  logMsg("unset_tir_oport: mask=%x, final=%x", mask, tir_oport_state); */
  return tir_oport_state;
};

int toggle_tir_oport(int mask){
  tir_oport_state ^= mask;  
  tir[1]->tir_oport = tir_oport_state;
  /*  logMsg("unset_tir_oport: mask=%x, final=%x", mask, tir_oport_state); */
  return tir_oport_state;
};

#endif /* _TIR_OPORT_INCLUDED */
