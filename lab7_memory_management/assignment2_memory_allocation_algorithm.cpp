/*
  Purpose of lab: to implement best fit and first fit algorithms (allocation of process into memory)
 */


/*
  source code from /src/utils/bitmap.cpp
 */ 

void BitMap::firstFit(int processMemorySize[4])
{
  memoryType processList[4] = {P1, P2, P3, P4};
  for(int i = 0; i < 4; i++)
  {
    int start = allocate(processMemorySize[i]/4);
    if(start != -1)
    {
      for(int j = 0; j < processMemorySize[i]/4; j++)
      {
	memoryTypeList[start + j] = processList[i];
      }
    }
  }
}
const int numOfUserPhysicalPages = 15984;

int BitMap::findBestFitAddress(int processMemoryPages)
{
  int bestFitStartAddress;
  int numOfUnallocatedPages = numOfUserPhysicalPages;
  int tempStartAddress;
  int tempNumOfUnallocatedPages = 0;
  int unallocatedFlag = 0;
  for(int i = 0; i < numOfUserPhysicalPages; i++)
  { 
    if(get(i) == 0 && !unallocatedFlag) // free
    {
      unallocatedFlag = 1;
      tempStartAddress = i;
      tempNumOfUnallocatedPages++;
    }
    if(get(i) == 0 && unallocatedFlag)
      tempNumOfUnallocatedPages++;
    
    if((get(i) == 1  || i == 15983) && unallocatedFlag)
    {
      if(processMemoryPages < tempNumOfUnallocatedPages
	 && tempNumOfUnallocatedPages < numOfUnallocatedPages)
      {
	
	numOfUnallocatedPages = tempNumOfUnallocatedPages;
	bestFitStartAddress = tempStartAddress;	
      }
      tempNumOfUnallocatedPages = 0;      
      unallocatedFlag = 0;
    }
    else continue;

  }
  return numOfUnallocatedPages == numOfUserPhysicalPages  ? -1 : bestFitStartAddress;
}
void BitMap::bestFit(int processMemorySize[4])
{
  memoryType processList[4] = {P1, P2, P3, P4};
  for(int i = 0; i < 4; i++)
  {
    int start = findBestFitAddress(processMemorySize[i]/4);
   
    if(start != -1)
    {
      for(int j = 0; j < processMemorySize[i]/4; j++)
      {
	set(start+j, 1);
	memoryTypeList[start + j] = processList[i];
      }
    }
  }
}

/* 
 source code from /src/utils/address_pool.cpp
 */

void AddressPool::firstFit(int processMemorySize[4])
{
  resources.firstFit(processMemorySize);
}
void AddressPool::bestFit(int processMemorySize[4])
{
  resources.bestFit(processMemorySize);
}

/*
  source code from /src/kernel/memory.cpp
 */


void  MemoryManager::firstFit(int processMemorySize[4], enum AddressPoolType type)
{
    if (type == AddressPoolType::KERNEL)
    {
      kernelPhysical.firstFit(processMemorySize);
    }
    else if (type == AddressPoolType::USER)
    {
      userPhysical.firstFit(processMemorySize);
    }
}
void MemoryManager::bestFit(int processMemorySize[4],enum AddressPoolType type)  
{
    if (type == AddressPoolType::KERNEL)
    {
      kernelPhysical.bestFit(processMemorySize);
    }
    else if (type == AddressPoolType::USER)
    {
      userPhysical.bestFit(processMemorySize);
    }
}

/*
  setup.cpp source code 
 */
#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    // stdio.moveCursor(0);
    // for (int i = 0; i < 25 * 80; ++i)
    // {
    //     stdio.print(' ');
    // }
    // stdio.moveCursor(0);

    asm_halt();
}

extern "C" void setup_kernel()
{

    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 内存管理器
    // also memory allocation using first Fit
    memoryManager.initialize();
    //memoryManager.printMemorySpace(AddressPoolType::USER);
    
    int processMemorySizeList[4] = {848,1668,448,1704};
    //memoryManager.firstFit(processMemorySizeList, AddressPoolType::USER);
    memoryManager.bestFit(processMemorySizeList, AddressPoolType::USER);
    memoryManager.printMemorySpace(AddressPoolType::USER);
    
    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}

