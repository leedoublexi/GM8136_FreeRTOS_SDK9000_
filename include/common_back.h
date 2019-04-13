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
               uint8_t PreStatus;  //20130115 预状态，防止同时的操作
               int Status;
               int TalkEndStatus;
               uint8_t KeyTouchNo; //按钮按下标志
               int KeyTouchTime;
               //状态 0 空闲 1 主叫对讲  2 被叫对讲  3 监视  4 被监视  5 主叫通话
               //6 被叫通话
               //30 电话通话
               int RecordPic;  //留照片  0 不留  1 呼叫留照片  2 通话留照片
               int IFrameCount; //I帧计数
               int IFrameNo;    //留第几个I帧
               int HavePicRecorded;  //有照片已录制
               uint8_t ManualRecordFlag; //手动录制

               char RemoteAddr[21];  //远端地址

               unsigned char AlarmByte[2];  //报警

               int CallConfirmFlag; //在线标志
               int Timer1Num;  //定时器1计数
               int OnlineFlag; //需检查在线确认
               int OnlineNum;  //在线确认序号
               uint16_t TimeOut;    //监视超时,  通话超时,  呼叫超时，无人接听
               int TalkTimeOut; //通话最长时间

               int PrevWindow;      //上一个窗口编号
               int TmpWindow;       //暂存窗口编号 用于弹出窗口时
               int CurrentWindow;   //当前窗口编号
               int ChildWindow;
               int isDisplayWindowFlag; //窗口正在显示中
               int DefenceDelayFlag;    //布防延时标志
               int DefenceDelayTime;   //计数
               int PassLen;            //密码长度
               int AlarmDelayFlag[MAXCOUNT];    //报警延时标志
               int AlarmDelayTime[MAXCOUNT];   //计数

               int CalibratePos;   //校准触摸屏十字位置 0 1 2 3
               int CalibrateSucc;  //校准成功
               unsigned char IP_Group[4];  //组播地址
               unsigned char Weather[3];   //天气预报

               int AddrLen;          //地址长度  S 12  B 6 M 8 H 6                 

               int isHost;           //'0' 主机 '1' 副机 '2' ...
               int ConnToHost;       //与主机连接正常 1 正常 0 不正常
               int ConnToHostTime;
               unsigned char HostIP[4]; //主机IP
               char HostAddr[21]; //主机Addr
               int DenNum;             //目标数量  副机
               unsigned char DenIP[SUBMAXNUM][4];    //副机IP
               char DenAddr[SUBMAXNUM][21];         //副机Addr
               int DenFlag[SUBMAXNUM];              //副机存在标志
               int TmpDenNum;          //目标数量暂存  副机
               unsigned char TmpDenIP[SUBMAXNUM][4];    //副机IP
               char TmpDenAddr[SUBMAXNUM][21];          //副机Addr

               int SendSyncFlag;  //发送同步信息标志
               int SendSyncTime;  //发送同步信息计数

               char SoundName[6][50];  //声音文件名称

               int NetLanStatus;   //Lan网络状态 1 断开  0 接通
               int OldNetLanStatus;   //Lan网络状态 1 断开  0 接通
               int OldNetSpeed;  //网络速度                
               int NoBreak;     //免扰状态 1 免扰  0 正常

               int ReportSend;  //设备定时报告状态已发送
               int RandReportTime; //设备定时报告状态随机时间
               int ReportTimeNum;  //计时

               int LcdLightFlag; //LCD背光标志
               int LcdLightTime; //时间
               int KeyPressTime;

               int NewInfo;  //有新信息
               int NewRecPic;  //有新留影

               int nowvideoframeno;   //当前视频帧编号
               int nowaudioframeno;   //当前音频帧编号

               int ForceEndWatch;  //有呼叫时，强制关监视

               int WatchTarget;  //监视目标
               int WatchTargetMaxNum; //监视目标最大数量

               int WatchKeyTouched;     //监视键 响应
               int WatchKeyTouchedTime;  //监视键 响应 计数

               int CenterKeyTouched;     //呼叫中心键 响应
               int CenterKeyTouchedTime;  //呼叫中心键 响应 计数                         

               int TestTouch; //测试触摸屏

               unsigned char OldVoiceHint;         //语音提示  0 关闭  1  打开
               unsigned char OldSpkVolume;         //SPK 音量
               unsigned char OldMicVolume;         //MIC 音量
               unsigned char OldTalkVoiceOpened;         //对讲时音量  0 关闭  1  打开
               unsigned char OldTalkVolume;        //对讲时音量
               unsigned char OldScrSave[3];        //屏幕保护  0 关闭  1  打开
               unsigned char OldBrightVolume;      //亮度
               unsigned char OldKeyVoice;          //按键音    0 关闭  1  打开

               int SceneStatus;  //情景模式当前状态               

               int CurrRingSec;  //当前振铃音乐选择项
               int isPlayRing;   //正在播放振铃

               int isRecordWav;  //正在录制留言
               int isPlayWav;    //正在播放留言

               int BootNo;

               #ifdef _CAPTURESCREEN
                 int OpenLockKeyNum;
                 int OpenLockTime;
               #endif


               int TalkWatchReturnType;  //0 --  主界面   1 -- Talk Watch 界面
               int TalkWatchWinType;  //0 -- Talk窗口  1 -- Watch窗口
               int TalkWatchButtonNum;

               unsigned char LanguageNo;    //语言下标  0 -- 中文    1 -- 英文
               int ImageButton_Text_wd;     //按钮文字间距

               int DefaultPage;

               int StateBar_Top_Flag; //顶部状态条显示标志

               int Text_Space;  //文本间隔

               uint8_t ClearVideoWindowFlag; //清视频窗口标志

               uint8_t NsMultiAddr[4];    //NS组播地址

               uint8_t Camera;      //0  -- 无  1 -- ov7725
               uint8_t VideoType;
               uint8_t MachineType;  //0x80  --- 8130

               uint8_t  PlayNullVoice;

               uint8_t Test_Disp_No;   //测试窗口 显示测试
               uint8_t Test_CurrPage;  //当前页
               uint8_t Test_FrameRate; //编码帧率
               uint8_t Test_FrameSize; //编码大小
               uint8_t Test_VideoClosed;    //视频关闭
               uint8_t Test_LocalWatchFlag; //本地视频监视
               uint8_t Test_EncodeWatchFlag; //本地视频编码解码后显示
               uint8_t Test_RecAudioFlag; //录音
               uint8_t Test_VideoOutput;    //视频输出  0  正常  1 自检
               uint8_t Test_PowerStayOnFreq;    //低功耗待机分频数
               uint8_t Test_VideoSdramRefresh;    //LCD显示时 SDRAM刷新周期  0  8周期  1 4周期
               uint8_t Test_OV7725FrameRate;    //OV7725 输出帧率

               uint8_t DecodeFrameSize; //解码大小
               uint16_t DecodeVideoWidth;
               uint16_t DecodeVideoHeight;

               uint8_t DecodeStreamFlag;  //正在解码流
               uint8_t DecodeJpegFlag;  //正在解静态图片

               uint8_t fb_page;  //显示页 0 - 1       当前页
               uint8_t fb_backpage;   //后台页
               uint32_t fb_addr[2];
			   uint32_t fb1_addr;

               uint8_t Conflict_Flag;    //IP地址冲突
               uint8_t Conflict_Mac[6];

               uint8_t GpioWatchDogStarted;  //在GPIO检测线程中启动WatchDog
               char DebugIP[20];
               uint8_t RemoteDebugInfo;  //发送远程调试信息

               uint8_t VideoWindowFlag;  //视频窗口打开标志
              #ifdef _REMOTECALLTEST  //远程呼叫测试
               uint8_t EnterTestMode;  //测试模式, 5秒自动接听
               uint8_t CloseTestRing;  //测试模式, 关闭铃声
              #endif

               uint8_t FirstWindowFlag;     //返回首页标志
               uint16_t FirstWindowTime;  //返回首页 计数

              #ifdef ELEVATORCONTROL_HANGZHOU      //20120822
               char Elevator_Addr[21];
               uint8_t Elevator_IP[4];
              #endif

               uint8_t WriteCfgFlag;     //20131025 初始化时写Cfg 标志
               uint8_t WriteCfgTime;         

			   uint8_t SendLockFlag;    //20180608 通话过程中开锁
              };

struct LocalCfg1{
               char FlashHead[6];         //头部特征码
               char Addr[20];             //地址编码
               char AddrVerifyMa[10];         //主副机同步码
               uint8_t Mac_Addr[6]; //网卡地址
               uint8_t IP[4];       //IP地址
               uint8_t IP_Mask[4];  //子网掩码
               uint8_t IP_Gate[4];  //网关地址
               uint8_t IP_NS[4];    //NS（名称解析）服务器地址
               uint8_t IP_Server[4];  //主服务器地址（与NS服务器可为同一个）
               uint8_t IP_Broadcast[4];  //广播地址

               int ReportTime;      //设备定时报告状态时间
               uint8_t DefenceStatus;       //布防状态      0 - 撤防  1 - 外出   2 - 在家   3 - 就寝   4 - 起床   5 - 自定义
                                                  //                        11 - 外出   12 - 在家   13 - 就寝   14 - 起床   15 - 自定义     延时
               uint8_t DefenceNum;          //防区模块个数
               uint8_t DefenceInfo[32][10]; //防区信息

               char EngineerPass[10];             //工程密码

               int In_DelayTime;                //进入延时
               int Out_DelayTime;               //外出延时
               int Alarm_DelayTime;               //报警延时
               int BellNum;                     //振铃次数
               uint8_t SoundSetup[8];          //声音          0 -- 7
               uint8_t RecPhoto;          //访客留影          0 --禁用 1 --启用
               uint8_t Language;          //语言选择    0 --简体中文 1 --繁体中文 2 -- ENLISH
               uint8_t RingType;          //铃声类型  0 普通型  1  卡通型  2  摇滚型   3  抒情型
               uint8_t CallWaitRing;      //回铃类型  0 普通型  1  卡通型  2  摇滚型   3  抒情型

               uint8_t VoiceHint;         //语音提示  0 关闭  1  打开
               uint8_t SpkVolume;         //SPK 音量
               uint8_t MicVolume;         //MIC 音量
               uint8_t TalkVoiceOpened;         //对讲时音量  0 关闭  1  打开
               uint8_t TalkVolume;        //对讲时音量
               uint8_t PhoneVoiceOpened;         //电话时音量  0 关闭  1  打开
               uint8_t PhoneVolume;        //电话时音量
               uint8_t ScrSave[3];        //屏幕保护  0 关闭  1  打开
               uint8_t MainPage;          //首页设置  0 默认界面  1 电话界面
               uint8_t BrightVolume;      //亮度
               uint8_t ContrastVolume;    //对比度
               uint8_t KeyVoice;          //按键音    0 关闭  1  打开

               int16_t Ts_X0;                   //触摸屏
               int16_t Ts_Y0;
               int16_t Ts_deltaX;
               int16_t Ts_deltaY;
               int16_t Ts_Mode;                 //模式  X   Y

               uint8_t EncodeFrameRate; //编码帧率
               uint8_t EncodeFrameSize; //编码大小
               uint8_t VideoClosed;    //视频关闭
               uint8_t VideoOutput;    //视频输出  0  正常  1 自检

               uint8_t VideoSdramRefresh;    //LCD显示时 SDRAM刷新周期  0  8周期  1 4周期

               uint8_t MultiMachineSupport;  //多分机支持,对主机有效，如不支持，则不发广播查询包
               uint8_t DenMaxNum;  //副机最大数量，这一数量需小于 SUBMAXNUM， 有利于减小广播包

               //防区设置
               uint8_t DefenceSetup_dType[MAXDEFENCENUM];    //防区类型     2 - 24小时防区  3 - 自定义防区
               uint8_t DefenceSetup_dDelay[MAXDEFENCENUM];   //延时时间     0 - 0秒   1 - 5秒  2 - 10秒  3 - 15秒  4 - 20秒  5 - 25秒  6 - 60秒
               uint8_t DefenceSetup_tType[MAXDEFENCENUM];    //探头类型     0 - 烟感  1 - 燃气  2 - 防拆  3 - 紧急  4 - 红外  5 - 门磁  6 - 窗磁

               uint8_t Scene_Defence[5][MAXDEFENCENUM];      //安防情景模式设置

               uint32_t EncodeErrorNum;                      //编码错误数目

               uint8_t ResetStatus;
               uint8_t Reset_DenNum;
               uint8_t Reset_IP[10][4];     //对方IP
               uint8_t Reset_Mac[10][6];    //对方Mac
               char Reset_Addr[10][21];   //对方Addr

               uint16_t BootNo;
              #ifdef _SEND_TO_SELF_TEST    //发送数据包给自己
               uint16_t SelfBootNo;
              #endif
              
               uint8_t MicDecValue; //展箱、展板模式,需减小 Mic音量
              };

struct Remote1{
               uint8_t DenNum;             //目标数量  主机+副机
               uint8_t LineUseNum;         //20101202 xu 占线数量
               uint8_t GroupIP[4]; //GroupIP
               uint8_t IP[10][4];    //对方IP
               uint8_t Added[10];                //已加入组
               char Addr[10][21];         //对方Addr
              };
struct TalkCall1{
                uint8_t BuildFlag; //幢
                char Build[5];
                uint8_t BuildMaxlen;
                uint8_t UnitFlag;  //单元
                char Unit[5];
                uint8_t UnitMaxlen;
                uint8_t RoomFlag;  //房号
                char Room[5];
                uint8_t RoomMaxlen;
                uint8_t Len;
                char TalkText[15];
               };
#ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
struct SyncSubData1
      {
               int DenNum;             //目标数量  副机     2
               unsigned char DenIP[SUBMAXNUM][4];    //副机IP    12
               char DenAddr[SUBMAXNUM][22];         //副机Addr   66

               //以下设置 传到副机  *************
               unsigned char DefenceStatus;       //布防状态     1
               unsigned char DefenceNum;          //防区模块个数    1
               unsigned char DefenceInfo[32][10]; //防区信息       320

               char EngineerPass[10];             //工程密码       10
      };
#endif

//通话数据结构
struct talkdata1
  {
//   unsigned char Order;     //操作命令
   char HostAddr[20];       //主叫方地址
   uint8_t HostIP[4]; //主叫方IP地址
   char AssiAddr[20];       //被叫方地址
   uint8_t AssiIP[4]; //被叫方IP地址
   uint32_t timestamp;  //时间戳
   uint16_t DataType;          //数据类型
   uint16_t Frameno;           //帧序号
   uint32_t Framelen;            //帧数据长度
   uint16_t TotalPackage;      //总包数
   uint16_t CurrPackage;       //当前包数
   uint16_t Datalen;           //数据长度
   uint16_t PackLen;       //数据包大小
  };

//UDP数据发送结构
struct Multi_Udp_Buff1{
               uint8_t isValid; //是否有效
               uint8_t SendMode; //ASK -- 多次发送   REPLY --- 单次发送
               uint8_t SendNum; //当前发送次数
               uint8_t MaxSendNum; //发送次数
               uint8_t CurrOrder;//当前命令状态,VIDEOTALK  VIDEOWATCH
                               //主要用于需解析时，如单次命令置为0
				int s_socket;                               
               uint16_t DestPort;
               char RemoteIP[20];
               uint8_t *buf;//[1520];
              // uint8_t buf[1520];
               uint16_t TimeNum;
               uint16_t DelayTime;  //等待时间
               uint16_t SendDelayTime; //发送等待时间计数  20101112
               uint16_t nlength;
              };

//Audio UDP数据发送结构
struct Audio_Udp_Buff1{
               uint8_t isValid; //是否有效
               uint8_t SendMode; //ASK -- 多次发送   REPLY --- 单次发送
               uint8_t SendNum; //当前发送次数
               uint8_t MaxSendNum; //发送次数
               uint8_t CurrOrder;//当前命令状态,VIDEOTALK  VIDEOWATCH
                               //主要用于需解析时，如单次命令置为0
				int s_socket;                               
               uint16_t DestPort;
               char RemoteIP[20];
               uint8_t buf[86 + 64];
               uint16_t TimeNum;
               uint16_t DelayTime;  //等待时间
               uint16_t SendDelayTime; //发送等待时间计数  20101112
               uint16_t nlength;
              };              
              
//COMM主动命令数据发送结构
struct Multi_Comm_Buff1{
               uint8_t isValid; //是否有效
               uint8_t SendNum; //当前发送次数
               uint8_t m_Comm;
               uint8_t buf[256];
               uint16_t nlength;
              };

struct PicStatBuf1{
               uint8_t Flag;                      //延时清提示信息标志
               uint8_t Type;                      //类型
               uint16_t Time;
               uint16_t MaxTime;                   //最长时间
              };

struct Info1{
               char FlashHead[8]; //Flash已存标志
               uint16_t MaxNum;   //最大信息数
               uint16_t TotalNum; //信息总数
               uint16_t NoReadedNum; //未读信息总数
               uint16_t TotalInfoPage;   //总信息页数
               uint16_t CurrentInfoPage; //当前信息页
               uint16_t CurrNo;    //当前信息序号
               uint16_t CurrPlayNo;  //当前播放序号
              };

//单条信息内容结构体
struct InfoContent1{
               uint8_t isValid;  //有效，未删除标志   1
               uint8_t isReaded; //已读标志    1
               uint8_t HavePic;  //有无图像标志  0   1 -- 640x480  2 -- 320x240
               char RemoteAddr[21];
               char Time[32];    //接收时间    32
               uint8_t Type;     //类型        1    信息类型或事件类型     0 -- 主叫　　1 -- 被叫未接听  2 -- 被叫接听
               uint32_t Sn;      //序号        4
               uint16_t Length;      //长度        4
              };                               //内存分配为444

struct displayinfo1
 {
  uint16_t totalpage;
  uint16_t pageno;
  uint16_t totalrow;
  char content_row[MAXROW][INFOROWLEN + 2];
  uint16_t isDeleted;  //在显示信息窗口时删除了信息
 };


struct downfile1
  {
   uint8_t FlagText[20];     //标志字符串
   uint32_t Faddr;    //Flash 地址
   uint32_t Filelen;            //文件大小  实际大小
   uint32_t FLen;               //Flash存储大小
   uint16_t FileType;         //文件类型 0 -- 主程序  1 -- 图片   2 -- 字库   3 -- 声音
   uint16_t Width;
   uint16_t Height;
   uint16_t xLeft;
   uint16_t yTop;
   uint16_t TotalPackage;      //总包数
   uint16_t CurrPackage;       //当前包数
   uint16_t Datalen;           //数据长度
  };

#ifndef CommonH
#define CommonH
  //本机状态设置
  struct Local1 Local;
  struct LocalCfg1 LocalCfg;

  u32 lcd_buffer0_addr[2];
  u32 lcd_buffer1_addr;

  int DeltaLen;  //数据包有效数据偏移量
  //远端地址
  struct Remote1 Remote;
  char NullAddr[21];   //空字符串

  char UdpPackageHead[15];
  uint16_t LocalPort[PORT_NUM];
  uint16_t RemotePort[PORT_NUM];

  struct Multi_Udp_Buff1 Multi_Udp_Buff[UDPSENDMAX]; //UDP发送缓冲
  struct Audio_Udp_Buff1 Audio_Udp_Buff[AUDIOSENDMAX]; //Audio UDP发送缓冲  

  struct PicStatBuf1 PicStatBuf;

  //视频图像
  //struct TImage video_image;

  //主界面
  struct TImage main_image;
  struct TImageButton menu_button[5];
  struct TImage logo_image;
  struct TImage state_image[6];

  struct TImage top_image;
  struct TImage middle_image;
  struct TImage bottom_image;
  struct TImageButton bigmenu_button[7];

  //进入设置
  struct TImageButton num_button[15];
  struct TEdit setup_pass_edit;
  struct TImage setup_key_image;

  //设置主窗口
  struct TImageButton setup_button[5];

  //键盘窗口
  struct TImage kb_image;
  struct TEdit kb_edit;
  struct TImageButton kb_button[15];

  //LAN设置窗口
  struct TImage lan_image;
  struct TImageButton lan_button[2];
  struct TEdit lan_edit[5];

  //房号设置窗口
  struct TImage addr_image;
  struct TImageButton addr_button[2];
  struct TEdit addr_edit[8];

  //修改密码窗口
  struct TImage pass_image;
  struct TImageButton pass_button[2];
  struct TEdit pass_edit[3];

  //屏幕调节窗口
  struct TImage screen_image;
  struct TImageButton screen_button[3];
  struct TImage brightness_image[6];

  //音量调节窗口
  struct TImage voice_image;
  struct TImageButton voice_button[5];
  struct TImage spk_image[6];
  struct TImage mic_image[6];

  //关于窗口
  struct TImageButton version_button[2];  

  //触摸屏校准窗口
  struct TImage calibrate_image;
  struct TImageButton cross_button[1];

  //呼出窗口
  struct TImage call_image;
  struct TEdit r2r_edit;

  //监视窗口
  struct TImage watchtarget_image;
  struct TImageButton watch_button[3];

  //呼入窗口
  struct TImage talkpic_image;
  struct TImageButton talkpic_button[3];
  struct TImage talkwatchpic_image;

  //监视视频窗口
  struct TImage watchpic_image;
  struct TImageButton watchpic_button[1];

  //布防窗口
  struct TImageButton fortify_button[2];

  //撤防窗口
  struct TEdit cancelfortify_edit;                //密码窗口文本框

  //报警图像
  struct TImage alarm_main_image;
  struct TImage alarm_image[2];

  //信息窗口
  struct TImage info_image;
  struct TImageButton info_button[3];
  struct TImageButton inforow_button[INFONUMPERPAGE];

  //信息查看窗口
  struct TImage infowin_image;
  struct TImageButton infowin_button[3];
  struct TImage infocontent_image;

  //短信息结构
  struct Info1 Info;                                       // INFOTYPENUM      INFOMAXITEM
  struct InfoContent1 InfoContent[INFOMAXITEM];
  char *InfoBuff[INFOMAXITEM];  

  //通话记录窗口
  struct TImage records_image;
  struct TImageButton records_button[3];
  struct TImageButton recordsrow_button[RECORDNUMPERPAGE];
  struct TImage records_pic_icon[RECORDNUMPERPAGE];

  //通话记录查看窗口
  struct TImage picwin_image;
  struct TImageButton picwin_button[3];
  struct TImage pic_blank_image;

  //通话记录结构
  struct Info1 Records;                                       // INFOTYPENUM      INFOMAXITEM
  struct InfoContent1 RecordsContent[RECORDMAXITEM];
  uint32_t RecordsBuff[RECORDMAXITEM];                         //照片存储地址
  struct InfoContent1 TmpRecordsContent;  //临时结构

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
  //本机状态设置
  extern struct Local1 Local;
  extern struct LocalCfg1 LocalCfg;

  extern u32 lcd_buffer0_addr[2];
  extern u32 lcd_buffer1_addr;

  extern int DeltaLen;  //数据包有效数据偏移量
  //远端地址
  extern struct Remote1 Remote;
  extern char NullAddr[21];   //空字符串

  extern char UdpPackageHead[15];
  extern uint16_t LocalPort[PORT_NUM];
  extern uint16_t RemotePort[PORT_NUM];

  extern struct Multi_Udp_Buff1 Multi_Udp_Buff[UDPSENDMAX]; //UDP发送缓冲
  extern struct Audio_Udp_Buff1 Audio_Udp_Buff[AUDIOSENDMAX]; //Audio UDP发送缓冲  

  extern struct PicStatBuf1 PicStatBuf;

  //视频图像
  //extern struct TImage video_image;

  //主界面
  extern struct TImage main_image;
  extern struct TImageButton menu_button[5];
  extern struct TImage logo_image;
  extern struct TImage state_image[6];

  extern struct TImage top_image;
  extern struct TImage middle_image;
  extern struct TImage bottom_image;
  extern struct TImageButton bigmenu_button[7];

  //进入设置
  extern struct TImageButton num_button[15];
  extern struct TEdit setup_pass_edit;
  extern struct TImage setup_key_image;

  //设置主窗口
  extern struct TImageButton setup_button[5];

  //键盘窗口
  extern struct TImage kb_image;
  extern struct TEdit kb_edit;
  extern struct TImageButton kb_button[15];

  //LAN设置窗口
  extern struct TImage lan_image;
  extern struct TImageButton lan_button[2];
  extern struct TEdit lan_edit[5];

  //房号设置窗口
  extern struct TImage addr_image;
  extern struct TImageButton addr_button[2];
  extern struct TEdit addr_edit[8];

  //修改密码窗口
  extern struct TImage pass_image;
  extern struct TImageButton pass_button[2];
  extern struct TEdit pass_edit[3];

  //屏幕调节窗口
  extern struct TImage screen_image;
  extern struct TImageButton screen_button[3];
  extern struct TImage brightness_image[6];

  //音量调节窗口
  extern struct TImage voice_image;
  extern struct TImageButton voice_button[5];
  extern struct TImage spk_image[6];
  extern struct TImage mic_image[6];

  //关于窗口
  extern struct TImageButton version_button[2];

  //触摸屏校准窗口
  extern struct TImage calibrate_image;
  extern struct TImageButton cross_button[1];

  //呼出窗口
  extern struct TImage call_image;
  extern struct TEdit r2r_edit;

  //监视窗口
  extern struct TImage watchtarget_image;
  extern struct TImageButton watch_button[3];

  //呼入窗口
  extern struct TImage talkpic_image;
  extern struct TImageButton talkpic_button[3];
  extern struct TImage talkwatchpic_image;

  //监视视频窗口
  extern struct TImage watchpic_image;
  extern struct TImageButton watchpic_button[1];

  //布防窗口
  extern struct TImageButton fortify_button[2];

  //撤防窗口
  extern struct TEdit cancelfortify_edit;                //密码窗口文本框

  //报警图像
  extern struct TImage alarm_main_image;
  extern struct TImage alarm_image[2];

  //信息窗口
  extern struct TImage info_image;
  extern struct TImageButton info_button[3];
  extern struct TImageButton inforow_button[INFONUMPERPAGE];

  //信息查看窗口
  extern struct TImage infowin_image;
  extern struct TImageButton infowin_button[3];
  extern struct TImage infocontent_image;

  //短信息结构
  extern struct Info1 Info;                                       // INFOTYPENUM      INFOMAXITEM
  extern struct InfoContent1 InfoContent[INFOMAXITEM];
  extern char *InfoBuff[INFOMAXITEM];            //信息内容

  //通话记录窗口
  extern struct TImage records_image;
  extern struct TImageButton records_button[3];
  extern struct TImageButton recordsrow_button[RECORDNUMPERPAGE];
  extern struct TImage records_pic_icon[RECORDNUMPERPAGE];

  //通话记录查看窗口
  extern struct TImage picwin_image;
  extern struct TImageButton picwin_button[3];
  extern struct TImage pic_blank_image;

  //通话记录结构
  extern struct Info1 Records;                                       // INFOTYPENUM      INFOMAXITEM
  extern struct InfoContent1 RecordsContent[RECORDMAXITEM];
  extern uint32_t RecordsBuff[RECORDMAXITEM];                         //照片存储地址
  extern struct InfoContent1 TmpRecordsContent;  //临时结构

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