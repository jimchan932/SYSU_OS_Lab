#include "memory.h"
#include "os_constant.h"
#include "stdlib.h"
#include "asm_utils.h"
#include "stdio.h"
#include "program.h"
#include "os_modules.h"
#define MEM_PAGES_TO_ALLOCATE 3
#define HEAPSIZE 3

int ArrayList::at(int vaddr)
{
    for(int i = 0; i < len; i++)
    {
	if(arr[i][0] == vaddr)
	{
	    return arr[i][1];
	}
    }
}

void ArrayList::append(int vaddr, int paddr)
{
    arr[len][0] = vaddr;
    arr[len][1] = paddr;
    len++;
}
void ArrayList::remove(int vaddr)
{
    for(int i = 0; i < len; i++)
    {
	if(arr[i][0] == vaddr)
	    for(int j = i; j < len; j++)
	    {
		arr[j][0] = arr[j+1][0];
		arr[j][1] = arr[j+1][1];
	    }
    }
    len--;
}
void MyHeap::init()
{    
}
bool MyHeap::isFull()
{
    return (count == MEM_PAGES_TO_ALLOCATE);
}

int MyHeap::findIndex(int elem)
{
    for(int i = 0; i < count; i++)
    {
	if(elem == arr[i])
	{

	    return i;

	}
    }
    
    return -1;
}

enum QUERY_TYPE MyHeap::append(int *replacedVAddr, int elem)
{
    
    int elemIndex = findIndex(elem);
    if(elemIndex == -1) // cannot find virtual address int the LRU heap
    {	    	
	if(!isFull())
	{
	
	    for(int i = count++; i > 0; i--)
	    {
		arr[i] = arr[i-1];
	    }
	    arr[0] = elem;
	    return MISS_AND_APPEND;	     
	}
	else
	{
	    *replacedVAddr = popback();	
	    for(int i = count-1; i > 0; i--)
	    {
		arr[i] = arr[i-1];
	    }
	    arr[0] = elem;	    	    
	    
	    return MISS_AND_REPLACE;	    
	}
    }
    else  // hit
    {
	
	
	for(int i = elemIndex;  i > 0; i--)
	{
	    arr[i] = arr[i-1];
	}
	arr[0] = elem;

	return HIT;	
    }
   
}
void MyHeap::print()
{
    for(int i = 0; i < count; i++)
    {
	printf("%d ->", arr[i]);
    }
}
int MyHeap::popback()
{
    return arr[count-1];
}

Queue::Queue()
{
    capacity = MEM_PAGES_TO_ALLOCATE;
    front = 0;
    rear = -1;
    count = 0;  
}
void Queue::init(int size)
{
    this->capacity = 3;
    this->front = 0;
    this->rear = -1;
    this->count = 0;  
}

int Queue::getCapacity()
{
    return capacity;
}

int *Queue::dequeue()
{
    if(isEmpty())
	printf("Underflow terminated\n");
    int *x = arr[front];

    front = (front + 1) % capacity;
    count--;

    return x;
}


void Queue::enqueue(int item[2])
{
    if(isFull())
    {
	printf("Overflow");
    }

    rear = (rear+1) % capacity;
    arr[rear][0] = item[0]; // virtual address
    arr[rear][1] = item[1]; // physical address
    count++;    
}

int *Queue::peek()
{
    return arr[front];
}

int Queue::size()
{
    return count;
}

bool Queue::isEmpty()
{
    return (size() == 0);
}

bool Queue::isFull()
{
    return (size() == capacity);
}


MemoryManager::MemoryManager()
{
    initialize();
}
void MemoryManager::initFIFOQueue()
{
    FIFOQueue.init(MEM_PAGES_TO_ALLOCATE);
}
void MemoryManager::initialize()
{
  
    this->totalMemory = 0;
    this->totalMemory = getTotalMemory();

    // 预留的内存
    int usedMemory = 256 * PAGE_SIZE + 0x100000;
    if (this->totalMemory < usedMemory)
    {
        printf("memory is too small, halt.\n");
        asm_halt();
    }
    // 剩余的空闲的内存
    int freeMemory = this->totalMemory - usedMemory;

    int freePages = freeMemory / PAGE_SIZE;
    int kernelPages = freePages / 2;
    int userPages = freePages - kernelPages;

    int kernelPhysicalStartAddress = usedMemory;
    int userPhysicalStartAddress = usedMemory + kernelPages * PAGE_SIZE;

    int kernelPhysicalBitMapStart = BITMAP_START_ADDRESS;
    int userPhysicalBitMapStart = kernelPhysicalBitMapStart + ceil(kernelPages, 8);
    int kernelVirtualBitMapStart = userPhysicalBitMapStart + ceil(userPages, 8);

    kernelPhysical.initialize(
        (char *)kernelPhysicalBitMapStart,
        kernelPages,
        kernelPhysicalStartAddress);

    userPhysical.initialize(
        (char *)userPhysicalBitMapStart,
        userPages,
        userPhysicalStartAddress);

    kernelVirtual.initialize(
        (char *)kernelVirtualBitMapStart,
        kernelPages,
        KERNEL_VIRTUAL_START);
 
    //memStartAddressFIFO = allocatePhysicalPages(AddressPoolType::KERNEL, MEM_PAGES_TO_ALLOCATE);
    //printf("Starting memory address of physical pages: %d \n\n", memStartAddressFIFO);
}

int MemoryManager::allocatePhysicalPages(enum AddressPoolType type, const int count)
{
    int start = -1;

    if (type == AddressPoolType::KERNEL)
    {
        start = kernelPhysical.allocate(count);
    }
    else if (type == AddressPoolType::USER)
    {
        start = userPhysical.allocate(count);
    }

    return (start == -1) ? 0 : start;
}

void MemoryManager::releasePhysicalPages(enum AddressPoolType type, const int paddr, const int count)
{
    if (type == AddressPoolType::KERNEL)
    {
        kernelPhysical.release(paddr, count);
    }
    else if (type == AddressPoolType::USER)
    {

        userPhysical.release(paddr, count);
    }
}

int MemoryManager::getTotalMemory()
{

    if (!this->totalMemory)
    {
        int memory = *((int *)MEMORY_SIZE_ADDRESS);
        // ax寄存器保存的内容
        int low = memory & 0xffff;
        // bx寄存器保存的内容
        int high = (memory >> 16) & 0xffff;

        this->totalMemory = low * 1024 + high * 64 * 1024;
    }

    return this->totalMemory;
}

void MemoryManager::openPageMechanism()
{
    // 页目录表指针
    int *directory = (int *)PAGE_DIRECTORY;
    //线性地址0~4MB对应的页表
    int *page = (int *)(PAGE_DIRECTORY + PAGE_SIZE);

    // 初始化页目录表
    memset(directory, 0, PAGE_SIZE);
    // 初始化线性地址0~4MB对应的页表
    memset(page, 0, PAGE_SIZE);

    int address = 0;
    // 将线性地址0~1MB恒等映射到物理地址0~1MB
    for (int i = 0; i < 256; ++i)
    {
        // U/S = 1, R/W = 1, P = 1
        page[i] = address | 0x7;
        address += PAGE_SIZE;
    }

    // 初始化页目录项

    // 0~1MB
    directory[0] = ((int)page) | 0x07;
    // 3GB的内核空间
    directory[768] = directory[0];
    // 最后一个页目录项指向页目录表
    directory[1023] = ((int)directory) | 0x7;

    // 初始化cr3，cr0，开启分页机制
    asm_init_page_reg(directory);

    printf("open page mechanism\n");
}

int MemoryManager::allocatePages(enum AddressPoolType type, const int count)
{
    // 第一步：从虚拟地址池中分配若干虚拟页
    int virtualAddress = allocateVirtualPages(type, count);
    if (!virtualAddress)
    {
        return 0;
    }

    bool flag;
    int physicalPageAddress;
    int vaddress = virtualAddress;

    // 依次为每一个虚拟页指定物理页
    for (int i = 0; i < count; ++i, vaddress += PAGE_SIZE)
    {
        flag = false;
        // 第二步：从物理地址池中分配一个物理页
        physicalPageAddress = allocatePhysicalPages(type, 1);
        if (physicalPageAddress)
        {
	  //printf("allocate physical page 0x%x\n", physicalPageAddress);

            // 第三步：为虚拟页建立页目录项和页表项，使虚拟页内的地址经过分页机制变换到物理页内。
            flag = connectPhysicalVirtualPage(vaddress, physicalPageAddress);
        }
        else
        {
            flag = false;
        }

        // 分配失败，释放前面已经分配的虚拟页和物理页表
        if (!flag)
        {
            // 前i个页表已经指定了物理页
            releasePages(type, virtualAddress, i);
            // 剩余的页表未指定物理页
            releaseVirtualPages(type, virtualAddress + i * PAGE_SIZE, count - i);
            return 0;
        }
    }

    return virtualAddress;
}

int MemoryManager::allocateVirtualPages(enum AddressPoolType type, const int count)
{
    int start = -1;

    if (type == AddressPoolType::KERNEL)
    {
        start = kernelVirtual.allocate(count);
    }

    return (start == -1) ? 0 : start;
}

bool MemoryManager::connectPhysicalVirtualPage(const int virtualAddress, const int physicalPageAddress)
{
    // 计算虚拟地址对应的页目录项和页表项
    int *pde = (int *)toPDE(virtualAddress);
    int *pte = (int *)toPTE(virtualAddress);

    // 页目录项无对应的页表，先分配一个页表
    if(!(*pde & 0x00000001)) 
    {
        // 从内核物理地址空间中分配一个页表
        int page = allocatePhysicalPages(AddressPoolType::KERNEL, 1);
        if (!page)
            return false;

        // 使页目录项指向页表
        *pde = page | 0x7;
        // 初始化页表
        char *pagePtr = (char *)(((int)pte) & 0xfffff000);
        memset(pagePtr, 0, PAGE_SIZE);
    }

    // 使页表项指向物理页
    *pte = physicalPageAddress | 0x7;

    return true;
}

int MemoryManager::toPDE(const int virtualAddress)
{
    return (0xfffff000 + (((virtualAddress & 0xffc00000) >> 22) * 4));
}

int MemoryManager::toPTE(const int virtualAddress)
{
    return (0xffc00000 + ((virtualAddress & 0xffc00000) >> 10) + (((virtualAddress & 0x003ff000) >> 12) * 4));
}

void MemoryManager::releasePages(enum AddressPoolType type, const int virtualAddress, const int count)
{
    int vaddr = virtualAddress;
    int *pte;
    for (int i = 0; i < count; ++i, vaddr += PAGE_SIZE)
    {
        // 第一步，对每一个虚拟页，释放为其分配的物理页
        releasePhysicalPages(type, vaddr2paddr(vaddr), 1);

        // 设置页表项为不存在，防止释放后被再次使用
        pte = (int *)toPTE(vaddr);
        *pte = 0;
    }

    // 第二步，释放虚拟页
    releaseVirtualPages(type, virtualAddress, count);
}

int MemoryManager::vaddr2paddr(int vaddr)
{
    int *pte = (int *)toPTE(vaddr);
    int page = (*pte) & 0xfffff000;
    int offset = vaddr & 0xfff;
    return (page + offset);
}

void MemoryManager::releaseVirtualPages(enum AddressPoolType type, const int vaddr, const int count)
{
    if (type == AddressPoolType::KERNEL)
    {
        kernelVirtual.release(vaddr, count);
    }
}



void MemoryManager::queryKernelPhysicalPageFIFO(const int referenceBit)
{
  int vaddr = referenceBit * PAGE_SIZE;
  if(kernelVirtual.get(referenceBit) == 0) // unallocated
  {      
      if(FIFOQueue.isFull())
      {	  
	  int *addressPair = FIFOQueue.dequeue();
	  int firstInVAddr = addressPair[0];
	  int firstInPAddr = addressPair[1];

	  // swap in and swap out virtual page
	  // remove old virtual page and allocate new virtual page	  
	  int virtualPageToReplace = firstInVAddr / PAGE_SIZE;
	  kernelVirtual.set(virtualPageToReplace, 0);
	  int *pte = (int *)toPTE(firstInVAddr);
	  *pte = 0; // set the virtual address that we swap out to 0
	  kernelVirtual.set(referenceBit, 1);
	  
	  // set the page table entry of virtual address to physical address
	  int flag = connectPhysicalVirtualPage(vaddr, firstInPAddr);
	  if(!flag)
	  {
	      printf("ERROR: Failed to make page directory entry and page table entry for mapping virtual page to physical frame\n");
	  }
	  int virtualPageToSwapOut = firstInVAddr / PAGE_SIZE;
	  int virtualPageToSwapIn = referenceBit;
	  printf("swapped out page %d (%d) swapped in page %d(%d), maps to frame %d\n",virtualPageToSwapOut, firstInVAddr, virtualPageToSwapIn, vaddr, firstInPAddr);
	  int newAddressPairItem[] = {vaddr, firstInPAddr};
	  FIFOQueue.enqueue(newAddressPairItem);
      }
      else // not full
      {
	  int mappingPAddr = allocatePhysicalPages(AddressPoolType::KERNEL, 1);
	  
	  if(!mappingPAddr)
	  {
	      printf("ERROR: Failed to allocate physical page");
	      return;
	  }
	 
	  kernelVirtual.set(referenceBit, 1); // allocate a virtual page
	  int flag = connectPhysicalVirtualPage(vaddr, mappingPAddr);
	  if(!flag)
	  {
	      printf("ERROR: Failed to make page directory entry and page table entry for mapping virtual page to physical frame\n");
	  }
	  int virtualPage = referenceBit;
	  printf("Page Fault, added page table entry: virtual page %d(%d) to frame %d\n", virtualPage, vaddr, vaddr2paddr(vaddr));
	  int newAddressPair[] = {vaddr, mappingPAddr}; 
	  FIFOQueue.enqueue(newAddressPair);  
      }  
  }
  else
  {
      printf("Success: page %d(%d) to physical addr(%d)\n", referenceBit, vaddr,  vaddr2paddr(vaddr));
  }
}
void MemoryManager::queryKernelPhysicalPageLRU(int referenceBit)
{
    
    int vaddr = referenceBit * PAGE_SIZE;
    int vaddrToReplace;
    enum QUERY_TYPE queryResult = LRUHeap.append(&vaddrToReplace, vaddr);
    if(queryResult == MISS_AND_APPEND)
    {
	  int mappingPAddr = allocatePhysicalPages(AddressPoolType::KERNEL, 1);	  
	  if(!mappingPAddr)
	  {
	      printf("ERROR: Failed to allocate physical page");
	      return;
	  }
	 
	  kernelVirtual.set(referenceBit, 1); // allocate a virtual page
	  int flag = connectPhysicalVirtualPage(vaddr, mappingPAddr);
	  if(!flag)
	  {
	      printf("ERROR: Failed to make page directory entry and page table entry for mapping virtual page to physical frame\n");
	  }
	  int virtualPage = referenceBit;
	  printf("Page Fault, added page table entry: virtual page %d(%d) to frame %d\n", virtualPage, vaddr, vaddr2paddr(vaddr));	 
	  LRUArrayList.append(vaddr, mappingPAddr);
	   
    }
    else if(queryResult == MISS_AND_REPLACE)
    {
	/*
	  int paddr = vaddr2paddr(vaddrToReplace);
	  kernelVirtual.set(vaddrToReplace, 0);         
	  connectPhysicalVirtualPage(vaddr, paddr);
	  kernelVirtual.set(vaddr, 1);
	  printf("Replaced virtual page %d with %d, maps to physical page %d\n", vaddrToReplace, vaddr, paddr);
	*/

	  int paddr = LRUArrayList.at(vaddrToReplace);	 	  
	  // swap in and swap out virtual page
	  // remove old virtual page and allocate new virtual page
	  //releaseVirtualPages(AddressPoolType::KERNEL, vaddrToReplace, 1);
	  int virtualPageToReplace = vaddrToReplace / PAGE_SIZE;
	  kernelVirtual.set(virtualPageToReplace, 0);
	  int *pte = (int *)toPTE(vaddrToReplace);
	  *pte = 0; // set the virtual address that we swap out to 0
	  kernelVirtual.set(referenceBit, 1);
	  
	  // set the page table entry of virtual address to physical address
	  int flag = connectPhysicalVirtualPage(vaddr, paddr);
	  if(!flag)
	  {
	      printf("ERROR: Failed to make page directory entry and page table entry for mapping virtual page to physical frame\n");
	  }
	  int virtualPageToSwapOut = vaddrToReplace / PAGE_SIZE;
	  int virtualPageToSwapIn = referenceBit;
	  printf("swapped out page %d (%d) swapped in page %d(%d), maps to frame %d\n",virtualPageToSwapOut, vaddrToReplace, virtualPageToSwapIn, vaddr, paddr);
	  LRUArrayList.remove(vaddrToReplace);
	  LRUArrayList.append(vaddr, paddr);	
    }
    else if(queryResult == HIT)
    {
	printf("Success: page %d(%d) to physical addr(%d)\n", referenceBit, vaddr,  vaddr2paddr(vaddr));	
    }    
}

void MemoryManager::queryListOfKernelPhysicalPagesLRU(int *vaddrList, int len)
{
    for(int i = 0; i < len; i++)
    {
	printf("%d: ", i + 1);
	queryKernelPhysicalPageLRU(vaddrList[i]);
    }
}

void MemoryManager::queryListOfKernelPhysicalPagesFIFO(const int *vaddrList, int len)
{      
    for(int i = 0; i < len; i++)
    {
	
	printf("%d: ", i + 1);
	queryKernelPhysicalPageFIFO(vaddrList[i]);
    }
}
