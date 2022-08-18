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

Semaphore chopstick[5];

void philosopherEats(int i)
{
  // 在这里看看邻居又没有使用筷子（资源）, 如果两个邻居都没有的话，那么哲学家就可以吃餐
  chopstick[i].P();
  chopstick[(i+1)%5].P();
  printf("Philosopher %d eats\n", i+1);
  // 在这里哲学家已经吃完了，就通过信号量释放资源
  chopstick[i].V();
  chopstick[(i+1)%5].V();
  printf("Philosopher %d thinks\n", i+1);  
}
void philosopher1Eats(void *arg)
{
  philosopherEats(0);
}

void philosopher2Eats(void *arg)
{
  philosopherEats(1);
}

void philosopher3Eats(void *arg)
{
  philosopherEats(2);
}

void philosopher4Eats(void *arg)
{
  philosopherEats(3);
}

void philosopher5Eats(void *arg)
{
  philosopherEats(4);
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

	// 初始化筷子Semaphore
    for(int i = 0; i < 5; i++)
    {
		chopstick[i].initialize(1);
    }

	// 执行哲学家吃餐的线程
    programManager.executeThread(philosopher4Eats, nullptr, "fifth thread", 1);
    programManager.executeThread(philosopher1Eats, nullptr, "second thread", 1);
    programManager.executeThread(philosopher2Eats, nullptr, "third thread", 1);
    programManager.executeThread(philosopher3Eats, nullptr, "fourth thread", 1);
    programManager.executeThread(philosopher5Eats, nullptr, "sixth thread", 1);

    asm_halt();
}
