#include "address_pool.h"
#include "os_constant.h"

AddressPool::AddressPool()
{
}

// 设置地址池BitMap
void AddressPool::initialize(char *bitmap, const int length, const int startAddress)
{
    resources.initialize(bitmap, length);
    this->startAddress = startAddress;
}

// 从地址池中分配count个连续页
int AddressPool::allocate(const int count)
{
    int start = resources.allocate(count);
    return (start == -1) ? -1 : (start * PAGE_SIZE + startAddress);
}
void AddressPool::firstFit(int processMemorySize[4])
{
  resources.firstFit(processMemorySize);
}
void AddressPool::bestFit(int processMemorySize[4])
{
  resources.bestFit(processMemorySize);
}

// 释放若干页的空间
void AddressPool::release(const int address, const int amount)
{
    resources.release((address - startAddress) / PAGE_SIZE, amount);
}
void AddressPool::printMemorySpace()
{
  resources.printMemorySpace();
}
