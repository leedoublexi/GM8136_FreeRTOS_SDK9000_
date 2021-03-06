
#include "stdlib.h"
#include "lib_spi.h"


#define x_main_h

#define CommonH
#include "common.h"

void Init_LocalVar(void); //初始化 Local 结构

void ResetLocalCfg(void);    //复位配置文件



uint8_t *Flash_Stream;        //Flash存储


spi_platform_setting_t flash_s;
u8* flash_rd_buf;
u8* flash_wt_buf;

void flash_init(void);

//读设置文件
void ReadCfgFile(void);
//写设置文件
void WriteCfgFile(void);
//写数据到Flash
void SaveToFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length);
//从Flash读数据
void ReadFromFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length);

//---------------------------------------------------------------------------
void flash_init(void)
{
	flash_rd_buf = gm_noncache_mem_alloc(4*1024, 32);   
	if(flash_rd_buf == NULL){	 
		printf("flash_rd_buf alloc memory fail!\n");
	}
	flash_wt_buf = gm_noncache_mem_alloc(64*1024, 32);
	if(flash_wt_buf == NULL){	 
		printf("flash_wt_buf alloc memory fail!\n");
	}   
	flash_s.buff_addr_r = flash_rd_buf;
	flash_s.buff_addr_w = flash_wt_buf;
	flash_s.option = GM_SPI_SPI020_FLASH;
	gm_spi_init(NULL, &flash_s);

	


}

void Init_LocalVar(void) //初始化 Local 结构
{
  uint8_t i, j;
  uint32_t temp_addr;
  uint8_t Scene_Defence1[5][MAXDEFENCENUM] = {
                                          {1, 1, 1, 1, 1, 1, 1, 1},
                                          {1, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 1}
                                         };
  Local.MachineType = 0x80;  //0x80  --- 8130
  Local.KeyTouchNo = 0;

  Flash_Stream = (u32 *)gm_noncache_mem_alloc(ALIGN(0x4000, 256), 256);
  flash_init();
 // vTaskDelay(100);

  ReadCfgFile();    //读本地设置文件
  
  Local.NetLanStatus = 0;   //网络状态 0 断开  1 接通
  Local.OldNetLanStatus = 0;   //网络状态 0 断开  1 接通
  Local.OldNetSpeed = 100;  //网络速度
  Local.NewInfo = 0;  //有新信息
  Local.NewRecPic = 0;  //有新留影
  
  Local.RecordPic = 0;  //留照片  0 不留
  Local.IFrameCount = 0; //I帧计数
  Local.IFrameNo = 125;    //留第几个I帧

  Local.ForceEndWatch = 0;  //有呼叫时，强制关监视


  Local.LcdLightFlag = 1; //LCD背光标志
  Local.LcdLightTime = 0; //时间
  Local.KeyPressTime = 0;

  LocalCfg.VoiceHint = 1;
  Local.OldVoiceHint = LocalCfg.VoiceHint;         //语音提示  0 关闭  1  打开
  Local.OldSpkVolume = LocalCfg.SpkVolume;         //SPK 音量
  Local.OldMicVolume = LocalCfg.MicVolume;         //MIC 音量
  Local.OldTalkVoiceOpened = LocalCfg.TalkVoiceOpened;         //对讲时音量  0 关闭  1  打开
  Local.OldTalkVolume = LocalCfg.TalkVolume;        //对讲时音量
  for(i=0; i<3; i++)
    Local.OldScrSave[i] =  LocalCfg.ScrSave[i];        //屏幕保护  0 关闭  1  打开

  if(LocalCfg.BrightVolume > 4)
    LocalCfg.BrightVolume = 3;  

  Local.OldBrightVolume = LocalCfg.BrightVolume;      //亮度
  Local.OldKeyVoice = LocalCfg.KeyVoice;          //按键音    0 关闭  1  打开

/*  
  //防区设置
  for(i=0; i<MAXDEFENCENUM; i++)
  {
    Local.OldDefenceSetup[i].dType = LocalCfg.DefenceSetup[i].dType;
    Local.OldDefenceSetup[i].dDelay = LocalCfg.DefenceSetup[i].dDelay;
    Local.OldDefenceSetup[i].tType = LocalCfg.DefenceSetup[i].tType;
  }
 */

  //20101116 布防延时改为 100S
  //外出延时
  LocalCfg.Out_DelayTime = 30;//100;
  //报警延时
  LocalCfg.Alarm_DelayTime = 30;
  if((LocalCfg.Addr[0] != 'S')&&(LocalCfg.Addr[0] != 'B'))
    LocalCfg.Addr[0] = 'B';

  if((LocalCfg.Addr[0] == 'S')||(LocalCfg.Addr[0] == 'B'))
  {
    Local.AddrLen = 12;  //地址长度  S 12  B 12 M 8 H 6
  }

  LocalCfg.Addr[Local.AddrLen] = '\0';
  LocalCfg.AddrVerifyMa[6] = '\0';
  
  Local.Weather[0] = 1;  //天气预报
  Local.Weather[1] = 20;//0;
  Local.Weather[2] = 27;//0;

  Local.ReportSend = 0;  //设备定时报告状态已发送
  Local.RandReportTime = 1;
  Local.ReportTimeNum  = 0;

  Local.nowvideoframeno = 1;   //当前视频帧编号
  Local.nowaudioframeno = 1;   //当前音频帧编号  

  Local.NoBreak = 0;   //免扰状态 1 免扰  0 正常
  //主机
  Local.isHost = '0';
  Local.ConnToHost = 0;
  Local.ConnToHostTime = 0;
  Local.HostIP[0] = 0;
  Local.HostIP[1] = 0;
  Local.HostIP[2] = 0;
  Local.HostIP[3] = 0;
  Local.isHost = LocalCfg.Addr[11];
  Local.DenNum = 0;
  memcpy(Local.HostAddr, LocalCfg.Addr, 20);
  Local.HostAddr[11] = '0';

  //防区状态
  LocalCfg.DefenceNum = 1;
  //20101028  保存 布防信息

  for(j=0; j<5; j++)
    for(i=0; i<MAXDEFENCENUM; i++)
    {
      LocalCfg.Scene_Defence[j][i] = Scene_Defence1[j][i];
      if(LocalCfg.Scene_Defence[j][i] == 1)
      {
        LocalCfg.DefenceInfo[i][2] = 1;  // 布防
        LocalCfg.DefenceInfo[i][3] = 0;  // 无报警
      }
      else
      {
        LocalCfg.DefenceInfo[i][2] = 0;  // 布防
        LocalCfg.DefenceInfo[i][3] = 0;  // 无报警
      }
    }

  //防区设置   0  1  2 - 延时防区   3  4  5  普通防区   6  7  24小时防区
  for(i=0; i<=2; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 3;   //防区类型     0 - 日防区  1 - 夜防区  2 - 24小时防区  3 - 自定义防区
    LocalCfg.DefenceSetup_dDelay[i] = 5;  //延时时间     0 - 0秒   1 - 5秒  2 - 10秒  3 - 15秒  4 - 20秒  5 - 25秒  6 - 60秒
    LocalCfg.DefenceSetup_tType[i] = 4;   //探头类型     0 - 烟感  1 - 燃气  2 - 防拆  3 - 紧急  4 - 红外  5 - 门磁  6 - 窗磁
  }
  
  for(i=3; i<=5; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 3;   //防区类型     0 - 日防区  1 - 夜防区  2 - 24小时防区  3 - 自定义防区
    LocalCfg.DefenceSetup_dDelay[i] = 0;  //延时时间     0 - 0秒   1 - 5秒  2 - 10秒  3 - 15秒  4 - 20秒  5 - 25秒  6 - 60秒
    LocalCfg.DefenceSetup_tType[i] = 4;   //探头类型     0 - 烟感  1 - 燃气  2 - 防拆  3 - 紧急  4 - 红外  5 - 门磁  6 - 窗磁
  }
  
  for(i=6; i<=7; i++)
  //for(i=0; i<=7; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 2;   //防区类型     0 - 日防区  1 - 夜防区  2 - 24小时防区  3 - 自定义防区
    LocalCfg.DefenceSetup_dDelay[i] = 0;  //延时时间     0 - 0秒   1 - 5秒  2 - 10秒  3 - 15秒  4 - 20秒  5 - 25秒  6 - 60秒
    LocalCfg.DefenceSetup_tType[i] = 4;   //探头类型     0 - 烟感  1 - 燃气  2 - 防拆  3 - 紧急  4 - 红外  5 - 门磁  6 - 窗磁
  }

  DeltaLen = 9 + sizeof(struct talkdata1);  //数据包有效数据偏移量

  Local.Status = 0;  //状态为空闲
  Local.TalkEndStatus = 0;  //状态为空闲

  Local.AlarmByte[0] = 0x0;         //报警
  Local.AlarmByte[1] = 0x0;         //报警
  Local.DefenceDelayFlag = 0;    //布防延时标志
  Local.DefenceDelayTime = 0;    //计数
  for(i=0; i<MAXCOUNT; i++)
  {
    Local.AlarmDelayFlag[i] = 0;     //报警延时标志
    Local.AlarmDelayTime[i] = 0;    //计数
  }

  Local.IP_Group[0] = 0; //组播地址
  Local.IP_Group[1] = 0;
  Local.IP_Group[2] = 0;
  Local.IP_Group[3] = 0;

  Local.NsMultiAddr[0] = 238;    //NS组播地址
  Local.NsMultiAddr[1] = 9;
  Local.NsMultiAddr[2] = 9;
  Local.NsMultiAddr[3] = 1;

  strcpy(NullAddr, "00000000000000000000");

  Remote.IP[0][0] = 192;
  Remote.IP[0][1] = 168;
  Remote.IP[0][2] = 68;
  Remote.IP[0][3] = 88;
  memcpy(Remote.Addr[0],NullAddr,20);
  memcpy(Remote.Addr[0],"S0001010101",11);      //

  Local.Test_OV7725FrameRate = 0;    //OV7725 输出帧率

 /* sprintf(Local.SoundName[0], "%s/%s\0", syswav_dir1, "ring1.wav");
  sprintf(Local.SoundName[1], "%s/%s\0", syswav_dir1, "ring2.wav");
  sprintf(Local.SoundName[2], "%s/%s\0", syswav_dir1, "ring3.wav");
  sprintf(Local.SoundName[3], "%s/%s\0", syswav_dir1, "ring4.wav");
  sprintf(Local.SoundName[4], "%s/%s\0", syswav_dir1, "ring4.wav");
  sprintf(Local.SoundName[5], "%s/%s\0", syswav_dir1, "ring4.wav");   */

  #ifdef _CAPTURESCREEN
    Local.OpenLockKeyNum = 0;
    Local.OpenLockTime = 0;
  #endif

  if(LocalCfg.Language == 2)  //语言  0 -- 中文简体   1 -- 中文繁体   2 -- 英文
    Local.LanguageNo = 1;
  else
    Local.LanguageNo = 0;
  xtm_printf("Init_LocalVar 1\n");

  //延时清提示信息标志
  PicStatBuf.Flag = 0;
  PicStatBuf.Type = 0;
  PicStatBuf.Time = 0;

  Local.Conflict_Flag = 0;    //IP地址冲突
  for(i=0; i<6; i++)
    Local.Conflict_Mac[i] = 0x0;

  Local.GpioWatchDogStarted = 1;  //在GPIO检测线程中启动WatchDog
  Local.RemoteDebugInfo = 0;      //发送远程调试信息

  LocalCfg.EncodeFrameSize = VIDEO_VGA; //编码大小
  LocalCfg.VideoClosed = 1; //视频关闭

 #ifdef _REMOTECALLTEST  //远程呼叫测试
  Local.EnterTestMode = 0;  //测试模式, 5秒自动接听
  Local.CloseTestRing = 0;  //测试模式, 关闭铃声
 #endif
  //20130318 测试
  //LocalCfg.VideoSdramRefresh = 1;    //LCD显示时 SDRAM刷新周期  0  8周期  1 4周期
  if(LocalCfg.MicDecValue > 3) //展箱、展板模式,需减小 Mic音量
    LocalCfg.MicDecValue = 0;  

  Local.SendLockFlag = 0;    //20180608 通话过程中开锁


}
//---------------------------------------------------------------------------
void ResetLocalCfg(void)    //复位配置文件
{
  uint8_t i, j;
  xtm_printf("ResetLocalCfg::\n");
  //头部特征码
  memcpy(LocalCfg.FlashHead, "XXXCID", 6);
  //地址编码
  memcpy(LocalCfg.Addr,NullAddr,20);
  memcpy(LocalCfg.Addr,"S00010108090",12);
  //主副机同步码
  strcpy(LocalCfg.AddrVerifyMa, "123456");
  //网卡地址
  LocalCfg.Mac_Addr[0] = 0x00;
  LocalCfg.Mac_Addr[1] = 0x23;
  LocalCfg.Mac_Addr[2] = 0x60;
  LocalCfg.Mac_Addr[3] = 0x22;
  LocalCfg.Mac_Addr[4] = 0x41;
  LocalCfg.Mac_Addr[5] = 0x63;
  //IP地址
  LocalCfg.IP[0] = 192;
  LocalCfg.IP[1] = 168;
  LocalCfg.IP[2] = 68;
  LocalCfg.IP[3] = 161;
  //子网掩码
  LocalCfg.IP_Mask[0] = 255;
  LocalCfg.IP_Mask[1] = 255;
  LocalCfg.IP_Mask[2] = 255;
  LocalCfg.IP_Mask[3] = 0;
  //网关地址
  LocalCfg.IP_Gate[0] = 192;
  LocalCfg.IP_Gate[1] = 168;
  LocalCfg.IP_Gate[2] = 68;
  LocalCfg.IP_Gate[3] = 1;
  //NS（名称解析）服务器地址
  LocalCfg.IP_NS[0] = 192;
  LocalCfg.IP_NS[1] = 168;
  LocalCfg.IP_NS[2] = 1;
  LocalCfg.IP_NS[3] = 1;
  //主服务器地址
  LocalCfg.IP_Server[0] = 192;
  LocalCfg.IP_Server[1] = 168;
  LocalCfg.IP_Server[2] = 68;
  LocalCfg.IP_Server[3] = 2;
  //广播地址
  LocalCfg.IP_Broadcast[0] = 192;
  LocalCfg.IP_Broadcast[1] = 168;
  LocalCfg.IP_Broadcast[2] = 1;
  LocalCfg.IP_Broadcast[3] = 255;
        
  //设备定时报告状态时间
  LocalCfg.ReportTime = 10;
  //布防状态
  LocalCfg.DefenceStatus = 0;
  //防区模块个数
  LocalCfg.DefenceNum = 1;
  for(i=0; i<8; i++)
    for(j=0; j<10; j++)
      LocalCfg.DefenceInfo[i][j] = 0;
  
  //工程密码
  strcpy(LocalCfg.EngineerPass, "123456");

  //防区设置   0  1  2 - 延时防区   3  4  5  普通防区   6  7  24小时防区
  for(i=0; i<=2; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 3;   //防区类型     0 - 日防区  1 - 夜防区  2 - 24小时防区  3 - 自定义防区
    LocalCfg.DefenceSetup_dDelay[i] = 5;  //延时时间     0 - 0秒   1 - 5秒  2 - 10秒  3 - 15秒  4 - 20秒  5 - 25秒  6 - 60秒
    LocalCfg.DefenceSetup_tType[i] = 4;   //探头类型     0 - 烟感  1 - 燃气  2 - 防拆  3 - 紧急  4 - 红外  5 - 门磁  6 - 窗磁
  }
  
  for(i=3; i<=5; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 3;   //防区类型     0 - 日防区  1 - 夜防区  2 - 24小时防区  3 - 自定义防区
    LocalCfg.DefenceSetup_dDelay[i] = 0;  //延时时间     0 - 0秒   1 - 5秒  2 - 10秒  3 - 15秒  4 - 20秒  5 - 25秒  6 - 60秒
    LocalCfg.DefenceSetup_tType[i] = 4;   //探头类型     0 - 烟感  1 - 燃气  2 - 防拆  3 - 紧急  4 - 红外  5 - 门磁  6 - 窗磁
  }
  for(i=6; i<=7; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 2;   //防区类型     0 - 日防区  1 - 夜防区  2 - 24小时防区  3 - 自定义防区
    LocalCfg.DefenceSetup_dDelay[i] = 0;  //延时时间     0 - 0秒   1 - 5秒  2 - 10秒  3 - 15秒  4 - 20秒  5 - 25秒  6 - 60秒
    LocalCfg.DefenceSetup_tType[i] = 4;   //探头类型     0 - 烟感  1 - 燃气  2 - 防拆  3 - 紧急  4 - 红外  5 - 门磁  6 - 窗磁
  }

  //进入延时
  LocalCfg.In_DelayTime = 30;
  //外出延时
  LocalCfg.Out_DelayTime = 30;
  //报警延时
  LocalCfg.Alarm_DelayTime = 30;

  //声音
  for(i=0; i<8; i++)
    LocalCfg.SoundSetup[i] = 0;
  
  //访客留影
  LocalCfg.RecPhoto = 0;
  //语言选择
  LocalCfg.Language = 1;
  //铃声类型  0 普通型  1  卡通型  2  摇滚型   3  抒情型
  LocalCfg.RingType = 0;
  //回铃类型  0 普通型  1  卡通型  2  摇滚型   3  抒情型
  LocalCfg.CallWaitRing = 0;
  //语音提示  0 关闭  1  打开
  LocalCfg.VoiceHint = 1;
  //SPK 音量
  LocalCfg.SpkVolume = 2;
  //MIC 音量
  LocalCfg.MicVolume = 2;
  //对讲时音量
  LocalCfg.TalkVoiceOpened = 1;
  LocalCfg.TalkVolume = 2;

  //屏幕保护
  LocalCfg.ScrSave[0] = 1;
  LocalCfg.ScrSave[1] = 0;
  LocalCfg.ScrSave[2] = 0;
  //首页设置  0 默认界面  1 电话界面
  LocalCfg.MainPage = 0;
  //亮度
  LocalCfg.BrightVolume = 2;
  //对比度
  LocalCfg.ContrastVolume = 2;
  //按键音    0 关闭  1  打开
  LocalCfg.KeyVoice = 1;
  //触摸屏
  LocalCfg.Ts_X0 = 2059;//1901;
  LocalCfg.Ts_Y0 = 1955;//2001;
  LocalCfg.Ts_deltaX = 3844;//3744;
  LocalCfg.Ts_deltaY = -3491;//3555;
  LocalCfg.Ts_Mode = 1;     //X   Y  正常

  LocalCfg.Language = 0;  //语言  0 -- 中文简体   1 -- 中文繁体   2 -- 英文

  LocalCfg.EncodeFrameRate = 3; //编码帧率
  LocalCfg.EncodeFrameSize = VIDEO_VGA; //编码大小
  LocalCfg.VideoClosed = 0; //视频关闭
  LocalCfg.VideoOutput = 0;    //视频输出  0  正常  1 自检
  LocalCfg.VideoSdramRefresh = 0;    //LCD显示时 SDRAM刷新周期  0  8周期  1 4周期
  LocalCfg.MultiMachineSupport = 0;  //多分机支持,对主机有效，如不支持，则不发广播查询包
  LocalCfg.DenMaxNum = 1;  //副机最大数量，这一数量需小于 SUBMAXNUM， 有利于减小广播包
  LocalCfg.BootNo = 0;
  //outportb(POWER_CONTROL1, 0x04);//电源控制寄存器1    4周期
}
//---------------------------------------------------------------------------
//读本地设置文件
void ReadCfgFile(void)
{
  uint8_t iCount;
  iCount = 0;
  while(iCount < 10)
  {
	  gm_spi_flash_data_read(LOCALCFG_ADDR, 0x1000, Flash_Stream);
    memcpy(&LocalCfg, Flash_Stream, sizeof(struct LocalCfg1));
    iCount ++;
	  ClearWatchDog();
    if((LocalCfg.FlashHead[0] == 'X')||(LocalCfg.FlashHead[1] == 'X')||(LocalCfg.FlashHead[2] == 'X')||(LocalCfg.FlashHead[3] == 'C')||(LocalCfg.FlashHead[4] == 'I')||(LocalCfg.FlashHead[5] == 'D'))
      break;
  }
  if(iCount > 1)
    xtm_printf("ReadCfgFile::LOCALCFG_ADDR  iCount = %d\n", iCount);
  
  if((LocalCfg.FlashHead[0] != 'X')||(LocalCfg.FlashHead[1] != 'X')||(LocalCfg.FlashHead[2] != 'X')||(LocalCfg.FlashHead[3] != 'C')||(LocalCfg.FlashHead[4] != 'I')||(LocalCfg.FlashHead[5] != 'D'))
  {
    iCount = 0;
    while(iCount < 10)
    {
	    gm_spi_flash_data_read(LOCALCFG_ADDR1, 0x1000, Flash_Stream);
      memcpy(&LocalCfg, Flash_Stream, sizeof(struct LocalCfg1));
      iCount ++;
	    ClearWatchDog();
      if((LocalCfg.FlashHead[0] == 'X')||(LocalCfg.FlashHead[1] == 'X')||(LocalCfg.FlashHead[2] == 'X')||(LocalCfg.FlashHead[3] == 'C')||(LocalCfg.FlashHead[4] == 'I')||(LocalCfg.FlashHead[5] == 'D'))
        break;
    }
    if(iCount > 1)
      xtm_printf("ReadCfgFile::LOCALCFG_ADDR1  iCount = %d\n", iCount);
  
    if((LocalCfg.FlashHead[0] != 'X')||(LocalCfg.FlashHead[1] != 'X')||(LocalCfg.FlashHead[2] != 'X')||(LocalCfg.FlashHead[3] != 'C')||(LocalCfg.FlashHead[4] != 'I')||(LocalCfg.FlashHead[5] != 'D'))
    {
      ResetLocalCfg();    //复位配置文件
      WriteCfgFile();     //写本地设置文件
    } 
  }
  LocalCfg.Addr[12] = '\0';

  //LocalCfg.BootNo ++;
  Local.WriteCfgFlag = 1;     //20131025 初始化时写Cfg 标志
  Local.WriteCfgTime = 0;
  //WriteCfgFile();
}
//---------------------------------------------------------------------------
//写本地设置文件
void WriteCfgFile(void)
{
  memcpy(Flash_Stream, &LocalCfg, sizeof(struct LocalCfg1));
  
  gm_spi_flash_data_write(LOCALCFG_ADDR, 0x1000, Flash_Stream);
  gm_spi_flash_data_write(LOCALCFG_ADDR1, 0x1000, Flash_Stream);
}
//---------------------------------------------------------------------------
//写数据到Flash
void SaveToFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length)
{
  gm_spi_flash_data_write(FlashAddr, length, buf);
}
//---------------------------------------------------------------------------
//从Flash读数据
void ReadFromFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length)
{
	gm_spi_flash_data_read(FlashAddr, length, buf);
}
//---------------------------------------------------------------------------


