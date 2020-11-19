/************************************************************************
* 版权所有 (C)2011, 成都熊谷加世有限公司。
*
* 文件名称： // DPS_message.c
* 文件标识： // 适用于A-300X  公共头文件
* 内容摘要： //
* 其它说明： //
* 当前版本： // Ver1.0
* 作    者： //
* 完成日期： //
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
* 修改日期 ：
* 版 本 号 ：
* 修 改 人 ：
* 修改内容 ：
* 修改记录2：…
************************************************************************/
#include "DPS_message.h"
#include "../CAN/can_app.h"

//注意:命令的最后一位用空格
char *POWER_Heart				="wsdo#0f,80,%x  ";
char *SCANNER_APPLY_ACK			="wsdo#%x,81,%x,%x ";
char *SCANNER_DATA_ACK			="wsdo#%x,81,31,%x ";
char *SEND_BIN_APPLY			="wsdo#%x,40,%x,%x,%x,%x,%x,%x,%x ";
char *SEND_BIN_Data				="wsdo#%x,41,%x,%x,%x,%x,%x,%x,%x ";
char *SEND_DADDR				="wsdo#%x,42,%x,%x,%x,%x,0xef,0xcd,0xab ";


char *POWER_READY				= "wsdo#%x,1 ";

struct can_frame Can_ORGINAL_DATA;

int p_wait_cmd_finished(struct dps_comm_result *result,int timeout)
{
    //timeout = timeout *2;
    while(timeout--)
    {
        usleep(100);
        if(result->back_result == pRESULT_OK)
        {
            pthread_mutex_unlock(result->rawcan_mutex);
            return pRESULT_OK;
        }
        if(result->back_result == pRESULT_FAILED)
        {
            pthread_mutex_unlock(result->rawcan_mutex);
            return pRESULT_FAILED;
        }
    }
    pthread_mutex_unlock(result->rawcan_mutex);
    return pRESULT_FAILED;
}

int write_to_rawcan(char *data,int len,struct dps_comm_result *result)
{
    int fifo_to_rawcan1;
    int i =2,ret;
    do
    {
        result->back_result = pRESULT_BUSY;
        fifo_to_rawcan1 =open(WRITE_TO_RAWCAN,O_RDWR | O_NONBLOCK);
        pthread_mutex_lock(result->rawcan_mutex);
        write(fifo_to_rawcan1,data,strlen(data));
        close(fifo_to_rawcan1);
        ret =p_wait_cmd_finished(result,5000);//500MS
        i--;
    }
    while((0>i)&&(ret == pRESULT_FAILED));
    return ret;
}
int write_to_rawcan_no_delay(char *data,int len,struct dps_comm_result *result)
{
    int fifo_to_rawcan1;
    fifo_to_rawcan1 =open(WRITE_TO_RAWCAN,O_RDWR | O_NONBLOCK);
    pthread_mutex_lock(result->rawcan_mutex);
    write(fifo_to_rawcan1,data,strlen(data));
    close(fifo_to_rawcan1);
    pthread_mutex_unlock(result->rawcan_mutex);
    return pRESULT_OK;
}



/***********************************************************************
 *函数名称:SEND_POWER_Heart_Rawcan()
 *输入参数:
 *返回参数:
 *函数功能：发送扫码枪数据申请应答
 *函数说明：
 *备    注:20190705 -shutan
 ************************************************************************/
int SEND_POWER_Heart_Rawcan(struct dps_comm_result *result)
{
    char cmd[50];
    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,"wsdo#f,80,%x ",result->send_heart);
    //printx("i = %x\r\n",result->send_heart);
    //printx("POWER_Heart:%s\r\n",cmd);
    write_to_rawcan_no_delay(cmd,strlen(cmd),result);
}


/***********************************************************************
 *函数名称:SEND_ScannerApplyACK_Rawcan()
 *输入参数:
 *返回参数:
 *函数功能：发送扫码枪数据申请应答
 *函数说明：
 *备    注:20190705 -shutan
 ************************************************************************/
int SEND_ScannerApplyACK_Rawcan(struct dps_comm_result *result)
{
    char cmd[50];
    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,SCANNER_APPLY_ACK,result->my_id,0x30,0xAA);
    printx("ScannerApplyACK:%s\n",cmd);
    write_to_rawcan_no_delay(cmd,strlen(cmd),result);

}
/***********************************************************************
 *函数名称:SEND_ScannerDATA_Rawcan()
 *输入参数:
 *返回参数:
 *函数功能：发送扫码枪数据应答
 *函数说明：
 *备    注:20190705 -shutan
 ************************************************************************/
int SEND_ScannerDataACK_Rawcan(struct dps_comm_result *result)
{
    char cmd[50];
    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,SCANNER_DATA_ACK,result->my_id,result->Scan_Data.scan_data_sta);
//	printx("ID:0x01,ScannerDataACK: %s\n",cmd);
    write_to_rawcan_no_delay(cmd,strlen(cmd),result);
}



/***********************************************************************
 *函数名称:SEND_DADDR_to_Rawcan()
 *输入参数:
 *返回参数:
 *函数功能：发送扫码枪数据申请应答
 *函数说明：
 *备    注:20190705 -shutan
 ************************************************************************/
int SEND_DADDR_to_Rawcan(struct dps_comm_result *result)
{
    char cmd[50];
    int len;
    char DADDR_1[5];
    memset(DADDR_1,0,sizeof(DADDR_1));
    DADDR_1[0] = (char)((result->DADDR_Data.DADDR) & 0xff);
    DADDR_1[1] = (char)((result->DADDR_Data.DADDR >> 8) & 0xff);
    DADDR_1[2] = (char)((result->DADDR_Data.DADDR >> 16) & 0xff);
    DADDR_1[3] = (char)((result->DADDR_Data.DADDR >> 24) & 0xff);
    memset(cmd,0,sizeof(cmd));
    len = sprintf(cmd,SEND_DADDR,
                  result->my_id,DADDR_1[0],DADDR_1[1],DADDR_1[2],DADDR_1[3]);
    printx("DPS%x:SEND_DADDR:%s\n",result->my_id,cmd);
    if(write_to_rawcan(cmd,strlen(cmd),result) != pRESULT_OK)
    {
        printx("DPS%x:SEND_DADDR %s Failed\n",result->my_id,cmd);
        return -1;
    }
    else
    {
        return 0;
    }
}

/***********************************************************************
 *函数名称:SEND_Upadata_Applay_to_Rawcan()
 *输入参数:
 *返回参数:
 *函数功能：发送扫码枪数据申请应答
 *函数说明：
 *备    注:20190705 -shutan
 ************************************************************************/
int SEND_Upadata_Applay_to_Rawcan(struct dps_comm_result *result)
{
    char cmd[50];
    int len;
//	result->Updata_Prgram.fileLen = 0x123456;
//	result->Updata_Prgram.Checksum = 0x12345678;
    char Checksum_1[5];
    char fileLen_1[4];
    memset(Checksum_1,0,sizeof(Checksum_1));
    memset(fileLen_1,0,sizeof(fileLen_1));
//	printx("########3Upadata_Applay:fileLen = %d,Checksum = %d",result->Updata_Prgram.fileLen,result->Updata_Prgram.Checksum);
    fileLen_1[0] = (char)((result->Updata_Prgram.fileLen) & 0xff);
    fileLen_1[1] = (char)((result->Updata_Prgram.fileLen >> 8) & 0xff);
    fileLen_1[2] = (char)((result->Updata_Prgram.fileLen >> 16) & 0xff);

    Checksum_1[0] = (char)((result->Updata_Prgram.Checksum) & 0xff);
    Checksum_1[1] = (char)((result->Updata_Prgram.Checksum >> 8) & 0xff);
    Checksum_1[2] = (char)((result->Updata_Prgram.Checksum >> 16) & 0xff);
    Checksum_1[3] = (char)((result->Updata_Prgram.Checksum >> 24) & 0xff);

    memset(cmd,0,sizeof(cmd));
    len = sprintf(cmd,SEND_BIN_APPLY,result->my_id,fileLen_1[0],fileLen_1[1],fileLen_1[2],
                  Checksum_1[0],Checksum_1[1],Checksum_1[2],Checksum_1[3]);

    //printx("DPS%x:SEND_BIN_APPLY:%s\r\n",result->my_id,cmd);
    if(write_to_rawcan(cmd,strlen(cmd),result) != pRESULT_OK)
    {
        printx("DPS%x:SEND_BIN_APPLY %s Failed\n",result->my_id,cmd);
        return -1;
    }
    else
    {
        return 0;
    }
}


/***********************************************************************
 *函数名称:SEND_Upadata_Applay_to_Rawcan()
 *输入参数:
 *返回参数:
 *函数功能：发送扫码枪数据申请应答
 *函数说明：
 *备    注:20190705 -shutan
 ************************************************************************/
int SEND_Upadata_to_Rawcan(struct dps_comm_result *result)
{
    char cmd[50];
    int len;
    memset(cmd,0,sizeof(cmd));
    len = sprintf(cmd,SEND_BIN_Data,result->my_id,
                  result->Updata_Prgram.ProData_check,
                  result->Updata_Prgram.Program[0],
                  result->Updata_Prgram.Program[1],
                  result->Updata_Prgram.Program[2],
                  result->Updata_Prgram.Program[3],
                  result->Updata_Prgram.Program[4],
                  result->Updata_Prgram.Program[5]
                 );
    //printx("DPS%x:SEND_BIN_Data:%s\r\n",result->my_id,cmd);
    if(write_to_rawcan(cmd,strlen(cmd),result) != pRESULT_OK)
    {
        printx("DPS%x:SEND_BIN_Data %s Failed\n",result->my_id,cmd);
        return -1;
    }
    else
    {
        return 0;
    }
}

/***********************************************************************
 *函数名称:SEND_LenData_to_Rawcan()
 *输入参数:
 *返回参数:
 *函数功能：发送扫码枪数据申请应答
 *函数说明：
 *备    注:20190705 -shutan
 ************************************************************************/
#if 1
int SEND_LenData_to_Rawcan(struct dps_comm_result *result)
{
    char data[8];
    char tmp[50];
    memset(data,0,sizeof(data));
    memset(tmp,0,sizeof(tmp));
    memcpy(data,result->Send_nbyte_data.data,8);
    if(result->Send_nbyte_data.len == 1)
        sprintf(tmp,"wsdo#%x,%x,%x ",result->Send_nbyte_data.id,result->Send_nbyte_data.reg,data[0]);
    else if(result->Send_nbyte_data.len == 2)
        sprintf(tmp,"wsdo#%x,%x,%x,%x ",result->Send_nbyte_data.id,result->Send_nbyte_data.reg,data[0],data[1]);
    else if(result->Send_nbyte_data.len == 3)
        sprintf(tmp,"wsdo#%x,%x,%x,%x,%x ",result->Send_nbyte_data.id,result->Send_nbyte_data.reg,data[0],data[1],data[2]);
    else if(result->Send_nbyte_data.len == 4)
        sprintf(tmp,"wsdo#%x,%x,%x,%x,%x,%x ",result->Send_nbyte_data.id,result->Send_nbyte_data.reg,data[0],data[1],data[2],data[3]);
    else if(result->Send_nbyte_data.len == 5)
        sprintf(tmp,"wsdo#%x,%x,%x,%x,%x,%x,%x ",result->Send_nbyte_data.id,result->Send_nbyte_data.reg,data[0],data[1],data[2],data[3],data[4]);
    else if(result->Send_nbyte_data.len == 6)
        sprintf(tmp,"wsdo#%x,%x,%x,%x,%x,%x,%x,%x ",result->Send_nbyte_data.id,result->Send_nbyte_data.reg,data[0],data[1],data[2],data[3],data[4],data[5]);
    else if(result->Send_nbyte_data.len == 7)
        sprintf(tmp,"wsdo#%x,%x,%x,%x,%x,%x,%x,%x,%x ",result->Send_nbyte_data.id,result->Send_nbyte_data.reg,data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
    else
    {
        printx("send_data_len_error:%d\n",result->Send_nbyte_data.len);
        return -1;
    }
    printx("DPS%x:SEND_Data %s\n",result->my_id,tmp);
    if(write_to_rawcan(tmp,strlen(tmp),result) != pRESULT_OK)
    {
        printx("DPS%x:SEND_Data %s Failed\n",result->my_id,tmp);
        return -1;
    }
    else
    {
        return 0;
    }

}


#endif



int dps_control_P1P2_Dps_Ready_01(struct dps_comm_result *result)
{
    char cmd[50];
    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,POWER_READY,result->my_id);
    printx("DPS%x:P1P2_Dps_Ready %s\n",result->my_id,cmd);
    if(write_to_rawcan(cmd,strlen(cmd),result) != pRESULT_OK)
    {
        printx("DPS%x:P1P2_Dps_Ready %s Failed\n",result->my_id,cmd);
        return -1;
    }
    else
    {
        result->boot_s = DEV_ONLINE;
        return 0;
    }
}

/***********************************************************************
 *函数名称:SendCurrt_voltData_To_AS4_0()
 *输入参数:
 *返回参数:
 *函数功能：
 *函数说明：
 *备    注:
 ************************************************************************/
int dps_send_current_vlot(char *data)
{
    struct message *msg;
    msg = (struct message *)malloc(sizeof(struct message));
    if(msg == NULL)
    {
        printf("dps1 %s:%d failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    msg->src = DPS_SRC;
    msg->type.pwr_msg.source = DPS_SRC;
    msg->type.pwr_msg.send_cmd = s_DPS_currt_volt;
    msg->type.pwr_msg.send_data.power_value.current = (float)data[1]+data[2]*256;
    msg->type.pwr_msg.send_data.power_value.voltage = (float)(data[3]+data[4]*256)/10.0;
    put_queue(&main_queue, (void *) msg);
}



/***********************************************************************
 *函数名称:dps_send_WeldAngle()
 *输入参数:
 *返回参数:
 *函数功能：发送角度数据到AS4.0
 *函数说明：
 *备    注:20190703 -shutan
 ************************************************************************/
int dps_send_WeldAngle(char *data)
{
    struct message *msg;
    msg = (struct message *)malloc(sizeof(struct message));
    if(msg == NULL)
    {
        printf("dps1 %s:%d failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    msg->src = DPS_SRC;
    msg->type.pwr_msg.source = DPS_SRC;
    msg->type.pwr_msg.send_cmd = s_DPS_Weld_Angle;//数据有正负
    msg->type.pwr_msg.send_data.Weld_Angle.angle_x= ((short)data[1]&(0x00FF)) | (((short)data[2]&(0x00FF))<<8);
    msg->type.pwr_msg.send_data.Weld_Angle.angle_y= ((short)data[3]&(0x00FF)) | (((short)data[4]&(0x00FF))<<8);
    msg->type.pwr_msg.send_data.Weld_Angle.angle_z= ((short)data[5]&(0x00FF)) | (((short)data[6]&(0x00FF))<<8);
#if 0
    printf("dps_send_WeldAngle:%4d %4d %4d\r\n",
           msg->type.pwr_msg.send_data.Weld_Angle.angle_x,
           msg->type.pwr_msg.send_data.Weld_Angle.angle_y,
           msg->type.pwr_msg.send_data.Weld_Angle.angle_z
          );
#endif
    put_queue(&main_queue, (void *)msg);
}

/***********************************************************************
 *函数名称:dps_send_LayerTemp()
 *输入参数:
 *返回参数:
 *函数功能：发送温度数据
 *函数说明：
 *备    注:20190703 -shutan
 ************************************************************************/
int dps_send_LayerTemp(char *data)
{
    struct message *msg;
    msg = (struct message *)malloc(sizeof(struct message));
    if(msg == NULL)
    {
        printf("dps1 %s:%d failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    msg->src = DPS_SRC;
    msg->type.pwr_msg.source = DPS_SRC;
    msg->type.pwr_msg.send_cmd = s_DPS_LayerTemp;
    msg->type.pwr_msg.send_data.IR_Temp =  ((unsigned short)data[1]&(0x00FF)) | (((unsigned short)data[2]&(0x00FF))<<8);
    msg->type.pwr_msg.send_data.Surface_Temp= ((unsigned short)data[3]&(0x00FF)) | (((unsigned short)data[4]&(0x00FF))<<8);
    put_queue(&main_queue, (void *)msg);
}


/***********************************************************************
 *函数名称:dps_send_LayerTemp()
 *输入参数:
 *返回参数:
 *函数功能：发送温度数据
 *函数说明：
 *备    注:20190703 -shutan
 ************************************************************************/
int dps_send_ScanData(struct dps_comm_result *result)
{
    struct message *msg;
    msg = (struct message *)malloc(sizeof(struct message));
    if(msg == NULL)
    {
        printf("dps1 %s:%d failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    msg->src = DPS_SRC;
    msg->type.pwr_msg.source = DPS_SRC;
    msg->type.pwr_msg.send_cmd = s_DPS_ScanData;
    memcpy(msg->type.pwr_msg.send_data.QR_code,result->Scan_Data.QR_code,sizeof(result->Scan_Data.QR_code));
    put_queue(&main_queue, (void *)msg);
}



/***********************************************************************
 *函数名称:dps_send_weldPosition()
 *输入参数:
 *返回参数:
 *函数功能：发送温度数据
 *函数说明：
 *备    注:20190926 -shutan
 ************************************************************************/
int dps_send_weldPosition(char *data)
{
    struct message *msg;
    msg = (struct message *)malloc(sizeof(struct message));
    if(msg == NULL)
    {
        printf("dps1 %s:%d failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    msg->src = DPS_SRC;
    msg->type.pwr_msg.source = DPS_SRC;
    msg->type.pwr_msg.send_cmd = s_DPS_weldPosition;
    if(data[1] == 0xbb)
    {
        memcpy(msg->type.pwr_msg.send_data.WeldPosition,"CW",sizeof("CW"));
    }
    else// if(data[1] == 0xcc)
    {
        memcpy(msg->type.pwr_msg.send_data.WeldPosition,"CCW",sizeof("CCW"));
    }
    put_queue(&main_queue, (void *)msg);
}



struct heart_message
{
    char heart;
};


void dps_heart_to_queue(struct dps_comm_result *result,char heart)
{
    struct heart_message *msg;
    msg = (struct heart_message *)malloc(sizeof(struct heart_message));
    if(msg == NULL)
    {
        printf("DPS:%x--%s:%d failed!\n",result->my_id,__FUNCTION__,__LINE__);
        XGPutToLog(xglogdata.log_handle,"(dps_heart_to_queue)DPS:%x--%s:%d failed!\n",result->my_id,__FUNCTION__,__LINE__);
        return ;
    }
    msg->heart = heart;
    put_queue(result->index.Heart_queue, (void *) msg);
}


int check_heart(unsigned char heartnew,unsigned char heartold)
{
    int oold,nnew;
    heartold = heartold +1;
    oold = heartold;
    nnew = heartnew;
    return abs(heartnew-heartold);
}



void *thread_for_dps_heart(void * arg)
{
    struct heart_message *msg;
    struct dps_comm_result *result = (struct dps_comm_result *)arg;
    int check_data;
    set_priority(88);
    int i=0;
    while(1)
    {
#if 1
        msg = (struct heart_message *)get_queue(result->index.Heart_queue, 3000);
        if(NULL == msg)
        {
            result->boot_s = DEV_DOWN;
            result->heart_check = 0;
            result->heart_check_last= 0;
            continue;
        }
        i++;
        result->send_heart = (char)i & 0xff;

        //if(result->my_id == 0x01)//心跳发送广播
        //{
        SEND_POWER_Heart_Rawcan(result);
        //}

        //printx("i = %d\r\n",result->send_heart);
        result->heart_check = msg->heart;
        check_data = check_heart(msg->heart,result->heart_check_last);
        if((check_data > 1)||(result->boot_s == DEV_ERR))
        {
            if(check_data > 3)
            {
                result->DADDR_Data.One_reset_sta = P_Off;//如果心跳掉线，重新
            }
            //printx("DPS%x:heart last:%x new:%x  check:%d\n ",result->my_id,result->heart_check_last,result->heart_check,check_data);
        }
        result->heart_check_last = result->heart_check;
        free(msg);
#endif
    }
    printl("end of %s\n",__FUNCTION__);
}




struct Scan_message
{
    char heart;
    char Sca_data[10];
};


void dps_Scanner_to_queue(struct dps_comm_result *result,char *data)
{
    struct Scan_message *msg;
    msg = (struct Scan_message *)malloc(sizeof(struct Scan_message));
    if(msg == NULL)
    {
        printf("DPS:%x--%s:%d failed!\n",result->my_id,__FUNCTION__,__LINE__);
        XGPutToLog(xglogdata.log_handle,"(dps_Scanner_to_queue)DPS:%x--%s:%d failed!\n",result->my_id,__FUNCTION__,__LINE__);
        return ;
    }
    msg->heart = data[1];
    memcpy(msg->Sca_data,data+2,6);
    //printx("Scanner:%s:%d \n",__FUNCTION__,__LINE__);
    put_queue(result->index.Scan_queue, (void *) msg);
}


void *thread_for_dps_Scanner(void * arg)
{
    struct Scan_message *msg;
    struct dps_comm_result *result = (struct dps_comm_result *)arg;
    int check_data;
    char Sca_data[1024];
    int scandatalen = 0;
    int i =0;
    int n =0;
    int checksum = 0;
    set_priority(77);
    result->Scan_Data.ScanData_check= 0;
    result->Scan_Data.ScanData_check_last= 0;
    memset(Sca_data,0,sizeof(Sca_data));
//	printx("DPS:sizeof(Sca_data) = %d\r\n",sizeof(Sca_data));
    while(1)
    {
        //printx("Scanner:%s:%d \n",__FUNCTION__,__LINE__);
        msg = (struct Scan_message *)get_queue(result->index.Scan_queue,3000);
        if(NULL == msg)
        {
            result->Scan_Data.ScanData_check= 0;
            result->Scan_Data.ScanData_check_last= 0;
            memset(Sca_data,0x00,sizeof(Sca_data));		//接收错误时
            n = 0;
            continue;
        }
        result->Scan_Data.ScanData_check = msg->heart;	//转存心跳数据
        if(0x00 == Sca_data[0])							//第一次接受数据时
        {
            result->Scan_Data.ScanData_check_last = result->Scan_Data.ScanData_check-1;
        }
        check_data = check_heart(result->Scan_Data.ScanData_check,result->Scan_Data.ScanData_check_last);
        if((check_data > 1))//接收失败
        {
            result->Scan_Data.scan_data_sta = Data_Err;	//回复失败
            result->Scan_Data.recvDataApply = P_Off;	//需要重新发送申请
            memset(Sca_data,0,sizeof(Sca_data));		//接收错误时
            n = 0;
        }
        else
        {
            if(P_On == result->Scan_Data.recvDataApply)	//得到了扫码枪发送申请
            {
                memcpy(&Sca_data[n],msg->Sca_data,6);
                scandatalen = 0;
                scandatalen = strlen(Sca_data);							//得到接收数据的长度
                if(scandatalen == result->Scan_Data.ScanDataLen)		//表示一帧数据接收完成//Sca_data[n+5] == 0x00 ||
                {
                    checksum = 0;
                    for(i = 0 ; i < scandatalen; i++)
                    {
                        checksum += (0x000000FF) & Sca_data[i];
                    }
                    checksum = 0x10000000-checksum;
                    printf("MY_checksum =%d,MY_scandatalen =%d\n",checksum,scandatalen);
                    if(checksum == result->Scan_Data.Checksum && scandatalen == result->Scan_Data.ScanDataLen)
                    {
                        //成功接收到一帧数据
                        memset(result->Scan_Data.QR_code,0,sizeof(result->Scan_Data.QR_code));
                        memcpy(result->Scan_Data.QR_code,Sca_data,sizeof(Sca_data));
                        dps_send_ScanData(result);

#if 0
                        printx("DPS:%x--Sca_data:%s\r\n",result->my_id,result->Scan_Data.QR_code);
#endif
                        result->Scan_Data.scan_data_sta = Data_On;	//回复成功
                        result->Scan_Data.recvDataApply = P_Off;
                        n = 0;
                        memset(Sca_data,0,sizeof(Sca_data));
                    }
                    else
                    {
                        result->Scan_Data.scan_data_sta = Data_Err;	//回复失败
                        memset(Sca_data,0,sizeof(Sca_data));	//接收错误时
                        n = 0;
                    }
                    //printf("DPS:%x-xxxxxx-Sca_data:%s\r\n",result->my_id,&Sca_data[0]);

                }
                else
                {
                    result->Scan_Data.scan_data_sta = Data_On;	//回复成功
                    n+=6;
                }

            }
            else
            {
                result->Scan_Data.scan_data_sta = Data_Err;				//回复失败
                memset(Sca_data,0,sizeof(Sca_data));					//接收错误时
            }
        }
        SEND_ScannerDataACK_Rawcan(result);
        result->Scan_Data.ScanData_check_last = result->Scan_Data.ScanData_check;
        free(msg);
    }
}



int p_generate_format(char *format,char *data)
{
    int i;
    int comma = 1;
    char *p = data;

    while(*p++ != '\0')
        if(*p == ',')
            comma++;

    if(comma == 0||comma > 9)
        return comma;

    strcpy(format,"read=%x");
    for(i = 1; i<comma; i++)
        strcat(format,",%x");

    return comma;
}




int data_from_power(char *oridata,struct CAN_data *recdata)
{
    int comma =0;
    char *data=recdata->buffer_data;
    char format[BUFFER_SIZE];
    memset(format,0,BUFFER_SIZE);
    memset(recdata,0,sizeof(struct CAN_data));

    comma = p_generate_format(format,oridata);

    if(comma ==0 || comma > 9)
    {
        return -1;
    }
    if(comma == 1)
        sscanf(oridata,format,&recdata->target);
    else if(comma == 2)
        sscanf(oridata,format,&data[0],&recdata->target);
    else if(comma == 3)
        sscanf(oridata,format,&data[0],&data[1],&recdata->target);
    else if(comma == 4)
        sscanf(oridata,format,&data[0],&data[1],&data[2],&recdata->target);
    else if(comma == 5)
        sscanf(oridata,format,&data[0],&data[1],&data[2],&data[3],&recdata->target);
    else if(comma == 6)
        sscanf(oridata,format,&data[0],&data[1],&data[2],&data[3],&data[4],&recdata->target);
    else if(comma == 7)
        sscanf(oridata,format,&data[0],&data[1],&data[2],&data[3],&data[4],&data[5],&recdata->target);
    else if(comma == 8)
        sscanf(oridata,format,&data[0],&data[1],&data[2],&data[3],&data[4],&data[5],&data[6],&recdata->target);

    else if(comma == 9)
        sscanf(oridata,format,&data[0],&data[1],&data[2],&data[3],&data[4],&data[5],&data[6],&data[7],&recdata->target);

    return comma-1;
}



int finddata(char *find,char **data)
{
    int i = 0;
    char *p = *data;
    char *seccmd;
    int num = 0;
    while(1)
    {
        if(NULL == strstr(p,"read="))
            break;
        p=strstr(p,"read=");
        if(i == 1)
            seccmd = strstr(p,"read=");
        p =p+5;
        i++;
    }
    if(i == 0)
        return 0;
    if(i == 1)
    {
        p = strstr(*data,"read=");
        strncpy(find,p,strlen(p));
        *data =p+5;
    }
    else
    {
        p = strstr(*data,"read=");
        strncpy(find,p,seccmd-p);
        *data =seccmd;
    }
    return i;
}




/**

  * @brief 	 how to use it ,and where to use it
  * @Name		 增加CAN通信缓存队列
  * @param 	 None
  * @retval	 None
  * @Author	 ZCJ
  * @Date		 2020/05/26 Create
  * @Version	 1.0 2020/05/26
  *			 1.1 2020/05/26 change sth
  * @Note

  **/
CANArrayBufType CANTxType,CANRxType;
void InitArrayBuf(void)
{
    CANTxType.ArrayBufInPtr=&CANTxType.ArrayBuf[0];
    CANTxType.ArrayBufOutPtr=&CANTxType.ArrayBuf[0];
    CANTxType.ArrayBufCtr=0;

    CANRxType.ArrayBufInPtr=&CANRxType.ArrayBuf[0];
    CANRxType.ArrayBufOutPtr=&CANRxType.ArrayBuf[0];
    CANRxType.ArrayBufCtr=0;
    XGPutToLog(xglogdata.log_handle,"(InitArrayBuf)");
}
static u16 ArrayGetBufCtr(CANArrayBufType *pbuf)
{
    return(pbuf->ArrayBufCtr);
}
/***********************************************************************************
*                              数据写入队列缓冲区
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
*                              取出队列缓冲区数据
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
u8 Can_Tx_Msg(CANBufType buffer)
{
    struct can_frame *can_data;
    can_data->can_dlc = buffer.DLC ;
    can_data->can_id = buffer.MSGID ;
    memcpy(can_data->data, &buffer.FrameData,sizeof(can_data->data));
    Send_Can_Data(can_data);
}
void Can_Rx_Msg(struct can_frame *can_orgdata, CANBufType* buffer )
{
    u8 i=0;
    buffer->DLC=can_orgdata->can_dlc;//得到DLC
    buffer->MSGID =can_orgdata->can_id;  //得到ID
    memcpy(&buffer->FrameData,&can_orgdata->data,sizeof(buffer->FrameData));
}

const ECANID_INTERNAL TransmitID3= { ChildLCD,LocalHostAddr2,TargetAddr };
/*
canid_t id;
id = *(canid_t*)&TransmitID3 ;

*/

void * RCV_Can_Layer(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        TASK_CAN_AutoSend();
        CanReceiveProcess(as4_comm_result);
        usleep(1000);
    }
}


void _SEND_ACK(void)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_SendAck;
    can_data.data[1]=CMD_ParaToLCD;
    can_data.data[2]=CMD_Layer;
    Send_Can_Data(&(can_data));
}

/**

  * @brief 	 how to use it ,and where to use it
  * @Name		 发送CAN应答信息模板
  * @param 	 None
  * @retval	 None
  * @Author	 ZCJ
  * @Date		 2020/06/09 Create
  * @Version
  * @Note	3 4 4 将结构体强制转换为整型，

  **/

/*
static void _SEND_ACK(u8 CMD,u8 flag,ECANID_INTERNAL* CID)
{
  u8 Tmpid;
  Tmpid=CID->source;
  CID->source=CID->target;
  CID->target=Tmpid;
//

//	if(CanStatus.OnLineSta==STA_ON)
//	  {
		 struct can_frame can_data;
		 can_data.can_dlc=3;
		 can_data.can_id=*(canid_t*)CID;//*(u32*)&TransmitID3;
		 can_data.data[0]=CMD_SendAck;
		 can_data.data[1]=flag;

		 Send_Can_Data(&(can_data));
//	  }
}
*/

/**************test************************/
/*
typedef struct
{
	struct can_frame can_data;
}CAN_send_group;
CAN_send_group canSend;
canSend.can_data.data[]



{ CMD , ID , DLC , }

init_can_data(ECANID_INTERNAL* ID_CAN ^^ )
{
	struct can_frame D_LAYER,D_HEART,D_TEMP;
	D_LAYER.data
}

int _SendCanData(ECANID_INTERNAL* CID , u8 CMD)
{
	switch (cmd)
	{
		case CMD_Layer:
			can_data.can_dlc=8;
			can_data.data[0]=CMD_Layer;
			can_data.data[1]=as4_comm_result->welding_position._cur_layer;
			can_data.data[2]=20;
			memcpy(&can_data.data[3],&as4_comm_result->Calc_Data.LAYER,5);//
			break;
		case CMD_Heartbeat:
			can_data.
			break;
		default:
			break;


	}
	 Send_Can_Data(&(can_data));

}



 void _SendCanData(ECANID_INTERNAL* CID , )
{
	struct can_frame can_data;
	can_data.can_dlc=8;
	can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
	can_data.data[0]=CMD_SendAck;
	can_data.data[1]=CMD_Layer;

	Send_Can_Data(&(can_data));
}

*/
/**************test************************/



int CAN_SEND_LAYER(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD;
    can_data.data[1]=CMD_Layer;
    can_data.data[2]=as4_comm_result->welding_position._cur_layer;
    can_data.data[3]=TolLayer;
    memcpy(&can_data.data[4],&as4_comm_result->Calc_Data.LAYER,4);//
#if 0
    PX("as4_comm_result->Calc_Data.LAYER=%s\n",as4_comm_result->Calc_Data.LAYER);
    PX("can_data[1]=%d\n", can_data.data[1]);
    PX("can_data[3]=%d\n", can_data.data[3]);
    PX("can_data[4]=%d\n", can_data.data[4]);
    PX("can_data[5]=%d\n", can_data.data[5]);
    PX("can_data[6]=%d\n", can_data.data[6]);
    PX("can_data[7]=%d\n", can_data.data[7]);
#endif
    Send_Can_Data(&(can_data));
}



void * Send_Can_Layer(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        if(as4_comm_result->Can_Layer_Data.rcv_sts == NeedToSendData )
        {
            CAN_SEND_LAYER(as4_comm_result);
        }
        usleep(6000);
    }
}
int SendLayerDataSts( struct _AS4_comm_result *as4_comm_result )
{
    as4_comm_result->Can_Layer_Data.rcv_sts = NeedToSendData ;
    return 0 ;
}




int parase_can_rcv_layerdata( CANBufType*  CANRxData,struct _AS4_comm_result *as4_comm_result )
{
    char Cur_layer[20];
    as4_comm_result->welding_position._cur_layer = CANRxData->FrameData[2] ;
    lay_reset(as4_comm_result,Err);
    //PX("rcv can layer data  data[1]=%d\n" , Can_ORGINAL_DATA.data[1]);
    sprintf(Cur_layer, "%d", as4_comm_result->welding_position._cur_layer);
    memcpy(as4_comm_result->Calc_Data.Now_LAYER,Cur_layer,sizeof(as4_comm_result->Calc_Data.Now_LAYER));

    if(   (0 != memcmp(as4_comm_result->Calc_Data.His_LAYER,			   as4_comm_result->Calc_Data.Now_LAYER,					 sizeof(as4_comm_result->Calc_Data.LAYER)))
            ||(0 != memcmp(&as4_comm_result->welding_position_His._0clock_welding,&as4_comm_result->welding_position._0clock_welding,sizeof(as4_comm_result->welding_position._0clock_welding)))
            ||(0 != memcmp(&as4_comm_result->welding_position_His._3clock_welding,&as4_comm_result->welding_position._3clock_welding,sizeof(as4_comm_result->welding_position._3clock_welding)))
            ||(0 != memcmp(&as4_comm_result->welding_position_His._6clock_welding,&as4_comm_result->welding_position._6clock_welding,sizeof(as4_comm_result->welding_position._6clock_welding)))
            ||(0 != memcmp(&as4_comm_result->welding_position_His._finish_sts,	  &as4_comm_result->welding_position._finish_sts,	 sizeof(as4_comm_result->welding_position._finish_sts)))
            ||(0 != memcmp(&as4_comm_result->welding_position_His._all_welding,   & as4_comm_result->welding_position._all_welding,	 sizeof(as4_comm_result->welding_position._all_welding)))
            ||(0 != memcmp(as4_comm_result->AS4_Final_SendData.WELD_CODE,  as4_comm_result->PipeCode,	 sizeof(as4_comm_result->AS4_Final_SendData.WELD_CODE)))
      )

    {
        DiscernHJPROCESS(as4_comm_result);
        Write_Scanner_data(as4_comm_result,Layer_CMD);//将二维码数据记录到本地
        printx("His_ALL_weld_time:%4d--His_LAYER:%s\n",as4_comm_result->Calc_Data.ALL_weld_time,as4_comm_result->Calc_Data.LAYER);
    }

    as4_comm_result->Can_Layer_Data.rcv_sts = NeedToSendData ;
    //PX("rcv can layer data  can_orgdata->data[0]=%d\n" , can_orgdata->data[0]);
}

CAN_STATUS          CanStatus;
static  void _CommWatchDog_Feed()
{
    CanStatus.OffLineCnt=0;
    CanStatus.OnLineSta=STA_ON;
}
static void _CommWatchDog_TASK(struct _AS4_comm_result *as4_comm_result )
{
    CanStatus.OffLineCnt++;
    if(CanStatus.OffLineCnt>MaxOfflinecnt)
    {
        CAN_SEND_LAYER(as4_comm_result);
        PX("layerShowDev is offline=%d\n",CanStatus.OffLineCnt);
        XGPutToLog(xglogdata.log_handle,"(_CommWatchDog_TASK)layerShowDev is offline=%d\n",CanStatus.OffLineCnt);
        CanStatus.OnLineSta=STA_OFF;

    }
}


/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送焊口数据
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
#if 0
int CAN_SEND_Pipe(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=32;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_Pipe;

    memcpy(&can_data.data[1],as4_comm_result->AS4_Final_SendData.WELD_CODE,32);//
#if 0
    PX("as4_comm_result->Calc_Data.LAYER=%s\n",as4_comm_result->Calc_Data.LAYER);
    PX("can_data[1]=%d\n", can_data.data[1]);
    PX("can_data[3]=%d\n", can_data.data[3]);
    PX("can_data[4]=%d\n", can_data.data[4]);
    PX("can_data[5]=%d\n", can_data.data[5]);
    PX("can_data[6]=%d\n", can_data.data[6]);
    PX("can_data[7]=%d\n", can_data.data[7]);
#endif
    Send_Can_Data(&(can_data));
}
#endif
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送系统温度数据
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/

int CAN_SEND_SYS_Temp(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_Sys_Temp;
    memcpy(&can_data.data[2], as4_comm_result->AS4_Final_SendData.TEMP, 6 );
    Send_Can_Data(&(can_data));
}
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送系统湿度数据
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
int CAN_SEND_SYS_Hmi(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_Sys_Hmi;
    memcpy(&can_data.data[2], as4_comm_result->AS4_Final_SendData.HUMIDITY, 6 );
    Send_Can_Data(&(can_data));
}

/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理CW/CCW
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
int CAN_SEND_Position(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_Position;
    memcpy(&can_data.data[2], as4_comm_result->AS4_Final_SendData.POSITION, 6 );
    Send_Can_Data(&(can_data));
}


/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送时间
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
#if 0
int CAN_SEND_Time(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_Time;
    memcpy(&can_data.data[1], as4_comm_result->AS4_Final_SendData.CJ_TIME, 7 );
    Send_Can_Data(&(can_data));
}
#endif
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送voltage
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
int CAN_SEND_Voltage(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_Voltage;
    memcpy(&can_data.data[2], as4_comm_result->AS4_Final_SendData.VOLTAGE, 6 );
    Send_Can_Data(&(can_data));
}

/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送current
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
int CAN_SEND_Current(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_Current;
    memcpy(&can_data.data[2], as4_comm_result->AS4_Final_SendData.CURRENT, 6 );
    Send_Can_Data(&(can_data));
}
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送送丝速度
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/25 Create
  * @Version 	1.0 2020/08/25
  *		     	1.1 2020/08/25 change sth
  * @Note

  **/
int CAN_SEND_SS_SPEED(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_SS_Speed;
    memcpy(&can_data.data[2], as4_comm_result->AS4_Final_SendData.SS_SPEED, 6 );
    Send_Can_Data(&(can_data));
}
/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送焊接速度
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/25 Create
  * @Version 	1.0 2020/08/25
  *		     	1.1 2020/08/25 change sth
  * @Note

  **/
int CAN_SEND_HJ_SPEED(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_HJ_Speed;
    memcpy(&can_data.data[2], as4_comm_result->AS4_Final_SendData.HJ_SPEED, 6 );
    Send_Can_Data(&(can_data));
}
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	发送 是否扫码成功标志。
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/09/27  Create
  * @Version 	1.0 2020/09/27
  *		     	1.1 2020/09/27 change sth
  * @Note
  **/
int CAN_SEND_ScanSts(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=4;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_ScanSts;
    can_data.data[2]=0xaa;//扫码成功
    can_data.data[3]=as4_comm_result->ScanMsg.IsNewData;//是否为新焊口
    //memcpy(&can_data.data[2] , as4_comm_result->AS4_Final_SendData.HJ_SPEED , 6 );
    Send_Can_Data(&(can_data));
}
void *  CAN_SEND_SCANNER (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        if(as4_comm_result->ScanMsg.sts == SCANSUCC)
        {
            CAN_SEND_ScanSts(as4_comm_result);
            //PX("send scan succ !\n");
            as4_comm_result->ScanMsg.sts = NoSCANData;
        }
        sleep(1);
    }
}

/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送TEAM CODE
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/24 Create
  * @Version 	1.0 2020/08/24
  *		     	1.1 2020/08/24 change sth
  * @Note

  **/
#if 0
int CAN_SEND_TeamCode(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_TeamCode;
    memcpy(&can_data.data[1], as4_comm_result->AS4_Final_SendData.TEAM_CODE, 7 );
    Send_Can_Data(&(can_data));
}

#endif


/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 发送焊层温度
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/25 Create
  * @Version 	1.0 2020/08/25
  *		     	1.1 2020/08/25 change sth
  * @Note

  **/
int CAN_SEND_LayerTemp(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_ParaToLCD_NAck;
    can_data.data[1]=CMD_LayerTemp;
    can_data.data[2] = as4_comm_result->IR_Temp;
//	can_data.data[2] = as4_comm_result->AS4_Final_SendData.LayerTemp >>8;
    Send_Can_Data(&(can_data));
}
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理 warning
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/25 Create
  * @Version 	1.0 2020/08/25
  *		     	1.1 2020/08/25 change sth
  * @Note

  **/
#if 0
int CAN_SEND_Warning(struct _AS4_comm_result *as4_comm_result)
{
    struct can_frame can_data;
    can_data.can_dlc=8;
    can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
    can_data.data[0]=CMD_Warning;
    memcpy(&can_data.data[1], as4_comm_result->AS4_Final_SendData.workExcep, 7 );
//	can_data.data[2] = as4_comm_result->AS4_Final_SendData.LayerTemp >>8;
    Send_Can_Data(&(can_data));
}
#endif
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 发送长数据申请
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/25 Create
  * @Version 	1.0 2020/08/25
  *		     	1.1 2020/08/25 change sth
  * @Note
  **/


DataBufType*        Codedata;
TRANSMIT_CTRL       CAN_CodeTrans,CAN_BinTrans,CAN_CodeTrans_Pipe,CAN_CodeTrans_Welder,CAN_CodeTrans_ProjectID,
                    CAN_CodeTrans_TeamCode,CAN_CodeTrans_Process,CAN_CodeTrans_EquipCode,CAN_CodeTrans_BoxID,
                    CAN_CodeTrans_HjProcess,CAN_CodeTrans_Warning,CAN_CodeTrans_Time,
                    CAN_CodeTrans_SignalStrength,CAN_CodeTrans_ICCID,CAN_CodeTrans_CPSI,CAN_CodeTrans_LBSx,CAN_CodeTrans_LBSy,
                    CAN_CodeTrans_IMEI,CAN_CodeTrans_VersionSM,CAN_CodeTrans_VersionAS;
DataBufType DecodeData;
TRANSMIT_CTRL  CodeTransCtrl,BinTransCtrl;



uint8_t _ReadDecodeData(DataBufType** Codedata,struct _AS4_comm_result *as4_comm_result,u8 cmdSwitch)
{
    int Datalen=0;
    switch (cmdSwitch)
    {
    case 0:
        //char test[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890数据采集系统9876543210";
        Datalen=sizeof(as4_comm_result->AS4_Final_SendData.WELD_CODE);
        //PX("Datalen=%d\n",Datalen);
        DecodeData.DLC=Datalen;
        //DecodeData.Readflag=NRead;
        memcpy(&DecodeData.CodeData, &as4_comm_result->AS4_Final_SendData.WELD_CODE,MaxDatalen);


        break;
    case 1:
        //char test[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890数据采集系统9876543210";
        Datalen=sizeof(as4_comm_result->AS4_Final_SendData.PERSON_CODE);
        //PX("Datalen=%d\n",Datalen);
        DecodeData.DLC=Datalen;
        //DecodeData.Readflag=NRead;
        memcpy(&DecodeData.CodeData, &as4_comm_result->AS4_Final_SendData.PERSON_CODE,MaxDatalen);

        break;
    }


    //PX("DecodeData.CodeData=%s--DecodeData.DLC=%d\n",DecodeData.CodeData, DecodeData.DLC);

    if(DecodeData.Readflag==NRead)
    {
        *Codedata=&DecodeData;
        DecodeData.Readflag=0;
        return Readok;
    }
    return Readerr;
}

void _WRITEDecodeData(uint8_t* Databuf,uint8_t BufCtr,uint8_t DLC,uint8_t Readflag)
{
    if(Readflag!=NRead)
    {
        memcpy(&DecodeData.CodeData[BufCtr],Databuf,DLC);
    }
    else
    {
        DecodeData.Readflag=Readflag;
        DecodeData.DLC=DLC;
    }
}

/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信10ms 发送长数据申请
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/26 Create
  * @Version 	1.0 2020/08/26
  *		     	1.1 2020/08/26 change sth
  * @Note
  **/
void _SEND_CodeApply(u32 Datalen,u32 Datasum,u8 CmdApply)
{
//	 if(CanStatus.OnLineSta==STA_ON)
    {

        struct can_frame can_data;
        can_data.can_dlc=8;
        can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
        can_data.data[0]=CMD_ParaToLCD;
        can_data.data[1]=CmdApply;
        memcpy(&can_data.data[2],&Datalen,2 );
        memcpy(&can_data.data[4],&Datasum,4 );
        //	can_data.data[2] = as4_comm_result->AS4_Final_SendData.LayerTemp >>8;
        Send_Can_Data(&(can_data));
    }

}
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信 发送长数据
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/26 Create
  * @Version 	1.0 2020/08/26
  *		     	1.1 2020/08/26 change sth
  * @Note
  **/
void _SEND_CodeData(u32 Datalen,u8* Dbuf,u32 DataCnt,u8 CmdData)
{
//	 if(CanStatus.OnLineSta==STA_ON)
    {

        struct can_frame can_data;
        can_data.can_dlc=(Datalen+CAN_FRAME_WIDTH-MaxCanbyte)&0xff;
        can_data.can_id=HOSTtoLayerAddr;
        //*(u32*)&TransmitID3;
        can_data.data[0]=CMD_ParaToLCD;
        can_data.data[1]=CmdData;
        can_data.data[2] =DataCnt&0xff;
        memcpy(&can_data.data[3],Dbuf,Datalen );
        Send_Can_Data(&(can_data));
    }
}


void _CodeApply_Task(u8 CmdApply,char* data,TRANSMIT_CTRL * CAN_Trans,int DataLen)
{
    u16 cnt;
    long Checksum=0x10000000;
    //pthread_mutex_lock(&mutex);
    //PX("_CodeApply_Task strlen=%d--data=%s\n",Datalen,DecodeData.CodeData);
    // if(_ReadDecodeData(&Codedata,as4_comm_result,1)==Readok)

    {
        for(cnt=0; cnt<DataLen; cnt++)
        {
            Checksum-=data[cnt];//as4_comm_result->AS4_Final_SendData.WELD_CODE[cnt];
        }
        _SEND_CodeApply(DataLen,Checksum,CmdApply);
        //PX("send _SEND_CodeApply succ , Codedata->DLC=%d--Checksum=%d--Codedata->CodeData=%s\n" , Codedata->DLC,Checksum,Codedata->CodeData);
        memset(CAN_Trans,0,sizeof(TRANSMIT_CTRL));
        CAN_Trans->Datalen=DataLen;
    }
    //pthread_mutex_unlock(&mutex);
}
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信 发送请求标志 准备发送
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/27 Create
  * @Version 	1.0 2020/08/27
  *		     	1.1 2020/08/27 change sth
  * @Note
  **/

pthread_mutex_t mutex;

u8 ReadySendCanSts(TRANSMIT_CTRL * CAN_Trans)
{
    //pthread_mutex_lock(&mutex);
    CAN_Trans->SendSts = NoSend;
    //pthread_mutex_unlock(&mutex);
}


static u8 _CodeData_Task(u8 CmdData,char* data,TRANSMIT_CTRL * CAN_Trans)
{
    //pthread_mutex_lock(&mutex);
    u8 Tempcnt;
    short Sendbyte;
    long Lastbyte;
    Lastbyte=CAN_Trans->Datalen - CAN_Trans->Datacnt;
    if(Lastbyte<=MaxCanbyte)
        Sendbyte=Lastbyte;
    else
        Sendbyte=MaxCanbyte;
    // PX("here is Sendbyte=MaxCanbyte;CAN_CodeTrans.CTSflag=%d\n",CAN_CodeTrans.CTSflag);
    if(CAN_Trans->CTSflag==ACK_SUCC)
    {
        CAN_Trans->CTSflag=0;
        if(CAN_Trans->Datacnt<CAN_Trans->Datalen)
        {
            Tempcnt=CAN_Trans->Framecnt&0xff;
            //PX("Tempcnt=CAN_CodeTrans.Framecnt=%d\n",Tempcnt);
            _SEND_CodeData(Sendbyte,data+CAN_Trans->Datacnt,Tempcnt,CmdData);
            CAN_Trans->Framecnt++;
            CAN_Trans->Datacnt+=Sendbyte;
            // PX("(_CodeData_Task)return 0 add=%p\n",&CAN_Trans);
            return 0;
        }
        else if(CAN_Trans->Datacnt==CAN_Trans->Datalen)
        {
            //PX("(_CodeData_Task)return SEND_OK\n");
            return SEND_OK;
        }
    }
    else if(CAN_Trans->CTSflag==ACK_ERR)
    {
        memset(CAN_Trans,0,sizeof(TRANSMIT_CTRL));
        PX("(_CodeData_Task)CAN_Trans->CTSflag==ACK_ERR\n");
        PX("(_CodeData_Task)add=%p-name=%s--CMD=0x%X\n",&CAN_Trans,data,CmdData);
        ReadySendCanSts(CAN_Trans);
    }
    // PX("(_CodeData_Task)return SEND_ERR\n");
// pthread_mutex_unlock(&mutex);
//pthread_mutex_destroy(&mutex);
    return SEND_ERR;
}

/**

  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信1hz 任务管理
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/05/22 Create
  * @Version 	1.0 2020/05/22
  *		     	1.1 2020/05/22 change sth
  * @Note

  **/

void *  CAN_TASK1000ms (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    pthread_t pid1;
    pthread_create(&pid1,0,CAN_SEND_SCANNER,as4_comm_result);

    while(1)
    {
        _CommWatchDog_TASK(as4_comm_result);
        weld_position_check(as4_comm_result);

        CAN_SEND_SYS_Temp(as4_comm_result);
        usleep(50000);
        CAN_SEND_SYS_Hmi(as4_comm_result);
        usleep(50000);
        CAN_SEND_Position(as4_comm_result);
        usleep(50000);
        CAN_SEND_Voltage(as4_comm_result);
        usleep(50000);
        CAN_SEND_Current(as4_comm_result);
        usleep(50000);
        CAN_SEND_SS_SPEED(as4_comm_result);
        usleep(50000);
        CAN_SEND_HJ_SPEED(as4_comm_result);
        usleep(50000);
        CAN_SEND_LayerTemp(as4_comm_result);
        usleep(50000);
        //_CodeApply_Task();
        //PX("cur layer =%s\n"  , as4_comm_result->Calc_Data.Now_LAYER);//
        usleep(100000);
    }
}
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信 判断是否准备发送
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/08/27 Create
  * @Version 	1.0 2020/08/27
  *		     	1.1 2020/08/27 change sth
  * @Note
  **/

u8 IsSendCanData(u8 CmdApply,u8 CmdData,char* data,TRANSMIT_CTRL * CAN_Trans,int DataLen)
{
    pthread_mutex_lock(&mutex);
    if( CAN_Trans->SendSts == NoSend)
    {
        _CodeApply_Task(CmdApply,data,CAN_Trans,DataLen);
    }
    _CodeData_Task(CmdData,data,CAN_Trans);
    pthread_mutex_unlock(&mutex);
}
/*
void *  CAN_CMD_Apply (void *arg)
{
  struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
  while(1)
  {
	  ReadySendCanSts(&CAN_CodeTrans_Welder);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_Pipe);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_Process);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_EquipCode);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_BoxID);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_Time);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_TeamCode);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_Warning);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_HjProcess);
	  sleep(1);
	  ReadySendCanSts(&CAN_CodeTrans_ProjectID);
	  sleep(1);

  }
}
*/
void *  CAN_CMD_Time (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_Time);
        sleep(1);
    }
}
void *  CAN_SEND_Time (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_TimeApply,CMD_Time,as4_comm_result->AS4_Final_SendData.CJ_TIME,&CAN_CodeTrans_Time,30);
    }
}

void *  CAN_CMD_Pipe (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_Pipe);
        sleep(1);
    }
}
void *  CAN_SEND_Pipe (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_PipeApply,CMD_Pipe,as4_comm_result->AS4_Final_SendData.WELD_CODE,&CAN_CodeTrans_Pipe,60);

    }
}



void *  CAN_CMD_Welder (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_Welder);
        sleep(1);
    }
}
void *  CAN_SEND_Welder (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_WelderApply,CMD_Welder,as4_comm_result->AS4_Final_SendData.PERSON_CODE,&CAN_CodeTrans_Welder,60);

    }
}



void *  CAN_CMD_Process (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_Process);
        sleep(1);
    }
}
void *  CAN_SEND_Process (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_ProcessApply,CMD_Process,as4_comm_result->AS4_Final_SendData.PROCESS,&CAN_CodeTrans_Process,20);
    }
}



void *  CAN_CMD_EquipCode (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_EquipCode);
        sleep(10);
    }
}
void *  CAN_SEND_EquipCode (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_DeviceIDApply,CMD_DeviceID,as4_comm_result->AS4_Final_SendData.EQUIP_CODE,&CAN_CodeTrans_EquipCode,20);
    }
}



void *  CAN_CMD_BoxID (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_BoxID);
        sleep(150);
    }
}
void *  CAN_SEND_BoxID (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_DataBoxIDApply,CMD_DataBoxID,as4_comm_result->AS4_Final_SendData.DATASBOX_CODE,&CAN_CodeTrans_BoxID,20);

    }
}



void *  CAN_CMD_TeamCode (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_TeamCode);
        sleep(1);
    }
}
void *  CAN_SEND_TeamCode (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_TeamCodeApply,CMD_TeamCode,as4_comm_result->AS4_Final_SendData.TEAM_CODE,&CAN_CodeTrans_TeamCode,20);

    }
}



void *  CAN_CMD_Warning (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_Warning);
        //PX("workExcep=%s\n",as4_comm_result->AS4_Final_SendData.workExcep);
        sleep(1);
    }
}
void *  CAN_SEND_Warning (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    int len;

    while(1)
    {
        usleep(1000);
        //len = strlen(as4_comm_result->AS4_Final_SendData.workExcep);as4_comm_result->AS4_Final_SendData.workExcep
        //PX("len_CMD_Warning=%d",len);
        IsSendCanData(CMD_WarningApply,CMD_Warning,as4_comm_result->AS4_Final_SendData.workExcep,&CAN_CodeTrans_Warning,40);
    }
}



void *  CAN_CMD_HjProcess (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_HjProcess);
        sleep(5);
    }
}
void *  CAN_SEND_HjProcess (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_HJ_ProcessApply,CMD_HJ_Process,as4_comm_result->AS4_Final_SendData.HJ_PROCESS,&CAN_CodeTrans_HjProcess,20);
    }
}



void *  CAN_CMD_ProjectID (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_ProjectID);
        sleep(5);
    }
}
void *  CAN_SEND_ProjectID (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_ProjectIDApply,CMD_ProjectID,as4_comm_result->AS4_Final_SendData.PROJECT_ID,&CAN_CodeTrans_ProjectID,20);
    }
}


void *  CAN_CMD_SignalStrength(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_SignalStrength);
        sleep(10);
    }
}
void *  CAN_SEND_SignalStrength(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_SingalStrengthApply,CMD_SingalStrength,as4_comm_result->Singal_Strength,&CAN_CodeTrans_SignalStrength,5);

    }
}



void *  CAN_CMD_ICCID(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_ICCID);
        sleep(13);
    }
}
void *  CAN_SEND_ICCID(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_ICCIDApply,CMD_ICCID,as4_comm_result->XGDeviceRcvData.CICCID,&CAN_CodeTrans_ICCID,30);
    }
}



void *  CAN_CMD_CPSI(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_CPSI);
        sleep(12);
    }
}
void *  CAN_SEND_CPSI(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_CPSIApply,CMD_CPSI,as4_comm_result->XGDeviceRcvData.CPSI,&CAN_CodeTrans_CPSI,100);
    }
}



void *  CAN_CMD_LBSx(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_LBSx);
        sleep(20);
    }
}
void *  CAN_SEND_LBSx(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_LBSxApply,CMD_LBSx,as4_comm_result->LBS_X,&CAN_CodeTrans_LBSx,20);

    }
}



/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	LBSY发送
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/09/ Create
  * @Version 	1.0 2020/09/07
  *		     	1.1 2020/07/07 change sth
  * @Note
  **/

void *  CAN_CMD_LBSy(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_LBSy);
        sleep(20);
    }
}
void *  CAN_SEND_LBSy(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_LBSyApply,CMD_LBSy,as4_comm_result->LBS_Y,&CAN_CodeTrans_LBSy,20);

    }
}



/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	IMEI 4G模块发送
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/09/ Create
  * @Version 	1.0 2020/09/07
  *		     	1.1 2020/07/07 change sth
  * @Note
  **/

void *  CAN_CMD_IMEI(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_IMEI);
        sleep(8);
    }
}
void *  CAN_SEND_IMEI(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_IMEIApply,CMD_IMEI,as4_comm_result->XGDeviceRcvData.IMEI,&CAN_CodeTrans_IMEI,20);

    }
}


/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	SM版本发送
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/09/ Create
  * @Version 	1.0 2020/09/07
  *		     	1.1 2020/07/07 change sth
  * @Note
  **/

void *  CAN_CMD_VerSM(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_VersionSM);
        sleep(9);
    }
}
void *  CAN_SEND_VerSM(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_VersionSMApply,CMD_VersionSM,SW_date,&CAN_CodeTrans_VersionSM,40);

    }
}


/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	AS版本发送
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/09/ Create
  * @Version 	1.0 2020/09/07
  *		     	1.1 2020/07/07 change sth
  * @Note
  **/
void *  CAN_CMD_VerAS(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        ReadySendCanSts(&CAN_CodeTrans_VersionAS);
        sleep(10);
    }
}
void *  CAN_SEND_VerAS(void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    while(1)
    {
        usleep(1000);
        IsSendCanData(CMD_VersionASApply,CMD_VersionAS,as4_comm_result->VersionAS,&CAN_CodeTrans_VersionAS,40);
    }
}

void *  CAN_TASK10ms (void *arg)
{
    struct _AS4_comm_result *as4_comm_result = (struct _AS4_comm_result *)arg;
    /*依次循环进行数据调用 一次只能调用一个数据进行发送*/
    pthread_mutex_init(&mutex,NULL);
#if 1
    pthread_t pid2;
    pthread_create(&pid2,0,CAN_CMD_Time,as4_comm_result);
    pthread_t pidTime;
    pthread_create(&pidTime,0,CAN_SEND_Time,as4_comm_result);


    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid3;
    pthread_create(&pid3,0,CAN_CMD_Pipe,as4_comm_result);
    pthread_t pidPipe;
    pthread_create(&pidPipe,0,CAN_SEND_Pipe,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/


    pthread_t pid4;
    pthread_create(&pid4,0,CAN_CMD_Welder,as4_comm_result);
    pthread_t pidWelder;
    pthread_create(&pidWelder,0,CAN_SEND_Welder,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid5;
    pthread_create(&pid5,0,CAN_CMD_Process,as4_comm_result);
    pthread_t pidProcess;
    pthread_create(&pidProcess,0,CAN_SEND_Process,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid6;
    pthread_create(&pid6,0,CAN_CMD_EquipCode,as4_comm_result);
    pthread_t pidEquipCode;
    pthread_create(&pidEquipCode,0,CAN_SEND_EquipCode,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid7;
    pthread_create(&pid7,0,CAN_CMD_BoxID,as4_comm_result);
    pthread_t pidBoxID;
    pthread_create(&pidBoxID,0,CAN_SEND_BoxID,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid8;
    pthread_create(&pid8,0,CAN_CMD_TeamCode,as4_comm_result);
    pthread_t pidTeamCode;
    pthread_create(&pidTeamCode,0,CAN_SEND_TeamCode,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid9;
    pthread_create(&pid9,0,CAN_CMD_Warning,as4_comm_result);
    pthread_t pidWarning;
    pthread_create(&pidWarning,0,CAN_SEND_Warning,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid10;
    pthread_create(&pid10,0,CAN_CMD_HjProcess,as4_comm_result);
    pthread_t pidHjProcess;
    pthread_create(&pidHjProcess,0,CAN_SEND_HjProcess,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid11;
    pthread_create(&pid11,0,CAN_CMD_ProjectID,as4_comm_result);
    pthread_t pidProjectID;
    pthread_create(&pidProjectID,0,CAN_SEND_ProjectID,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/

    pthread_t pid12;
    pthread_create(&pid12,0,CAN_CMD_SignalStrength,as4_comm_result);
    pthread_t pidSignalStrength;
    pthread_create(&pidSignalStrength,0,CAN_SEND_SignalStrength,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid13;
    pthread_create(&pid13,0,CAN_CMD_ICCID,as4_comm_result);
    pthread_t pidICCID;
    pthread_create(&pidICCID,0,CAN_SEND_ICCID,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid14;
    pthread_create(&pid14,0,CAN_CMD_CPSI,as4_comm_result);
    pthread_t pidCPSI;
    pthread_create(&pidCPSI,0,CAN_SEND_CPSI,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid15;
    pthread_create(&pid15,0,CAN_CMD_LBSx,as4_comm_result);
    pthread_t pidLBSx;
    pthread_create(&pidLBSx,0,CAN_SEND_LBSx,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid16;
    pthread_create(&pid16,0,CAN_CMD_LBSy,as4_comm_result);
    pthread_t pidLBSy;
    pthread_create(&pidLBSy,0,CAN_SEND_LBSy,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid17;
    pthread_create(&pid17,0,CAN_CMD_IMEI,as4_comm_result);
    pthread_t pidIMEI;
    pthread_create(&pidIMEI,0,CAN_SEND_IMEI,as4_comm_result);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/

    pthread_t pid18;
    pthread_create(&pid18,0,CAN_CMD_VerAS,as4_comm_result);
    pthread_t pidAS;
    pthread_create(&pidAS,0,CAN_SEND_VerAS,as4_comm_result);

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`*/
    pthread_t pid19;
    pthread_create(&pid19,0,CAN_CMD_VerSM,as4_comm_result);
    pthread_t pidVerSM;
    pthread_create(&pidVerSM,0,CAN_SEND_VerSM,as4_comm_result);
#endif

#if 0
    pthread_t pid20;
    pthread_create(&pid20,0,CAN_CMD_Apply,as4_comm_result);
#endif

    while(1)
    {

        sleep(1);
#if 0
        PX("TempSingalStrength=%s--%d\n",TempSingalStrength,as4_comm_result->XGDeviceRcvData.SignalStrength);
        PX("ICCID=%s\n",as4_comm_result->XGDeviceRcvData.CICCID);
        PX("CPSI=%s\n",as4_comm_result->XGDeviceRcvData.CPSI);
        PX("LBS_X=%s--%f\n",LBS_X,as4_comm_result->XGDeviceRcvData.LBS.x);
        PX("LBS_Y=%s--%f\n",LBS_Y,as4_comm_result->XGDeviceRcvData.LBS.y);
        PX("IMEI=%s\n",as4_comm_result->XGDeviceRcvData.IMEI);
#endif

#if 0
        usleep(1000);
        IsSendCanData(CMD_WelderApply,CMD_Welder,as4_comm_result->AS4_Final_SendData.PERSON_CODE,&CAN_CodeTrans_Welder,60);
        usleep(1000);
        IsSendCanData(CMD_PipeApply,CMD_Pipe,as4_comm_result->AS4_Final_SendData.WELD_CODE,&CAN_CodeTrans_Pipe,60);
        usleep(1000);
        IsSendCanData(CMD_ProcessApply,CMD_Process,as4_comm_result->AS4_Final_SendData.PROCESS,&CAN_CodeTrans_Process,20);
        usleep(1000);
        IsSendCanData(CMD_DeviceIDApply,CMD_DeviceID,as4_comm_result->AS4_Final_SendData.EQUIP_CODE,&CAN_CodeTrans_EquipCode,20);
        usleep(1000);
        IsSendCanData(CMD_DataBoxIDApply,CMD_DataBoxID,as4_comm_result->AS4_Final_SendData.DATASBOX_CODE,&CAN_CodeTrans_BoxID,20);
        usleep(1000);
        IsSendCanData(CMD_TimeApply,CMD_Time,as4_comm_result->AS4_Final_SendData.CJ_TIME,&CAN_CodeTrans_Time,30);
        usleep(1000);
        IsSendCanData(CMD_TeamCodeApply,CMD_TeamCode,as4_comm_result->AS4_Final_SendData.TEAM_CODE,&CAN_CodeTrans_TeamCode,20);
        usleep(1000);
        IsSendCanData(CMD_WarningApply,CMD_Warning,as4_comm_result->AS4_Final_SendData.workExcep,&CAN_CodeTrans_Warning,40);
        usleep(1000);
        IsSendCanData(CMD_HJ_ProcessApply,CMD_HJ_Process,as4_comm_result->AS4_Final_SendData.HJ_PROCESS,&CAN_CodeTrans_HjProcess,20);
        usleep(1000);
        IsSendCanData(CMD_ProjectIDApply,CMD_ProjectID,as4_comm_result->AS4_Final_SendData.PROJECT_ID,&CAN_CodeTrans_ProjectID,20);

        usleep(1000);
        IsSendCanData(CMD_SingalStrengthApply,CMD_SingalStrength,as4_comm_result->Singal_Strength,&CAN_CodeTrans_SignalStrength,5);
        usleep(1000);
        IsSendCanData(CMD_ICCIDApply,CMD_ICCID,as4_comm_result->XGDeviceRcvData.CICCID,&CAN_CodeTrans_ICCID,30);
        usleep(1000);
        IsSendCanData(CMD_CPSIApply,CMD_CPSI,as4_comm_result->XGDeviceRcvData.CPSI,&CAN_CodeTrans_CPSI,100);
        usleep(1000);
        IsSendCanData(CMD_LBSxApply,CMD_LBSx,as4_comm_result->LBS_X,&CAN_CodeTrans_LBSx,20);
        usleep(1000);
        IsSendCanData(CMD_LBSyApply,CMD_LBSy,as4_comm_result->LBS_Y,&CAN_CodeTrans_LBSy,20);
        usleep(1000);
        IsSendCanData(CMD_IMEIApply,CMD_IMEI,as4_comm_result->XGDeviceRcvData.IMEI,&CAN_CodeTrans_IMEI,20);

        usleep(1000);
        IsSendCanData(CMD_VersionASApply,CMD_VersionAS,as4_comm_result->VersionAS,&CAN_CodeTrans_VersionAS,40);
        usleep(1000);
        IsSendCanData(CMD_VersionSMApply,CMD_VersionSM,SW_date,&CAN_CodeTrans_VersionSM,40);
#endif
    }
}





const ECANID_INTERNAL ReceiveLayerID= {0x03,0x08,0x0a};

uint8_t CanReceiveProcess(struct _AS4_comm_result *as4_comm_result )
{
    uint8_t Ackflag=0,Tmpid;
    ECANID_INTERNAL CID;
    CANBufType  CANRxData,CANTxData;
    memset(&CANRxData,0,sizeof(CANRxData));
    if(Can_RxMsg(&CANRxData)!=RX_NO_ERR)
        return 0;
    memcpy(&CID,&CANRxData.MSGID,2);
    switch(CANRxData.FrameData[0])
    {
    case CMD_SendAck://0x81
        switch(CANRxData.FrameData[1])
        {

        case CMD_CodeApply://0x30应答发送扫码数据申请
            CAN_CodeTrans.CTSflag=CANRxData.FrameData[2];//CANRxData.FrameData[2];
            //PX("REV CMD_CodeApply from databox,--CAN_CodeTrans.CTSflag=%d\n",CAN_CodeTrans.CTSflag);
            break;
        case CMD_CodeData://应答发送扫码数据
            CAN_CodeTrans.CTSflag=CANRxData.FrameData[2];//CANRxData.FrameData[2];
            //PX("rcv CMD_CodeData from databox--CAN_CodeTrans.CTSflag=%d\n",CAN_CodeTrans.CTSflag);
            break;
        case CMD_BinApply:
            if( CANRxData.MSGID == LayerDevToHostAddr )//((ECANID_INTERNAL*)&ReceiveLayerID )
            {

                CAN_BinTrans.CTSflag=CANRxData.FrameData[2];
                PX("readY to send BinData---CAN_BinTrans.CTSflag=%x\n",CAN_BinTrans.CTSflag);
            }
            break;
        case CMD_BinData:

            if( CANRxData.MSGID == LayerDevToHostAddr )//((ECANID_INTERNAL*)&ReceiveLayerID )
            {
                CAN_BinTrans.SendSts=CANRxData.FrameData[2];
                if( CAN_BinTrans.SendSts == Data_Err)
                {
                    //此处不添加大括号会报错 why？

                    PX("!");
                }
                else
                {
                    PX("*");
                }

            }
            break;

        case CMD_ParaToLCD://LCD参数
            switch(CANRxData.FrameData[2])
            {

            case CMD_Layer:
                // PX("rcv cmd CMD_Layer CMD_SendAck!\n");
                as4_comm_result->Can_Layer_Data.rcv_sts = SendDataSUCC ;
                break;
            case CMD_Pipe:
                //pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Pipe.CTSflag=CANRxData.FrameData[3];//PX("REV CMD_CodeApply from databox,--CAN_CodeTrans.CTSflag=%d\n",CAN_CodeTrans.CTSflag);
                //pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Pipe from databox,--CAN_CodeTrans_Pipe.CTSflag=%d\n",CAN_CodeTrans_Pipe.CTSflag);
                break;
            case CMD_PipeApply:
                //pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Pipe.CTSflag=CANRxData.FrameData[3];//PX("rcv CMD_CodeData from databox--CAN_CodeTrans.CTSflag=%d\n",CAN_CodeTrans.CTSflag);
                //pthread_mutex_unlock(&mutex);
                //PX("REV CMD_CodeApply from databox,--CAN_CodeTrans_Pipe.CTSflag=%d\n",CAN_CodeTrans_Pipe.CTSflag);
                break;

            case CMD_Welder:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Welder.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_Welder.CTSflag=%d\n",CAN_CodeTrans_Welder.CTSflag);
                break;
            case CMD_WelderApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Welder.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_Welder.CTSflag=%d\n",CAN_CodeTrans_Welder.CTSflag);
                break;
            case CMD_Process:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Process.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_Process.CTSflag=%d\n",CAN_CodeTrans_Process.CTSflag);
                break;
            case CMD_ProcessApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Process.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_Welder.CTSflag=%d\n",CAN_CodeTrans_Process.CTSflag);
                break;

            case CMD_DeviceID:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_EquipCode.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_EquipCode.CTSflag=%d\n",CAN_CodeTrans_EquipCode.CTSflag);
                break;
            case CMD_DeviceIDApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_EquipCode.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_EquipCode.CTSflag=%d\n",CAN_CodeTrans_EquipCode.CTSflag);
                break;

            case CMD_DataBoxID:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_BoxID.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_BoxID.CTSflag=%d\n",CAN_CodeTrans_BoxID.CTSflag);
                break;
            case CMD_DataBoxIDApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_BoxID.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_BoxID.CTSflag=%d\n",CAN_CodeTrans_BoxID.CTSflag);
                break;

            case CMD_Time:
                //pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Time.CTSflag=CANRxData.FrameData[3];
                //pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_Time.CTSflag=%d\n",CAN_CodeTrans_Time.CTSflag);
                break;
            case CMD_TimeApply:
                //pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Time.CTSflag=CANRxData.FrameData[3];
                //pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_Time.CTSflag=%d\n",CAN_CodeTrans_Time.CTSflag);
                break;

            case CMD_TeamCode:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_TeamCode.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_TeamCode.CTSflag=%d\n",CAN_CodeTrans_TeamCode.CTSflag);
                break;
            case CMD_TeamCodeApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_TeamCode.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_TeamCode.CTSflag=%d\n",CAN_CodeTrans_TeamCode.CTSflag);
                break;

            case CMD_Warning:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Warning.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_Warning.CTSflag=%d\n",CAN_CodeTrans_Warning.CTSflag);
                break;
            case CMD_WarningApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_Warning.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_Warning.CTSflag=%d\n",CAN_CodeTrans_Warning.CTSflag);
                break;

            case CMD_HJ_Process:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_HjProcess.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_HjProcess.CTSflag=%d\n",CAN_CodeTrans_HjProcess.CTSflag);
                break;
            case CMD_HJ_ProcessApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_HjProcess.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_HjProcess.CTSflag=%d\n",CAN_CodeTrans_HjProcess.CTSflag);
                break;

            case CMD_ProjectID:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_ProjectID.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_Welder from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_ProjectIDApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_ProjectID.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;



            case CMD_SingalStrength:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_SignalStrength.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_SingalStrengthApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_SignalStrength.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;

            case CMD_ICCID:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_ICCID.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_ICCIDApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_ICCID.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;

            case CMD_CPSI:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_CPSI.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_CPSIApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_CPSI.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;

            case CMD_LBSx:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_LBSx.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_LBSxApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_LBSx.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;

            case CMD_LBSy:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_LBSy.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_LBSyApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_LBSy.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;

            case CMD_IMEI:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_IMEI.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_IMEIApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_IMEI.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;

            case CMD_VersionAS:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_VersionAS.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_VersionASApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_VersionAS.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;

            case CMD_VersionSM:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_VersionSM.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            case CMD_VersionSMApply:
                pthread_mutex_lock(&mutex);
                CAN_CodeTrans_VersionSM.CTSflag=CANRxData.FrameData[3];
                pthread_mutex_unlock(&mutex);
                //PX("REV CMD_WelderApply from databox,--CAN_CodeTrans_ProjectID.CTSflag=%d\n",CAN_CodeTrans_ProjectID.CTSflag);
                break;
            }
            break;

        }
        break;
    case CMD_Heartbeat://0x80//
        if(CANRxData.MSGID ==LayerDevToHostAddr)
        {
            _CommWatchDog_Feed();
            //	PX("CMD_Heartbeat=%x\n" , CANRxData.MSGID);
        }
        break;

    case CMD_GetReady://0x01
        if(CANRxData.MSGID ==LayerDevToHostAddr)
        {
            CAN_SEND_LAYER(as4_comm_result);
            PX("layerDev now is online\n");
            XGPutToLog(xglogdata.log_handle,"(CanReceiveProcess)layerDev now is online\n");
        }

        break;

    case CMD_ParaToLCD://LCD参数
        switch(CANRxData.FrameData[1])
        {
        case CMD_Layer:
            parase_can_rcv_layerdata(&CANRxData,as4_comm_result);
            // PX("Rcv layer succ\n");
            _SEND_ACK();
            break;
        }

    }
    return 1;
}
int  can_rcv(struct can_frame *can_orgdata)
{
    CANBufType TempRxBuf;
    Can_Rx_Msg( can_orgdata, &TempRxBuf);
    ArrayPutFrame(&CANRxType,&TempRxBuf);
    return 0 ;
}


int process_dpscmd(struct CAN_data *recdata,struct dps_comm_result *result )//struct CAN_data *recdata
{
    char cmd[50];
    short int dps_16;
    unsigned char *buffer_data =  recdata->buffer_data;
    int ret = 0;
    static int vol_sts;

    switch(buffer_data[0])
    {
    //PX("@@@@@@@rcv can data 	recdata->target=%s\n" , recdata->target );
    case 0x80://心跳帧
    {
        result->heart_check = buffer_data[1];
        if(DEV_DOWN == result->boot_s)
        {
            result->boot_s = DEV_ERR;
        }
        dps_heart_to_queue(result,buffer_data[1]);
        //printx("DPS-01-Heart:%x,status:%x\n",buffer_data[1],buffer_data[2]);
        break;
    }

    case 0x81://应答帧
    {
        if((buffer_data[1] == 0x40)||
                (buffer_data[1] == 0x41)||
                (buffer_data[1] == 0x42))
        {
            if(buffer_data[1] == 0x40)//应答发送BIN数据申请
            {
                if(buffer_data[2] == 0xAA)
                {
                    printf("1\n");
                    result ->Updata_Prgram.send_bin_apply_sta = Data_On;
                }
                else if(buffer_data[2] == 0xEE)
                {
                    printf("2\n");
                    result ->Updata_Prgram.send_bin_apply_sta = Data_Err;
                }
            }
            else if(buffer_data[1] == 0x41)//应答发送BIN数据,byte[2]= 0xAA：数据正常，byte[2]=0xEE:数据有误。
            {
                if(buffer_data[2] == 0xAA)
                {
                    // printf("3\n");
                    result ->Updata_Prgram.send_bin_sta = Data_On;
                }
                else if(buffer_data[2] == 0xEE)
                {
                    //printf("4");
                    result ->Updata_Prgram.send_bin_sta = Data_Err;
                }
            }
            else if(buffer_data[1] == 0x42)//应答重置设备地址,byte[2]=0xAA：重置正常，byte[2]=0xEE:重置有误
            {
                if(buffer_data[2] == 0xAA)
                {
                    printf("5\n");
                    result->DADDR_Data.reset_add_sta = Data_On;
                }
                else if(buffer_data[2] == 0xEE)
                {
                    printf("6\n");
                    result->DADDR_Data.reset_add_sta = Data_Err;
                }
            }
            else
            {

            }
            result->back_result = pRESULT_OK;
        }
        else
        {
            result->back_result = pRESULT_FAILED;
        }
        break;
    }
    case 0x97://电流电压
    {
        //PX("@@@@@@@rcv can data 	recdata->target=%d\n" , recdata->target );

        if( 0x0f == recdata->target )//焊机发送电流电压数据
        {
            vol_sts = 1 ;
            //PX("VAL AND CUR FROM WELDER\n");
            dps_send_current_vlot(buffer_data);
        }
        else if( (0x0a == recdata->target) &&  (vol_sts == 0 ) )//数据采集板发送电流电压数据
        {
            //PX("VAL AND CUR FROM DATABOX\n");
            dps_send_current_vlot(buffer_data);
        }
        break;
    }
    case 0x94://传感器角度反馈值帧
    {
        dps_send_WeldAngle(buffer_data);
        break;
    }
    case 0x95://传感器温度反馈值帧
    {
        dps_send_LayerTemp(buffer_data);
        break;
    }
    case 0x30://接收扫码数据申请
    {
        result->Scan_Data.ScanDataLen=
            (((unsigned int)buffer_data[3]&(0x000000FF))<<16)	|
            (((unsigned int)buffer_data[2]&(0x000000FF))<<8)	|
            (((unsigned int)buffer_data[1]&(0x000000FF)));
        result->Scan_Data.Checksum =
            (((unsigned int)buffer_data[7]&(0x000000FF))<<24)	|
            (((unsigned int)buffer_data[6]&(0x000000FF))<<16)	|
            (((unsigned int)buffer_data[5]&(0x000000FF))<<8)	|
            (((unsigned int)buffer_data[4]&(0x000000FF)));
        printx("DPS:%x--ScanDataLen=%d---Checksum=%d\n",result->my_id,result->Scan_Data.ScanDataLen,result->Scan_Data.Checksum);
        result->Scan_Data.recvDataApply = P_On;
        SEND_ScannerApplyACK_Rawcan(result);
        break;
    }
    case 0x31://接收扫码数据
    {
        dps_Scanner_to_queue(result,buffer_data);
        break;
    }
    case 0x9A://接收无线通信连接状态
    {
        printx("wireless_communication\n");
        break;
    }
    case 0x96://CW或者CCW
    {
        dps_send_weldPosition(buffer_data);
        break;
    }

    default:
        //printx("dps-01:cmd error not with:%x\n",buffer_data[0]);
        ret = -1;
        break;
    }
    return ret;
}




void process_power_cmd(struct dps_comm_result *result,char *buffer)
{
    char findcmd[BUFFER_SIZE];
    int ret,find_read;
    unsigned char buffer_data[8];
    struct CAN_data rec_candata;
    struct can_frame *can_orgdata  ;
    //can_orgdata = datagogogo(can_orgdata) ;

    do
    {
        memset(findcmd,'\0',BUFFER_SIZE);
        find_read = finddata(findcmd,&buffer);
        if(find_read > 0)
        {
            ret = data_from_power(findcmd,&rec_candata);
            if(0 < ret)
            {

                if(0 > process_dpscmd(&rec_candata,result ))
                {
                    printl("dpscmd error:%s\n",findcmd);
                }
            }
        }
    }
    while(find_read > 1);
}



void *thread_read_from_dps(void * arg)
{
    int fifo_from_power;
    char cmd[50];
    int len;
    fd_set        fds;
    char buffer[BUFFER_SIZE];
    char findcmd[BUFFER_SIZE];
    unsigned char buffer_data[8];
    int ret,find_read;
    short int dps_16;
    struct dps_comm_result *result = (struct dps_comm_result *)arg;
    set_priority(89);
    fifo_from_power = open(result->index.readIndex, O_RDWR | O_NONBLOCK);
    if(fifo_from_power<0)
    {
        printf("open fifo/fifo_from_power1 failed\n");
        XGPutToLog(xglogdata.log_handle,"(thread_read_from_dps)open fifo/fifo_from_power1 failed\n");
        exit(1);
    }
    while(1)
    {
        ret = read(fifo_from_power, buffer, BUFFER_SIZE);
        if(ret > 0)
            continue;
        break;
    }
    while(1)
    {
        FD_ZERO(&fds);
        FD_SET(fifo_from_power, &fds);
        memset(buffer,'\0',BUFFER_SIZE);

        ret = select(fifo_from_power + 1, &fds, NULL, NULL, NULL);
        if(ret > 0)
        {
            if(FD_ISSET(fifo_from_power,&fds))
            {
                ret = read(fifo_from_power, buffer, BUFFER_SIZE);
                if(ret > 0)
                {
                    buffer[ret -1] = '\0';
                    process_power_cmd(result,buffer);
                }
            }
        }
    }
    printl("end of %s\n",__FUNCTION__);
}


#if 0
void *thread_for_Updata_Program(void * arg)
{
    struct dps_comm_result *result = (struct dps_comm_result *)arg;

    FILE *fp =NULL;
    char *Buf = NULL,*tmp = NULL;
    int fileLen =0, nbytes = 0;
    int	i = 0;
    int ret = -1;
    int err = 0;
    int count = 0;
    unsigned int checksum = 0;
    set_priority(70);

    while(1)
    {
        /*******************打开源bin	,每隔10S持续检测目标文件是否存在********/
        while(1)
        {
            fp = fopen(result->index.Program_pathname,"rb");//以只读的方式打开文件
            if(fp == NULL)
            {
                printf("DPS-Program:%s\r\n",result->index.Program_pathname);
                perror("DPS-Pro_file_OpenError");
                sleep(50);
            }
            else
            {
                err = 0;
                break;//跳出第一次循环
            }
        }
        /******************* 获取源bin长度***************************************************/
        fileLen = 0;
        fseek(fp, 0L, SEEK_END);			// 定位到文件尾
        fileLen = ftell(fp);				// 得到文件长度
        fseek(fp,0L, SEEK_SET); 			// 再次定位到文件
        printx("DPS:%x-size:%d\r\n",result->my_id,fileLen);
        if(fileLen > 64*1024)				//bin文件最大64K
        {
            printx("DPS-%s:%d fileLen Error\n",__FUNCTION__,__LINE__);
            fclose(fp);
            err = -1;
        }
        /******************读源bin到buffer*******************************************************/
        Buf = (char *)malloc(fileLen+1024);
        if(Buf == NULL)
        {
            printx("DPS-%s:%d failed\n",__FUNCTION__,__LINE__);
            fclose(fp);
            err = -1;
        }
        memset(Buf, 0x00, fileLen+1024);//清空缓存数组

        nbytes = fread(Buf, 1, fileLen, fp);
        if(nbytes != fileLen)
        {
            printx("Program file read error\r\n");
            free(Buf);
            fclose(fp);
            err = -1;
        }
        fclose(fp);//关闭程序文件

        /********************* 计算校验和*****************************************************/
        tmp = Buf;
        checksum = 0;
        for(i = 0 ; i < fileLen+1024 ; i++)
        {
            checksum += (0x000000FF) & *tmp++;
        }
        checksum = 0x10000000-checksum;
        printx("DPS:%x--checksum=%d\r\n",result->my_id,checksum);

        /*********************转录数据**********************************************************/
        result->Updata_Prgram.Checksum = checksum;
        result->Updata_Prgram.fileLen = fileLen;
        tmp = Buf;//重新指向首位
        i = 0;
        count = (fileLen % 6 > 0)? ((int)(fileLen/6 +1)) : ((int)(fileLen/6));
        /*********************发送更新程序申请**********************************************/
        ret = -1;
        printx("fileLen_count==%d\r\n",count);
        ret = SEND_Upadata_Applay_to_Rawcan(result);
        if(ret == 0 && result ->Updata_Prgram.send_bin_apply_sta == Data_On  && 0 == err)//申请发送成功且可以更新数据
        {

            printx("\r\n###################Start Updata Program#################\r\n");
            for(i = 0; i < count ; i++)
            {
                printx(".");
                memset(result->Updata_Prgram.Program,0x00,sizeof(result->Updata_Prgram.Program));
                result->Updata_Prgram.ProData_check = (unsigned char)(0xFF&i);
                memcpy(result->Updata_Prgram.Program,tmp,6);
                ret = SEND_Upadata_to_Rawcan(result);
                if(ret == 0 && Data_On == result ->Updata_Prgram.send_bin_sta)
                {
                    tmp+=6;
                }
                else
                {
                    printx("\r\nDPS:%x-Updata_send_error---count = %d\r\n",result->my_id,i);

                    break;
                }
            }
            if(i>=count)//发送完成
            {
                printx("\r\n##############DPS:%x-Updata_send_OK###############3",result->my_id);
                printx("delete Bin file:\r\n");
                if(remove(result->index.Program_pathname) == 0)
                {
                    printx("remove file:%s___OK\r\n",result->index.Program_pathname);
                }
                else
                {
                    printx("remove file:%s___ERROR\r\n",result->index.Program_pathname);
                    perror("remove file\r\n");
                }
            }
        }
        free(Buf);
        sleep(20);

    }


}

#endif
/***********************************************************************
 *函数名称:Updata_Program()
 *输入参数:
 *返回参数:
 *函数功能：计算bin文件的检验和
 *函数说明：
 *备    注:20190710 -shutan
 ************************************************************************/

int Updata_Program(struct dps_comm_result *result)
{
    FILE *fp =NULL;
    char *Buf = NULL,*tmp = NULL;
    int fileLen =0, nbytes = 0;
    int	i = 0;
    int ret = -1;//程序运行返回值
    int count = 0;//程序发送的分段数
    int rem = 0;//分段数的余数
    unsigned int checksum = 0;//校验值
    /*******************打开源bin	****************************/

    fp = fopen(result->index.Program_pathname,"rb");//以只读的方式打开bin文件
    if(fp == NULL)
    {
        printf("DPS-Program:%s\n",result->index.Program_pathname);
        perror("DPS-Pro_file_OpenError");
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)DPS-Program:%s\n",result->index.Program_pathname);
        return -1;
    }
    /******************* 获取源bin长度***************************************************/
    fileLen = 0;
    fseek(fp, 0L, SEEK_END);			// 定位到文件尾
    fileLen = ftell(fp);				// 得到文件长度
    fseek(fp,0L, SEEK_SET);				// 再次定位到文件
    printx("DPS:%x-size:%d\n",result->my_id,fileLen);
    XGPutToLog(xglogdata.log_handle,"(Updata_Program)DPS:%x-size:%d\n",result->my_id,fileLen);
    if(fileLen > 64*1024)				//bin文件最大64K
    {
        printx("DPS-%s:%d fileLen Error\n",__FUNCTION__,__LINE__);
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)DPS-%s:%d fileLen Error\n",__FUNCTION__,__LINE__);
        fclose(fp);
        return -1;
    }

    /******************读源bin到buffer*******************************************************/
    Buf = (char *)malloc(fileLen+1024);
    if(Buf == NULL)
    {
        printx("DPS-%s:%d failed\n",__FUNCTION__,__LINE__);
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)DPS-%s:%d failed\n",__FUNCTION__,__LINE__);
        fclose(fp);
        return -1;
    }
    memset(Buf, 0x00, fileLen+1024);

    nbytes = fread(Buf, 1, fileLen, fp);
    if(nbytes != fileLen)
    {
        printx("Program file read error\n");
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)Program file read error\n");
        free(Buf);
        fclose(fp);
        return -1;
    }
    fclose(fp);
    /********************* 计算校验和*****************************************************/
    tmp = Buf;
    checksum = 0;
    for(i = 0 ; i < fileLen+1024 ; i++)
    {
        checksum += (0x000000FF) & *tmp++;
    }
    checksum = 0x10000000-checksum;
    printx("DPS:%x--checksum=%d\n",result->my_id,checksum);
    XGPutToLog(xglogdata.log_handle,"(Updata_Program)DPS:%x--checksum=%d\n",result->my_id,checksum);
    /*********************转录数据**********************************************************/

    result->Updata_Prgram.Checksum = checksum;
    result->Updata_Prgram.fileLen = fileLen;
    /*********************计算分段发送的次数**********************************************/
    tmp = Buf;//重新指向首位
    i = 0;
    rem = 0;
    rem = fileLen % 6;
    count = (rem > 0)? ((int)(fileLen/6 +1)) : ((int)(fileLen/6));
    printx("fileLen_count==%d,rem =%d\n",count,rem);
    XGPutToLog(xglogdata.log_handle,"(Updata_Program)fileLen_count==%d,rem =%d\n",count,rem);
    /*********************发送更新程序申请**********************************************/
    ret = -1;
    ret = SEND_Upadata_Applay_to_Rawcan(result);
    if(ret == 0 && result ->Updata_Prgram.send_bin_apply_sta == Data_On )//申请发送成功且可以更新数据
    {
        printx("\n###Start Updata Program:%s#################\n",result->index.Program_pathname);
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)\n###Start Updata Program:%s#################\n",result->index.Program_pathname);
        for(i = 0; i < count ; i++)
        {
            printf(".");//程序更新进度
            memset(result->Updata_Prgram.Program,0x00,sizeof(result->Updata_Prgram.Program));
            result->Updata_Prgram.ProData_check = (unsigned char)(0xFF&i);
            memcpy(result->Updata_Prgram.Program,tmp,6);
            if(i == count-1 && rem != 0)//最后一帧数据可能不够6位
            {
                result->Send_nbyte_data.data[0] = (unsigned char)(0xFF&i);//放心跳
                memcpy(result->Send_nbyte_data.data+1,tmp,6);//重新拷贝最后的几位到数据缓存区
                result->Send_nbyte_data.id = result->my_id;
                result->Send_nbyte_data.reg = 0x41;
                result->Send_nbyte_data.len = rem+1;//心跳占据一位
                ret = SEND_LenData_to_Rawcan(result);
            }
            else
            {
                ret = SEND_Upadata_to_Rawcan(result);
            }
            if(ret == 0 && Data_On == result ->Updata_Prgram.send_bin_sta)
            {
                tmp+=6;
            }
            else
            {
                printx("\nDPS:%x-Updata_send_error---count = %d\n",result->my_id,i);
                XGPutToLog(xglogdata.log_handle,"(Updata_Program)DPS:%x-Updata_send_error---count = %d\n",result->my_id,i);
                break;
            }
        }
        if(i>=count)//发送完成
        {
            printx("\n##############DPS:%x-Updata_send_OK######i= %d#########\n,",result->my_id,i);
            XGPutToLog(xglogdata.log_handle,"(Updata_Program)##############DPS:%x-Updata_send_OK######i= %d#########\n,",result->my_id,i);
            //	printx("delete Bin file:\r\n");
            //	if(remove(result->index.Program_pathname) == 0)
            //	{
            //		printx("remove file:%s___OK\r\n",result->index.Program_pathname);
            //	}
            //	else
            //	{
            //		printx("remove file:%s___ERROR\r\n",result->index.Program_pathname);
            //		perror("remove file\r\n");
            //	}
        }
    }

    free(Buf);
    return 0;
}



/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信10ms 发送bin数据申请
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/11/18 Create
  * @Version 	1.0 2020/11/18
  *
  * @Note
  **/
void _SEND_BinApply(u32 Datalen,u32 Datasum)
{
//	 if(CanStatus.OnLineSta==STA_ON)
    {

        struct can_frame can_data;
        can_data.can_dlc=8;
        can_data.can_id=HOSTtoLayerAddr;//*(u32*)&TransmitID3;
        can_data.data[0]=CMD_BinApply;
        memcpy(&can_data.data[1],&Datalen,3 );
        memcpy(&can_data.data[4],&Datasum,4 );
        //	can_data.data[2] = as4_comm_result->AS4_Final_SendData.LayerTemp >>8;
        Send_Can_Data(&(can_data));
    }

}
/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	CAN通信 发送bin长数据
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/11/18 Create
  * @Version 	1.0 2020/11/18

  * @Note
  **/
void _SEND_BinData(u32 checksum,u8* Dbuf,u32 Datalen)
{
//	 if(CanStatus.OnLineSta==STA_ON)
    {

        struct can_frame can_data;
        can_data.can_dlc=(Datalen+2)&0xff;
        can_data.can_id=HOSTtoLayerAddr;
        can_data.data[0]=CMD_BinData;
        can_data.data[1]=checksum;
        memcpy(&can_data.data[2],Dbuf,Datalen);
        Send_Can_Data(&(can_data));
    }
}

/**
  * @brief   	how to use it ,and where to use it
  * @Name	 	等待接收到 数据发送成功的标志
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/11/19 Create
  * @Version 	1.0 2020/11/19

  * @Note
  **/

int wait_to_send(int timeout)
{
    while(timeout--)
    {
        usleep(100);
        if(CAN_BinTrans.SendSts == Data_On)
        {
            // pthread_mutex_unlock(result->rawcan_mutex);
            //PX("&");
            return Data_On;
        }
    }
    //  pthread_mutex_unlock(result->rawcan_mutex);
    //PX("^");
    return Data_Err;
}


/**
  * @brief   	a fun to update code by Remote
  * @Name	 	Updata_Program_Screen()
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/11/ Create
  * @Version 	1.0 2020/11/03
  *		     	1.1 2020/11/ change sth
  * @Note  (struct dps_comm_result *result)
  *app起始地址在BootLoader之后，偏移量为0x200的倍数即可
**/
int Updata_Program_Screen(void *arg)
{
    MAIN_PID_DATA *maindata = (MAIN_PID_DATA *)arg;
	CAN_BinTrans.SendSts = Data_On;

    FILE *fp =NULL;
    char *Buf = NULL,*tmp = NULL;
    int fileLen =0, nbytes = 0;
    int	i = 0;
    int ret = -1;//程序运行返回值
    int count = 0;//程序发送的分段数
    int rem = 0;//分段数的余数
    unsigned int checksum = 0;//校验值
    /*******************打开源bin	****************************/

    fp = fopen(Program_2814,"rb");//以只读的方式打开bin文件
    if(fp == NULL)
    {
        printf("2814-Program:%s\n",Program_2814);
        perror("2814-Program_file_OpenError");
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)2814-Program:%s\n",Program_2814);
        return -1;
    }
    /******************* 获取源bin长度***************************************************/
    fileLen = 0;
    fseek(fp, 0L, SEEK_END);			// 定位到文件尾
    fileLen = ftell(fp);				// 得到文件长度
    fseek(fp,0L, SEEK_SET);				// 再次定位到文件
    printx("Program_2814:%x-size:%d\n",HOSTtoLayerAddr,fileLen);
    XGPutToLog(xglogdata.log_handle,"(Updata_Program)Program_2814:%x-size:%d\n",HOSTtoLayerAddr,fileLen);
    if(fileLen > 64*1024)				//bin文件最大64K
    {
        printx("Program_2814-%s:%d fileLen Error\n",__FUNCTION__,__LINE__);
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)Program_2814-%s:%d fileLen Error\n",__FUNCTION__,__LINE__);
        fclose(fp);
        return -1;
    }
    /******************读源bin到buffer*******************************************************/
    Buf = (char *)malloc(fileLen);
    if(Buf == NULL)
    {
        printx("Program_2814-%s:%d failed\n",__FUNCTION__,__LINE__);
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)Program_2814-%s:%d failed\n",__FUNCTION__,__LINE__);
        fclose(fp);
        return -1;
    }
    memset(Buf, 0x00, fileLen);

    nbytes = fread(Buf, 1, fileLen, fp);
    if(nbytes != fileLen)
    {
        printx("Program file read error\n");
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)Program file read error\n");
        free(Buf);
        fclose(fp);
        return -1;
    }
    fclose(fp);
    /********************* 计算校验和*****************************************************/
    tmp = Buf;
    checksum = 0;
    for(i = 0 ; i < fileLen ; i++)
    {
        checksum += (0x000000FF) & *tmp++;
    }
    checksum = 0x10000000-checksum;//CAN_BinTrans.Tmpsum=0x10000000;
	 
    printx("Program_2814:%x--checksum=%d\n",HOSTtoLayerAddr,checksum);
    XGPutToLog(xglogdata.log_handle,"(Updata_Program)DPS:%x--checksum=%d\n",HOSTtoLayerAddr,checksum);

    /*********************计算分段发送的次数**********************************************/
    tmp = Buf;//重新指向首位
    i = 0;
    rem = 0;
    rem = fileLen % 6;
    count = (rem > 0)? ((int)(fileLen/6 +1)) : ((int)(fileLen/6));
    printx("fileLen_count==%d,rem =%d\n",count,rem);
    XGPutToLog(xglogdata.log_handle,"(Updata_Program)fileLen_count==%d,rem =%d\n",count,rem);
    /*********************发送更新程序申请**********************************************/
    ret = -1;
    _SEND_BinApply(fileLen,checksum);
    sleep(5);
    if( CAN_BinTrans.CTSflag == Data_On  )//申请发送成功且可以更新数据
    {
        PX("\n###Start Updata Program:%s#################\n",Program_2814);
        XGPutToLog(xglogdata.log_handle,"(Updata_Program)\n###Start Updata Program:%s#################\n",Program_2814);
        for(i = 0; i < count ; i++)
        {
            printf(".");//程序更新进度
            PX("%d^%d-",i,count);
            memset(maindata->Updata_2814_Bin.Program,0x00,sizeof(maindata->Updata_2814_Bin.Program));
            //maindata->Updata_2814_Bin.ProData_check = (unsigned char)(0xFF&i);
            memcpy(maindata->Updata_2814_Bin.Program,tmp,6);

            if(i == count-1 && rem != 0)//最后一帧数据可能不够6位
            {
                /*
                    result->Send_nbyte_data.data[0] = (unsigned char)(0xFF&i);//放心跳
                    memcpy(result->Send_nbyte_data.data+1,tmp,6);//重新拷贝最后的几位到数据缓存区
                    result->Send_nbyte_data.id = result->my_id;
                    result->Send_nbyte_data.reg = 0x41;
                    result->Send_nbyte_data.len = rem+1;//心跳占据一位
                    ret = SEND_LenData_to_Rawcan(result);
                    */
                memcpy(maindata->Updata_2814_Bin.Program,tmp,rem);
                _SEND_BinData(i,maindata->Updata_2814_Bin.Program,rem);
                PX("the last count@@@】\n");
            }
            else
            {
                //ret = SEND_Upadata_to_Rawcan(result);
                _SEND_BinData(i,maindata->Updata_2814_Bin.Program,6);
            }


            if(Data_On == CAN_BinTrans.SendSts )//ret == 0 && Data_On == maindata->Updata_2814_Bin.send_bin_sts
            {
                tmp+=6;
                CAN_BinTrans.SendSts = Data_Err;
                wait_to_send(10000);//

            }
            else
            {
                printx("\nDPS:%x-Updata_send_error---count = %d\n",HOSTtoLayerAddr,i);
                XGPutToLog(xglogdata.log_handle,"(Updata_Program)DPS:%x-Updata_send_error---count = %d\n",HOSTtoLayerAddr,i);
                break;//发送失败就没必要再运行该程序了
            }
        }

        if(i>=count)//发送完成
        {
            printx("\n##############DPS:%x-Updata_send_OK######i= %d#########\n,",HOSTtoLayerAddr,i);
            XGPutToLog(xglogdata.log_handle,"(Updata_Program)##############DPS:%x-Updata_send_OK######i= %d#########\n,",HOSTtoLayerAddr,i);
        }
    }

    free(Buf);
    return 0;
}

void * thread_for_Update_2814_bin(void * arg)
{
    Updata_Program_Screen(arg);
    while (1)
    {
        sleep(10);
    }
}



void *thread_for_send_data(void * arg)
{
    //printx("Senddata:%s:%d \n",__FUNCTION__,__LINE__);
    struct dps_comm_result *result = (struct dps_comm_result *)arg;
    //int ret = -1;
    //int i =0;
    //result->DADDR_Data.One_reset_sta = P_Off;
    //result->DADDR_Data.reset_add_sta = Data_Err;
    set_priority(60);
    while(1)
    {
        /*********************发送地址***************************************/
        /*
        		if(P_Off == result->DADDR_Data.One_reset_sta && 0 != result->DADDR_Data.DADDR && 0x01 == result->my_id)
        		{
        			ret = -1;
        			ret = SEND_DADDR_to_Rawcan(result);
        			if(ret == 0 && result->DADDR_Data.reset_add_sta == Data_On)
        			{
        				result->DADDR_Data.One_reset_sta = P_On;
        				printx("SEND_DADDR_OK!\r\n");
        			}
        			else
        			{
        				printx("SEND_DADDR_ERROR!\r\n");
        			}
        		}
        */
        sleep(10);
#if 0
        printx("Senddata:%s:%d \n",__FUNCTION__,__LINE__);
#endif
    }
}












