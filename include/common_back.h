#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <types.h>
#include "lib_gpio.h"
#include "lib_timer.h"

#include "x_disp.h"

#include "x_touch.h"
#include "x_audio.h"
/*#include "x_video.h"*/

#include "x_main.h"
#include "x_lcd.h"
#include "x_comm.h"
#include "x_gpio.h"

#include "x_udp.h"

//#include "x_zt2083.h"

#include "x_first.h"
#include "x_info.h"
#include "x_record.h"
#include "x_setup.h"
#include "x_talk.h"
#include "x_alarm.h"
#include "x_timer.h"
//#include "m_init.h"

#include "define.h"
struct Local1{
               uint8_t PreStatus;  //20130115 Ԥ״̬����ֹͬʱ�Ĳ���
               int Status;
               int TalkEndStatus;
               uint8_t KeyTouchNo; //��ť���±�־
               int KeyTouchTime;
               //״̬ 0 ���� 1 ���жԽ�  2 ���жԽ�  3 ����  4 ������  5 ����ͨ��
               //6 ����ͨ��
               //30 �绰ͨ��
               int RecordPic;  //����Ƭ  0 ����  1 ��������Ƭ  2 ͨ������Ƭ
               int IFrameCount; //I֡����
               int IFrameNo;    //���ڼ���I֡
               int HavePicRecorded;  //����Ƭ��¼��
               uint8_t ManualRecordFlag; //�ֶ�¼��

               char RemoteAddr[21];  //Զ�˵�ַ

               unsigned char AlarmByte[2];  //����

               int CallConfirmFlag; //���߱�־
               int Timer1Num;  //��ʱ��1����
               int OnlineFlag; //��������ȷ��
               int OnlineNum;  //����ȷ�����
               uint16_t TimeOut;    //���ӳ�ʱ,  ͨ����ʱ,  ���г�ʱ�����˽���
               int TalkTimeOut; //ͨ���ʱ��

               int PrevWindow;      //��һ�����ڱ��
               int TmpWindow;       //�ݴ洰�ڱ�� ���ڵ�������ʱ
               int CurrentWindow;   //��ǰ���ڱ��
               int ChildWindow;
               int isDisplayWindowFlag; //����������ʾ��
               int DefenceDelayFlag;    //������ʱ��־
               int DefenceDelayTime;   //����
               int PassLen;            //���볤��
               int AlarmDelayFlag[MAXCOUNT];    //������ʱ��־
               int AlarmDelayTime[MAXCOUNT];   //����

               int CalibratePos;   //У׼������ʮ��λ�� 0 1 2 3
               int CalibrateSucc;  //У׼�ɹ�
               unsigned char IP_Group[4];  //�鲥��ַ
               unsigned char Weather[3];   //����Ԥ��

               int AddrLen;          //��ַ����  S 12  B 6 M 8 H 6                 

               int isHost;           //'0' ���� '1' ���� '2' ...
               int ConnToHost;       //�������������� 1 ���� 0 ������
               int ConnToHostTime;
               unsigned char HostIP[4]; //����IP
               char HostAddr[21]; //����Addr
               int DenNum;             //Ŀ������  ����
               unsigned char DenIP[SUBMAXNUM][4];    //����IP
               char DenAddr[SUBMAXNUM][21];         //����Addr
               int DenFlag[SUBMAXNUM];              //�������ڱ�־
               int TmpDenNum;          //Ŀ�������ݴ�  ����
               unsigned char TmpDenIP[SUBMAXNUM][4];    //����IP
               char TmpDenAddr[SUBMAXNUM][21];          //����Addr

               int SendSyncFlag;  //����ͬ����Ϣ��־
               int SendSyncTime;  //����ͬ����Ϣ����

               char SoundName[6][50];  //�����ļ�����

               int NetLanStatus;   //Lan����״̬ 1 �Ͽ�  0 ��ͨ
               int OldNetLanStatus;   //Lan����״̬ 1 �Ͽ�  0 ��ͨ
               int OldNetSpeed;  //�����ٶ�                
               int NoBreak;     //����״̬ 1 ����  0 ����

               int ReportSend;  //�豸��ʱ����״̬�ѷ���
               int RandReportTime; //�豸��ʱ����״̬���ʱ��
               int ReportTimeNum;  //��ʱ

               int LcdLightFlag; //LCD�����־
               int LcdLightTime; //ʱ��
               int KeyPressTime;

               int NewInfo;  //������Ϣ
               int NewRecPic;  //������Ӱ

               int nowvideoframeno;   //��ǰ��Ƶ֡���
               int nowaudioframeno;   //��ǰ��Ƶ֡���

               int ForceEndWatch;  //�к���ʱ��ǿ�ƹؼ���

               int WatchTarget;  //����Ŀ��
               int WatchTargetMaxNum; //����Ŀ���������

               int WatchKeyTouched;     //���Ӽ� ��Ӧ
               int WatchKeyTouchedTime;  //���Ӽ� ��Ӧ ����

               int CenterKeyTouched;     //�������ļ� ��Ӧ
               int CenterKeyTouchedTime;  //�������ļ� ��Ӧ ����                         

               int TestTouch; //���Դ�����

               unsigned char OldVoiceHint;         //������ʾ  0 �ر�  1  ��
               unsigned char OldSpkVolume;         //SPK ����
               unsigned char OldMicVolume;         //MIC ����
               unsigned char OldTalkVoiceOpened;         //�Խ�ʱ����  0 �ر�  1  ��
               unsigned char OldTalkVolume;        //�Խ�ʱ����
               unsigned char OldScrSave[3];        //��Ļ����  0 �ر�  1  ��
               unsigned char OldBrightVolume;      //����
               unsigned char OldKeyVoice;          //������    0 �ر�  1  ��

               int SceneStatus;  //�龰ģʽ��ǰ״̬               

               int CurrRingSec;  //��ǰ��������ѡ����
               int isPlayRing;   //���ڲ�������

               int isRecordWav;  //����¼������
               int isPlayWav;    //���ڲ�������

               int BootNo;

               #ifdef _CAPTURESCREEN
                 int OpenLockKeyNum;
                 int OpenLockTime;
               #endif


               int TalkWatchReturnType;  //0 --  ������   1 -- Talk Watch ����
               int TalkWatchWinType;  //0 -- Talk����  1 -- Watch����
               int TalkWatchButtonNum;

               unsigned char LanguageNo;    //�����±�  0 -- ����    1 -- Ӣ��
               int ImageButton_Text_wd;     //��ť���ּ��

               int DefaultPage;

               int StateBar_Top_Flag; //����״̬����ʾ��־

               int Text_Space;  //�ı����

               uint8_t ClearVideoWindowFlag; //����Ƶ���ڱ�־

               uint8_t NsMultiAddr[4];    //NS�鲥��ַ

               uint8_t Camera;      //0  -- ��  1 -- ov7725
               uint8_t VideoType;
               uint8_t MachineType;  //0x80  --- 8130

               uint8_t  PlayNullVoice;

               uint8_t Test_Disp_No;   //���Դ��� ��ʾ����
               uint8_t Test_CurrPage;  //��ǰҳ
               uint8_t Test_FrameRate; //����֡��
               uint8_t Test_FrameSize; //�����С
               uint8_t Test_VideoClosed;    //��Ƶ�ر�
               uint8_t Test_LocalWatchFlag; //������Ƶ����
               uint8_t Test_EncodeWatchFlag; //������Ƶ����������ʾ
               uint8_t Test_RecAudioFlag; //¼��
               uint8_t Test_VideoOutput;    //��Ƶ���  0  ����  1 �Լ�
               uint8_t Test_PowerStayOnFreq;    //�͹��Ĵ�����Ƶ��
               uint8_t Test_VideoSdramRefresh;    //LCD��ʾʱ SDRAMˢ������  0  8����  1 4����
               uint8_t Test_OV7725FrameRate;    //OV7725 ���֡��

               uint8_t DecodeFrameSize; //�����С
               uint16_t DecodeVideoWidth;
               uint16_t DecodeVideoHeight;

               uint8_t DecodeStreamFlag;  //���ڽ�����
               uint8_t DecodeJpegFlag;  //���ڽ⾲̬ͼƬ

               uint8_t fb_page;  //��ʾҳ 0 - 1       ��ǰҳ
               uint8_t fb_backpage;   //��̨ҳ
               uint32_t fb_addr[2];
			   uint32_t fb1_addr;

               uint8_t Conflict_Flag;    //IP��ַ��ͻ
               uint8_t Conflict_Mac[6];

               uint8_t GpioWatchDogStarted;  //��GPIO����߳�������WatchDog
               char DebugIP[20];
               uint8_t RemoteDebugInfo;  //����Զ�̵�����Ϣ

               uint8_t VideoWindowFlag;  //��Ƶ���ڴ򿪱�־
              #ifdef _REMOTECALLTEST  //Զ�̺��в���
               uint8_t EnterTestMode;  //����ģʽ, 5���Զ�����
               uint8_t CloseTestRing;  //����ģʽ, �ر�����
              #endif

               uint8_t FirstWindowFlag;     //������ҳ��־
               uint16_t FirstWindowTime;  //������ҳ ����

              #ifdef ELEVATORCONTROL_HANGZHOU      //20120822
               char Elevator_Addr[21];
               uint8_t Elevator_IP[4];
              #endif

               uint8_t WriteCfgFlag;     //20131025 ��ʼ��ʱдCfg ��־
               uint8_t WriteCfgTime;         

			   uint8_t SendLockFlag;    //20180608 ͨ�������п���
              };

struct LocalCfg1{
               char FlashHead[6];         //ͷ��������
               char Addr[20];             //��ַ����
               char AddrVerifyMa[10];         //������ͬ����
               uint8_t Mac_Addr[6]; //������ַ
               uint8_t IP[4];       //IP��ַ
               uint8_t IP_Mask[4];  //��������
               uint8_t IP_Gate[4];  //���ص�ַ
               uint8_t IP_NS[4];    //NS�����ƽ�������������ַ
               uint8_t IP_Server[4];  //����������ַ����NS��������Ϊͬһ����
               uint8_t IP_Broadcast[4];  //�㲥��ַ

               int ReportTime;      //�豸��ʱ����״̬ʱ��
               uint8_t DefenceStatus;       //����״̬      0 - ����  1 - ���   2 - �ڼ�   3 - ����   4 - ��   5 - �Զ���
                                                  //                        11 - ���   12 - �ڼ�   13 - ����   14 - ��   15 - �Զ���     ��ʱ
               uint8_t DefenceNum;          //����ģ�����
               uint8_t DefenceInfo[32][10]; //������Ϣ

               char EngineerPass[10];             //��������

               int In_DelayTime;                //������ʱ
               int Out_DelayTime;               //�����ʱ
               int Alarm_DelayTime;               //������ʱ
               int BellNum;                     //�������
               uint8_t SoundSetup[8];          //����          0 -- 7
               uint8_t RecPhoto;          //�ÿ���Ӱ          0 --���� 1 --����
               uint8_t Language;          //����ѡ��    0 --�������� 1 --�������� 2 -- ENLISH
               uint8_t RingType;          //��������  0 ��ͨ��  1  ��ͨ��  2  ҡ����   3  ������
               uint8_t CallWaitRing;      //��������  0 ��ͨ��  1  ��ͨ��  2  ҡ����   3  ������

               uint8_t VoiceHint;         //������ʾ  0 �ر�  1  ��
               uint8_t SpkVolume;         //SPK ����
               uint8_t MicVolume;         //MIC ����
               uint8_t TalkVoiceOpened;         //�Խ�ʱ����  0 �ر�  1  ��
               uint8_t TalkVolume;        //�Խ�ʱ����
               uint8_t PhoneVoiceOpened;         //�绰ʱ����  0 �ر�  1  ��
               uint8_t PhoneVolume;        //�绰ʱ����
               uint8_t ScrSave[3];        //��Ļ����  0 �ر�  1  ��
               uint8_t MainPage;          //��ҳ����  0 Ĭ�Ͻ���  1 �绰����
               uint8_t BrightVolume;      //����
               uint8_t ContrastVolume;    //�Աȶ�
               uint8_t KeyVoice;          //������    0 �ر�  1  ��

               int16_t Ts_X0;                   //������
               int16_t Ts_Y0;
               int16_t Ts_deltaX;
               int16_t Ts_deltaY;
               int16_t Ts_Mode;                 //ģʽ  X   Y

               uint8_t EncodeFrameRate; //����֡��
               uint8_t EncodeFrameSize; //�����С
               uint8_t VideoClosed;    //��Ƶ�ر�
               uint8_t VideoOutput;    //��Ƶ���  0  ����  1 �Լ�

               uint8_t VideoSdramRefresh;    //LCD��ʾʱ SDRAMˢ������  0  8����  1 4����

               uint8_t MultiMachineSupport;  //��ֻ�֧��,��������Ч���粻֧�֣��򲻷��㲥��ѯ��
               uint8_t DenMaxNum;  //���������������һ������С�� SUBMAXNUM�� �����ڼ�С�㲥��

               //��������
               uint8_t DefenceSetup_dType[MAXDEFENCENUM];    //��������     2 - 24Сʱ����  3 - �Զ������
               uint8_t DefenceSetup_dDelay[MAXDEFENCENUM];   //��ʱʱ��     0 - 0��   1 - 5��  2 - 10��  3 - 15��  4 - 20��  5 - 25��  6 - 60��
               uint8_t DefenceSetup_tType[MAXDEFENCENUM];    //̽ͷ����     0 - �̸�  1 - ȼ��  2 - ����  3 - ����  4 - ����  5 - �Ŵ�  6 - ����

               uint8_t Scene_Defence[5][MAXDEFENCENUM];      //�����龰ģʽ����

               uint32_t EncodeErrorNum;                      //���������Ŀ

               uint8_t ResetStatus;
               uint8_t Reset_DenNum;
               uint8_t Reset_IP[10][4];     //�Է�IP
               uint8_t Reset_Mac[10][6];    //�Է�Mac
               char Reset_Addr[10][21];   //�Է�Addr

               uint16_t BootNo;
              #ifdef _SEND_TO_SELF_TEST    //�������ݰ����Լ�
               uint16_t SelfBootNo;
              #endif
              
               uint8_t MicDecValue; //չ�䡢չ��ģʽ,���С Mic����
              };

struct Remote1{
               uint8_t DenNum;             //Ŀ������  ����+����
               uint8_t LineUseNum;         //20101202 xu ռ������
               uint8_t GroupIP[4]; //GroupIP
               uint8_t IP[10][4];    //�Է�IP
               uint8_t Added[10];                //�Ѽ�����
               char Addr[10][21];         //�Է�Addr
              };
struct TalkCall1{
                uint8_t BuildFlag; //��
                char Build[5];
                uint8_t BuildMaxlen;
                uint8_t UnitFlag;  //��Ԫ
                char Unit[5];
                uint8_t UnitMaxlen;
                uint8_t RoomFlag;  //����
                char Room[5];
                uint8_t RoomMaxlen;
                uint8_t Len;
                char TalkText[15];
               };
#ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
struct SyncSubData1
      {
               int DenNum;             //Ŀ������  ����     2
               unsigned char DenIP[SUBMAXNUM][4];    //����IP    12
               char DenAddr[SUBMAXNUM][22];         //����Addr   66

               //�������� ��������  *************
               unsigned char DefenceStatus;       //����״̬     1
               unsigned char DefenceNum;          //����ģ�����    1
               unsigned char DefenceInfo[32][10]; //������Ϣ       320

               char EngineerPass[10];             //��������       10
      };
#endif

//ͨ�����ݽṹ
struct talkdata1
  {
//   unsigned char Order;     //��������
   char HostAddr[20];       //���з���ַ
   uint8_t HostIP[4]; //���з�IP��ַ
   char AssiAddr[20];       //���з���ַ
   uint8_t AssiIP[4]; //���з�IP��ַ
   uint32_t timestamp;  //ʱ���
   uint16_t DataType;          //��������
   uint16_t Frameno;           //֡���
   uint32_t Framelen;            //֡���ݳ���
   uint16_t TotalPackage;      //�ܰ���
   uint16_t CurrPackage;       //��ǰ����
   uint16_t Datalen;           //���ݳ���
   uint16_t PackLen;       //���ݰ���С
  };

//UDP���ݷ��ͽṹ
struct Multi_Udp_Buff1{
               uint8_t isValid; //�Ƿ���Ч
               uint8_t SendMode; //ASK -- ��η���   REPLY --- ���η���
               uint8_t SendNum; //��ǰ���ʹ���
               uint8_t MaxSendNum; //���ʹ���
               uint8_t CurrOrder;//��ǰ����״̬,VIDEOTALK  VIDEOWATCH
                               //��Ҫ���������ʱ���絥��������Ϊ0
				int s_socket;                               
               uint16_t DestPort;
               char RemoteIP[20];
               uint8_t *buf;//[1520];
              // uint8_t buf[1520];
               uint16_t TimeNum;
               uint16_t DelayTime;  //�ȴ�ʱ��
               uint16_t SendDelayTime; //���͵ȴ�ʱ�����  20101112
               uint16_t nlength;
              };

//Audio UDP���ݷ��ͽṹ
struct Audio_Udp_Buff1{
               uint8_t isValid; //�Ƿ���Ч
               uint8_t SendMode; //ASK -- ��η���   REPLY --- ���η���
               uint8_t SendNum; //��ǰ���ʹ���
               uint8_t MaxSendNum; //���ʹ���
               uint8_t CurrOrder;//��ǰ����״̬,VIDEOTALK  VIDEOWATCH
                               //��Ҫ���������ʱ���絥��������Ϊ0
				int s_socket;                               
               uint16_t DestPort;
               char RemoteIP[20];
               uint8_t buf[86 + 64];
               uint16_t TimeNum;
               uint16_t DelayTime;  //�ȴ�ʱ��
               uint16_t SendDelayTime; //���͵ȴ�ʱ�����  20101112
               uint16_t nlength;
              };              
              
//COMM�����������ݷ��ͽṹ
struct Multi_Comm_Buff1{
               uint8_t isValid; //�Ƿ���Ч
               uint8_t SendNum; //��ǰ���ʹ���
               uint8_t m_Comm;
               uint8_t buf[256];
               uint16_t nlength;
              };

struct PicStatBuf1{
               uint8_t Flag;                      //��ʱ����ʾ��Ϣ��־
               uint8_t Type;                      //����
               uint16_t Time;
               uint16_t MaxTime;                   //�ʱ��
              };

struct Info1{
               char FlashHead[8]; //Flash�Ѵ��־
               uint16_t MaxNum;   //�����Ϣ��
               uint16_t TotalNum; //��Ϣ����
               uint16_t NoReadedNum; //δ����Ϣ����
               uint16_t TotalInfoPage;   //����Ϣҳ��
               uint16_t CurrentInfoPage; //��ǰ��Ϣҳ
               uint16_t CurrNo;    //��ǰ��Ϣ���
               uint16_t CurrPlayNo;  //��ǰ�������
              };

//������Ϣ���ݽṹ��
struct InfoContent1{
               uint8_t isValid;  //��Ч��δɾ����־   1
               uint8_t isReaded; //�Ѷ���־    1
               uint8_t HavePic;  //����ͼ���־  0   1 -- 640x480  2 -- 320x240
               char RemoteAddr[21];
               char Time[32];    //����ʱ��    32
               uint8_t Type;     //����        1    ��Ϣ���ͻ��¼�����     0 -- ���С���1 -- ����δ����  2 -- ���н���
               uint32_t Sn;      //���        4
               uint16_t Length;      //����        4
              };                               //�ڴ����Ϊ444

struct displayinfo1
 {
  uint16_t totalpage;
  uint16_t pageno;
  uint16_t totalrow;
  char content_row[MAXROW][INFOROWLEN + 2];
  uint16_t isDeleted;  //����ʾ��Ϣ����ʱɾ������Ϣ
 };


struct downfile1
  {
   uint8_t FlagText[20];     //��־�ַ���
   uint32_t Faddr;    //Flash ��ַ
   uint32_t Filelen;            //�ļ���С  ʵ�ʴ�С
   uint32_t FLen;               //Flash�洢��С
   uint16_t FileType;         //�ļ����� 0 -- ������  1 -- ͼƬ   2 -- �ֿ�   3 -- ����
   uint16_t Width;
   uint16_t Height;
   uint16_t xLeft;
   uint16_t yTop;
   uint16_t TotalPackage;      //�ܰ���
   uint16_t CurrPackage;       //��ǰ����
   uint16_t Datalen;           //���ݳ���
  };

#ifndef CommonH
#define CommonH
  //����״̬����
  struct Local1 Local;
  struct LocalCfg1 LocalCfg;

  u32 lcd_buffer0_addr[2];
  u32 lcd_buffer1_addr;

  int DeltaLen;  //���ݰ���Ч����ƫ����
  //Զ�˵�ַ
  struct Remote1 Remote;
  char NullAddr[21];   //���ַ���

  char UdpPackageHead[15];
  uint16_t LocalPort[PORT_NUM];
  uint16_t RemotePort[PORT_NUM];

  struct Multi_Udp_Buff1 Multi_Udp_Buff[UDPSENDMAX]; //UDP���ͻ���
  struct Audio_Udp_Buff1 Audio_Udp_Buff[AUDIOSENDMAX]; //Audio UDP���ͻ���  

  struct PicStatBuf1 PicStatBuf;

  //��Ƶͼ��
  //struct TImage video_image;

  //������
  struct TImage main_image;
  struct TImageButton menu_button[5];
  struct TImage logo_image;
  struct TImage state_image[6];

  struct TImage top_image;
  struct TImage middle_image;
  struct TImage bottom_image;
  struct TImageButton bigmenu_button[7];

  //��������
  struct TImageButton num_button[15];
  struct TEdit setup_pass_edit;
  struct TImage setup_key_image;

  //����������
  struct TImageButton setup_button[5];

  //���̴���
  struct TImage kb_image;
  struct TEdit kb_edit;
  struct TImageButton kb_button[15];

  //LAN���ô���
  struct TImage lan_image;
  struct TImageButton lan_button[2];
  struct TEdit lan_edit[5];

  //�������ô���
  struct TImage addr_image;
  struct TImageButton addr_button[2];
  struct TEdit addr_edit[8];

  //�޸����봰��
  struct TImage pass_image;
  struct TImageButton pass_button[2];
  struct TEdit pass_edit[3];

  //��Ļ���ڴ���
  struct TImage screen_image;
  struct TImageButton screen_button[3];
  struct TImage brightness_image[6];

  //�������ڴ���
  struct TImage voice_image;
  struct TImageButton voice_button[5];
  struct TImage spk_image[6];
  struct TImage mic_image[6];

  //���ڴ���
  struct TImageButton version_button[2];  

  //������У׼����
  struct TImage calibrate_image;
  struct TImageButton cross_button[1];

  //��������
  struct TImage call_image;
  struct TEdit r2r_edit;

  //���Ӵ���
  struct TImage watchtarget_image;
  struct TImageButton watch_button[3];

  //���봰��
  struct TImage talkpic_image;
  struct TImageButton talkpic_button[3];
  struct TImage talkwatchpic_image;

  //������Ƶ����
  struct TImage watchpic_image;
  struct TImageButton watchpic_button[1];

  //��������
  struct TImageButton fortify_button[2];

  //��������
  struct TEdit cancelfortify_edit;                //���봰���ı���

  //����ͼ��
  struct TImage alarm_main_image;
  struct TImage alarm_image[2];

  //��Ϣ����
  struct TImage info_image;
  struct TImageButton info_button[3];
  struct TImageButton inforow_button[INFONUMPERPAGE];

  //��Ϣ�鿴����
  struct TImage infowin_image;
  struct TImageButton infowin_button[3];
  struct TImage infocontent_image;

  //����Ϣ�ṹ
  struct Info1 Info;                                       // INFOTYPENUM      INFOMAXITEM
  struct InfoContent1 InfoContent[INFOMAXITEM];
  char *InfoBuff[INFOMAXITEM];  

  //ͨ����¼����
  struct TImage records_image;
  struct TImageButton records_button[3];
  struct TImageButton recordsrow_button[RECORDNUMPERPAGE];
  struct TImage records_pic_icon[RECORDNUMPERPAGE];

  //ͨ����¼�鿴����
  struct TImage picwin_image;
  struct TImageButton picwin_button[3];
  struct TImage pic_blank_image;

  //ͨ����¼�ṹ
  struct Info1 Records;                                       // INFOTYPENUM      INFOMAXITEM
  struct InfoContent1 RecordsContent[RECORDMAXITEM];
  uint32_t RecordsBuff[RECORDMAXITEM];                         //��Ƭ�洢��ַ
  struct InfoContent1 TmpRecordsContent;  //��ʱ�ṹ

  struct TImage bitstream_image[6];
  struct TImage error_image;

  struct TAudio soundtip_wav;
  struct TAudio ring_wav;
  struct TAudio backring_wav;
  struct TAudio modisucc_wav;
  struct TAudio modifail_wav;
  struct TAudio inputerror_wav;
  struct TAudio passerror_wav;
  struct TAudio cancelfortify_wav;
  struct TAudio defencedelay_wav;
  struct TAudio fortify_wav;
  struct TAudio cancelalarm_wav;
  struct TAudio alarmdelay_wav;
  struct TAudio alarm_wav;
  struct TAudio null_wav;
  struct TAudio wav_1k[60];
  struct TAudio icn8505_wav;   //2018-07-24, add by zz

#else
  //����״̬����
  extern struct Local1 Local;
  extern struct LocalCfg1 LocalCfg;

  extern u32 lcd_buffer0_addr[2];
  extern u32 lcd_buffer1_addr;

  extern int DeltaLen;  //���ݰ���Ч����ƫ����
  //Զ�˵�ַ
  extern struct Remote1 Remote;
  extern char NullAddr[21];   //���ַ���

  extern char UdpPackageHead[15];
  extern uint16_t LocalPort[PORT_NUM];
  extern uint16_t RemotePort[PORT_NUM];

  extern struct Multi_Udp_Buff1 Multi_Udp_Buff[UDPSENDMAX]; //UDP���ͻ���
  extern struct Audio_Udp_Buff1 Audio_Udp_Buff[AUDIOSENDMAX]; //Audio UDP���ͻ���  

  extern struct PicStatBuf1 PicStatBuf;

  //��Ƶͼ��
  //extern struct TImage video_image;

  //������
  extern struct TImage main_image;
  extern struct TImageButton menu_button[5];
  extern struct TImage logo_image;
  extern struct TImage state_image[6];

  extern struct TImage top_image;
  extern struct TImage middle_image;
  extern struct TImage bottom_image;
  extern struct TImageButton bigmenu_button[7];

  //��������
  extern struct TImageButton num_button[15];
  extern struct TEdit setup_pass_edit;
  extern struct TImage setup_key_image;

  //����������
  extern struct TImageButton setup_button[5];

  //���̴���
  extern struct TImage kb_image;
  extern struct TEdit kb_edit;
  extern struct TImageButton kb_button[15];

  //LAN���ô���
  extern struct TImage lan_image;
  extern struct TImageButton lan_button[2];
  extern struct TEdit lan_edit[5];

  //�������ô���
  extern struct TImage addr_image;
  extern struct TImageButton addr_button[2];
  extern struct TEdit addr_edit[8];

  //�޸����봰��
  extern struct TImage pass_image;
  extern struct TImageButton pass_button[2];
  extern struct TEdit pass_edit[3];

  //��Ļ���ڴ���
  extern struct TImage screen_image;
  extern struct TImageButton screen_button[3];
  extern struct TImage brightness_image[6];

  //�������ڴ���
  extern struct TImage voice_image;
  extern struct TImageButton voice_button[5];
  extern struct TImage spk_image[6];
  extern struct TImage mic_image[6];

  //���ڴ���
  extern struct TImageButton version_button[2];

  //������У׼����
  extern struct TImage calibrate_image;
  extern struct TImageButton cross_button[1];

  //��������
  extern struct TImage call_image;
  extern struct TEdit r2r_edit;

  //���Ӵ���
  extern struct TImage watchtarget_image;
  extern struct TImageButton watch_button[3];

  //���봰��
  extern struct TImage talkpic_image;
  extern struct TImageButton talkpic_button[3];
  extern struct TImage talkwatchpic_image;

  //������Ƶ����
  extern struct TImage watchpic_image;
  extern struct TImageButton watchpic_button[1];

  //��������
  extern struct TImageButton fortify_button[2];

  //��������
  extern struct TEdit cancelfortify_edit;                //���봰���ı���

  //����ͼ��
  extern struct TImage alarm_main_image;
  extern struct TImage alarm_image[2];

  //��Ϣ����
  extern struct TImage info_image;
  extern struct TImageButton info_button[3];
  extern struct TImageButton inforow_button[INFONUMPERPAGE];

  //��Ϣ�鿴����
  extern struct TImage infowin_image;
  extern struct TImageButton infowin_button[3];
  extern struct TImage infocontent_image;

  //����Ϣ�ṹ
  extern struct Info1 Info;                                       // INFOTYPENUM      INFOMAXITEM
  extern struct InfoContent1 InfoContent[INFOMAXITEM];
  extern char *InfoBuff[INFOMAXITEM];            //��Ϣ����

  //ͨ����¼����
  extern struct TImage records_image;
  extern struct TImageButton records_button[3];
  extern struct TImageButton recordsrow_button[RECORDNUMPERPAGE];
  extern struct TImage records_pic_icon[RECORDNUMPERPAGE];

  //ͨ����¼�鿴����
  extern struct TImage picwin_image;
  extern struct TImageButton picwin_button[3];
  extern struct TImage pic_blank_image;

  //ͨ����¼�ṹ
  extern struct Info1 Records;                                       // INFOTYPENUM      INFOMAXITEM
  extern struct InfoContent1 RecordsContent[RECORDMAXITEM];
  extern uint32_t RecordsBuff[RECORDMAXITEM];                         //��Ƭ�洢��ַ
  extern struct InfoContent1 TmpRecordsContent;  //��ʱ�ṹ

  extern struct TImage bitstream_image[6];
  extern struct TImage error_image;
  //extern struct TImage main_image11111[19];

  extern struct TAudio soundtip_wav;
  extern struct TAudio ring_wav;
  extern struct TAudio backring_wav;
  extern struct TAudio modisucc_wav;
  extern struct TAudio modifail_wav;
  extern struct TAudio inputerror_wav;
  extern struct TAudio passerror_wav;
  extern struct TAudio cancelfortify_wav;
  extern struct TAudio fortify_wav;
  extern struct TAudio cancelalarm_wav;
  extern struct TAudio defencedelay_wav;
  extern struct TAudio alarmdelay_wav;
  extern struct TAudio alarm_wav;
  extern struct TAudio null_wav;
  extern struct TAudio wav_1k[60];
  extern struct TAudio icn8505_wav;   //2018-07-24, add by zz  
#endif