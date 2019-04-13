
#include "stdlib.h"
#include "lib_spi.h"


#define x_main_h

#define CommonH
#include "common.h"

void Init_LocalVar(void); //��ʼ�� Local �ṹ

void ResetLocalCfg(void);    //��λ�����ļ�



uint8_t *Flash_Stream;        //Flash�洢


spi_platform_setting_t flash_s;
u8* flash_rd_buf;
u8* flash_wt_buf;

void flash_init(void);

//�������ļ�
void ReadCfgFile(void);
//д�����ļ�
void WriteCfgFile(void);
//д���ݵ�Flash
void SaveToFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length);
//��Flash������
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

void Init_LocalVar(void) //��ʼ�� Local �ṹ
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

  ReadCfgFile();    //�����������ļ�
  
  Local.NetLanStatus = 0;   //����״̬ 0 �Ͽ�  1 ��ͨ
  Local.OldNetLanStatus = 0;   //����״̬ 0 �Ͽ�  1 ��ͨ
  Local.OldNetSpeed = 100;  //�����ٶ�
  Local.NewInfo = 0;  //������Ϣ
  Local.NewRecPic = 0;  //������Ӱ
  
  Local.RecordPic = 0;  //����Ƭ  0 ����
  Local.IFrameCount = 0; //I֡����
  Local.IFrameNo = 125;    //���ڼ���I֡

  Local.ForceEndWatch = 0;  //�к���ʱ��ǿ�ƹؼ���


  Local.LcdLightFlag = 1; //LCD�����־
  Local.LcdLightTime = 0; //ʱ��
  Local.KeyPressTime = 0;

  LocalCfg.VoiceHint = 1;
  Local.OldVoiceHint = LocalCfg.VoiceHint;         //������ʾ  0 �ر�  1  ��
  Local.OldSpkVolume = LocalCfg.SpkVolume;         //SPK ����
  Local.OldMicVolume = LocalCfg.MicVolume;         //MIC ����
  Local.OldTalkVoiceOpened = LocalCfg.TalkVoiceOpened;         //�Խ�ʱ����  0 �ر�  1  ��
  Local.OldTalkVolume = LocalCfg.TalkVolume;        //�Խ�ʱ����
  for(i=0; i<3; i++)
    Local.OldScrSave[i] =  LocalCfg.ScrSave[i];        //��Ļ����  0 �ر�  1  ��

  if(LocalCfg.BrightVolume > 4)
    LocalCfg.BrightVolume = 3;  

  Local.OldBrightVolume = LocalCfg.BrightVolume;      //����
  Local.OldKeyVoice = LocalCfg.KeyVoice;          //������    0 �ر�  1  ��

/*  
  //��������
  for(i=0; i<MAXDEFENCENUM; i++)
  {
    Local.OldDefenceSetup[i].dType = LocalCfg.DefenceSetup[i].dType;
    Local.OldDefenceSetup[i].dDelay = LocalCfg.DefenceSetup[i].dDelay;
    Local.OldDefenceSetup[i].tType = LocalCfg.DefenceSetup[i].tType;
  }
 */

  //20101116 ������ʱ��Ϊ 100S
  //�����ʱ
  LocalCfg.Out_DelayTime = 30;//100;
  //������ʱ
  LocalCfg.Alarm_DelayTime = 30;
  if((LocalCfg.Addr[0] != 'S')&&(LocalCfg.Addr[0] != 'B'))
    LocalCfg.Addr[0] = 'B';

  if((LocalCfg.Addr[0] == 'S')||(LocalCfg.Addr[0] == 'B'))
  {
    Local.AddrLen = 12;  //��ַ����  S 12  B 12 M 8 H 6
  }

  LocalCfg.Addr[Local.AddrLen] = '\0';
  LocalCfg.AddrVerifyMa[6] = '\0';
  
  Local.Weather[0] = 1;  //����Ԥ��
  Local.Weather[1] = 20;//0;
  Local.Weather[2] = 27;//0;

  Local.ReportSend = 0;  //�豸��ʱ����״̬�ѷ���
  Local.RandReportTime = 1;
  Local.ReportTimeNum  = 0;

  Local.nowvideoframeno = 1;   //��ǰ��Ƶ֡���
  Local.nowaudioframeno = 1;   //��ǰ��Ƶ֡���  

  Local.NoBreak = 0;   //����״̬ 1 ����  0 ����
  //����
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

  //����״̬
  LocalCfg.DefenceNum = 1;
  //20101028  ���� ������Ϣ

  for(j=0; j<5; j++)
    for(i=0; i<MAXDEFENCENUM; i++)
    {
      LocalCfg.Scene_Defence[j][i] = Scene_Defence1[j][i];
      if(LocalCfg.Scene_Defence[j][i] == 1)
      {
        LocalCfg.DefenceInfo[i][2] = 1;  // ����
        LocalCfg.DefenceInfo[i][3] = 0;  // �ޱ���
      }
      else
      {
        LocalCfg.DefenceInfo[i][2] = 0;  // ����
        LocalCfg.DefenceInfo[i][3] = 0;  // �ޱ���
      }
    }

  //��������   0  1  2 - ��ʱ����   3  4  5  ��ͨ����   6  7  24Сʱ����
  for(i=0; i<=2; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 3;   //��������     0 - �շ���  1 - ҹ����  2 - 24Сʱ����  3 - �Զ������
    LocalCfg.DefenceSetup_dDelay[i] = 5;  //��ʱʱ��     0 - 0��   1 - 5��  2 - 10��  3 - 15��  4 - 20��  5 - 25��  6 - 60��
    LocalCfg.DefenceSetup_tType[i] = 4;   //̽ͷ����     0 - �̸�  1 - ȼ��  2 - ����  3 - ����  4 - ����  5 - �Ŵ�  6 - ����
  }
  
  for(i=3; i<=5; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 3;   //��������     0 - �շ���  1 - ҹ����  2 - 24Сʱ����  3 - �Զ������
    LocalCfg.DefenceSetup_dDelay[i] = 0;  //��ʱʱ��     0 - 0��   1 - 5��  2 - 10��  3 - 15��  4 - 20��  5 - 25��  6 - 60��
    LocalCfg.DefenceSetup_tType[i] = 4;   //̽ͷ����     0 - �̸�  1 - ȼ��  2 - ����  3 - ����  4 - ����  5 - �Ŵ�  6 - ����
  }
  
  for(i=6; i<=7; i++)
  //for(i=0; i<=7; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 2;   //��������     0 - �շ���  1 - ҹ����  2 - 24Сʱ����  3 - �Զ������
    LocalCfg.DefenceSetup_dDelay[i] = 0;  //��ʱʱ��     0 - 0��   1 - 5��  2 - 10��  3 - 15��  4 - 20��  5 - 25��  6 - 60��
    LocalCfg.DefenceSetup_tType[i] = 4;   //̽ͷ����     0 - �̸�  1 - ȼ��  2 - ����  3 - ����  4 - ����  5 - �Ŵ�  6 - ����
  }

  DeltaLen = 9 + sizeof(struct talkdata1);  //���ݰ���Ч����ƫ����

  Local.Status = 0;  //״̬Ϊ����
  Local.TalkEndStatus = 0;  //״̬Ϊ����

  Local.AlarmByte[0] = 0x0;         //����
  Local.AlarmByte[1] = 0x0;         //����
  Local.DefenceDelayFlag = 0;    //������ʱ��־
  Local.DefenceDelayTime = 0;    //����
  for(i=0; i<MAXCOUNT; i++)
  {
    Local.AlarmDelayFlag[i] = 0;     //������ʱ��־
    Local.AlarmDelayTime[i] = 0;    //����
  }

  Local.IP_Group[0] = 0; //�鲥��ַ
  Local.IP_Group[1] = 0;
  Local.IP_Group[2] = 0;
  Local.IP_Group[3] = 0;

  Local.NsMultiAddr[0] = 238;    //NS�鲥��ַ
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

  Local.Test_OV7725FrameRate = 0;    //OV7725 ���֡��

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

  if(LocalCfg.Language == 2)  //����  0 -- ���ļ���   1 -- ���ķ���   2 -- Ӣ��
    Local.LanguageNo = 1;
  else
    Local.LanguageNo = 0;
  xtm_printf("Init_LocalVar 1\n");

  //��ʱ����ʾ��Ϣ��־
  PicStatBuf.Flag = 0;
  PicStatBuf.Type = 0;
  PicStatBuf.Time = 0;

  Local.Conflict_Flag = 0;    //IP��ַ��ͻ
  for(i=0; i<6; i++)
    Local.Conflict_Mac[i] = 0x0;

  Local.GpioWatchDogStarted = 1;  //��GPIO����߳�������WatchDog
  Local.RemoteDebugInfo = 0;      //����Զ�̵�����Ϣ

  LocalCfg.EncodeFrameSize = VIDEO_VGA; //�����С
  LocalCfg.VideoClosed = 1; //��Ƶ�ر�

 #ifdef _REMOTECALLTEST  //Զ�̺��в���
  Local.EnterTestMode = 0;  //����ģʽ, 5���Զ�����
  Local.CloseTestRing = 0;  //����ģʽ, �ر�����
 #endif
  //20130318 ����
  //LocalCfg.VideoSdramRefresh = 1;    //LCD��ʾʱ SDRAMˢ������  0  8����  1 4����
  if(LocalCfg.MicDecValue > 3) //չ�䡢չ��ģʽ,���С Mic����
    LocalCfg.MicDecValue = 0;  

  Local.SendLockFlag = 0;    //20180608 ͨ�������п���


}
//---------------------------------------------------------------------------
void ResetLocalCfg(void)    //��λ�����ļ�
{
  uint8_t i, j;
  xtm_printf("ResetLocalCfg::\n");
  //ͷ��������
  memcpy(LocalCfg.FlashHead, "XXXCID", 6);
  //��ַ����
  memcpy(LocalCfg.Addr,NullAddr,20);
  memcpy(LocalCfg.Addr,"S00010108090",12);
  //������ͬ����
  strcpy(LocalCfg.AddrVerifyMa, "123456");
  //������ַ
  LocalCfg.Mac_Addr[0] = 0x00;
  LocalCfg.Mac_Addr[1] = 0x23;
  LocalCfg.Mac_Addr[2] = 0x60;
  LocalCfg.Mac_Addr[3] = 0x22;
  LocalCfg.Mac_Addr[4] = 0x41;
  LocalCfg.Mac_Addr[5] = 0x63;
  //IP��ַ
  LocalCfg.IP[0] = 192;
  LocalCfg.IP[1] = 168;
  LocalCfg.IP[2] = 68;
  LocalCfg.IP[3] = 161;
  //��������
  LocalCfg.IP_Mask[0] = 255;
  LocalCfg.IP_Mask[1] = 255;
  LocalCfg.IP_Mask[2] = 255;
  LocalCfg.IP_Mask[3] = 0;
  //���ص�ַ
  LocalCfg.IP_Gate[0] = 192;
  LocalCfg.IP_Gate[1] = 168;
  LocalCfg.IP_Gate[2] = 68;
  LocalCfg.IP_Gate[3] = 1;
  //NS�����ƽ�������������ַ
  LocalCfg.IP_NS[0] = 192;
  LocalCfg.IP_NS[1] = 168;
  LocalCfg.IP_NS[2] = 1;
  LocalCfg.IP_NS[3] = 1;
  //����������ַ
  LocalCfg.IP_Server[0] = 192;
  LocalCfg.IP_Server[1] = 168;
  LocalCfg.IP_Server[2] = 68;
  LocalCfg.IP_Server[3] = 2;
  //�㲥��ַ
  LocalCfg.IP_Broadcast[0] = 192;
  LocalCfg.IP_Broadcast[1] = 168;
  LocalCfg.IP_Broadcast[2] = 1;
  LocalCfg.IP_Broadcast[3] = 255;
        
  //�豸��ʱ����״̬ʱ��
  LocalCfg.ReportTime = 10;
  //����״̬
  LocalCfg.DefenceStatus = 0;
  //����ģ�����
  LocalCfg.DefenceNum = 1;
  for(i=0; i<8; i++)
    for(j=0; j<10; j++)
      LocalCfg.DefenceInfo[i][j] = 0;
  
  //��������
  strcpy(LocalCfg.EngineerPass, "123456");

  //��������   0  1  2 - ��ʱ����   3  4  5  ��ͨ����   6  7  24Сʱ����
  for(i=0; i<=2; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 3;   //��������     0 - �շ���  1 - ҹ����  2 - 24Сʱ����  3 - �Զ������
    LocalCfg.DefenceSetup_dDelay[i] = 5;  //��ʱʱ��     0 - 0��   1 - 5��  2 - 10��  3 - 15��  4 - 20��  5 - 25��  6 - 60��
    LocalCfg.DefenceSetup_tType[i] = 4;   //̽ͷ����     0 - �̸�  1 - ȼ��  2 - ����  3 - ����  4 - ����  5 - �Ŵ�  6 - ����
  }
  
  for(i=3; i<=5; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 3;   //��������     0 - �շ���  1 - ҹ����  2 - 24Сʱ����  3 - �Զ������
    LocalCfg.DefenceSetup_dDelay[i] = 0;  //��ʱʱ��     0 - 0��   1 - 5��  2 - 10��  3 - 15��  4 - 20��  5 - 25��  6 - 60��
    LocalCfg.DefenceSetup_tType[i] = 4;   //̽ͷ����     0 - �̸�  1 - ȼ��  2 - ����  3 - ����  4 - ����  5 - �Ŵ�  6 - ����
  }
  for(i=6; i<=7; i++)
  {
    LocalCfg.DefenceSetup_dType[i] = 2;   //��������     0 - �շ���  1 - ҹ����  2 - 24Сʱ����  3 - �Զ������
    LocalCfg.DefenceSetup_dDelay[i] = 0;  //��ʱʱ��     0 - 0��   1 - 5��  2 - 10��  3 - 15��  4 - 20��  5 - 25��  6 - 60��
    LocalCfg.DefenceSetup_tType[i] = 4;   //̽ͷ����     0 - �̸�  1 - ȼ��  2 - ����  3 - ����  4 - ����  5 - �Ŵ�  6 - ����
  }

  //������ʱ
  LocalCfg.In_DelayTime = 30;
  //�����ʱ
  LocalCfg.Out_DelayTime = 30;
  //������ʱ
  LocalCfg.Alarm_DelayTime = 30;

  //����
  for(i=0; i<8; i++)
    LocalCfg.SoundSetup[i] = 0;
  
  //�ÿ���Ӱ
  LocalCfg.RecPhoto = 0;
  //����ѡ��
  LocalCfg.Language = 1;
  //��������  0 ��ͨ��  1  ��ͨ��  2  ҡ����   3  ������
  LocalCfg.RingType = 0;
  //��������  0 ��ͨ��  1  ��ͨ��  2  ҡ����   3  ������
  LocalCfg.CallWaitRing = 0;
  //������ʾ  0 �ر�  1  ��
  LocalCfg.VoiceHint = 1;
  //SPK ����
  LocalCfg.SpkVolume = 2;
  //MIC ����
  LocalCfg.MicVolume = 2;
  //�Խ�ʱ����
  LocalCfg.TalkVoiceOpened = 1;
  LocalCfg.TalkVolume = 2;

  //��Ļ����
  LocalCfg.ScrSave[0] = 1;
  LocalCfg.ScrSave[1] = 0;
  LocalCfg.ScrSave[2] = 0;
  //��ҳ����  0 Ĭ�Ͻ���  1 �绰����
  LocalCfg.MainPage = 0;
  //����
  LocalCfg.BrightVolume = 2;
  //�Աȶ�
  LocalCfg.ContrastVolume = 2;
  //������    0 �ر�  1  ��
  LocalCfg.KeyVoice = 1;
  //������
  LocalCfg.Ts_X0 = 2059;//1901;
  LocalCfg.Ts_Y0 = 1955;//2001;
  LocalCfg.Ts_deltaX = 3844;//3744;
  LocalCfg.Ts_deltaY = -3491;//3555;
  LocalCfg.Ts_Mode = 1;     //X   Y  ����

  LocalCfg.Language = 0;  //����  0 -- ���ļ���   1 -- ���ķ���   2 -- Ӣ��

  LocalCfg.EncodeFrameRate = 3; //����֡��
  LocalCfg.EncodeFrameSize = VIDEO_VGA; //�����С
  LocalCfg.VideoClosed = 0; //��Ƶ�ر�
  LocalCfg.VideoOutput = 0;    //��Ƶ���  0  ����  1 �Լ�
  LocalCfg.VideoSdramRefresh = 0;    //LCD��ʾʱ SDRAMˢ������  0  8����  1 4����
  LocalCfg.MultiMachineSupport = 0;  //��ֻ�֧��,��������Ч���粻֧�֣��򲻷��㲥��ѯ��
  LocalCfg.DenMaxNum = 1;  //���������������һ������С�� SUBMAXNUM�� �����ڼ�С�㲥��
  LocalCfg.BootNo = 0;
  //outportb(POWER_CONTROL1, 0x04);//��Դ���ƼĴ���1    4����
}
//---------------------------------------------------------------------------
//�����������ļ�
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
      ResetLocalCfg();    //��λ�����ļ�
      WriteCfgFile();     //д���������ļ�
    } 
  }
  LocalCfg.Addr[12] = '\0';

  //LocalCfg.BootNo ++;
  Local.WriteCfgFlag = 1;     //20131025 ��ʼ��ʱдCfg ��־
  Local.WriteCfgTime = 0;
  //WriteCfgFile();
}
//---------------------------------------------------------------------------
//д���������ļ�
void WriteCfgFile(void)
{
  memcpy(Flash_Stream, &LocalCfg, sizeof(struct LocalCfg1));
  
  gm_spi_flash_data_write(LOCALCFG_ADDR, 0x1000, Flash_Stream);
  gm_spi_flash_data_write(LOCALCFG_ADDR1, 0x1000, Flash_Stream);
}
//---------------------------------------------------------------------------
//д���ݵ�Flash
void SaveToFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length)
{
  gm_spi_flash_data_write(FlashAddr, length, buf);
}
//---------------------------------------------------------------------------
//��Flash������
void ReadFromFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length)
{
	gm_spi_flash_data_read(FlashAddr, length, buf);
}
//---------------------------------------------------------------------------


