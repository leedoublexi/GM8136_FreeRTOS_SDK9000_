
#include "stdlib.h"

#define x_timer_h

#define CommonH
#include "common.h"

uint16_t Timer_Num = 0;  //����
void timer_init(void);
void timer_thread_func(void);

void CheckBrightVolume(void); //�����������������ֵ


void OnlineCheckFunc(void); //����ȷ�ϼ�⺯��
void TimeReportStatusFunc(void); //�豸��ʱ����״̬����
void ClearTipFunc(void);    //��״̬��ʾ��Ϣ����
void TalkCtrlFunc(void);  //�Խ����ƣ���ʾͨ��ʱ����жϳ�ʱ

//��������ź�
void CheckEth(void);
int net_recv_count = 0;

#ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
#define REFRESHTIME  30
void FindSubFunc(void); //Ϊ��������ʱ���Ҹ���
void SyncSubFunc(void); //Ϊ��������ʱ�븱��ͬ��״̬
void RefreshDen(void);  //���¸��������͵�ַ
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

		 //�洢���Ⱥ�����ֵ
		if(((Timer_Num % (TIMERPERSEC*10))==0)&&(Timer_Num != 0))
		 {
		  //xtm_printf("VideoCount = %d\n", VideoCount);
		  #ifdef _XTM8900_SUPPORT  //XTM8900����֧��
    		net_curr_count = net_get_recvcount();
		  #endif
		  #ifdef _XTM9000_SUPPORT  //XTM9000����֧��
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
		  if(Local.WriteCfgFlag == 1)	  //20131025 ��ʼ��ʱдCfg ��־
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
		
	
		//����6��û���յ�����ȷ�ϣ�����Ϊ����
		if(Local.OnlineFlag == 1)
		 {
		  OnlineCheckFunc();
		  //�Խ����ƣ���ʾͨ��ʱ����жϳ�ʱ
		  TalkCtrlFunc();
		 }
		
		//��״̬��ʾ��Ϣ����
		ClearTipFunc();
		
		//�豸��ʱ����״̬����
		if(LocalCfg.ReportTime != 0)
		  {
		   if(Local.ReportSend == 1)
			if(Local.ReportTimeNum >= (LocalCfg.ReportTime*TIMERPERSEC))
			 {
			  Local.RandReportTime = rand() % LocalCfg.ReportTime;	  //���������
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
		#ifdef _MULTI_MACHINE_SUPPORT	//��ֻ�֧��
			//Ϊ��������ʱ���Ҹ���	30S
			if(LocalCfg.MultiMachineSupport == 1) 
			 {
			  if(Local.isHost == '0')
			   {
				if(Local.Status == 0)
				 if((Timer_Num % (TIMERPERSEC*REFRESHTIME))==0)
				  {
		
				   if(LocalCfg.DenMaxNum != Local.DenNum)  //��󸱻������뵱ǰ������ͬ���������
					{
					 RefreshDenFlag = 1;
					 Local.TmpDenNum = Local.DenNum; //���¸�������
					 FindSubFunc();
					}
				   Local.SendSyncFlag = 1;	//����ͬ����Ϣ��־
				   Local.SendSyncTime = 0;	//����ͬ����Ϣ����
				  }
				if(Local.SendSyncFlag == 1)
				 {
				  if(Local.SendSyncTime > TIMERPERSEC*2)
				   {
					if(RefreshDenFlag == 1)
					 {
					  RefreshDenFlag = 0;
					  RefreshDen();  //���¸��������͵�ַ
					 }
					if(Local.DenNum > 0)
					  SyncSubFunc(); //Ϊ��������ʱ�븱��ͬ��״̬
					Local.SendSyncFlag = 0;  //����ͬ����Ϣ��־
					Local.SendSyncTime = 0;  //����ͬ����Ϣ����
				   }
				  Local.SendSyncTime ++;
				 }
			   }
			  else	 //Ϊ����, ��ʱ�ж�����������
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
	 
	 CheckDefenceDelay_Func(); //��鲼����ʱ����	 	   
	 		  
     //�����ʱ��������
	 CheckDelayAlarm();
		 
	 //��鷵����ҳ��־
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
void CheckBrightVolume(void) //�����������������ֵ
{
  uint8_t i, j;
  uint8_t SaveOK = 0;


  //��������    
  if((Local.OldVoiceHint != LocalCfg.VoiceHint)
       ||(Local.OldSpkVolume != LocalCfg.SpkVolume)
       ||(Local.OldMicVolume != LocalCfg.MicVolume)
       ||(Local.OldTalkVoiceOpened != LocalCfg.TalkVoiceOpened)
       ||(Local.OldTalkVolume != LocalCfg.TalkVolume)
       ||(Local.OldBrightVolume != LocalCfg.BrightVolume)
       )
     {
      //xtm_printf("CheckBrightVolume:: Local.OldVoiceHint = %d, LocalCfg.VoiceHint = %d, Local.OldSpkVolume = %d, LocalCfg.SpkVolume = %d,\n");
      Local.OldVoiceHint = LocalCfg.VoiceHint;         //������ʾ  0 �ر�  1  ��
      Local.OldSpkVolume = LocalCfg.SpkVolume;         //SPK ����
      Local.OldMicVolume = LocalCfg.MicVolume;         //MIC ����
      Local.OldTalkVoiceOpened = LocalCfg.TalkVoiceOpened;         //�Խ�ʱ����  0 �ر�  1  ��
      Local.OldTalkVolume = LocalCfg.TalkVolume;        //�Խ�ʱ����
      Local.OldBrightVolume = LocalCfg.BrightVolume;      //����
      SaveOK = 1;
     }
  if(SaveOK == 1)
   {
    WriteCfgFile();  //д���������ļ�
    printf("CheckBrightVolume::Write\n");
   }
}
//---------------------------------------------------------------------------
//��״̬��ʾ��Ϣ����
void ClearTipFunc(void)
{
  uint8_t CloseLcd;
  if(Local.Status != 0)
      Local.KeyPressTime = 0;
  //�粻�ڿ������棬���ʱ��30S�޲����Զ�����
  //���ں��кͲ�������    �����û����ӽ���  ���ڱ������� ����ȡ����������   ���� ���Խ���
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
  //���ڿ������棬60S�޲�����LCD
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
            //�ر�LCD
            xtm_printf("Lcd_Power 0\n");
            Lcd_Power(0, LocalCfg.BrightVolume);
            if(Local.CurrentWindow != 0)
             {
              Local.CurrentWindow = 0;
              DisplayFirstWindow();
             }
           }
		  printf("ClearTipFunc::Local.LcdLightFlag = %d\n", Local.LcdLightFlag);
          Local.LcdLightFlag = 0; //LCD�����־
          Local.LcdLightTime = 0; //ʱ��
        }  
       }
     }
   }
  else
   {
    if(Local.LcdLightFlag == 0)
     {
      //����LCD����
      Lcd_Power(1, LocalCfg.BrightVolume);
     }
    Local.LcdLightFlag = 1; //LCD�����־
    Local.LcdLightTime = 0; //ʱ��
   }

  //��ʱ����ʾ��Ϣ��־
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
        case 16:   //��ʾ�Խ�ͼ���������
                  //�б���
                  xtm_printf("Local.AlarmByte[0] = 0x%X, Local.AlarmByte[1] = 0x%X\n", Local.AlarmByte[0], Local.AlarmByte[1]);
                  CloseVideoWindow();
                  if((Local.AlarmByte[0] != 0)||(Local.AlarmByte[1] != 0))
                     DisplayAlarmWindow(REFRESH_TOP); //��������
                  else
                   {
                    DisplayFirstWindow();
                   }
                  Local.OnlineFlag = 0; 
                  Local.Status = 0;  //״̬Ϊ����
                break;
       }
      PicStatBuf.Type = 0;

      Local.Status = 0;  //״̬Ϊ����
      Local.PreStatus = 0;  //20130115 Ԥ״̬����ֹͬʱ�Ĳ���      
     }
   }
}
//---------------------------------------------------------------------------
void OnlineCheckFunc(void) //����ȷ�ϼ�⺯��
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
           ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10)) //�Խ�
             TalkEnd_Func();
         if((Local.Status == 3)||(Local.Status == 4))  //����
             WatchEnd_Func();
         //Ϊ��ֹ����һֱ��
         //StopPlayWavFile();  //�ر�����
         xtm_printf("û���յ�����ȷ�ϣ�ǿ�ƽ���, Local.Timer1Num = %d, Local.Status = %d, Local.CallConfirmFlag = %d\n", \
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
      //�Խ�ʱ���з���������ȷ�ϰ���ÿ��һ��
      //���ʱ���ط���������ȷ�ϰ���ÿ��һ��
    //  printf("Local.Status = %d\n", Local.Status);
      if((Local.Status == 2)||(Local.Status == 6)
        ||(Local.Status == 8)||(Local.Status == 10)
        ||(Local.Status == 3))
       {
        //ͷ��
        memcpy(sendb, UdpPackageHead, 6);
        //����
        if((Local.Status == 2)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))  //�Խ�
          sendb[6] = VIDEOTALK;
        if(Local.Status == 3)  //����
          sendb[6] = VIDEOWATCH;

        sendb[7]=ASK;        //����
        sendb[8]=CALLCONFIRM;//ͨ������ȷ��
        //������
        if(Local.Status == 3) //����ʱ������Ϊ���з�
         {
          memcpy(sendb+9, LocalCfg.Addr, 20);
          memcpy(sendb+29, LocalCfg.IP, 4);
          memcpy(sendb+33, Remote.Addr[0], 20);
          memcpy(sendb+53, Remote.IP[0], 4);
         }
        //printf("Remote.IP[0] = %d.%d.%d.%d\n", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
        if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6)
          ||(Local.Status == 8)||(Local.Status == 10))  //�Խ�ʱ������Ϊ���з�
         {
          memcpy(sendb+9, Remote.Addr[0], 20);
          memcpy(sendb+29, Remote.IP[0], 4);
          memcpy(sendb+33, LocalCfg.Addr, 20);
          memcpy(sendb+53, LocalCfg.IP, 4);
         }
        //ȷ�����
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
void TimeReportStatusFunc(void) //�豸��ʱ����״̬����
{
  uint8_t k;
  uint16_t sendlength;
  uint8_t sendb[1520];
  char RemoteIP[20];

  //ͷ��
  memcpy(sendb, UdpPackageHead, 6);
  //����
  sendb[6] = REPORTSTATUS;

  sendb[7]=ASK;        //����
  //������
  memcpy(sendb+8, LocalCfg.Addr, 20);
  memcpy(sendb+28, LocalCfg.Mac_Addr, 6);

  sendb[34] = LocalCfg.DefenceStatus;
  sendb[35] = LocalCfg.DefenceNum;
  //Ĭ��Ϊ10������
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
void TalkCtrlFunc(void)  //�Խ����ƣ���ʾͨ��ʱ����жϳ�ʱ
{
  int CallTimeOut;
  //1S
  if((Local.TimeOut % TIMERPERSEC)==0)
   {
    switch(Local.Status)
     {
      case 1:  //���жԽ�
      case 2:  //���жԽ�
             CallTimeOut = CALLTIMEOUT;
             //�ڱ�����ʱ����������Ӱ���ܣ���������ʱֵ�ȸ�����3S
             if((LocalCfg.Addr[0] == 'S')||(LocalCfg.Addr[0] == 'B'))
              {
               if(Local.isHost == '0')
                 CallTimeOut = CALLTIMEOUT;
               else
                 CallTimeOut = CALLTIMEOUT + 3*TIMERPERSEC;
              }

			 //5������
			 if(Local.Status == 2)
				 if(Local.TimeOut > 5*TIMERPERSEC)
				 	Talk_Func(); 
				 
             if(Local.TimeOut > CallTimeOut)
              {
               //�鿴�Ƿ��������鲥����
               //DropMultiGroup(m_VideoSocket, NULL);

               //���г�ʱ
               CallTimeOut_Func();
               Local.TimeOut = 0;
              }

            #ifdef _REMOTECALLTEST  //Զ�̺��в���
             if(Local.EnterTestMode == 1)  //����ģʽ, 5���Զ�����
              //����5���Զ�����
              if(Local.Status == 2)
               if(Local.TimeOut > 5*TIMERPERSEC)
                {
                 Local.EnterTestMode = 0;  //����ģʽ, 5���Զ�����;
                 //���жԽ�
                 OperateTalkWatchPicWindow(0, 16);
                 break;
                }
            #endif
             break;
      case 5:  //����ͨ��
      case 6:  //����ͨ��
             //��ʱ
               /*sprintf(talkpic_osd.Text, "%02d:%02d\0", Local.TimeOut/TIMERPERSEC/60,
                      (Local.TimeOut/TIMERPERSEC)%60);
               SetOsdPos(&talkpic_osd);
               ShowOsd(0, &talkpic_osd, REFRESH);  */

             if(Local.TimeOut > Local.TalkTimeOut)
              {
               Local.TalkTimeOut = TALKTIMEOUT; //ͨ���ʱ��
               TalkEnd_Func();
               if(Local.Status == 0)
                 Local.OnlineFlag = 0;
               Local.TimeOut = 0;
               xtm_printf("ͨ����ʱ\n");
              }
             break;
      case 3:  //����    ��ʱ
               if(Local.TimeOut > WATCHTIMEOUT)
                {
                 WatchEnd_Func();
                 if(Local.Status == 0)
                   Local.OnlineFlag = 0;
                 Local.TimeOut = 0;
                 xtm_printf("���ӳ�ʱ\n");
                }
               break;
      case 4:  //������
             if(Local.TimeOut > WATCHTIMEOUT)
              {
               WatchEnd_Func();
               if(Local.Status == 0)
                 Local.OnlineFlag = 0;
               Local.TimeOut = 0;
               xtm_printf("���ӳ�ʱ\n");
              }
             break;
     }
    }
   Local.TimeOut ++;       //���ӳ�ʱ,  ͨ����ʱ,  ���г�ʱ�����˽���
}
//---------------------------------------------------------------------------
//��������ź�
void CheckEth(void)
{
  if(Local.NetLanStatus != Local.OldNetLanStatus)   //����״̬ 0 �Ͽ�  1 ��ͨ
   {
    Local.OldNetLanStatus = Local.NetLanStatus;
    if (Local.NetLanStatus == 0)
     {
      //SetGpio(GPIOD_OUT_DATA, NET_LED_HIGH, SET_HIGH); //������LED
      if(Local.CurrentWindow == 0)
        DisplayImage(&state_image[0], FROMFILE, NOFULLPAGE);
     }

    if (Local.NetLanStatus == 1)
     {
      //SetGpio(GPIOD_OUT_DATA, NET_LED_LOW, SET_LOW); //������LED
      if(Local.CurrentWindow == 0)
        DisplayImage(&state_image[1], FROMFILE, NOFULLPAGE);
      //�������ARP
      //SendFreeArp();
     }
   } 
}
//---------------------------------------------------------------------------
#ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
void FindSubFunc(void) //Ϊ��������ʱ���Ҹ���
{
  uint8_t i, j, sendlength, isExist;
  uint8_t sendb[128];
  char RemoteIP[20];
  Local.TmpDenNum = Local.DenNum; //���¸�������
  //�ݶ��������Ϊ3��
  for(j=0; j<LocalCfg.DenMaxNum; j++)
   {
      //ͷ��
      memcpy(sendb, UdpPackageHead, 6);
      //����  ,�����㲥����
      sendb[6] = HOSTTOSUB;
      sendb[7] = ASK;    //����
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

      //���Ҹø����Ƿ��Ѿ����ڣ�����������跢�Ͳ�������
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
void RefreshDen(void)  //���¸��������͵�ַ
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
void SyncSubFunc(void) //Ϊ��������ʱ�븱��ͬ��״̬
{
  uint8_t i,j;
  uint16_t sendlength;
  uint8_t sendb[1024];
  char RemoteIP[20];
  struct SyncSubData1 SubData1;
  if(Local.DenNum > 0)
   {
    SubData1.DenNum = Local.DenNum;             //Ŀ������  ����
    memcpy(SubData1.DenIP, Local.DenIP, sizeof(Local.DenIP));    //����IP
    memcpy(SubData1.DenAddr, Local.DenAddr, sizeof(Local.DenAddr));    //����Addr
               //�������� ��������  *************
    SubData1.DefenceStatus = LocalCfg.DefenceStatus;       //����״̬
    SubData1.DefenceNum = LocalCfg.DefenceNum;          //����ģ�����
    memcpy(SubData1.DefenceInfo, LocalCfg.DefenceInfo, sizeof(LocalCfg.DefenceInfo));    //������Ϣ

    memcpy(SubData1.EngineerPass, LocalCfg.EngineerPass, sizeof(LocalCfg.EngineerPass));    //��������
//    printf("syncsub1 size = %d\n", sizeof(struct SyncSubData1));

    //�ݶ��������Ϊ3��
    for(j=0; j<Local.DenNum; j++)
      {
       //ͷ��
       memcpy(sendb, UdpPackageHead, 6);
       //����  ,�����㲥����
       sendb[6] = HOSTTOSUB;
       sendb[7] = ASK;    //����
       sendb[8] = SYNCSUB;    //ͬ��״̬
       sendb[9] = 1;    //���ݰ����

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

