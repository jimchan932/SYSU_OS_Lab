#include "bitmap.h"
#include "stdlib.h"
#include "stdio.h"

BitMap::BitMap()
{
}

void BitMap::initialize(char *bitmap, const int length)
{
    this->bitmap = bitmap;
    this->length = length;

    int bytes = ceil(length, 8);
    
    for (int i = 0; i < bytes; ++i)
    {
        bitmap[i] = 0;
    }
    for(int i = 0; i < length; i++)
    {
      	memoryTypeList[i] = UNALLOCATED;
    }
    const int occupiedBlockSize = 3571;
    for(int i = 0; i < occupiedBlockSize; i++)
    {
      set(100+i, 1);
      set(4171+i, 1);     
      set(7942+i, 1);
      set(11813+i, 1);
      memoryTypeList[100+i] = OCCUPIED;      
      memoryTypeList[4171+i] = OCCUPIED;      
      memoryTypeList[7942+i] = OCCUPIED;
      memoryTypeList[11813+i] = OCCUPIED;      
    }
}
void BitMap::printMemory(memoryType memType, int startAddress, int endAddress)
{
  switch(memType)
    {
    case OCCUPIED:
      printf("Occupied: ");
      break;
    case UNALLOCATED:
      printf("Free space: ");
      break;
    case P1:
      printf("Process 1: ");
      break;
    case P2: 
      printf("Process 2: ");
      break;
    case P3:
      printf("Process 3: ");
      break;
    case P4:
      printf("Process 4: ");      
      break;
    }
  int blockByteSize =  4*(endAddress - startAddress + 1);
  printf("Page %d to %d (%d KB)\n", startAddress, endAddress, blockByteSize);
}
void BitMap::printMemorySpace()
{
  memoryType currentType = memoryTypeList[0];
  int memoryTypeStartAddress = 0;
  printf("--------------------------\n");
  int i = 0;
  for(; i < length; i++)
  {
    if(memoryTypeList[i] == currentType) continue;
    else
    {
      printMemory(currentType, memoryTypeStartAddress, i-1);
      memoryTypeStartAddress = i;
      currentType = memoryTypeList[i];
      printf("--------------------------\n");
    }	
  }
  printMemory(currentType, memoryTypeStartAddress, i-1);
}
/*
void BitMap::setBitmap(const int index, int length, const bool status, memoryType type)
{
  
  for(int i = index; i < length; i++)
  {
    set(i, 1);
  }
  }*/

bool BitMap::get(const int index) const
{
    int pos = index / 8;
    int offset = index % 8;

    return (bitmap[pos] & (1 << offset));
}

void BitMap::set(const int index, const bool status)
{
    int pos = index / 8;
    int offset = index % 8;

    // 清0
    bitmap[pos] = bitmap[pos] & (~(1 << offset));

    // 置1
    if (status)
    {
        bitmap[pos] = bitmap[pos] | (1 << offset);
    }
}

int BitMap::allocate(const int count)
{
    if (count == 0)
        return -1;

    int index, empty, start;

    index = 0;
    while (index < length)
    {
        // 越过已经分配的资源
        while (index < length && get(index))
            ++index;

        // 不存在连续的count个资源
        if (index == length)
            return -1;

        // 找到1个未分配的资源
        // 检查是否存在从index开始的连续count个资源
        empty = 0;
        start = index;
        while ((index < length) && (!get(index)) && (empty < count))
        {
            ++empty;
            ++index;
        }

        // 存在连续的count个资源
        if (empty == count)
        {
            for (int i = 0; i < count; ++i)
            {
                set(start + i, true);
            }

            return start;
        }
    }

    return -1;
}


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
void BitMap::release(const int index, const int count)
{
    for (int i = 0; i < count; ++i)
    {
        set(index + i, false);
    }
}

char *BitMap::getBitmap()
{
    return (char *)bitmap;
}

int BitMap::size() const
{
    return length;
}
