
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdarg.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "task.h"


#include "lwip/sockets.h"  /* equivalent of <sys/socket.h> */
#include "lwip/sys.h"
#include <task.h>

#include "lib_dma.h"
#include "audio_proc.h"
#include "g711core.h"
#include "lib_wdt.h"
//#include "common.h"


#define x_udp_h

#define CommonH
#include "common.h"

#define MAX_UDP_LENGTH   1520
#define MAX_DOWN_PACK    512  //最大下载包数  512K

#define MAX_VIDEO_PACK   60  //每帧最大视频包数

int UDP_DataSocket = -1;
int UDP_VideoSocket = -1;

xTaskHandle  UdpDataRecvHandle;
xTaskHandle  UdpVideoRecvHandle;
xTaskHandle  multi_sendHandle;

#ifdef _PON_SUPPORT
  int UDP_PonSocket = -1;
  xTaskHandle  UdpPonRecvHandle;
#endif

xSemaphoreHandle  *multi_send_sem;
//xSemaphoreHandle *multi_send_mutex;


uint8_t send_b[1600];
uint8_t recvBuf[1600];   //堆栈不能太大
uint8_t recvBuf1[1600];  //堆栈不能太大
uint8_t recvBuf2[1600];  //

uint8_t *Memory6_Stream; //用于下载
void xtm_printf(char *fmt, ...);

uint8_t Udp_InitFlag = 0;
int Init_Udp(void);    //初始化UDP
int UnInit_Udp(void);    

void AddMultiGroup(int m_Socket, char *McastAddr);  //加入组播组
void DropNsMultiGroup(int m_Socket, char *McastAddr);  //退出NS组播组

void UdpDataRecvTask(int *s_socket);
void UdpVideoRecvTask(int *s_socket);

#ifdef _PON_SUPPORT
  void UdpPonRecvTask(int *s_socket);
  //搜索所有设备应答
  void RecvPonSearchAllEquip_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);
  //写设备地址应答
  void RecvPonWriteEquipAddr_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);
#endif

void MultiSendTask(void);

uint16_t old_audioFrameno = 0;
uint8_t CheckSameNet(uint8_t *IP1, uint8_t *IP2, uint8_t *IPMask);
void CreateMultiUdpBuff(int s_socket, uint8_t SendMode, char *RemoteIP, uint16_t DestPort, uint8_t *udpdata, uint16_t nlength, uint8_t CurrOrder);
void CheckMultiUdpBuff(void);   //检查UDP发送缓存
void MultiUdpSendFailDeal(uint8_t i);
uint8_t ClearMultiUdpBuff(uint8_t Order1, uint8_t Order2, uint8_t Order3, char *cFromIP);   //Clear UDP发送缓存

uint16_t UdpSendBuff(int s_socket, char *Ip_Addr, int Port, char *sendbuf, int sendlen);

void CheckRemoteAddr(void);  //检查远端地址

void RecvUdpTest_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
void RecvUdpTest1_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//搜索所有设备应答
void RecvSearchAllEquip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint8_t *DestIP, uint16_t DestPort, uint16_t length);
//写设备地址应答
void RecvWriteEquipAddr_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint8_t *DestIP, uint16_t DestPort, uint16_t length);

#ifdef _ROOM_SEARCH_SUPPORT      //室内机搜索 小门口机、4.3室内机
 //室内机搜索所有设备应答
 void RecvRoomSearchAllEquip_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);
 //室内机写设备地址应答
 void RecvRoomWriteEquipAddr_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);

 //室内机搜索 小门口机、4.3室内机
 void RecvRegisterIDCard_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);
#endif

//下载
void RecvDownLoadFile_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
uint8_t WriteAllToFlash(uint8_t *recv_buf, struct downfile1 *DownData);

//读Flash内容
void RecvReadFlash_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//远程重启动
void RecvRemoteReset_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//发送远程调试信息
void RecvRemoteDebugInfo_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
void send_debug_info(int s_socket, char *debug_info);

#ifdef _REMOTECALLTEST  //远程呼叫测试
//接收远程呼叫测试　
void RecvRemoteCallTest_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
#endif

#ifdef _ADDTABLE_SUPPORT  //地址表支持
//读地址表信息
void RecvReadAddrTable_Func(int s_socket, uint8_t *recv_buf, uint8_t *cFromIP, uint16_t DestPort, uint16_t length);
//void RecvReadAddrTable_Func(uint8_t *recv_buf, uint8_t *cFromIP, uint16_t DestPort, uint16_t length);
//读地址表文件
extern void ReadAddrTableFile(void);
#endif

//解析
void RecvNSAsk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);  //解析请求
void RecvNSReply_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);//解析应答

uint16_t Curr_Frameno = 10000;
uint16_t PackFrameno[MAXVIDEOBUF][MAX_VIDEO_PACK];
uint8_t PackExist[MAXVIDEOBUF][MAX_VIDEO_PACK];

//信息
void RecvMessage_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//设备定时报告状态
void RecvReportStatus_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//管理中心查询设备状态
void RecvQueryStatus_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//报警
void RecvAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t lengtht);
//取消报警
void RecvCancelAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//复位密码
void RecvResetPass_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//对讲
void RecvTalkCall_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //对讲呼叫
void RecvTalkCallAnswer_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //对讲呼叫应答
void RecvTalkLineUse_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //对讲占线应答
void RecvTalkCallConfirm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort); //对讲在线确认
uint8_t AudioStartFlag = 0;
uint8_t AudioStartTime = 0;
uint8_t AudioTalkOpenFlag = 0;
void RecvTalkCallStart_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //对讲开始通话
void RecvTalkCallEnd_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //对讲呼叫结束
void RecvTalkRemoteOpenLock_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //远程开锁
void RecvTalkCenterTurnTalk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //中心人工转接
void RecvTalkTrusteeship_Talk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //托管

void RecvTalkCallUpDown_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);  //对讲数据

//监视
void RecvWatchCallAnswer_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //监视呼叫应答
void RecvWatchLineUse_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //监视占线应答
void RecvWatchCallConfirm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort); //监视在线确认
void RecvWatchCallEnd_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //监视呼叫结束

void RecvWatchCallUpDown_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);  //监视数据

void AudioDeal_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t length);  //音频数据处理
void VideoDeal_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t length);  //视频数据处理

void WatchEnd_ClearStatus(void); //监视结束，清状态和关闭音视频
void TalkEnd_ClearStatus(int isTurn); //对讲结束，清状态和关闭音视频

#ifdef _REMOTEWRITEOV7725REGISTER   //远程写OV7725寄存器  调试用
 void RecvRemoteWriteOV7725Register_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
#endif

//主机与副机
#ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
void RecvTalkLeaveGroup_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort); //退出组播组（主叫方->被叫方，被叫方应答）
void ExitGroup(int s_socket, uint8_t *buf);      //向其它被叫方退出组播组命令
void SingleExitGroup(int s_socket, uint8_t *buf); //向单个被叫方退出组播组命令

void RecvSyncSub_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //为主机，定时与副机同步状态
void CheckSubData1(struct SyncSubData1 SubData1);  //检查同步数据1有无改变
void RecvSubDefence_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length); //布防（副机－＞主机）
void RecvSubAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length); //报警（副机－＞主机）
void RecvSubAlarmTip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length); //报警提示（主机－＞副机）
void RecvSubDefenceTip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPortP, uint16_t length); //布防提示（主机－＞副机）
void RecvSubFind_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length); //查找副机（主机－＞副机）
#endif

#ifdef _REMOTE_FORTIFY_SUPPORT  //远程布撤防支持
 void RecvRemoteFortify_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
#endif

#ifdef ELEVATORCONTROL_HANGZHOU
 void RecvElevatorControl_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);  //电梯控制
#endif

uint16_t video_decode_no;  //视频解码包号
uint16_t old_mjpeg_stream_no;  //视频解码包号

//视频接收缓冲  未解码
#define MAX_STREAM_NUM 2
uint8_t Curr_Mjpeg_Stream = 0;
uint8_t *mjpeg_stream[MAX_STREAM_NUM];       //mjpeg 流
uint16_t mjpeg_stream_len[MAX_STREAM_NUM];   //mjpeg 流
uint16_t mjpeg_stream_offset = 0x26f;

uint8_t Down_RecvFlag[MAX_DOWN_PACK];          //下载 包标志
uint8_t head_640x480[0x26f] = {
0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x02, 0x00, 0x76, 
0x00, 0x76, 0x00, 0x00, 0xff, 0xdb, 0x00, 0x43, 0x00, 0x0d, 0x09, 0x0a, 0x0b, 0x0a, 0x08, 0x0d, 
0x0b, 0x0a, 0x0b, 0x0e, 0x0e, 0x0d, 0x0f, 0x13, 0x20, 0x15, 0x13, 0x12, 0x12, 0x13, 0x27, 0x1c, 
0x1e, 0x17, 0x20, 0x2e, 0x29, 0x31, 0x30, 0x2e, 0x29, 0x2d, 0x2c, 0x33, 0x3a, 0x4a, 0x3e, 0x33, 
0x36, 0x46, 0x37, 0x2c, 0x2d, 0x40, 0x57, 0x41, 0x46, 0x4c, 0x4e, 0x52, 0x53, 0x52, 0x32, 0x3e, 
0x5a, 0x61, 0x5a, 0x50, 0x60, 0x4a, 0x51, 0x52, 0x4f, 0xff, 0xdb, 0x00, 0x43, 0x01, 0x0e, 0x0e, 
0x0e, 0x13, 0x11, 0x13, 0x26, 0x15, 0x15, 0x26, 0x4f, 0x35, 0x2d, 0x35, 0x4f, 0x4f, 0x4f, 0x4f, 
0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 
0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 
0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0xff, 0xc0, 
0x00, 0x11, 0x08, 0x01, 0xe0, 0x02, 0x80, 0x03, 0x01, 0x22, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 
0x01, 0xff, 0xc4, 0x00, 0x1f, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
0x0a, 0x0b, 0xff, 0xc4, 0x00, 0xb5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 
0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 
0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 
0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 
0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 
0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 
0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 
0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 
0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 
0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 
0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 
0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xff, 0xc4, 0x00, 0x1f, 0x01, 0x00, 0x03, 
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0xff, 0xc4, 0x00, 0xb5, 0x11, 0x00, 
0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 
0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 
0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 
0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 
0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 
0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 
0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 
0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 
0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 
0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 
0xfa, 0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00
};
int FirstFrameFlag = 0; //第一帧，判断jpeg header
int FirstFrameFinished = 0;
void ResetMjpegHeader(void);
//---------------------------------------------------------------------------
void xtm_printf(char *fmt, ...)
{
	char debug_info[500];
	va_list args;		//定义一个va_list类型的变量，用来储存单个参数  
	va_start(args, fmt); //使args指向可变参数的第一个参数  
	vprintf(fmt, args);	//必须用vprintf等带V的	
	
	if(Local.RemoteDebugInfo == 1)
	{  
		//sprintf(debug_info, fmt, ...);
		vsprintf(debug_info, fmt, args);
		send_debug_info(UDP_DataSocket, debug_info);
	}
	va_end(args);		//结束可变参数的获取  	
}
//---------------------------------------------------------------------------
void ResetMjpegHeader(void)
{
	int i;
	for(i=0; i<MAX_STREAM_NUM; i++)
	{
	  memcpy(mjpeg_stream[i], head_640x480, mjpeg_stream_offset);
	  mjpeg_stream_offset = 0x26f;
	}	 
}
//---------------------------------------------------------------------------


int Init_Udp(void)    //初始化UDP
{
	struct sockaddr_in clieAddrUDP;
	int result, i, SndBufLen, RcvBufLen;
	int bBroadcast;

	Memory6_Stream = MEMORY_STEAM_BUF0;

	xtm_printf("Init_Udp::1\n");
	strcpy(UdpPackageHead, "XXXCID");
	LocalPort[0] = 8300;
	LocalPort[1] = 8302;
	RemotePort[0] = LocalPort[0];
	RemotePort[1] = LocalPort[1];
	
  #ifdef _PON_SUPPORT   //PON 支持 20140314
    strcpy(PonPackageHead, "PONUSR");
    LocalPort[2] = 8310;
    RemotePort[2] = LocalPort[2];
  #endif

	for(i=0; i<UDPSENDMAX; i++)
	{
	  Multi_Udp_Buff[i].isValid = 0;
	  Multi_Udp_Buff[i].SendNum = 0;
	  Multi_Udp_Buff[i].DelayTime = 200;
	  Multi_Udp_Buff[i].buf = (uint8_t *)malloc(1520);
	}	

	xtm_printf("Init_Udp::2\n");
	ClearWatchDog();

	for(i=0; i<MAX_STREAM_NUM; i++)
	{
	  xtm_printf("Init_Udp::2.1, i = %d\n", i);
	  mjpeg_stream[i] = MJPEG_STEAM_BUF + i*65536*2;
	  mjpeg_stream_offset = 0x26f;
	  memcpy(mjpeg_stream[i], head_640x480, mjpeg_stream_offset);
	  mjpeg_stream_len[i] = 0;
	}

	//xtm_printf("Init_Udp::3, mjpeg_stream = 0x%X, 0x%X, 0x%X, 0x%X\n", 
	//	mjpeg_stream[0][0], mjpeg_stream[0][1], mjpeg_stream[0][2], mjpeg_stream[0][3]);
	//xtm_printf("Init_Udp::3.1, mjpeg_stream = 0x%X, 0x%X, 0x%X, 0x%X\n", 
	//	mjpeg_stream[0][0], mjpeg_stream[0][1], mjpeg_stream[0][2], mjpeg_stream[0][3]);	
  xtm_printf("OpenUdp::1\n");
  UDP_DataSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if( UDP_DataSocket < 0 )
  {
    xtm_printf("OpenUdp UDP_DataSocket err\n");
    return -1;
  }else{
    xtm_printf("OpenUdp ok, UDP_DataSocket = %d\n", UDP_DataSocket);
  }
	ClearWatchDog();

	bBroadcast=1;
  setsockopt(UDP_DataSocket, SOL_SOCKET, SO_BROADCAST,(const char*)&bBroadcast,sizeof(bBroadcast));

  // 填充服务端的资料 
  clieAddrUDP.sin_family = AF_INET;
  clieAddrUDP.sin_port = htons(LocalPort[0]);
  clieAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("192.168.1.112");
  memset(&(clieAddrUDP.sin_zero), 0, sizeof(clieAddrUDP.sin_zero));
  result = bind(UDP_DataSocket, (struct sockaddr *)&clieAddrUDP,sizeof(struct sockaddr));

  xtm_printf("OpenUdp::2\n");
  UDP_VideoSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if( UDP_VideoSocket<0 )
  {
    xtm_printf("OpenUdp UDP_VideoSocket err\r\n");
    return -1;
  }else{
    printf("OpenUdp ok, UDP_VideoSocket = %d\r\n", UDP_VideoSocket);
  }
	ClearWatchDog();

  //SndBufLen = 64*1024;       //128K
  //result=setsockopt(m_Socket,SOL_SOCKET,SO_SNDBUF,(char*)&nZero,sizeof((char*)&nZero));
  RcvBufLen =  128*1024;       //128K
  result = getsockopt(UDP_VideoSocket, SOL_SOCKET, SO_RCVBUF, (char*)&RcvBufLen, sizeof(int));
	xtm_printf("OpenUdp::2.1, SO_RCVBUF result = %d, RcvBufLen = %d\n", result, RcvBufLen);
  result = setsockopt(UDP_VideoSocket, SOL_SOCKET, SO_RCVBUF, (char*)&RcvBufLen, sizeof(int));	
	xtm_printf("OpenUdp::2.2, SO_RCVBUF result = %d\n", result);

  //填充服务端的资料 
  clieAddrUDP.sin_family = AF_INET;
  clieAddrUDP.sin_port = htons(LocalPort[1]);
  clieAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("192.168.1.112");
  memset(&(clieAddrUDP.sin_zero), 0, sizeof(clieAddrUDP.sin_zero));
  result = bind(UDP_VideoSocket, (struct sockaddr *)&clieAddrUDP,sizeof(struct sockaddr));

  #ifdef  _PON_SUPPORT
    xtm_printf("OpenUdp::3\n");
    UDP_PonSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if( UDP_PonSocket<0 )
    {
      xtm_printf("OpenUdp UDP_PonSocket err\r\n");
      return -1;
    }else{
      printf("OpenUdp ok, UDP_PonSocket = %d\r\n", UDP_PonSocket);
    }
  
    // 填充服务端的资料 
    clieAddrUDP.sin_family = AF_INET;
    clieAddrUDP.sin_port = htons(LocalPort[2]);
    clieAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("192.168.1.112");
    memset(&(clieAddrUDP.sin_zero), 0, sizeof(clieAddrUDP.sin_zero));
    result = bind(UDP_PonSocket, (struct sockaddr *)&clieAddrUDP,sizeof(struct sockaddr));
  #endif

	printf("OpenUdp::4\n");
	ClearWatchDog();
	AddMultiGroup(UDP_VideoSocket, NSMULTIADDR);
	printf("OpenUdp::5\n");
  
  //
  xTaskCreate((TaskFunction_t) &UdpDataRecvTask, 
  	          (const char *  ) "UdpDataRecvTask", 
  	          (const uint16_t) 8192, 
  	          (void *        ) &UDP_DataSocket, 
  	          (UBaseType_t   ) UDPDATA_PRIORITY, 
  	          (TaskHandle_t *) &UdpDataRecvHandle);
	printf("OpenUdp::6\n");

  xTaskCreate((TaskFunction_t) &UdpVideoRecvTask, 
  	          (const char *  ) "UdpVideoRecvTask", 
  	          (const uint16_t) 8192, 
  	          (void *        ) &UDP_VideoSocket, 
  	          (UBaseType_t   ) UDPVIDEO_PRIORITY, 
  	          (TaskHandle_t *) &UdpVideoRecvHandle);
	printf("OpenUdp::7\n");

  #ifdef  _PON_SUPPORT
    xTaskCreate((TaskFunction_t) &UdpPonRecvTask,
  	            (const char *  ) "UdpPonRecvTask", 
  	            (const uint16_t) 8192, 
  	            (void *        ) &UDP_PonSocket, 
  	            (UBaseType_t   ) UDPPON_PRIORITY , 
  	            (TaskHandle_t *) &UdpPonRecvHandle);
  #endif

  //用于UDP和Comm通信
	multi_send_sem = xSemaphoreCreateCounting(10, 0);
	//互斥锁
	//multi_send_mutex = xSemaphoreCreateMutex();

	xTaskCreate((TaskFunction_t) &MultiSendTask, 
		          (const char *  ) "MultiSendTask", 
		          (const uint16_t) 4096, 
		          (void *        ) NULL, 
		          (UBaseType_t   ) MULTISEND_PRIORITY, 
		          (TaskHandle_t *) &multi_sendHandle);

	Udp_InitFlag = 1;
  return 0;  
}

int UnInit_Udp(void)
{
	int i;
	for(i=0; i<UDPSENDMAX; i++)
	{
		free(Multi_Udp_Buff[i].buf); 
	}	
	xtm_printf("UnInit_Udp::1\n");
	vTaskDelete(multi_sendHandle);

	xtm_printf("UnInit_Udp::2\n");
	vSemaphoreDelete(multi_send_sem);
	DropNsMultiGroup(UDP_VideoSocket, NSMULTIADDR);

	xtm_printf("UnInit_Udp::3\n");
	vTaskDelete(UdpDataRecvHandle);

	xtm_printf("UnInit_Udp::4\n");
	vTaskDelete(UdpVideoRecvHandle);

  #ifdef  _PON_SUPPORT
    xtm_printf("UnInit_Udp::5\n");
    vTaskDelete(UdpPonRecvHandle);
  #endif

	xtm_printf("UnInit_Udp::6\n");
	close(UDP_DataSocket);

	xtm_printf("UnInit_Udp::7\n");
	close(UDP_VideoSocket);

  #ifdef  _PON_SUPPORT
    xtm_printf("UnInit_Udp::8\n");
    close(UDP_PonSocket);
	#endif

	xtm_printf("UnInit_Udp::9\n");

	Udp_InitFlag = 0;
  return 0;
}

void AddMultiGroup(int m_Socket, char *McastAddr)  //加入组播组
{
// Winsock1.0
  struct ip_mreq mcast; // Winsock1.0
  char tmpIP[20];

  int on = 1;
  if ((setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on))) < 0) 
  {		
    printf("recv setsockopt(SO_REUSEADDR): error, sd = %d\n", m_Socket);		
  }

  memset(&mcast, 0, sizeof(struct ip_mreq));	
  mcast.imr_interface.s_addr = htonl(INADDR_ANY);	
  sprintf(tmpIP, "%d.%d.%d.%d", LocalCfg.IP_Gate[0], LocalCfg.IP_Gate[1], LocalCfg.IP_Gate[2], LocalCfg.IP_Gate[3]);
  mcast.imr_multiaddr.s_addr = inet_addr(tmpIP);
  if( setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_IF, (char*)&mcast,sizeof(mcast)) < 0)
  {		
    printf("recv setsockopt(IP_MULTICAST_IF): error, sd = %d\n", m_Socket);		
  }

  memset(&mcast, 0, sizeof(struct ip_mreq));	
  mcast.imr_interface.s_addr = htonl(INADDR_ANY);  
  mcast.imr_multiaddr.s_addr = inet_addr("238.9.9.1");
  //printf("mcast.imr_multiaddr.s_addr = 0x%X, mcast.imr_interface.s_addr = 0x%X\n", mcast.imr_multiaddr.s_addr, mcast.imr_interface.s_addr);
  if( setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast,sizeof(mcast)) < 0)
  {
    printf("set multicast error., m_Socket = %d\n\r", m_Socket);
    return;
  }
  // enable loopback in case someone else needs the data	
  if ((setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &on, sizeof(on))) < 0) 
  {		
  	printf("recv setsockopt(IP_MULTICAST_LOOP): error\n");		
	  return;	
  }  
}
//---------------------------------------------------------------------------
void DropNsMultiGroup(int m_Socket, char *McastAddr)  //退出NS组播组
{
// Winsock1.0
  struct ip_mreq mcast; // Winsock1.0

  memset(&mcast, 0, sizeof(struct ip_mreq));
  mcast.imr_multiaddr.s_addr = inet_addr(McastAddr);
  mcast.imr_interface.s_addr = INADDR_ANY;
  if( setsockopt(m_Socket,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&mcast,sizeof(mcast))  < 0)
  {
    printf("drop multicast error.\n\r");
    return;
  }
}
//---------------------------------------------------------------------------
void UdpDataRecvTask(int *s_socket)
{
	struct sockaddr_in from;
	
	int nLength, recv_len;
	char FromIP[20];
	uint16_t DestPort;
	nLength = sizeof(struct sockaddr_in);
	printf("UdpDataRecvTask::*s_socket = %d, UDP_DataSocket = %d\n", *s_socket, UDP_DataSocket);
	while(1)
	{
		recv_len = recvfrom(*s_socket,
				        	(char *)recvBuf,
					        sizeof(recvBuf),
					        0,
					        (struct sockaddr *)&from,
					        (socklen_t*)&nLength );

		//printf("UdpDataRecvTask::*s_socket = %d, UDP_DataSocket = %d\n", *s_socket, UDP_DataSocket);
		if(recv_len <= 0)
			continue;
		
		recvBuf[recv_len] = '\0';
		strcpy(FromIP, inet_ntoa(from.sin_addr));				
		DestPort=ntohs(from.sin_port);

		if( (recvBuf[0] == UdpPackageHead[0])&&(recvBuf[1] == UdpPackageHead[1])&&(recvBuf[2] == UdpPackageHead[2])&& \
		    (recvBuf[3] == UdpPackageHead[3])&&(recvBuf[4] == UdpPackageHead[4])&&(recvBuf[5] == UdpPackageHead[5]))
		{
			switch(recvBuf[6])
			{
				case SENDMESSAGE: //信息
						RecvMessage_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
					break;
				case REPORTSTATUS:	 //设备定时报告状态
						if(recv_len >= SMALLESTSIZE)
						{
							RecvReportStatus_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("设备定时报告状态应答长度异常\n");
						}
					break;
				case QUERYSTATUS:	//管理中心查询设备状态
						if(recv_len >= SMALLESTSIZE)
						{
							RecvQueryStatus_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("管理中心查询设备状态长度异常\n");
						}
					break;
				case ALARM:   //报警
						if(recv_len >= SMALLESTSIZE)
						{
							RecvAlarm_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("报警应答长度异常\n");
						}
					break;
				case CANCELALARM:	//取消报警
						if(recv_len >= SMALLESTSIZE)
						{
							RecvCancelAlarm_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("取消报警应答长度异常\n");
						}
					break;
				case RESETPASS:    //复位密码
						if(recv_len >= SMALLESTSIZE)
						{
							RecvResetPass_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("复位密码长度异常\n");
						}
					break;
				case SEARCHALLEQUIP:  //252  //搜索所有设备（管理中心－＞设备）
						if(recv_len >= SMALLESTSIZE)//DestIP
						{
							RecvSearchAllEquip_Func(*s_socket, recvBuf, FromIP, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("搜索所有设备长度异常\n");
						}
					break;
				case WRITEEQUIPADDR:  //254  //写设备地址（管理中心－＞设备）
						if(recv_len >= SMALLESTSIZE)
						{
							RecvWriteEquipAddr_Func(*s_socket, recvBuf, FromIP, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("写设备地址长度异常\n");
						}
					break;
				case DOWNLOADFILE:	 //下载
						if(recv_len >= 9)
						{
							//xtm_printf("DataPortDeal::1111\n");
							RecvDownLoadFile_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("下载长度异常\n");
						}
					break;
				case READFLASH: 	 //读FLASH内容
						if(recv_len >= 9)
						{
							//xtm_printf("DataPortDeal::1111\n");
						  RecvReadFlash_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("读FLASH内容长度异常\n");
						}
					break;
				case REMOTERESET:	   //远程重启动
						if(recv_len >= 9)
						{
							RecvRemoteReset_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("远程重启动长度异常\n");
						}
					break;
				case UDP_TEST:
						RecvUdpTest_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
					break;
				case 250:
						RecvUdpTest1_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
					break;              
				case REMOTEDEBUGINFO:	   //发送远程调试信息
						if(recv_len == 29)
						{
							RecvRemoteDebugInfo_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("发送远程调试信息长度异常\n");
						}
					break;
        #ifdef _REMOTECALLTEST  //远程呼叫测试
					case REMOTETEST:	  //接收远程呼叫测试　
							if(recv_len == 49)
							{
								RecvRemoteCallTest_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
							}
							else
							{
								xtm_printf("接收远程呼叫测试长度异常\n");
							}
						break;
        #endif
				case NSSERVERORDER:  //主机名解析(NS服务器)
						switch(recvBuf[7] & 0x03)
						{
							case 2://解析回应
									if(recv_len >= SMALLESTSIZE)
									{
										RecvNSReply_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
									}
									else
									{
										xtm_printf("解析应答数据长度异常\n");
									}
								break;
						}
					break;
	      #ifdef _ADDTABLE_SUPPORT  //地址表支持
          case READ_ADDRTABLE: //读地址表信息
                RecvReadAddrTable_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
                break;
        #endif 
			}
		}
		
		if(recv_len > 0)
		{
			//printf("UdpDataRecvTask::len = %d\n", recv_len);
		}
	}
	//vTaskDelete(NULL);
}
//---------------------------------------------------------------------------
//信息
void RecvMessage_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  struct InfoContent1 TmpInfoCon;
  char TmpContent[INFOMAXSIZE];          //
  INT16 j;
  uint8_t isAddrOK;


  isAddrOK = 1;
  for(j=8; j<(8+Local.AddrLen); j++)
  {
    if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
      isAddrOK = 0;
      xtm_printf("recv message addr not ok\n");
      break;
    }
  }
  //地址匹配
  if(isAddrOK == 1)
  {
    recv_buf[7] = REPLY;
    //xtm_printf("RecvMessage_Func1::recv_buf[30] = 0x%X, 0x%X, 0x%X, 0x%X\n",  recv_buf[30], recv_buf[31], recv_buf[32], recv_buf[33]);
      //xtm_printf("RecvMessage_Func::cFromIP = %d.%d.%d.%d, RemoteMac = 0x%X:0x%X:0x%X:0x%X:0x%X:0x%X\n", cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3], 
      //     RemoteMac[0], RemoteMac[1], RemoteMac[2], RemoteMac[3], RemoteMac[4], RemoteMac[5]);
    UdpSendBuff(s_socket, cFromIP, DestPort, (char *)recv_buf, length);

    memcpy(&TmpInfoCon.Type, recv_buf + 28, sizeof(TmpInfoCon.Type));
    memcpy(&TmpInfoCon.Sn, recv_buf + 30, sizeof(TmpInfoCon.Sn));
    memcpy(&TmpInfoCon.Length, recv_buf + 34, sizeof(TmpInfoCon.Length));
    if(TmpInfoCon.Length > INFOMAXSIZE)
      TmpInfoCon.Length = INFOMAXSIZE;

    xtm_printf("TmpInfoCon.Type = %d, TmpInfoCon.Length = %d, TmpInfoCon.Sn = 0x%X, Info.TotalNum = %d\n", TmpInfoCon.Type, TmpInfoCon.Length,  TmpInfoCon.Sn, Info.TotalNum);
    //只接收普通信息  
    if(TmpInfoCon.Type == 1)
    {
      TmpInfoCon.isValid = 1;
      TmpInfoCon.isReaded = 0;

      //sprintf(TmpInfoCon.Time, "%04d-%02d-%02d %02d:%02d:%02d\0", tm_t->tm_year + 1900, tm_t->tm_mon+1,
      //       tm_t->tm_mday, tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec);

      memcpy(TmpContent, recv_buf + 36, TmpInfoCon.Length);
      TmpContent[TmpInfoCon.Length] = '\0';

      for(j = 0; j < Info.TotalNum; j ++)
      {
        if(InfoContent[j].Sn == TmpInfoCon.Sn)
        {
          xtm_printf("RecvMessage_Func::该信息序号已存在, 0x%X 0x%X, j = %d\n", (uint16_t)((TmpInfoCon.Sn & 0xffff0000) >> 16), (uint16_t)(TmpInfoCon.Sn & 0xffff), j);
          return;
        }
      }
      //如信息未到最大数量，顺序后移，将新信息插入到头部
      if(Info.TotalNum < Info.MaxNum)
      {
        if(Info.TotalNum > 0)
          for(j = (Info.TotalNum - 1); j >= 0; j --)
          {
            memcpy(&InfoContent[j+1], &InfoContent[j], sizeof(TmpInfoCon));
            strcpy(InfoBuff[j+1], InfoBuff[j]);
          }
         Info.TotalNum ++;
      }
      else   //丢弃最后一条，顺序后移，将新信息插入到头部
      {
        for(j = (Info.TotalNum - 2); j >= 0; j --)
        {
          memcpy(&InfoContent[j+1], &InfoContent[j], sizeof(TmpInfoCon));
          strcpy(InfoBuff[j+1], InfoBuff[j]);
        }
      }

      InfoContent[0].isValid = TmpInfoCon.isValid;  //有效，未删除标志   1
      InfoContent[0].isReaded = TmpInfoCon.isReaded; //已读标志    1
      InfoContent[0].HavePic = TmpInfoCon.HavePic;  //有无图像标志    1
      strcpy(InfoContent[0].RemoteAddr, TmpInfoCon.RemoteAddr);
      strcpy(InfoContent[0].Time, TmpInfoCon.Time);    //接收时间    32
      InfoContent[0].Type = TmpInfoCon.Type;     //类型        1    信息类型或事件类型
      InfoContent[0].Sn = TmpInfoCon.Sn;      //序号        4
      InfoContent[0].Length = TmpInfoCon.Length;       //长度        4
      strcpy(InfoBuff[0], TmpContent);

      for(j=0; j<Info.TotalNum; j++)
        xtm_printf("RecvMessage_Func::Info.TotalNum = %d, InfoBuff[%d] = %s\n", Info.TotalNum, j, InfoBuff[j]);

      WriteInfoCfgFile();     //写信息设置文件
      WriteInfoFile();        //写信息文件
      Recv_NewInfo();
    }
    else
    {
      xtm_printf("信息数据长度错误或类型不对\n");
    }
  }   
}
//---------------------------------------------------------------------------
//设备定时报告状态
void RecvReportStatus_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j;
  uint8_t isAddrOK;
  uint16_t curr_time;

  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
  {
    if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
      isAddrOK = 0;
      break;
    }
  }
  //xtm_printf("RecvReportStatus_Func::isAddrOK = %d\n", isAddrOK);
  //地址匹配
  if(isAddrOK == 1)
  {
    if((recv_buf[7] & 0x03) == REPLY)   //应答
    {
      if(ClearMultiUdpBuff(REPORTSTATUS, ASK, LocalCfg.Addr[0], cFromIP))
      {
        curr_time = recv_buf[29];
        curr_time = curr_time << 8;
        curr_time += recv_buf[28];
        if(curr_time != LocalCfg.ReportTime)
        {
          LocalCfg.ReportTime = curr_time;
          WriteCfgFile();
          xtm_printf("收到设备定时报告状态应答, LocalCfg.ReportTime = %d\n", LocalCfg.ReportTime);
        }
        //xtm_printf("收到设备定时报告状态应答\n");
      }
    }
  }
}
//---------------------------------------------------------------------------
//管理中心查询设备状态
void RecvQueryStatus_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j, k;
  uint8_t isAddrOK;
  uint16_t sendlength;

  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
  {  
  	if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  }
  //地址匹配
  if(isAddrOK == 1)
  {
    //头部
    memcpy(send_b, UdpPackageHead, 6);
    //命令
    send_b[6] = QUERYSTATUS;
    send_b[7] = REPLY;    //主叫
    memcpy(send_b + 8, LocalCfg.Addr, 20);
    memcpy(send_b + 28, LocalCfg.Mac_Addr, 6);
    send_b[34] = LocalCfg.DefenceStatus;
    send_b[35] = LocalCfg.DefenceNum;
    for(k=0; k<(LocalCfg.DefenceNum*6); k++)
      memcpy(send_b + 36 + 10*k, LocalCfg.DefenceInfo[k], 10);
    
    sendlength = 36 + LocalCfg.DefenceNum*6*10;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, length, 0);

    if(((recv_buf[29] << 8) + recv_buf[28]) != LocalCfg.ReportTime)
    {
      LocalCfg.ReportTime = (recv_buf[29] << 8) + recv_buf[28];
      WriteCfgFile();
    }
    xtm_printf("收到管理中心查询设备状态\n");
  }
}
//---------------------------------------------------------------------------
//报警
void RecvAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j;
  uint8_t isAddrOK;
  unsigned char AlarmByte;
  unsigned char tmp_p;

  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
  {
    if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
      isAddrOK = 0;
      break;
    }
  }
  //地址匹配
  if(isAddrOK == 1)
  {
    if((recv_buf[7] & 0x03) == REPLY)   //应答
    {
      if(ClearMultiUdpBuff(ALARM, ASK, LocalCfg.Addr[0], cFromIP))
      {
        AlarmByte = (recv_buf[37] & 0x3F);
        tmp_p = 0x01;
        for(j=0; j<6; j++)
        {
          if((AlarmByte & tmp_p) == tmp_p)
            LocalCfg.DefenceInfo[j][3] = 2; //报警已接收
          tmp_p = (tmp_p << 1);
        }
        xtm_printf("收到报警应答\n");
      }
    }
  }
}
//---------------------------------------------------------------------------
//取消报警
void RecvCancelAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j;
  uint8_t isAddrOK;
  uint8_t sendlength;

  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
  {
    if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  }
  //地址匹配
  if(isAddrOK == 1)
  {
    if((recv_buf[7] & 0x03) == REPLY)   //应答
    {
      if(ClearMultiUdpBuff(CANCELALARM, ASK, LocalCfg.Addr[0], cFromIP))
      {
        xtm_printf("收到取消报警应答\n");
      }
    }
  }
}
//---------------------------------------------------------------------------
void RecvResetPass_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j;
  uint8_t isAddrOK;
  uint16_t sendlength;

  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
  {
    if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
      isAddrOK = 0;
      break;
    }
  }
  //地址匹配
  if(isAddrOK == 1)
  {
    if((recv_buf[7] & 0x03) == ASK)   //应答
    {
      memcpy(send_b, recv_buf, length);
      send_b[7] = REPLY;    //应答
      sendlength = length;
      CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

      //复位密码
      switch(recv_buf[28])
      {
        case 1: //复位工程密码
            xtm_printf("复位工程密码成功\n");
            if(strcmp(LocalCfg.EngineerPass, "1234") != 0)
            {
              strcpy(LocalCfg.EngineerPass, "1234");
              WriteCfgFile();     //向Flash中存储文件
            }
          break;
        case 2: //复位开锁密码
          break;
      }
    }
  }
}

//---------------------------------------------------------------------------
uint16_t  OldTestPackNo = 0;
//收发测试
void RecvUdpTest_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint16_t PackNo, i;

  //xtm_printf("RecvUdpTest_Func:: %d, %d, %d\n", length, recv_buf[8], recv_buf[9]);
  PackNo = recv_buf[8];
  PackNo = PackNo << 8;
  PackNo += recv_buf[9];
  if((OldTestPackNo + 1) != PackNo)
    xtm_printf("OldTestPackNo = %d, PackNo = %d\n", OldTestPackNo, PackNo);

  OldTestPackNo = PackNo;
  xtm_printf("%d, %d\n", recv_buf[8], recv_buf[9]);

  //sendlength = length;
  memcpy(send_b, recv_buf, length);
  send_b[7] = REPLY;

  CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, length, 0);    // Send_Stream+0x0a
}
//---------------------------------------------------------------------------
#define MAXTESTPACKNUM  28
uint32_t TotalTestNum = 0;
//连续包接收测试
void RecvUdpTest1_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint16_t PackNo;
  uint16_t CurrNo;
  uint8_t j, PackOK, LostPackNo[20], LostPackNum;;

  TotalTestNum ++;
  if((TotalTestNum % 50) == 0)
  	CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, length, 0); 
  //xtm_printf("%d, %d, %d\n", length, recv_buf[8], recv_buf[9]);
  PackNo = recv_buf[8];
  PackNo = PackNo << 8;
  PackNo += recv_buf[9];
  if(OldTestPackNo != PackNo)
  //  xtm_printf("OldTestPackNo = %d, PackNo = %d\n", OldTestPackNo, PackNo);
  {
    for(j=0; j<MAX_VIDEO_PACK; j++)
    {
      PackExist[0][j] = 0;
      PackFrameno[0][j] = 0;
    }
  }
  CurrNo =  recv_buf[11];
  OldTestPackNo = PackNo;
 // xtm_printf("%d, %d, %d\n", recv_buf[8], recv_buf[9], CurrNo);
#if 1
  if(PackExist[0][CurrNo] == 1)
    xtm_printf("multi recv %d, %d, %d\n", recv_buf[8], recv_buf[9], CurrNo);
  PackExist[0][CurrNo] = 1;
  if(CurrNo == (MAXTESTPACKNUM - 1))
  {
    //xtm_printf("%d, %d, %d\n", recv_buf[8], recv_buf[9], CurrNo);
    PackOK = 1;
    LostPackNo[0] = 100;
    LostPackNum = 0;
    for(j=0; j<MAXTESTPACKNUM; j++)
    {
      if(PackExist[0][j] == 0)
      {
        PackOK = 0;
        LostPackNo[LostPackNum] = j;
        LostPackNum ++;
        // break;
      }
    }
    for(j=0; j<MAX_VIDEO_PACK; j++)
    {
      PackExist[0][j] = 0;
      PackFrameno[0][j] = 0;
    }
    if(PackOK == 0)
    {
      for(j=0; j<LostPackNum; j++)
        xtm_printf("%d, %d, LostPackNo[%d] = %d\n", recv_buf[8], recv_buf[9], j, LostPackNo[j]);
      return;
    }
  }
#endif   
}
//--------------------------------------------------------------------------
//检查是否在同一子网
uint8_t CheckSameNet(uint8_t *IP1, uint8_t *IP2, uint8_t *IPMask)
{
  uint8_t isOK = 1, i;
  for(i=0; i<4; i++)
  {
  	if((IP1[i] & IPMask[i]) != (IP2[i] & IPMask[i]))
  	{
		  isOK = 0;
		  break;
  	}
  }
  return isOK;
}
//--------------------------------------------------------------------------

//搜索所有设备应答
void RecvSearchAllEquip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint8_t *DestIP, uint16_t DestPort, uint16_t length)
{
  uint8_t isAddrOK, i, dIP[4];
  uint16_t sendlength;
  char tmp_text[20];
  uint32_t Ip_Int;

  isAddrOK = 1;
  xtm_printf("RecvSearchAllEquip_Func  = %d.%d.%d.%d\n", \
  	LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);

  xtm_printf("RecvSearchAllEquip_Func1, s_socket = %d, cFromIP = %s, IP = %d.%d.%d.%d\n", \
  	s_socket, cFromIP, LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);

  if(recv_buf[7] == REPLY)
    return;

  #if 0
    for(j=8; j<8+Local.AddrLen; j++)
      if(LocalCfg.Addr[j-8] != recv_buf[j])
      {
        isAddrOK = 0;
        break;
      }
  #endif
  
  Ip_Int=inet_addr(cFromIP);
  memcpy(dIP, &Ip_Int, 4);
  if(CheckSameNet(LocalCfg.IP, dIP, LocalCfg.IP_Mask) == 0)
  {
	  // sprintf(cFromIP, "%d.%d.%d.255", dIP[0], dIP[1], dIP[2]);
	  strcpy(cFromIP, "255.255.255.255");
  }
    
  //地址匹配
  if(isAddrOK == 1)
  {
    //xtm_printf("RecvSearchAllEquip_Func2, s_socket = %d, cFromIP = %s, UDP_DataSocket = %d\n", s_socket, cFromIP, UDP_DataSocket);

    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答

    send_b[8] = 0x05;
    send_b[9] = 0;

    //地址编码
    memcpy(send_b + 10, LocalCfg.Addr, 20);
    //网卡地址
    memcpy(send_b + 30, LocalCfg.Mac_Addr, 6);
    //IP地址
    memcpy(send_b + 36, LocalCfg.IP, 4);
    //子网掩码
    memcpy(send_b + 40, LocalCfg.IP_Mask, 4);
    //网关地址
    memcpy(send_b + 44, LocalCfg.IP_Gate, 4);
    //服务器地址
    memcpy(send_b + 48, LocalCfg.IP_Server, 4);
    //版本号
    for(i=0; i<20; i++)
      tmp_text[i] = '\0';

    #ifdef _SEND_TO_SELF_TEST    //发送数据包给自己
      sprintf(tmp_text, "%s-%04X-%04X", SERIALNUM, LocalCfg.BootNo, LocalCfg.SelfBootNo);
    #else
      sprintf(tmp_text, "%s-%04X", SERIALNUM, LocalCfg.BootNo);
    #endif
    
    memcpy(send_b + 52, tmp_text, 16);
    //多分机支持
    send_b[68] = LocalCfg.MultiMachineSupport;
    send_b[69] = LocalCfg.DenMaxNum;  //副机最大数量，这一数量需小于 SUBMAXNUM， 有利于减小广播包
    send_b[74] = LocalCfg.Language;
    send_b[90] = LocalCfg.MicDecValue;  //展箱、展板模式,需减小 Mic音量
    //同步码
    memcpy(send_b + 92, LocalCfg.AddrVerifyMa, 6);
    sendlength = 98;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
  }
}
//---------------------------------------------------------------------------
//写设备地址应答
void RecvWriteEquipAddr_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint8_t *DestIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j, isAddrOK, dIP[4];
  uint16_t sendlength;
  uint32_t Ip_Int;

  if(recv_buf[7] == REPLY)
    return;

  xtm_printf("RecvWriteEquipAddr_Func::length = %d\n", length);

  isAddrOK = 1;
  #if 1
    for(j=10; j<10+Local.AddrLen; j++)
    {
      if(LocalCfg.Addr[j-10] != recv_buf[j])
      {
        isAddrOK = 0;
        break;
      }
    }

    for(j=30; j<30+6; j++)
    {
      if(LocalCfg.Mac_Addr[j-30] != recv_buf[j])
      {
        isAddrOK = 0;
        break;
      }
    }

    for(j=36; j<36+4; j++)
    {
      if(LocalCfg.IP[j-36] != recv_buf[j])
      {
        isAddrOK = 0;
        break;
      }
    }
  #endif

  Ip_Int=inet_addr(cFromIP);
  memcpy(dIP, &Ip_Int, 4);
  if(CheckSameNet(LocalCfg.IP, dIP, LocalCfg.IP_Mask) == 0)
  {
	  //sprintf(cFromIP, "%d.%d.%d.255", dIP[0], dIP[1], dIP[2]);
	  strcpy(cFromIP, "255.255.255.255");
  }

  //地址匹配
  if(isAddrOK == 1)
  {
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答
    sendlength = length;

    UdpSendBuff(s_socket, cFromIP, RemotePort[0], (char *)send_b, sendlength);

    //设备定时报告状态时间
    LocalCfg.ReportTime = 10;
       
    //写地址设置
    memcpy(LocalCfg.Addr, recv_buf + 52, 20);
    memcpy(LocalCfg.Mac_Addr, recv_buf + 72, 6);
    memcpy(LocalCfg.IP, recv_buf + 78, 4);
    memcpy(LocalCfg.IP_Mask, recv_buf + 82, 4);
    memcpy(LocalCfg.IP_Gate, recv_buf + 86, 4);
    memcpy(LocalCfg.IP_Server, recv_buf + 90, 4);
    LocalCfg.MultiMachineSupport = recv_buf[94];
    LocalCfg.DenMaxNum = recv_buf[95];  //副机最大数量，这一数量需小于 SUBMAXNUM， 有利于减小广播包

    LocalCfg.MicDecValue = recv_buf[112];  //展箱、展板模式,需减小 Mic音量

    if(LocalCfg.MicDecValue > 3) //展箱、展板模式,需减小 Mic音量
    {
      LocalCfg.MicDecValue = 0;
    }
//    SetMicVolume();

    //同步码
    memcpy(LocalCfg.AddrVerifyMa, recv_buf + 114, 6);
    LocalCfg.AddrVerifyMa[6] = '\0';    

    if((LocalCfg.Addr[0] != 'S')&&(LocalCfg.Addr[0] != 'B'))
      LocalCfg.Addr[0] = 'B';
    
    if(LocalCfg.Addr[0] == 'B')
      memcpy(LocalCfg.Addr + 5, "000000", 6);
    
    if((LocalCfg.Addr[0] == 'S')||(LocalCfg.Addr[0] == 'B'))
    {
      Local.AddrLen = 12;  //地址长度  S 12  B 12 M 8 H 6
    }
    
    //主机
    Local.isHost = '0';
    Local.isHost = LocalCfg.Addr[11];
    Local.DenNum = 0;
    memcpy(Local.HostAddr, LocalCfg.Addr, 20);
    Local.HostAddr[11] = '0';
    WriteCfgFile();  //写本地设置文件

    #ifdef _XTM8900_SUPPORT  //XTM8900网卡支持
		  net_refresh_addr(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
	  #endif
	  #ifdef _XTM9000_SUPPORT  //XTM9000网卡支持
		  net_refresh_addr_9000(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
	  #endif	
  }
}
//---------------------------------------------------------------------------
uint16_t OldPackage = 0;
//下载
void RecvDownLoadFile_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
#if 1
  uint16_t PackDataLen = 1024;//256;
  uint16_t sendlength, i;
  uint8_t DownOK;
  uint16_t RemotePort1;
  char RemoteIP1[20];

  struct downfile1 DownData;

  memcpy(&DownData, recv_buf + 10, sizeof(struct downfile1));
  if((DownData.FlagText[0] == FLAGTEXT[0])&&(DownData.FlagText[1] == FLAGTEXT[1])&&(DownData.FlagText[2] == FLAGTEXT[2]) && \
     (DownData.FlagText[3] == FLAGTEXT[3])&&(DownData.FlagText[4] == FLAGTEXT[4])&&(DownData.FlagText[5] == FLAGTEXT[5]))
  {
    //xtm_printf("RecvDownLoadFile_Func::recv_buf[8] = 0x%X\n", recv_buf[8]);
    switch(recv_buf[8])
    {
      case STARTDOWN:
          if( (Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)||
              (Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))
            TalkEnd_Func();
          
          if(Local.Status == 3)  //状态为监视
            WatchEnd_Func();

          xtm_printf("RecvDownLoadFile_Func::  STARTDOWN  length = %d\n", length);
          for(i=0; i<MAX_DOWN_PACK; i++)
            Down_RecvFlag[i] = 0;           //下载 包标志
          
          memcpy(send_b, recv_buf, length);
          send_b[7] = REPLY;    //应答
          sendlength = length;
          CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        break;
      case DOWN:              //下载
          if(DownData.TotalPackage > MAX_DOWN_PACK)
          {
            xtm_printf("DownData.TotalPackage = %d, too large\n", DownData.TotalPackage);
            break;
          }
          Down_RecvFlag[DownData.CurrPackage] = 1;

          #ifdef _PACK_1024
            memcpy(Memory6_Stream + DownData.CurrPackage * 1024, recv_buf + 10 + sizeof(struct downfile1), DownData.Datalen);
          #endif

          xtm_printf("RecvDownLoadFile_Func::length = %d, DownData.TotalPackage = %d, DownData.CurrPackage = %d\n", length, DownData.TotalPackage, DownData.CurrPackage);
          DownOK = 1;
          // xtm_printf("DestIP2 = %d.%d.%d.%d\n", cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3]);
					strcpy( RemoteIP1, cFromIP);
          RemotePort1 = DestPort;
          memcpy(send_b, recv_buf, length);
          if(DownData.CurrPackage == (DownData.TotalPackage - 1))
          {
            xtm_printf("WriteAllToFlash start\n");
					  //tt1 = gm_timer_get_tick(0, 0);
            DownOK = WriteAllToFlash(recv_buf, &DownData);
					  //tt2 = gm_timer_get_tick(0, 0);
            xtm_printf("WriteAllToFlash finish\n");

            #ifdef  _ADDTABLE_SUPPORT
              if(DownData.Faddr == ADDRTABLE_ADDR)
              {
                //ReadAddrTableFile();
              }
            #endif
          }
          if(DownOK)
          {
            //memcpy(send_b, recv_buf, length);
            send_b[7] = REPLY;    //应答
            sendlength = length;
            
            CreateMultiUdpBuff(s_socket, SINGLE_SEND, RemoteIP1, RemotePort1, send_b, sendlength, 0);
          }
          else
          {
            send_b[7] = ASK;    //应答
            send_b[8] = DOWNFAIL;
            sendlength = length;
            CreateMultiUdpBuff(s_socket, SINGLE_SEND, RemoteIP1, RemotePort1, send_b, sendlength, 0);
            xtm_printf("send DOWNFAIL\n");
          }
        break;
      case DOWNFINISHONE:         //下载完成一个
          memcpy(send_b, recv_buf, length);
          send_b[7] = REPLY;    //应答
          sendlength = length;
          CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
          xtm_printf("下载完成一个\n");
        break;
      case STOPDOWN:             //停止下载
          memcpy(send_b, recv_buf, length);
          send_b[7] = REPLY;    //应答
          sendlength = length;
          CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
          xtm_printf("停止下载\n");
        break;
    }
  }
#endif  
}
//---------------------------------------------------------------------------
uint8_t WriteAllToFlash(uint8_t *recv_buf, struct downfile1 *DownData)
{
#if 1
  uint16_t PackOK, total;
  uint32_t paddr, offset, i, FlashLen, PackLen;
  uint16_t sendlength;
  uint8_t m_crc, m_crc1;
  uint32_t file_len;
  unsigned int tt1, tt2, tt3, tt4;

  PackOK = 1;
  for(i=0; i<DownData->TotalPackage; i++)
  {
    if(Down_RecvFlag[i] == 0)
    {
      PackOK = 0;
      xtm_printf("WriteAllToFlash::PackOK = %d, i = %d\n", PackOK, i);
      break;
    }
  }
  if(PackOK)
  {
    //check crc
    if(DownData->FileType == 0)
    {
      m_crc = recv_buf[9];
      m_crc1 = 0;
      file_len = 0;
      #ifdef _PACK_1024
        for(i=0; i<DownData->Filelen; i++)
        {
          m_crc1 += Memory6_Stream[i];
          // ClearWatchDog();
        }
      #endif
      xtm_printf("m_crc = 0x%X, m_crc1 = 0x%X, Filelen = %d\n", m_crc, m_crc1, DownData->Filelen);
      if(m_crc != m_crc1)
      {
        PackOK = 0;
        xtm_printf("WriteAllToFlash::crc error\n");
      }
    }
  }

  if(PackOK)
  {
    paddr = DownData->Faddr;
    if((DownData->FileType == 1)||(DownData->FileType == 2))   //图片  声音
    {
  	  xtm_printf("paddr = 0x%X, 0x%X  0x%X  0x%X  0x%X \n", paddr, recv_buf[30], recv_buf[31], recv_buf[32], recv_buf[33]);
      gm_spi_flash_data_write(paddr, 256, recv_buf + 30);
      paddr += 256;
    }
    
    #ifdef _PACK_1024
      if((DownData->Filelen % 256) == 0)
        FlashLen = DownData->Filelen;
      else
        FlashLen = (DownData->Filelen / 256)*256 + 256;
      
      xtm_printf("paddr = 0x%X, Filelen = %d, FlashLen= %d\n", paddr, DownData->Filelen, FlashLen);

	    tt1 = gm_timer_get_tick(0, 0);  
      #if 1
        PackLen = 65536;
	      
	      if((FlashLen % PackLen) == 0)
		      total = FlashLen / PackLen;
	      else
		      total = FlashLen / PackLen + 1;
       
	      for(i=0; i<total; i++)
	      {
		      if(i == (total - 1))
			      gm_spi_flash_data_write(paddr + PackLen*i, FlashLen - PackLen*i, Memory6_Stream + PackLen*i); 
		      else
		      {
			      tt3 = gm_timer_get_tick(0, 0);
			      gm_spi_flash_data_write(paddr + PackLen*i, PackLen, Memory6_Stream + PackLen*i); 
			      ClearWatchDog();
			      tt4 = gm_timer_get_tick(0, 0);		
			      xtm_printf("time, %d, PackLen = %d\n", tt4 - tt3, PackLen);
		      }
	      }
      #else		
        PackLen = FlashLen;
        gm_spi_flash_data_write(paddr, FlashLen, Memory6_Stream);   
      #endif
	    tt2 = gm_timer_get_tick(0, 0);
	    xtm_printf("WriteFlash all time, %d, PackLen = %d\n", tt2 - tt1, PackLen);
    #endif	
  }

  for(i=0; i<MAX_DOWN_PACK; i++)
    Down_RecvFlag[i] = 0;           //下载 包标志
  
  if(PackOK)
    return 1;
  else
    return 0;
#endif
}
//---------------------------------------------------------------------------
//读Flash内容
void RecvReadFlash_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
#if 1
  uint16_t sendlength;
  uint32_t tmp_addr;
  uint16_t ReadLen;

  struct downfile1 DownData;

  memcpy(&DownData, recv_buf + 10, sizeof(struct downfile1));
  // memcpy(&tmp_addr, recv_buf + 8, 4);
  // memcpy(&ReadLen, recv_buf + 12, 2);

  //xtm_printf("0x%X, 0x%X, 0x%X, 0x%X\n", recv_buf[29], recv_buf[30], recv_buf[31], recv_buf[32]);
  xtm_printf("RecvReadFlash_Func::DownData.Faddr = 0x%X, DownData.FLen = 0x%X, DownData.CurrPackage = 0x%X\n", \
              DownData.Faddr, \
              DownData.FLen, \
              DownData.CurrPackage);
  ReadLen = 1024;

  if((DownData.FlagText[0] == FLAGTEXT[0])&&(DownData.FlagText[1] == FLAGTEXT[1])&&(DownData.FlagText[2] == FLAGTEXT[2])
     &&(DownData.FlagText[3] == FLAGTEXT[3])&&(DownData.FlagText[4] == FLAGTEXT[4])&&(DownData.FlagText[5] == FLAGTEXT[5]))
  {
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答
    sendlength = 10 + sizeof(struct downfile1) + ReadLen;
    ReadFromFlash(DownData.Faddr + (uint32_t)(DownData.CurrPackage) * ReadLen, send_b + 10 + sizeof(struct downfile1), ReadLen);
    //ReadFromFlash(DownData.Faddr + DownData.CurrPackage * ReadLen, send_b + 10 + sizeof(struct downfile1), ReadLen);
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
  }
#endif
}

//---------------------------------------------------------------------------
//远程重启动
void RecvRemoteReset_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  s32 wdt_sec=1; 
  uint16_t sendlength;
  memcpy(send_b, recv_buf, length);
  send_b[7] = REPLY;    //应答
  sendlength = length;
  UdpSendBuff(s_socket, cFromIP, DestPort, (char *)send_b, length);
  xtm_printf("DataPortDeal::远程重启动, %s\n", cFromIP);
  Local.GpioWatchDogStarted = 0;  //在GPIO检测线程中启动WatchDog
  
  //gm_wdt_init();
  //gm_wdt_ioctl(WDIOC_SETTIMEOUT,&wdt_sec);
}

//---------------------------------------------------------------------------
#ifdef _ADDTABLE_SUPPORT  //地址表支持
//读地址表信息
void RecvReadAddrTable_Func(int s_socket, uint8_t *recv_buf, uint8_t *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint16_t sendlength;

  memcpy(send_b, recv_buf, length);
  send_b[7] = REPLY;    //应答
  memcpy(send_b + 10,  AddrTable.Version, 30);
  send_b[40] = 0;
  send_b[41] = 0;
  send_b[42] = (AddrTable.TotalNum >> 8) & 0xff;
  send_b[43] = AddrTable.TotalNum & 0xff;
  sendlength = 44;

  UdpSendBuff(s_socket, cFromIP, DestPort, (char *)send_b, length);
}
#endif     
//---------------------------------------------------------------------------
void UdpVideoRecvTask(int *s_socket)
{
	struct sockaddr_in from;
	
	int nLength, recv_len;
	char FromIP[20];
	uint16_t DestPort;
	nLength = sizeof(struct sockaddr_in);
	printf("UdpVideoRecvTask::*s_socket = %d\n", *s_socket);
	while(1)
	{
		recv_len = recvfrom(*s_socket,
			                  (char *)recvBuf1,
			                  sizeof(recvBuf1),
			                  0,
			                  (struct sockaddr *)&from,
			                  (socklen_t*)&nLength);

		//printf("%d\n", recv_len);
		//printf("UdpVideoRecvTask::1\n");
		if(recv_len <= 0)
		{			
			printf("UdpVideoRecvTask::%d\n", recv_len);
			continue;
		}
		//recvBuf[recv_len] = '\0';
		//strcpy(FromIP, inet_ntoa(from.sin_addr));
		//memcpy(cFromIP, from.sin_addr, 4);
		
		//DestPort=ntohs(from.sin_port);
		//printf("UdpVideoRecvTask::FromIP = %s:%d\n", FromIP, DestPort);
		if((recvBuf1[0] == UdpPackageHead[0])&&(recvBuf1[1] == UdpPackageHead[1])&&(recvBuf1[2] == UdpPackageHead[2])&&
			 (recvBuf1[3] == UdpPackageHead[3])&&(recvBuf1[4] == UdpPackageHead[4])&&(recvBuf1[5] == UdpPackageHead[5]))
		{		
			recvBuf[recv_len] = '\0';
			strcpy(FromIP, inet_ntoa(from.sin_addr));
		
			DestPort=ntohs(from.sin_port);		
			switch(recvBuf1[6])
			{
				case NSORDER:	//主机名解析（子网内广播）
				// case NSSERVERORDER:  //主机名解析(NS服务器)
						switch(recvBuf1[7] & 0x03)
						{
							case 1://解析
									if(recv_len >= SMALLESTSIZE)
									{
										xtm_printf("解析请求, recv_len = %d\n", recv_len);
										RecvNSAsk_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									}
									else
									{
										xtm_printf("解析请求数据长度异常, recv_len = %d\n", recv_len);
									}
								break;
							case 2://解析回应
									if(recv_len >= SMALLESTSIZE)
									{
									  RecvNSReply_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									}
									else
									{
										xtm_printf("解析应答数据长度异常\n");
									}
								break;
						}
					break;
				case VIDEOTALK:    //局域网可视对讲
						//xtm_printf("buff[6] = %d, buff[7] = %d, buff[8] = %d\n", buff[6], buff[7], buff[8]);
						switch(recvBuf1[8])	  //42 + 8
					  {
			        #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
							  case LEAVEGROUP:		  //退出组播组（主叫方->被叫方，被叫方应答）
									  if(recv_len >= SMALLESTSIZE)
										{
										  RecvTalkLeaveGroup_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
										}
										else
										{
											xtm_printf("对讲退出组播组数据长度异常\n");
										}
									break;
			        #endif
							case CALL:		//对方发起对讲
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCall_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("对讲呼叫数据长度异常\n");
									}
								break;
							case LINEUSE: 	   //对方正忙
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkLineUse_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("占线应答数据长度异常\n");
									}
								break;
							case CALLANSWER:	//呼叫应答
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCallAnswer_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("对讲呼叫应答数据长度异常\n");
									}
									break;
							case CALLSTART:  //被叫方开始通话
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCallStart_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("对讲开始通话数据长度异常\n");
									}
								break;
							case CALLCONFIRM: 	//通话在线确认
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCallConfirm_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("对讲通话在线确认数据长度异常\n");
									}
								break;
							case CALLEND:  //通话结束
									//结束播放视频
									//如为对方结束，需应答
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCallEnd_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("结束对讲数据长度异常\n");
									}
								break;
							case REMOTEOPENLOCK:	  //远程开锁
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkRemoteOpenLock_Func(*s_socket, recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("远程开锁数据长度异常\n");
									}
								break;
							case CENTERTURNTALK:	   //中心人工转接
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCenterTurnTalk_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("中心人工转接数据长度异常\n");
									}
								break;
							case TRUSTEESHIP_TALK:	//托管
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkTrusteeship_Talk_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("托管数据长度异常\n");
									}
								break;
							case CALLUP: //通话上行
							case CALLDOWN: //通话下行
									RecvTalkCallUpDown_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
								break;
						}
					break;
				case VIDEOWATCH:	 //局域网监控
						switch(recvBuf1[8])
						{
							case CALL:		//对方发起监视	室内机不响应监视
								break;
							case LINEUSE: 	   //对方正忙
								  if(recv_len >= SMALLESTSIZE)
									{
										RecvWatchLineUse_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("占线应答数据长度异常\n");
									}
								break;
							case CALLANSWER:	//呼叫应答
									if(recv_len >= SMALLESTSIZE)
									{
										RecvWatchCallAnswer_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("监视呼叫应答数据长度异常\n");
									}
								break;
							case CALLCONFIRM: 	//通话在线确认
									if(recv_len >= SMALLESTSIZE)
									{
										RecvWatchCallConfirm_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("监视通话在线确认数据长度异常\n");
									}
								break;
							case CALLEND:  //通话结束
									//结束播放视频
									//如为对方结束，需应答
									if(recv_len >= SMALLESTSIZE)
									{
										RecvWatchCallEnd_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("结束监视数据长度异常\n");
									}
								break;
							case CALLUP: //通话上行
							case CALLDOWN: //通话下行
									RecvWatchCallUpDown_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
								break;
					  }
					break;
        #ifdef ELEVATORCONTROL_HANGZHOU
				  case ELEVATR_ORDER1:
						  if(recv_len >= SMALLESTSIZE)
							{
							  RecvElevatorControl_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
							}
						  else
							{
							  xtm_printf("电梯控制数据长度异常\n");
							}
						break;
        #endif
	      #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
					case HOSTTOSUB: //主机与副机通信
						  switch(recvBuf1[8])
							{
							  case SYNCSUB: 	   //同步状态
										if(recv_len >= SMALLESTSIZE)
										{
										  RecvSyncSub_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
										}
										else
										{
											xtm_printf("同步状态数据长度异常\n");
										}
									break;
							  case SUBDEFENCE:		 //布防（副机－＞主机）
										RecvSubDefence_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							  case SUBALARM:		 //报警（副机－＞主机）
										RecvSubAlarm_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							  case SUBALARMTIP: 	  //报警提示（主机－＞副机）
										RecvSubAlarmTip_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							  case SUBDEFENCETIP:		//布防提示（主机－＞副机）
										RecvSubDefenceTip_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							  case SUBFIND: 	  //查找副机（主机－＞副机）
										RecvSubFind_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							}
						break;
	      #endif
        #ifdef _REMOTE_FORTIFY_SUPPORT  //远程布撤防支持
					case REMOTE_FORTIFY:
							switch(recvBuf1[7] & 0x03)
							{
							  case 1://请求
									  if(recv_len >= SMALLESTSIZE)
									  {
										  xtm_printf("远程布撤防请求, recv_len = %d\n", recv_len);
										  RecvRemoteFortify_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									  }
									  else
									  {
										  xtm_printf("远程布撤防请求数据长度异常, recv_len = %d\n", recv_len);
									  }
									break;
							  case 2://应答
									  if(recv_len >= SMALLESTSIZE)
									  {
									    xtm_printf("远程布撤防请求应答, recv_len = %d\n", recv_len);
									  }
									  else
									  {
										  xtm_printf("远程布撤防应答数据长度异常, recv_len = %d\n", recv_len);
									  }
									break;
							}
						break;
        #endif
			}
		}
		if(recv_len > 0)
		{
			//printf("UdpVideoRecvTask::len = %d\n", recv_len);
		}		
	}
	//vTaskDelete(NULL);		
}

#ifdef  _PON_SUPPORT
//---------------------------------------------------------------------------
void  UdpPonRecvTask(int *s_socket)
{
	struct sockaddr_in from;
	
	int nLength, recv_len;
	char FromIP[20];
	uint16_t DestPort;
	UINT8 *DestIP;  //lsx add not value set
	nLength = sizeof(struct sockaddr_in);
	printf("UdpPonRecvTask::*s_socket = %d, UDP_PonSocket = %d\n", *s_socket, UDP_PonSocket);
	xtm_printf("UdpPonRecvTask::*s_socket = %d, UDP_PonSocket = %d\n", *s_socket, UDP_PonSocket);
  while(1)
  {
		recv_len = recvfrom(*s_socket,
					              (char *)recvBuf2,
					              sizeof(recvBuf2),
					              0,
					              (struct sockaddr *)&from,
					              (socklen_t*)&nLength );

		//printf("UdpPonRecvTask::*s_socket = %d, UDP_PonSocket = %d\n", *s_socket, UDP_PonSocket);
		if(recv_len <= 0)
			continue;
		
		recvBuf2[recv_len] = '\0';
		strcpy(FromIP, inet_ntoa(from.sin_addr));				
		DestPort = ntohs(from.sin_port);

		if((recvBuf2[0] == PonPackageHead[0])&&(recvBuf2[1] == PonPackageHead[1])&&(recvBuf2[2] == PonPackageHead[2])&&
			 (recvBuf2[3] == PonPackageHead[3])&&(recvBuf2[4] == PonPackageHead[4])&&(recvBuf2[5] == PonPackageHead[5]))
		{
			switch(recvBuf[6])
			{
        case PONSEARCHALLEQUIP:  //240  //搜索所有设备（管理中心－＞设备）
            if(recv_len >= SMALLESTSIZE)
            {
              RecvPonSearchAllEquip_Func(recvBuf2, FromIP, DestIP, DestPort, recv_len);
            }
            else
            {
              xtm_printf("搜索所有设备长度异常\n");
            }
          break;
        case PONWRITEEQUIPADDR:  //241  //写设备地址（管理中心－＞设备）
            if(recv_len >= SMALLESTSIZE)
            {
              RecvPonWriteEquipAddr_Func(recvBuf2, FromIP, DestIP, DestPort, recv_len);
            }
            else
            {
              xtm_printf("写设备地址长度异常\n");
            }
          break;
			}
		}
  }
}
//---------------------------------------------------------------------------
void RecvPonSearchAllEquip_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length)
{
  xtm_printf("RecvPonSearchAllEquip_Func\n");
  if(recv_buf[7] == ASK)
    return;

  if(recv_buf[7] == REPLY)
  {
		if(ClearMultiUdpBuff(PONSEARCHALLEQUIP, ASK, 0, cFromIP))
		{
      PonSearch.isValid = 1;
      if(recv_buf[9] > 1)
        recv_buf[9] = 0;
      PonSearch.dev_mode = recv_buf[9];

      memcpy(PonSearch.OldMac, recv_buf + 10, 6); //原网卡地址
      memcpy(PonSearch.Mac_Addr, recv_buf + 10, 6); //网卡地址
      memcpy(PonSearch.IP, recv_buf + 16, 4); //IP地址
      memcpy(PonSearch.IP_Mask, recv_buf + 20, 4); //子网掩码
      memcpy(PonSearch.IP_Gate, recv_buf + 24, 4); //网关地址

      memcpy(PonSearch.loid, recv_buf + 28, 30);
      PonSearch.loid[29] = '\0';
      memcpy(PonSearch.passwd, recv_buf + 58, 30);
      PonSearch.passwd[29] = '\0';
      memcpy(PonSearch.hw_sn, recv_buf + 88, 30);
      PonSearch.hw_sn[29] = '\0';
      memcpy(PonSearch.gpon_sn, recv_buf + 118, 30);
      PonSearch.gpon_sn[29] = '\0';
      memcpy(PonSearch.gpon_passwd, recv_buf + 148, 30);
      PonSearch.gpon_passwd[29] = '\0';
      memcpy(&PonSearch.temperature, recv_buf + 178, 2);
      memcpy(&PonSearch.volt, recv_buf + 180, 2);
      memcpy(&PonSearch.tx_bias, recv_buf + 182, 2);
      memcpy(&PonSearch.tx_pwr, recv_buf + 184, 2);
      memcpy(&PonSearch.rx_pwr, recv_buf + 186, 2);

      xtm_printf("    Tempurature(1/256 degree C): %d\n", PonSearch.temperature);
      xtm_printf("    Voltage(20mV)              : %d\n", PonSearch.volt);
      xtm_printf("    Tx bias current(2uA)       : %d\n", PonSearch.tx_bias);
      xtm_printf("    Tx power(0.002dB)          : %d\n", PonSearch.tx_pwr);
      xtm_printf("    Rx power(0.002dB)          : %d\n", PonSearch.rx_pwr);

      PonSearch_Result();
		}
  }
}
//---------------------------------------------------------------------------
//写设备地址应答
void RecvPonWriteEquipAddr_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length)
{
  if(recv_buf[7] == ASK)
    return;

  xtm_printf("RecvPonWriteEquipAddr_Func \n");

  if(recv_buf[7] == REPLY)
  {
    if(ClearMultiUdpBuff(PONWRITEEQUIPADDR, ASK, 0, cFromIP))
    {
      PonSearch.isValid = 1;
      RoomSearchFlag = 0;
      RoomSearchTime = 0;
      PlayModiSuccWav();
      PonWrite_Result();
      //InitArp_Table();
    }
  }
}
#endif
//---------------------------------------------------------------------------
void CreateMultiUdpBuff(int s_socket, uint8_t SendMode, char *RemoteIP, uint16_t DestPort, uint8_t *udpdata, uint16_t nlength, uint8_t CurrOrder)
{
  uint8_t i;
  if(Udp_InitFlag != 1)
  	return;

  for(i=0; i<UDPSENDMAX; i++)
    if(Multi_Udp_Buff[i].isValid == 0)
    {
      //xtm_printf("CreateMultiUdpBuff::i = %d, RemoteIP = %s:%d\n", i, RemoteIP, DestPort);
      //加锁
      //xSemaphoreTake(multi_send_mutex, portMAX_DELAY);
      if((Multi_Udp_Buff[i].buf[6] == VIDEOWATCH)&&(Multi_Udp_Buff[i].buf[8] == CALL))
        xtm_printf("CreateMultiUdpBuff:: VIDEOWATCH i = %d\n", i);
      
      Multi_Udp_Buff[i].isValid = 1; //是否有效
      Multi_Udp_Buff[i].s_socket = s_socket;
      Multi_Udp_Buff[i].SendMode = SendMode; //ASK -- 多次发送   REPLY --- 单次发送
      
      if(Multi_Udp_Buff[i].SendMode == MULTI_SEND)
        Multi_Udp_Buff[i].MaxSendNum = 6; //当前发送次数
      else
        Multi_Udp_Buff[i].MaxSendNum = 1; //当前发送次数
     
      Multi_Udp_Buff[i].CurrOrder = CurrOrder;//当前命令状态,VIDEOTALK  VIDEOWATCH
      //主要用于需解析时，如单次命令置为0
      Multi_Udp_Buff[i].DestPort = DestPort;

      strcpy(Multi_Udp_Buff[i].RemoteIP, RemoteIP);
      memcpy(Multi_Udp_Buff[i].buf, udpdata, nlength);
      Multi_Udp_Buff[i].SendNum = 0;
      Multi_Udp_Buff[i].TimeNum = 0;
      Multi_Udp_Buff[i].DelayTime = 600;  //等待时间  200ms
      Multi_Udp_Buff[i].SendDelayTime = 0; //发送等待时间计数  20101112
  	  //给管理软件发送的保持最小字节为512  
  	  if(DestPort == RemotePort[0])
   		  if(nlength < 512)
     		  nlength = 512;     
      
      Multi_Udp_Buff[i].nlength = nlength;

	    //解锁
      //xSemaphoreGive(multi_send_mutex);
     
	    xSemaphoreGive(multi_send_sem);

      //xtm_printf("CreateMultiUdpBuff::i = %d, %d %d, MaxSendNum = %d,  0x%X %X %X %X %X %X\n", i, Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8], Multi_Udp_Buff[i].MaxSendNum, \
      //            Multi_Udp_Buff[i].buf[0], Multi_Udp_Buff[i].buf[1], Multi_Udp_Buff[i].buf[2], Multi_Udp_Buff[i].buf[3], Multi_Udp_Buff[i].buf[4], Multi_Udp_Buff[i].buf[5]);
      break;
    }

    if(i >= UDPSENDMAX)
      xtm_printf("CreateMultiUdpBuff::buff is full\n");
}
//---------------------------------------------------------------------------
void MultiSendTask(void)
{
  uint8_t HaveDataSend, i;
  printf("MultiSendTask::..., %d\n", 10/portTICK_RATE_MS);
  while(1)
  {
  	xSemaphoreTake(multi_send_sem, portMAX_DELAY);
    HaveDataSend = 1;
	  //printf("MultiSendTask::1\n");
    while(HaveDataSend)
    { 
  	  CheckMultiUdpBuff();
      //判断数据是否全部发送完，若是，线程休眠
      HaveDataSend = 0;
      for(i=0; i<UDPSENDMAX; i++)
        if(Multi_Udp_Buff[i].isValid == 1)
        {
          HaveDataSend = 1;
          break;
        }	  
  	  vTaskDelay(10/portTICK_RATE_MS);
    }
  }
  //vTaskDelete(NULL);
}
void CheckMultiUdpBuff(void)   //检查UDP发送缓存
{
  uint8_t i;

  for(i=0; i<UDPSENDMAX; i++)
    if(Multi_Udp_Buff[i].isValid == 1)
    {
      //xtm_printf("CheckMultiUdpBuff::socket = %d, RemoteIP = %s:%d\n", Multi_Udp_Buff[i].s_socket, Multi_Udp_Buff[i].RemoteIP, Multi_Udp_Buff[i].DestPort);
       if(Multi_Udp_Buff[i].TimeNum == 0){
		   UdpSendBuff(Multi_Udp_Buff[i].s_socket, Multi_Udp_Buff[i].RemoteIP, Multi_Udp_Buff[i].DestPort, (char *)Multi_Udp_Buff[i].buf, Multi_Udp_Buff[i].nlength);
		   
		   if((Multi_Udp_Buff[i].buf[7] & 0x03) == REPLY)  //应答数据包， 只发一次
			{
			 Multi_Udp_Buff[i].isValid = 0;
			 continue;
			}
		   if(Multi_Udp_Buff[i].SendMode == SINGLE_SEND)  //只发一次
			{
			 Multi_Udp_Buff[i].isValid = 0;
			 continue;
			}
		   Multi_Udp_Buff[i].SendNum ++;
       	}

       if(Multi_Udp_Buff[i].SendNum > 1)
        if((Multi_Udp_Buff[i].buf[6] != VIDEOTALK)||(Multi_Udp_Buff[i].buf[8] != CALL))
         if(Multi_Udp_Buff[i].DelayTime > 200)
           Multi_Udp_Buff[i].DelayTime = 200;         
       if(Multi_Udp_Buff[i].SendNum >= Multi_Udp_Buff[i].MaxSendNum)
        {
           //加锁
           //xSemaphoreTake(multi_send_mutex, portMAX_DELAY);
           MultiUdpSendFailDeal(i);
           //解锁
           //xSemaphoreGive(multi_send_mutex);		   
        }
	   Multi_Udp_Buff[i].TimeNum += 10;
	   
	   if(Multi_Udp_Buff[i].TimeNum >= Multi_Udp_Buff[i].DelayTime)
		 Multi_Udp_Buff[i].TimeNum = 0;
    }
}
//---------------------------------------------------------------------------
void MultiUdpSendFailDeal(uint8_t i)
{
  uint8_t k, j;
  uint8_t Net1[4], Net2[4];
  uint8_t RemoteIP[4], RemoteIP1[4];
  switch(Multi_Udp_Buff[i].buf[6])
   {
    #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
    case HOSTTOSUB: //主机与副机通信
                 Multi_Udp_Buff[i].isValid = 0;
                 if(Multi_Udp_Buff[i].buf[8] == SYNCSUB)
                  {
                    Local.DenNum --;
                  }
                 xtm_printf("主副机通信失败, %d, %d, %d, %s\n", Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8],  Multi_Udp_Buff[i].buf[44] - '0', \
                            Multi_Udp_Buff[i].RemoteIP);
                 break;
    #endif
    case NSORDER:
               #ifdef ELEVATORCONTROL_HANGZHOU      //20120822
                if(Multi_Udp_Buff[i].CurrOrder == ELEVATR_ORDER1)
                 {
                  Multi_Udp_Buff[i].isValid = 0;
                  xtm_printf("电梯控制, 查找门口机失败\n");
                  break;
                 }
                #endif
                 if(Multi_Udp_Buff[i].CurrOrder == 255) //主机向副机解析
                  {
                   Multi_Udp_Buff[i].isValid = 0;
                   xtm_printf("查找副机失败\n");
                  }
                 else
                  {
                   // Multi_Udp_Buff[i].isValid = 0;
                    xtm_printf("解析失败\n");

                   //若该命令为子网解析，则下一个命令为向服务器解析
                   Multi_Udp_Buff[i].SendNum = 0;
                   Multi_Udp_Buff[i].TimeNum = 0;
                   Multi_Udp_Buff[i].DelayTime = 600;  //等待时间  200ms
                   Multi_Udp_Buff[i].SendDelayTime = 0; //发送等待时间计数  20101112
                   //更改UDP端口
                   Multi_Udp_Buff[i].DestPort = LocalPort[0];
                  
                   sprintf(Multi_Udp_Buff[i].RemoteIP, "%d.%d.%d.%d", 
				   		LocalCfg.IP_Server[0], LocalCfg.IP_Server[1], LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
                   //命令, 服务器解析
                   Multi_Udp_Buff[i].buf[6] = NSSERVERORDER;
                   xtm_printf("正在向NS服务器解析地址, %s\n", Multi_Udp_Buff[i].RemoteIP);
                  }
                 break;
    case NSSERVERORDER: //服务器解析
                 Multi_Udp_Buff[i].isValid = 0;
                 xtm_printf("服务器解析失败\n");
                 //监视窗口
                 if(Local.CurrentWindow == 16)
                  {
                   display_text(20, 200, 0, COLOR_YELLOW, "解析失败", 0, 24, NOFULLPAGE);
                   //延时清提示信息标志
                   PicStatBuf.Type = Local.CurrentWindow;
                   PicStatBuf.Time = 0;
                   PicStatBuf.Flag = 1;
                  }
                 break;
    case VIDEOTALK:    //局域网可视对讲
                 xtm_printf("VIDEOTALK\n");
                 switch(Multi_Udp_Buff[i].buf[8])
                  {
                   case JOINGROUP:        //加入组播组（主叫方->被叫方，被叫方应答）
                                          Multi_Udp_Buff[i].isValid = 0;
                                          xtm_printf("加入组播组失败, %d\n", Multi_Udp_Buff[i].buf[6]);
                                          break;
                   case CALL:
                                            //有主副机，清其它的呼叫
                                            for(k=0; k<UDPSENDMAX; k++)
                                             if(Multi_Udp_Buff[k].isValid == 1)
                                              if(Multi_Udp_Buff[k].buf[8] == CALL)
                                               if(k != i)
                                                 Multi_Udp_Buff[k].isValid = 0;

                                            Multi_Udp_Buff[i].isValid = 0;
                                            if(Local.Status == 0)
                                             {
                                              //延时清提示信息标志
                                              PicStatBuf.Type = Local.CurrentWindow;
                                              PicStatBuf.Time = 0;
                                              PicStatBuf.Flag = 1;
                                              if(Local.CurrentWindow == 16)
                                               {
                                                 display_text(20, 200, 0, COLOR_YELLOW, "呼叫失败", 0, 24, NOFULLPAGE);
                                               } 
                                             }
                                          xtm_printf("呼叫失败, %d, %d, %s\n", Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8], Multi_Udp_Buff[i].RemoteIP);
                                          break;
                   case CALLEND:  //通话结束
                   #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
                   case TURNTALK: //转接
                   #endif
                                          Multi_Udp_Buff[i].isValid = 0;
                                          Local.OnlineFlag = 0;
                                          Local.CallConfirmFlag = 0; //设置在线标志
                                          if(Multi_Udp_Buff[i].buf[8] == TURNTALK)
                                            xtm_printf("转接失败, %d, %d, RemoteHost = %s\n", Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8], 
                                                   Multi_Udp_Buff[i].RemoteIP);
                                          //对讲结束，清状态和关闭音视频
                                          TalkEnd_ClearStatus(0);
                                          break;
                   default: //为其它命令，本次通信结束
                                          Multi_Udp_Buff[i].isValid = 0;
                                          xtm_printf("通信失败1, %d, %d, RemoteHost = %s\n", Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8], 
                                                   Multi_Udp_Buff[i].RemoteIP);
                                          break;
                  }
                 break;
    case VIDEOWATCH:     //局域网监控
                           switch(Multi_Udp_Buff[i].buf[8])
                            {
                             case CALL:
                                            Multi_Udp_Buff[i].isValid = 0;
                                          //  Local.Status = 0;
                                            //延时清提示信息标志
                                            PicStatBuf.Type = Local.CurrentWindow;
                                            PicStatBuf.Time = 0;
                                            PicStatBuf.Flag = 1;

                                            display_text(20, 200, 0, COLOR_YELLOW, "监视失败", 0, 24, NOFULLPAGE);
                                            /*if(Local.CurrentWindow == 13)
                                             {
                                               strcpy(Label_Call.Text, WatchFail_Text[Local.LanguageNo]);
                                               ShowOsdLabel24(&Label_Call, REFRESH, cWhite, 0);
                                             }     */
                                            xtm_printf("监视失败, %d, i = %d\n", Multi_Udp_Buff[i].buf[6], i);
                                          break;
                             case CALLEND:  //通话结束
                                          Multi_Udp_Buff[i].isValid = 0;
                                          Local.OnlineFlag = 0;
                                          Local.CallConfirmFlag = 0; //设置在线标志

                                          WatchEnd_ClearStatus();
                                          break;
                             default: //为其它命令，本次通信结束
                                          Multi_Udp_Buff[i].isValid = 0;
                                          xtm_printf("通信失败2, %d\n", Multi_Udp_Buff[i].buf[6]);
                                          break;
                            }
                           break;
    default: //为其它命令，本次通信结束
                           Multi_Udp_Buff[i].isValid = 0;
                      //     Local.Status = 0;
                           if(Multi_Udp_Buff[i].buf[6] != REPORTSTATUS)
                             xtm_printf("通信失败3, %d\n", Multi_Udp_Buff[i].buf[6]);
                           break;
   }  
}
//---------------------------------------------------------------------------
uint8_t ClearMultiUdpBuff(uint8_t Order1, uint8_t Order2, uint8_t Order3, char *cFromIP)   //Clear UDP发送缓存
{
  uint8_t i, clear_ok;

  clear_ok = 0;
  for(i=0; i<UDPSENDMAX; i++){
	  if(Multi_Udp_Buff[i].isValid == 1){
		  if(Multi_Udp_Buff[i].SendNum	< MAXSENDNUM){
			  if(Multi_Udp_Buff[i].buf[6] == Order1){
				  if((Multi_Udp_Buff[i].buf[7] & 0x03) == Order2){
					  if(Multi_Udp_Buff[i].buf[8] == Order3){
						  if(strcmp(Multi_Udp_Buff[i].RemoteIP, cFromIP) == 0)
							{
									 Multi_Udp_Buff[i].isValid = 0;
							  
									 clear_ok = 1;
							}
						}
					}
				}
			}
		}
	}
  return clear_ok;
}
//-----------------------------------------------------------------------
uint16_t UdpSendBuff(int s_socket, char *Ip_Addr, int Port, char *sendbuf, int sendlen)
{
    uint16_t rtnValue = 0;

    struct sockaddr_in clieAddrUDP;
    clieAddrUDP.sin_family=AF_INET;
	clieAddrUDP.sin_port = htons(Port);
    clieAddrUDP.sin_addr.s_addr = inet_addr(Ip_Addr);

    //printf("s_socket = %d, Ip_Addr = %s:%d, sendlen = %d\n", s_socket, Ip_Addr, Port, sendlen);
    if(s_socket >= 0)
      rtnValue = sendto(s_socket,sendbuf,sendlen,0,(struct sockaddr *)&clieAddrUDP,sizeof(clieAddrUDP));
    return rtnValue;
}
//---------------------------------------------------------------------------
//发送远程调试信息
void RecvRemoteDebugInfo_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint16_t sendlength;
  char FlagText[50];
  memcpy(FlagText, recv_buf + 9, 20);

  if((FlagText[0] == FLAGTEXT[0])&&(FlagText[1] == FLAGTEXT[1])&&(FlagText[2] == FLAGTEXT[2])
     &&(FlagText[3] == FLAGTEXT[3])&&(FlagText[4] == FLAGTEXT[4])&&(FlagText[5] == FLAGTEXT[5]))
   {
    //xtm_printf("RecvDownLoadFile_Func::recv_buf[8] = 0x%X\n", recv_buf[8]);
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答
    sendlength = length;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
    switch(recv_buf[8])
     {
      case STOPDEBUG:       //停止
             xtm_printf("收到远程调试信息 停止\n");
             Local.RemoteDebugInfo = 0;      //发送远程调试信息
             break;
      case STARTDEBUG:       //开始
             Local.RemoteDebugInfo = 1;      //发送远程调试信息
             strcpy(Local.DebugIP, cFromIP);
             xtm_printf("收到远程调试信息 开始, %s, Local.DebugIP = %s\n", LocalCfg.Addr, Local.DebugIP);
             break;
      case REMOTE_REBOOT:       //重启动
             xtm_printf("收到远程调试信息 重启动\n");
             //sync();
             //system("reboot");
             break;
     }
   }
}
//---------------------------------------------------------------------------
void send_debug_info(int s_socket, char *debug_info)
{
#if 1
  uint16_t sendlength;
  uint32_t infolen;
  uint8_t RemoteMac[6];
  uint8_t debug_sendb[512];
  //xtm_printf("send_debug_info::Local.RemoteDebugInfo = %d, debug_info = %s\n", Local.RemoteDebugInfo, debug_info);
  if(Local.RemoteDebugInfo == 1)      //发送远程调试信息
   {
    memcpy(debug_sendb, UdpPackageHead, 6);
    debug_sendb[6] = REMOTEDEBUGINFO;
    debug_sendb[7] = ASK;    //主叫
    debug_sendb[8] = DEBUGDATA;    //子命令
    memcpy(debug_sendb + 9, FLAGTEXT, 20);       //标志
    infolen = strlen(debug_info);
    memcpy(debug_sendb + 29, &infolen, 4);
    memcpy(debug_sendb + 33, debug_info, infolen);
    sendlength = 33 + infolen;

    UdpSendBuff(s_socket, Local.DebugIP, RemotePort[0], debug_sendb, sendlength);
   }
#endif  
}
//---------------------------------------------------------------------------
#ifdef _REMOTECALLTEST  //远程呼叫测试
//接收远程呼叫测试　
void RecvRemoteCallTest_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
#if 0
  uint8_t RemoteAddr[20];
  uint8_t FlagText[50];

  xtm_printf("RecvRemoteCallTest_Func\n");
  memcpy(FlagText, recv_buf + 9, 20);
  if((FlagText[0] == FLAGTEXT[0])&&(FlagText[1] == FLAGTEXT[1])&&(FlagText[2] == FLAGTEXT[2])
     &&(FlagText[3] == FLAGTEXT[3])&&(FlagText[4] == FLAGTEXT[4])&&(FlagText[5] == FLAGTEXT[5]))
   {
    if(Local.LcdLightFlag == 0)
     {
      //点亮LCD背光
      Lcd_Power(1, LocalCfg.BrightVolume);
      Local.LcdLightFlag = 1; //LCD背光标志
      Local.LcdLightTime = 0; //时间
     }
        
    memcpy(RemoteAddr, recv_buf + 29 + 1, 19);
    RemoteAddr[19] = '\0';
    xtm_printf("接收远程呼叫测试 %s\n", RemoteAddr);
    if(Local.Status == 0)
     {
      Local.EnterTestMode = 1;  //测试模式, 5秒自动接听;
      Local.CloseTestRing = 1;  //测试模式, 关闭铃声
      switch(recv_buf[8])
       {
        case STARTTEST:
              CallR2R_Func(RemoteAddr);     //呼叫住户
              break;
        case ENTERTESTMODE:

              xtm_printf("RecvRemoteCallTest_Func::Local.EnterTestMode = %d\n", Local.EnterTestMode);
              break;
       }
     }
    else
     {
      switch(recv_buf[8])
       {
        case STOPCALL:
              xtm_printf("RecvRemoteCallTest_Func::STOPCALL\n");
              if(Local.Status != 0)
               {
                 TalkEnd_Func();
               }
              break;
       }
     }     
   }
#endif  
}

#endif
//---------------------------------------------------------------------------
#ifdef ELEVATORCONTROL_HANGZHOU
void RecvElevatorControl_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)  //电梯控制
{
  if((recv_buf[7] & 0x03) == REPLY)   //应答
   if(ClearMultiUdpBuff(ELEVATR_ORDER1, ASK, CALL, cFromIP))
    {
     xtm_printf("收到控制电梯应答\n");
    }
}
#endif
//-----------------------------------------------------------------------
//解析
void RecvNSAsk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)  //解析请求
{
  uint8_t i,j;
  uint8_t isAddrOK;
  char tmp_text[20];
  uint16_t sendlength;
  isAddrOK = 1;
  for(j=8; j<(8+Local.AddrLen); j++)
   if(LocalCfg.Addr[j-8] != recv_buf[j])
   {
    isAddrOK = 0;
    break;
   }
  //xtm_printf("RecvNSAsk_Func 1:: isAddrOK = %d, Local.AddrLen = %d\n", isAddrOK, Local.AddrLen);
  //xtm_printf("RecvNSAsk_Func 2:: %X %X %X %X %X %X %X %X %X %X %X --- %X %X %X %X %X %X %X %X %X %X %X --- %X %X %X %X %X %X %X %X %X %X %X\n", LocalCfg.Addr[0], LocalCfg.Addr[1], LocalCfg.Addr[2], LocalCfg.Addr[3], LocalCfg.Addr[4], LocalCfg.Addr[5], LocalCfg.Addr[6], LocalCfg.Addr[7], LocalCfg.Addr[8], LocalCfg.Addr[9], LocalCfg.Addr[10], \
  //    recv_buf[8], recv_buf[9], recv_buf[10], recv_buf[11], recv_buf[12], recv_buf[13], recv_buf[14], recv_buf[15], recv_buf[16], recv_buf[17], recv_buf[18], \
  //    recv_buf[32], recv_buf[33], recv_buf[34], recv_buf[35], recv_buf[36], recv_buf[37], recv_buf[38], recv_buf[39], recv_buf[40], recv_buf[41], recv_buf[42]);
  //不是本机所发
  if(isAddrOK == 0)
   {
    isAddrOK = 1;

    for(j=32; j<(32+Local.AddrLen); j++)
     if(LocalCfg.Addr[j-32] != recv_buf[j])
      {
       isAddrOK = 0;
       break;
      }
    memcpy(tmp_text, recv_buf + 32, 12);
    tmp_text[12] = '\0';
    xtm_printf("RecvNSAsk_Func 2:: isAddrOK = %d, Local.AddrLen = %d, LocalCfg.Addr = %s, Dest = %s\n", isAddrOK, Local.AddrLen, LocalCfg.Addr, tmp_text);
  //要求解析的是本机地址
    if(isAddrOK == 1)
     {
      memcpy(send_b, recv_buf, 32);
      send_b[7] = Local.MachineType | REPLY;    //应答

      if(Local.isHost == '0')  //主机
       {
        send_b[32] = Local.DenNum + 1;   //地址个数

        memcpy(send_b + 33, LocalCfg.Addr, 20);
        memcpy(send_b + 53, LocalCfg.IP, 4);
        for(i=0; i<Local.DenNum; i++)
         {
          memcpy(send_b + 57 + 24*i, Local.DenAddr[i], 20);
          memcpy(send_b + 57 + 20 +24*i, Local.DenIP[i], 4);
         }
        sendlength = 57 + 24*Local.DenNum;
       }
      else                    //副机
       {
        send_b[32] = 1;   //地址个数

        memcpy(send_b + 33, LocalCfg.Addr, 20);
        memcpy(send_b + 53, LocalCfg.IP, 4);
        sendlength = 57;
       }

      CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
      xtm_printf("Local.isHost = 0x%X, Local.DenNum = %d, cFromIP = %s\n", Local.isHost, Local.DenNum, cFromIP);
     }
  }
}
//---------------------------------------------------------------------------
void RecvNSReply_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)//解析应答
{
  int i,j,k;
  int CurrOrder;
  int isAddrOK;
  int AddrNo;
  int DenAddrLen;
  char Old_Addr[21], New_Addr[21];
  uint16_t sendlength;
  uint8_t Order;
  char Door_Addr[21];
  uint8_t Door_IP[4];
  char RemoteIP[20];

  xtm_printf("RecvNSReply_Func:: %s, recv_buf[32] = %d\n", cFromIP, recv_buf[32]);

  isAddrOK = 0;
  for(i=0; i<UDPSENDMAX; i++)
   if(Multi_Udp_Buff[i].isValid == 1)
     if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
      if((Multi_Udp_Buff[i].buf[6] == NSORDER)||(Multi_Udp_Buff[i].buf[6] == NSSERVERORDER))
       if(((Multi_Udp_Buff[i].buf[7] & 0x03) == ASK)&&(recv_buf[32] > 0))
        {
         //判断要求解析地址是否匹配
         switch(Multi_Udp_Buff[i].buf[32])
          {
           case 'S':
           case 'B':
           case 'H':
                    DenAddrLen = 12;
                    break;
           case 'W':
           case 'Z':
                    DenAddrLen = 5;
                    break;
           case 'M':
                    DenAddrLen = 8;
                    break;
          }
         AddrNo = 0;
         isAddrOK = 0;
         memcpy(Old_Addr, Multi_Udp_Buff[i].buf + 32, DenAddrLen);
         Old_Addr[DenAddrLen] = '\0';
         for(k=0; k<recv_buf[32]; k++)
          {
           memcpy(New_Addr, recv_buf + 32 + 1 + k*24, DenAddrLen);
           New_Addr[DenAddrLen] = '\0';
           if(strcmp(Old_Addr, New_Addr) == 0)
            {
             isAddrOK = 1;
             AddrNo = k;
             break;
            }
          }  
         if(isAddrOK == 1)
          {
           CurrOrder = Multi_Udp_Buff[i].CurrOrder;
           Multi_Udp_Buff[i].isValid = 0;
           Multi_Udp_Buff[i].SendNum = 0;
           break;
          }
        }


  if(isAddrOK == 1)
   { //收到正确的解析回应
       if(CurrOrder == VIDEOTALK) //对讲
        {
           Remote.DenNum = recv_buf[32];
           if(Remote.DenNum > (SUBMAXNUM + 1))
             Remote.DenNum = SUBMAXNUM + 1;
           Remote.LineUseNum = 0;
           if((Remote.DenNum >= 1)&&(Remote.DenNum <= 10))
            {
               for(j=0; j<Remote.DenNum; j++)
                {
                 Remote.IP[j][0] = recv_buf[53+24*j];
                 Remote.IP[j][1] = recv_buf[54+24*j];
                 Remote.IP[j][2] = recv_buf[55+24*j];
                 Remote.IP[j][3] = recv_buf[56+24*j];

                 for(k=0; k<20; k++)
                   Remote.Addr[j][k] = recv_buf[33+24*j+k];
                 Remote.GroupIP[0] = 236;
                 Remote.GroupIP[1] = LocalCfg.IP[1];
                 Remote.GroupIP[2] = LocalCfg.IP[2];
                 Remote.GroupIP[3] = LocalCfg.IP[3];

                 if(LocalCfg.EncodeFrameSize == VIDEO_VGA)
                   Local.VideoType = 0x10;
                 else
                   Local.VideoType = 0x08;
                 CreateTalkBuff(send_b, VIDEOTALK, Local.MachineType | Local.VideoType | ASK, CALL, Remote.Addr[j], Remote.IP[j]);

                 sendlength = 62;
				 sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2], Remote.IP[j][3]);

                 CreateMultiUdpBuff(s_socket, MULTI_SEND, RemoteIP, LocalPort[1], send_b, sendlength, 0);
                 xtm_printf("解析地址成功,正在发起呼叫, Remote.IP = %d.%d.%d.%d, Remote.Addr = %s\n", Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2], Remote.IP[j][3], 
                             Remote.Addr[j]);
                }
              }
        }

       if(CurrOrder == VIDEOWATCH) //监视
        {
           Remote.DenNum = 1;
           Remote.IP[0][0] = recv_buf[53+24*AddrNo];
           Remote.IP[0][1] = recv_buf[54+24*AddrNo];
           Remote.IP[0][2] = recv_buf[55+24*AddrNo];
           Remote.IP[0][3] = recv_buf[56+24*AddrNo];
           for(k=0; k<20; k++)
             Remote.Addr[0][k] = recv_buf[33+24*AddrNo+k];
           Remote.GroupIP[0] = 236;
           Remote.GroupIP[1] = LocalCfg.IP[1];
           Remote.GroupIP[2] = LocalCfg.IP[2];
           Remote.GroupIP[3] = LocalCfg.IP[3];
           Order = Local.MachineType | ASK;     //本机支持的视频编码类型;
           CreateTalkBuff(send_b, VIDEOWATCH, Order, CALL, Remote.Addr[0], Remote.IP[0]);

           sendlength = 62;

          /* Remote.IP[0][0] = 192;
           Remote.IP[0][1] = 168;
           Remote.IP[0][2] = 68;
           Remote.IP[0][3] = 98;  */
		   sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);	
           CreateMultiUdpBuff(s_socket, MULTI_SEND, RemoteIP, LocalPort[1], send_b, sendlength, 0);
           xtm_printf("解析地址成功,正在发起监视 %d.%d.%d.%d\n", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
        }
      #ifdef ELEVATORCONTROL_HANGZHOU      //20120822
       if(CurrOrder == ELEVATR_ORDER1) //电梯控制
        {
                 memcpy(Door_Addr, recv_buf + 33, 20);
                 Door_IP[0] = recv_buf[53];
                 Door_IP[1] = recv_buf[54];
                 Door_IP[2] = recv_buf[55];
                 Door_IP[3] = recv_buf[56];

                 //头部
                 memcpy(send_b, UdpPackageHead, 6);
                 //命令  ,子网广播解析
                 send_b[6] = ELEVATR_ORDER1;
                 send_b[7] = ASK;    //主叫
                 send_b[8] = CALL;

                 memcpy(send_b+9, LocalCfg.Addr, 20);
                 memcpy(send_b+29, LocalCfg.IP, 4);
                 memcpy(send_b+33, Local.Elevator_Addr, 20);
                 memcpy(send_b+53, Local.Elevator_IP, 4);

                 sendlength = 57;
				 sprintf(RemoteIP, "%d.%d.%d.%d", Door_IP[0], Door_IP[1], Door_IP[2], Door_IP[3]);
                 CreateMultiUdpBuff(s_socket, MULTI_SEND, RemoteIP, LocalPort[1], send_b, sendlength, 0);
                 xtm_printf("解析地址成功,正在发起 控制电梯 %d.%d.%d.%d\n", Door_IP[0], Door_IP[1], Door_IP[2], Door_IP[3]);
        }
      #endif        
  }
}
//---------------------------------------------------------------------------
void RecvTalkCall_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //对讲呼叫
{
  uint8_t i, j;
  uint8_t isAddrOK;
  uint8_t sendlength;
  char str[50];
 // char IP_Group[15]; 

  //已经处在结束通话切换界面状态
  if(PicStatBuf.Flag != 0)
   {
    xtm_printf("RecvTalkCall_Func::PicStatBuf.Flag = %d\n", PicStatBuf.Flag);
    return;
   }
  if(Local.KeyTouchNo != 0)
   {
    xtm_printf("RecvTalkCall_Func::Local.KeyTouchNo = %d, 正在处理界面中...\n", Local.KeyTouchNo);
    return;
   }

  for(i=0; i<MAX_STREAM_NUM; i++)
   {
	  memcpy(mjpeg_stream[i], head_640x480, mjpeg_stream_offset);
	  mjpeg_stream_offset = 0x26f;
   }	
  for(j=0; j<30; j++)
   {
    PackExist[0][j] = 0;
    PackExist[1][j] = 0;
   } 
  //查看目标地址是否是本机
  isAddrOK = 1;
  for(j=33; j<(33+Local.AddrLen); j++)
   if(LocalCfg.Addr[j-33] != recv_buf[j])
   {
    //xtm_printf("LocalCfg.Addr[%d] = 0x%X, recv_buf[%d] = 0x%X\n", j-33, LocalCfg.Addr[j-33], j, recv_buf[j]);
    isAddrOK = 0;
    break;
   }
  xtm_printf("RecvTalkCall_Func:: isAddrOK = %d\n", isAddrOK);
  if(isAddrOK == 0)
    return;

  //本机状态为监视
  if((Local.Status == 3)||(Local.Status == 4))
   {
     Local.ForceEndWatch = 1;  //有呼叫时，强制关监视
     WatchEnd_Func();
     return;
   }

  //本机状态为空闲
  if(Local.Status == 0)
   {
    if(Local.PreStatus == 1)  //20130115 预状态，防止同时的操作
      return;
    Local.PreStatus = 1;

    //20150430 挪到前面，以免在显示界面时收到太多的广播导致冲掉缓存
    memcpy(send_b, recv_buf, 62);

    //memcpy(Remote.DenIP, cFromIP, 4);
    //SearchArp_Table(Remote.DenIP, Remote.DenMac);
    //获取对方地址
    memcpy(Remote.Addr[0], recv_buf+9, 20);
    memcpy(Remote.IP[0], recv_buf+29, 4);
    Remote.DenNum = 1;
    xtm_printf("Remote.IP = %d.%d.%d.%d\n",   \
            Remote.IP[0][0],Remote.IP[0][1],Remote.IP[0][2],Remote.IP[0][3]);

    CheckRemoteAddr();  //检查远端地址

 /*   if(recv_buf[57] == 1)
     {
      //查看是否在其它组播组内
      DropMultiGroup(m_VideoSocket, NULL);

      Local.IP_Group[0] = recv_buf[58]; //组播地址
      Local.IP_Group[1] = recv_buf[59];
      Local.IP_Group[2] = recv_buf[60];
      Local.IP_Group[3] = recv_buf[61];
      sprintf(IP_Group, "%d.%d.%d.%d\0",
                     Local.IP_Group[0],Local.IP_Group[1],Local.IP_Group[2],Local.IP_Group[3]);
      AddMultiGroup(m_VideoSocket, IP_Group);
     }         */
    FirstFrameFlag = 1; //第一帧，判断jpeg header
    FirstFrameFinished = 0;
	ResetMjpegHeader();
			
    Local.TimeOut = 0;  
    Local.Status = 2;  //状态为被对讲
    Local.TalkEndStatus = 1;
 	//xtm_printf("RecvTalkCall_Func::3.2.1, mjpeg_stream = 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", 
	// mjpeg_stream[0][0], mjpeg_stream[0][1], mjpeg_stream[0][2], mjpeg_stream[0][3],
	// mjpeg_stream[1][0], mjpeg_stream[1][1], mjpeg_stream[1][2], mjpeg_stream[1][3]);

    //显示对讲窗口
    DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, TALKWINTYPE);

    Local.DecodeFrameSize = VIDEO_VGA;
    xtm_printf("Local.DecodeFrameSize = %d\n", Local.DecodeFrameSize);
 	//xtm_printf("RecvTalkCall_Func::3.2.2, mjpeg_stream = 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", 
	// mjpeg_stream[0][0], mjpeg_stream[0][1], mjpeg_stream[0][2], mjpeg_stream[0][3],
	// mjpeg_stream[1][0], mjpeg_stream[1][1], mjpeg_stream[1][2], mjpeg_stream[1][3]);    
    //开始播放视频
    Local.DecodeVideoWidth = 640;
    Local.DecodeVideoHeight = 480;
    
   // Local.VideoWindowFlag = 0;  //视频窗口打开标志
    if((SCREEN_WIDTH == 800)&&(SCREEN_HEIGHT == 480))
      DisplayVideoWindow(0, 640, 0, 480, REMOTE_VIDEO);
    if((SCREEN_WIDTH == 480)&&(SCREEN_HEIGHT == 272))
      DisplayVideoWindow(0, 480, 0, 272, REMOTE_VIDEO);    

    CreateAudioSend_Stream();      //音视频发送Stream

    Local.RecordPic = 1;   //呼叫留照片
    Local.IFrameCount = 0; //I帧计数
    Local.HavePicRecorded = 0;
    Local.ManualRecordFlag = 0; //手动录制

    TmpRecordsContent.HavePic = 0;  //有无图像标志  0   1 -- 640x480  2 -- 320x240
    strcpy(TmpRecordsContent.RemoteAddr, Remote.Addr[0]);
    TmpRecordsContent.Type = 1;     //       0 -- 主叫　　1 -- 被叫未接听  2 -- 被叫接听
    TmpRecordsContent.Length = 0;       //长度

   #ifdef _REMOTECALLTEST  //远程呼叫测试
    if(Local.CloseTestRing == 0)  //测试模式, 关闭铃声
   #endif
     if(Local.NoBreak == 0)  //免扰状态 1 免扰  0 正常
      {
       StopPlayWavFile();
       StartPlayWav(ring_wav.Faddr, CYCLE_PLAY);
      }
	 xtm_printf("RecvTalkCall_Func:: 3\n");
   #ifdef _REMOTECALLTEST  //远程呼叫测试
    Local.CloseTestRing = 0;  //测试模式, 关闭铃声
   #endif

    Local.CallConfirmFlag = 1; //设置在线标志
    Local.Timer1Num = 0;
    Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听
    Local.OnlineNum = 0;     //在线确认序号
    Local.OnlineFlag = 1;

    strcpy(str, Remote.Addr[0]);
    if(Remote.Addr[0][0] == 'B')
     {
      str[5] = Remote.Addr[0][11];
      str[6] = '\0';
     }

    strcat(str, "  直通对讲呼叫");
    xtm_printf("%s\n", str);

    //完成处理后再给应答
    //memcpy(send_b, recv_buf, 62);
    send_b[7] = Local.MachineType | ASK;    //主叫
    send_b[8] = CALLANSWER;//对讲应答
    sendlength = 62;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

    //time(&t);
    //Local.call_tm_t = localtime(&t);
    memcpy(Local.RemoteAddr, Remote.Addr[0], 20);
    Local.RemoteAddr[12] = '\0';    		
   }
  //本机为忙
  else
   {
    if((Remote.IP[0][0] != cFromIP[0])||(Remote.IP[0][1] != cFromIP[1])||(Remote.IP[0][2] != cFromIP[2])||(Remote.IP[0][3] != cFromIP[3]))
     {
      memcpy(send_b, recv_buf, 57);
      send_b[7]=Local.MachineType | ASK;    //主叫
      send_b[8]=LINEUSE;//占线应答
      sendlength=57;
      CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
      xtm_printf("发送占线应答\m");
     } 

    xtm_printf("对方发起对讲呼叫, 本机占线, %s\n", cFromIP);
   }
}
//---------------------------------------------------------------------------
void CheckRemoteAddr(void)  //检查远端地址
{
    switch(Remote.Addr[0][0])
     {
      case 'Z':  //中心机呼叫
      case 'W':  //围墙机呼叫
               Remote.Addr[0][5] = '\0';
               break;
      case 'M':  //单元门口机呼叫
               Remote.Addr[0][8] = '\0';
               break;
      case 'S':  //室内机呼叫
      case 'B':  //别墅型室内机呼叫
      case 'H':  //别墅门口机呼叫
               Remote.Addr[0][12] = '\0';
               break;
     }
}
//-----------------------------------------------------------------------
void RecvTalkCallAnswer_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //对讲呼叫应答
{
  uint8_t j;
  if((recv_buf[7] & 0x03) == ASK)   //应答
   if(ClearMultiUdpBuff(VIDEOTALK, ASK, CALL, cFromIP))
    {
            xtm_printf("Remote.DenNum = %d\n",Remote.DenNum);
              if((LocalCfg.Addr[0] == 'Z')
                 ||(Remote.DenNum == 1))
               {
                //memcpy(Remote.DenIP, cFromIP, 4);
                //SearchArp_Table(Remote.DenIP, Remote.DenMac);
               }
              else
               if(Remote.DenNum > 1)
                {
                /* Remote.DenIP[0] = Remote.GroupIP[0];
                 Remote.DenIP[1] = Remote.GroupIP[1];
                 Remote.DenIP[2] = Remote.GroupIP[2];
                 Remote.DenIP[3] = Remote.GroupIP[3]; */
                }

               for(j=0; j<Remote.DenNum; j++)
                {
                 if((Remote.IP[j][0] == cFromIP[0])&&(Remote.IP[j][1] == cFromIP[1])&&(Remote.IP[j][2] == cFromIP[2])&&(Remote.IP[j][3] == cFromIP[3]))
                  {
                   break;
                  } 
                }
            xtm_printf("收到对讲呼叫应答, %s\n", cFromIP);
            if(Local.Status == 1)    //状态为主叫对讲
              return;                
                
            if(Local.Status == 0)
              Local.Status = 1;  //状态为主叫对讲
            Local.TalkEndStatus = 1;
            Local.CallConfirmFlag = 1; //设置在线标志
            Local.Timer1Num = 0;
            Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听
            Local.OnlineNum = 0;     //在线确认序号
            Local.OnlineFlag = 1;
            xtm_printf("Local.Status = %d\n", Local.Status);

            LocalCfg.EncodeFrameSize = VIDEO_VGA; //编码大小

            //开始录制视频
            //StartRecVideo(LocalCfg.EncodeFrameSize);
                        
            CreateAudioSend_Stream();      //音视频发送Stream


            StopPlayWavFile();
            WaitAudioUnuse(200); //等待声卡空闲			
            StartPlayWav(backring_wav.Faddr, CYCLE_PLAY);
           }   
}
//-----------------------------------------------------------------------
void RecvTalkLineUse_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //对讲占线应答
{
  int isUse;
  int CallNextCenter = 0;
  char CenterAddr[20];


  if((recv_buf[7] & 0x03) == ASK)   //应答
   if(ClearMultiUdpBuff(VIDEOTALK, ASK, CALL, cFromIP))
    {
            isUse = 1;
            #ifdef _MULTI_CENTER_SUPPORT  //多中心机支持
             if(Remote.Addr[0][0] == 'Z')
              {
               xtm_printf("中心 %s 占线\n", Remote.Addr[0]);

               Remote.Addr[0][4] ++;
               if(Remote.Addr[0][4] <= '9')
                {
                 strcpy(CenterAddr, Remote.Addr[0]);
                 isUse = 0;
                 CallNextCenter = 1;
                }
              }
            #endif
            Remote.LineUseNum ++;
            if(isUse == 1)
             //20101202 xu 此处会引起，主叫呼叫多分机，振铃过程中，其它分机呼叫无法退出, 增加　占线数量
             if(Remote.DenNum == Remote.LineUseNum)
              {
              /* if(Local.CurrentWindow == 16)
                {
                 strcpy(Label_Call.Text, InUse_Text[Local.LanguageNo]);
                 ShowOsdLabel24(&Label_Call, REFRESH, cWhite, 0);
                }         */
               display_text(20, 200, 0, COLOR_YELLOW, "对方正忙", 0, 24, NOFULLPAGE); 
               //延时清提示信息标志
               PicStatBuf.Type = 16;
               PicStatBuf.Time = 0;
               PicStatBuf.Flag = 1;
              }
            xtm_printf("收到对讲占线应答, %s\n", cFromIP);
    }

  #ifdef _MULTI_CENTER_SUPPORT  //多中心机支持
   if(CallNextCenter == 1)
    {
     xtm_printf("自动呼叫 %s\n", Remote.Addr[0]);

     CallCenter_Func(CenterAddr);
    }
  #endif
}
//-----------------------------------------------------------------------
void RecvTalkCallConfirm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort) //对讲在线确认
{
  uint8_t sendlength;
  //本机为主叫方
  if(((Local.Status == 1)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
     &&((recv_buf[7] & 0x03) == ASK))
   {
    memcpy(send_b, recv_buf, 61);
    send_b[7] = Local.MachineType | REPLY;    //应答
    sendlength = 61;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
    Local.CallConfirmFlag = 1;

    //xtm_printf("收到对讲在线确认\n");

    //如已在通话中，但收到不为通话方的在线确认，则发退出组播组命令
    #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
     if(Local.Status == 5)
       if((Remote.IP[0][0] != recv_buf[53])||(Remote.IP[0][1] != recv_buf[54])||(Remote.IP[0][2] != recv_buf[55])||(Remote.IP[0][3] != recv_buf[56]))
         SingleExitGroup(s_socket, recv_buf); //向单个被叫方退出组播组命令
    #endif
   }
  else  //本机为被叫方
   if(((Local.Status == 2)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
      &&((recv_buf[7] & 0x03) == REPLY))
   {
    Local.CallConfirmFlag = 1;

    //xtm_printf("收到对方应答本机对讲在线确认\n");
   }
}
//-----------------------------------------------------------------------
void RecvTalkCallStart_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //对讲开始通话
{
  uint8_t isAddrOK;
  uint8_t sendlength;

  Local.TalkTimeOut = TALKTIMEOUT; //通话最长时间
  //本机为主叫方 应答
  if((Local.Status == 1)&&((recv_buf[7] & 0x03) == ASK))
   {
    memcpy(send_b, recv_buf, 57);
    send_b[7]=Local.MachineType | REPLY;    //应答
    sendlength=57;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

    #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
     ExitGroup(s_socket, recv_buf);      //向其它被叫方退出组播组命令
    #endif

    //获取被叫方地址
    memcpy(Remote.Addr[0], recv_buf+33, 20);
    memcpy(Remote.IP[0], recv_buf+53, 4);
    Remote.DenNum = 1;
    xtm_printf("RecvTalkCallStart_Func::Remote.Addr[0] = %s, Remote.IP[0] = %d.%d.%d.%d\n", Remote.Addr[0], Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);

    //memcpy(Remote.DenIP, cFromIP, 4);

    StopPlayWavFile();  //关闭铃音
    WaitAudioUnuse(1000); //延时是为了等待声音播放完成

    //打开音频录制、播放，视频播放
    //#ifdef _USBCAMERA_SUPPORT     //USB Camera支持
    //  StartPlayVideo(CIF_W, CIF_H, WINDOWMODE);
    //#endif
    
   // Local.VideoWindowFlag = 0;  //视频窗口打开标志
    //开始播放视频
    if(Local.DecodeFrameSize == VIDEO_VGA) //解码大小
     {
      Local.DecodeVideoWidth = 640;
      Local.DecodeVideoHeight = 480;
      DisplayVideoWindow(0, 640, 0, 480, REMOTE_VIDEO);
     }
    if(Local.DecodeFrameSize == VIDEO_QVGA) //解码大小
     {
      Local.DecodeVideoWidth = 320;
      Local.DecodeVideoHeight = 240;
      DisplayVideoWindow(160, 320, 120, 240, REMOTE_VIDEO);
     }

    //SoundSetSpeakerDac(0xc0);
    StartRecAudio();
    AudioStartFlag = 1;
    AudioStartTime = 0;
	AudioTalkOpenFlag = 1;
	SetPlayAudioVariable();	
    //StartPlayAudio();
    //SoundSetSpeakerDac(0xc0);

    Local.Status = 5;  //状态为主叫通话
    Local.TalkEndStatus = 1;
    Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听

    xtm_printf("对方开始通话, %s\n", cFromIP);
   }
  else  //本机为被叫方 发起
   if(Local.Status == 2)
   {
    //xtm_printf("RecvTalkCallStart_Func::对方应答本机开始通话, %s\n", cFromIP);
    //本机主动
    if((recv_buf[7] & 0x03) == REPLY)
     if(ClearMultiUdpBuff(VIDEOTALK, ASK, CALLSTART, cFromIP))
      {
              //查看是否在其它组播组内
              //DropMultiGroup(m_VideoSocket, NULL);

			  StopPlayWavFile();  //关闭铃音
printf("RecvTalkCallStart_Func::1\n");
			  WaitAudioUnuse(1000); //延时是为了等待声音播放完成

              //打开音频录制、播放，视频录制
              if((Remote.Addr[0][0] == 'S')||(Remote.Addr[0][0] == 'B')||(Remote.Addr[0][0] == 'Z'))
               {
                //StartRecVideo(LocalCfg.EncodeFrameSize);
               }
printf("RecvTalkCallStart_Func::2\n"); 

              //SoundSetSpeakerDac(0xc0);
              StartRecAudio();
              AudioStartFlag = 1;
              AudioStartTime = 0;
			  AudioTalkOpenFlag = 1;
			  SetPlayAudioVariable();
              //StartPlayAudio();
              //SoundSetSpeakerDac(0xc0);
            
              Local.Status = 6;  //状态为被叫通话
              Local.TalkEndStatus = 1;
              Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听
              CheckRemoteAddr();  //检查远端地址
              Local.RecordPic = 2;   //通话留照片
              Local.IFrameCount = 0; //I帧计数
              Local.HavePicRecorded = 0;
              Local.ManualRecordFlag = 0; //手动录制
printf("RecvTalkCallStart_Func::3\n");

              TmpRecordsContent.Type = 2;     //       0 -- 主叫　　1 -- 被叫未接听  2 -- 被叫接听

              xtm_printf("对方应答本机开始通话, %s\n", cFromIP);

             }
   }

  if((Local.Status == 5)||(Local.Status == 6))
   {
   }     
}

//-----------------------------------------------------------------------

void RecvTalkCallEnd_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //对讲呼叫结束
{
  uint8_t isAddrOK;
  uint8_t sendlength;
  //本机被动
  if(((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6) \
     ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10)) \
     &&((recv_buf[7] & 0x03) == ASK))
   {
    isAddrOK = 0;

    //本机为主叫方
    if((Local.Status == 1)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
      if((recv_buf[53] == Remote.IP[0][0]) && (recv_buf[54] == Remote.IP[0][1]) && (recv_buf[55] == Remote.IP[0][2]) &&(recv_buf[56] == Remote.IP[0][3]))
        isAddrOK = 1;

    //本机为被叫方
    if((Local.Status == 2)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
      if((recv_buf[29] == Remote.IP[0][0]) && (recv_buf[30] == Remote.IP[0][1]) && (recv_buf[31] == Remote.IP[0][2]) &&(recv_buf[32] == Remote.IP[0][3]))
        isAddrOK = 1;

     if(isAddrOK == 1)
      {
       Local.OnlineFlag = 0;
       Local.CallConfirmFlag = 0; //设置在线标志

       memcpy(send_b, recv_buf, 57);
       send_b[7]=Local.MachineType | REPLY;    //应答
       sendlength=57;

       //UdpSendBuff(s_socket, cFromIP, DestPort, (char *)send_b, sendlength);
       CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, VIDEOTALK);

       #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
        ExitGroup(s_socket, recv_buf);      //向其它被叫方退出组播组命令
       #endif
       TalkEnd_ClearStatus(0);

       xtm_printf("对方结束对讲, %s:%d, s_socket = %d, Remote.DenNum = %d\n", cFromIP, DestPort, s_socket, Remote.DenNum);
      }

   }
  else  //本机主动
   if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
     ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))   
   {
    Local.OnlineFlag = 0;
    Local.CallConfirmFlag = 0; //设置在线标志

    //本机主动
    if((recv_buf[7]& 0x03) == REPLY)
     if(ClearMultiUdpBuff(VIDEOTALK, ASK, CALLEND, cFromIP))
      {
              TalkEnd_ClearStatus(0);

              if(Local.CurrentWindow == 16)
               {
               }
              xtm_printf("对方应答本机结束对讲\n");
      }
   }
}

//-----------------------------------------------------------------------
void TalkEnd_ClearStatus(int isTurn) //对讲结束，清状态和关闭音视频
{
  uint8_t buf[10];
  if(isTurn == 1)
   {
    Local.Status = 0;  //状态为空闲

    StopRecAudio();
    StopPlayAudio();
    return;
   }

  if(Local.TalkEndStatus == 1)
   {
    Local.TalkEndStatus = 0;
    Local.OnlineFlag = 0;
    Local.CallConfirmFlag = 0; //设置在线标志
    xtm_printf("TalkEnd_ClearStatus Local.Status=%d\n", Local.Status);

    //查看是否在其它组播组内
    //DropMultiGroup(m_VideoSocket, NULL);
    switch(Remote.Addr[0][0])
     {
      case 'Z':  //中心机呼叫
      case 'W':  //围墙机呼叫
               Remote.Addr[0][5] = '\0';
               break;
      case 'H':  //别墅门口机呼叫
               Remote.Addr[0][6] = '\0';
               break;
      case 'M':  //单元门口机呼叫
               Remote.Addr[0][8] = '\0';
                        break;
      case 'S':  //室内机呼叫
      case 'B':  //别墅型室内机呼叫      
               Remote.Addr[0][12] = '\0';
               break;
     }
#if 1
    StopPlayWavFile();  //关闭铃音
    AudioTalkOpenFlag = 0;
    switch(Local.Status)
     {
      case 1: //本机主叫
             /*WaitAudioUnuse(1000); //延时是为了等待声音播放完成，否则会有杂音
             StopPlayVideo();     */
             //StopRecVideo();
             break;
      case 2: //本机被叫
             //写照片函数
             WritePicFunc();
             break;
      case 5: //本机主叫通话

             //StopRecVideo();

             StopRecAudio();
             StopPlayAudio();
             break;
      case 6: //本机被叫通话

            // StopRecVideo();
             
             StopRecAudio();


             StopPlayAudio();



             //写照片函数
             WritePicFunc();

             break;
     }

    if(Local.CurrentWindow != 16)  //在其它窗口
      Local.CurrentWindow = 16;
    CloseVideoWindow();
    //延时清提示信息标志
    PicStatBuf.Type = Local.CurrentWindow;
    PicStatBuf.Time = 0;
    PicStatBuf.Flag = 1;

//    SoundSetSpeakerVolume(LocalCfg.SpkVolume); //设置音量
#endif
    //Local.Status = 0;
   }
  else
    xtm_printf("TalkEnd_ClearStatus 重复\n"); 
}
//-----------------------------------------------------------------------
void RecvTalkRemoteOpenLock_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //远程开锁
{
  if((Local.Status == 2)||(Local.Status == 6))  //状态为被对讲 或被叫通话
  //if(Local.Status == 6)  //状态为被对讲
   {
    //本机主动
    if((recv_buf[7]& 0x03) == REPLY)
     if(ClearMultiUdpBuff(VIDEOTALK, ASK, REMOTEOPENLOCK, cFromIP))
      {
              #ifdef _ZHUHAIJINZHEN      //珠海进祯  呼叫时可开锁  通话时开锁2秒后挂断
              if(Local.Status == 6)  //状态为被叫通话
               {
                if((Local.TimeOut + 2*(1000/INTRTIME)) < TALKTIMEOUT)
                  Local.TalkTimeOut = Local.TimeOut + 2*(1000/INTRTIME);
                else
                  Local.TalkTimeOut = TALKTIMEOUT;
               }
              #endif
			  Local.SendLockFlag = 1;    //20180608 通话过程中开锁
              xtm_printf("对方应答本机远程开锁\n");
      }     
   }
}
//-----------------------------------------------------------------------
void RecvTalkCenterTurnTalk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //中心人工转接
{
  uint8_t j;
  uint8_t isAddrOK;
  uint8_t sendlength;
  uint8_t DestAddr[21], DestAddr1[21];

  //查看目标地址是否是本机
  isAddrOK = 1;
  for(j=33; j<33+Local.AddrLen; j++)
   if(LocalCfg.Addr[j-33] != recv_buf[j])
   {
    isAddrOK = 0;
    break;
   }
  xtm_printf("RecvTalkCenterTurnTalk_Func:: isAddrOK = %d\n", isAddrOK);
  if(isAddrOK == 0)
    return;

  if((Local.Status == 5)||(Local.Status == 6))
   {
    //完成处理后再给应答
    memcpy(send_b, recv_buf, 81);
    send_b[7] = Local.MachineType | REPLY;    //主叫
    sendlength = 81;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

    TalkEnd_ClearStatus(1);            //本机为主叫方，不关对讲，只关音频 、视频播放

    memcpy(DestAddr, recv_buf + 57, 20);
    if(DestAddr[0] == 'B')
     {
      memcpy(DestAddr1, DestAddr + 1, 4);
      DestAddr1[4] = '\0';
     }
    else
     {
      memcpy(DestAddr1, DestAddr + 1, 10);
      DestAddr1[10] = '\0';
     }
    CallR2R_Func(DestAddr1);
   }  
}
//-----------------------------------------------------------------------
void RecvTalkTrusteeship_Talk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //托管
{
  uint16_t sendlength;
  char RemoteIP[20];
  if((recv_buf[7] & 0x03) == ASK)   //应答
   if(ClearMultiUdpBuff(VIDEOTALK, ASK, CALL, cFromIP))
    {
      memcpy(Remote.Addr[0], recv_buf + 33, 20);
      xtm_printf("RecvTalkTrusteeship_Talk_Func::Remote.Addr[0] = %s\n", Remote.Addr[0]);
      if(Local.Status == 0)
       {
         CreateNSBuff(send_b, NSORDER, ASK);

         sendlength = 56;
		 sprintf(RemoteIP, "%d.%d.%d.%d", Local.NsMultiAddr[0], Local.NsMultiAddr[1], Local.NsMultiAddr[2], Local.NsMultiAddr[3]);

         CreateMultiUdpBuff(s_socket, MULTI_SEND, RemoteIP, LocalPort[1], send_b, sendlength, VIDEOTALK);
       }
      else
       {
        xtm_printf("托管, 本机正忙,无法呼叫\n");
       }
    }  
}
//-----------------------------------------------------------------------
void RecvTalkCallUpDown_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)  //对讲数据
{
  //已经处在结束通话切换界面状态
  if(PicStatBuf.Flag != 0)
   {
    //xtm_printf("RecvTalkCallUpDown_Func::PicStatBuf.Flag = %d\n", PicStatBuf.Flag);
    return;
   }
  //xtm_printf("recv_buf[6] = %d, recv_buf[7] = %d, recv_buf[8] = %d, recv_buf[61] = %d\n", recv_buf[6], recv_buf[7], recv_buf[8], recv_buf[61]);
  if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
     ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))  //状态为对讲
   switch(recv_buf[61])
   {
    case 1://音频
           //if((cFromIP[0] != Remote.IP[0][0])||(cFromIP[1] != Remote.IP[0][1])||(cFromIP[2] != Remote.IP[0][2])||(cFromIP[3] != Remote.IP[0][3]))
           // {
           //  break;
           // }    
           AudioDeal_Func(s_socket, recv_buf, cFromIP, length);  //音频数据处理
           break;
    case 2://视频  I帧  352*288
    case 3://视频  P帧  352*288
    case 4://视频  I帧  720*480
    case 5://视频  P帧  720*480
           VideoDeal_Func(s_socket, recv_buf, cFromIP, length);
           break;
  }
}
//-----------------------------------------------------------------------
//监视
void RecvWatchCallAnswer_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //监视呼叫应答
{
  uint8_t i;
  if((recv_buf[7] & 0x03) == ASK)   //应答
   if(ClearMultiUdpBuff(VIDEOWATCH, ASK, CALL, cFromIP))
    {
      //memcpy(Remote.DenIP, cFromIP, 4);
 
      FirstFrameFlag = 1; //第一帧，判断jpeg header
      FirstFrameFinished = 0;
	  ResetMjpegHeader();	  

      Local.DecodeFrameSize = VIDEO_VGA;

      //开始播放视频
      Local.DecodeVideoWidth = 640;
      Local.DecodeVideoHeight = 480;

    //  Local.VideoWindowFlag = 0;  //视频窗口打开标志
      if((SCREEN_WIDTH == 800)&&(SCREEN_HEIGHT == 480))
        DisplayVideoWindow(0, 640, 0, 480, REMOTE_VIDEO);
      if((SCREEN_WIDTH == 480)&&(SCREEN_HEIGHT == 272))
        DisplayVideoWindow(0, 480, 0, 272, REMOTE_VIDEO);

  #if 0 //test QVGA
    Local.DecodeFrameSize = VIDEO_QVGA;
    Local.DecodeVideoWidth = 320;
    Local.DecodeVideoHeight = 240;
     DisplayVideoWindow(0, 320, 0, 240, REMOTE_VIDEO);
  #endif

       Local.Status = 3;  //状态为监视
       Local.CallConfirmFlag = 1; //设置在线标志
       Local.Timer1Num = 0;
       Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听
       Local.OnlineNum = 0;     //在线确认序号
       Local.OnlineFlag = 1;
       xtm_printf("Local.Status = %d\n", Local.Status);

       xtm_printf("收到监视呼叫应答\n");
    }
}
//-----------------------------------------------------------------------
void RecvWatchLineUse_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //监视占线应答
{
  if((recv_buf[7] & 0x03) == ASK)   //应答
   if(ClearMultiUdpBuff(VIDEOWATCH, ASK, CALL, cFromIP))
    {
     //延时清提示信息标志
     PicStatBuf.Type = 16;
     PicStatBuf.Time = 0;
     PicStatBuf.Flag = 1;

     display_text(20, 200, 0, COLOR_YELLOW, "对方正忙", 0, 24, NOFULLPAGE);
     xtm_printf("收到监视占线应答, %s\n", cFromIP);
    }   
}
//-----------------------------------------------------------------------
void RecvWatchCallConfirm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort) //监视在线确认
{
  uint8_t sendlength;
  //本机为主叫方
  if(((Local.Status == 3)||(Local.Status == 4))&&((recv_buf[7] & 0x03) == ASK))
   {
    memcpy(send_b, recv_buf, 61);
    send_b[7] = Local.MachineType | REPLY;    //应答
    sendlength = 61;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
    Local.CallConfirmFlag = 1;

    //xtm_printf("收到监视在线确认\n");

   }
  else  //本机为被叫方
   if(Local.Status == 3)
    {
     Local.CallConfirmFlag = 1;

     //xtm_printf("收到对方应答本机监视在线确认\n");
    }
}
//-----------------------------------------------------------------------
void RecvWatchCallEnd_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //监视呼叫结束
{
  uint8_t sendlength;

  //本机被动
  if(((Local.Status == 3)||(Local.Status == 4))&&((recv_buf[7] & 0x03) == ASK))
   {
       Local.OnlineFlag = 0;
       Local.CallConfirmFlag = 0; //设置在线标志

       memcpy(send_b, recv_buf, 57);
       send_b[7]=Local.MachineType | REPLY;    //应答
       sendlength=57;
       CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

       WatchEnd_ClearStatus();

       xtm_printf("对方结束监视, %s\n", cFromIP);
   }
  else  //本机主动
   if(Local.Status == 3)
   {
    Local.OnlineFlag = 0;
    Local.CallConfirmFlag = 0; //设置在线标志
    //本机主动
    if((recv_buf[7]& 0x03) == REPLY)
     if(ClearMultiUdpBuff(VIDEOWATCH, ASK, CALLEND, cFromIP))
      {
              WatchEnd_ClearStatus();

              xtm_printf("对方应答本机结束监视\n");
      }
   }
}
//-----------------------------------------------------------------------
void RecvWatchCallUpDown_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)  //监视数据
{
  if((Local.Status == 3)||(Local.Status == 4))  //状态为监视
   switch(recv_buf[61])
   {
    case 2://视频  I帧  352*288
    case 3://视频  P帧  352*288
    case 4://视频  I帧  720*480
    case 5://视频  P帧  720*480
           VideoDeal_Func(s_socket, recv_buf, cFromIP, length);
           break;
  }
}
//-----------------------------------------------------------------------
//监视结束，清状态和关闭音视频
void WatchEnd_ClearStatus(void)
{
    switch(Local.Status)
     {
      case 3: //本机监视
             if(Local.ForceEndWatch == 0)  //有呼叫时，强制关监视
              {
               //延时清提示信息标志
               PicStatBuf.Type = 16;
               PicStatBuf.Time = 0;
               PicStatBuf.Flag = 1;
              }
             else
              {
              // ShowFirstWindow();
               Local.Status = 0;  //状态为空闲
               Local.ForceEndWatch = 0;  //有呼叫时，强制关监视
               Local.PreStatus = 0;
              }
             CloseVideoWindow();
             break;
      case 4: //本机被监视
             Local.Status = 0;  //状态为空闲
             break;
     }
    if(Local.CurrentWindow == 13)
     {
      //延时清提示信息标志
      PicStatBuf.Type = 13;
      PicStatBuf.Time = 0;
      PicStatBuf.Flag = 1;
     }
}
//-----------------------------------------------------------------------
uint16_t old_audio_Frameno = 0;

void AudioDeal_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t length)  //音频数据处理
{
#if 1
  uint32_t Maddr1;
  uint8_t Stream_Pos;
  uint16_t talk_Datalen, talk_Frameno;

  if((Local.Status == 5)||(Local.Status == 6))
  //if(1)
   {
    if(AudioStartFlag == 1)
     {
      AudioStartTime ++;
      if(AudioStartTime > 3)
       {
        AudioStartFlag = 0;
        AudioStartTime = 0;
        RestoreSpeakerDac();
       }
     }
        
    //帧序号
//    memcpy(&talkdata, recv_buf + 9, sizeof(talkdata));
    talk_Frameno = recv_buf[64];
    talk_Frameno <<= 8;
    talk_Frameno += recv_buf[63];
	if((old_audio_Frameno + 1) != talk_Frameno)
		printf("audio::%d, %d\n", old_audio_Frameno, talk_Frameno);
	old_audio_Frameno = talk_Frameno;

    talk_Datalen = recv_buf[74];
    talk_Datalen <<= 8;
    talk_Datalen += recv_buf[73];

    if(talk_Datalen != AUDIOBLK_8/2)
     {
      xtm_printf("talk_Datalen = %d\n", talk_Datalen);
      return;
     }
 #if 1
    G711Decoder(PcmO_Stream + PcmO_iPut, recv_buf + 86, talk_Datalen, 0);

    //printf("talk_Frameno = %d\n", talk_Frameno);
    //if((talk_Frameno > 1000)&&(talk_Frameno <1050))
	//	return;
    PcmO_iPut += (talk_Datalen*2);
    if(PcmO_iPut >= PCM_BUF_MAX)
      PcmO_iPut = 0;

   #if 0
    PcmO_Len ++;
   #else
    if(PcmO_iPut >= PcmO_iGet)
      PcmO_Len = (PcmO_iPut - PcmO_iGet);
    else
      PcmO_Len = (PcmO_iPut + PCM_BUF_MAX - PcmO_iGet);
   #endif
   if(AudioTalkOpenFlag){
   		if(PcmO_Len >= 4*(talk_Datalen*2))
		{
   			StartPlayAudio();
   			AudioTalkOpenFlag = 0;
  		}
	}
   else{
   	    //printf("1\n");
   		sendAudioRecvMessage();
		//if(PcmO_Len > 2*(talk_Datalen*2))
		//	sendAudioRecvMessage();
   }
   if(PcmO_Len > 128*10){
   	  printf("%d\n", PcmO_Len);
	  //sendAudioRecvMessage();
   	}

         
 //   xtm_printf("PcmO_iPut = 0x%X, PcmO_iGet = 0x%X, PcmO_Len = 0x%X\n", PcmO_iPut, PcmO_iGet, PcmO_Len);
  //  CheckPlayAudioBuf();
 #endif
   }
#endif  
}
//-----------------------------------------------------------------------
void VideoDeal_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t length)  //视频数据处理
{
#if 1
  uint16_t j;
  uint32_t temp_addr, Maddr1;
  uint8_t PackOK, FramenoOK, LostPackNo[20], LostPackNum, val;
  uint16_t OldFrameno;

  uint16_t talk_Frameno, talk_CurrPackage, talk_TotalPackage, talk_PackLen, talk_Datalen, talk_Framelen;
  uint8_t *Video_Maddr;
  uint32_t Video_Len;
  if((Local.Status == 2)||(Local.Status == 3)||(Local.Status == 5)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
  //if(1)
   {
    //帧序号
    //memcpy(&talkdata, recv_buf + 9, sizeof(talkdata));
    talk_Frameno = recv_buf[64];
    talk_Frameno <<= 8;
    talk_Frameno += recv_buf[63];

    talk_TotalPackage = recv_buf[70];
    talk_TotalPackage <<= 8;
    talk_TotalPackage += recv_buf[69];

    talk_CurrPackage = recv_buf[72];
    talk_CurrPackage <<= 8;
    talk_CurrPackage += recv_buf[71];

    talk_Datalen = recv_buf[74];
    talk_Datalen <<= 8;
    talk_Datalen += recv_buf[73];

    talk_PackLen = recv_buf[76];
    talk_PackLen <<= 8;
    talk_PackLen += recv_buf[75];

    talk_Framelen = recv_buf[66];
    talk_Framelen <<= 8;
    talk_Framelen += recv_buf[65];

  	if(talk_CurrPackage == 1)
   	 {
		if(FirstFrameFlag == 1) //第一帧，判断jpeg header
		{
		  FirstFrameFlag = 0;
		  if((recv_buf[86] == 0xff)&&(recv_buf[87] = 0xd8))
		  	mjpeg_stream_offset = 0;
		  xtm_printf("VideoDeal_Func::recv_buf = 0x%X, 0x%X, talk_PackLen = %d, talk_Datalen = %d\n", recv_buf[86], recv_buf[87], talk_PackLen, talk_Datalen);
		  FirstFrameFinished = 1;
		}
   	 } 	
	//xtm_printf("VideoDeal_Func::FirstFrameFlag = %d, FirstFrameFinished = %d\n", FirstFrameFlag, FirstFrameFinished);
	if(FirstFrameFinished == 0)
		return;
	//xtm_printf("VideoDeal_Func::3.1, mjpeg_stream = 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", 
	//	mjpeg_stream[0][0], mjpeg_stream[0][1], mjpeg_stream[0][2], mjpeg_stream[0][3],
	//	mjpeg_stream[1][0], mjpeg_stream[1][1], mjpeg_stream[1][2], mjpeg_stream[1][3]);	

#if 1
    if(Curr_Frameno != talk_Frameno){
		//xtm_printf("%d----%d\n", talk_Framelen, talk_TotalPackage);
     if(talk_CurrPackage != 1)
      {
        xtm_printf("PackOK 1 ....11111111111............ = %d, talkdata.Frameno = %d, talkdata.CurrPackage = %d, talkdata.TotalPackage = %d, Curr_Frameno = %d, talk_Frameno = %d\n", \
                PackOK, talk_Frameno, talk_CurrPackage, talk_TotalPackage, Curr_Frameno, talk_Frameno);
      }
    } 
    if(Curr_Frameno > talk_Frameno)
        xtm_printf("talkdata.Frameno = %d, Curr_Frameno = %d\n", \
                talk_Frameno, Curr_Frameno);
    //接收到新的一帧，此处清一下，避免混乱
    if(Curr_Frameno != talk_Frameno)
//    if(talkdata.CurrPackage == 1)
     {
      //xtm_printf("PackOK 1 ................ = %d, talkdata.Frameno = %d\n", PackOK, talkdata.Frameno);
      for(j=0; j<MAX_VIDEO_PACK; j++)
       {
        PackExist[0][j] = 0;
        PackExist[1][j] = 0;
        PackFrameno[0][j] = 0;
        PackFrameno[1][j] = 0;
       }
     }
#if 1
    if(talk_Datalen > UDPPACKMAX)
     {
      xtm_printf("VideoDeal_Func::talk_Datalen = 0x%X\n", talk_Datalen);
      return;
     }    
    memcpy(mjpeg_stream[Curr_Mjpeg_Stream] + mjpeg_stream_offset + (talk_CurrPackage - 1)* talk_PackLen, recv_buf + 86, talk_Datalen);
#endif
    Curr_Frameno = talk_Frameno;

    PackFrameno[Curr_Mjpeg_Stream][talk_CurrPackage-1] = talk_Frameno;
    PackExist[Curr_Mjpeg_Stream][talk_CurrPackage-1] = 1;
	
	//xtm_printf("%d, %d\n", talk_TotalPackage, talk_CurrPackage);
    if(talk_TotalPackage == talk_CurrPackage)
     {
      //xtm_printf("VideoDeal_Func::talk_TotalPackage = %d, talk_CurrPackage = %d\n", talk_TotalPackage, talk_CurrPackage);
      PackOK = 1;
      LostPackNo[0] = 100;
      LostPackNum = 0;
      for(j=0; j<talk_TotalPackage; j++)
       if(PackExist[Curr_Mjpeg_Stream][j] == 0)
        {
         PackOK = 0;
         LostPackNo[LostPackNum] = j;
         LostPackNum ++;
         //break;
        }

      if(PackOK == 1)
       {
        FramenoOK = 1;
        OldFrameno = PackFrameno[Curr_Mjpeg_Stream][0];
        for(j=1; j<talk_TotalPackage; j++)
         if(OldFrameno != PackFrameno[Curr_Mjpeg_Stream][j])
          {
           FramenoOK = 0;
           break;
          }
       }

      for(j=0; j<MAX_VIDEO_PACK; j++)
       {
        PackExist[0][j] = 0;
        PackExist[1][j] = 0;
        PackFrameno[0][j] = 0;
        PackFrameno[1][j] = 0;
       }
      if(PackOK == 0)
       {
        for(j=0; j<LostPackNum; j++)
          xtm_printf("PackOK 2 ................ = %d, Frameno = %d, TotalPackage = %d, %d, LostPackNo[%d] = %d\n", PackOK, talk_Frameno, talk_TotalPackage, (uint16_t)(talk_Framelen & 0xffff), j, LostPackNo[j]);
        return;
       }
      if(FramenoOK == 0)
       {
        xtm_printf("FramenoOK................ = %d, OldFrameno = %d, PackFrameno[%d][%d] = %d\n", FramenoOK, talk_Frameno, Curr_Mjpeg_Stream, j, PackFrameno[Curr_Mjpeg_Stream][j]);
        return;
       }

 #if 1
      video_decode_no = talk_Frameno;  //视频解码包号
      old_mjpeg_stream_no = Curr_Mjpeg_Stream;  //视频解码包号
      Video_Maddr = mjpeg_stream[Curr_Mjpeg_Stream];
      Video_Len = talk_Framelen + mjpeg_stream_offset;
	  if(mjpeg_stream_offset == 0x26f){
	  	Video_Len += 2;
	  	mjpeg_stream[Curr_Mjpeg_Stream][Video_Len - 2] = 0xff;
		mjpeg_stream[Curr_Mjpeg_Stream][Video_Len - 1] = 0xd9;
	  }
      mjpeg_stream_len[Curr_Mjpeg_Stream] = Video_Len;


      if((Local.Status == 2)||(Local.Status == 6))
       if((Local.HavePicRecorded == 0)||(Local.ManualRecordFlag == 1)) //手动录制  //有照片已录制
        {
         Local.IFrameCount ++;
         if(Local.IFrameCount > 30)
          {
           //有jpeg 头
          /* if((mjpeg_stream[Curr_Mjpeg_Stream][mjpeg_stream_offset] == 0xff)&&(mjpeg_stream[Curr_Mjpeg_Stream][mjpeg_stream_offset+1] == 0xd8)
		   	  &&(mjpeg_stream[Curr_Mjpeg_Stream][mjpeg_stream_len[Curr_Mjpeg_Stream] - 4] == 0xff)&&(mjpeg_stream[Curr_Mjpeg_Stream][mjpeg_stream_len[Curr_Mjpeg_Stream]-3] == 0xd9))
           	{
           	  TmpRecordsContent.Length = mjpeg_stream_len[Curr_Mjpeg_Stream] - mjpeg_stream_offset - 2;  
              memcpy(Memory6_Stream, mjpeg_stream[Curr_Mjpeg_Stream] + mjpeg_stream_offset, TmpRecordsContent.Length);
			           	
           	}
		   else*/
		   	{
              memcpy(Memory6_Stream, mjpeg_stream[Curr_Mjpeg_Stream], mjpeg_stream_len[Curr_Mjpeg_Stream]);
			  TmpRecordsContent.Length = mjpeg_stream_len[Curr_Mjpeg_Stream];
		   	}  
           Local.IFrameCount = 0;
           Local.RecordPic = 0;
           Local.HavePicRecorded = 1;  //有照片已录制
           Local.ManualRecordFlag = 0; //手动录制
           TmpRecordsContent.HavePic = 0;
           if(Local.DecodeFrameSize == VIDEO_VGA) //解码大小
             TmpRecordsContent.HavePic = 1;  //有无图像标志  0   1 -- 640x480  2 -- 320x240
           if(Local.DecodeFrameSize == VIDEO_QVGA) //解码大小
             TmpRecordsContent.HavePic = 2;  //有无图像标志  0   1 -- 640x480  2 -- 320x240
           
           xtm_printf("capture talk pic\n");
          }
        }

        
      Curr_Mjpeg_Stream ++;
      if(Curr_Mjpeg_Stream > 1)
        Curr_Mjpeg_Stream = 0;
        
       //xtm_printf("Video_Maddr = 0x%X, Video_Len = 0x%X\n", Video_Maddr, Video_Len);

      if(Video_Len < 0xff00)
        DecoceVideo(Video_Maddr, Video_Len);
      else
        xtm_printf("Video_Len = 0x%X\n", Video_Len);
 #endif
     }
#endif
   }
#endif  
}
//-----------------------------------------------------------------------
#ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
//退出组播组（主叫方->被叫方，被叫方应答）
void RecvTalkLeaveGroup_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)
{
  uint16_t sendlength;

  xtm_printf("recv exit group, %s\n", cFromIP);
  //本机为被叫方 被叫对讲
  if((Local.Status == 2)&&((recv_buf[7] & 0x03) == ASK))
   {
    memcpy(send_b, recv_buf, 61);
    send_b[7] = Local.MachineType | REPLY;    //应答
    sendlength = 61;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

    //printf("TalkEnd_ClearStatus 111\n");
    TalkEnd_ClearStatus(0);
    xtm_printf("对方要求离开组播组, %s\n", cFromIP);
   }
  else  //本机为主叫方 发起
   if((recv_buf[7] & 0x03) == REPLY)
   {
    //本机主动
    if(ClearMultiUdpBuff(VIDEOTALK, ASK, LEAVEGROUP, cFromIP))
     {
       xtm_printf("对方应答本机离开组播组, %s\n", cFromIP);
     }
   }
}
//-----------------------------------------------------------------------
void ExitGroup(int s_socket, uint8_t *buf)      //向其它被叫方退出组播组命令
{
  uint8_t i,j;
  uint8_t isIP;
  uint16_t sendlength;
  char RemoteIP[20];
  //退出组播组
  if(Remote.DenNum > 1)
   {
     for(j=0; j<Remote.DenNum; j++)
      {
       Remote.Added[j] = 0;
       xtm_printf("%d.%d.%d.%d  %d.%d.%d.%d\n",Remote.IP[j][0],Remote.IP[j][1],Remote.IP[j][2],Remote.IP[j][3],
              buf[53],buf[54],buf[55],buf[56]);
       //不为开始通话的那一方
       if((Remote.IP[j][0] == buf[53])&&(Remote.IP[j][1] == buf[54])
        &&(Remote.IP[j][2] == buf[55])&&(Remote.IP[j][3] == buf[56]))
         isIP = 1;
       else
         isIP = 0;

	   sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2], Remote.IP[j][3]);
       //检查是否已经在发送缓冲区内   //xu 20101109
       for(i=0; i<UDPSENDMAX; i++)
        if(Multi_Udp_Buff[i].isValid == 1)
         {
          
          if((strcmp(Multi_Udp_Buff[i].RemoteIP, RemoteIP) == 0)&&(Multi_Udp_Buff[i].buf[8] == LEAVEGROUP))
           {
            isIP = 1;
            xtm_printf("退出组播组命令已在缓冲内, %d.%d.%d.%d\n", Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2], Remote.IP[j][3]);
            break;
           }
         }

       if(isIP == 0)
        {
         //头部
         memcpy(send_b, UdpPackageHead, 6);
         //命令 
         send_b[6] = VIDEOTALK;
         send_b[7] = Local.MachineType | ASK;    //主叫
         send_b[8] = LEAVEGROUP;
         memcpy(send_b+9, LocalCfg.Addr, 20);
         memcpy(send_b+29, LocalCfg.IP, 4);
         memcpy(send_b+33, Remote.Addr[j], 20);
         memcpy(send_b+53, Remote.IP[j], 4);

         send_b[57] = Remote.GroupIP[0];
         send_b[58] = Remote.GroupIP[1];
         send_b[59] = Remote.GroupIP[2];
         send_b[60] = Remote.GroupIP[3];

         sendlength = 61;

         CreateMultiUdpBuff(s_socket, MULTI_SEND, RemoteIP, LocalPort[1], send_b, sendlength, 0);
        }
      }
   }
}
//-----------------------------------------------------------------------
void SingleExitGroup(int s_socket, uint8_t *buf) //向单个被叫方退出组播组命令
{
  uint16_t sendlength;
  //头部
  memcpy(send_b, UdpPackageHead, 6);
  //命令
  send_b[6] = VIDEOTALK;
  send_b[7] = Local.MachineType | ASK;    //主叫
  send_b[8] = LEAVEGROUP;
  memcpy(send_b+9, LocalCfg.Addr, 20);
  memcpy(send_b+29, LocalCfg.IP, 4);
  memcpy(send_b+33,  buf + 33, 20);
  memcpy(send_b+53, buf + 53, 4);                       

  send_b[57] = Remote.GroupIP[0];
  send_b[58] = Remote.GroupIP[1];
  send_b[59] = Remote.GroupIP[2];
  send_b[60] = Remote.GroupIP[3];

  sendlength = 61;

  CreateMultiUdpBuff(s_socket, MULTI_SEND, buf + 53, LocalPort[1], send_b, sendlength, 0);
  xtm_printf("单个 正在要求退出组播组, RemoteHost = %d.%d.%d.%d\0", \
                  buf[53], buf[54], buf[55], buf[56]);

}
//-----------------------------------------------------------------------
//同步状态
void RecvSyncSub_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //为主机，定时与副机同步状态
{
  uint8_t j;
  uint8_t isAddrOK;
  uint16_t sendlength;
  struct SyncSubData1 SubData1;  

  //本机被动
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost != '0')
     {
      memcpy(send_b, recv_buf, sizeof(struct SyncSubData1) + 34);
      send_b[7] = Local.MachineType | REPLY;    //应答
      sendlength = sizeof(struct SyncSubData1) + 34;
      CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

      isAddrOK = 1;    //查看主机地址是否匹配
      for(j=10; j<10+Local.AddrLen; j++)
       if(Local.HostAddr[j-10] != recv_buf[j])
        {
         isAddrOK = 0;
         break;
        }
      if(isAddrOK == 1)
       {
        memcpy(Local.HostIP, recv_buf + 30, 4);
        if(recv_buf[9] == 1)
         {
          Local.ConnToHost = 1;
          Local.ConnToHostTime = 0;

          memcpy(&SubData1,recv_buf+34, sizeof(struct SyncSubData1));
          CheckSubData1(SubData1);
         }
       }

      xtm_printf("收到主机同步状态命令\n");
     }
   }
  else  //本机主动
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SYNCSUB, cFromIP))
     {
      xtm_printf("对方应答本机同步状态, %d.%d.%d.%d\n", cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3]);
     }
   }
}
//-----------------------------------------------------------------------
void CheckSubData1(struct SyncSubData1 SubData1)  //检查同步数据1有无改变
{
#if 0
  uint8_t DataChanged;
  uint8_t DefenceChanged;
  uint8_t i,j;
  DataChanged = 0;
  DefenceChanged = 0;
  Local.DenNum = SubData1.DenNum;             //目标数量  副机
  memcpy(Local.DenIP, SubData1.DenIP, sizeof(Local.DenIP));    //副机IP
  memcpy(Local.DenAddr, SubData1.DenAddr, sizeof(Local.DenAddr));    //副机Addr

  xtm_printf("Local.DenNum = %d\n", Local.DenNum);
//  printf("Local.DenNum = %d, sizeof(struct SyncSubData1) = %d\n", Local.DenNum, sizeof(struct SyncSubData1));
  for(i=0; i<Local.DenNum; i++)
    Local.DenAddr[i][12] = '\0';

  /*if(DefenceChanged == 0)        //布防状态
   if(LocalCfg.DefenceStatus != SubData1.DefenceStatus)
    {
     DefenceChanged = 1;
     xtm_printf("SubData1 1, 布防状态\n");
    }  */

  if(DefenceChanged == 0)       //防区模块个数
   if(LocalCfg.DefenceNum != SubData1.DefenceNum)
    {
     DefenceChanged = 1;
     xtm_printf("SubData1 2, 防区模块个数\n");
    }
  if(DefenceChanged == 0)       //防区信息
   for(i=0; i<32; i++)
    for(j=0; j<10; j++)
     if(LocalCfg.DefenceInfo[i][j] != SubData1.DefenceInfo[i][j])
      {
       DefenceChanged = 1;
       i = 32;
       xtm_printf("SubData1 3, 防区信息\n");
       break;
      }

  LocalCfg.EngineerPass[4] = '\0';
  SubData1.EngineerPass[4] = '\0';
  if(DataChanged == 0)       //工程密码
   if(strcmp(LocalCfg.EngineerPass, SubData1.EngineerPass) != 0)
    {
     DataChanged = 1;
     xtm_printf("SubData1 4, 密码\n");
    }

  if(DataChanged == 1)
   {
    memcpy(LocalCfg.EngineerPass, SubData1.EngineerPass, sizeof(LocalCfg.EngineerPass));    //工程密码
    WriteCfgFile();  //写本地设置文件
   }

  //20101120
  if(DefenceChanged == 1)
   {
    LocalCfg.DefenceNum = SubData1.DefenceNum;          //防区模块个数
    memcpy(LocalCfg.DefenceInfo, SubData1.DefenceInfo, sizeof(LocalCfg.DefenceInfo));    //防区信息

    //布防状态
    if(LocalCfg.DefenceStatus != SubData1.DefenceStatus)
     {
      LocalCfg.DefenceStatus = SubData1.DefenceStatus;       //布防状态
      if(LocalCfg.DefenceStatus == 0)
       {
        // CancelFortifyDeal(); //撤防界面处理
       }
      else
       {
        //if(Local.CurrentWindow == 0)
        //  DisplayImage(&state_image[3], MAINYUVFBPAGE, FROMMEMORY);
        //SetGpio((unsigned long)GPIO_DATA_REG, FORTIFY_LED, SET_LOW);  //开布防 LED
       }
     }
    if((LocalCfg.DefenceStatus == 1)||(LocalCfg.DefenceStatus == 2)||(LocalCfg.DefenceStatus == 3)||(LocalCfg.DefenceStatus == 4)||(LocalCfg.DefenceStatus == 5))
     {
      Local.DefenceDelayFlag = 0;
      Local.DefenceDelayTime = 0;
     }
   }
#endif  
}
//-----------------------------------------------------------------------
//布防（副机－＞主机）
void RecvSubDefence_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j;
  uint8_t isAddrOK;
  uint16_t sendlength;
  uint8_t isOK;

  //本机被动
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost == '0')  //为主机
     {
      isAddrOK = 1;    //查看主机地址是否匹配
      for(j=9; j<9+Local.AddrLen; j++)
       if(LocalCfg.Addr[j-9] != recv_buf[j])
        {
         isAddrOK = 0;
         break;
        }
      if(isAddrOK == 1)
       {
        isOK = 1;
        memcpy(send_b, recv_buf, 62);
        send_b[7] = REPLY;    //应答
        send_b[62] = isOK;
        sendlength = 63;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        if(isOK == 1)
         {
          switch(recv_buf[57])
           {
            case 0: //撤防
                   //20101109  不在报警状态下，副机才可撤防
                   xtm_printf("Local.AlarmByte[0] = 0x%02X, Local.AlarmByte[1] = 0x%02X\n", Local.AlarmByte[0], Local.AlarmByte[1]);
                   //if((Local.AlarmByte[0] == 0)&&(Local.AlarmByte[1] == 0))
                   //  CancelFortifyDeal(); //撤防界面处理
                   break;
            case 1: //外出布防
            case 2: //在家布防
            case 3: //就寝布防
            case 4: //起床布防
            case 5: //自定义布防
                  LocalCfg.DefenceStatus = recv_buf[57];
                 // FortifyFinishDeal(); //布防完成界面处理
                  break;
            case 11: //外出布防延时
            case 12: //在家布防延时
            case 13: //就寝布防延时中
            case 14://起床布防延时中

            case 15://自定义布防延时中
                     LocalCfg.DefenceStatus = recv_buf[57];
                   //  if(Local.CurrentWindow == 32)
                   //    DisplayCancelFortifyWindow(1);
                     Local.DefenceDelayFlag = 1;    //布防延时标志
                     Local.DefenceDelayTime = 0;  //计数
                 break;
           }
         }
       }

      //20101104
     // SendHostAssiOrder(LocalCfg.DefenceStatus, 1);

      xtm_printf("收到副机布防命令 %d\n", recv_buf[57]);
     }
   }
  else  //本机主动
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SUBDEFENCE, cFromIP))
     {
      xtm_printf("对方应答副机布防命令 %d\n", recv_buf[57]);
     }
   }
}
//-----------------------------------------------------------------------
//布防提示（主机－＞副机）
void RecvSubDefenceTip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j;
  uint8_t isAddrOK;
  uint16_t sendlength;

  //本机被动
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost != '0')  //为副机
     {
      isAddrOK = 1;    //查看副机地址是否匹配
      for(j=33; j<33+Local.AddrLen; j++)
       if(LocalCfg.Addr[j-33] != recv_buf[j])
        {
         isAddrOK = 0;
         break;
        }
      if(isAddrOK == 1)
       {
        memcpy(send_b, recv_buf, 58);
        send_b[7] = REPLY;    //应答
        sendlength = 58;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        switch(recv_buf[57])
         {
          case 0: //撤防
                // CancelFortifyDeal(); //撤防界面处理
                 break;
          case 1: //外出布防
          case 2: //在家布防
          case 3: //就寝布防
          case 4://起床布防
          case 5://自定义布防
                  LocalCfg.DefenceStatus = recv_buf[57];
                  //FortifyFinishDeal(); //布防完成界面处理
                 //break;
          case 11: //外出布防延时
          case 12: //在家布防延时
          case 13: //就寝布防延时中
          case 14://起床布防延时中

          case 15://自定义布防延时中
                     LocalCfg.DefenceStatus = recv_buf[57];
                    // if(Local.CurrentWindow == 32)
                    //   DisplayCancelFortifyWindow(1);
                     Local.DefenceDelayFlag = 1;    //布防延时标志
                     Local.DefenceDelayTime = 0;  //计数
                 break;
         }
       }

      xtm_printf("收到主机布防提示命令 %d\n", recv_buf[57]);
     }
   }
  else  //本机主动
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SUBDEFENCETIP, cFromIP))
     {
      xtm_printf("对方应答主机布防提示命令 %d\n", recv_buf[57]);
     }
   }
}
//-----------------------------------------------------------------------
//报警（副机－＞主机）
void RecvSubAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  int j;
  int isAddrOK;
  int sendlength;
  int isOK;

  //本机被动
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost == '0')  //为主机
     {
      isAddrOK = 1;    //查看主机地址是否匹配
      for(j=9; j<9+Local.AddrLen; j++)
       if(LocalCfg.Addr[j-9] != recv_buf[j])
        {
         isAddrOK = 0;
         break;
        }
      if(isAddrOK == 1)
       {
        isOK = 1;
        memcpy(send_b, recv_buf, 62);
        send_b[7] = REPLY;    //应答
        send_b[58] = isOK;
        sendlength = 59;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

        if(isOK == 1)
         {
          //此处插入报警处理
          
          if(recv_buf[57] == 37) //劫持报警
           {
            //SendAlarmFunc(0x01, Local.AlarmByte);
            //CancelAlarmDeal(); //取消报警界面处理
            xtm_printf("劫持报警\n");
           }
         }
       }

      xtm_printf("收到副机报警命令 %d\n", recv_buf[57]);
     }
   }
  else  //本机主动
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SUBALARM, cFromIP))
     {
      xtm_printf("对方应答副机报警命令 %d\n", recv_buf[57]);
     }
   }
}
//-----------------------------------------------------------------------
//报警提示（主机－＞副机）
void RecvSubAlarmTip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  int j;
  int isAddrOK;
  int sendlength;
  int isOK;

  //本机被动
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost == '0')  //为主机
     {
      isAddrOK = 1;    //查看主机地址是否匹配
      for(j=9; j<9+Local.AddrLen; j++)
       if(LocalCfg.Addr[j-9] != recv_buf[j])
        {
         isAddrOK = 0;
         break;
        }
      if(isAddrOK == 1)
       {
        isOK = 1;
        memcpy(send_b, recv_buf, 62);
        send_b[7] = REPLY;    //应答
        send_b[62] = isOK;
        sendlength = 63;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        if(isOK == 1)
         {
          switch(recv_buf[57])
           {
            case 0:
                   //撤防
                   break;
            case 1:
                   //外出布防
                   break;
            case 2:
                   //在家布防
                   break;
            case 3:
                   //就寝布防
                   break;
            case 4:
                   //起床布防
                   break;
            case 5:
                   //自定义布防
                   break;
           }
         }
       }

      xtm_printf("收到副机报警命令 %d\n", recv_buf[57]);
     }
   }
  else  //本机主动
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SUBCONTROL, cFromIP))
     {
      xtm_printf("对方应答副机报警提示命令 %d\n", recv_buf[57]);
     }
   }
}
//-----------------------------------------------------------------------
void RecvSubFind_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length) //查找副机（主机－＞副机）
{
#if 0
  int i,j,k;
  int isAddrOK;
  int sendlength;
  int datalength;
  int isOK;
  int DenAddrLen;
  char TmpAddrVerifyMa[10];

    //struct timeval tv1,tv2;
    //gettimeofday(&tv1, NULL);
    //printf("tv1.tv_sec=%d, tv1.tv_usec=%d, %s\n", tv1.tv_sec,tv1.tv_usec, cFromIP);

  isOK = 0;
  //本机被动
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost != '0')  //为副机
     {
      isAddrOK = 1;    //查看副机地址是否匹配
      for(j=33; j<33+Local.AddrLen; j++)
       if(LocalCfg.Addr[j-33] != recv_buf[j])
        {
         isAddrOK = 0;
         break;
        }
      if(isAddrOK == 1)
       {
        memcpy(send_b, recv_buf, 57);
        send_b[7] = REPLY;    //应答
        memcpy(send_b + 53, LocalCfg.IP, 4);
        memcpy(send_b + 57, LocalCfg.AddrVerifyMa, 6);
        send_b[63] = 0;
        sendlength = 64;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        xtm_printf("收到查找副机命令 %d, %d.%d.%d.%d\n", recv_buf[6], cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3]);
       }
     }
   }
  else  //本机主动
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    //printf("RecvSubFind_Func %s\n", cFromIP);
    //本机主动
    if((recv_buf[7]& 0x03) == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if(Multi_Udp_Buff[i].buf[6] == HOSTTOSUB)
          if((Multi_Udp_Buff[i].buf[7] & 0x03) == ASK)
           if(Multi_Udp_Buff[i].buf[8] == SUBFIND)
           {
              //判断要求查找地址是否匹配
              isAddrOK = 1;
              switch(Multi_Udp_Buff[i].buf[33])
               {
                case 'S':
                case 'B':
                         DenAddrLen = 12;
                         break;
                case 'H':
                         DenAddrLen = 6;
                         break;
                case 'W':
                case 'Z':
                         DenAddrLen = 5;
                         break;
                case 'M':
                         DenAddrLen = 8;
                         break;
               }
              isAddrOK = 1;    //查看副机地址是否匹配 
              for(j=33; j<33+DenAddrLen; j++)
               if(Multi_Udp_Buff[i].buf[j] != recv_buf[j])
                {
                 isAddrOK = 0;
                 break;
                }

              memcpy(TmpAddrVerifyMa, recv_buf + 57, 6);
              TmpAddrVerifyMa[6] = '\0';
              #if 0
               char tmp_text1[20], tmp_text2[20];
               memcpy(tmp_text1, Multi_Udp_Buff[i].buf + 33, DenAddrLen);
               tmp_text1[12] = '\0';
               memcpy(tmp_text2, recv_buf + 33, DenAddrLen);
               tmp_text2[12] = '\0';
               printf("tmp_text1 = %s, tmp_text2 = %s\n", tmp_text1, tmp_text2);
               printf("TmpAddrVerifyMa = %s, LocalCfg.AddrVerifyMa = %s, isAddrOK = %d, DenAddrLen = %d\n", TmpAddrVerifyMa, LocalCfg.AddrVerifyMa, isAddrOK, DenAddrLen);
              #endif
              if(isAddrOK == 1)
               {
                if(strcmp(TmpAddrVerifyMa, LocalCfg.AddrVerifyMa) == 0)   //主副机同步码
                 {
                  isOK = 1;
                  Multi_Udp_Buff[i].isValid = 0;
                  Multi_Udp_Buff[i].SendNum = 0;
                  xtm_printf("对方应答查找副机命令 %d.%d.%d.%d\n", cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3]);
                 }
                else
                 {
                  xtm_printf("对方应答查找副机命令, 但是验证码错误 %d.%d.%d.%d, %s\n", cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3], TmpAddrVerifyMa);
                 }
                break;
               }
             }
   }

    if(isOK == 1) //主机查找副机成功
     {
      //查看列表中是否已有该副机信息
      isAddrOK = 0;
      for(i=0; i<Local.TmpDenNum; i++)
       {
        isAddrOK = 1;
        for(j=0; j<Local.AddrLen; j++)
         if(Local.TmpDenAddr[i][j] != recv_buf[j+33])
          {
           isAddrOK = 0;
           break;
          }
        if(isAddrOK == 1)
          for(j=0; j<4; j++)
           if(Local.TmpDenIP[i][j] != recv_buf[j+53])
            {
             isAddrOK = 0;
             break;
            }
        if(isAddrOK == 1)
          break;
       }
      //printf("isAddrOK = %d, Local.TmpDenNum = %d\n", isAddrOK, Local.TmpDenNum);
      if(isAddrOK == 0)
       {
        memcpy(Local.TmpDenIP[Local.TmpDenNum], recv_buf + 53, 4);
        memcpy(Local.TmpDenAddr[Local.TmpDenNum], recv_buf + 33, 20);
        Local.TmpDenAddr[Local.TmpDenNum][12] = '\0';
        Local.TmpDenNum ++;
       }
   }

    //gettimeofday(&tv2, NULL);
    //printf("tv2.tv_sec=%d, tv2.tv_usec=%d\n", tv2.tv_sec,tv2.tv_usec);
#endif	
}
#endif
//-----------------------------------------------------------------------
#ifdef _REMOTE_FORTIFY_SUPPORT  //远程布撤防支持
void RecvRemoteFortify_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
#if 0
  uint8_t i, j;
  char password[10];
  uint8_t sendlength;
  uint8_t is_OK;
  memcpy(password, recv_buf + 58, 4);
  password[4] = '\0';
  memcpy(send_b, recv_buf, 63);
  send_b[7] = Local.MachineType | REPLY;    //应答
  if(Local.Status != 0)
    send_b[57] = 0;
  else
   {
    is_OK = 0;
    if(recv_buf[62] == 2)
      is_OK = 1;
    if(recv_buf[62] == 1)
     if(strcmp(password, LocalCfg.EngineerPass) == 0)
       is_OK = 1;
       
    if(is_OK == 1)
     {
      send_b[57] = 1;
      if((Local.AlarmByte[0] != 0)||(Local.AlarmByte[1] != 0))
       {
              for(i=0; i<8; i++)
               for(j=0; j<10; j++)
                LocalCfg.DefenceInfo[i][3] = 0;
              //20101118 xu
              if((Local.AlarmByte[0] != 0)||(Local.AlarmByte[1] != 0))
                SendCancelAlarmFunc(); //取消报警函数

              for(i=0; i<MAXDEFENCENUM; i++)
               {
                Local.AlarmDelayFlag[i] = 0;     //报警延时标志
                Local.AlarmDelayTime[i] = 0;
               }

              for(i=0; i<2; i++)
                Local.AlarmByte[i] = 0;

              PlayCancelAlarmWav();

              CancelAlarmDeal(); //取消报警界面处理
       }
      else
       {
        if(LocalCfg.DefenceStatus == 0)
         {
          LocalCfg.DefenceStatus = 11;
          Local.DefenceDelayFlag = 1;    //布防延时标志
          Local.DefenceDelayTime = 0;    //计数

          DisplayCancelFortifyWindow(NOREFRESH);
         }
        else
         {
          PlayCancelFortifyWav();
          CancelFortifyDeal(); //撤防界面处理
         }
       }
     }
    else
      send_b[57] = 0;
   }

  sendlength = length;
  CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
#endif  
}
#endif
//-----------------------------------------------------------------------
