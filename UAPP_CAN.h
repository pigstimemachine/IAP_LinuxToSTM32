#ifndef UAPP_CAN_H
#define UAPP_CAN_H
#include "sys.h"
typedef struct
{
   u8 count;
   u8 status;
   u8 workmode;
   u8 error;
   u8 stage;
   u8 speedfeedback;	
}HEARTBEAT_DATA;

typedef struct
{
   u16 WeldingCurrent;
   u16 WeldingVoltage;
   u16 PAR1;	
}FEEDBACK_DATA;


typedef struct
{
   u8 CANTxReady;
   u8 CANRxReady;
   u8 CANWaitAck;
   u8 OnLineSta;
	 u8 OffLineCnt;
   u8 MemSyncSign;
}CAN_STATUS;

#define Readok               0
#define Readerr              1

#define NRead                0x55



#define MaxDatalen           100
#define MaxErrNum            10

typedef struct {
	u8   CodeData[MaxDatalen];
	u8   His_CodeData[MaxDatalen];
	u16  DLC;
	u8   Readflag;
}DataBufType;


typedef struct 
{
	int layer;
	int max_layer;
	char layer_str[5];    
	char id;
	int rcv_sts;
    int send_sts;
    long cnt;
    int onlinests;
}CAN_LAYER;


#if !TRANSMIT_FLAG  
#define TRANSMIT_FLAG         1

//typedef struct
//{
//	u8   CTSflag;
//	u32  Datasum;
//	u32  Framecnt;
//	u32  Datalen;
//	u32  Datacnt;
//	long long Tmpsum;
//}TRANSMIT_CTRL;

#endif
typedef struct
{
	u8   CTSflag;
	long Datasum;
	u32  Framecnt;
	u32  Datalen;
	u32  Datacnt;
	long Tmpsum;
}TRANSMIT_CTRL;
struct XGGPS_DATA{
	float x;
	float y;
};


/*******SignalStrength*******
5格:大于 -85 dbm
4格: -85   dbm 至 -90  dBm
3格: -90   dbm 至 -95  dBm
2格: -95   dbm 至 -100 dBm
1格: -100 dbm 至 -105 dBm
脱网:小于 -105 dbm
**************/
struct XGDevice_data{
	int SignalStrength;//dBm
	char SignalSrth[5];
	char CICCID[30];
	char CPSI[100];
	char FrontCPSI[100];
	char MiddleCPSI[100];
	struct XGGPS_DATA GPS;//GPS位置，数值需要除100，得到经纬度
    struct XGGPS_DATA LBS;//LBS位置
	char IMEI[20];
};

struct _AS4_Final_SendData
{
	char PROJECT_ID[30];	//项目编码
	char TEAM_CODE[20];		//机组编号
	char PROCESS[20];		//工艺规程
	char EQUIP_CODE[20];	//设备编号
	char DATASBOX_CODE[20];	//数据盒编号
	char HJ_PROCESS[20];	//焊接工艺
	char PERSON_CODE[60];	//人员编号
	char PERSON_CODE_Front[60];	//人员编号换行前段
	char WELD_CODE[60];		//焊口编号
	char WELD_CODE_Front[60];		//焊口编号前段
	char POSITION[10];		//焊接位置
	char LAYER[10];			//焊层
	char CURRENT[20];		//焊接电流
	char VOLTAGE[20];		//焊枪电压
	char SS_SPEED[20];		//送丝速度
	char TEMP[20];			//环境温度	
	char HUMIDITY[20];		//环境湿度	
	char HJ_SPEED[20];		//焊接速度	
	int LayerTemp;		//焊层温度	
	char workExcep[40];		//工作异常	
	char CJ_TIME[30];		//时间	
	struct XGDevice_data DeviceData;
    char VerSM[40];
    char VerAS[40];
    int  welding_flag;
	int Current_num;
};
#define OnlineMinCur	10
  enum 
{
	Welding_off,
	Welding_on,
};


#define MaxCanbyte            5  

#define ResetKey              0xabcdef

#define FAIL_COMM             130


#define NeedToSendData		1
#define SendDataSUCC		0

extern CAN_LAYER can_layer;
extern struct _AS4_Final_SendData AS4Data;
extern struct _AS4_Final_SendData His_AS4Data;


extern DataBufType DecodeData,CAN_DataPipe,CAN_DataWelder,CAN_DataProjectID,CAN_DataTeamCode,CAN_DataProcess,CAN_DataEquipCode,
						CAN_DataBoxID,CAN_DataHjProcess,CAN_DataWarning,CAN_DataTime,
						CAN_DataSingalStrength,CAN_DataICCID,CAN_DataCPSI,CAN_DataLBSx,CAN_DataLBSy,CAN_DataIMEI,
                        CAN_DataVersionSM,CAN_DataVersionAS;

extern TRANSMIT_CTRL       CAN_CodeTrans,CAN_BinTrans,CAN_CodeTrans_Pipe,CAN_CodeTrans_Welder,CAN_CodeTrans_ProjectID,
					CAN_CodeTrans_TeamCode,CAN_CodeTrans_Process,CAN_CodeTrans_EquipCode,CAN_CodeTrans_BoxID,
					CAN_CodeTrans_HjProcess,CAN_CodeTrans_Warning,CAN_CodeTrans_Time,
					 CAN_CodeTrans_SignalStrength,CAN_CodeTrans_ICCID,CAN_CodeTrans_CPSI,CAN_CodeTrans_LBS,
                    CAN_CodeTrans_LBSx,CAN_CodeTrans_LBSy,CAN_CodeTrans_IMEI,
                     CAN_CodeTrans_VersionSM,CAN_CodeTrans_VersionAS;

void CAN_TASK1ms(void);  //1000HZ
void CAN_TASK1000ms(void);  //1HZ
void _CAN_TASK10ms(void); //100HZ
void _CAN_TASK250ms(void);

void _SEND_LayerFeedback(void);
int send_layer_data(void);
void _SEND_GetReady(void);
void _SensorVal_Angle(u8* Databuf);
u8 _UAPP_Comm_INIT(void);

#endif





