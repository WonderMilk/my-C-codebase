#include <stdint.h>

#define THREAD_LIVE 0x0FFFFFFF //268,435,455

uint8_t CleanupThread_CS_(uint32_t* threadID, uint32_t timeToWait);
uint16_t CleanupThreads_CS_(uint32_t* threadID, uint16_t numberOfThreads, uint8_t* completedThreads, uint32_t timeToWait);
uint32_t CreateThread_CS_(void* Start, void* Args);
void WaitForThread_CS_(uint32_t threadID);
void WaitForThreads_CS_(uint32_t *threadID, uint32_t numberOfThreads);