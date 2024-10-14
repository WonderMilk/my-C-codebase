#include "sysInfo.h"
#include <stdint.h>

static int8_t OSInfo = 0x80;
/*OSInfo
OSInfo is used to determine the type of operating system
and how many bits said operating system is.

the fitst two bits determine the OS.
0000 0001 = Windows
0000 0010 = Mac
0000 0011 = Linux
0000 0000 = other 

the third bit determines how many bits the OS is
0000 0000 = 32 bit
0000 0100 = 64 bit

if the MSB is a 1, then OSInfo is negative and that signifies
that this value has not been set yet
*/
#ifdef WINOS_64
	static const int8_t OSBaseInfo = 0x05;
#elifdef WINOS_32
	static const int8_t OSBaseInfo = 0x01;
#elifdef MAC
	static const int8_t OSBaseInfo = 0x06;
#elifdef LINUX
	static const int8_t OSBaseInfo = 0x07;
#endif

static int8_t setOSInfo()
{
	/*
	setOSInfo is a function that will set OSInfo to the value of OSBaseInfo if OSInfo is negative (not set)
	returns early if OSInfo is greater than 0 because that should mean OSInfo has already been set
	*/
	
	if(OSInfo > 0)
	{
		return OSInfo;
	}
	OSInfo = OSInfo & 0x00;
	OSInfo = OSInfo | OSBaseInfo;
	return 0;
}

int8_t getOSInfo()
{
	if(OSInfo < 0)
	{
		setOSInfo();
	}
	return OSInfo;
}
