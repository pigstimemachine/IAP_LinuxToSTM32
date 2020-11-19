
/*
20201111
bootloader 5k   app 60k  bin 60k
STM32C8T6 SIZE 64KB  RAM 20KB字节

20201106 
    增加焊接过程 无法按动焊层按键的功能
    优化界面布局，使得焊层更易被发现。

int Date_20200522(void)
{
	目标地址0xa 源地址0x08 3位识别帧0x01
    初始化 主机发送焊层指令，使本机焊层初始化为主机已存储的焊层，标志位置1
    主机发送焊层和焊层符号如（1，"RW" ，max_layer）,
    本设备接收到后，显示“RW”，
    按下按键增加焊层到2 ，3 等 ，在max_layer内叠加，
    主机接收到后，改变焊层和显示的值
    生成随机序列号匹配主机，主机不对，标志位置0
}

20200515
CAN通信发送互相发送数据 通信失败则报错
数码管显示焊层 以及报错相关信息
按键增加或者减少焊层，发送给数据采集盒

通过CAN发送数据下载


//立焊层 单独的按键，按下后单独的焊层，在焊层数据里面又没有显示。

上的脚本中首先需要计算GPIO的编号，比如需要采用PC(23)，那么C组是第三组那么可以利用公式


其中num是GPIO的编号，n是第几组gpio，m是当前的gpio的序号。经过计算PC23的GPIO编号为87。

所以当执行
*/


#include "lib.h"
//#include	"CH454CMD.H"
#include "oled.h"
#include "led.h"
//#include "font.h" 
#include "pic.h"
#include "ShowInterface.h"
int main(void)
{		
    Stm32_Clock_Init(9);	//系统时钟设置
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH,ADDR_APP_OFFSET);
	delay_init(72);
    uart_init(72,9600);	
    TIM2_Int_Init(9,7199);
    CAN_Mode_Init(1,8,7,9,0);
    EXTIX_Init();
	SysTask_Create();
    _SEND_GetReady(); 
    LED_Init();
    OLED_Init();
 //   LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	LCD_ShowPicture(40,90,240,46,gImage_black);
	delay_ms(65535);  
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	Interface.page = 1;//PageMain;
	//_UAPP_Comm_INIT();

	
	while(1)
	{
		
	  Timer1_IsrHandler();
	}	
}




