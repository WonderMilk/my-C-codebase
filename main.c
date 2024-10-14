#include "CrSh\threads.h"
#include "CrSh\sysInfo.h"
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

struct myData
{
	int32_t d1;
	int16_t d2;
	int8_t d3;
};

void* test(void* args)
{
	
	//printf("testaaa\n");
	struct myData *argDat = args;

	printf("%d\n", argDat->d1);
	Sleep(1000);
	printf("%d\n", argDat->d2);
	printf("%d\n", argDat->d3);
	free(argDat);
	return (void*)300;
}

int32_t main()
{
	uint8_t returnValOne, returnValTwo;
	uint32_t threadID[5];
	for(int8_t i = 0;i < 5;i++)
	{
		struct myData *temp;
		temp = malloc(sizeof(struct myData));
		temp->d1 = i;
		temp->d2 = i+1;
		temp->d3 = i+2;
		
		threadID[i] = CreateThread_CS_(&test,temp);
	}
	for(uint8_t IDslot = 0;IDslot != 5;IDslot++)
	{
		printf("thread id is %d\n", threadID[IDslot]);
	}
	uint8_t completedThreads[5];
	returnValOne = CleanupThreads_CS_(&threadID[0],5,&completedThreads[0],0);
	printf("return val before waiting for threads to finish %d\n", returnValOne);
	printf("%d %d %d %d %d\n",threadID[0],threadID[1],threadID[2],threadID[3],threadID[4]);
	printf("%d %d %d %d %d\n",completedThreads[0],completedThreads[1],completedThreads[2],completedThreads[3],completedThreads[4]);
	WaitForThreads_CS_(&threadID[0], 5);
	Sleep(5000);
	returnValTwo = CleanupThreads_CS_(&threadID[0],5,&completedThreads[0],0);
	printf("return val after waiting for threads to finish %d\n",returnValTwo);
	printf("%d %d %d %d %d\n",threadID[0],threadID[1],threadID[2],threadID[3],threadID[4]);
	printf("%d %d %d %d %d\n",completedThreads[0],completedThreads[1],completedThreads[2],completedThreads[3],completedThreads[4]);
	printf("os is %d\n", getOSInfo());
	return 0;
}