
/*
20201111
bootloader 5k   app 60k  bin 60k
STM32C8T6 SIZE 64KB  RAM 20KB�ֽ�

20201106 
    ���Ӻ��ӹ��� �޷��������㰴���Ĺ���
    �Ż����沼�֣�ʹ�ú�����ױ����֡�

int Date_20200522(void)
{
	Ŀ���ַ0xa Դ��ַ0x08 3λʶ��֡0x01
    ��ʼ�� �������ͺ���ָ�ʹ���������ʼ��Ϊ�����Ѵ洢�ĺ��㣬��־λ��1
    �������ͺ���ͺ�������磨1��"RW" ��max_layer��,
    ���豸���յ�����ʾ��RW����
    ���°������Ӻ��㵽2 ��3 �� ����max_layer�ڵ��ӣ�
    �������յ��󣬸ı亸�����ʾ��ֵ
    ����������к�ƥ���������������ԣ���־λ��0
}

20200515
CANͨ�ŷ��ͻ��෢������ ͨ��ʧ���򱨴�
�������ʾ���� �Լ����������Ϣ
�������ӻ��߼��ٺ��㣬���͸����ݲɼ���

ͨ��CAN������������


//������ �����İ��������º󵥶��ĺ��㣬�ں�������������û����ʾ��

�ϵĽű���������Ҫ����GPIO�ı�ţ�������Ҫ����PC(23)����ôC���ǵ�������ô�������ù�ʽ


����num��GPIO�ı�ţ�n�ǵڼ���gpio��m�ǵ�ǰ��gpio����š���������PC23��GPIO���Ϊ87��

���Ե�ִ��
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
    Stm32_Clock_Init(9);	//ϵͳʱ������
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




