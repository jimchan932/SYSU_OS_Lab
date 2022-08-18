#include <iostream>
#include <stdarg.h>
#include <string.h>

void myPrintf(const char * FormatStr, int numOfArgs, ...)
{
	int formatStrLen = strlen(FormatStr);
	int storeArgFlag = false;
	va_list parameter;
	va_start(parameter, numOfArgs);
	for(int i = 0; i < formatStrLen; i++)
	{
		if(*(FormatStr + i) == '%')
		{
			storeArgFlag = true;
			continue;
		}
		if(storeArgFlag)
		{
			switch(*(FormatStr + i))
			{
				case 'd':
					std::cout << va_arg(parameter, int);
					break;
			}
			storeArgFlag = false;
			continue;
		}
		std::cout << *(FormatStr + i);
	}
	va_end(parameter);
	std::cout << std::endl;
}

int main()
{
	myPrintf("hello world %d %d %d %d", 4, 1, 2 ,3 ,4);
	return 0;
}
