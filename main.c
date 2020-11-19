#include "lib.h"

#define MAX_BOOT_NUM        3072//3k
#define MAX_APP_NUM         63488  
#define MAX_BIN1_NUM        MAX_APP_NUM
#define MAX_BIN2_NUM        MAX_APP_NUM

#define MAX_PARA_NUM        64

#define STM32_FLASH_BASE    0x08000000 	//STM32 FLASH的起始地址
#define ADDR_APP_OFFSET     MAX_BOOT_NUM
#define ADDR_BIN1_OFFSET    MAX_BOOT_NUM+MAX_APP_NUM
//#define ADDR_BIN2_OFFSET    MAX_BOOT_NUM+MAX_APP_NUM+MAX_BIN1_NUM
#define ADDR_PARA_OFFSET    MAX_BOOT_NUM+MAX_APP_NUM+MAX_BIN1_NUM

 
#define ADDR_INIT           0x00
#define ADDR_DEVID          0x02
#define ADDR_ADDR           0x06
#define ADDR_FREQ           0x07
#define ADDR_ValidDev       0x08

#define ADDR_BinLen1        0x09
#define ADDR_BinSum1        0x0d
#define ADDR_Updata1        0x11
#define ADDR_BinLen2        0x13
#define ADDR_BinSum2        0x17
#define ADDR_Updata2        0x1b
#define ADDR_Cidnum         0x1d



#define INIT_OK             0xaabb
#define UPDATA_OK           0x7788

#define MaxDatabyte         STM_SECTOR_SIZE
typedef struct
{
    u8  Bindata[MaxDatabyte];
    u32 Datalen;
    u32 Checksum;
    u16 Flag;
} UPDATA_CTRL;

u8 MEM_Databuf[MAX_PARA_NUM*2];
UPDATA_CTRL UpdataCtrl;
typedef  void (*iapfun)(void);				//定义一个函数类型的参数.
iapfun jump2app;


    
long _BIN_Checksum(UPDATA_CTRL* Updata)
{
    long Datacnt=0,Databyte=0,cnt=0;
    long Checksum=0x10000000;
    while(1)
    {
        if(Updata->Datalen-Datacnt>MaxDatabyte)
        {
            Databyte=MaxDatabyte;
        }
        else
        {
            Databyte=Updata->Datalen-Datacnt;
        }

        if(Databyte!=0)
        {
            STMFLASH_Read(STM32_FLASH_BASE+ADDR_BIN1_OFFSET+Datacnt,(u16*)Updata->Bindata,Databyte/2);
            for(cnt=0; cnt<Databyte; cnt++)
            {
                Checksum-=Updata->Bindata[cnt];
            }
            Datacnt+=Databyte;
        }
        else
        {
            return Checksum;
        }
    }

}

/*


*/
u8 _APP_Update_TASK(UPDATA_CTRL* Updata)
{
    long Datacnt=0,Databyte=0;
    while(1)
    {
        if(Updata->Datalen-Datacnt>MaxDatabyte)
        {
            Databyte=MaxDatabyte;
        }
        else
        {
            Databyte=Updata->Datalen-Datacnt;
        }

        if(Databyte!=0)
        {
            STMFLASH_Read(STM32_FLASH_BASE+ADDR_BIN1_OFFSET+Datacnt,(u16*)Updata->Bindata,Databyte/2);
            STMFLASH_WriteNoErase(STM32_FLASH_BASE+ADDR_APP_OFFSET+Datacnt,(u16*)Updata->Bindata,Databyte/2);
            Datacnt+=Databyte;
        }
        else
        {
            return 1;
        }
    }
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
    if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
    {
        jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)
        MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
        jump2app();									//跳转到APP.
    }
}

int main(void)
{
    u16 INIT_FLAG=0;
    long Checksum=0;
    Stm32_Clock_Init(9);	//系统时钟设置
    delay_init(72);
    
    STMFLASH_Read(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
    memcpy(&INIT_FLAG,MEM_Databuf+ADDR_INIT,2);
    memcpy(&UpdataCtrl.Flag,MEM_Databuf+ADDR_Updata1,2);
    memcpy(&UpdataCtrl.Datalen,MEM_Databuf+ADDR_BinLen1,4);
    memcpy(&UpdataCtrl.Checksum,MEM_Databuf+ADDR_BinSum1,4);
    if((UpdataCtrl.Flag==UPDATA_OK)&&(UpdataCtrl.Datalen<MAX_BIN1_NUM))//&&(INIT_FLAG==INIT_OK)
    {
        Checksum=_BIN_Checksum(&UpdataCtrl);
        if(Checksum==UpdataCtrl.Checksum)
        {
            STMFLASH_Erase(STM32_FLASH_BASE+ADDR_APP_OFFSET,MAX_APP_NUM/2);
            _APP_Update_TASK(&UpdataCtrl);
            UpdataCtrl.Flag=0;
            memcpy(MEM_Databuf+ADDR_Updata1,&UpdataCtrl.Flag,2);
            STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
        }
    }
    
//	UpdataCtrl.Flag=UPDATA_OK;
//	INIT_FLAG=INIT_OK;
//	UpdataCtrl.Datalen=20710;
//	memcpy(MEM_Databuf+ADDR_INIT,&INIT_FLAG,2);
//	memcpy(MEM_Databuf+ADDR_Updata,&UpdataCtrl.Flag,2);
//	memcpy(MEM_Databuf+ADDR_BinLen,&UpdataCtrl.Datalen,4);
//	memcpy(MEM_Databuf+ADDR_BinSum,&Checksum,4);
//	STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
    iap_load_app(STM32_FLASH_BASE+ADDR_APP_OFFSET);
    while(1)
    {

    }
}




