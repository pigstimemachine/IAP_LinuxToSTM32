#include "lib.h"
#include	"CH454CMD.H"
extern UINT8 mBCD[19];

u8 TmpRxData[MAX_DataWidth];
u8 TmpTxData[MAX_DataWidth];
CONN_CTRL ConnCtrl;
UPDATA_CTRL    UpdataCtrl;
TRANSMIT_CTRL  BinTransCtrl;
// DataBufType* Codedata;
 u8 MEM_Databuf[MAX_PARA_NUM*2];
extern ALPHABET alphabet[ALPHABET_NUM];
//void _CommWatchDog_Feed(void);
void _Timer_Set(u32* Timer,u32 nms);

void _COMM_STA_INIT(void)
{
   ConnCtrl.CommINIT=1;
}	

/*
void _SYSPARA_INIT(void)
{
   u16 INIT_FLAG=0; u32 INIT_DEVID=123456789;
	 INIT_FLAG=INIT_OK;
	 ConnCtrl.Addr=0x00;
	 ConnCtrl.Freq=100;
	 ConnCtrl.Cidnum=1;
	 memset(MEM_Databuf,0,sizeof(MEM_Databuf));
	 memcpy(MEM_Databuf+ADDR_INIT,&INIT_FLAG,2);
	 memcpy(MEM_Databuf+ADDR_ADDR,&ConnCtrl.Addr,1);
	 memcpy(MEM_Databuf+ADDR_FREQ,&ConnCtrl.Freq,1);
	 memcpy(MEM_Databuf+ADDR_Cidnum,&ConnCtrl.Cidnum,1);
	 memcpy(MEM_Databuf+ADDR_DEVID,&INIT_DEVID,4);
	 STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
}
*/
/*
u8 _UAPP_Comm_INIT(void)
{
	 u16 INIT_FLAG=0;
   memset(&ConnCtrl,0,sizeof(ConnCtrl));//从机地址频率
	 memset(&UpdataCtrl,0,sizeof(UpdataCtrl));//接收到更新
	 memset(&CodeTransCtrl,0,sizeof(CodeTransCtrl));
	 memset(&BinTransCtrl,0,sizeof(BinTransCtrl));
	 //FLASH读取各项参数 Cidnum,Addr,Freq,ValidDev,UpdataCtrl.Datalen,UpdataCtrl.Checksum 
   STMFLASH_Read(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
	 memcpy(&INIT_FLAG,MEM_Databuf+ADDR_INIT,2);
	 memcpy(&ConnCtrl.Freq,MEM_Databuf+ADDR_FREQ,1);
	 memcpy(&ConnCtrl.ValidDev,MEM_Databuf+ADDR_ValidDev,1);
	 memcpy(&ConnCtrl.Cidnum,MEM_Databuf+ADDR_Cidnum,1);
	 ConnCtrl.Addr=0x00;
	 if(INIT_FLAG==INIT_OK)
	 {	 

	 }
	 else
	 {
      _SYSPARA_INIT();
	 }	 	
	 NRF24L01_ModifyRxAddr(ConnCtrl.Addr);
	 NRF24L01_ModifyDevID(MEM_Databuf+ADDR_DEVID);
	 NRF24L01_ModifyChannel(ConnCtrl.Freq);
	 NRF24L01_Init();
   return 0;	
}
*/
u16 _ChecksumTask(u8* buf,u8 len)
{
	 u8 cnt; u32 Checksum=0x10000;
   for(cnt=0;cnt<len;cnt++)
	 {
	    Checksum-=buf[cnt];
	 }
	 return Checksum;
}

u8 _FrameChecksum(u8 *Frame)
{
	 u8 DLC; u32 Checksum,Countsum;
   DLC=Frame[0];
	 memcpy(&Checksum,&Frame[DLC],2);
	 Countsum=_ChecksumTask(Frame,DLC);
	 if(Checksum==Countsum)
	     return 1;
   else
       return 0;		 
}

/*
void _SEND_ACK(u8 Cmd,u8 Para,u8 Taraddr)
{
   u8 TmpDbuf[32]={0},cnt,len=0x05; u32 Checksum; 
	 TmpDbuf[0]=len;
	 TmpDbuf[1]=NRF24L01_ReadSourAddr();
	 TmpDbuf[2]=RFCMD_SendAck;
	 TmpDbuf[3]=Cmd;
	 TmpDbuf[4]=Para;
	 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
	 memcpy(&TmpDbuf[len],&Checksum,2);
	 NRF24L01_TxWithArray(TmpDbuf,Taraddr); 
}	
*/

/*
void _SEND_ReqConn(u8 Taraddr)
{
   u8 TmpDbuf[32]={0},cnt,len=0x03; u32 Checksum;
	 if((ConnCtrl.CommINIT==1)&&(ConnCtrl.OnLineSta==STA_OFF))
	 {	 
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_ReqConn;
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_TxWithArray(TmpDbuf,Taraddr);
	 }	 
}	
*/
/*
void _SEND_HartBeat(u8 Taraddr)
{
   u8 TmpDbuf[32]={0},cnt,len=0x07; u32 Checksum;
	 ConnCtrl.TimerS++;
	 if((ConnCtrl.OnLineSta==STA_ON)&&(ConnCtrl.DataTransMode==0))
	 {	 
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_Heartbeat;
		 TmpDbuf[3]=ConnCtrl.TimerS&0xff;
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_TxWithArray(TmpDbuf,Taraddr);
	 }	 
}	
*/

/*
void _SET_DevID(u8* DevID,u8 Taraddr)
{
   u8 TmpDbuf[32]={0},cnt,len=0x0d; u32 Checksum;
	 if((ConnCtrl.OnLineSta==STA_ON)&&(ConnCtrl.DataTransMode==0))
	 {	 
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_SetDevID;
		 memcpy(&TmpDbuf[3],DevID,4);
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_TxWithArray(TmpDbuf,Taraddr);
	 }	 
}	
*/
/*
void _SET_CommFreq(u8 Channel,u8 Taraddr)
{
   u8 TmpDbuf[32]={0},cnt,len=0x04; u32 Checksum;
	 if((ConnCtrl.OnLineSta==STA_ON)&&(ConnCtrl.DataTransMode==0))
	 {	 
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_SetCommFreq;
		 TmpDbuf[3]=Channel;
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_TxWithArray(TmpDbuf,Taraddr);
	 }	 
}	
*/

/*
void _SEND_SensorValue(short Temp1,short Temp2,short AngleX,short AngleY,short AngleZ,u8 Taraddr)
{
   u8 TmpDbuf[32]={0},cnt,len=0x0d; u32 Checksum;
	 if((ConnCtrl.OnLineSta==STA_ON)&&(ConnCtrl.DataTransMode==0))
	 {	 
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_SensorVal;//0x83
		 memcpy(&TmpDbuf[3],&Temp1,2);
		 memcpy(&TmpDbuf[5],&Temp2,2);
		 memcpy(&TmpDbuf[7],&AngleX,2);
		 memcpy(&TmpDbuf[9],&AngleY,2);
		 memcpy(&TmpDbuf[11],&AngleZ,2);
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_TxWithArray(TmpDbuf,Taraddr);
	 }	 
}	
*/


/*
static void _SEND_CodeApply(u32 Datalen,u32 Datasum,u8 Taraddr,u8 ReuseCnt)
{
   u8 TmpDbuf[32]={0},cnt,len=0x0a; u32 Checksum;
	 if((ConnCtrl.OnLineSta==STA_ON)&&(ConnCtrl.DataTransMode!=Mode_BinTrans))
	 {	 
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_SendCode;//0x34
		 memcpy(&TmpDbuf[3],&Datalen,3);
		 memcpy(&TmpDbuf[6],&Datasum,4);
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_ModifyTxAddr(Taraddr);
		 for(cnt=0;cnt<ReuseCnt;cnt++)
		 {		 
		     if(NRF24L01_TxPacket(TmpDbuf,MaxTout)==TX_OK) break;			 
		 } 
	 }    
}

*/

/*
static u8 _SEND_CodeData(u32 Datalen,u8* Dbuf,u32 DataCnt,u8 Taraddr,u8 ReuseCnt)
{
   u8 TmpDbuf[32]={0},cnt,len=Datalen+4; u32 Checksum;
	 if((ConnCtrl.OnLineSta==STA_ON)&&(ConnCtrl.DataTransMode!=Mode_BinTrans))
	 {	 
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_SendCodeData;//0x35
		 TmpDbuf[3]=DataCnt&0xff;		 
		 memcpy(&TmpDbuf[4],Dbuf,Datalen);
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_ModifyTxAddr(Taraddr);
		 for(cnt=0;cnt<ReuseCnt;cnt++)
		 {
		     if(NRF24L01_TxPacket(TmpDbuf,MaxTout)==TX_OK) return 1;			 
		 } 
		 //NRF24L01_TxWithArray(TmpDbuf,Taraddr);
		 
	 }	
	 return 0; 
}	
*/

/*
u8 _CodeData_Task(void)
{
	 u8 Tempcnt; short Sendbyte; long Lastbyte;
	 Lastbyte=Codedata->DLC-CodeTransCtrl.Datacnt;
	 if(Lastbyte<=MaxDatabyte)
	   Sendbyte=Lastbyte;
	 else 
		 Sendbyte=MaxDatabyte;
   if(ConnCtrl.DataTransMode==Mode_CodeTrans)
	 {
		  _Timer_Set(&ConnCtrl.TimerS3,TransDelayms);
		  _CommWatchDog_Feed();
		  if(CodeTransCtrl.Datacnt<CodeTransCtrl.Datalen)
			{	
					Tempcnt=CodeTransCtrl.Framecnt&0xff;
					if(_SEND_CodeData(Sendbyte,Codedata->CodeData+CodeTransCtrl.Datacnt,Tempcnt,Hostaddr,MaxRetryCnt)==1)//
					{	
					   CodeTransCtrl.Framecnt++; 
					   CodeTransCtrl.Datacnt+=Sendbyte;
					   return 0;
					}
          else 
          {
						 memset(&CodeTransCtrl,0,sizeof(CodeTransCtrl));
					   ConnCtrl.DataTransMode=0;
						 return SEND_ERR;
					}						
			}	
			else if(CodeTransCtrl.Datacnt==CodeTransCtrl.Datalen)
			{
				  memset(&CodeTransCtrl,0,sizeof(CodeTransCtrl));
				  ConnCtrl.DataTransMode=0;
			    return SEND_OK;
			}						
	 }		 
	 return SEND_ERR;
}	

*/

/*
u8 _GET_SysPara(DataBufType* data)
{
	 long Tmpnum,Tmppara; char Tmpbuf[10],TmpDLC;//&D&
   if((data->CodeData[0]=='&')&&(data->CodeData[(data->DLC-1)]=='&'))
	 {
		  if(data->CodeData[3]!='D') return 0;
          memcpy(&Tmpnum,&data->CodeData[1],2);
		  Tmpnum=atol((char*)&Tmpnum);//函数名： atol 功 能： 把字符串转换成长整型数 用 法： long atol(const char *nptr);
		  TmpDLC=(data->DLC-5);
		  if(TmpDLC>sizeof(Tmpbuf)) return 1;
		  memcpy(Tmpbuf,&data->CodeData[4],TmpDLC);
		  Tmppara=atol(Tmpbuf);
		  switch(Tmpnum)
			{
				 case SYS_DEVID://0X01
				    NRF24L01_ModifyDevID((u8*)&Tmppara);
				   //调用存储函数                 
				    memcpy(MEM_Databuf+ADDR_DEVID,(u8*)&Tmppara,4);
				    STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
//				    _LED_SetTime(2,1000,1000,3000);
				   break;
				 case SYS_INIT://0x02
					 if(strstr((char*)data->CodeData,PARA_INIT)==(char*)&data->CodeData[4])
					 {
						  //初始化系统参数
						  _SYSPARA_INIT();
//					    _LED_SetTime(2,1000,1000,3000);
					 }	 
				   break;
			}	
			return 1;
	 }	
   return 0;	 
}	
*/
/*
void _CodeApply_Task(void)
{
	 u16 cnt; long Checksum1=0x10000000;
	 if(ConnCtrl.DataTransMode==0)
	 {	 
		 if(_ReadDecodeData(&Codedata)==Readok)
		 {
				if(_GET_SysPara(Codedata)==0)
				{	
					for(cnt=0;cnt<Codedata->DLC;cnt++)
					{
						 Checksum1-=Codedata->CodeData[cnt];
					}
					_SEND_CodeApply(Codedata->DLC,(u32)Checksum1,Hostaddr,MaxRetryCnt);
					memset(&CodeTransCtrl,0,sizeof(CodeTransCtrl));
					CodeTransCtrl.Datalen=Codedata->DLC;
					ConnCtrl.DataTransMode=Mode_CodeWait;		
					_Timer_Set(&ConnCtrl.TimerS3,TransDelayms);				
				}			
		 }	 
   }
}

*/
/*
static u8 _Rx_BinApply(u8* buf)
{
	 u32 TmpDatlen,TmpDatsum; u16 UPDATA_FLAG; 
	 //读取FLASH 比对len和sum，如果不相同则返回SUCC
	 memcpy(&TmpDatlen,MEM_Databuf+ADDR_BinLen,4);
	 memcpy(&TmpDatsum,MEM_Databuf+ADDR_BinSum,4);
	 memset(&BinTransCtrl,0,sizeof(BinTransCtrl));
	 memcpy(&(BinTransCtrl.Datalen),&buf[3],3);
	 memcpy(&(BinTransCtrl.Datasum),&buf[6],4);
	 BinTransCtrl.Tmpsum=0x10000000;
	 if(((TmpDatlen==BinTransCtrl.Datalen)&&(TmpDatsum==BinTransCtrl.Datasum))||(BinTransCtrl.Datalen>MAX_BIN_NUM))
   {		 
		  memset(&BinTransCtrl,0,sizeof(BinTransCtrl));
		  delay_ms(10);
	    return ACK_ERR;
	 }	 
	 else
	 {
		  _Timer_Set(&ConnCtrl.TimerS3,TransDelayms);
		  ConnCtrl.DataTransMode=Mode_BinTrans;
            STMFLASH_Erase(STM32_FLASH_BASE+ADDR_BIN_OFFSET,MAX_BIN_NUM/2);
//		  _LED_SetTime(1,100,100,0); 
		  delay_ms(10);
	    return ACK_SUCC;
	 }
}
*/
/*
static u8 _Rx_BinData(u8* buf)
{
	 u8 DLC,cnt,Tmpcnt; u32 AddrOffset; u16 UPDATA_FLAG; 
	 DLC=buf[0];
	 Tmpcnt=buf[3];
	 if((BinTransCtrl.Framecnt&0xff)==Tmpcnt)
	 {	 
		 _Timer_Set(&ConnCtrl.TimerS3,TransDelayms);
	   _CommWatchDog_Feed();
		 for(cnt=4;cnt<DLC;cnt++)
		 {
				BinTransCtrl.Tmpsum-=buf[cnt];
		 } 
		 //向FLASH写入BIN数据
		 DLC-=4;
		 AddrOffset=STM32_FLASH_BASE+ADDR_BIN_OFFSET+BinTransCtrl.Datacnt;
		 STMFLASH_WriteNoErase(AddrOffset,(u16*)&buf[4],(DLC/2));
		 BinTransCtrl.Datacnt+=DLC;
		 if(BinTransCtrl.Datacnt>=BinTransCtrl.Datalen)
		 {
		    if(BinTransCtrl.Tmpsum==BinTransCtrl.Datasum)
				{	
					 //向FLASH写入新的Datasum,Datalen,和程序升级标志
					 UPDATA_FLAG=UPDATA_OK;
					 memcpy(MEM_Databuf+ADDR_BinLen,&(BinTransCtrl.Datalen),4);
					 memcpy(MEM_Databuf+ADDR_BinSum,&(BinTransCtrl.Datasum),4);
					 memcpy(MEM_Databuf+ADDR_Updata,&(UPDATA_FLAG),2);
					 STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
					 ConnCtrl.DataTransMode=0;
//					 _LED_SetTime(2,500,500,3000);
				   return ACK_SUCC;
				}	
        else 
				{	
					 ConnCtrl.DataTransMode=0;
           return ACK_ERR;	
        }					
		 }
		 BinTransCtrl.Framecnt++;
     return ACK_SUCC;	 
		 
	 }
   return ACK_ERR;	 
}


*/

/*
u8 _RX_SourAddr(u8 Addr)
{
   NRF24L01_ModifyRxAddr(Addr);
	 ConnCtrl.Addr=Addr;
	 memcpy(MEM_Databuf+ADDR_ADDR,&ConnCtrl.Addr,1);
	 STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
	 return ACK_SUCC;
}
*/
/*
void _FrameAnalysis(void)
{
    u8 Addr,CMD,CMDack,AckResult,Ackflag=0;
    if(NRF24L01_RxWithArray(TmpRxData)==RX_NO_ERR)
	  {
	    if(_FrameChecksum(TmpRxData))
			{
         CMD=TmpRxData[2];
				 Addr=TmpRxData[1];
				 switch(CMD)
				 {
					  case RFCMD_Heartbeat://0x80心跳帧
//							 _CommWatchDog_Feed();
//                             _LED_SetTime(0,200,500,700);
						   break;
						case RFCMD_SendAck://0x81
							 CMDack=TmpRxData[3];
                            AckResult=TmpRxData[4];
						   switch(CMDack)
							 {
								  case RFCMD_ReqConn://0x01 接收到主机发送的应答，则从机上线
										 ConnCtrl.OnLineSta=STA_ON;
//                                        _LED_SetTime(1,60000,0,0);       
									   break;
									case RFCMD_SetCommFreq://0x33
										
									   break;
									case RFCMD_SendCode://0x34
									   ConnCtrl.DataTransMode=Mode_CodeTrans;
									   break;
									case RFCMD_SendCodeData://0x35
//										 CodeTransCtrl.CTSflag=AckResult;											 
									   break;
							 }	 
						   break;
						case RFCMD_SendBin://0x40
							 Ackflag=_Rx_BinApply(TmpRxData);
						   break;
						case RFCMD_SendBinData://0x41
							 _Rx_BinData(TmpRxData);
						   break;
						case RFCMD_SetSourAddr://0x42
							 Ackflag=_RX_SourAddr(TmpRxData[3]);
						   break;
						
				 }	
				 if(Ackflag!=0)
            _SEND_ACK(CMD,Ackflag,Hostaddr);				 
			}	
	 }
}	
*/
void _Timer_Set(u32* Timer,u32 nms)
{
   *Timer=CpuTimerms+nms;
}	

void _Timer_Task(void)
{
   if((ConnCtrl.TimerS3<CpuTimerms)&&(ConnCtrl.TimerS3!=0))
	 {
       ConnCtrl.DataTransMode=0;  
		   ConnCtrl.TimerS3=0;
		   //_LED_SetTime(1,60000,0,0);
	 }	 	 
}	


u8 _Read_OnlineSta(void)
{
    return ConnCtrl.OnLineSta;
}


void _CommWatchDog_TASK(void)
{
		ConnCtrl.OfflineCnt++;
		if(ConnCtrl.OfflineCnt>MaxOfflinecnt)
		{
			 ConnCtrl.OfflineCnt=0;
			 ConnCtrl.OnLineSta=STA_OFF;
			 //_LED_SetTime(1,500,500,0);
		}	 
}	

static void _CommWatchDog_Feed(void)
{
    ConnCtrl.OfflineCnt=0;	 
}	


/////////////COMM_TASK//////////////////////////////////



void _Comm_TASK1000ms(void)  //1HZ
{
//   _SEND_HartBeat(Hostaddr);//发送心跳帧
//	 _CommWatchDog_TASK();//未上线则连续快速闪烁
//	 _SEND_ReqConn(Hostaddr);//从机发送请求上线指令。

    //ch454_write( CH454_TWINKLE | 0x07 );    

   //发送显示命令

}	

void _Comm_TASK1ms(void) //1000HZ
{
//	 _FrameAnalysis(); 
    
}	

void _Comm_TASK10ms(void) //100HZ
{
   _Timer_Task();
}	

void _Comm_TASK100ms(void) //10HZ
{
    
 
}	

void _Comm_TASK250ms(void) //4HZ
{
//	 SENSER_VAL SenserData;
//	 _READ_SenserData(&SenserData);//读取传感器中电流电压温度角度的值
  // _SEND_SensorValue(SenserData.Temp1,SenserData.TempA,SenserData.AngleX,SenserData.AngleY,SenserData.AngleZ,Hostaddr);//发送温度角度到数据板
  // _CodeApply_Task();
}	

void _Comm_TASK5ms(void)
{
/*    
     TASK_NRF24L01_AutoSend();
	 if(_CodeData_Task()==SEND_OK)
	 {
	    _LED_SetTime(2,100,100,3000);//接收到扫码成功的回传
	 }
*/	
}	
