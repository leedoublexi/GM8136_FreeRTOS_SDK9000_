
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
#define MAX_DOWN_PACK    512  //������ذ���  512K

#define MAX_VIDEO_PACK   60  //ÿ֡�����Ƶ����

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
uint8_t recvBuf[1600];   //��ջ����̫��
uint8_t recvBuf1[1600];  //��ջ����̫��
uint8_t recvBuf2[1600];  //

uint8_t *Memory6_Stream; //��������
void xtm_printf(char *fmt, ...);

uint8_t Udp_InitFlag = 0;
int Init_Udp(void);    //��ʼ��UDP
int UnInit_Udp(void);    

void AddMultiGroup(int m_Socket, char *McastAddr);  //�����鲥��
void DropNsMultiGroup(int m_Socket, char *McastAddr);  //�˳�NS�鲥��

void UdpDataRecvTask(int *s_socket);
void UdpVideoRecvTask(int *s_socket);

#ifdef _PON_SUPPORT
  void UdpPonRecvTask(int *s_socket);
  //���������豸Ӧ��
  void RecvPonSearchAllEquip_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);
  //д�豸��ַӦ��
  void RecvPonWriteEquipAddr_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);
#endif

void MultiSendTask(void);

uint16_t old_audioFrameno = 0;
uint8_t CheckSameNet(uint8_t *IP1, uint8_t *IP2, uint8_t *IPMask);
void CreateMultiUdpBuff(int s_socket, uint8_t SendMode, char *RemoteIP, uint16_t DestPort, uint8_t *udpdata, uint16_t nlength, uint8_t CurrOrder);
void CheckMultiUdpBuff(void);   //���UDP���ͻ���
void MultiUdpSendFailDeal(uint8_t i);
uint8_t ClearMultiUdpBuff(uint8_t Order1, uint8_t Order2, uint8_t Order3, char *cFromIP);   //Clear UDP���ͻ���

uint16_t UdpSendBuff(int s_socket, char *Ip_Addr, int Port, char *sendbuf, int sendlen);

void CheckRemoteAddr(void);  //���Զ�˵�ַ

void RecvUdpTest_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
void RecvUdpTest1_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//���������豸Ӧ��
void RecvSearchAllEquip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint8_t *DestIP, uint16_t DestPort, uint16_t length);
//д�豸��ַӦ��
void RecvWriteEquipAddr_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint8_t *DestIP, uint16_t DestPort, uint16_t length);

#ifdef _ROOM_SEARCH_SUPPORT      //���ڻ����� С�ſڻ���4.3���ڻ�
 //���ڻ����������豸Ӧ��
 void RecvRoomSearchAllEquip_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);
 //���ڻ�д�豸��ַӦ��
 void RecvRoomWriteEquipAddr_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);

 //���ڻ����� С�ſڻ���4.3���ڻ�
 void RecvRegisterIDCard_Func(UINT8 *recv_buf, UINT8 *cFromIP, UINT8 *DestIP, UINT16 DestPort, UINT16 length);
#endif

//����
void RecvDownLoadFile_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
uint8_t WriteAllToFlash(uint8_t *recv_buf, struct downfile1 *DownData);

//��Flash����
void RecvReadFlash_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//Զ��������
void RecvRemoteReset_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//����Զ�̵�����Ϣ
void RecvRemoteDebugInfo_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
void send_debug_info(int s_socket, char *debug_info);

#ifdef _REMOTECALLTEST  //Զ�̺��в���
//����Զ�̺��в��ԡ�
void RecvRemoteCallTest_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
#endif

#ifdef _ADDTABLE_SUPPORT  //��ַ��֧��
//����ַ����Ϣ
void RecvReadAddrTable_Func(int s_socket, uint8_t *recv_buf, uint8_t *cFromIP, uint16_t DestPort, uint16_t length);
//void RecvReadAddrTable_Func(uint8_t *recv_buf, uint8_t *cFromIP, uint16_t DestPort, uint16_t length);
//����ַ���ļ�
extern void ReadAddrTableFile(void);
#endif

//����
void RecvNSAsk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);  //��������
void RecvNSReply_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);//����Ӧ��

uint16_t Curr_Frameno = 10000;
uint16_t PackFrameno[MAXVIDEOBUF][MAX_VIDEO_PACK];
uint8_t PackExist[MAXVIDEOBUF][MAX_VIDEO_PACK];

//��Ϣ
void RecvMessage_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//�豸��ʱ����״̬
void RecvReportStatus_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//�������Ĳ�ѯ�豸״̬
void RecvQueryStatus_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//����
void RecvAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t lengtht);
//ȡ������
void RecvCancelAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//��λ����
void RecvResetPass_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
//�Խ�
void RecvTalkCall_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //�Խ�����
void RecvTalkCallAnswer_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //�Խ�����Ӧ��
void RecvTalkLineUse_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //�Խ�ռ��Ӧ��
void RecvTalkCallConfirm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort); //�Խ�����ȷ��
uint8_t AudioStartFlag = 0;
uint8_t AudioStartTime = 0;
uint8_t AudioTalkOpenFlag = 0;
void RecvTalkCallStart_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //�Խ���ʼͨ��
void RecvTalkCallEnd_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //�Խ����н���
void RecvTalkRemoteOpenLock_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //Զ�̿���
void RecvTalkCenterTurnTalk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //�����˹�ת��
void RecvTalkTrusteeship_Talk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //�й�

void RecvTalkCallUpDown_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);  //�Խ�����

//����
void RecvWatchCallAnswer_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //���Ӻ���Ӧ��
void RecvWatchLineUse_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //����ռ��Ӧ��
void RecvWatchCallConfirm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort); //��������ȷ��
void RecvWatchCallEnd_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //���Ӻ��н���

void RecvWatchCallUpDown_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);  //��������

void AudioDeal_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t length);  //��Ƶ���ݴ���
void VideoDeal_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t length);  //��Ƶ���ݴ���

void WatchEnd_ClearStatus(void); //���ӽ�������״̬�͹ر�����Ƶ
void TalkEnd_ClearStatus(int isTurn); //�Խ���������״̬�͹ر�����Ƶ

#ifdef _REMOTEWRITEOV7725REGISTER   //Զ��дOV7725�Ĵ���  ������
 void RecvRemoteWriteOV7725Register_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
#endif

//�����븱��
#ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
void RecvTalkLeaveGroup_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort); //�˳��鲥�飨���з�->���з������з�Ӧ��
void ExitGroup(int s_socket, uint8_t *buf);      //���������з��˳��鲥������
void SingleExitGroup(int s_socket, uint8_t *buf); //�򵥸����з��˳��鲥������

void RecvSyncSub_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort);  //Ϊ��������ʱ�븱��ͬ��״̬
void CheckSubData1(struct SyncSubData1 SubData1);  //���ͬ������1���޸ı�
void RecvSubDefence_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length); //��������������������
void RecvSubAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length); //��������������������
void RecvSubAlarmTip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length); //������ʾ����������������
void RecvSubDefenceTip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPortP, uint16_t length); //������ʾ����������������
void RecvSubFind_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length); //���Ҹ�������������������
#endif

#ifdef _REMOTE_FORTIFY_SUPPORT  //Զ�̲�����֧��
 void RecvRemoteFortify_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);
#endif

#ifdef ELEVATORCONTROL_HANGZHOU
 void RecvElevatorControl_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length);  //���ݿ���
#endif

uint16_t video_decode_no;  //��Ƶ�������
uint16_t old_mjpeg_stream_no;  //��Ƶ�������

//��Ƶ���ջ���  δ����
#define MAX_STREAM_NUM 2
uint8_t Curr_Mjpeg_Stream = 0;
uint8_t *mjpeg_stream[MAX_STREAM_NUM];       //mjpeg ��
uint16_t mjpeg_stream_len[MAX_STREAM_NUM];   //mjpeg ��
uint16_t mjpeg_stream_offset = 0x26f;

uint8_t Down_RecvFlag[MAX_DOWN_PACK];          //���� ����־
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
int FirstFrameFlag = 0; //��һ֡���ж�jpeg header
int FirstFrameFinished = 0;
void ResetMjpegHeader(void);
//---------------------------------------------------------------------------
void xtm_printf(char *fmt, ...)
{
	char debug_info[500];
	va_list args;		//����һ��va_list���͵ı������������浥������  
	va_start(args, fmt); //ʹargsָ��ɱ�����ĵ�һ������  
	vprintf(fmt, args);	//������vprintf�ȴ�V��	
	
	if(Local.RemoteDebugInfo == 1)
	{  
		//sprintf(debug_info, fmt, ...);
		vsprintf(debug_info, fmt, args);
		send_debug_info(UDP_DataSocket, debug_info);
	}
	va_end(args);		//�����ɱ�����Ļ�ȡ  	
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


int Init_Udp(void)    //��ʼ��UDP
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
	
  #ifdef _PON_SUPPORT   //PON ֧�� 20140314
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

  // ������˵����� 
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

  //������˵����� 
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
  
    // ������˵����� 
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

  //����UDP��Commͨ��
	multi_send_sem = xSemaphoreCreateCounting(10, 0);
	//������
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

void AddMultiGroup(int m_Socket, char *McastAddr)  //�����鲥��
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
void DropNsMultiGroup(int m_Socket, char *McastAddr)  //�˳�NS�鲥��
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
				case SENDMESSAGE: //��Ϣ
						RecvMessage_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
					break;
				case REPORTSTATUS:	 //�豸��ʱ����״̬
						if(recv_len >= SMALLESTSIZE)
						{
							RecvReportStatus_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("�豸��ʱ����״̬Ӧ�𳤶��쳣\n");
						}
					break;
				case QUERYSTATUS:	//�������Ĳ�ѯ�豸״̬
						if(recv_len >= SMALLESTSIZE)
						{
							RecvQueryStatus_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("�������Ĳ�ѯ�豸״̬�����쳣\n");
						}
					break;
				case ALARM:   //����
						if(recv_len >= SMALLESTSIZE)
						{
							RecvAlarm_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("����Ӧ�𳤶��쳣\n");
						}
					break;
				case CANCELALARM:	//ȡ������
						if(recv_len >= SMALLESTSIZE)
						{
							RecvCancelAlarm_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("ȡ������Ӧ�𳤶��쳣\n");
						}
					break;
				case RESETPASS:    //��λ����
						if(recv_len >= SMALLESTSIZE)
						{
							RecvResetPass_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("��λ���볤���쳣\n");
						}
					break;
				case SEARCHALLEQUIP:  //252  //���������豸���������ģ����豸��
						if(recv_len >= SMALLESTSIZE)//DestIP
						{
							RecvSearchAllEquip_Func(*s_socket, recvBuf, FromIP, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("���������豸�����쳣\n");
						}
					break;
				case WRITEEQUIPADDR:  //254  //д�豸��ַ���������ģ����豸��
						if(recv_len >= SMALLESTSIZE)
						{
							RecvWriteEquipAddr_Func(*s_socket, recvBuf, FromIP, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("д�豸��ַ�����쳣\n");
						}
					break;
				case DOWNLOADFILE:	 //����
						if(recv_len >= 9)
						{
							//xtm_printf("DataPortDeal::1111\n");
							RecvDownLoadFile_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("���س����쳣\n");
						}
					break;
				case READFLASH: 	 //��FLASH����
						if(recv_len >= 9)
						{
							//xtm_printf("DataPortDeal::1111\n");
						  RecvReadFlash_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("��FLASH���ݳ����쳣\n");
						}
					break;
				case REMOTERESET:	   //Զ��������
						if(recv_len >= 9)
						{
							RecvRemoteReset_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("Զ�������������쳣\n");
						}
					break;
				case UDP_TEST:
						RecvUdpTest_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
					break;
				case 250:
						RecvUdpTest1_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
					break;              
				case REMOTEDEBUGINFO:	   //����Զ�̵�����Ϣ
						if(recv_len == 29)
						{
							RecvRemoteDebugInfo_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
						}
						else
						{
							xtm_printf("����Զ�̵�����Ϣ�����쳣\n");
						}
					break;
        #ifdef _REMOTECALLTEST  //Զ�̺��в���
					case REMOTETEST:	  //����Զ�̺��в��ԡ�
							if(recv_len == 49)
							{
								RecvRemoteCallTest_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
							}
							else
							{
								xtm_printf("����Զ�̺��в��Գ����쳣\n");
							}
						break;
        #endif
				case NSSERVERORDER:  //����������(NS������)
						switch(recvBuf[7] & 0x03)
						{
							case 2://������Ӧ
									if(recv_len >= SMALLESTSIZE)
									{
										RecvNSReply_Func(*s_socket, recvBuf, FromIP, DestPort, recv_len);
									}
									else
									{
										xtm_printf("����Ӧ�����ݳ����쳣\n");
									}
								break;
						}
					break;
	      #ifdef _ADDTABLE_SUPPORT  //��ַ��֧��
          case READ_ADDRTABLE: //����ַ����Ϣ
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
//��Ϣ
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
  //��ַƥ��
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
    //ֻ������ͨ��Ϣ  
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
          xtm_printf("RecvMessage_Func::����Ϣ����Ѵ���, 0x%X 0x%X, j = %d\n", (uint16_t)((TmpInfoCon.Sn & 0xffff0000) >> 16), (uint16_t)(TmpInfoCon.Sn & 0xffff), j);
          return;
        }
      }
      //����Ϣδ�����������˳����ƣ�������Ϣ���뵽ͷ��
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
      else   //�������һ����˳����ƣ�������Ϣ���뵽ͷ��
      {
        for(j = (Info.TotalNum - 2); j >= 0; j --)
        {
          memcpy(&InfoContent[j+1], &InfoContent[j], sizeof(TmpInfoCon));
          strcpy(InfoBuff[j+1], InfoBuff[j]);
        }
      }

      InfoContent[0].isValid = TmpInfoCon.isValid;  //��Ч��δɾ����־   1
      InfoContent[0].isReaded = TmpInfoCon.isReaded; //�Ѷ���־    1
      InfoContent[0].HavePic = TmpInfoCon.HavePic;  //����ͼ���־    1
      strcpy(InfoContent[0].RemoteAddr, TmpInfoCon.RemoteAddr);
      strcpy(InfoContent[0].Time, TmpInfoCon.Time);    //����ʱ��    32
      InfoContent[0].Type = TmpInfoCon.Type;     //����        1    ��Ϣ���ͻ��¼�����
      InfoContent[0].Sn = TmpInfoCon.Sn;      //���        4
      InfoContent[0].Length = TmpInfoCon.Length;       //����        4
      strcpy(InfoBuff[0], TmpContent);

      for(j=0; j<Info.TotalNum; j++)
        xtm_printf("RecvMessage_Func::Info.TotalNum = %d, InfoBuff[%d] = %s\n", Info.TotalNum, j, InfoBuff[j]);

      WriteInfoCfgFile();     //д��Ϣ�����ļ�
      WriteInfoFile();        //д��Ϣ�ļ�
      Recv_NewInfo();
    }
    else
    {
      xtm_printf("��Ϣ���ݳ��ȴ�������Ͳ���\n");
    }
  }   
}
//---------------------------------------------------------------------------
//�豸��ʱ����״̬
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
  //��ַƥ��
  if(isAddrOK == 1)
  {
    if((recv_buf[7] & 0x03) == REPLY)   //Ӧ��
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
          xtm_printf("�յ��豸��ʱ����״̬Ӧ��, LocalCfg.ReportTime = %d\n", LocalCfg.ReportTime);
        }
        //xtm_printf("�յ��豸��ʱ����״̬Ӧ��\n");
      }
    }
  }
}
//---------------------------------------------------------------------------
//�������Ĳ�ѯ�豸״̬
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
  //��ַƥ��
  if(isAddrOK == 1)
  {
    //ͷ��
    memcpy(send_b, UdpPackageHead, 6);
    //����
    send_b[6] = QUERYSTATUS;
    send_b[7] = REPLY;    //����
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
    xtm_printf("�յ��������Ĳ�ѯ�豸״̬\n");
  }
}
//---------------------------------------------------------------------------
//����
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
  //��ַƥ��
  if(isAddrOK == 1)
  {
    if((recv_buf[7] & 0x03) == REPLY)   //Ӧ��
    {
      if(ClearMultiUdpBuff(ALARM, ASK, LocalCfg.Addr[0], cFromIP))
      {
        AlarmByte = (recv_buf[37] & 0x3F);
        tmp_p = 0x01;
        for(j=0; j<6; j++)
        {
          if((AlarmByte & tmp_p) == tmp_p)
            LocalCfg.DefenceInfo[j][3] = 2; //�����ѽ���
          tmp_p = (tmp_p << 1);
        }
        xtm_printf("�յ�����Ӧ��\n");
      }
    }
  }
}
//---------------------------------------------------------------------------
//ȡ������
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
  //��ַƥ��
  if(isAddrOK == 1)
  {
    if((recv_buf[7] & 0x03) == REPLY)   //Ӧ��
    {
      if(ClearMultiUdpBuff(CANCELALARM, ASK, LocalCfg.Addr[0], cFromIP))
      {
        xtm_printf("�յ�ȡ������Ӧ��\n");
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
  //��ַƥ��
  if(isAddrOK == 1)
  {
    if((recv_buf[7] & 0x03) == ASK)   //Ӧ��
    {
      memcpy(send_b, recv_buf, length);
      send_b[7] = REPLY;    //Ӧ��
      sendlength = length;
      CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

      //��λ����
      switch(recv_buf[28])
      {
        case 1: //��λ��������
            xtm_printf("��λ��������ɹ�\n");
            if(strcmp(LocalCfg.EngineerPass, "1234") != 0)
            {
              strcpy(LocalCfg.EngineerPass, "1234");
              WriteCfgFile();     //��Flash�д洢�ļ�
            }
          break;
        case 2: //��λ��������
          break;
      }
    }
  }
}

//---------------------------------------------------------------------------
uint16_t  OldTestPackNo = 0;
//�շ�����
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
//���������ղ���
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
//����Ƿ���ͬһ����
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

//���������豸Ӧ��
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
    
  //��ַƥ��
  if(isAddrOK == 1)
  {
    //xtm_printf("RecvSearchAllEquip_Func2, s_socket = %d, cFromIP = %s, UDP_DataSocket = %d\n", s_socket, cFromIP, UDP_DataSocket);

    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //Ӧ��

    send_b[8] = 0x05;
    send_b[9] = 0;

    //��ַ����
    memcpy(send_b + 10, LocalCfg.Addr, 20);
    //������ַ
    memcpy(send_b + 30, LocalCfg.Mac_Addr, 6);
    //IP��ַ
    memcpy(send_b + 36, LocalCfg.IP, 4);
    //��������
    memcpy(send_b + 40, LocalCfg.IP_Mask, 4);
    //���ص�ַ
    memcpy(send_b + 44, LocalCfg.IP_Gate, 4);
    //��������ַ
    memcpy(send_b + 48, LocalCfg.IP_Server, 4);
    //�汾��
    for(i=0; i<20; i++)
      tmp_text[i] = '\0';

    #ifdef _SEND_TO_SELF_TEST    //�������ݰ����Լ�
      sprintf(tmp_text, "%s-%04X-%04X", SERIALNUM, LocalCfg.BootNo, LocalCfg.SelfBootNo);
    #else
      sprintf(tmp_text, "%s-%04X", SERIALNUM, LocalCfg.BootNo);
    #endif
    
    memcpy(send_b + 52, tmp_text, 16);
    //��ֻ�֧��
    send_b[68] = LocalCfg.MultiMachineSupport;
    send_b[69] = LocalCfg.DenMaxNum;  //���������������һ������С�� SUBMAXNUM�� �����ڼ�С�㲥��
    send_b[74] = LocalCfg.Language;
    send_b[90] = LocalCfg.MicDecValue;  //չ�䡢չ��ģʽ,���С Mic����
    //ͬ����
    memcpy(send_b + 92, LocalCfg.AddrVerifyMa, 6);
    sendlength = 98;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
  }
}
//---------------------------------------------------------------------------
//д�豸��ַӦ��
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

  //��ַƥ��
  if(isAddrOK == 1)
  {
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //Ӧ��
    sendlength = length;

    UdpSendBuff(s_socket, cFromIP, RemotePort[0], (char *)send_b, sendlength);

    //�豸��ʱ����״̬ʱ��
    LocalCfg.ReportTime = 10;
       
    //д��ַ����
    memcpy(LocalCfg.Addr, recv_buf + 52, 20);
    memcpy(LocalCfg.Mac_Addr, recv_buf + 72, 6);
    memcpy(LocalCfg.IP, recv_buf + 78, 4);
    memcpy(LocalCfg.IP_Mask, recv_buf + 82, 4);
    memcpy(LocalCfg.IP_Gate, recv_buf + 86, 4);
    memcpy(LocalCfg.IP_Server, recv_buf + 90, 4);
    LocalCfg.MultiMachineSupport = recv_buf[94];
    LocalCfg.DenMaxNum = recv_buf[95];  //���������������һ������С�� SUBMAXNUM�� �����ڼ�С�㲥��

    LocalCfg.MicDecValue = recv_buf[112];  //չ�䡢չ��ģʽ,���С Mic����

    if(LocalCfg.MicDecValue > 3) //չ�䡢չ��ģʽ,���С Mic����
    {
      LocalCfg.MicDecValue = 0;
    }
//    SetMicVolume();

    //ͬ����
    memcpy(LocalCfg.AddrVerifyMa, recv_buf + 114, 6);
    LocalCfg.AddrVerifyMa[6] = '\0';    

    if((LocalCfg.Addr[0] != 'S')&&(LocalCfg.Addr[0] != 'B'))
      LocalCfg.Addr[0] = 'B';
    
    if(LocalCfg.Addr[0] == 'B')
      memcpy(LocalCfg.Addr + 5, "000000", 6);
    
    if((LocalCfg.Addr[0] == 'S')||(LocalCfg.Addr[0] == 'B'))
    {
      Local.AddrLen = 12;  //��ַ����  S 12  B 12 M 8 H 6
    }
    
    //����
    Local.isHost = '0';
    Local.isHost = LocalCfg.Addr[11];
    Local.DenNum = 0;
    memcpy(Local.HostAddr, LocalCfg.Addr, 20);
    Local.HostAddr[11] = '0';
    WriteCfgFile();  //д���������ļ�

    #ifdef _XTM8900_SUPPORT  //XTM8900����֧��
		  net_refresh_addr(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
	  #endif
	  #ifdef _XTM9000_SUPPORT  //XTM9000����֧��
		  net_refresh_addr_9000(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
	  #endif	
  }
}
//---------------------------------------------------------------------------
uint16_t OldPackage = 0;
//����
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
          
          if(Local.Status == 3)  //״̬Ϊ����
            WatchEnd_Func();

          xtm_printf("RecvDownLoadFile_Func::  STARTDOWN  length = %d\n", length);
          for(i=0; i<MAX_DOWN_PACK; i++)
            Down_RecvFlag[i] = 0;           //���� ����־
          
          memcpy(send_b, recv_buf, length);
          send_b[7] = REPLY;    //Ӧ��
          sendlength = length;
          CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        break;
      case DOWN:              //����
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
            send_b[7] = REPLY;    //Ӧ��
            sendlength = length;
            
            CreateMultiUdpBuff(s_socket, SINGLE_SEND, RemoteIP1, RemotePort1, send_b, sendlength, 0);
          }
          else
          {
            send_b[7] = ASK;    //Ӧ��
            send_b[8] = DOWNFAIL;
            sendlength = length;
            CreateMultiUdpBuff(s_socket, SINGLE_SEND, RemoteIP1, RemotePort1, send_b, sendlength, 0);
            xtm_printf("send DOWNFAIL\n");
          }
        break;
      case DOWNFINISHONE:         //�������һ��
          memcpy(send_b, recv_buf, length);
          send_b[7] = REPLY;    //Ӧ��
          sendlength = length;
          CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
          xtm_printf("�������һ��\n");
        break;
      case STOPDOWN:             //ֹͣ����
          memcpy(send_b, recv_buf, length);
          send_b[7] = REPLY;    //Ӧ��
          sendlength = length;
          CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
          xtm_printf("ֹͣ����\n");
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
    if((DownData->FileType == 1)||(DownData->FileType == 2))   //ͼƬ  ����
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
    Down_RecvFlag[i] = 0;           //���� ����־
  
  if(PackOK)
    return 1;
  else
    return 0;
#endif
}
//---------------------------------------------------------------------------
//��Flash����
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
    send_b[7] = REPLY;    //Ӧ��
    sendlength = 10 + sizeof(struct downfile1) + ReadLen;
    ReadFromFlash(DownData.Faddr + (uint32_t)(DownData.CurrPackage) * ReadLen, send_b + 10 + sizeof(struct downfile1), ReadLen);
    //ReadFromFlash(DownData.Faddr + DownData.CurrPackage * ReadLen, send_b + 10 + sizeof(struct downfile1), ReadLen);
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
  }
#endif
}

//---------------------------------------------------------------------------
//Զ��������
void RecvRemoteReset_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  s32 wdt_sec=1; 
  uint16_t sendlength;
  memcpy(send_b, recv_buf, length);
  send_b[7] = REPLY;    //Ӧ��
  sendlength = length;
  UdpSendBuff(s_socket, cFromIP, DestPort, (char *)send_b, length);
  xtm_printf("DataPortDeal::Զ��������, %s\n", cFromIP);
  Local.GpioWatchDogStarted = 0;  //��GPIO����߳�������WatchDog
  
  //gm_wdt_init();
  //gm_wdt_ioctl(WDIOC_SETTIMEOUT,&wdt_sec);
}

//---------------------------------------------------------------------------
#ifdef _ADDTABLE_SUPPORT  //��ַ��֧��
//����ַ����Ϣ
void RecvReadAddrTable_Func(int s_socket, uint8_t *recv_buf, uint8_t *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint16_t sendlength;

  memcpy(send_b, recv_buf, length);
  send_b[7] = REPLY;    //Ӧ��
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
				case NSORDER:	//�����������������ڹ㲥��
				// case NSSERVERORDER:  //����������(NS������)
						switch(recvBuf1[7] & 0x03)
						{
							case 1://����
									if(recv_len >= SMALLESTSIZE)
									{
										xtm_printf("��������, recv_len = %d\n", recv_len);
										RecvNSAsk_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									}
									else
									{
										xtm_printf("�����������ݳ����쳣, recv_len = %d\n", recv_len);
									}
								break;
							case 2://������Ӧ
									if(recv_len >= SMALLESTSIZE)
									{
									  RecvNSReply_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									}
									else
									{
										xtm_printf("����Ӧ�����ݳ����쳣\n");
									}
								break;
						}
					break;
				case VIDEOTALK:    //���������ӶԽ�
						//xtm_printf("buff[6] = %d, buff[7] = %d, buff[8] = %d\n", buff[6], buff[7], buff[8]);
						switch(recvBuf1[8])	  //42 + 8
					  {
			        #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
							  case LEAVEGROUP:		  //�˳��鲥�飨���з�->���з������з�Ӧ��
									  if(recv_len >= SMALLESTSIZE)
										{
										  RecvTalkLeaveGroup_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
										}
										else
										{
											xtm_printf("�Խ��˳��鲥�����ݳ����쳣\n");
										}
									break;
			        #endif
							case CALL:		//�Է�����Խ�
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCall_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("�Խ��������ݳ����쳣\n");
									}
								break;
							case LINEUSE: 	   //�Է���æ
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkLineUse_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("ռ��Ӧ�����ݳ����쳣\n");
									}
								break;
							case CALLANSWER:	//����Ӧ��
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCallAnswer_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("�Խ�����Ӧ�����ݳ����쳣\n");
									}
									break;
							case CALLSTART:  //���з���ʼͨ��
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCallStart_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("�Խ���ʼͨ�����ݳ����쳣\n");
									}
								break;
							case CALLCONFIRM: 	//ͨ������ȷ��
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCallConfirm_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("�Խ�ͨ������ȷ�����ݳ����쳣\n");
									}
								break;
							case CALLEND:  //ͨ������
									//����������Ƶ
									//��Ϊ�Է���������Ӧ��
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCallEnd_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("�����Խ����ݳ����쳣\n");
									}
								break;
							case REMOTEOPENLOCK:	  //Զ�̿���
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkRemoteOpenLock_Func(*s_socket, recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("Զ�̿������ݳ����쳣\n");
									}
								break;
							case CENTERTURNTALK:	   //�����˹�ת��
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkCenterTurnTalk_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("�����˹�ת�����ݳ����쳣\n");
									}
								break;
							case TRUSTEESHIP_TALK:	//�й�
									if(recv_len >= SMALLESTSIZE)
									{
										RecvTalkTrusteeship_Talk_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("�й����ݳ����쳣\n");
									}
								break;
							case CALLUP: //ͨ������
							case CALLDOWN: //ͨ������
									RecvTalkCallUpDown_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
								break;
						}
					break;
				case VIDEOWATCH:	 //���������
						switch(recvBuf1[8])
						{
							case CALL:		//�Է��������	���ڻ�����Ӧ����
								break;
							case LINEUSE: 	   //�Է���æ
								  if(recv_len >= SMALLESTSIZE)
									{
										RecvWatchLineUse_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("ռ��Ӧ�����ݳ����쳣\n");
									}
								break;
							case CALLANSWER:	//����Ӧ��
									if(recv_len >= SMALLESTSIZE)
									{
										RecvWatchCallAnswer_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("���Ӻ���Ӧ�����ݳ����쳣\n");
									}
								break;
							case CALLCONFIRM: 	//ͨ������ȷ��
									if(recv_len >= SMALLESTSIZE)
									{
										RecvWatchCallConfirm_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("����ͨ������ȷ�����ݳ����쳣\n");
									}
								break;
							case CALLEND:  //ͨ������
									//����������Ƶ
									//��Ϊ�Է���������Ӧ��
									if(recv_len >= SMALLESTSIZE)
									{
										RecvWatchCallEnd_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
									}
									else
									{
										xtm_printf("�����������ݳ����쳣\n");
									}
								break;
							case CALLUP: //ͨ������
							case CALLDOWN: //ͨ������
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
							  xtm_printf("���ݿ������ݳ����쳣\n");
							}
						break;
        #endif
	      #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
					case HOSTTOSUB: //�����븱��ͨ��
						  switch(recvBuf1[8])
							{
							  case SYNCSUB: 	   //ͬ��״̬
										if(recv_len >= SMALLESTSIZE)
										{
										  RecvSyncSub_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort);
										}
										else
										{
											xtm_printf("ͬ��״̬���ݳ����쳣\n");
										}
									break;
							  case SUBDEFENCE:		 //��������������������
										RecvSubDefence_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							  case SUBALARM:		 //��������������������
										RecvSubAlarm_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							  case SUBALARMTIP: 	  //������ʾ����������������
										RecvSubAlarmTip_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							  case SUBDEFENCETIP:		//������ʾ����������������
										RecvSubDefenceTip_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							  case SUBFIND: 	  //���Ҹ�������������������
										RecvSubFind_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									break;
							}
						break;
	      #endif
        #ifdef _REMOTE_FORTIFY_SUPPORT  //Զ�̲�����֧��
					case REMOTE_FORTIFY:
							switch(recvBuf1[7] & 0x03)
							{
							  case 1://����
									  if(recv_len >= SMALLESTSIZE)
									  {
										  xtm_printf("Զ�̲���������, recv_len = %d\n", recv_len);
										  RecvRemoteFortify_Func(*s_socket, (uint8_t *)recvBuf1, FromIP, DestPort, recv_len);
									  }
									  else
									  {
										  xtm_printf("Զ�̲������������ݳ����쳣, recv_len = %d\n", recv_len);
									  }
									break;
							  case 2://Ӧ��
									  if(recv_len >= SMALLESTSIZE)
									  {
									    xtm_printf("Զ�̲���������Ӧ��, recv_len = %d\n", recv_len);
									  }
									  else
									  {
										  xtm_printf("Զ�̲�����Ӧ�����ݳ����쳣, recv_len = %d\n", recv_len);
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
        case PONSEARCHALLEQUIP:  //240  //���������豸���������ģ����豸��
            if(recv_len >= SMALLESTSIZE)
            {
              RecvPonSearchAllEquip_Func(recvBuf2, FromIP, DestIP, DestPort, recv_len);
            }
            else
            {
              xtm_printf("���������豸�����쳣\n");
            }
          break;
        case PONWRITEEQUIPADDR:  //241  //д�豸��ַ���������ģ����豸��
            if(recv_len >= SMALLESTSIZE)
            {
              RecvPonWriteEquipAddr_Func(recvBuf2, FromIP, DestIP, DestPort, recv_len);
            }
            else
            {
              xtm_printf("д�豸��ַ�����쳣\n");
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

      memcpy(PonSearch.OldMac, recv_buf + 10, 6); //ԭ������ַ
      memcpy(PonSearch.Mac_Addr, recv_buf + 10, 6); //������ַ
      memcpy(PonSearch.IP, recv_buf + 16, 4); //IP��ַ
      memcpy(PonSearch.IP_Mask, recv_buf + 20, 4); //��������
      memcpy(PonSearch.IP_Gate, recv_buf + 24, 4); //���ص�ַ

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
//д�豸��ַӦ��
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
      //����
      //xSemaphoreTake(multi_send_mutex, portMAX_DELAY);
      if((Multi_Udp_Buff[i].buf[6] == VIDEOWATCH)&&(Multi_Udp_Buff[i].buf[8] == CALL))
        xtm_printf("CreateMultiUdpBuff:: VIDEOWATCH i = %d\n", i);
      
      Multi_Udp_Buff[i].isValid = 1; //�Ƿ���Ч
      Multi_Udp_Buff[i].s_socket = s_socket;
      Multi_Udp_Buff[i].SendMode = SendMode; //ASK -- ��η���   REPLY --- ���η���
      
      if(Multi_Udp_Buff[i].SendMode == MULTI_SEND)
        Multi_Udp_Buff[i].MaxSendNum = 6; //��ǰ���ʹ���
      else
        Multi_Udp_Buff[i].MaxSendNum = 1; //��ǰ���ʹ���
     
      Multi_Udp_Buff[i].CurrOrder = CurrOrder;//��ǰ����״̬,VIDEOTALK  VIDEOWATCH
      //��Ҫ���������ʱ���絥��������Ϊ0
      Multi_Udp_Buff[i].DestPort = DestPort;

      strcpy(Multi_Udp_Buff[i].RemoteIP, RemoteIP);
      memcpy(Multi_Udp_Buff[i].buf, udpdata, nlength);
      Multi_Udp_Buff[i].SendNum = 0;
      Multi_Udp_Buff[i].TimeNum = 0;
      Multi_Udp_Buff[i].DelayTime = 600;  //�ȴ�ʱ��  200ms
      Multi_Udp_Buff[i].SendDelayTime = 0; //���͵ȴ�ʱ�����  20101112
  	  //������������͵ı�����С�ֽ�Ϊ512  
  	  if(DestPort == RemotePort[0])
   		  if(nlength < 512)
     		  nlength = 512;     
      
      Multi_Udp_Buff[i].nlength = nlength;

	    //����
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
      //�ж������Ƿ�ȫ�������꣬���ǣ��߳�����
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
void CheckMultiUdpBuff(void)   //���UDP���ͻ���
{
  uint8_t i;

  for(i=0; i<UDPSENDMAX; i++)
    if(Multi_Udp_Buff[i].isValid == 1)
    {
      //xtm_printf("CheckMultiUdpBuff::socket = %d, RemoteIP = %s:%d\n", Multi_Udp_Buff[i].s_socket, Multi_Udp_Buff[i].RemoteIP, Multi_Udp_Buff[i].DestPort);
       if(Multi_Udp_Buff[i].TimeNum == 0){
		   UdpSendBuff(Multi_Udp_Buff[i].s_socket, Multi_Udp_Buff[i].RemoteIP, Multi_Udp_Buff[i].DestPort, (char *)Multi_Udp_Buff[i].buf, Multi_Udp_Buff[i].nlength);
		   
		   if((Multi_Udp_Buff[i].buf[7] & 0x03) == REPLY)  //Ӧ�����ݰ��� ֻ��һ��
			{
			 Multi_Udp_Buff[i].isValid = 0;
			 continue;
			}
		   if(Multi_Udp_Buff[i].SendMode == SINGLE_SEND)  //ֻ��һ��
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
           //����
           //xSemaphoreTake(multi_send_mutex, portMAX_DELAY);
           MultiUdpSendFailDeal(i);
           //����
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
    #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
    case HOSTTOSUB: //�����븱��ͨ��
                 Multi_Udp_Buff[i].isValid = 0;
                 if(Multi_Udp_Buff[i].buf[8] == SYNCSUB)
                  {
                    Local.DenNum --;
                  }
                 xtm_printf("������ͨ��ʧ��, %d, %d, %d, %s\n", Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8],  Multi_Udp_Buff[i].buf[44] - '0', \
                            Multi_Udp_Buff[i].RemoteIP);
                 break;
    #endif
    case NSORDER:
               #ifdef ELEVATORCONTROL_HANGZHOU      //20120822
                if(Multi_Udp_Buff[i].CurrOrder == ELEVATR_ORDER1)
                 {
                  Multi_Udp_Buff[i].isValid = 0;
                  xtm_printf("���ݿ���, �����ſڻ�ʧ��\n");
                  break;
                 }
                #endif
                 if(Multi_Udp_Buff[i].CurrOrder == 255) //�����򸱻�����
                  {
                   Multi_Udp_Buff[i].isValid = 0;
                   xtm_printf("���Ҹ���ʧ��\n");
                  }
                 else
                  {
                   // Multi_Udp_Buff[i].isValid = 0;
                    xtm_printf("����ʧ��\n");

                   //��������Ϊ��������������һ������Ϊ�����������
                   Multi_Udp_Buff[i].SendNum = 0;
                   Multi_Udp_Buff[i].TimeNum = 0;
                   Multi_Udp_Buff[i].DelayTime = 600;  //�ȴ�ʱ��  200ms
                   Multi_Udp_Buff[i].SendDelayTime = 0; //���͵ȴ�ʱ�����  20101112
                   //����UDP�˿�
                   Multi_Udp_Buff[i].DestPort = LocalPort[0];
                  
                   sprintf(Multi_Udp_Buff[i].RemoteIP, "%d.%d.%d.%d", 
				   		LocalCfg.IP_Server[0], LocalCfg.IP_Server[1], LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
                   //����, ����������
                   Multi_Udp_Buff[i].buf[6] = NSSERVERORDER;
                   xtm_printf("������NS������������ַ, %s\n", Multi_Udp_Buff[i].RemoteIP);
                  }
                 break;
    case NSSERVERORDER: //����������
                 Multi_Udp_Buff[i].isValid = 0;
                 xtm_printf("����������ʧ��\n");
                 //���Ӵ���
                 if(Local.CurrentWindow == 16)
                  {
                   display_text(20, 200, 0, COLOR_YELLOW, "����ʧ��", 0, 24, NOFULLPAGE);
                   //��ʱ����ʾ��Ϣ��־
                   PicStatBuf.Type = Local.CurrentWindow;
                   PicStatBuf.Time = 0;
                   PicStatBuf.Flag = 1;
                  }
                 break;
    case VIDEOTALK:    //���������ӶԽ�
                 xtm_printf("VIDEOTALK\n");
                 switch(Multi_Udp_Buff[i].buf[8])
                  {
                   case JOINGROUP:        //�����鲥�飨���з�->���з������з�Ӧ��
                                          Multi_Udp_Buff[i].isValid = 0;
                                          xtm_printf("�����鲥��ʧ��, %d\n", Multi_Udp_Buff[i].buf[6]);
                                          break;
                   case CALL:
                                            //�����������������ĺ���
                                            for(k=0; k<UDPSENDMAX; k++)
                                             if(Multi_Udp_Buff[k].isValid == 1)
                                              if(Multi_Udp_Buff[k].buf[8] == CALL)
                                               if(k != i)
                                                 Multi_Udp_Buff[k].isValid = 0;

                                            Multi_Udp_Buff[i].isValid = 0;
                                            if(Local.Status == 0)
                                             {
                                              //��ʱ����ʾ��Ϣ��־
                                              PicStatBuf.Type = Local.CurrentWindow;
                                              PicStatBuf.Time = 0;
                                              PicStatBuf.Flag = 1;
                                              if(Local.CurrentWindow == 16)
                                               {
                                                 display_text(20, 200, 0, COLOR_YELLOW, "����ʧ��", 0, 24, NOFULLPAGE);
                                               } 
                                             }
                                          xtm_printf("����ʧ��, %d, %d, %s\n", Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8], Multi_Udp_Buff[i].RemoteIP);
                                          break;
                   case CALLEND:  //ͨ������
                   #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
                   case TURNTALK: //ת��
                   #endif
                                          Multi_Udp_Buff[i].isValid = 0;
                                          Local.OnlineFlag = 0;
                                          Local.CallConfirmFlag = 0; //�������߱�־
                                          if(Multi_Udp_Buff[i].buf[8] == TURNTALK)
                                            xtm_printf("ת��ʧ��, %d, %d, RemoteHost = %s\n", Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8], 
                                                   Multi_Udp_Buff[i].RemoteIP);
                                          //�Խ���������״̬�͹ر�����Ƶ
                                          TalkEnd_ClearStatus(0);
                                          break;
                   default: //Ϊ�����������ͨ�Ž���
                                          Multi_Udp_Buff[i].isValid = 0;
                                          xtm_printf("ͨ��ʧ��1, %d, %d, RemoteHost = %s\n", Multi_Udp_Buff[i].buf[6], Multi_Udp_Buff[i].buf[8], 
                                                   Multi_Udp_Buff[i].RemoteIP);
                                          break;
                  }
                 break;
    case VIDEOWATCH:     //���������
                           switch(Multi_Udp_Buff[i].buf[8])
                            {
                             case CALL:
                                            Multi_Udp_Buff[i].isValid = 0;
                                          //  Local.Status = 0;
                                            //��ʱ����ʾ��Ϣ��־
                                            PicStatBuf.Type = Local.CurrentWindow;
                                            PicStatBuf.Time = 0;
                                            PicStatBuf.Flag = 1;

                                            display_text(20, 200, 0, COLOR_YELLOW, "����ʧ��", 0, 24, NOFULLPAGE);
                                            /*if(Local.CurrentWindow == 13)
                                             {
                                               strcpy(Label_Call.Text, WatchFail_Text[Local.LanguageNo]);
                                               ShowOsdLabel24(&Label_Call, REFRESH, cWhite, 0);
                                             }     */
                                            xtm_printf("����ʧ��, %d, i = %d\n", Multi_Udp_Buff[i].buf[6], i);
                                          break;
                             case CALLEND:  //ͨ������
                                          Multi_Udp_Buff[i].isValid = 0;
                                          Local.OnlineFlag = 0;
                                          Local.CallConfirmFlag = 0; //�������߱�־

                                          WatchEnd_ClearStatus();
                                          break;
                             default: //Ϊ�����������ͨ�Ž���
                                          Multi_Udp_Buff[i].isValid = 0;
                                          xtm_printf("ͨ��ʧ��2, %d\n", Multi_Udp_Buff[i].buf[6]);
                                          break;
                            }
                           break;
    default: //Ϊ�����������ͨ�Ž���
                           Multi_Udp_Buff[i].isValid = 0;
                      //     Local.Status = 0;
                           if(Multi_Udp_Buff[i].buf[6] != REPORTSTATUS)
                             xtm_printf("ͨ��ʧ��3, %d\n", Multi_Udp_Buff[i].buf[6]);
                           break;
   }  
}
//---------------------------------------------------------------------------
uint8_t ClearMultiUdpBuff(uint8_t Order1, uint8_t Order2, uint8_t Order3, char *cFromIP)   //Clear UDP���ͻ���
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
//����Զ�̵�����Ϣ
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
    send_b[7] = REPLY;    //Ӧ��
    sendlength = length;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
    switch(recv_buf[8])
     {
      case STOPDEBUG:       //ֹͣ
             xtm_printf("�յ�Զ�̵�����Ϣ ֹͣ\n");
             Local.RemoteDebugInfo = 0;      //����Զ�̵�����Ϣ
             break;
      case STARTDEBUG:       //��ʼ
             Local.RemoteDebugInfo = 1;      //����Զ�̵�����Ϣ
             strcpy(Local.DebugIP, cFromIP);
             xtm_printf("�յ�Զ�̵�����Ϣ ��ʼ, %s, Local.DebugIP = %s\n", LocalCfg.Addr, Local.DebugIP);
             break;
      case REMOTE_REBOOT:       //������
             xtm_printf("�յ�Զ�̵�����Ϣ ������\n");
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
  if(Local.RemoteDebugInfo == 1)      //����Զ�̵�����Ϣ
   {
    memcpy(debug_sendb, UdpPackageHead, 6);
    debug_sendb[6] = REMOTEDEBUGINFO;
    debug_sendb[7] = ASK;    //����
    debug_sendb[8] = DEBUGDATA;    //������
    memcpy(debug_sendb + 9, FLAGTEXT, 20);       //��־
    infolen = strlen(debug_info);
    memcpy(debug_sendb + 29, &infolen, 4);
    memcpy(debug_sendb + 33, debug_info, infolen);
    sendlength = 33 + infolen;

    UdpSendBuff(s_socket, Local.DebugIP, RemotePort[0], debug_sendb, sendlength);
   }
#endif  
}
//---------------------------------------------------------------------------
#ifdef _REMOTECALLTEST  //Զ�̺��в���
//����Զ�̺��в��ԡ�
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
      //����LCD����
      Lcd_Power(1, LocalCfg.BrightVolume);
      Local.LcdLightFlag = 1; //LCD�����־
      Local.LcdLightTime = 0; //ʱ��
     }
        
    memcpy(RemoteAddr, recv_buf + 29 + 1, 19);
    RemoteAddr[19] = '\0';
    xtm_printf("����Զ�̺��в��� %s\n", RemoteAddr);
    if(Local.Status == 0)
     {
      Local.EnterTestMode = 1;  //����ģʽ, 5���Զ�����;
      Local.CloseTestRing = 1;  //����ģʽ, �ر�����
      switch(recv_buf[8])
       {
        case STARTTEST:
              CallR2R_Func(RemoteAddr);     //����ס��
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
void RecvElevatorControl_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)  //���ݿ���
{
  if((recv_buf[7] & 0x03) == REPLY)   //Ӧ��
   if(ClearMultiUdpBuff(ELEVATR_ORDER1, ASK, CALL, cFromIP))
    {
     xtm_printf("�յ����Ƶ���Ӧ��\n");
    }
}
#endif
//-----------------------------------------------------------------------
//����
void RecvNSAsk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)  //��������
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
  //���Ǳ�������
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
  //Ҫ��������Ǳ�����ַ
    if(isAddrOK == 1)
     {
      memcpy(send_b, recv_buf, 32);
      send_b[7] = Local.MachineType | REPLY;    //Ӧ��

      if(Local.isHost == '0')  //����
       {
        send_b[32] = Local.DenNum + 1;   //��ַ����

        memcpy(send_b + 33, LocalCfg.Addr, 20);
        memcpy(send_b + 53, LocalCfg.IP, 4);
        for(i=0; i<Local.DenNum; i++)
         {
          memcpy(send_b + 57 + 24*i, Local.DenAddr[i], 20);
          memcpy(send_b + 57 + 20 +24*i, Local.DenIP[i], 4);
         }
        sendlength = 57 + 24*Local.DenNum;
       }
      else                    //����
       {
        send_b[32] = 1;   //��ַ����

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
void RecvNSReply_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)//����Ӧ��
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
         //�ж�Ҫ�������ַ�Ƿ�ƥ��
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
   { //�յ���ȷ�Ľ�����Ӧ
       if(CurrOrder == VIDEOTALK) //�Խ�
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
                 xtm_printf("������ַ�ɹ�,���ڷ������, Remote.IP = %d.%d.%d.%d, Remote.Addr = %s\n", Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2], Remote.IP[j][3], 
                             Remote.Addr[j]);
                }
              }
        }

       if(CurrOrder == VIDEOWATCH) //����
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
           Order = Local.MachineType | ASK;     //����֧�ֵ���Ƶ��������;
           CreateTalkBuff(send_b, VIDEOWATCH, Order, CALL, Remote.Addr[0], Remote.IP[0]);

           sendlength = 62;

          /* Remote.IP[0][0] = 192;
           Remote.IP[0][1] = 168;
           Remote.IP[0][2] = 68;
           Remote.IP[0][3] = 98;  */
		   sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);	
           CreateMultiUdpBuff(s_socket, MULTI_SEND, RemoteIP, LocalPort[1], send_b, sendlength, 0);
           xtm_printf("������ַ�ɹ�,���ڷ������ %d.%d.%d.%d\n", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
        }
      #ifdef ELEVATORCONTROL_HANGZHOU      //20120822
       if(CurrOrder == ELEVATR_ORDER1) //���ݿ���
        {
                 memcpy(Door_Addr, recv_buf + 33, 20);
                 Door_IP[0] = recv_buf[53];
                 Door_IP[1] = recv_buf[54];
                 Door_IP[2] = recv_buf[55];
                 Door_IP[3] = recv_buf[56];

                 //ͷ��
                 memcpy(send_b, UdpPackageHead, 6);
                 //����  ,�����㲥����
                 send_b[6] = ELEVATR_ORDER1;
                 send_b[7] = ASK;    //����
                 send_b[8] = CALL;

                 memcpy(send_b+9, LocalCfg.Addr, 20);
                 memcpy(send_b+29, LocalCfg.IP, 4);
                 memcpy(send_b+33, Local.Elevator_Addr, 20);
                 memcpy(send_b+53, Local.Elevator_IP, 4);

                 sendlength = 57;
				 sprintf(RemoteIP, "%d.%d.%d.%d", Door_IP[0], Door_IP[1], Door_IP[2], Door_IP[3]);
                 CreateMultiUdpBuff(s_socket, MULTI_SEND, RemoteIP, LocalPort[1], send_b, sendlength, 0);
                 xtm_printf("������ַ�ɹ�,���ڷ��� ���Ƶ��� %d.%d.%d.%d\n", Door_IP[0], Door_IP[1], Door_IP[2], Door_IP[3]);
        }
      #endif        
  }
}
//---------------------------------------------------------------------------
void RecvTalkCall_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //�Խ�����
{
  uint8_t i, j;
  uint8_t isAddrOK;
  uint8_t sendlength;
  char str[50];
 // char IP_Group[15]; 

  //�Ѿ����ڽ���ͨ���л�����״̬
  if(PicStatBuf.Flag != 0)
   {
    xtm_printf("RecvTalkCall_Func::PicStatBuf.Flag = %d\n", PicStatBuf.Flag);
    return;
   }
  if(Local.KeyTouchNo != 0)
   {
    xtm_printf("RecvTalkCall_Func::Local.KeyTouchNo = %d, ���ڴ��������...\n", Local.KeyTouchNo);
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
  //�鿴Ŀ���ַ�Ƿ��Ǳ���
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

  //����״̬Ϊ����
  if((Local.Status == 3)||(Local.Status == 4))
   {
     Local.ForceEndWatch = 1;  //�к���ʱ��ǿ�ƹؼ���
     WatchEnd_Func();
     return;
   }

  //����״̬Ϊ����
  if(Local.Status == 0)
   {
    if(Local.PreStatus == 1)  //20130115 Ԥ״̬����ֹͬʱ�Ĳ���
      return;
    Local.PreStatus = 1;

    //20150430 Ų��ǰ�棬��������ʾ����ʱ�յ�̫��Ĺ㲥���³������
    memcpy(send_b, recv_buf, 62);

    //memcpy(Remote.DenIP, cFromIP, 4);
    //SearchArp_Table(Remote.DenIP, Remote.DenMac);
    //��ȡ�Է���ַ
    memcpy(Remote.Addr[0], recv_buf+9, 20);
    memcpy(Remote.IP[0], recv_buf+29, 4);
    Remote.DenNum = 1;
    xtm_printf("Remote.IP = %d.%d.%d.%d\n",   \
            Remote.IP[0][0],Remote.IP[0][1],Remote.IP[0][2],Remote.IP[0][3]);

    CheckRemoteAddr();  //���Զ�˵�ַ

 /*   if(recv_buf[57] == 1)
     {
      //�鿴�Ƿ��������鲥����
      DropMultiGroup(m_VideoSocket, NULL);

      Local.IP_Group[0] = recv_buf[58]; //�鲥��ַ
      Local.IP_Group[1] = recv_buf[59];
      Local.IP_Group[2] = recv_buf[60];
      Local.IP_Group[3] = recv_buf[61];
      sprintf(IP_Group, "%d.%d.%d.%d\0",
                     Local.IP_Group[0],Local.IP_Group[1],Local.IP_Group[2],Local.IP_Group[3]);
      AddMultiGroup(m_VideoSocket, IP_Group);
     }         */
    FirstFrameFlag = 1; //��һ֡���ж�jpeg header
    FirstFrameFinished = 0;
	ResetMjpegHeader();
			
    Local.TimeOut = 0;  
    Local.Status = 2;  //״̬Ϊ���Խ�
    Local.TalkEndStatus = 1;
 	//xtm_printf("RecvTalkCall_Func::3.2.1, mjpeg_stream = 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", 
	// mjpeg_stream[0][0], mjpeg_stream[0][1], mjpeg_stream[0][2], mjpeg_stream[0][3],
	// mjpeg_stream[1][0], mjpeg_stream[1][1], mjpeg_stream[1][2], mjpeg_stream[1][3]);

    //��ʾ�Խ�����
    DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, TALKWINTYPE);

    Local.DecodeFrameSize = VIDEO_VGA;
    xtm_printf("Local.DecodeFrameSize = %d\n", Local.DecodeFrameSize);
 	//xtm_printf("RecvTalkCall_Func::3.2.2, mjpeg_stream = 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", 
	// mjpeg_stream[0][0], mjpeg_stream[0][1], mjpeg_stream[0][2], mjpeg_stream[0][3],
	// mjpeg_stream[1][0], mjpeg_stream[1][1], mjpeg_stream[1][2], mjpeg_stream[1][3]);    
    //��ʼ������Ƶ
    Local.DecodeVideoWidth = 640;
    Local.DecodeVideoHeight = 480;
    
   // Local.VideoWindowFlag = 0;  //��Ƶ���ڴ򿪱�־
    if((SCREEN_WIDTH == 800)&&(SCREEN_HEIGHT == 480))
      DisplayVideoWindow(0, 640, 0, 480, REMOTE_VIDEO);
    if((SCREEN_WIDTH == 480)&&(SCREEN_HEIGHT == 272))
      DisplayVideoWindow(0, 480, 0, 272, REMOTE_VIDEO);    

    CreateAudioSend_Stream();      //����Ƶ����Stream

    Local.RecordPic = 1;   //��������Ƭ
    Local.IFrameCount = 0; //I֡����
    Local.HavePicRecorded = 0;
    Local.ManualRecordFlag = 0; //�ֶ�¼��

    TmpRecordsContent.HavePic = 0;  //����ͼ���־  0   1 -- 640x480  2 -- 320x240
    strcpy(TmpRecordsContent.RemoteAddr, Remote.Addr[0]);
    TmpRecordsContent.Type = 1;     //       0 -- ���С���1 -- ����δ����  2 -- ���н���
    TmpRecordsContent.Length = 0;       //����

   #ifdef _REMOTECALLTEST  //Զ�̺��в���
    if(Local.CloseTestRing == 0)  //����ģʽ, �ر�����
   #endif
     if(Local.NoBreak == 0)  //����״̬ 1 ����  0 ����
      {
       StopPlayWavFile();
       StartPlayWav(ring_wav.Faddr, CYCLE_PLAY);
      }
	 xtm_printf("RecvTalkCall_Func:: 3\n");
   #ifdef _REMOTECALLTEST  //Զ�̺��в���
    Local.CloseTestRing = 0;  //����ģʽ, �ر�����
   #endif

    Local.CallConfirmFlag = 1; //�������߱�־
    Local.Timer1Num = 0;
    Local.TimeOut = 0;       //���ӳ�ʱ,  ͨ����ʱ,  ���г�ʱ�����˽���
    Local.OnlineNum = 0;     //����ȷ�����
    Local.OnlineFlag = 1;

    strcpy(str, Remote.Addr[0]);
    if(Remote.Addr[0][0] == 'B')
     {
      str[5] = Remote.Addr[0][11];
      str[6] = '\0';
     }

    strcat(str, "  ֱͨ�Խ�����");
    xtm_printf("%s\n", str);

    //��ɴ�����ٸ�Ӧ��
    //memcpy(send_b, recv_buf, 62);
    send_b[7] = Local.MachineType | ASK;    //����
    send_b[8] = CALLANSWER;//�Խ�Ӧ��
    sendlength = 62;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

    //time(&t);
    //Local.call_tm_t = localtime(&t);
    memcpy(Local.RemoteAddr, Remote.Addr[0], 20);
    Local.RemoteAddr[12] = '\0';    		
   }
  //����Ϊæ
  else
   {
    if((Remote.IP[0][0] != cFromIP[0])||(Remote.IP[0][1] != cFromIP[1])||(Remote.IP[0][2] != cFromIP[2])||(Remote.IP[0][3] != cFromIP[3]))
     {
      memcpy(send_b, recv_buf, 57);
      send_b[7]=Local.MachineType | ASK;    //����
      send_b[8]=LINEUSE;//ռ��Ӧ��
      sendlength=57;
      CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
      xtm_printf("����ռ��Ӧ��\m");
     } 

    xtm_printf("�Է�����Խ�����, ����ռ��, %s\n", cFromIP);
   }
}
//---------------------------------------------------------------------------
void CheckRemoteAddr(void)  //���Զ�˵�ַ
{
    switch(Remote.Addr[0][0])
     {
      case 'Z':  //���Ļ�����
      case 'W':  //Χǽ������
               Remote.Addr[0][5] = '\0';
               break;
      case 'M':  //��Ԫ�ſڻ�����
               Remote.Addr[0][8] = '\0';
               break;
      case 'S':  //���ڻ�����
      case 'B':  //���������ڻ�����
      case 'H':  //�����ſڻ�����
               Remote.Addr[0][12] = '\0';
               break;
     }
}
//-----------------------------------------------------------------------
void RecvTalkCallAnswer_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //�Խ�����Ӧ��
{
  uint8_t j;
  if((recv_buf[7] & 0x03) == ASK)   //Ӧ��
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
            xtm_printf("�յ��Խ�����Ӧ��, %s\n", cFromIP);
            if(Local.Status == 1)    //״̬Ϊ���жԽ�
              return;                
                
            if(Local.Status == 0)
              Local.Status = 1;  //״̬Ϊ���жԽ�
            Local.TalkEndStatus = 1;
            Local.CallConfirmFlag = 1; //�������߱�־
            Local.Timer1Num = 0;
            Local.TimeOut = 0;       //���ӳ�ʱ,  ͨ����ʱ,  ���г�ʱ�����˽���
            Local.OnlineNum = 0;     //����ȷ�����
            Local.OnlineFlag = 1;
            xtm_printf("Local.Status = %d\n", Local.Status);

            LocalCfg.EncodeFrameSize = VIDEO_VGA; //�����С

            //��ʼ¼����Ƶ
            //StartRecVideo(LocalCfg.EncodeFrameSize);
                        
            CreateAudioSend_Stream();      //����Ƶ����Stream


            StopPlayWavFile();
            WaitAudioUnuse(200); //�ȴ���������			
            StartPlayWav(backring_wav.Faddr, CYCLE_PLAY);
           }   
}
//-----------------------------------------------------------------------
void RecvTalkLineUse_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //�Խ�ռ��Ӧ��
{
  int isUse;
  int CallNextCenter = 0;
  char CenterAddr[20];


  if((recv_buf[7] & 0x03) == ASK)   //Ӧ��
   if(ClearMultiUdpBuff(VIDEOTALK, ASK, CALL, cFromIP))
    {
            isUse = 1;
            #ifdef _MULTI_CENTER_SUPPORT  //�����Ļ�֧��
             if(Remote.Addr[0][0] == 'Z')
              {
               xtm_printf("���� %s ռ��\n", Remote.Addr[0]);

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
             //20101202 xu �˴����������к��ж�ֻ�����������У������ֻ������޷��˳�, ���ӡ�ռ������
             if(Remote.DenNum == Remote.LineUseNum)
              {
              /* if(Local.CurrentWindow == 16)
                {
                 strcpy(Label_Call.Text, InUse_Text[Local.LanguageNo]);
                 ShowOsdLabel24(&Label_Call, REFRESH, cWhite, 0);
                }         */
               display_text(20, 200, 0, COLOR_YELLOW, "�Է���æ", 0, 24, NOFULLPAGE); 
               //��ʱ����ʾ��Ϣ��־
               PicStatBuf.Type = 16;
               PicStatBuf.Time = 0;
               PicStatBuf.Flag = 1;
              }
            xtm_printf("�յ��Խ�ռ��Ӧ��, %s\n", cFromIP);
    }

  #ifdef _MULTI_CENTER_SUPPORT  //�����Ļ�֧��
   if(CallNextCenter == 1)
    {
     xtm_printf("�Զ����� %s\n", Remote.Addr[0]);

     CallCenter_Func(CenterAddr);
    }
  #endif
}
//-----------------------------------------------------------------------
void RecvTalkCallConfirm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort) //�Խ�����ȷ��
{
  uint8_t sendlength;
  //����Ϊ���з�
  if(((Local.Status == 1)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
     &&((recv_buf[7] & 0x03) == ASK))
   {
    memcpy(send_b, recv_buf, 61);
    send_b[7] = Local.MachineType | REPLY;    //Ӧ��
    sendlength = 61;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
    Local.CallConfirmFlag = 1;

    //xtm_printf("�յ��Խ�����ȷ��\n");

    //������ͨ���У����յ���Ϊͨ����������ȷ�ϣ����˳��鲥������
    #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
     if(Local.Status == 5)
       if((Remote.IP[0][0] != recv_buf[53])||(Remote.IP[0][1] != recv_buf[54])||(Remote.IP[0][2] != recv_buf[55])||(Remote.IP[0][3] != recv_buf[56]))
         SingleExitGroup(s_socket, recv_buf); //�򵥸����з��˳��鲥������
    #endif
   }
  else  //����Ϊ���з�
   if(((Local.Status == 2)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
      &&((recv_buf[7] & 0x03) == REPLY))
   {
    Local.CallConfirmFlag = 1;

    //xtm_printf("�յ��Է�Ӧ�𱾻��Խ�����ȷ��\n");
   }
}
//-----------------------------------------------------------------------
void RecvTalkCallStart_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //�Խ���ʼͨ��
{
  uint8_t isAddrOK;
  uint8_t sendlength;

  Local.TalkTimeOut = TALKTIMEOUT; //ͨ���ʱ��
  //����Ϊ���з� Ӧ��
  if((Local.Status == 1)&&((recv_buf[7] & 0x03) == ASK))
   {
    memcpy(send_b, recv_buf, 57);
    send_b[7]=Local.MachineType | REPLY;    //Ӧ��
    sendlength=57;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

    #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
     ExitGroup(s_socket, recv_buf);      //���������з��˳��鲥������
    #endif

    //��ȡ���з���ַ
    memcpy(Remote.Addr[0], recv_buf+33, 20);
    memcpy(Remote.IP[0], recv_buf+53, 4);
    Remote.DenNum = 1;
    xtm_printf("RecvTalkCallStart_Func::Remote.Addr[0] = %s, Remote.IP[0] = %d.%d.%d.%d\n", Remote.Addr[0], Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);

    //memcpy(Remote.DenIP, cFromIP, 4);

    StopPlayWavFile();  //�ر�����
    WaitAudioUnuse(1000); //��ʱ��Ϊ�˵ȴ������������

    //����Ƶ¼�ơ����ţ���Ƶ����
    //#ifdef _USBCAMERA_SUPPORT     //USB Camera֧��
    //  StartPlayVideo(CIF_W, CIF_H, WINDOWMODE);
    //#endif
    
   // Local.VideoWindowFlag = 0;  //��Ƶ���ڴ򿪱�־
    //��ʼ������Ƶ
    if(Local.DecodeFrameSize == VIDEO_VGA) //�����С
     {
      Local.DecodeVideoWidth = 640;
      Local.DecodeVideoHeight = 480;
      DisplayVideoWindow(0, 640, 0, 480, REMOTE_VIDEO);
     }
    if(Local.DecodeFrameSize == VIDEO_QVGA) //�����С
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

    Local.Status = 5;  //״̬Ϊ����ͨ��
    Local.TalkEndStatus = 1;
    Local.TimeOut = 0;       //���ӳ�ʱ,  ͨ����ʱ,  ���г�ʱ�����˽���

    xtm_printf("�Է���ʼͨ��, %s\n", cFromIP);
   }
  else  //����Ϊ���з� ����
   if(Local.Status == 2)
   {
    //xtm_printf("RecvTalkCallStart_Func::�Է�Ӧ�𱾻���ʼͨ��, %s\n", cFromIP);
    //��������
    if((recv_buf[7] & 0x03) == REPLY)
     if(ClearMultiUdpBuff(VIDEOTALK, ASK, CALLSTART, cFromIP))
      {
              //�鿴�Ƿ��������鲥����
              //DropMultiGroup(m_VideoSocket, NULL);

			  StopPlayWavFile();  //�ر�����
printf("RecvTalkCallStart_Func::1\n");
			  WaitAudioUnuse(1000); //��ʱ��Ϊ�˵ȴ������������

              //����Ƶ¼�ơ����ţ���Ƶ¼��
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
            
              Local.Status = 6;  //״̬Ϊ����ͨ��
              Local.TalkEndStatus = 1;
              Local.TimeOut = 0;       //���ӳ�ʱ,  ͨ����ʱ,  ���г�ʱ�����˽���
              CheckRemoteAddr();  //���Զ�˵�ַ
              Local.RecordPic = 2;   //ͨ������Ƭ
              Local.IFrameCount = 0; //I֡����
              Local.HavePicRecorded = 0;
              Local.ManualRecordFlag = 0; //�ֶ�¼��
printf("RecvTalkCallStart_Func::3\n");

              TmpRecordsContent.Type = 2;     //       0 -- ���С���1 -- ����δ����  2 -- ���н���

              xtm_printf("�Է�Ӧ�𱾻���ʼͨ��, %s\n", cFromIP);

             }
   }

  if((Local.Status == 5)||(Local.Status == 6))
   {
   }     
}

//-----------------------------------------------------------------------

void RecvTalkCallEnd_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //�Խ����н���
{
  uint8_t isAddrOK;
  uint8_t sendlength;
  //��������
  if(((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6) \
     ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10)) \
     &&((recv_buf[7] & 0x03) == ASK))
   {
    isAddrOK = 0;

    //����Ϊ���з�
    if((Local.Status == 1)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
      if((recv_buf[53] == Remote.IP[0][0]) && (recv_buf[54] == Remote.IP[0][1]) && (recv_buf[55] == Remote.IP[0][2]) &&(recv_buf[56] == Remote.IP[0][3]))
        isAddrOK = 1;

    //����Ϊ���з�
    if((Local.Status == 2)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
      if((recv_buf[29] == Remote.IP[0][0]) && (recv_buf[30] == Remote.IP[0][1]) && (recv_buf[31] == Remote.IP[0][2]) &&(recv_buf[32] == Remote.IP[0][3]))
        isAddrOK = 1;

     if(isAddrOK == 1)
      {
       Local.OnlineFlag = 0;
       Local.CallConfirmFlag = 0; //�������߱�־

       memcpy(send_b, recv_buf, 57);
       send_b[7]=Local.MachineType | REPLY;    //Ӧ��
       sendlength=57;

       //UdpSendBuff(s_socket, cFromIP, DestPort, (char *)send_b, sendlength);
       CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, VIDEOTALK);

       #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
        ExitGroup(s_socket, recv_buf);      //���������з��˳��鲥������
       #endif
       TalkEnd_ClearStatus(0);

       xtm_printf("�Է������Խ�, %s:%d, s_socket = %d, Remote.DenNum = %d\n", cFromIP, DestPort, s_socket, Remote.DenNum);
      }

   }
  else  //��������
   if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
     ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))   
   {
    Local.OnlineFlag = 0;
    Local.CallConfirmFlag = 0; //�������߱�־

    //��������
    if((recv_buf[7]& 0x03) == REPLY)
     if(ClearMultiUdpBuff(VIDEOTALK, ASK, CALLEND, cFromIP))
      {
              TalkEnd_ClearStatus(0);

              if(Local.CurrentWindow == 16)
               {
               }
              xtm_printf("�Է�Ӧ�𱾻������Խ�\n");
      }
   }
}

//-----------------------------------------------------------------------
void TalkEnd_ClearStatus(int isTurn) //�Խ���������״̬�͹ر�����Ƶ
{
  uint8_t buf[10];
  if(isTurn == 1)
   {
    Local.Status = 0;  //״̬Ϊ����

    StopRecAudio();
    StopPlayAudio();
    return;
   }

  if(Local.TalkEndStatus == 1)
   {
    Local.TalkEndStatus = 0;
    Local.OnlineFlag = 0;
    Local.CallConfirmFlag = 0; //�������߱�־
    xtm_printf("TalkEnd_ClearStatus Local.Status=%d\n", Local.Status);

    //�鿴�Ƿ��������鲥����
    //DropMultiGroup(m_VideoSocket, NULL);
    switch(Remote.Addr[0][0])
     {
      case 'Z':  //���Ļ�����
      case 'W':  //Χǽ������
               Remote.Addr[0][5] = '\0';
               break;
      case 'H':  //�����ſڻ�����
               Remote.Addr[0][6] = '\0';
               break;
      case 'M':  //��Ԫ�ſڻ�����
               Remote.Addr[0][8] = '\0';
                        break;
      case 'S':  //���ڻ�����
      case 'B':  //���������ڻ�����      
               Remote.Addr[0][12] = '\0';
               break;
     }
#if 1
    StopPlayWavFile();  //�ر�����
    AudioTalkOpenFlag = 0;
    switch(Local.Status)
     {
      case 1: //��������
             /*WaitAudioUnuse(1000); //��ʱ��Ϊ�˵ȴ�����������ɣ������������
             StopPlayVideo();     */
             //StopRecVideo();
             break;
      case 2: //��������
             //д��Ƭ����
             WritePicFunc();
             break;
      case 5: //��������ͨ��

             //StopRecVideo();

             StopRecAudio();
             StopPlayAudio();
             break;
      case 6: //��������ͨ��

            // StopRecVideo();
             
             StopRecAudio();


             StopPlayAudio();



             //д��Ƭ����
             WritePicFunc();

             break;
     }

    if(Local.CurrentWindow != 16)  //����������
      Local.CurrentWindow = 16;
    CloseVideoWindow();
    //��ʱ����ʾ��Ϣ��־
    PicStatBuf.Type = Local.CurrentWindow;
    PicStatBuf.Time = 0;
    PicStatBuf.Flag = 1;

//    SoundSetSpeakerVolume(LocalCfg.SpkVolume); //��������
#endif
    //Local.Status = 0;
   }
  else
    xtm_printf("TalkEnd_ClearStatus �ظ�\n"); 
}
//-----------------------------------------------------------------------
void RecvTalkRemoteOpenLock_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //Զ�̿���
{
  if((Local.Status == 2)||(Local.Status == 6))  //״̬Ϊ���Խ� �򱻽�ͨ��
  //if(Local.Status == 6)  //״̬Ϊ���Խ�
   {
    //��������
    if((recv_buf[7]& 0x03) == REPLY)
     if(ClearMultiUdpBuff(VIDEOTALK, ASK, REMOTEOPENLOCK, cFromIP))
      {
              #ifdef _ZHUHAIJINZHEN      //�麣����  ����ʱ�ɿ���  ͨ��ʱ����2���Ҷ�
              if(Local.Status == 6)  //״̬Ϊ����ͨ��
               {
                if((Local.TimeOut + 2*(1000/INTRTIME)) < TALKTIMEOUT)
                  Local.TalkTimeOut = Local.TimeOut + 2*(1000/INTRTIME);
                else
                  Local.TalkTimeOut = TALKTIMEOUT;
               }
              #endif
			  Local.SendLockFlag = 1;    //20180608 ͨ�������п���
              xtm_printf("�Է�Ӧ�𱾻�Զ�̿���\n");
      }     
   }
}
//-----------------------------------------------------------------------
void RecvTalkCenterTurnTalk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //�����˹�ת��
{
  uint8_t j;
  uint8_t isAddrOK;
  uint8_t sendlength;
  uint8_t DestAddr[21], DestAddr1[21];

  //�鿴Ŀ���ַ�Ƿ��Ǳ���
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
    //��ɴ�����ٸ�Ӧ��
    memcpy(send_b, recv_buf, 81);
    send_b[7] = Local.MachineType | REPLY;    //����
    sendlength = 81;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

    TalkEnd_ClearStatus(1);            //����Ϊ���з������ضԽ���ֻ����Ƶ ����Ƶ����

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
void RecvTalkTrusteeship_Talk_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //�й�
{
  uint16_t sendlength;
  char RemoteIP[20];
  if((recv_buf[7] & 0x03) == ASK)   //Ӧ��
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
        xtm_printf("�й�, ������æ,�޷�����\n");
       }
    }  
}
//-----------------------------------------------------------------------
void RecvTalkCallUpDown_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)  //�Խ�����
{
  //�Ѿ����ڽ���ͨ���л�����״̬
  if(PicStatBuf.Flag != 0)
   {
    //xtm_printf("RecvTalkCallUpDown_Func::PicStatBuf.Flag = %d\n", PicStatBuf.Flag);
    return;
   }
  //xtm_printf("recv_buf[6] = %d, recv_buf[7] = %d, recv_buf[8] = %d, recv_buf[61] = %d\n", recv_buf[6], recv_buf[7], recv_buf[8], recv_buf[61]);
  if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
     ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))  //״̬Ϊ�Խ�
   switch(recv_buf[61])
   {
    case 1://��Ƶ
           //if((cFromIP[0] != Remote.IP[0][0])||(cFromIP[1] != Remote.IP[0][1])||(cFromIP[2] != Remote.IP[0][2])||(cFromIP[3] != Remote.IP[0][3]))
           // {
           //  break;
           // }    
           AudioDeal_Func(s_socket, recv_buf, cFromIP, length);  //��Ƶ���ݴ���
           break;
    case 2://��Ƶ  I֡  352*288
    case 3://��Ƶ  P֡  352*288
    case 4://��Ƶ  I֡  720*480
    case 5://��Ƶ  P֡  720*480
           VideoDeal_Func(s_socket, recv_buf, cFromIP, length);
           break;
  }
}
//-----------------------------------------------------------------------
//����
void RecvWatchCallAnswer_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //���Ӻ���Ӧ��
{
  uint8_t i;
  if((recv_buf[7] & 0x03) == ASK)   //Ӧ��
   if(ClearMultiUdpBuff(VIDEOWATCH, ASK, CALL, cFromIP))
    {
      //memcpy(Remote.DenIP, cFromIP, 4);
 
      FirstFrameFlag = 1; //��һ֡���ж�jpeg header
      FirstFrameFinished = 0;
	  ResetMjpegHeader();	  

      Local.DecodeFrameSize = VIDEO_VGA;

      //��ʼ������Ƶ
      Local.DecodeVideoWidth = 640;
      Local.DecodeVideoHeight = 480;

    //  Local.VideoWindowFlag = 0;  //��Ƶ���ڴ򿪱�־
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

       Local.Status = 3;  //״̬Ϊ����
       Local.CallConfirmFlag = 1; //�������߱�־
       Local.Timer1Num = 0;
       Local.TimeOut = 0;       //���ӳ�ʱ,  ͨ����ʱ,  ���г�ʱ�����˽���
       Local.OnlineNum = 0;     //����ȷ�����
       Local.OnlineFlag = 1;
       xtm_printf("Local.Status = %d\n", Local.Status);

       xtm_printf("�յ����Ӻ���Ӧ��\n");
    }
}
//-----------------------------------------------------------------------
void RecvWatchLineUse_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //����ռ��Ӧ��
{
  if((recv_buf[7] & 0x03) == ASK)   //Ӧ��
   if(ClearMultiUdpBuff(VIDEOWATCH, ASK, CALL, cFromIP))
    {
     //��ʱ����ʾ��Ϣ��־
     PicStatBuf.Type = 16;
     PicStatBuf.Time = 0;
     PicStatBuf.Flag = 1;

     display_text(20, 200, 0, COLOR_YELLOW, "�Է���æ", 0, 24, NOFULLPAGE);
     xtm_printf("�յ�����ռ��Ӧ��, %s\n", cFromIP);
    }   
}
//-----------------------------------------------------------------------
void RecvWatchCallConfirm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort) //��������ȷ��
{
  uint8_t sendlength;
  //����Ϊ���з�
  if(((Local.Status == 3)||(Local.Status == 4))&&((recv_buf[7] & 0x03) == ASK))
   {
    memcpy(send_b, recv_buf, 61);
    send_b[7] = Local.MachineType | REPLY;    //Ӧ��
    sendlength = 61;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
    Local.CallConfirmFlag = 1;

    //xtm_printf("�յ���������ȷ��\n");

   }
  else  //����Ϊ���з�
   if(Local.Status == 3)
    {
     Local.CallConfirmFlag = 1;

     //xtm_printf("�յ��Է�Ӧ�𱾻���������ȷ��\n");
    }
}
//-----------------------------------------------------------------------
void RecvWatchCallEnd_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //���Ӻ��н���
{
  uint8_t sendlength;

  //��������
  if(((Local.Status == 3)||(Local.Status == 4))&&((recv_buf[7] & 0x03) == ASK))
   {
       Local.OnlineFlag = 0;
       Local.CallConfirmFlag = 0; //�������߱�־

       memcpy(send_b, recv_buf, 57);
       send_b[7]=Local.MachineType | REPLY;    //Ӧ��
       sendlength=57;
       CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

       WatchEnd_ClearStatus();

       xtm_printf("�Է���������, %s\n", cFromIP);
   }
  else  //��������
   if(Local.Status == 3)
   {
    Local.OnlineFlag = 0;
    Local.CallConfirmFlag = 0; //�������߱�־
    //��������
    if((recv_buf[7]& 0x03) == REPLY)
     if(ClearMultiUdpBuff(VIDEOWATCH, ASK, CALLEND, cFromIP))
      {
              WatchEnd_ClearStatus();

              xtm_printf("�Է�Ӧ�𱾻���������\n");
      }
   }
}
//-----------------------------------------------------------------------
void RecvWatchCallUpDown_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)  //��������
{
  if((Local.Status == 3)||(Local.Status == 4))  //״̬Ϊ����
   switch(recv_buf[61])
   {
    case 2://��Ƶ  I֡  352*288
    case 3://��Ƶ  P֡  352*288
    case 4://��Ƶ  I֡  720*480
    case 5://��Ƶ  P֡  720*480
           VideoDeal_Func(s_socket, recv_buf, cFromIP, length);
           break;
  }
}
//-----------------------------------------------------------------------
//���ӽ�������״̬�͹ر�����Ƶ
void WatchEnd_ClearStatus(void)
{
    switch(Local.Status)
     {
      case 3: //��������
             if(Local.ForceEndWatch == 0)  //�к���ʱ��ǿ�ƹؼ���
              {
               //��ʱ����ʾ��Ϣ��־
               PicStatBuf.Type = 16;
               PicStatBuf.Time = 0;
               PicStatBuf.Flag = 1;
              }
             else
              {
              // ShowFirstWindow();
               Local.Status = 0;  //״̬Ϊ����
               Local.ForceEndWatch = 0;  //�к���ʱ��ǿ�ƹؼ���
               Local.PreStatus = 0;
              }
             CloseVideoWindow();
             break;
      case 4: //����������
             Local.Status = 0;  //״̬Ϊ����
             break;
     }
    if(Local.CurrentWindow == 13)
     {
      //��ʱ����ʾ��Ϣ��־
      PicStatBuf.Type = 13;
      PicStatBuf.Time = 0;
      PicStatBuf.Flag = 1;
     }
}
//-----------------------------------------------------------------------
uint16_t old_audio_Frameno = 0;

void AudioDeal_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t length)  //��Ƶ���ݴ���
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
        
    //֡���
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
void VideoDeal_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t length)  //��Ƶ���ݴ���
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
    //֡���
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
		if(FirstFrameFlag == 1) //��һ֡���ж�jpeg header
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
    //���յ��µ�һ֡���˴���һ�£��������
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
      video_decode_no = talk_Frameno;  //��Ƶ�������
      old_mjpeg_stream_no = Curr_Mjpeg_Stream;  //��Ƶ�������
      Video_Maddr = mjpeg_stream[Curr_Mjpeg_Stream];
      Video_Len = talk_Framelen + mjpeg_stream_offset;
	  if(mjpeg_stream_offset == 0x26f){
	  	Video_Len += 2;
	  	mjpeg_stream[Curr_Mjpeg_Stream][Video_Len - 2] = 0xff;
		mjpeg_stream[Curr_Mjpeg_Stream][Video_Len - 1] = 0xd9;
	  }
      mjpeg_stream_len[Curr_Mjpeg_Stream] = Video_Len;


      if((Local.Status == 2)||(Local.Status == 6))
       if((Local.HavePicRecorded == 0)||(Local.ManualRecordFlag == 1)) //�ֶ�¼��  //����Ƭ��¼��
        {
         Local.IFrameCount ++;
         if(Local.IFrameCount > 30)
          {
           //��jpeg ͷ
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
           Local.HavePicRecorded = 1;  //����Ƭ��¼��
           Local.ManualRecordFlag = 0; //�ֶ�¼��
           TmpRecordsContent.HavePic = 0;
           if(Local.DecodeFrameSize == VIDEO_VGA) //�����С
             TmpRecordsContent.HavePic = 1;  //����ͼ���־  0   1 -- 640x480  2 -- 320x240
           if(Local.DecodeFrameSize == VIDEO_QVGA) //�����С
             TmpRecordsContent.HavePic = 2;  //����ͼ���־  0   1 -- 640x480  2 -- 320x240
           
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
#ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
//�˳��鲥�飨���з�->���з������з�Ӧ��
void RecvTalkLeaveGroup_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)
{
  uint16_t sendlength;

  xtm_printf("recv exit group, %s\n", cFromIP);
  //����Ϊ���з� ���жԽ�
  if((Local.Status == 2)&&((recv_buf[7] & 0x03) == ASK))
   {
    memcpy(send_b, recv_buf, 61);
    send_b[7] = Local.MachineType | REPLY;    //Ӧ��
    sendlength = 61;
    CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

    //printf("TalkEnd_ClearStatus 111\n");
    TalkEnd_ClearStatus(0);
    xtm_printf("�Է�Ҫ���뿪�鲥��, %s\n", cFromIP);
   }
  else  //����Ϊ���з� ����
   if((recv_buf[7] & 0x03) == REPLY)
   {
    //��������
    if(ClearMultiUdpBuff(VIDEOTALK, ASK, LEAVEGROUP, cFromIP))
     {
       xtm_printf("�Է�Ӧ�𱾻��뿪�鲥��, %s\n", cFromIP);
     }
   }
}
//-----------------------------------------------------------------------
void ExitGroup(int s_socket, uint8_t *buf)      //���������з��˳��鲥������
{
  uint8_t i,j;
  uint8_t isIP;
  uint16_t sendlength;
  char RemoteIP[20];
  //�˳��鲥��
  if(Remote.DenNum > 1)
   {
     for(j=0; j<Remote.DenNum; j++)
      {
       Remote.Added[j] = 0;
       xtm_printf("%d.%d.%d.%d  %d.%d.%d.%d\n",Remote.IP[j][0],Remote.IP[j][1],Remote.IP[j][2],Remote.IP[j][3],
              buf[53],buf[54],buf[55],buf[56]);
       //��Ϊ��ʼͨ������һ��
       if((Remote.IP[j][0] == buf[53])&&(Remote.IP[j][1] == buf[54])
        &&(Remote.IP[j][2] == buf[55])&&(Remote.IP[j][3] == buf[56]))
         isIP = 1;
       else
         isIP = 0;

	   sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2], Remote.IP[j][3]);
       //����Ƿ��Ѿ��ڷ��ͻ�������   //xu 20101109
       for(i=0; i<UDPSENDMAX; i++)
        if(Multi_Udp_Buff[i].isValid == 1)
         {
          
          if((strcmp(Multi_Udp_Buff[i].RemoteIP, RemoteIP) == 0)&&(Multi_Udp_Buff[i].buf[8] == LEAVEGROUP))
           {
            isIP = 1;
            xtm_printf("�˳��鲥���������ڻ�����, %d.%d.%d.%d\n", Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2], Remote.IP[j][3]);
            break;
           }
         }

       if(isIP == 0)
        {
         //ͷ��
         memcpy(send_b, UdpPackageHead, 6);
         //���� 
         send_b[6] = VIDEOTALK;
         send_b[7] = Local.MachineType | ASK;    //����
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
void SingleExitGroup(int s_socket, uint8_t *buf) //�򵥸����з��˳��鲥������
{
  uint16_t sendlength;
  //ͷ��
  memcpy(send_b, UdpPackageHead, 6);
  //����
  send_b[6] = VIDEOTALK;
  send_b[7] = Local.MachineType | ASK;    //����
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
  xtm_printf("���� ����Ҫ���˳��鲥��, RemoteHost = %d.%d.%d.%d\0", \
                  buf[53], buf[54], buf[55], buf[56]);

}
//-----------------------------------------------------------------------
//ͬ��״̬
void RecvSyncSub_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort)  //Ϊ��������ʱ�븱��ͬ��״̬
{
  uint8_t j;
  uint8_t isAddrOK;
  uint16_t sendlength;
  struct SyncSubData1 SubData1;  

  //��������
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost != '0')
     {
      memcpy(send_b, recv_buf, sizeof(struct SyncSubData1) + 34);
      send_b[7] = Local.MachineType | REPLY;    //Ӧ��
      sendlength = sizeof(struct SyncSubData1) + 34;
      CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

      isAddrOK = 1;    //�鿴������ַ�Ƿ�ƥ��
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

      xtm_printf("�յ�����ͬ��״̬����\n");
     }
   }
  else  //��������
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SYNCSUB, cFromIP))
     {
      xtm_printf("�Է�Ӧ�𱾻�ͬ��״̬, %d.%d.%d.%d\n", cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3]);
     }
   }
}
//-----------------------------------------------------------------------
void CheckSubData1(struct SyncSubData1 SubData1)  //���ͬ������1���޸ı�
{
#if 0
  uint8_t DataChanged;
  uint8_t DefenceChanged;
  uint8_t i,j;
  DataChanged = 0;
  DefenceChanged = 0;
  Local.DenNum = SubData1.DenNum;             //Ŀ������  ����
  memcpy(Local.DenIP, SubData1.DenIP, sizeof(Local.DenIP));    //����IP
  memcpy(Local.DenAddr, SubData1.DenAddr, sizeof(Local.DenAddr));    //����Addr

  xtm_printf("Local.DenNum = %d\n", Local.DenNum);
//  printf("Local.DenNum = %d, sizeof(struct SyncSubData1) = %d\n", Local.DenNum, sizeof(struct SyncSubData1));
  for(i=0; i<Local.DenNum; i++)
    Local.DenAddr[i][12] = '\0';

  /*if(DefenceChanged == 0)        //����״̬
   if(LocalCfg.DefenceStatus != SubData1.DefenceStatus)
    {
     DefenceChanged = 1;
     xtm_printf("SubData1 1, ����״̬\n");
    }  */

  if(DefenceChanged == 0)       //����ģ�����
   if(LocalCfg.DefenceNum != SubData1.DefenceNum)
    {
     DefenceChanged = 1;
     xtm_printf("SubData1 2, ����ģ�����\n");
    }
  if(DefenceChanged == 0)       //������Ϣ
   for(i=0; i<32; i++)
    for(j=0; j<10; j++)
     if(LocalCfg.DefenceInfo[i][j] != SubData1.DefenceInfo[i][j])
      {
       DefenceChanged = 1;
       i = 32;
       xtm_printf("SubData1 3, ������Ϣ\n");
       break;
      }

  LocalCfg.EngineerPass[4] = '\0';
  SubData1.EngineerPass[4] = '\0';
  if(DataChanged == 0)       //��������
   if(strcmp(LocalCfg.EngineerPass, SubData1.EngineerPass) != 0)
    {
     DataChanged = 1;
     xtm_printf("SubData1 4, ����\n");
    }

  if(DataChanged == 1)
   {
    memcpy(LocalCfg.EngineerPass, SubData1.EngineerPass, sizeof(LocalCfg.EngineerPass));    //��������
    WriteCfgFile();  //д���������ļ�
   }

  //20101120
  if(DefenceChanged == 1)
   {
    LocalCfg.DefenceNum = SubData1.DefenceNum;          //����ģ�����
    memcpy(LocalCfg.DefenceInfo, SubData1.DefenceInfo, sizeof(LocalCfg.DefenceInfo));    //������Ϣ

    //����״̬
    if(LocalCfg.DefenceStatus != SubData1.DefenceStatus)
     {
      LocalCfg.DefenceStatus = SubData1.DefenceStatus;       //����״̬
      if(LocalCfg.DefenceStatus == 0)
       {
        // CancelFortifyDeal(); //�������洦��
       }
      else
       {
        //if(Local.CurrentWindow == 0)
        //  DisplayImage(&state_image[3], MAINYUVFBPAGE, FROMMEMORY);
        //SetGpio((unsigned long)GPIO_DATA_REG, FORTIFY_LED, SET_LOW);  //������ LED
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
//��������������������
void RecvSubDefence_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j;
  uint8_t isAddrOK;
  uint16_t sendlength;
  uint8_t isOK;

  //��������
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost == '0')  //Ϊ����
     {
      isAddrOK = 1;    //�鿴������ַ�Ƿ�ƥ��
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
        send_b[7] = REPLY;    //Ӧ��
        send_b[62] = isOK;
        sendlength = 63;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        if(isOK == 1)
         {
          switch(recv_buf[57])
           {
            case 0: //����
                   //20101109  ���ڱ���״̬�£������ſɳ���
                   xtm_printf("Local.AlarmByte[0] = 0x%02X, Local.AlarmByte[1] = 0x%02X\n", Local.AlarmByte[0], Local.AlarmByte[1]);
                   //if((Local.AlarmByte[0] == 0)&&(Local.AlarmByte[1] == 0))
                   //  CancelFortifyDeal(); //�������洦��
                   break;
            case 1: //�������
            case 2: //�ڼҲ���
            case 3: //���޲���
            case 4: //�𴲲���
            case 5: //�Զ��岼��
                  LocalCfg.DefenceStatus = recv_buf[57];
                 // FortifyFinishDeal(); //������ɽ��洦��
                  break;
            case 11: //���������ʱ
            case 12: //�ڼҲ�����ʱ
            case 13: //���޲�����ʱ��
            case 14://�𴲲�����ʱ��

            case 15://�Զ��岼����ʱ��
                     LocalCfg.DefenceStatus = recv_buf[57];
                   //  if(Local.CurrentWindow == 32)
                   //    DisplayCancelFortifyWindow(1);
                     Local.DefenceDelayFlag = 1;    //������ʱ��־
                     Local.DefenceDelayTime = 0;  //����
                 break;
           }
         }
       }

      //20101104
     // SendHostAssiOrder(LocalCfg.DefenceStatus, 1);

      xtm_printf("�յ������������� %d\n", recv_buf[57]);
     }
   }
  else  //��������
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SUBDEFENCE, cFromIP))
     {
      xtm_printf("�Է�Ӧ�𸱻��������� %d\n", recv_buf[57]);
     }
   }
}
//-----------------------------------------------------------------------
//������ʾ����������������
void RecvSubDefenceTip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  uint8_t j;
  uint8_t isAddrOK;
  uint16_t sendlength;

  //��������
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost != '0')  //Ϊ����
     {
      isAddrOK = 1;    //�鿴������ַ�Ƿ�ƥ��
      for(j=33; j<33+Local.AddrLen; j++)
       if(LocalCfg.Addr[j-33] != recv_buf[j])
        {
         isAddrOK = 0;
         break;
        }
      if(isAddrOK == 1)
       {
        memcpy(send_b, recv_buf, 58);
        send_b[7] = REPLY;    //Ӧ��
        sendlength = 58;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        switch(recv_buf[57])
         {
          case 0: //����
                // CancelFortifyDeal(); //�������洦��
                 break;
          case 1: //�������
          case 2: //�ڼҲ���
          case 3: //���޲���
          case 4://�𴲲���
          case 5://�Զ��岼��
                  LocalCfg.DefenceStatus = recv_buf[57];
                  //FortifyFinishDeal(); //������ɽ��洦��
                 //break;
          case 11: //���������ʱ
          case 12: //�ڼҲ�����ʱ
          case 13: //���޲�����ʱ��
          case 14://�𴲲�����ʱ��

          case 15://�Զ��岼����ʱ��
                     LocalCfg.DefenceStatus = recv_buf[57];
                    // if(Local.CurrentWindow == 32)
                    //   DisplayCancelFortifyWindow(1);
                     Local.DefenceDelayFlag = 1;    //������ʱ��־
                     Local.DefenceDelayTime = 0;  //����
                 break;
         }
       }

      xtm_printf("�յ�����������ʾ���� %d\n", recv_buf[57]);
     }
   }
  else  //��������
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SUBDEFENCETIP, cFromIP))
     {
      xtm_printf("�Է�Ӧ������������ʾ���� %d\n", recv_buf[57]);
     }
   }
}
//-----------------------------------------------------------------------
//��������������������
void RecvSubAlarm_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  int j;
  int isAddrOK;
  int sendlength;
  int isOK;

  //��������
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost == '0')  //Ϊ����
     {
      isAddrOK = 1;    //�鿴������ַ�Ƿ�ƥ��
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
        send_b[7] = REPLY;    //Ӧ��
        send_b[58] = isOK;
        sendlength = 59;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);

        if(isOK == 1)
         {
          //�˴����뱨������
          
          if(recv_buf[57] == 37) //�ٳֱ���
           {
            //SendAlarmFunc(0x01, Local.AlarmByte);
            //CancelAlarmDeal(); //ȡ���������洦��
            xtm_printf("�ٳֱ���\n");
           }
         }
       }

      xtm_printf("�յ������������� %d\n", recv_buf[57]);
     }
   }
  else  //��������
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SUBALARM, cFromIP))
     {
      xtm_printf("�Է�Ӧ�𸱻��������� %d\n", recv_buf[57]);
     }
   }
}
//-----------------------------------------------------------------------
//������ʾ����������������
void RecvSubAlarmTip_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length)
{
  int j;
  int isAddrOK;
  int sendlength;
  int isOK;

  //��������
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost == '0')  //Ϊ����
     {
      isAddrOK = 1;    //�鿴������ַ�Ƿ�ƥ��
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
        send_b[7] = REPLY;    //Ӧ��
        send_b[62] = isOK;
        sendlength = 63;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        if(isOK == 1)
         {
          switch(recv_buf[57])
           {
            case 0:
                   //����
                   break;
            case 1:
                   //�������
                   break;
            case 2:
                   //�ڼҲ���
                   break;
            case 3:
                   //���޲���
                   break;
            case 4:
                   //�𴲲���
                   break;
            case 5:
                   //�Զ��岼��
                   break;
           }
         }
       }

      xtm_printf("�յ������������� %d\n", recv_buf[57]);
     }
   }
  else  //��������
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    if(ClearMultiUdpBuff(HOSTTOSUB, ASK, SUBCONTROL, cFromIP))
     {
      xtm_printf("�Է�Ӧ�𸱻�������ʾ���� %d\n", recv_buf[57]);
     }
   }
}
//-----------------------------------------------------------------------
void RecvSubFind_Func(int s_socket, uint8_t *recv_buf, char *cFromIP, uint16_t DestPort, uint16_t length) //���Ҹ�������������������
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
  //��������
  if((recv_buf[7] & 0x03) == ASK)
   {
    if(Local.isHost != '0')  //Ϊ����
     {
      isAddrOK = 1;    //�鿴������ַ�Ƿ�ƥ��
      for(j=33; j<33+Local.AddrLen; j++)
       if(LocalCfg.Addr[j-33] != recv_buf[j])
        {
         isAddrOK = 0;
         break;
        }
      if(isAddrOK == 1)
       {
        memcpy(send_b, recv_buf, 57);
        send_b[7] = REPLY;    //Ӧ��
        memcpy(send_b + 53, LocalCfg.IP, 4);
        memcpy(send_b + 57, LocalCfg.AddrVerifyMa, 6);
        send_b[63] = 0;
        sendlength = 64;
        CreateMultiUdpBuff(s_socket, SINGLE_SEND, cFromIP, DestPort, send_b, sendlength, 0);
        xtm_printf("�յ����Ҹ������� %d, %d.%d.%d.%d\n", recv_buf[6], cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3]);
       }
     }
   }
  else  //��������
   if((recv_buf[7]& 0x03) == REPLY) 
   {
    //printf("RecvSubFind_Func %s\n", cFromIP);
    //��������
    if((recv_buf[7]& 0x03) == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if(Multi_Udp_Buff[i].buf[6] == HOSTTOSUB)
          if((Multi_Udp_Buff[i].buf[7] & 0x03) == ASK)
           if(Multi_Udp_Buff[i].buf[8] == SUBFIND)
           {
              //�ж�Ҫ����ҵ�ַ�Ƿ�ƥ��
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
              isAddrOK = 1;    //�鿴������ַ�Ƿ�ƥ�� 
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
                if(strcmp(TmpAddrVerifyMa, LocalCfg.AddrVerifyMa) == 0)   //������ͬ����
                 {
                  isOK = 1;
                  Multi_Udp_Buff[i].isValid = 0;
                  Multi_Udp_Buff[i].SendNum = 0;
                  xtm_printf("�Է�Ӧ����Ҹ������� %d.%d.%d.%d\n", cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3]);
                 }
                else
                 {
                  xtm_printf("�Է�Ӧ����Ҹ�������, ������֤����� %d.%d.%d.%d, %s\n", cFromIP[0], cFromIP[1], cFromIP[2], cFromIP[3], TmpAddrVerifyMa);
                 }
                break;
               }
             }
   }

    if(isOK == 1) //�������Ҹ����ɹ�
     {
      //�鿴�б����Ƿ����иø�����Ϣ
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
#ifdef _REMOTE_FORTIFY_SUPPORT  //Զ�̲�����֧��
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
  send_b[7] = Local.MachineType | REPLY;    //Ӧ��
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
                SendCancelAlarmFunc(); //ȡ����������

              for(i=0; i<MAXDEFENCENUM; i++)
               {
                Local.AlarmDelayFlag[i] = 0;     //������ʱ��־
                Local.AlarmDelayTime[i] = 0;
               }

              for(i=0; i<2; i++)
                Local.AlarmByte[i] = 0;

              PlayCancelAlarmWav();

              CancelAlarmDeal(); //ȡ���������洦��
       }
      else
       {
        if(LocalCfg.DefenceStatus == 0)
         {
          LocalCfg.DefenceStatus = 11;
          Local.DefenceDelayFlag = 1;    //������ʱ��־
          Local.DefenceDelayTime = 0;    //����

          DisplayCancelFortifyWindow(NOREFRESH);
         }
        else
         {
          PlayCancelFortifyWav();
          CancelFortifyDeal(); //�������洦��
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
