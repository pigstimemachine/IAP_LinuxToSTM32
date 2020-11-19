/*
 * TaskMgr.h
 *
 *  Created on: 2017年2月10日
 *      Author: Administrator
 */

#ifndef TASKMGR_H_
#define TASKMGR_H_
#include "sys.h"

#define ERR           1
#define NOERR         0

#define Unit_Msec     0
#define Unit_Sec      1
#define Unit_Min      2
#define Unit_Hour     3
#define Unit_Day      4


typedef void   (*tmr_fnct_ptr) (void);
typedef u8 TMR_STATE;
#define TIMER_LIST_MAX		    6U		/* 计时器列表大小，支持5个软件定时器 */
#define TIMER_MAX             3

#define Dir_WR                0
#define Dir_RD                1

#define timer0                0
#define timer1                1

/* timer state */
#define TMR_STATE_UNUSED                    (TMR_STATE)(0u)
#define TMR_STATE_STOPPED                   (TMR_STATE)(1u)
#define TMR_STATE_RUNNING                   (TMR_STATE)(2u)
#define TMR_STATE_COMPLETED                 (TMR_STATE)(3u)


#define  OPT_TMR_NONE                        (0u)  /* No option selected                                 */


#define  OPT_TMR_ONE_SHOT                    (1u)  /* Timer will not auto restart when it expires        */
#define  OPT_TMR_PERIODIC                    (2u)  /* Timer will     auto restart when it expires        */

#define  Task_create                         timer_create

typedef struct{
	u32		  expire;				/* 定时器到期时间             */
  u32        period;
	tmr_fnct_ptr  callback;				/* 回调函数					  */
	TMR_STATE	  state;			    /* 定时器状态				  */
	u32		  opt;					/* 操作类型        			  */
}timer_t;

typedef struct
{
	timer_t		*timer_list[TIMER_LIST_MAX];
	u8		timer_count;
	u32      timer_tick;
}timer_struct;

typedef struct
{
	u16 Msecond:10;
	u16 Second:6;
	u32 Minute:6;
	u32 Hour:5;
	u32 Day:11;
	u32 res:10;
}TIME_FORM;

typedef struct
{
	u32 InterruptCount;
}CpuTimer;


void CPU_TimerInit(void);
u8 SysTimer_Init(u8 *TimerNum);
u8 SysTimer_Timeout(u8 *TimerNum,u8 Unit);

void timer_init(void);


u8 timer_create(timer_t   		*ptimer,	/* point to timer_t */
					 u32  		period,		/* period			*/
					 u32  		opt,
					 tmr_fnct_ptr	pcallback,
					 u8          TimerNum);


u8 timer_del(timer_t *ptimer);
u8 timer_start(timer_t *ptimer,u8 TimerNum);
u8 timer_cancel(timer_t *ptimer,u8 TimerNum);

extern  CpuTimer  CpuTimer0;

void timer_task(u32 *CpuTick,u8 TimerNum);
void SysTask_Create(void);
u32  GetSysTick(void);
void GetSysTime(TIME_FORM **Systime);
void SysTimer_wipe(void);
void SysTimer_RecTask(u8 Dir);
void Timer0_IsrHandler(void);
void Timer1_IsrHandler(void);
void Timer2_IsrHandler(void);
#endif /* TASKMGR_H_ */
