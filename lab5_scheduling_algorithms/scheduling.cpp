// 注意我在每个schedule 函数都把 running->state == ProgamStatus::RUNNING 的if语句
// 删除了， 因为我们并没有实现到中断，不需要从running 换到 waiting，
//而RR算法因为要在一段时间片运行所以需要。
void ProgramManager::schedule_FCFS()
{
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    if (readyPrograms.size() == 0)
    {
        interruptManager.setInterruptStatus(status);
        return;
    }

    if (running->status == ProgramStatus::DEAD)
    {
        releasePCB(running);
    }

    ListItem *item = readyPrograms.front();
    PCB *next = ListItem2PCB(item, tagInGeneralList);
    PCB *cur = running;
    next->status = ProgramStatus::RUNNING;
    running = next;
    readyPrograms.pop_front();
    asm_switch_thread(cur, next);
    interruptManager.setInterruptStatus(status);
}

void ProgramManager::schedule_SJF()
{
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    if (readyPrograms.size() == 0)
    {
        interruptManager.setInterruptStatus(status);
        return;
    }
    
    if (running->status == ProgramStatus::DEAD)
    {
        releasePCB(running);
    }
	// 记录目前最短的线程执行时间。
    int shortestJobTime = ListItem2PCB(readyPrograms.front(), tagInGeneralList)->burstTime;
	// 记录最短执行时间在就绪队列的地址。
    int indexOfShortestJob = 0;    

	// 找出最小元素的算法
    for(int i = 1; i < readyPrograms.size(); i++)
    {
      int tempTime = ListItem2PCB(readyPrograms.at(i), tagInGeneralList)->burstTime;
      if(tempTime < shortestJobTime)
      {
	indexOfShortestJob = i;
	shortestJobTime = tempTime;
      }
    }
 
    // 上下切换
    PCB *next = ListItem2PCB(readyPrograms.at(indexOfShortestJob), tagInGeneralList);
    PCB *cur = running;
    next->status = ProgramStatus::RUNNING;
    running = next;
	// 根据地址把PCB从就绪队列删除
    readyPrograms.erase(indexOfShortestJob);
    
    asm_switch_thread(cur, next);

    interruptManager.setInterruptStatus(status);
}

void ProgramManager::schedule_priority()
{
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    if (readyPrograms.size() == 0)
    {
        interruptManager.setInterruptStatus(status);
        return;
    }
	
    if (running->status == ProgramStatus::DEAD)
    {
        releasePCB(running);
    }

    ListItem *nextItem = nullptr;
    
    while(nextItem == nullptr)
    {

      ListItem *tempItem;

	  // 找出目前优先度的线程。
      for(int i = 0; i < readyPrograms.size(); i++)
      {
		  tempItem = readyPrograms.at(i);
		  if(ListItem2PCB(tempItem, tagInGeneralList)->priority == currentPriority)
		  {

			nextItem = tempItem;
			break;
		  }
      }
	  // 如果没有目前优先度的线程，那么increment currentPriority, 以在下一个循环找下一个优先度的线程。
      if(nextItem == nullptr) ++currentPriority;
    }
	// 上下切换
    PCB *next = ListItem2PCB(nextItem, tagInGeneralList);
    PCB *cur = running;
    next->status = ProgramStatus::RUNNING;
    running = next;
	// 把PCB从就绪队列删除。
    readyPrograms.erase(nextItem);
    
    asm_switch_thread(cur, next);

    interruptManager.setInterruptStatus(status);
}
