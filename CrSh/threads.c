#include "threads.h"
#include "sysInfo.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef WINOS
	#ifndef INFINITE
		#define INFINITE 0xFFFFFFFF
	#endif
	#include <synchapi.h>
	#include <processthreadsapi.h>
	#include <handleapi.h>
	#include <errhandlingapi.h>
#endif

#ifdef WINOS
	static void** CS_GetThreadHandle(uint32_t *threadID, uint32_t numberOfThreads)
	{
		//printf("get handle start\n");
		if(numberOfThreads <= 0)
		{
			//printf("return NULL if numberOfThreads is less than or equal to 0\n");
			return NULL;
		}
		void** handles = malloc(numberOfThreads*sizeof(void*));
		if(handles == NULL)
		{
			//printf("return NULL if handels == NULL\n");
			return NULL;
		}
		if(numberOfThreads == 1)
		{
			//printf("if there is a single thread\n");
			handles[0] = OpenThread(THREAD_ALL_ACCESS,0,threadID[0]);
		}
		else
		{
			//printf("else if there is more than 1 thread\n");
			for(uint8_t i = 0; i < numberOfThreads; i++)
			{
				//printf("looping to get handels\n");
				handles[i] = OpenThread(THREAD_ALL_ACCESS,0,threadID[i]);
			}
		}
		//printf("returning the handles\n");
		return handles;
	}
#endif

static uint8_t CS_isThreadActive(uint32_t threadID)
{
	
	void** handle = CS_GetThreadHandle(&threadID, 1);
	if(WaitForSingleObject(handle[0],0) == 0)
	{
		return 0;//thread is alive
	}
	else
	{
		return 1;//thread is finished
	}
}

void WaitForThread_CS_(uint32_t threadID)
{
	#ifdef WINOS
		void** handle = CS_GetThreadHandle(&threadID, 1);
		WaitForSingleObject(handle[0], INFINITE);
		free(handle);
	#endif
}
void WaitForThreads_CS_(uint32_t *threadID, uint32_t numberOfThreads)
{
	#ifdef WINOS
		void** handles = CS_GetThreadHandle(threadID, numberOfThreads);
		for(int i = 0; i < 5;i++)
		{
			WaitForSingleObject(handles[i], 0);
		}
		WaitForMultipleObjectsEx(numberOfThreads, handles, 1, INFINITE, 0);
		free(handles);
	#endif
}
uint32_t CreateThread_CS_(void* Start, void* Args)
{
	
	#ifdef WINOS	
		unsigned long int identifier;
		CreateThread(
			NULL, 
			0, 
			(unsigned long(*)(void*))Start, 
			Args, 
			0, 
			&identifier);
			
		return (uint32_t)identifier;
	#endif
	
	
}
uint8_t CleanupThread_CS_(uint32_t* threadID, uint32_t timeToWait)
{
	
	time_t curTime = time(NULL);
	uint32_t startTime = curTime;
	do
	{
		uint8_t isThreadAlive = CS_isThreadActive(threadID[0]);
		if(isThreadAlive == 0)
		{
			break;
		}
		if(timeToWait <= (curTime - startTime))
		{//if the loop has looped for an amount of seconds greater than or equal to timeToWait
			return 1;//return 1 to indicate the thread did not finish
		}
	}
	while(1);//while thread is currently running, loop
	
	#ifdef WINOS
		void** handle = CS_GetThreadHandle(threadID, 1);
		if(handle == NULL)
		{//if handle was unable to be malloced return 2 as we will be unable to close out the handle and need to keep the threadID around
			return 2;
		}
		GetExitCodeThread(handle[0], (unsigned long int*)threadID);//at this point the threadID value is replaced with the return value of the function
		CloseHandle(handle[0]);
		free(handle);
	#endif
	return 0;//we return 0 if the value in threadID at the end of this function is the value returned from the thread
}
uint16_t CleanupThreads_CS_(uint32_t* threadID, uint16_t numberOfThreads, uint8_t* completedThreads, uint32_t timeToWait)
{
	/*completedThreads is an array of char variables that represent if any given thread has completed and returned its value.
	EX:
	
	threadID = 			[5, 2212, 7, 8, 2215, 10]
	completedThreads =	[0,  1  , 0, 0,   1 , 0 ]
	
	in the example, the nth element of any value in the completedThreads array, represents whether or not that same position in
	the threadID array has been closed succsessfuly, and you will be able to get its return value from the position that used to hold
	its threadID. Any completedThreads value of 0 indicates a completed thread and any non zero value represents a an uncompleted thread.
	The threadID of that thread will remain in that slot untouched.
	
	if the return value of this function is 0, all threads have been completed successfully and there will be no need to check for which
	threads aren't closed. If the return value is anything but 0, the value returned indicates how many threads failed to close,
	meaning the above example would return a value of "2" to represent that two threads failed to close. When checking an array of threadID's
	for which threads didn't close, you don't need to check every threadID, just up until you find some number of threadID's not closed
	equal to the return value
	
	completedThreads is expected to be malloced by the caller before pass in and also expected to be freed by the caller. completedThreads
	also expects the malloced size to allow for an equal number of elements to the number of threadID's passed in
	*/
	uint16_t threadsStillActive = 0;
	time_t startTime = time(NULL);
	for(uint16_t curThread = 0;curThread < numberOfThreads;curThread++)
	{//loop through each thread and call CleanupThread_CS_ on each individual thread
		time_t curTime = time(NULL);
		uint32_t timeToWaitLeft = timeToWait - (curTime - startTime);
		if(CleanupThread_CS_(&threadID[curThread], timeToWaitLeft) == 0)
		{//if thread was successfully cleaned up
			completedThreads[curThread] = 0;//indicate this thread returned properly
		}
		else
		{//else thread was not successfully cleaned up
			completedThreads[curThread] = 1;//indicate this thread was not returned properly
			threadsStillActive++;
		}
	}
	return threadsStillActive;
}

