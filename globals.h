//  Globals, to be shared among all device classes.
//

// The first, and so far the only, variable that is defined in this file
//  is the flag for a coda run in progress.  This is necessary in order
//  to lock out configuration changes while a run is in progress.
int CODA_RUN_IN_PROGRESS;
