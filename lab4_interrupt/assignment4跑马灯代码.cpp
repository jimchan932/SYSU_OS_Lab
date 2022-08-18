// 跑马灯： 显示自己学号和英文名
extern "C" void schoolnumber_interrupt()
{
	// 学号与姓名
    char studentNumberAndNameArray[] = "20337021 Jim";
    const int lengthOfArray = 12; // 字符串长度
    // 清空屏幕
    for (int i = 0; i < 80; ++i)
    {
        stdio.print(0, i, ' ', 0x07);
    }
    
    ++times;
    char str[] = "interrupt happend: ";
    char number[10];
    int temp = times;

    // 移动光标到(0,0)输出字符
    stdio.moveCursor(0);
    for(int i = 0; str[i]; ++i ) {
        stdio.print(str[i]);
    }

	// 我们在这里对应时间来每一秒打印一个字
	
    int outputIndex = times % lengthOfArray;
	// 打印空格
	for(int i = 0; i < outputIndex; i++)
    {
      stdio.print(' ');
    }
	// 打印目前中断的次数所需要的字
    stdio.print(studentNumberAndNameArray[outputIndex]);
}
