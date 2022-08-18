#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

//Semaphore semaphore;

Semaphore mutex;
Semaphore write;
int cheese_burger;
int readcount = 0;
char quotes[4][300] =
  {"The greatest glory in living lives not in never falling, but in rising everytime we fall",
   "Lost time is never found again",
   "Tell me and I forget. Teach me and I remember. Involve me and I learn",
   "Hard work beats talent when talent doesn't work hard"};

char *strcpy(char *destination, const char *source)
{
  if(destination == nullptr)
    {
      return nullptr;
    }

  char *ptr = destination;

  while(*source != '\0')
    {
      *destination = *source;
      destination++;
      source++;
    }
  *destination = '\0';

  return ptr;
}
void readQuote(int n)
{
  mutex.P();
  readcount++; // The number of readers has now increased by 1

  if(readcount==1)
    write.P(); // This ensures no writer can enter if there is even one reader
  mutex.V(); // other readers can enter while this reader is inside th critical section.
  /* current reader performs reading here */
  printf(quotes[n]);
  printf("\n");
  
  mutex.P();
  readcount--;
  if(readcount ==0)
    write.V();
  mutex.V();
} 

void readFirstQuote(void *arg)
{
  readQuote(0);
}
void readSecondQuote(void *arg)
{
  readQuote(1);
} 

void readThirdQuote(void *arg)
{
  readQuote(2);
}
void readFourthQuote(void *arg)
{
  readQuote(3);
}

void writeQuote(int i, char quote[100])
{
  /* write requests for critical section */
  write.P();
  /* perform the write */
  strcpy(quotes[i], quote);
  // leaves the critical section
  write.V();
}

void writeFirstQuote(void *arg)
{
  writeQuote(1, "");
}

void writeSecondQuote(void *arg)
{
  writeQuote(3, "Strive not to be a success, but to be of value");
}
void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);


    mutex.initialize(1);
    write.initialize(1);

    programManager.executeThread(readFirstQuote, nullptr, "second thread", 1);
    programManager.executeThread(readSecondQuote, nullptr, "third thread", 1);
    programManager.executeThread(readThirdQuote, nullptr, "fourth thread", 1);
    programManager.executeThread(readFourthQuote, nullptr, "fifth thread", 1);
    programManager.executeThread(writeFirstQuote, nullptr, "sixth thread", 1);
    programManager.executeThread(writeSecondQuote, nullptr, "seventh thread", 1);
    
    programManager.executeThread(readFourthQuote, nullptr, "eighth thread", 1);
    programManager.executeThread(readSecondQuote, nullptr, "ninth thread", 1);
    
    
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
