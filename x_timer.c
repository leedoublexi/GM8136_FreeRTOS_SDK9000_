
#include "stdlib.h"

#define x_timer_h

#define CommonH
#include "common.h"

uint16_t Timer_Num = 0;  //计数
void timer_init(void);
void timer_thread_func(void);

void CheckBrightVolume(void); //检查亮度音量等设置值


void OnlineCheckFunc(void); //在线确认检测函数
void TimeReportStatusFunc(void); //设备定时报告状态函数
void ClearTipFunc(void);    //清状态提示信息函数
void TalkCtrlFunc(void);  //对讲控制，显示通话时间和判断超时

//检测网络信号
void CheckEth(void);
int net_recv_count = 0;

#ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
#define REFRESHTIME  30
void FindSubFunc(void); //为主机，定时查找副机
void SyncSubFunc(void); //为主机，定时与副机同步状态
void RefreshDen(void);  //更新副机数量和地址
uint8_t RefreshDenFlag = 0;
#endif

//---------------------------------------------------------------------------
void timer_init(void)
{
	
	int ret = xTaskCreate(timer_thread_func, "timer_thread_func", 4096, NULL, TIMER_PRIORITY, NULL ); 
	xtm_printf("timer_init::ret = %d\n", ret);
}

void timer_thread_func(void)
{
    uint16_t val;
	int net_curr_count = 0;
	//unsigned int timeticket1, timeticket2;
	xtm_printf("timer_thread_func::init...\n");
	while(1){
#if 1		
        //timeticket1 = gm_timer_get_tick(0, 0);
        //printf("timer_thread_func::%d\n", timeticket1 - timeticket2);
		//timeticket2 = timeticket1;

		if(Local.CurrentWindow == 0)
		 if((Timer_Num % (TIMERPERSEC*2))==0)
		  {
			CheckEth();
		  }

		 //存储亮度和音量值
		if(((Timer_Num % (TIMERPERSEC*10))==0)&&(Timer_Num != 0))
		 {
		  //xtm_printf("VideoCount = %d\n", VideoCount);
		  #ifdef _XTM8900_SUPPORT  //XTM8900网卡支持
    		net_curr_count = net_get_recvcount();
		  #endif
		  #ifdef _XTM9000_SUPPORT  //XTM9000网卡支持
			net_curr_count = net_get_recvcount_9000();
		  #endif
		  
		  xtm_printf("net_get_recvcount = %d\n", net_curr_count);
		  if(Local.NetLanStatus == 1){
		    if(net_recv_count == net_curr_count){
				LocalCfg.BootNo ++;
				//WriteCfgFile();
				xtm_printf("net not recv, reset..............\n");
				//Local.GpioWatchDogStarted = 0;
		    }
			net_recv_count = net_curr_count;
		  }
				
		  CheckBrightVolume();
		  if(Local.WriteCfgFlag == 1)	  //20131025 初始化时写Cfg 标志
		   {
			Local.WriteCfgTime ++;
			if(Local.WriteCfgTime > 1)
			 {
			  Local.WriteCfgFlag = 0;
			  WriteCfgFile();
			  xtm_printf("timer_thread_func::WriteCfgFile()\n");
			 }
		   }		
		 }
		
	
		//连续6秒没有收到在线确认，则认为断线
		if(Local.OnlineFlag == 1)
		 {
		  OnlineCheckFunc();
		  //对讲控制，显示通话时间和判断超时
		  TalkCtrlFunc();
		 }
		
		//清状态提示信息函数
		ClearTipFunc();
		
		//设备定时报告状态函数
		if(LocalCfg.ReportTime != 0)
		  {
		   if(Local.ReportSend == 1)
			if(Local.ReportTimeNum >= (LocalCfg.ReportTime*TIMERPERSEC))
			 {
			  Local.RandReportTime = rand() % LocalCfg.ReportTime;	  //产生随机数
			  Local.ReportSend = 0;
			  Local.ReportTimeNum = 0;
			  //xtm_printf("Local.RandReportTime = %d, LocalCfg.ReportTime = %d, rand() = %d, RAND_MAX = %d, net_get_recvcount = %d\n", 
			  //	Local.RandReportTime, LocalCfg.ReportTime, rand(),RAND_MAX, net_get_recvcount());
			 }
		   if(Local.ReportSend == 0)
			if(Local.ReportTimeNum >= (Local.RandReportTime*TIMERPERSEC))
			 {
			  Local.ReportSend = 1;
			  TimeReportStatusFunc();
			 }
		   Local.ReportTimeNum ++;
		  }
#if 1			
		#ifdef _MULTI_MACHINE_SUPPORT	//多分机支持
			//为主机，定时查找副机	30S
			if(LocalCfg.MultiMachineSupport == 1) 
			 {
			  if(Local.isHost == '0')
			   {
				if(Local.Status == 0)
				 if((Timer_Num % (TIMERPERSEC*REFRESHTIME))==0)
				  {
		
				   if(LocalCfg.DenMaxNum != Local.DenNum)  //最大副机数量与当前数量相同，无需查找
					{
					 RefreshDenFlag = 1;
					 Local.TmpDenNum = Local.DenNum; //更新副机数量
					 FindSubFunc();
					}
				   Local.SendSyncFlag = 1;	//发送同步信息标志
				   Local.SendSyncTime = 0;	//发送同步信息计数
				  }
				if(Local.SendSyncFlag == 1)
				 {
				  if(Local.SendSyncTime > TIMERPERSEC*2)
				   {
					if(RefreshDenFlag == 1)
					 {
					  RefreshDenFlag = 0;
					  RefreshDen();  //更新副机数量和地址
					 }
					if(Local.DenNum > 0)
					  SyncSubFunc(); //为主机，定时与副机同步状态
					Local.SendSyncFlag = 0;  //发送同步信息标志
					Local.SendSyncTime = 0;  //发送同步信息计数
				   }
				  Local.SendSyncTime ++;
				 }
			   }
			  else	 //为副机, 定时判断与主机连接
			   {
				if(Local.ConnToHost == 1)
				 if((Timer_Num % (TIMERPERSEC*REFRESHTIME))==0)
				  {
				   xtm_printf("Local.ConnToHostTime = %d\n", Local.ConnToHostTime);
				   Local.ConnToHostTime ++;
				   if(Local.ConnToHostTime > 3)
					 Local.ConnToHost = 0;
				  }
			   }
			 }

     #endif
	 if((Timer_Num % (TIMERPERSEC))==0)
	   ShowAlarmImage();
	 
	 CheckDefenceDelay_Func(); //检查布防延时函数	 	   
	 		  
     //检查延时报警函数
	 CheckDelayAlarm();
		 
	 //检查返回首页标志
	 CheckFirstWindowFlag();
#endif
	 Timer_Num ++;
#endif
	 //xtm_printf("timer_thread_func::Timer_Num = %d\n", Timer_Num);

	 vTaskDelay(500/portTICK_RATE_MS);
	}
vTaskDelete(NULL);

}
//---------------------------------------------------------------------------
void CheckBrightVolume(void) //检查亮度音量等设置值
{
  uint8_t i, j;
  uint8_t SaveOK = 0;


  //其它设置    
  if((Local.OldVoiceHint != LocalCfg.VoiceHint)
       ||(Local.OldSpkVolume != LocalCfg.SpkVolume)
       ||(Local.OldMicVolume != LocalCfg.MicVolume)
       ||(Local.OldTalkVoiceOpened != LocalCfg.TalkVoiceOpened)
       ||(Local.OldTalkVolume != LocalCfg.TalkVolume)
       ||(Local.OldBrightVolume != LocalCfg.BrightVolume)
       )
     {
      //xtm_printf("CheckBrightVolume:: Local.OldVoiceHint = %d, LocalCfg.VoiceHint = %d, Local.OldSpkVolume = %d, LocalCfg.SpkVolume = %d,\n");
      Local.OldVoiceHint = LocalCfg.VoiceHint;         //语音提示  0 关闭  1  打开
      Local.OldSpkVolume = LocalCfg.SpkVolume;         //SPK 音量
      Local.OldMicVolume = LocalCfg.MicVolume;         //MIC 音量
      Local.OldTalkVoiceOpened = LocalCfg.TalkVoiceOpened;         //对讲时音量  0 关闭  1  打开
      Local.OldTalkVolume = LocalCfg.TalkVolume;        //对讲时音量
      Local.OldBrightVolume = LocalCfg.BrightVolume;      //亮度
      SaveOK = 1;
     }
  if(SaveOK == 1)
   {
    WriteCfgFile();  //写本地设置文件
    printf("CheckBrightVolume::Write\n");
   }
}
//---------------------------------------------------------------------------
//清状态提示信息函数
void ClearTipFunc(void)
{
  uint8_t CloseLcd;
  if(Local.Status != 0)
      Local.KeyPressTime = 0;
  //如不在开机界面，则计时，30S无操作自动返回
  //不在呼叫和操作界面    不在用户监视界面  不在报警界面 不在取消报警界面   不在 测试界面
  if((Local.CurrentWindow != 0)&&(Local.CurrentWindow != 34)&&(Local.CurrentWindow != 35)&&(Local.CurrentWindow != 192)
    &&(Local.Status == 0))
   {
    Local.KeyPressTime ++;
    if(Local.KeyPressTime > (IDLE_TIME*TIMERPERSEC))   //10*30
     {
      Local.KeyPressTime = 0;
      Local.CurrentWindow = 0;                                             
      DisplayFirstWindow();
     }
   }
  //如在开机界面，60S无操作关LCD
  if(Local.CurrentWindow == 0)
   {
    if(Local.LcdLightFlag == 1)
     {
      Local.LcdLightTime ++;
      if(Local.LcdLightTime > (CLOSELCD_TIME*TIMERPERSEC))
       {
        CloseLcd = 1;

        if(CloseLcd == 1)
         {
          if(Local.LcdLightFlag == 1)
           {
            //关闭LCD
            xtm_printf("Lcd_Power 0\n");
            Lcd_Power(0, LocalCfg.BrightVolume);
            if(Local.CurrentWindow != 0)
             {
              Local.CurrentWindow = 0;
              DisplayFirstWindow();
             }
           }
		  printf("ClearTipFunc::Local.LcdLightFlag = %d\n", Local.LcdLightFlag);
          Local.LcdLightFlag = 0; //LCD背光标志
          Local.LcdLightTime = 0; //时间
        }  
       }
     }
   }
  else
   {
    if(Local.LcdLightFlag == 0)
     {
      //点亮LCD背光
      Lcd_Power(1, LocalCfg.BrightVolume);
     }
    Local.LcdLightFlag = 1; //LCD背光标志
    Local.LcdLightTime = 0; //时间
   }

  //延时清提示信息标志
  if(PicStatBuf.Flag == 1)
   {
    PicStatBuf.Time ++;
    //1S
    if(PicStatBuf.Time >= (TIMERPERSEC*1))
     {
      PicStatBuf.Flag = 0;
      PicStatBuf.Time = 0;
      switch(PicStatBuf.Type)
       {
        case 16:   //显示对讲图像操作窗口
                  //有报警
                  xtm_printf("Local.AlarmByte[0] = 0x%X, Local.AlarmByte[1] = 0x%X\n", Local.AlarmByte[0], Local.AlarmByte[1]);
                  CloseVideoWindow();
                  if((Local.AlarmByte[0] != 0)||(Local.AlarmByte[1] != 0))
                     DisplayAlarmWindow(REFRESH_TOP); //报警窗口
                  else
                   {
                    DisplayFirstWindow();
                   }
                  Local.OnlineFlag = 0; 
                  Local.Status = 0;  //状态为空闲
                break;
       }
      PicStatBuf.Type = 0;

      Local.Status = 0;  //状态为空闲
      Local.PreStatus = 0;  //20130115 预状态，防止同时的操作      
     }
   }
}
//---------------------------------------------------------------------------
void OnlineCheckFunc(void) //在线确认检测函数
{
  uint8_t sendb[128], DenIP[4];
  uint16_t sendlength;
  char RemoteIP[20];

  if((Local.Status != 21)&&(Local.Status != 22)&&(Local.Status != 23))
   {
    if(Local.Timer1Num >=(TIMERPERSEC*6))
     {
      if(Local.CallConfirmFlag == 0)
       {
         if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
           ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10)) //对讲
             TalkEnd_Func();
         if((Local.Status == 3)||(Local.Status == 4))  //监视
             WatchEnd_Func();
         //为防止铃音一直响
         //StopPlayWavFile();  //关闭铃音
         xtm_printf("没有收到在线确认，强制结束, Local.Timer1Num = %d, Local.Status = %d, Local.CallConfirmFlag = %d\n", \
                     Local.Timer1Num, Local.Status, Local.CallConfirmFlag);         
         if(Local.Status == 0)
          {
           Local.OnlineFlag = 0;
           DisplayFirstWindow();
          }
       }
      else
        Local.CallConfirmFlag = 0;
      Local.Timer1Num = 0;
     }
    else
     if((Local.Timer1Num % TIMERPERSEC)==0)
     {
      //对讲时被叫方发送在线确认包，每秒一个
      //监控时主控方发送在线确认包，每秒一个
    //  printf("Local.Status = %d\n", Local.Status);
      if((Local.Status == 2)||(Local.Status == 6)
        ||(Local.Status == 8)||(Local.Status == 10)
        ||(Local.Status == 3))
       {
        //头部
        memcpy(sendb, UdpPackageHead, 6);
        //命令
        if((Local.Status == 2)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))  //对讲
          sendb[6] = VIDEOTALK;
        if(Local.Status == 3)  //监视
          sendb[6] = VIDEOWATCH;

        sendb[7]=ASK;        //主叫
        sendb[8]=CALLCONFIRM;//通话在线确认
        //子命令
        if(Local.Status == 3) //监视时，本机为主叫方
         {
          memcpy(sendb+9, LocalCfg.Addr, 20);
          memcpy(sendb+29, LocalCfg.IP, 4);
          memcpy(sendb+33, Remote.Addr[0], 20);
          memcpy(sendb+53, Remote.IP[0], 4);
         }
        //printf("Remote.IP[0] = %d.%d.%d.%d\n", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
        if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6)
          ||(Local.Status == 8)||(Local.Status == 10))  //对讲时，本机为被叫方
         {
          memcpy(sendb+9, Remote.Addr[0], 20);
          memcpy(sendb+29, Remote.IP[0], 4);
          memcpy(sendb+33, LocalCfg.Addr, 20);
          memcpy(sendb+53, LocalCfg.IP, 4);
         }
        //确认序号
        sendb[60] = (Local.OnlineNum & 0xFF000000) >> 24;
        sendb[59] = (Local.OnlineNum & 0x00FF0000) >> 16;
        sendb[58] = (Local.OnlineNum & 0x0000FF00) >> 8;
        sendb[57] = Local.OnlineNum & 0x000000FF;
        Local.OnlineNum ++;
        sendlength=61;

		sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);

        CreateMultiUdpBuff(UDP_VideoSocket, SINGLE_SEND, RemoteIP, LocalPort[1], sendb, sendlength, 0);

       }
     }
    Local.Timer1Num ++;
   }
}
//---------------------------------------------------------------------------
void TimeReportStatusFunc(void) //设备定时报告状态函数
{
  uint8_t k;
  uint16_t sendlength;
  uint8_t sendb[1520];
  char RemoteIP[20];

  //头部
  memcpy(sendb, UdpPackageHead, 6);
  //命令
  sendb[6] = REPORTSTATUS;

  sendb[7]=ASK;        //主叫
  //子命令
  memcpy(sendb+8, LocalCfg.Addr, 20);
  memcpy(sendb+28, LocalCfg.Mac_Addr, 6);

  sendb[34] = LocalCfg.DefenceStatus;
  sendb[35] = LocalCfg.DefenceNum;
  //默认为10个防区
  #define _DEFENCENUM  10
  for(k=0; k<10; k++)
    memcpy(sendb + 36 + _DEFENCENUM*k, LocalCfg.DefenceInfo[k], 10);

  sendlength = 36 + _DEFENCENUM*10;
  //xtm_printf("TimeReportStatusFunc...\n");
  sprintf(RemoteIP, "%d.%d.%d.%d", LocalCfg.IP_Server[0], LocalCfg.IP_Server[1], LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
  //sprintf(RemoteIP, "%d.%d.%d.%d", 192, 168, 1, 255);
  //sprintf(RemoteIP, "%d.%d.%d.%d", 255, 255, 255, 255);
  CreateMultiUdpBuff(UDP_DataSocket, MULTI_SEND, RemoteIP, LocalPort[0], sendb, sendlength, 0);  
}
//---------------------------------------------------------------------------
void TalkCtrlFunc(void)  //对讲控制，显示通话时间和判断超时
{
  int CallTimeOut;
  //1S
  if((Local.TimeOut % TIMERPERSEC)==0)
   {
    switch(Local.Status)
     {
      case 1:  //主叫对讲
      case 2:  //被叫对讲
             CallTimeOut = CALLTIMEOUT;
             //在被呼叫时，主机有留影功能，故主机超时值比副机少3S
             if((LocalCfg.Addr[0] == 'S')||(LocalCfg.Addr[0] == 'B'))
              {
               if(Local.isHost == '0')
                 CallTimeOut = CALLTIMEOUT;
               else
                 CallTimeOut = CALLTIMEOUT + 3*TIMERPERSEC;
              }

			 //5秒后接听
			 if(Local.Status == 2)
				 if(Local.TimeOut > 5*TIMERPERSEC)
				 	Talk_Func(); 
				 
             if(Local.TimeOut > CallTimeOut)
              {
               //查看是否在其它组播组内
               //DropMultiGroup(m_VideoSocket, NULL);

               //呼叫超时
               CallTimeOut_Func();
               Local.TimeOut = 0;
              }

            #ifdef _REMOTECALLTEST  //远程呼叫测试
             if(Local.EnterTestMode == 1)  //测试模式, 5秒自动接听
              //振铃5秒自动接听
              if(Local.Status == 2)
               if(Local.TimeOut > 5*TIMERPERSEC)
                {
                 Local.EnterTestMode = 0;  //测试模式, 5秒自动接听;
                 //被叫对讲
                 OperateTalkWatchPicWindow(0, 16);
                 break;
                }
            #endif
             break;
      case 5:  //主叫通话
      case 6:  //被叫通话
             //计时
               /*sprintf(talkpic_osd.Text, "%02d:%02d\0", Local.TimeOut/TIMERPERSEC/60,
                      (Local.TimeOut/TIMERPERSEC)%60);
               SetOsdPos(&talkpic_osd);
               ShowOsd(0, &talkpic_osd, REFRESH);  */

             if(Local.TimeOut > Local.TalkTimeOut)
              {
               Local.TalkTimeOut = TALKTIMEOUT; //通话最长时间
               TalkEnd_Func();
               if(Local.Status == 0)
                 Local.OnlineFlag = 0;
               Local.TimeOut = 0;
               xtm_printf("通话超时\n");
              }
             break;
      case 3:  //监视    计时
               if(Local.TimeOut > WATCHTIMEOUT)
                {
                 WatchEnd_Func();
                 if(Local.Status == 0)
                   Local.OnlineFlag = 0;
                 Local.TimeOut = 0;
                 xtm_printf("监视超时\n");
                }
               break;
      case 4:  //被监视
             if(Local.TimeOut > WATCHTIMEOUT)
              {
               WatchEnd_Func();
               if(Local.Status == 0)
                 Local.OnlineFlag = 0;
               Local.TimeOut = 0;
               xtm_printf("监视超时\n");
              }
             break;
     }
    }
   Local.TimeOut ++;       //监视超时,  通话超时,  呼叫超时，无人接听
}
//---------------------------------------------------------------------------
//检测网络信号
void CheckEth(void)
{
  if(Local.NetLanStatus != Local.OldNetLanStatus)   //网络状态 0 断开  1 接通
   {
    Local.OldNetLanStatus = Local.NetLanStatus;
    if (Local.NetLanStatus == 0)
     {
      //SetGpio(GPIOD_OUT_DATA, NET_LED_HIGH, SET_HIGH); //关网络LED
      if(Local.CurrentWindow == 0)
        DisplayImage(&state_image[0], FROMFILE, NOFULLPAGE);
     }

    if (Local.NetLanStatus == 1)
     {
      //SetGpio(GPIOD_OUT_DATA, NET_LED_LOW, SET_LOW); //开网络LED
      if(Local.CurrentWindow == 0)
        DisplayImage(&state_image[1], FROMFILE, NOFULLPAGE);
      //发送免费ARP
      //SendFreeArp();
     }
   } 
}
//---------------------------------------------------------------------------
#ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
void FindSubFunc(void) //为主机，定时查找副机
{
  uint8_t i, j, sendlength, isExist;
  uint8_t sendb[128];
  char RemoteIP[20];
  Local.TmpDenNum = Local.DenNum; //更新副机数量
  //暂定副机最多为3个
  for(j=0; j<LocalCfg.DenMaxNum; j++)
   {
      //头部
      memcpy(sendb, UdpPackageHead, 6);
      //命令  ,子网广播解析
      sendb[6] = HOSTTOSUB;
      sendb[7] = ASK;    //主叫
      sendb[8] = SUBFIND;

      memcpy(sendb+9, LocalCfg.Addr,20);
      memcpy(sendb+29, LocalCfg.IP,4);
      memcpy(sendb+33, LocalCfg.Addr, 20);
      if(LocalCfg.Addr[0] == 'S')
        sendb[33+11] = j + '1';
      if(LocalCfg.Addr[0] == 'B')
        sendb[33+11] = j + '1';
      sendb[53] = 0;
      sendb[54] = 0;
      sendb[55] = 0;
      sendb[56] = 0;

      sendlength = 57;

      //查找该副机是否已经存在，如存在则无需发送查找命令
      isExist = 0;
      for(i=0; i<LocalCfg.DenMaxNum; i++)  
       {
        //xtm_printf("Local.DenAddr[%d][11] = %d, sendb[44] = %d\n", i, Local.DenAddr[i][11] - '0', sendb[44] - '0');
        if(Local.DenAddr[i][11] == sendb[44])
         {
          isExist = 1;
          break;
         }
       }
      if(!isExist){
	  	sprintf(RemoteIP, "%d.%d.%d.%d", LocalCfg.IP_Broadcast[0], LocalCfg.IP_Broadcast[1], LocalCfg.IP_Broadcast[2], LocalCfg.IP_Broadcast[3]);
        CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, 0);
      	}
   }
}
//---------------------------------------------------------------------------
uint8_t RefreshCount = 0;
void RefreshDen(void)  //更新副机数量和地址
{
  uint8_t i,j;
  uint8_t tmp_i;
  uint8_t DenChange;
  DenChange = 0;

  if(Local.TmpDenNum > Local.DenNum)
    DenChange = 1;
  if(Local.TmpDenNum < Local.DenNum)
   {
    RefreshCount ++;
    if(RefreshCount > 3)
      DenChange = 1;
   }
  if(Local.TmpDenNum == Local.DenNum)
    for(i=0; i<Local.TmpDenNum; i++)
     if(strcmp(Local.DenAddr[i], Local.TmpDenAddr[i]) != 0)
      {
       DenChange = 1;
       break;
      }
  if(Local.TmpDenNum == Local.DenNum)
    for(i=0; i<Local.TmpDenNum; i++)
     if((Local.DenIP[i][0] != Local.TmpDenIP[i][0])||(Local.DenIP[i][1] != Local.TmpDenIP[i][1])
        ||(Local.DenIP[i][2] != Local.TmpDenIP[i][2])||(Local.DenIP[i][3] != Local.TmpDenIP[i][3]))
      {
       DenChange = 1;
       break;
      }
  if(DenChange == 1)
   {
    RefreshCount = 0;
    tmp_i = 0;
    for(i=0; i<SUBMAXNUM; i++)
     {
      if(tmp_i >= Local.TmpDenNum)
        break;
      for(j=0; j<Local.TmpDenNum; j++)
       {
        if(Local.TmpDenAddr[j][Local.AddrLen-1] == ('1' + i))
         {
          memcpy(Local.DenAddr[tmp_i], Local.TmpDenAddr[j], sizeof(Local.DenAddr[tmp_i]));
          memcpy(Local.DenIP[tmp_i], Local.TmpDenIP[j], sizeof(Local.DenIP[tmp_i]));
          Local.DenFlag[tmp_i] = 1;
          tmp_i ++;
          break;
         }
       }
     }
    Local.DenNum = Local.TmpDenNum;
   }
  if(Local.DenNum > 0)
   {
     xtm_printf("RefreshDen::Local.DenNum = %d, Local.DenIP[0] = %d.%d.%d.%d, Local.DenIP[1] = %d.%d.%d.%d\n",
             Local.DenNum, Local.DenIP[0][0], Local.DenIP[0][1], Local.DenIP[0][2], Local.DenIP[0][3],
                Local.DenIP[1][0], Local.DenIP[1][1], Local.DenIP[1][2], Local.DenIP[1][3]);
   }
}
//---------------------------------------------------------------------------
void SyncSubFunc(void) //为主机，定时与副机同步状态
{
  uint8_t i,j;
  uint16_t sendlength;
  uint8_t sendb[1024];
  char RemoteIP[20];
  struct SyncSubData1 SubData1;
  if(Local.DenNum > 0)
   {
    SubData1.DenNum = Local.DenNum;             //目标数量  副机
    memcpy(SubData1.DenIP, Local.DenIP, sizeof(Local.DenIP));    //副机IP
    memcpy(SubData1.DenAddr, Local.DenAddr, sizeof(Local.DenAddr));    //副机Addr
               //以下设置 传到副机  *************
    SubData1.DefenceStatus = LocalCfg.DefenceStatus;       //布防状态
    SubData1.DefenceNum = LocalCfg.DefenceNum;          //防区模块个数
    memcpy(SubData1.DefenceInfo, LocalCfg.DefenceInfo, sizeof(LocalCfg.DefenceInfo));    //防区信息

    memcpy(SubData1.EngineerPass, LocalCfg.EngineerPass, sizeof(LocalCfg.EngineerPass));    //工程密码
//    printf("syncsub1 size = %d\n", sizeof(struct SyncSubData1));

    //暂定副机最多为3个
    for(j=0; j<Local.DenNum; j++)
      {
       //头部
       memcpy(sendb, UdpPackageHead, 6);
       //命令  ,子网广播解析
       sendb[6] = HOSTTOSUB;
       sendb[7] = ASK;    //主叫
       sendb[8] = SYNCSUB;    //同步状态
       sendb[9] = 1;    //数据包编号

       memcpy(sendb+10, LocalCfg.Addr,20);
       memcpy(sendb+30, LocalCfg.IP,4);

       memcpy(sendb+34, &SubData1, sizeof(struct SyncSubData1));
       sendlength = 34 + sizeof(struct SyncSubData1);

	   sprintf(RemoteIP, "%d.%d.%d.%d", Local.DenIP[j][0], Local.DenIP[j][1], Local.DenIP[j][2], Local.DenIP[j][3]);

       CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, 0);
      }
   }
}
#endif
//---------------------------------------------------------------------------

