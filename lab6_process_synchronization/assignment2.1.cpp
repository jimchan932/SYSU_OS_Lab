#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
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

// 模拟从数据库读取数据的函数
void readQuote(int i)
{
  printf(quotes[i]);
  printf("\n");
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
// 模拟在数据库更新项目的函数。 我们在这里假设他需要的时间是比 RR 调度更长。
void writeQuote(int i, char quote[100]) 
{
 
  int delay = 0xfffffff;
  while(delay)
    delay--;
  strcpy(quotes[i], quote);

}

void writeFirstQuote(void *arg)  
{  
  writeQuote(1, "Do not go gentle into that good night, Old age should burn and rave at close of day; Rage, rage against the dying of the light.");
}

void writeSecondQuote(void *arg)
{
  writeQuote(3, "Drink your tea slowly and reverently, as if it is the axis on which the world earth revolves – slowly, evenly, without rushing toward the future; Live the actual moment. Only this moment is life.");
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


    //mutex.initialize(1);
    // write.initialize(1);

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
    int pid1 = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    //  int pid2 = programManager.executeThread(second_thread, nullptr, "second thread",1);
    if (pid1 == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    programManager.schedule();

    asm_halt();
}
