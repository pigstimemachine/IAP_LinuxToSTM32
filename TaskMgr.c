/*
 * TaskMgr.c
 *
 *  Created on: 2017年2月10日
 *      Author: Administrator
 */
#include "TaskMgr.h"
#include "lib.h"


/*************************************************/
TIME_FORM SysTimer={0,0,0,0,0};//系统时间
/* timer list(arry) */
static timer_struct	  Usertimer[TIMER_MAX]= {0};
timer_t   		timer0_task0,timer0_task1,timer0_task2,timer0_task3,timer0_task4,timer1_task0,timer1_task1,timer1_task2,timer1_task3,timer1_task4,timer1_task5;
CpuTimer  CpuTimer0,CpuTimer1,CpuTimer2;
static u8 timer_unlink(timer_t *ptimer,u8 TimerNum);
static u8 timer_link(timer_t *ptimer,u8 TimerNum);
static void timer_cleanup(timer_t *ptimer);
static void Timer0_Task0(void);
static void Timer0_Task1(void);
static void Timer0_Task2(void);
static void Timer0_Task3(void);
static void Timer0_Task4(void);
static void Timer1_Task0(void);
static void Timer1_Task1(void);
static void Timer1_Task2(void);
static void Timer1_Task3(void);
static void Timer1_Task4(void);
static void Timer1_Task5(void);




void CPU_TimerInit(void)
{
	TIM2_Int_Init(9,7199);
}

static void SysTimer_Operation(void)
{
	TIME_FORM *Timer;
	Timer=&SysTimer;
	Timer->Msecond++;
	if(Timer->Msecond>=1000)
	{
		Timer->Msecond=0;
		Timer->Second++;
		if(Timer->Second>=60)
		{
			Timer->Second=0;
			Timer->Minute++;
			if(Timer->Minute>=60)
			{
				Timer->Minute=0;
				Timer->Hour++;
				if(Timer->Hour>=24)
				{
					Timer->Hour=0;
					Timer->Day++;
					if(Timer->Day>=2047)
					{
						Timer->Day=0;
					}
				}
			}
		}
	}
}
/*timer0_task :紧急任务   timer1_task :非紧急任务*/
void SysTask_Create(void)
{
	timer_init();
	Task_create(&timer0_task0,1,OPT_TMR_PERIODIC,Timer0_Task0,timer0);     
	Task_create(&timer0_task1,100,OPT_TMR_PERIODIC,Timer0_Task1,timer0);     
	Task_create(&timer1_task0,1000,OPT_TMR_PERIODIC,Timer1_Task0,timer1);  
	Task_create(&timer1_task1,5,OPT_TMR_PERIODIC,Timer1_Task1,timer1);
	Task_create(&timer1_task2,100,OPT_TMR_PERIODIC,Timer1_Task2,timer1);
	Task_create(&timer1_task3,1,OPT_TMR_PERIODIC,Timer1_Task3,timer1);
	Task_create(&timer1_task4,333,OPT_TMR_PERIODIC,Timer1_Task4,timer1);
	Task_create(&timer1_task5,10,OPT_TMR_PERIODIC,Timer1_Task5,timer1);
}

u32 GetSysTick(void)
{
  return CpuTimer0.InterruptCount;
}

void GetSysTime(TIME_FORM **Systime)
{
	*Systime=&SysTimer;
}

void Timer0_IsrHandler(void)
{
  CpuTimer0.InterruptCount++;
  timer_task(&CpuTimer0.InterruptCount,0);
	SysTimer_Operation();
}

void Timer1_IsrHandler(void)
{
	timer_task(&CpuTimer0.InterruptCount,1);
}

void Timer2_IsrHandler(void)
{
  CpuTimer2.InterruptCount++;
}

/**************** public function implement ***********************************/
/*
 * 初始化软件计时器，
 */
void timer_init(void)
{
	u8 i,j;
	for(j=0;j<TIMER_MAX;j++)
	{
		for(i=0;i<TIMER_LIST_MAX;i++){
			Usertimer[j].timer_list[i] = NULL;
		}
		Usertimer[j].timer_count=0;
		Usertimer[j].timer_tick=0;
	}

}

/* 在裸机中使用timer时，该函数在硬件定时器中断服务程序中运行，在操作系统中
 * 用1个独立任务来代替 */
void timer_task(u32 *CpuTick,u8 TimerNum)
{
	u8 i;
	timer_struct *ptmr = NULL;
	ptmr=&Usertimer[TimerNum];
	ptmr->timer_tick=*CpuTick;
	for(i=0;i<TIMER_LIST_MAX;i++)
	{
		if(!ptmr->timer_list[i])  continue;				
		if(ptmr->timer_tick >= ptmr->timer_list[i]->expire)
		{
			if(ptmr->timer_list[i]->callback != NULL)
			{
				ptmr->timer_list[i]->callback();
			}
			if(ptmr->timer_list[i]->opt == OPT_TMR_PERIODIC)
			{             
				ptmr->timer_list[i]->expire = ptmr->timer_tick + ptmr->timer_list[i]->period;    
			}
			else
			{
				ptmr->timer_list[i]->state = TMR_STATE_COMPLETED;
				timer_unlink(ptmr->timer_list[i],TimerNum);
			}
		}
  }
}

/* initalize timer,1:OK ; 0: fail
 * 创建1个计时器，该操作分配一个软件计时器结构，任何企图安装软件计时器的
 * 的软件模块必须首先创建一个计时器结构。计时器结构包含允许更改计时器处理
 * 设施和超过软件计时器的控制信息。此操作创立的计时器将引用到软件计时器列
 * 表的一个入口
 */
u8 timer_create(timer_t   		*ptimer,	/* point to timer_t */
					 u32  		period,		/* period			*/
					 u32  		opt,
					 tmr_fnct_ptr	pcallback,
					 u8          TimerNum)
{
	if(!ptimer)
		return  0;

	ptimer->expire   = 0;
    ptimer->period   = period;
	ptimer->opt      = opt;
	ptimer->callback = pcallback;
	ptimer->state    = TMR_STATE_STOPPED;
	timer_start(ptimer,TimerNum);
	return 1;
}

/*
 * 删除一个计时器。此操作删除先前创立的软件计时器，释放计时器结构占据的内存。
 * 1：成功；0：失败
 */
u8 timer_del(timer_t *ptimer)
{
	if(!ptimer)
		return 0;
	timer_cleanup(ptimer);
	ptimer = NULL;

	return 1;
}

/*
 * 启动一个计时器。该操作安装一个先前创立的软件计时器到计时器处理设施中
 * 计时器在此操作完成后开始运行
 */
u8 timer_start(timer_t *ptimer,u8 TimerNum)
{
	if(!ptimer)
			return 0;

	switch(ptimer->state)
	{
		case TMR_STATE_RUNNING:
				timer_unlink(ptimer,TimerNum);
				timer_link(ptimer,TimerNum);
				return 1;
		case TMR_STATE_STOPPED:
		case TMR_STATE_COMPLETED:
				timer_link(ptimer,TimerNum);
				return 1;
		case TMR_STATE_UNUSED:
				return 0;
		default:
				return 0;
	}
}

/* 取消当前运行的计时。此操作通过从计时器处理设施中删除当前运行的计时器，
 * 取消一个计时器,把timer_id指向的列表项设置为NULL
 */
u8 timer_cancel(timer_t *ptimer,u8 TimerNum)
{
    if(!ptimer)
        return 0;

   return timer_unlink(ptimer,TimerNum);
}

/* static function implement **************************************/
static void timer_cleanup(timer_t *ptimer)
{
	if(!ptimer)
		return;
	ptimer->state    = TMR_STATE_UNUSED;
	ptimer->callback = NULL;
	ptimer->expire   = 0;
	ptimer->opt      = 0;
}

static u8 timer_link(timer_t *ptimer,u8 TimerNum)
{
	u8 i;
	timer_struct *ptmr = NULL;
	ptmr=&Usertimer[TimerNum];
	if(!ptimer)
		return 0;

	if(ptmr->timer_count >= TIMER_LIST_MAX)
		return 0;

	for(i=0;i<TIMER_LIST_MAX;i++)
	{
		if(ptmr->timer_list[i] != NULL)
        continue;
		ptimer->state  = TMR_STATE_RUNNING;
		ptimer->expire = ptimer->period + ptmr->timer_tick;
		ptmr->timer_list[i]  = ptimer;
		ptmr->timer_count++;
		return 1;
	}

    return 0;
}

static u8 timer_unlink(timer_t *ptimer,u8 TimerNum)
{
	u8 i;
	timer_struct *ptmr = NULL;
	ptmr=&Usertimer[TimerNum];
	if(!ptimer)
			return 0;
	for(i=0;i<TIMER_LIST_MAX;i++)
	{
			if(ptmr->timer_list[i] != ptimer)
					continue;
			ptmr->timer_list[i] = NULL;
			ptimer->state = TMR_STATE_STOPPED;
			ptmr->timer_count--;
			return 1;
	}
	return 0;
}


static void Timer0_Task0(void)   //1ms
{
 //   NRF24L01_TASK1MS();
    CAN_TASK1ms();
}

static void Timer0_Task1(void)   //100ms
{
	  
}

static void Timer0_Task2(void)   //2ms
{
	
}

static void Timer0_Task3(void)   //2ms
{
	
}

static void Timer0_Task4(void)   //2ms
{
	  
}


static void Timer1_Task0(void)    //1s
{
     _Comm_TASK1000ms();
     CAN_TASK1000ms();    
}

static void Timer1_Task1(void)    //5ms
{
//	  _CodeModule_TASK5MS();
	  _Comm_TASK5ms();
	   
}
static void Timer1_Task2(void)    //100ms
{   
//    NRF24L01_TASK100MS();	  
//	  _SenserFilter_Task();
}

static void Timer1_Task3(void)    //1ms
{
	  _Comm_TASK1ms();
//	  _CodeModule_TASK1MS();
}

static void Timer1_Task4(void)    //250ms
{
    _Comm_TASK250ms();
    _CAN_TASK250ms();//CAN通信
    _Show_TASK250ms();
}

static void Timer1_Task5(void)    //10ms
{
    _Comm_TASK10ms();
    _CAN_TASK10ms();
	_LED_TASK10MS();
	  
}


