#include <stdint.h>

#ifdef _WIN64

	#ifndef WINOS
		#define WINOS 
	#endif
	
	#ifndef WINOS_64
		#define WINOS_64
	#endif
	
#elif _WIN32

	#ifndef WINOS
		#define WINOS
	#endif
	
	#ifndef WINOS_32
		#define WINOS_32
	#endif
	
#elif __APPLE__ || __MARCH__

	#ifndef MAC
		#define MAC
	#endif
	
#elif __linux__
	#ifndef LINUX
		#define LINUX
	#endif
#endif

int8_t getOSInfo();