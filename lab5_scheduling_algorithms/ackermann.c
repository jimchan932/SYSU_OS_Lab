#include <stdio.h>
#include <time.h>

// ackermann 函数
int ack(int m, int n)
{
  if(m== 0)
    {
      return n+1;
    }
  else if((m > 0) && (n == 0))
    {
      return ack(m-1, 1);
    }
  else if((m > 0) && (n > 0))
    {
      return ack(m-1, ack(m, n-1));
    }
}
// 目的： 估计线程的执行时间。
// to measure execution time of thread (for SJF)
int main()
{
  
  clock_t start1 = clock();
  int x  =ack(3,12);
  clock_t end1 = clock();
  clock_t start2 = clock();
  int y = ack(3, 11);
  clock_t end2 = clock();
  clock_t start3 = clock();
  int z = ack(3, 8);
  clock_t end3 = clock();
  
  double execTime1 = (double)(end1 - start1) / CLOCKS_PER_SEC;
  double execTime2 = (double)(end2 - start2) / CLOCKS_PER_SEC;
  double execTime3 = (double)(end3 - start3) / CLOCKS_PER_SEC;  
  printf("ack(3,12) = %d\n", x);
  printf("Execution time: %lf\n\n", execTime1);
  printf("ack(3,11) = %d\n", y);
  printf("Execution time: %lf\n\n", execTime2);
  printf("ack(3,8) = %d\n", z);
  printf("Execution time: %lf\n\n", execTime3);
  return 0;
}
