#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define  OS_TIMER_MAX  32

struct timer_t {
    void (*pfun)(void* para);
    unsigned int delay;
    unsigned int period;
    bool run;
};
struct timer_t timer_list[OS_TIMER_MAX];

typedef uint8_t timer_id;

void timer_init(void)
{
  uint8_t index;

  for (index = 0; index < OS_TIMER_MAX; index++)
  {
      timer_list[index].pfun = NULL;
  }
}

timer_id timer_creat(void(*pFunction)(void* para),
                          const unsigned int delay,
                          const unsigned int period,
                          bool run)
{
  uint8_t index = 0;

  while ((timer_list[index].pfun != NULL) && (index < OS_TIMER_MAX))
  {
      index++;
  }

  printf("time index %d\r\n",index);

  timer_list[index].pfun  = pFunction;

  timer_list[index].delay  = delay;

  timer_list[index].period  = period;

  timer_list[index].run  = run;

  return index;
}

bool timer_delete(const timer_id index)
{
  if(index >= OS_TIMER_MAX) return false;

  if (timer_list[index].pfun == NULL)
  {
    return false;
  }
  timer_list[index].pfun   = NULL;
  timer_list[index].delay  = 0;
  timer_list[index].period = 0;
  timer_list[index].run    = false;

  return true;
}

void timer_start(const timer_id index)
{
    if(index >= OS_TIMER_MAX) return;

    timer_list[index].run    = true;
}

void timer_stop(const timer_id index)
{
    if(index >= OS_TIMER_MAX) return;

    timer_list[index].run    = false;
}

void timer_run(void* para)
{
  uint8_t index;
  for (index = 0; index < OS_TIMER_MAX; index++)
  {
    if(timer_list[index].delay == 0){
        if (timer_list[index].run)
        {
            (*timer_list[index].pfun)(para);
            if (timer_list[index].period == 0)
            {
                timer_delete(index);
            }
            else timer_list[index].delay = timer_list[index].period;
        }
    }
    else
    {
        timer_list[index].delay -= 1;
    }
  }
}

void f1(void* para)
{}

void f2(void* para)
{}

void f3(void* para)
{}


int main()
{
    printf("Hello world!\n");

    timer_init();

    timer_id t1 = timer_creat(f1, 0, 100,false);
    timer_id t2 = timer_creat(f2, 10, 200,false);
    timer_id t3 = timer_creat(f3, 20, 400,false);

	void *para;
    timer_run(para);

    timer_start(t1);
    timer_stop(t1);

    timer_delete(t1);
    timer_delete(t2);
    timer_delete(t3);
	
    return 0;
}
