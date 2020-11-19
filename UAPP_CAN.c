#include "lib.h"
//#include	"CH454CMD.H"
#include "oled.h"
#include "ShowInterface.h"
#include "pic.h"
#include "can.h"
extern CONN_CTRL ConnCtrl;
HEARTBEAT_DATA      HeartBeatData;
FEEDBACK_DATA       FeedBackData;
CAN_STATUS          CanStatus;
ECANID_INTERNAL     ReceiveID;
DataBufType*        Codedata;
DataBufType DecodeData,CAN_DataPipe,CAN_DataWelder,CAN_DataProjectID,CAN_DataTeamCode,CAN_DataProcess,CAN_DataEquipCode,
            CAN_DataBoxID,CAN_DataHjProcess,CAN_DataWarning,CAN_DataTime,
            CAN_DataSingalStrength,CAN_DataICCID,CAN_DataCPSI,CAN_DataLBSx,CAN_DataLBSy,CAN_DataIMEI,
            CAN_DataVersionSM,CAN_DataVersionAS;

TRANSMIT_CTRL       CAN_CodeTrans,CAN_BinTrans,CAN_CodeTrans_Pipe,CAN_CodeTrans_Welder,CAN_CodeTrans_ProjectID,
                    CAN_CodeTrans_TeamCode,CAN_CodeTrans_Process,CAN_CodeTrans_EquipCode,CAN_CodeTrans_BoxID,
                    CAN_CodeTrans_HjProcess,CAN_CodeTrans_Warning,CAN_CodeTrans_Time,
                    CAN_CodeTrans_SignalStrength,CAN_CodeTrans_ICCID,CAN_CodeTrans_CPSI,CAN_CodeTrans_LBS,
                    CAN_CodeTrans_LBSx,CAN_CodeTrans_LBSy,CAN_CodeTrans_IMEI,
                    CAN_CodeTrans_VersionSM,CAN_CodeTrans_VersionAS;

static void _CommWatchDog_TASK(void);
static void _CommWatchDog_Feed(void);
#define ADDR_SEL(x) (x==LocalHostAddr1? ADDR_BIN1_OFFSET:ADDR_BIN2_OFFSET)
#define ERR_COMM(x) (x>0? 0:FAIL_COMM)


CAN_LAYER can_layer= {0};
struct _AS4_Final_SendData AS4Data= {0};
struct _AS4_Final_SendData His_AS4Data= {0};

u8 _Read_CanOnlineSta(void)
{
    return CanStatus.OnLineSta;
}

static void _SEND_HartBeat(ECANID_INTERNAL* CID)
{
    short Onlinenum=_Read_OnlineSta();
    CANBufType TxFrame;
    u32 SYStick;
    SYStick=GetSysTick();
    memset(TxFrame.FrameData,0,sizeof(TxFrame.FrameData));
    TxFrame.MSGID=*(Uint16*)CID;
    TxFrame.FrameData[0]=CMD_Heartbeat;
    TxFrame.FrameData[1]=(SYStick>>10)&0xff;
    TxFrame.FrameData[4]=ERR_COMM(Onlinenum);
    TxFrame.DLC=8;
    Can_TxMsg(&TxFrame);
}

void _SEND_CodeApply(u32 Datalen,u32 Datasum)
{
    if(CanStatus.OnLineSta==STA_ON)
    {
        CANBufType TxFrame;
        TxFrame.MSGID=*(Uint16*)&TransmitID0;
        TxFrame.FrameData[0]=CMD_CodeApply;
        memcpy(&TxFrame.FrameData[1],&Datalen,3);
        memcpy(&TxFrame.FrameData[4],&Datasum,4);
        TxFrame.DLC=8;
        Can_TxMsg(&TxFrame);
    }
}

void _SEND_CodeData(u32 Datalen,u8* Dbuf,u32 DataCnt)
{
    if(CanStatus.OnLineSta==STA_ON)
    {
        CANBufType TxFrame;
        TxFrame.MSGID=*(Uint16*)&TransmitID0;
        TxFrame.FrameData[0]=CMD_CodeData;
        TxFrame.FrameData[1]=DataCnt&0xff;
        memcpy(&TxFrame.FrameData[2],Dbuf,Datalen);
        TxFrame.DLC=(Datalen+2)&0xff;
        Can_TxMsg(&TxFrame);
    }
}

/*
void _CodeApply_Task(void)//发送扫码请求
{
	 u16 cnt; long Checksum=0x10000000;
   if(_ReadDecodeData(&Codedata)==Readok)
	 {
			for(cnt=0;cnt<Codedata->DLC;cnt++)
			{
				 Checksum-=Codedata->CodeData[cnt];
			}
			_SEND_CodeApply(Codedata->DLC,Checksum);
			memset(&CAN_CodeTrans,0,sizeof(CAN_CodeTrans));
			CAN_CodeTrans.Datalen=Codedata->DLC;
	 }
}
*/

/*
static u8 _CodeData_Task(void)
{
	 u8 Tempcnt; short Sendbyte; long Lastbyte;
	 Lastbyte=Codedata->DLC-CAN_CodeTrans.Datacnt;
	 if(Lastbyte<=MaxCanbyte)
	   Sendbyte=Lastbyte;
	 else
		 Sendbyte=MaxCanbyte;
   if(CAN_CodeTrans.CTSflag==ACK_SUCC)
	 {
		  CAN_CodeTrans.CTSflag=0;
		  if(CAN_CodeTrans.Datacnt<CAN_CodeTrans.Datalen)
			{
					Tempcnt=CAN_CodeTrans.Framecnt&0xff;
					_SEND_CodeData(Sendbyte,Codedata->CodeData+CAN_CodeTrans.Datacnt,Tempcnt);
					CAN_CodeTrans.Framecnt++;
				  CAN_CodeTrans.Datacnt+=Sendbyte;
				  return 0;
			}
			else if(CAN_CodeTrans.Datacnt==CAN_CodeTrans.Datalen)
			{
			    return SEND_OK;
			}
	 }
   else if(CAN_CodeTrans.CTSflag==ACK_ERR)
	 {
	    memset(&CAN_CodeTrans,0,sizeof(CAN_CodeTrans));
	 }
	 return SEND_ERR;
}
*/

/*
u8 _Rx_ResetDevid(u8* buf)
{
   u32 TmpKey=0;
	 memcpy(&TmpKey,buf+5,3);
	 if(TmpKey==ResetKey)
	 {
	    NRF24L01_ModifyDevID(buf+1);
		  memcpy(MEM_Databuf+ADDR_DEVID,buf+1,(TX_ADR_WIDTH-1));
		  STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
		  return ACK_SUCC;
	 }
	 return ACK_ERR;
}
*/
void offline_sts()
{
    //  can_layer.onlinests = 1;
    can_layer.layer = 0 ;
    _SEND_GetReady();
}

static void _CommWatchDog_TASK(void)
{
    CanStatus.OffLineCnt++;
    if(CanStatus.OffLineCnt>MaxOfflinecnt)
    {
        CanStatus.OnLineSta=STA_OFF;

        //  CharToNixieTube(NoCanConnect);
        //   ch454_write( CH454_TWINKLE | 0x07 );
        memset( &can_layer, 0, sizeof(can_layer));
    }
    if( can_layer.rcv_sts == 0 )
    {
        can_layer.layer = 0 ;
        _SEND_GetReady();
    }
}

static  void _CommWatchDog_Feed()
{
//  ch454_write( CH454_TWINKLE );
    CanStatus.OffLineCnt=0;
    CanStatus.OnLineSta=STA_ON;
    // CharToNixieTube(can_layer.layer_str);

}

static void _SEND_ACK(u8 CMD,u8 flag,ECANID_INTERNAL* CID)
{
    u8 Tmpid;
    Tmpid=CID->source;
    CID->source=CID->target;
    CID->target=Tmpid;
    if(CanStatus.OnLineSta==STA_ON)
    {
        CANBufType TxFrame;
        TxFrame.MSGID=*(Uint16*)CID;
        TxFrame.FrameData[0]=CMD_SendAck;
        TxFrame.FrameData[1]=CMD;
        TxFrame.FrameData[2]=flag;
        TxFrame.DLC=3;
        Can_TxMsg(&TxFrame);
    }
}
static void _SEND_LCD_ACK(u8 CMD,u8 CMDPara,u8 flag,ECANID_INTERNAL* CID)
{
    u8 Tmpid;
    Tmpid=CID->source;
    CID->source=CID->target;
    CID->target=Tmpid;
    if(CanStatus.OnLineSta==STA_ON)
    {
        CANBufType TxFrame;
        TxFrame.MSGID=*(Uint16*)CID;
        TxFrame.FrameData[0]=CMD_SendAck;
        TxFrame.FrameData[1]=CMD;
        TxFrame.FrameData[2]=CMDPara;
        TxFrame.FrameData[3]=flag;
        TxFrame.DLC=4;
        Can_TxMsg(&TxFrame);
    }
}

/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	_SEND_LayerFeedback 发送焊层参数到数据盒
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/18 Create
  * @Version 	1.0 2020/05/18
  * @Note

  **/
void _SEND_LayerFeedback(void)
{
    if(  can_layer.layer != 0  )
    {
        CANBufType TxFrame;
        TxFrame.MSGID=*(Uint16*)&TransmitID3;
        TxFrame.FrameData[0]=CMD_ParaToLCD;
        TxFrame.FrameData[1]=CMD_Layer;
        TxFrame.FrameData[2]=can_layer.layer;
        TxFrame.FrameData[3]=can_layer.max_layer;
        memcpy(&TxFrame.FrameData[4],&can_layer.layer_str, 4 );
        TxFrame.DLC=8;
        Can_TxMsg(&TxFrame);
    }


}

int send_layer_data()
{
    if((can_layer.send_sts == NeedToSendData)  )
    {
        _SEND_LayerFeedback();
    }
    return 0;
}

int send_data_succ()
{
    can_layer.send_sts = SendDataSUCC;
    return 0;
}
int can_rcv_layer(CANBufType*  RxData)
{
    can_layer.layer = RxData->FrameData[2];
    can_layer.max_layer =  RxData->FrameData[3] ;
    memcpy(&can_layer.layer_str, &RxData->FrameData[4], 4);

    // CharToNixieTube(can_layer.layer_str);
    can_layer.rcv_sts = 1;
    return 0;
}
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 温度接收
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
int can_rcv_temp(CANBufType*  RxData)
{
    memcpy(&AS4Data.TEMP, &RxData->FrameData[2], 6);

    return 0;
}
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 湿度接收
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/24  Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
int can_rcv_hmi(CANBufType*  RxData)
{
    memcpy(&AS4Data.HUMIDITY, &RxData->FrameData[2], 6);

    return 0;
}
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz position
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/24  Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
int can_rcv_position(CANBufType*  RxData)
{
    memcpy(&AS4Data.POSITION, &RxData->FrameData[2], 6);

    return 0;
}

/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz voltage
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/24  Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
int can_rcv_voltage(CANBufType*  RxData)
{
    memcpy(&AS4Data.VOLTAGE, &RxData->FrameData[2], 6);

    return 0;
}
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz current
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/24 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/

int can_rcv_current(CANBufType*  RxData)
{
    memcpy(&AS4Data.CURRENT, &RxData->FrameData[2], 6);
    AS4Data.Current_num = atoi(AS4Data.CURRENT);
    if( AS4Data.Current_num > OnlineMinCur )
    {
        AS4Data.welding_flag = Welding_on;
    }
    else
    {
        AS4Data.welding_flag = Welding_off;
    }
    return 0;
}

/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz SS_Speed
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/25 Create
  * @Version 	1.0 2020/08/25
  *		     	1.1 2020/08/25 change sth
  * @Note

  **/
int can_rcv_SS_Speed(CANBufType*  RxData)
{
    memcpy(&AS4Data.SS_SPEED, &RxData->FrameData[2], 6);

    return 0;
}
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz HJ_Speed
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/25 Create
  * @Version 	1.0 2020/08/25
  *		     	1.1 2020/08/25 change sth
  * @Note

  **/
int can_rcv_HJ_Speed(CANBufType*  RxData)
{
    memcpy(&AS4Data.HJ_SPEED, &RxData->FrameData[2], 6);

    return 0;
}


/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz LayerTemp
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/25 Create
  * @Version 	1.0 2020/08/25
  *		     	1.1 2020/08/25 change sth
  * @Note

  **/
int can_rcv_LayerTemp(CANBufType*  RxData)
{
    AS4Data.LayerTemp =(int)RxData->FrameData[2] ;

    return 0;
}
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	扫码枪识别是否有新数据
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/09/27 Create
  * @Version 	1.0 2020/09/27
  *		     	1.1 2020/09/27 change sth
  * @Note
#define CMD_ScanSts		0x3d//扫码标志

byte0 CMD_ParaToLCD_NAck
byte1 CMD_ScanSts
byte2 ScanSts
byte3 IsNewData
  **/
int can_rcv_ScanData(CANBufType*  RxData)
{
    if( RxData->FrameData[2] == 0xaa )
    {
        if( RxData->FrameData[3] == 1 )
        {
            LCD_ShowString(35,160,"AAAAAAAAAAAAAAAAA",WHITE,WHITE,16,0);
            LCD_ShowString(35,160,"RcvNewScanData",RED,WHITE,16,0);
            //LCD_ShowPicture(40,90,120,126,gImage_scansucc);
        }
        else
        {
            LCD_ShowString(35,160,"AAAAAAAAAAAAAAAAA",WHITE,WHITE,16,0);
            LCD_ShowString(35,160,"RcvScanData",RED,WHITE,16,0);
            //LCD_ShowPicture(40,90,120,126,gImage_scansucc);
        }
    }

    return 0;
}

void _SEND_GetReady(void)
{
    CANBufType TxFrame;
    TxFrame.MSGID=*(Uint16*)&TransmitID3;
    TxFrame.FrameData[0]=CMD_GetReady;
    TxFrame.DLC=2;
    Can_TxMsg(&TxFrame);
}


u8 _ReadDecodeData(DataBufType** Codedata)
{
    if(DecodeData.Readflag==NRead)
    {
        *Codedata=&DecodeData;
        DecodeData.Readflag=0;
        return Readok;
    }
    return Readerr;
}

void _WRITEDecodeData(u8* Databuf,u8 BufCtr,u8 DLC,u8 Readflag,DataBufType *Data)
{
    if(Readflag!=NRead)
    {
        memcpy(&Data->CodeData[BufCtr],Databuf,DLC);
    }
    else
    {
        Data->Readflag=Readflag;
        Data->DLC=DLC;
    }
}

u8 _Rx_CodeApply(u8* buf,u8 Addr,TRANSMIT_CTRL * CAN_Trans)
{
    memset(CAN_Trans,0,sizeof(TRANSMIT_CTRL));
    memcpy(&(CAN_Trans->Datalen),&buf[2],2);
    memcpy(&(CAN_Trans->Datasum),&buf[4],4);
    CAN_Trans->Tmpsum=0x10000000;
    // ConnCtrl.DataTransMode=Mode_CodeTrans;
    // _Timer_Set(&ConnCtrl.TimerS3,TransDelayms);
    delay_ms(10);
    return ACK_SUCC;
}
//u8 _Rx_CodeData(u8* buf,u8 Addr)
u8 _Rx_CodeData (CANBufType* buf,u8 Addr,TRANSMIT_CTRL * CAN_Trans,DataBufType *DataBuf)
{
    u8 DLC,cnt,Tmpcnt;
    DLC=buf->DLC;
    Tmpcnt=buf->FrameData[2];
    // _Timer_Set(&ConnCtrl.TimerS3,TransDelayms);
    // _CommWatchDog_Feed(Addr);
    if((CAN_Trans->Framecnt&0xff)==Tmpcnt)
    {
        for(cnt=CAN_FRAME_WIDTH-MaxCanbyte; cnt<DLC; cnt++)
        {
            CAN_Trans->Tmpsum-=buf->FrameData[cnt];
        }
        DLC-=CAN_FRAME_WIDTH-MaxCanbyte;
        _WRITEDecodeData(&buf->FrameData[3],CAN_Trans->Datacnt,DLC,0,DataBuf);
        CAN_Trans->Datacnt+=DLC;
        if(CAN_Trans->Datacnt>=CAN_Trans->Datalen)
        {
            if(CAN_Trans->Tmpsum==CAN_Trans->Datasum)
            {
                _WRITEDecodeData(&buf->FrameData[3],CAN_Trans->Datacnt,CAN_Trans->Datacnt,NRead,DataBuf);
                //发送完所有
                return ACK_SUCC;
            }
            else
            {
                //memset(CAN_Trans,0,sizeof(TRANSMIT_CTRL));
                return ACK_ERR;
            }
        }
        CAN_Trans->Framecnt++;
        return ACK_SUCC;
    }
    //memset(CAN_Trans,0,sizeof(TRANSMIT_CTRL));
    return ACK_ERR;
}

void _SYSPARA_INIT(void)
{
    u16 INIT_FLAG=0;
    u32 INIT_DEVID=123456789;
    INIT_FLAG=INIT_OK;
    ConnCtrl.Addr=0x00;
    ConnCtrl.Freq=100;
    ConnCtrl.Cidnum=1;
    memset(MEM_Databuf,0, sizeof(MEM_Databuf)  );//sizeof(MEM_Databuf)  MAX_PARA_NUM*2
    memcpy(MEM_Databuf+ADDR_INIT,&INIT_FLAG,2);
    /*
    memcpy(MEM_Databuf+ADDR_ADDR,&ConnCtrl.Addr,1);
    memcpy(MEM_Databuf+ADDR_FREQ,&ConnCtrl.Freq,1);
    memcpy(MEM_Databuf+ADDR_Cidnum,&ConnCtrl.Cidnum,1);
    memcpy(MEM_Databuf+ADDR_DEVID,&INIT_DEVID,4);*/
    STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
}

u8 _UAPP_Comm_INIT(void)
{
    u16 INIT_FLAG=0;
    STMFLASH_Read(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
    memcpy(&INIT_FLAG,MEM_Databuf+ADDR_INIT,2);

    if(INIT_FLAG==INIT_OK)
    {

    }
    else
    {
        _SYSPARA_INIT();
    }
    return 0;
}



u8 _Rx_BinApply(u8* buf)//,u8 cid
{
    u32 TmpDatlen,TmpDatsum;
    STMFLASH_Read(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
    memcpy(&TmpDatlen,MEM_Databuf+ADDR_BinLen,4);
    memcpy(&TmpDatsum,MEM_Databuf+ADDR_BinSum,4);
    memset(&CAN_BinTrans,0,sizeof(CAN_BinTrans));
    memcpy(&(CAN_BinTrans.Datalen),&buf[1],3);
    memcpy(&(CAN_BinTrans.Datasum),&buf[4],4);
    CAN_BinTrans.Tmpsum=0x10000000;
    if(((TmpDatlen==CAN_BinTrans.Datalen)&&(TmpDatsum==CAN_BinTrans.Datasum))||(CAN_BinTrans.Datalen>MAX_BIN1_NUM))
    {
        memset(&CAN_BinTrans,0,sizeof(CAN_BinTrans));
        return ACK_ERR;
    }
    else
    {
        // _LED_SetTime(1,30,30,0);

        STMFLASH_Erase(STM32_FLASH_BASE+ADDR_BIN1_OFFSET,MAX_BIN1_NUM/2);
        return ACK_SUCC;
    }
}


u8 _Rx_BinData(u8* buf,u8 DLC)
{
    u8 cnt,Tmpcnt;
    u32 AddrOffset;
    u16 UpdateFlag=0;
    Tmpcnt=buf[1];
    if((CAN_BinTrans.Framecnt&0xff)==Tmpcnt)
    {
        for(cnt=2; cnt<DLC; cnt++)
        {
            CAN_BinTrans.Tmpsum-=buf[cnt];
        }
        DLC-=2;
        AddrOffset=STM32_FLASH_BASE+ADDR_BIN1_OFFSET+CAN_BinTrans.Datacnt;
        STMFLASH_WriteNoErase(AddrOffset,(u16*)&buf[2],DLC/2);
        CAN_BinTrans.Datacnt+=DLC;
        if(CAN_BinTrans.Datacnt>=CAN_BinTrans.Datalen)
        {
            if(CAN_BinTrans.Tmpsum==CAN_BinTrans.Datasum)
            {
                //向FLASH写入新的Datasum,Datalen,和程序升级标志
                UpdateFlag=UPDATA_OK;
                memcpy(MEM_Databuf+ADDR_BinLen1,&CAN_BinTrans.Datalen,4);
                memcpy(MEM_Databuf+ADDR_BinSum1,&CAN_BinTrans.Datasum,4);
                memcpy(MEM_Databuf+ADDR_Updata1,&UpdateFlag,2);

                //STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
                STMFLASH_Erase(STM32_FLASH_BASE+ADDR_PARA_OFFSET,MAX_PARA_NUM);
                STMFLASH_WriteNoErase(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
                //_LED_SetTime(2,100,100,3000);
                return ACK_SUCC;
            }
            else
                return ACK_ERR;
        }
        CAN_BinTrans.Framecnt++;
        return ACK_SUCC;

    }
    return ACK_ERR;
}

u8 CanReceiveProcess(void)
{
    u8 Ackflag=0,AckLcdflag=0;
    ECANID_INTERNAL CID;
    u8 Addr;
    CANBufType  CANRxData;
    memset(&CANRxData,0,sizeof(CANRxData));
    if(Can_RxMsg(&CANRxData)!=RX_NO_ERR)
        return 0;
    memcpy(&CID,&CANRxData.MSGID,2);
    Addr=CANRxData.FrameData[1];
    switch(CANRxData.FrameData[0])
    {
    case CMD_SendAck://0x81
        switch(CANRxData.FrameData[1])
        {
        case RFCMD_ReqConn://0x01 接收到主机发送的应答，则从机上线
            ConnCtrl.OnLineSta=STA_ON;
            //  _LED_SetTime(1,60000,0,0);
            break;
//				  case CMD_CodeApply://0x30应答发送扫码数据申请
//						CAN_CodeTrans.CTSflag=CANRxData.FrameData[2];
//					  break;
//					case CMD_CodeData://应答发送扫码数据
//						CAN_CodeTrans.CTSflag=CANRxData.FrameData[2];
//					  break;
        case CMD_ParaToLCD:
            switch(CANRxData.FrameData[2])
            {
            case CMD_Layer:
                send_data_succ();
                break;
            }
            break;

        }
        break;
    case CMD_GetReady://0x01

        break;
    case CMD_WeldingStatusCheck://0x05

        break;
    case CMD_Heartbeat://0x80//
        /*
                 0x07C0 2810ID     0000 01111 1000 000?
                 0x07D0 4GboardID  0000 01111 1010 000?
                 0X0788 1068-4     0000 01111 0001 000?
                 0x00d0            0000 00001 1010 000
                 0X0450            0000 01000 1010 001
        */
        //   LedProcess(CANRxData.MSGID);
        _CommWatchDog_Feed();
        memcpy(&HeartBeatData,&CANRxData.FrameData[1],CANRxData.DLC-1);
        break;

    case CMD_FeedBack://0x97
        memcpy(&FeedBackData,&CANRxData.FrameData[1],CANRxData.DLC-1);
        break;
    case CMD_ErrorAlert://0xa0
        break;

    case CMD_RESET://0x42
        //     Ackflag=_Rx_ResetDevid(CANRxData.FrameData);
        break;

    case CMD_ParaToLCD_NAck :
        switch(CANRxData.FrameData[1])
        {
        case CMD_Sys_Temp:
            can_rcv_temp(&CANRxData);
            break;
        case CMD_Sys_Hmi:
            can_rcv_hmi(&CANRxData);
            break;
        case CMD_Position:
            can_rcv_position(&CANRxData);
            break;
        case CMD_Voltage:
            can_rcv_voltage(&CANRxData);
            break;
        case CMD_Current:
            can_rcv_current(&CANRxData);
            break;
        case CMD_SS_Speed:
            can_rcv_SS_Speed(&CANRxData);
            break;
        case CMD_HJ_Speed:
            can_rcv_HJ_Speed(&CANRxData);
            break;
        case CMD_LayerTemp:
            can_rcv_LayerTemp(&CANRxData);
            break;
        case CMD_ScanSts:
            //can_rcv_ScanData(&CANRxData);
            break;
        }

    /********************LCD显示**********************************/
    case CMD_ParaToLCD:
        switch(CANRxData.FrameData[1])
        {

        case CMD_Layer:
            can_rcv_layer(&CANRxData);
            _SEND_LCD_ACK(CANRxData.FrameData[0],CANRxData.FrameData[1],ACK_SUCC,&CID);
            break;


        case CMD_PipeApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_Pipe);
            break;
        case CMD_Pipe:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_Pipe,&CAN_DataPipe);
            break;

        case CMD_WelderApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_Welder);
            break;
        case CMD_Welder:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_Welder,&CAN_DataWelder);
            break;

        case CMD_ProcessApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_Process);
            break;
        case CMD_Process:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_Process,&CAN_DataProcess);
            break;

        case CMD_DeviceIDApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_EquipCode);
            break;
        case CMD_DeviceID:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_EquipCode,&CAN_DataEquipCode);
            break;

        case CMD_DataBoxIDApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_BoxID);
            break;
        case CMD_DataBoxID:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_BoxID,&CAN_DataBoxID);
            break;

        case CMD_TimeApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_Time);
            break;
        case CMD_Time:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_Time,&CAN_DataTime);
            break;

        case CMD_TeamCodeApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_TeamCode);
            break;
        case CMD_TeamCode:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_TeamCode,&CAN_DataTeamCode);
            break;

        case CMD_WarningApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_Warning);
            //memset(&CAN_DataWarning , 0 ,sizeof(CAN_DataWarning));
            break;
        case CMD_Warning:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_Warning,&CAN_DataWarning);
            if( CAN_CodeTrans_Warning.Datacnt == CAN_CodeTrans_Warning.Datalen )
            {
                memcpy(CAN_DataWarning.His_CodeData,CAN_DataWarning.CodeData,sizeof(CAN_DataWarning.His_CodeData));
                //memcpy( His_AS4Data.workExcep , CAN_DataWarning.His_CodeData , sizeof(His_AS4Data.workExcep));
            }
            break;

        case CMD_HJ_ProcessApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_HjProcess);
            break;
        case CMD_HJ_Process:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_HjProcess,&CAN_DataHjProcess);
            break;

        case CMD_ProjectIDApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_ProjectID);
            break;
        case CMD_ProjectID:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_ProjectID,&CAN_DataProjectID);
            break;
        case CMD_SingalStrengthApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_SignalStrength);
            break;
        case CMD_SingalStrength:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_SignalStrength,&CAN_DataSingalStrength);
            break;

        case CMD_ICCIDApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_ICCID);
            break;
        case CMD_ICCID:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_ICCID,&CAN_DataICCID);
            break;

        case CMD_CPSIApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_CPSI);
            break;
        case CMD_CPSI:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_CPSI,&CAN_DataCPSI);
            break;

        case CMD_LBSxApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_LBSx);
            break;
        case CMD_LBSx:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_LBSx,&CAN_DataLBSx);
            break;

        case CMD_LBSyApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_LBSy);
            break;
        case CMD_LBSy:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_LBSy,&CAN_DataLBSy);
            break;

        case CMD_IMEIApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_IMEI);
            break;
        case CMD_IMEI:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_IMEI,&CAN_DataIMEI);
            break;

        case CMD_VersionSMApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_VersionSM);
            break;
        case CMD_VersionSM:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_VersionSM,&CAN_DataVersionSM);
            break;

        case CMD_VersionASApply://
            AckLcdflag=_Rx_CodeApply(CANRxData.FrameData,Addr,&CAN_CodeTrans_VersionAS);
            break;
        case CMD_VersionAS:
            AckLcdflag = _Rx_CodeData(&CANRxData,Addr,&CAN_CodeTrans_VersionAS,&CAN_DataVersionAS);
            break;
        }
        break;


    case CMD_Angledata:
        _SensorVal_Angle(CANRxData.FrameData);
        break;


    case CMD_BinApply:
        if( CANRxData.MSGID == HOSTtoLayerAddr )
        {
            Ackflag = _Rx_BinApply( CANRxData.FrameData) ;
        }
        break;
    case CMD_BinData:
        if( CANRxData.MSGID == HOSTtoLayerAddr )
        {
            Ackflag = _Rx_BinData(CANRxData.FrameData, CANRxData.DLC);
        }
        break;

    default:
        break;
    }
    if(Ackflag!=0)
    {
        _SEND_ACK(CANRxData.FrameData[0],Ackflag,&CID);
    }
    if(AckLcdflag!=0)
    {
        _SEND_LCD_ACK(CANRxData.FrameData[0],CANRxData.FrameData[1],AckLcdflag,&CID);
    }

    return 1;
}

void CAN_TASK1ms(void)  //1000HZ
{
    CanReceiveProcess();
    TASK_CAN_AutoSend();
    KeyCtl();
    send_layer_data();
    weld_angle();
}

void CAN_TASK1000ms(void)  //1HZ
{
    _CommWatchDog_TASK();
    _SEND_HartBeat((ECANID_INTERNAL*)&TransmitID3);
    _LED_SetTime(2,500,0,1000);
    IsInterfaceChange();
    if( Interface.page == PageMain )
    {
        ShowWarning();
    }


}

void _Show_TASK250ms(void)
{
    WhichInterface();
}

void _CAN_TASK10ms(void) //100HZ
{

    /*
     _CodeApply_Task();
    if(_CodeData_Task()==SEND_OK)
     {
        _LED_SetTime(2,100,100,3000);//接收到扫码成功的回传
     }
    */
}

void _CAN_TASK250ms(void) //3HZ
{
//    _SEND_TempFeedback();//发送温度
//	_SEND_AngleFeedback();//发送角度
}
