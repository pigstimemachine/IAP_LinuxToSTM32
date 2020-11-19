#include "DPS_message.h"
#include "string.h"


void * thread_for_DPS(void * arg)
{   		
	XGLog(&xglogdata ,"###thread_for_DPS" , "Start Log ");
	struct power_message *Pwr_msg;
	struct dps_comm_result result1;//0x01
	struct dps_comm_result result2;//0x08
	pthread_t pid1,pid2,pid3,pid4,pid5,pid6;
	pthread_mutex_t rawcan_mutex;
	pthread_mutex_init(&rawcan_mutex, NULL);	
/********************result1*************************************/	
	result1.rawcan_mutex = &rawcan_mutex;
	result1.my_id = 0x01;
	result1.heart_check = 0;
	result1.heart_check_last = 0;
	result1.boot_s = DEV_DOWN;
	result1.index.readIndex = READ_POWER1;
	result1.index.writeIndex=WRITE_TO_RAWCAN;
	result1.index.Program_pathname=Program01;
	result1.index.Heart_queue = &dps_heart_queue_01;
	result1.index.Scan_queue = &dps_Scan_queue;
/***************************************************************/	
	result1.Scan_Data.recvDataApply = P_Off;//只在01接收
	result1.DADDR_Data.DADDR = 0;
/********************result2*************************************/	
	result2.rawcan_mutex = &rawcan_mutex;
	result2.my_id = 0x08;
	result2.heart_check = 0;
	result2.heart_check_last = 0;
	result2.boot_s = DEV_DOWN;
	result2.index.readIndex = READ_POWER2;
	result2.index.writeIndex=WRITE_TO_RAWCAN;
	result2.index.Program_pathname=Program08;
	result2.index.Heart_queue = &dps_heart_queue_08;
	result2.index.Scan_queue = &dps_Scan_queue;
/***************************************************************/	
	set_priority(89);
	pthread_create(&pid1, 0, thread_read_from_dps,&result1);
	pthread_create(&pid2, 0, thread_read_from_dps,&result2);
	pthread_create(&pid3, 0, thread_for_dps_heart,&result1);
	pthread_create(&pid4, 0, thread_for_dps_heart,&result2);
	pthread_create(&pid5, 0, thread_for_dps_Scanner,&result1);//分段接收接收扫码枪数据
	pthread_create(&pid6, 0, thread_for_send_data,&result1);//发送数据
	sleep(5);//等待其他组件初始化完毕后再更新程序
	Updata_Program(&result1);//检测更新固件1068-4
	sleep(2);
	Updata_Program(&result2);//检测更新固件2810-2
	sleep(2);	
	while(1)
	{		
		Pwr_msg = (struct power_message *)get_queue(&DPS_queue, 1000);		
		if(Pwr_msg == NULL)
			continue;
		if(MAIN_DATA == Pwr_msg->source)
		{
			switch (Pwr_msg->receive_cmd)
			{	
				case r_DPS_EquipCode://接收设备编号
					result1.DADDR_Data.DADDR = Pwr_msg->receive_data.DADDR;
					result2.DADDR_Data.DADDR = Pwr_msg->receive_data.DADDR;
					printx("DPS-int-EQu=[%d]\n",Pwr_msg->receive_data.DADDR);
					break;
				default:
					printx("DPS_queue REC cmd not my cmd =%x\n",Pwr_msg->receive_cmd);
				break;
			}
		}
		free(Pwr_msg);			
	}
	printx("DPS_queue #############ERROR\n");
	XGLog(&xglogdata ,"@@@thread_for_DPS" , "Finish Log");
}


int init_thread_DPS(void)
{
	pthread_t pid1;
	pthread_create(&pid1, 0, thread_for_DPS, 0);
}

