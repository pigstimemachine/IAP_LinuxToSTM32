#ifndef _CAN_H
#define _CAN_H
#include "lib.h"

#define CAN_FRAME_WIDTH      8//32字节的用户数据宽度



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

#define TX_ADR_WIDTH    5   	//5字节的地址宽度
#define RX_ADR_WIDTH    5   	//5字节的地址宽度
#define FRAME_WIDTH     32  	  //32字节的用户数据宽度

#define MaxFifoNum      100 
#define CountPriod      100



typedef struct {
	Uint8	             	  FrameData[CAN_FRAME_WIDTH];
	Uint8                 DLC;
	Uint16                MSGID;
}CANBufType;


#define CanArrayBufSize      100
typedef struct {
	CANBufType	           ArrayBuf[CanArrayBufSize+1];
	CANBufType             *ArrayBufInPtr;
	CANBufType             *ArrayBufOutPtr;
	int16			       ArrayBufCtr;
}CANArrayBufType;





typedef struct tagECANID_INTERNAL
{
	unsigned short reserved:3;	//0b000
	unsigned short source:4;	//Source address
	unsigned short target:4;	//Target address
}ECANID_INTERNAL;


#define CMD_GetReady           0x01
#define CMD_WeldingSW          0x02
#define CMD_WFCtrl             0x02
#define CMD_GFCtrl             0x03
#define CMD_CycStatusCheack    0x04
#define CMD_WeldingStatusCheck 0x05
#define CMD_WalkStatusCheck    0x06
#define CMD_HorStatusCheck     0x07
#define CMD_VtcStatusCheck     0x08
#define CMD_OscStatusCheck     0x09
#define CMD_WeldParameter      0x12
#define CMD_TrailCheck         0x83


#define CMD_WeldingPara       0x0f
#define CMD_Heartbeat         0x80
#define CMD_SendAck           0x81
#define CMD_PowerOn           0x83
#define CMD_PwmStart          0x90
#define CMD_ModeSwitch        0x91
#define CMD_MemStore          0x92
#define CMD_MemRecall         0x93
#define CMD_PwmStop           0x94
#define CMD_FeedBack          0x97
#define CMD_SettingPara       0x98
#define CMD_SendParaAck       0x99
#define CMD_ErrorAlert        0xa0
#define CMD_AutoTrack         0x9a
#define CMD_SwingCtrl         0x9b
#define CMD_EdgeSign          0x9d
#define CMD_Recall            0x1f

#define CMD_Tempdata          0x95
#define CMD_Angledata         0x94
#define CMD_CodeApply         0x30
#define CMD_CodeData          0x31
#define CMD_BinApply          0x40
#define CMD_BinData           0x41
#define CMD_RESET             0x42


//#define CMD_IRTEMP			0X4B//焊层红外温度
/********************************************************/
#define CMD_ParaToLCD       0x50//lcd参数帧 byte0
#define CMD_ParaToLCD_NAck 0xb0//lcd参数帧 不需应答

#define CMD_Layer          	0x10//焊层
#define CMD_Voltage			0x11//电压/
#define CMD_Current			0x12//电流
#define CMD_Sys_Temp	    0x13//系统温度
#define CMD_Sys_Hmi	        0x14//系统湿度
#define CMD_Position	    0x15//CW/CCW
#define CMD_SS_Speed		0x16//送丝速度
#define CMD_HJ_Speed		0x17//焊接速度
#define CMD_LayerTemp		0x18//焊层温度

#define CMD_Pipe			0x19//焊口
#define CMD_PipeApply		0x1a//焊口请求

#define CMD_Welder			0x1b//焊工//
#define CMD_WelderApply		0x1c//焊工//请求

#define CMD_Process			0x1d//工艺规程
#define CMD_ProcessApply	0x1e//工艺规程请求

#define	CMD_DeviceID		0x1f//设备编号
#define	CMD_DeviceIDApply	0x20//设备编号请求

#define CMD_DataBoxID		0x21//数据盒编号
#define CMD_DataBoxIDApply	0x22//数据盒编号请求

#define CMD_Time	        0x23//time
#define CMD_TimeApply	    0x24//time请求

#define CMD_TeamCode	    0x25//机组编号
#define CMD_TeamCodeApply	0x26//机组编号请求

#define CMD_Warning			0x27//报警
#define CMD_WarningApply	0x28//报警请求

#define CMD_HJ_Process		0x29//焊接工艺
#define CMD_HJ_ProcessApply	0x2a//焊接工艺请求

#define CMD_ProjectID		0x2b//项目编码
#define CMD_ProjectIDApply	0x2c//项目编码请求


#define CMD_SingalStrength		0x2d//信号强度
#define CMD_SingalStrengthApply	0x2e//信号强度请求

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

#define CMD_ScanSts		0x3d//扫码标志
/********************************************************/


#define WireSpeed             0x53
#define TrvSpeed              0xc2

#define RX_MAX_NUM            255
#define TX_MAX_NUM            255

#define WFswitchOn            0x5a
#define WFswitchOff           0xa5
#define WFstatusOn            0x55
#define WFstatusOff           0x4a
#define WFswitch(x)           (x==0?WFswitchOff:WFswitchOn)
#define WFstatus(x)           (x==0?WFstatusOff:WFstatusOn)

#define BroadcastAddr         0x0f

#define MainboardAddr         0x01

#define MainpanelAddr         0x02
#define WirefeederAddr        0x03
#define WirepanelAddr         0x05

#define RobotcommAddr         0x08

#define FCAWcommAddr          0x06
#define ReserveAddr           0x0A



#define LocalHostAddr1        0x01
#define LocalHostAddr2        0x08
#define TargetAddr           0x0A
#define ChildLCD	0x03
#define ChildLocalHostAddr2 0x01


#define HOSTtoLayerAddr			0x453////*(u32*)&TransmitID3; 0x03 0x0a 0x08


extern const ECANID_INTERNAL  TransmitID0;
extern const ECANID_INTERNAL  TransmitID1;
extern const ECANID_INTERNAL  TransmitID2;
extern const ECANID_INTERNAL  TransmitID3;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
u8 Can_Tx_Msg(CANBufType buffer);
void Can_Rx_Msg(u8 fifox,CANBufType* buffer);
void Can_sendMsg(u16 CID,u8*BYTE,u8 len);

u8 Can_RxMsg(CANBufType *RxFrame);
u8 Can_TxMsg(CANBufType *TxFrame);
void TASK_CAN_AutoSend(void);

void InitArrayBuf(void);
#endif





