/* Utility data structures */

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


/*
  source code from memory.cpp
 */

void MemoryManager::initFIFOQueue()
{
    FIFOQueue.init(MEM_PAGES_TO_ALLOCATE);
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
