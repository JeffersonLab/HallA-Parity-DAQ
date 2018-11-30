#include "../cfSock/GreenSock.h"

extern "C" int cfSockCommand(int crate_number,
			     int command_type, int command, 
			     int req_param, int req_param_2,
			     int req_param3,
			     char *reply,   char *msg);

extern "C" int GreenSockCommand(int crate_number,
				struct greenRequest *gRequest);
