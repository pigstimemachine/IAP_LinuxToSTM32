/************************************************************************
* ��Ȩ���� (C)2011, �ɶ��ܹȼ������޹�˾��
* 
* �ļ����ƣ� // DPS_message.h
* �ļ���ʶ�� // ������A-300X  ����ͷ�ļ�
* ����ժҪ�� //
* ����˵���� // 
* ��ǰ�汾�� // Ver1.0
* ��    �ߣ� // 
* ������ڣ� // 
* 
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
* �޸����� ��
* �� �� �� ��1
* �� �� �� ��
* �޸����� �� 
* �޸ļ�¼2����
************************************************************************/
#ifndef __DPS_MESSAGE_H__
#define __DPS_MESSAGE_H__

#include "../pub/msg_queue.h"
#include "../pub/pub.h"
#include "../CAN/can_app.h"

enum data_status
{
	Data_On = 0xAA,	  		//��������
	Data_Err = 0xEE,	  	//���ݴ���
};

struct Index_name
{
	char *readIndex;
	char *writeIndex;
	char *Program_pathname;
	struct msg_queue *Heart_queue;
	struct msg_queue *Scan_queue;
};


struct _Updata_Prgram
{
	unsigned int Checksum;
	unsigned int fileLen;
	enum data_status send_bin_sta;			//��¼����bin�ǵ����ݷ���״̬	
	enum data_status send_bin_apply_sta;	//��¼��������ʱ��������
	enum p_status One_updata_sta;			//��¼�Ƿ�����
	char ProData_check;
	char Program[10];
};

struct _DADDR_Data
{
	enum data_status reset_add_sta;			//	
	enum p_status One_reset_sta;		//��¼�Ƿ�����
	unsigned int DADDR;					//�豸��ַ
	
};


struct _Scan_Data
{
	enum data_status scan_data_sta;			//���յ���ɨ��ǹ�������Ƿ�����
	unsigned int Checksum;						//ɨ��ǹ����У���
	unsigned int ScanDataLen;					//ɨ��ǹ���ݳ���
	char ScanData_check;
	char ScanData_check_last;
	enum p_status recvDataApply;
	char QR_code[1024];
	
};

struct _Send_nbyte_data
{
	int id;
	int len;
	int reg;
	char data[8];
};

struct dps_comm_result
{
	char heart_check;
	char heart_check_last;
	char send_heart;
	enum dev_back back_result ;			//rawcan�����Ƿ�ɹ�
	enum dev_online boot_s;				//can����״̬
	pthread_mutex_t *rawcan_mutex;
	int my_id;							//����ID 
	struct Index_name index;
	struct _Updata_Prgram Updata_Prgram;//���³������
	struct _DADDR_Data DADDR_Data;		//�����豸��ַ���
	struct _Scan_Data Scan_Data;
	struct _Send_nbyte_data Send_nbyte_data;	
};


#define NeedToSendData		0
#define SendDataSUCC		1

#define BroadcastAddr         0x0f

#define MainboardAddr         0x01

#define MainpanelAddr         0x02
#define WirefeederAddr        0x03
#define WirepanelAddr         0x05

#define RobotcommAddr         0x08

#define FCAWcommAddr          0x06
#define ReserveAddr           0x0A

#define CMD_Heartbeat         0x80


#define LocalHostAddr1        0x01
#define LocalHostAddr2        0x0A
#define TargetAddr           0x08
#define ChildLCD	0x03

#define CMD_GetReady	0x01


/********************��ʾ��CMD***********************/
#define CMD_ParaToLCD       0x50//lcd����֡ byte0
#define CMD_ParaToLCD_NAck 0xb0//lcd����֡ ����Ӧ��

#define CMD_Layer          	0x10//����
#define CMD_Voltage			0x11//��ѹ/
#define CMD_Current			0x12//����
#define CMD_Sys_Temp	    0x13//ϵͳ�¶�
#define CMD_Sys_Hmi	        0x14//ϵͳʪ��
#define CMD_Position	    0x15//CW/CCW
#define CMD_SS_Speed		0x16//��˿�ٶ�
#define CMD_HJ_Speed		0x17//�����ٶ�
#define CMD_LayerTemp		0x18//�����¶�

#define CMD_Pipe			0x19//����
#define CMD_PipeApply		0x1a//��������

#define CMD_Welder			0x1b//����//
#define CMD_WelderApply		0x1c//����//����

#define CMD_Process			0x1d//���չ��
#define CMD_ProcessApply	0x1e//���չ������

#define	CMD_DeviceID		0x1f//�豸���
#define	CMD_DeviceIDApply	0x20//�豸�������

#define CMD_DataBoxID		0x21//���ݺб��
#define CMD_DataBoxIDApply	0x22//���ݺб������

#define CMD_Time	        0x23//time
#define CMD_TimeApply	    0x24//time����

#define CMD_TeamCode	    0x25//������
#define CMD_TeamCodeApply	0x26//����������

#define CMD_Warning			0x27//����
#define CMD_WarningApply	0x28//��������

#define CMD_HJ_Process		0x29//���ӹ���
#define CMD_HJ_ProcessApply	0x2a//���ӹ�������

#define CMD_ProjectID		0x2b//��Ŀ����
#define CMD_ProjectIDApply	0x2c//��Ŀ��������


#define CMD_SingalStrength		0x2d//�ź�ǿ��
#define CMD_SingalStrengthApply	0x2e//�ź�ǿ������

#define CMD_ICCID		0x2f//ICCID
#define CMD_ICCIDApply	0x30//

#define CMD_CPSI		0x31//
#define CMD_CPSIApply	0x32//

#define CMD_LBSx		0x33//
#define CMD_LBSxApply	0x34//

#define CMD_LBSy		0x35//
#define CMD_LBSyApply	0x36//i


#define CMD_IMEI		0x37//
#define CMD_IMEIApply	0x38//

#define CMD_VersionSM		0x39//
#define CMD_VersionSMApply	0x3a//

#define CMD_VersionAS		0x3b//
#define CMD_VersionASApply	0x3c//

#define CMD_ScanSts		0x3d//ɨ���־

/**************END******��ʾ��CMD***********************/



#define CMD_Tempdata          0x95
#define CMD_Angledata         0x94
#define CMD_CodeApply         0x30
#define CMD_CodeData          0x31
#define CMD_BinApply          0x40
#define CMD_BinData           0x41
#define CMD_RESET             0x42

#define CMD_SendAck			0x81
#define ACK_SUCC       0xaa
#define ACK_ERR        0xee
#define SEND_OK        0x01
#define SEND_ERR       0x02


#define bool int
#define true 1
#define false 0




//#define Uint8     u8
//#define Uint16    u16

//#define int16     short

//#define uint8_t u8
//#define uint16_t u16
//#define int 	u32
#define Readok               0
#define Readerr              1

#define NRead                0x55

typedef   unsigned int u32; 

#define MaxDatalen           100
#define MaxErrNum            10
#define NoSend			1//δ����״̬
#define MaxCanbyte 5


typedef struct
{
	u8   CTSflag;
	long Datasum;
	u32  Framecnt;
	u32  Datalen;
	u32  Datacnt;
	long Tmpsum;
	u8   SendSts;
}TRANSMIT_CTRL;
typedef struct {
	u8   CodeData[MaxDatalen];
	u16  DLC;
	u8   Readflag;
}DataBufType;

#define HOSTtoLayerAddr			0x453////*(u32*)&TransmitID3;
#define LayerDevToHostAddr		0x543
#define MaxOfflinecnt     5

typedef struct tagECANID_INTERNAL
{
	unsigned short reserved:3;	//0b000//���λ
	unsigned short source:4;	//Source address
	unsigned short target:4;	//Target address
}ECANID_INTERNAL;

typedef union
{
	ECANID_INTERNAL S_TransmitID ;
	canid_t  I_TransmitID;//u32 
}ECANID;
//const ECANID_INTERNAL ReceiveSM100ID={0x03,0x0a,0x08};


/*
id:   ���ñ�׼֡, id���� 11bit�� MSB [ 4bitĿ���ַ | 4bitԴ��ַ | 3bit���� ] LSB  ��ʼ��ַΪ���λ ����ַΪ���λ
			typedef struct tagECANID_INTERNAL
			{
				unsigned short reserved:3;	//0b000
				unsigned short source:4;	//Source address
				unsigned short target:4;	//Target address
			}ECANID_INTERNAL;
*/


typedef struct
{
   u8 CANTxReady;
   u8 CANRxReady;
   u8 CANWaitAck;
   u8 OnLineSta;
	 u8 OffLineCnt;
   u8 MemSyncSign;
}CAN_STATUS;


#define CAN_FRAME_WIDTH      8//32�ֽڵ��û����ݿ��



#define ABUF_NO_ERR               0

#define ABUF_FRAMEFORMATS_ERR     1

#define ABUF_TXBUFFULL_ERR        2

#define ABUF_CHARSTRING_ERR       3

#define ABUF_ArrayLength_ERR      4

#define ABUF_RXBUFLESS_ERR        5

#define RX_NO_ERR               0
#define TX_NO_ERR               0
#define TX_BUFFULL              1
#define RX_BUFLESS              1

#define RX_NO

#define TX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define FRAME_WIDTH     32  	  //32�ֽڵ��û����ݿ��

#define MaxFifoNum      100 
#define CountPriod      100


#define Uint8     u8
#define Uint16    u16
#define Uint32    u32
#define int16     short

typedef struct {
	canid_t 			 MSGID;//Uint16 
	Uint8				 DLC;
	__u8	             FrameData[CAN_FRAME_WIDTH] __attribute__((aligned(8)));
}CANBufType;

/*
struct can_frame {
	canid_t can_id;  // 32 bit CAN_ID + EFF/RTR/ERR flags 
	__u8    can_dlc; // data length code: 0 .. 8 
	__u8    data[8] __attribute__((aligned(8)));
};
*/


#define STA_ON         1
#define STA_OFF        0

#define CanArrayBufSize      100
typedef struct {
	CANBufType	           ArrayBuf[CanArrayBufSize+1];
	CANBufType             *ArrayBufInPtr;
	CANBufType             *ArrayBufOutPtr;
	int16			       ArrayBufCtr;
}CANArrayBufType;

/*
extern TRANSMIT_CTRL    CAN_CodeTrans,CAN_BinTrans,CAN_CodeTrans_Pipe,CAN_CodeTrans_Welder,CAN_CodeTrans_ProjectID,
						CAN_CodeTrans_TeamCode,CAN_CodeTrans_Process,CAN_CodeTrans_EquipCode,CAN_CodeTrans_BoxID,
						CAN_CodeTrans_HjProcess,CAN_CodeTrans_Warning,CAN_CodeTrans_Time;
*/
void *thread_read_from_dps(void * arg);


void *thread_for_dps_heart(void * arg);


void *thread_for_dps_Scanner(void * arg);


int init_thread_DPS(void);

int Updata_Program(struct dps_comm_result *result);


int SEND_DADDR_to_Rawcan(struct dps_comm_result *result);
void *thread_for_send_data(void * arg);

int  can_rcv(struct can_frame *can_orgdata);

int CAN_SEND_LAYER(struct _AS4_comm_result *as4_comm_result);


void * Send_Can_Layer(void *arg);
int init_thread_CanSendLayer(void);
void * RCV_Can_Layer(void *arg);
int init_thread_CanRcvLayer(void);


u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);

//void TASK_CAN_AutoSend(void);
u8 CanReceiveProcess(struct _AS4_comm_result *as4_comm_result );


u8 Can_Tx_Msg(CANBufType buffer);
void Can_Rx_Msg(struct can_frame *can_orgdata , CANBufType* buffer );
void _SEND_ACK(void);

void InitArrayBuf(void);
void *  CAN_TASK1000ms (void *arg);
int SendLayerDataSts( struct _AS4_comm_result *as4_comm_result );
void *  CAN_TASK10ms (void *arg);

u8 ReadySendCanSts(TRANSMIT_CTRL * CAN_Trans);


void * thread_for_Update_2814_bin(void * arg);

#endif






