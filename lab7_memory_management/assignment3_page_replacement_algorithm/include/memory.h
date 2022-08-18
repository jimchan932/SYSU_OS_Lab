#ifndef MEMORY_H
#define MEMORY_H
#include "address_pool.h"
class ArrayList
{
private:
    int len;
    
    int arr[3][2];
public:
    void remove(int vaddr);
    void append(int vaddr, int paddr);
    int at(int vaddr);
};

enum QUERY_TYPE {HIT, MISS_AND_REPLACE, MISS_AND_APPEND};
class MyHeap
{
private:
    //int numOfUsedPhysicalPages;
    int findIndex(int elem);
    int arr[3];
    int count;
public:
    void print();
    bool isFull();
    void init();
    enum QUERY_TYPE append(int *replacedVaddr, int elem);
    int popback();
};


class Queue
{
    int arr[3][2];
    int capacity;
    int front;
    int rear;
    int count;
public:
    Queue();
    void init(int size);    
    int getCapacity();
    int *dequeue();
    void enqueue(int item[2]);
    int *peek();
    int size();
    bool isEmpty();
    bool isFull();
};

enum AddressPoolType
{
    USER,
    KERNEL
};

class MemoryManager
{
public:
    Queue FIFOQueue;
    MyHeap LRUHeap;
    ArrayList LRUArrayList;
    // 可管理的内存容量
    int totalMemory;
    int memStartAddressFIFO;
    // 内核物理地址池
    AddressPool kernelPhysical;
    // 用户物理地址池
    AddressPool userPhysical;
    // 内核虚拟地址池
    AddressPool kernelVirtual;

public:
    
    MemoryManager();
    // 初始化地址池
    void initialize();
    void initFIFOQueue();
    //int getFIFOQueueLength();
    // 从type类型的物理地址池中分配count个连续的页
    // 成功，返回起始地址；失败，返回0
    int allocatePhysicalPages(enum AddressPoolType type, const int count);

    // 释放从paddr开始的count个物理页
    void releasePhysicalPages(enum AddressPoolType type, const int startAddress, const int count);

    // 获取内存总容量
    int getTotalMemory();

    // 开启分页机制
    void openPageMechanism();

    // 页内存分配
    int allocatePages(enum AddressPoolType type, const int count);

    // 虚拟页分配
    int allocateVirtualPages(enum AddressPoolType type, const int count);

    // 建立虚拟页到物理页的联系
    bool connectPhysicalVirtualPage(const int virtualAddress, const int physicalPageAddress);

    // 计算virtualAddress的页目录项的虚拟地址
    int toPDE(const int virtualAddress);

    // 计算virtualAddress的页表项的虚拟地址
    int toPTE(const int virtualAddress);

    // 页内存释放
    void releasePages(enum AddressPoolType type, const int virtualAddress, const int count);    

    // 找到虚拟地址对应的物理地址
    int vaddr2paddr(int vaddr);

    // 释放虚拟页
    void releaseVirtualPages(enum AddressPoolType type, const int vaddr, const int count);
    void queryKernelPhysicalPageFIFO(const int vaddr); // we will be querying kernel physical page
    void queryListOfKernelPhysicalPagesFIFO(const int *vaddr, int len);
    void queryKernelPhysicalPageLRU(int vaddr);
    void queryListOfKernelPhysicalPagesLRU(int *vaddr, int len);
	
};

#endif
