#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

typedef struct
{
    void (*pfun)(void* para);
    uint16_t delay;
    uint16_t period;
    bool run;
    void* para;
} timer_t;

typedef struct timer_node
{
    timer_t timer;
    struct timer_node *prev;
    struct timer_node *next;
} timer_node,*timer_list;

bool timer_list_init(timer_list *L)
{
    *L=(timer_list)malloc(sizeof(timer_node));
    if(*L)
    {
        (*L)->next=(*L)->prev=*L;
        (*L)->timer.pfun=NULL;
        (*L)->timer.delay=0;
        (*L)->timer.period=0;
        (*L)->timer.run=false;
        return true;
    }
    else
        return false;
}

bool timer_list_destory(timer_list *L)
{
    timer_list q,p=(*L)->next;
    while(p!=*L)
    {
        q=p->next;
        free(p);
        p=q;
    }
    free(*L);
    *L=NULL;
    (*L)->timer.pfun=NULL;
    (*L)->timer.delay=0;
    (*L)->timer.period=0;
    (*L)->timer.run=false;
    return true;
}

bool timer_list_clear(timer_list L)
{
    timer_list q,p=L->next;
    while(p!=L)
    {
        q=p->next;
        free(p);
        p=q;
    }
    L->next=L->prev=L;
    L->timer.pfun=NULL;
    L->timer.delay=0;
    L->timer.period=0;
    L->timer.run=false;
    return true;
}

bool timer_list_is_empty(timer_list L)
{
    if(L->next==L&&L->prev==L)
        return true;
    else
        return false;
}

int timer_list_length(timer_list L)
{
    int i=0;
    timer_list p=L->next;
    while(p!=L)
    {
        i++;
        p=p->next;
    }
    return i;
}

bool timer_list_get_item(timer_list L,int i,timer_t *e)
{
    int j=1;
    timer_list p=L->next;
    while(p!=L&&j<i)
    {
        p=p->next;
        j++;
    }
    if(p==L||j>i)
        return false;
    *e=p->timer;
    return true;
}

bool struct_compare(timer_t a, timer_t b)
{
    return (*a.pfun==*b.pfun   &&
            a.delay==b.delay   &&
            a.period==b.period &&
            a.run==b.run);
}

uint8_t timer_list_locate_item(timer_list L,timer_t e)
{
    uint8_t i=0;
    timer_list p=L->next;
    while(p!=L)
    {
        i++;
        if(struct_compare(p->timer,e))
            return i;
        p=p->next;
    }
    return 0;
}

timer_list timer_list_get_locate_p(timer_list L,int i)
{
    int j;
    timer_list p=L;
    for(j=1; j<=i; j++)
        p=p->next;
    return p;
}

bool timer_list_get_locate_prev_item(timer_list L,timer_t cur_e,timer_t *pre_e)
{
    timer_list p=L->next->next;
    while(p!=L)
    {
        if(struct_compare(p->timer,cur_e))
        {
            *pre_e=p->prev->timer;
            return true;
        }
        p=p->next;
    }
    return false;
}

bool timer_list_get_next_item(timer_list L,timer_t cur_e,timer_t *next_e)
{
    timer_list p=L->next->next;
    while(p!=L)
    {
        if(struct_compare(p->prev->timer,cur_e))
        {
            *next_e=p->timer;
            return true;
        }
        p=p->next;
    }
    return false;
}

bool timer_list_insert(timer_list L,int i,timer_t e)
{
    timer_list p,s;
    if(i<1||i>timer_list_length(L)+1)
        return false;
    p=timer_list_get_locate_p(L,i-1);
    if(!p)
        return false;
    s=(timer_list)malloc(sizeof(timer_node));
    if(!s)
        return false;
    //s->timer=e;
    memcpy(&(s->timer),&e,sizeof(timer_t));
    s->prev=p;
    s->next=p->next;
    p->next->prev=s;
    p->next=s;
    return true;
}

bool timer_list_delete(timer_list L,int i)
{
    timer_list p;
    if(i<1||i>timer_list_length(L))
        return false;
    p=timer_list_get_locate_p(L,i);
    if(!p)
        return false;
    //*e=p->timer;
    p->prev->next=p->next;
    p->next->prev=p->prev;
    free(p);
    return true;
}

bool timer_list_item_update(timer_list L,int i,timer_t t)
{
    timer_list p;
    if(i<1||i>timer_list_length(L))
        return false;
    p=timer_list_get_locate_p(L,i);
    if(!p)
        return false;
    p->timer=t;
    return true;
}

#if DEBUG
void timer_list_traverse(timer_list L)
{
    timer_list p=L->next;
    while(p!=L)
    {
        printf("%d ",(p->timer.delay));
        p=p->next;
    }
    printf("\n");
}

void timer_list_traverse_back(timer_list L)
{
    timer_list p=L->prev;
    while(p!=L)
    {
        printf("%d ",(p->timer.delay));
        p=p->prev;
    }
    printf("\n");
}
#endif

//测试实例

timer_list l;

bool timer_init(void)
{
    return timer_list_init(&l);
}

timer_t timer_creat(void(*pFunction)(void* para),
                    const uint16_t delay,
                    const uint16_t period,
                    const bool run,
                    void* para)
{
    timer_t t;
    t.pfun  = pFunction;
    t.delay  = delay;
    t.period  = period;
    t.run  = run;
    t.para = para;
    timer_list_insert(l,1,t);
    return l->timer;
}


bool timer_delete(const timer_t t)
{
    uint8_t i =timer_list_locate_item(l,t);
    return timer_list_delete(l,i);
}

void timer_start(timer_t t)
{
    uint8_t i =timer_list_locate_item(l,t);
    t.run=true;
    timer_list_item_update(l,i,t);
}

void timer_stop(timer_t t)
{
    uint8_t i =timer_list_locate_item(l,t);
    t.run=false;
    timer_list_item_update(l,i,t);
}


void timer_sched()
{
    timer_list p=l->next;
    while(p!=l)
    {
        printf("delay=%d\n",(p->timer.delay));

        if(p->timer.delay == 0)
        {
            if (p->timer.run)
            {
                (*p->timer.pfun)(p->timer.para);

                if (p->timer.period == 0)
                {
                    timer_delete(p->timer);
                }
                else p->timer.delay = p->timer.period;
            }
        }
        else
        {
            p->timer.delay -= 1;
        }
        p=p->next;
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

    timer_t t1=timer_creat(f1, 0, 100, false, NULL);
    timer_t t2=timer_creat(f2, 10, 200, false, NULL);
    timer_t t3=timer_creat(f3, 20, 400, false, NULL);

    timer_sched();

    timer_delete(t1);
    timer_delete(t2);
    timer_delete(t3);

    return 0;
}
