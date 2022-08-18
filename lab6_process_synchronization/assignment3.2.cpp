#include "sync.h"
#include "asm_utils.h"
#include "stdio.h"
#include "os_modules.h"
#include "program.h"

SpinLock::SpinLock()
{
    initialize();
}

void SpinLock::initialize()
{
    bolt = 0;
}

void SpinLock::lock()
{
    uint32 key = 1;

    do
    {
        asm_atomic_exchange(&key, &bolt);
        //printf("pid: %d\n", programManager.running->pid);
    } while (key);
}

void SpinLock::unlock()
{
    bolt = 0;
}

Semaphore::Semaphore()
{
    initialize(0);
}

void Semaphore::initialize(uint32 counter)
{
    this->counter = counter;
    semLock.initialize();
    waiting.initialize();
}

void Semaphore::P()
{
    PCB *cur = nullptr;

    while (true)
    {
        semLock.lock();
        if (counter > 0)
        {
            --counter;
            semLock.unlock();
            return;
        }

        cur = programManager.running;
        waiting.push_back(&(cur->tagInGeneralList));
        cur->status = ProgramStatus::BLOCKED;

        semLock.unlock();
        programManager.schedule();
    }
}

void Semaphore::V()
{
    semLock.lock();
    ++counter;
    if (waiting.size())
    {
        PCB *program = ListItem2PCB(waiting.front(), tagInGeneralList);
        waiting.pop_front();
        semLock.unlock();
        programManager.MESA_WakeUp(program);
    }
    else
    {
        semLock.unlock();
    }
}
// 管程类的函数
// 线程在想进入临界区是执行这代码
void DP::pickup(int i)
{
  state[i] = HUNGRY;
  test(i);
  if(state[i] != EATING)
    self[i].P();
}
// 线程在想退出临界区是执行这代码，并释放资源给邻居让他们使用
void DP::putdown(int i)
{
  state[i] = THINKING;
  test((i+4) %5);
  test((i+1) % 5);
}

void DP::test(int i)
{
  // 检测是否有邻居哲学家在吃餐
  if((state[(i+4) % 5] != EATING) &&
     (state[i] == HUNGRY) &&
     (state[(i+1) % 5] != EATING))
    {
      state[i] = EATING;
      self[i].V();
    }
}

void DP::init()
{
  for(int i = 0; i < 5; i++)
    {
      state[i] = THINKING;
    }
}
