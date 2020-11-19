#include "lib.h"

CANArrayBufType CANTxType,CANRxType;

const ECANID_INTERNAL TransmitID0={0,LocalHostAddr1,TargetAddr}; 
const ECANID_INTERNAL TransmitID1={0,LocalHostAddr2,BroadcastAddr};
const ECANID_INTERNAL TransmitID2={0,LocalHostAddr2,LocalHostAddr2};
const ECANID_INTERNAL TransmitID3={ ChildLCD,LocalHostAddr2,TargetAddr };

const ECANID_INTERNAL ReceiveID0={ChildLocalHostAddr2,TargetAddr,LocalHostAddr2};//0x0a 0x08 
const ECANID_INTERNAL ReceiveID1={0,TargetAddr,BroadcastAddr};//0x0a 0x0f
const ECANID_INTERNAL ReceiveID2={0,LocalHostAddr1,BroadcastAddr};//0x01 0x0f

const ECANID_INTERNAL ReceiveSM100ID={0x03,0x0a,0x08};//0x01 0x0f



static void InitArrayBuf(void);

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
		u16 i=0,j=0;
		if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
		tsjw-=1;//�ȼ�ȥ1.����������
		tbs2-=1;
		tbs1-=1;
		brp-=1;
//		RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	 
//		GPIOA->CRH&=0XFFF00FFF; 
//		GPIOA->CRH|=0X000B8000;//PA11 RX,PA12 TX�������   	 
//		GPIOA->ODR|=3<<11;


        RCC->APB2ENR|=1<<0;    //��������ʱ��
        RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��	
        AFIO->MAPR&=0xFFFF9FFF; //1001
        AFIO->MAPR|=0X00004000;//�ر�jtag   0100 	 
		GPIOB->CRH&=0XFFFFFF00; //PB8 RX  PB9 TX
		GPIOB->CRH|=0X000000B8;//PB8 PB9  TX�������   	 
		GPIOB->ODR|=3<<8;
       
		RCC->APB1ENR|=1<<25;//ʹ��CANʱ�� CANʹ�õ���APB1��ʱ��(max:36M)	
				
		CAN->MCR=0x0000;	//�˳�˯��ģʽ(ͬʱ��������λΪ0)
		CAN->MCR|=1<<0;		//����CAN�����ʼ��ģʽ
		while((CAN->MSR&1<<0)==0)
		{
			i++;
			if(i>100)return 2;//�����ʼ��ģʽʧ��
		}
		CAN->MCR|=0<<7;		//��ʱ�䴥��ͨ��ģʽ
		CAN->MCR|=1<<6;		//����Զ����߹���
		CAN->MCR|=0<<5;		//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
		CAN->MCR|=0<<4;		//�������Զ�����
		CAN->MCR|=0<<3;		//���Ĳ�����,�µĸ��Ǿɵ�
		CAN->MCR|=0<<2;		//���ȼ��ɱ��ı�ʶ������
		CAN->BTR=0x00000000;//���ԭ��������.
		CAN->BTR|=mode<<30;	//ģʽ���� 0,��ͨģʽ;1,�ػ�ģʽ;
		CAN->BTR|=tsjw<<24; //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ
		CAN->BTR|=tbs2<<20; //Tbs2=tbs2+1��ʱ�䵥λ
		CAN->BTR|=tbs1<<16;	//Tbs1=tbs1+1��ʱ�䵥λ
		CAN->BTR|=brp<<0;  	//��Ƶϵ��(Fdiv)Ϊbrp+1
							//������:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)

		//��������ʼ��
		CAN->FMR|=1<<0;			                                                //�������鹤���ڳ�ʼ��ģʽ
		CAN->FA1R&=~(1<<0);		                                              //������0������
		CAN->FA1R&=~(1<<1);                                                 //������1������
		CAN->FA1R&=~(1<<2);		                                              //������2������
		CAN->FS1R|=0x07<<0; 		                                            //������0,1,2,3λ��Ϊ32λ.
		CAN->FM1R|=0<<0;		                                                //������0�����ڱ�ʶ������λģʽ
		CAN->FM1R|=0<<1;		                                                //������1�����ڱ�ʶ������λģʽ
		CAN->FM1R|=0<<2;		                                                //������2�����ڱ�ʶ������λģʽ
		CAN->FFA1R|=0<<0;		                                                //������0������FIFO0
		CAN->FFA1R|=0<<1;		                                                //������1������FIFO0
		CAN->FFA1R|=0<<2;		                                                //������2������FIFO0



/*��Ҫ���ε���1������Ҫ���ε���0*/
		CAN->sFilterRegister[0].FR1=*(u16*)&ReceiveID0<<21;                //32λID//
		CAN->sFilterRegister[0].FR2=0X00000000;    	//32λMASK	FFE
       
		CAN->sFilterRegister[1].FR1=*(u16*)&ReceiveID1<<21;               //32λID//
		CAN->sFilterRegister[1].FR2=0XFFE00000;   //0XFOE            //32λMASK	
		
		CAN->sFilterRegister[2].FR1=*(u16*)&ReceiveID2<<21;                //32λID//
		CAN->sFilterRegister[2].FR2=0XF0000000;    	//32λMASK	FFE



		CAN->FA1R|=1<<0;		//���������0	
		CAN->FA1R|=1<<1;		//���������1	
		CAN->FA1R|=1<<2;		//���������2	
		
		CAN->FMR&=~(1<<0);  //���������������ģʽ

		CAN->IER|=1<<1;			//FIFO0��Ϣ�Һ��ж�����.	    
		MY_NVIC_Init(1,1,USB_LP_CAN_RX0_IRQChannel,2);//��2	
		
//		MY_NVIC_Init(1,3,CAN_TX_IRQn,2);
		
		CAN->MCR&=~(1<<0);	//����CAN�˳���ʼ��ģʽ
		while((CAN->MSR&1<<0)==1)
		{
			i++;
			if(i>0XFFF0)return 3;//�˳���ʼ��ģʽʧ��
		}
		while((CAN->ESR&1<<2)==1<<2)  //ȷ���˳�����ģʽ
		{
			j++;
			if(j>0XFFF0) return 1;
		}		
		
		InitArrayBuf();
		return 0;
}   


static void InitArrayBuf(void)
{
	CANTxType.ArrayBufInPtr=&CANTxType.ArrayBuf[0];
	CANTxType.ArrayBufOutPtr=&CANTxType.ArrayBuf[0];
	CANTxType.ArrayBufCtr=0;

	CANRxType.ArrayBufInPtr=&CANRxType.ArrayBuf[0];
	CANRxType.ArrayBufOutPtr=&CANRxType.ArrayBuf[0];
	CANRxType.ArrayBufCtr=0;
}

static u16 ArrayGetBufCtr(CANArrayBufType *pbuf)
{
	return(pbuf->ArrayBufCtr);       
}
/***********************************************************************************
*                              ����д����л�����
***********************************************************************************/

static u8 ArrayPutFrame(CANArrayBufType *Arraybuf,CANBufType *Frame)
{   
	 if(Arraybuf->ArrayBufCtr<CanArrayBufSize)
	 {
			 Arraybuf->ArrayBufCtr++;
			 memcpy(Arraybuf->ArrayBufInPtr->FrameData,Frame->FrameData,CAN_FRAME_WIDTH);
			 Arraybuf->ArrayBufInPtr->DLC=Frame->DLC;
			 Arraybuf->ArrayBufInPtr->MSGID=Frame->MSGID;
			 Arraybuf->ArrayBufInPtr++;
			 if(Arraybuf->ArrayBufInPtr==&Arraybuf->ArrayBuf[CanArrayBufSize])
					 Arraybuf->ArrayBufInPtr=&Arraybuf->ArrayBuf[0]; 
			 return ABUF_NO_ERR;
			 
	 }
	 return ABUF_TXBUFFULL_ERR;
}
            
    
/***********************************************************************************
*                              ȡ�����л���������
***********************************************************************************/
static u8 ArrayGetFrame(CANArrayBufType *Arraybuf,CANBufType *Frame)
{             
   if(ArrayGetBufCtr(Arraybuf)<1)   
   {
       return ABUF_RXBUFLESS_ERR;                 
   }
	 Arraybuf->ArrayBufCtr--;
	 memcpy(Frame->FrameData,Arraybuf->ArrayBufOutPtr->FrameData,CAN_FRAME_WIDTH);
	 Frame->DLC=Arraybuf->ArrayBufOutPtr->DLC;
	 Frame->MSGID=Arraybuf->ArrayBufOutPtr->MSGID;
	 Arraybuf->ArrayBufOutPtr++;
	 if(Arraybuf->ArrayBufOutPtr==&Arraybuf->ArrayBuf[CanArrayBufSize])
			Arraybuf->ArrayBufOutPtr=&Arraybuf->ArrayBuf[0];   
   return ABUF_NO_ERR;
}

u8 Can_Tx_Msg(CANBufType buffer)
{	   
	u8 mbox;	  
	if(CAN->TSR&(1<<26))mbox=0;			//����0Ϊ��
	else if(CAN->TSR&(1<<27))mbox=1;	//����1Ϊ��
	else if(CAN->TSR&(1<<28))mbox=2;	//����2Ϊ��
	else return(0);
	CAN->sTxMailBox[mbox].TIR=0;		//���֮ǰ������  
	CAN->sTxMailBox[mbox].TIR|=(((unsigned long)buffer.MSGID)<<21);		 
	CAN->sTxMailBox[mbox].TIR|=0<<2;	  
	CAN->sTxMailBox[mbox].TIR|=0<<1;
	CAN->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN->sTxMailBox[mbox].TDTR|=buffer.DLC;		   //����DLC.
	//���������ݴ�������.
	CAN->sTxMailBox[mbox].TDHR=(((u32)buffer.FrameData[7]<<24)|
								((u32)buffer.FrameData[6]<<16)|
 								((u32)buffer.FrameData[5]<<8)|
								((u32)buffer.FrameData[4]));
	CAN->sTxMailBox[mbox].TDLR=(((u32)buffer.FrameData[3]<<24)|
								((u32)buffer.FrameData[2]<<16)|
 								((u32)buffer.FrameData[1]<<8)|
								((u32)buffer.FrameData[0]));
	CAN->sTxMailBox[mbox].TIR|=1<<0; //��������������
	return 1;
}

void Can_Rx_Msg(u8 fifox,CANBufType* buffer)
{	    
	//u8 i=0;
	buffer->DLC=CAN->sFIFOMailBox[fifox].RDTR&0x0F;//�õ�DLC
	buffer->MSGID =CAN->sFIFOMailBox[fifox].RIR>>21;  //�õ�ID
	buffer->FrameData[0]=CAN->sFIFOMailBox[fifox].RDLR&0XFF;
	buffer->FrameData[1]=(CAN->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	buffer->FrameData[2]=(CAN->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	buffer->FrameData[3]=(CAN->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	buffer->FrameData[4]=CAN->sFIFOMailBox[fifox].RDHR&0XFF;
	buffer->FrameData[5]=(CAN->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	buffer->FrameData[6]=(CAN->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	buffer->FrameData[7]=(CAN->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN->RF0R|=0X20;//�ͷ�FIFO0����
	else if(fifox==1)CAN->RF1R|=0X20;//�ͷ�FIFO1���� 

}







void USB_LP_CAN1_RX0_IRQHandler(void)
{	 
     CANBufType TempRxBuf;
	 Can_Rx_Msg(0,&TempRxBuf);
	 ArrayPutFrame(&CANRxType,&TempRxBuf);
}	

u8 Can_TxMsg(CANBufType *TxFrame)
{
	 if(ArrayPutFrame(&CANTxType,TxFrame)==ABUF_NO_ERR)
	 	  return TX_NO_ERR;
	 else
		  return TX_BUFFULL;	 
}

u8 Can_RxMsg(CANBufType *RxFrame)
{
	 if(ArrayGetFrame(&CANRxType,RxFrame)==ABUF_NO_ERR)
	 {	 	 
		 return RX_NO_ERR;
	 }	 
	 else 
		 return RX_BUFLESS; 
}

void TASK_CAN_AutoSend(void)
{
	 CANBufType Tmpbuf;
   if(ArrayGetFrame(&CANTxType,&Tmpbuf)==ABUF_NO_ERR)
	 {
		  Can_Tx_Msg(Tmpbuf);
	 }	 	 
}	
