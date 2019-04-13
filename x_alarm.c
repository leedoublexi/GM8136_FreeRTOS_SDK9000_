
#include "stdlib.h"

#define x_alarm_h

#define CommonH
#include "common.h"

void DisplayCancelFortifyWindow(uint8_t RefreshType);    //��ʾ��������
void PreDealCancelFortifyWindow(uint8_t wintype);    //��������Ԥ��������������Ч��
void OperateCancelFortifyWindow(uint8_t wintype, uint8_t currwindow);    //�������ڲ���
void CheckTouchCancelFortifyWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������
void CancelFortifyDeal(void); //�������洦��
void PlayFortifySuccWav(void); //���� �����ɹ� ��ʾ
void PlayCancelFortifyWav(void);
void PlayDefenceDelayWav(void); //���� ������ʱ�� ��ʾ

void PlayCancelAlarmWav(void);
void PlayAlarmDelayWav(void); //���� ������ʱ�� ��ʾ
void PlayAlarmWav(void); //���� ���� ����

void DisplayFortifyWindow(uint8_t RefreshType);    //��ʾ��������
void PreDealFortifyWindow(uint8_t wintype);    //��������Ԥ��������������Ч��
void OperateFortifyWindow(uint8_t wintype, uint8_t currwindow);    //�������ڲ���
void CheckTouchFortifyWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void DisplayAlarmWindow(uint8_t RefreshType);    //��ʾ��������
void PreDealAlarmWindow(uint8_t wintype);    //��������Ԥ��������������Ч��
void OperateAlarmWindow(uint8_t wintype, uint8_t currwindow);    //�������ڲ���
void CheckTouchAlarmWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������
uint8_t AlarmImageFlag = 0;
uint8_t AlarmImageNo;
void ShowAlarmImage(void);

void DisplayCancelAlarmWindow(uint8_t RefreshType);    //��ʾȡ����������
void PreDealCancelAlarmWindow(uint8_t wintype);    //ȡ����������Ԥ��������������Ч��
void OperateCancelAlarmWindow(uint8_t wintype, uint8_t currwindow);    //ȡ���������ڲ���
void CheckTouchCancelAlarmWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������
void CancelAlarmDeal(void); //ȡ���������洦��

void CheckDefenceDelay_Func(void); //��鲼����ʱ����
void FortifyFinishDeal(void); //������ɽ��洦��
void CheckDelayAlarm(void);  //�����ʱ��������
void SendAlarmFunc(uint8_t SpecialByte, uint8_t AlarmByte[2]); //������������
void SendCancelAlarmFunc(void); //ȡ����������

struct Defence1{
               uint8_t Count;  //������������
               uint8_t Flag;   //����������־
               uint8_t eLevel; //��ƽ
               uint8_t Old_eLevel; //ǰһ�ε�ƽ               
               }Defence[MAXCOUNT];
void InitDefence(void);
void CheckDefence(unsigned char DefenceByte);    //����������
//--------------------------------------------------------------------------
void DisplayCancelFortifyWindow(uint8_t RefreshType)    //��ʾ��������
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

  DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, FULLPAGE);

  DisplayImage(&setup_key_image, FROMFILE, FULLPAGE);

  display_text(cancelfortify_edit.xLeft, cancelfortify_edit.yTop - 32, 0, COLOR_BLACK, "���������볷��:", 0, 24, FULLPAGE);

  strcpy(num_button[12].buttonname, "ɾ��");
  strcpy(num_button[13].buttonname, "ȷ��");
  //��ť
  for(i=12; i<14; i++)
   {
    num_button[i].FontType = 24;
    num_button[i].FontColor = COLOR_BLACK;
    num_button[i].Text_xLeft = (num_button[i].width - strlen(num_button[i].buttonname)*12)/2;
    num_button[i].Text_yTop = (num_button[i].height - 24)/2;
   }

  for(i=12; i<14; i++)
    DisplayImageButton_Text(&num_button[i], num_button[i].FontColor, 2, FULLPAGE);
  Local.CurrentWindow = 31;

  printf("Local.CurrentWindow  = %d\n", Local.CurrentWindow );
  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);


}
//--------------------------------------------------------------------------
void PreDealCancelFortifyWindow(uint8_t wintype)    //��������Ԥ��������������Ч��
{
  if((wintype >= 0)&&(wintype <= 13))
   {
    DisplayImageButton(&num_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  if((wintype >= 12)&&(wintype <= 14))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//--------------------------------------------------------------------------
void OperateCancelFortifyWindow(uint8_t wintype, uint8_t currwindow)    //�������ڲ���
{
  uint8_t i;
  char str[5];
  if(wintype < 14)
    DisplayImageButton(&num_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
  if((wintype >= 12)&&(wintype <= 13))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
		   printf("OperateCancelFortifyWindow::wintype = %d\n", wintype);
           if(cancelfortify_edit.BoxLen < cancelfortify_edit.MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
             strcat(cancelfortify_edit.Text, str);
             str[0] = '*';
             str[1] = '\0';

             display_text(cancelfortify_edit.xLeft + cancelfortify_edit.CursorX + cancelfortify_edit.BoxLen*setup_pass_edit.fWidth,
                     cancelfortify_edit.yTop + cancelfortify_edit.CursorY - 2, cancelfortify_edit.Space, cancelfortify_edit.CursorCorlor, str, 0, 24, NOFULLPAGE);

             cancelfortify_edit.BoxLen ++;
            }
           break;
    case 10: //���
    case 12:
          cancelfortify_edit.Text[0] = 0;
          cancelfortify_edit.BoxLen = 0;
          DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
          break;
    case 11: //ȷ��
    case 13: //ȷ��
          xtm_printf("cancelfortify_edit.BoxLen = %d\n", cancelfortify_edit.BoxLen);
          cancelfortify_edit.Text[cancelfortify_edit.BoxLen] = '\0';
          LocalCfg.EngineerPass[4] = '\0';
          if(strcmp(cancelfortify_edit.Text, LocalCfg.EngineerPass) == 0)
           {
              PlayCancelFortifyWav();

              //��������
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);

              CancelFortifyDeal(); //�������洦��

              #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
               //SendHostAssiOrder(0, 0);
              #endif
           }
          else
           {
              PlayPassErrorWav();

              //��������
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
           }
          break;
    default:
          OperateQuickmenu(wintype, 101);
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchCancelFortifyWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<14; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&num_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   {
    TouchNum = CheckBigmenuFunc(XLCD, YLCD);  //��ݰ�ť����
    if(TouchNum >= 100)
      isInTouch = 1;
   }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
void CancelFortifyDeal(void) //�������洦��
{
  int i;
  for(i=0; i<MAXDEFENCENUM; i++)
   if(Local.AlarmDelayFlag[i] == 1)
    {
     StopPlayWavFile();  //�رձ�����ʱ��ʾ��
    }
  for(i=0; i<MAXDEFENCENUM; i++)
   {
    Local.AlarmDelayFlag[i] = 0;     //������ʱ��־
    Local.AlarmDelayTime[i] = 0;
   }
  
  //SetGpio(GPIOD_OUT_DATA, FORTIFY_LED_HIGH, SET_HIGH); //�ز���LED

  LocalCfg.DefenceStatus = 0;
  Local.DefenceDelayFlag = 0;    //������ʱ��־
  Local.DefenceDelayTime = 0;    //����

  //21011118  xu
  if((Local.CurrentWindow == 31)||(Local.CurrentWindow == 35))
    DisplayFortifyWindow(REFRESH_MIDDLE);

  //д���������ļ�
  WriteCfgFile();  
}
//---------------------------------------------------------------------------
void PlayFortifySuccWav(void) //���� �����ɹ� ��ʾ
{
  if((LocalCfg.VoiceHint == 1)&&(Local.Status == 0))
   {
    WaitAudioUnuse(200); //�ȴ���������
    StartPlayWav(fortify_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//--------------------------------------------------------------------------
void PlayCancelFortifyWav(void)
{
  if(LocalCfg.VoiceHint == 1)
   {
    /*WaitAudioUnuse(200); //�ȴ���������*/
    StartPlayWav(cancelfortify_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//--------------------------------------------------------------------------
void DisplayFortifyWindow(uint8_t RefreshType)    //��ʾ��������
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

  for(i=0; i<2; i++)
    DisplayImageButton(&fortify_button[i], IMAGEUP, FROMFILE, FULLPAGE);
  Local.CurrentWindow = 32;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealFortifyWindow(uint8_t wintype)    //��������Ԥ��������������Ч��
{
  if((wintype >= 0)&&(wintype <= 1))
   {
    DisplayImageButton(&fortify_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//--------------------------------------------------------------------------
void OperateFortifyWindow(uint8_t wintype, uint8_t currwindow)    //�������ڲ���
{
  uint8_t i;
  int tmplen;
  char tmpchar[10];
  char str[5];
  if(wintype < 2)
    DisplayImageButton(&fortify_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0: //�������
           if((LocalCfg.DefenceStatus !=1)&&(LocalCfg.DefenceStatus !=11))
            {
             PlayDefenceDelayWav(); //���� ������ʱ�� ��ʾ

             LocalCfg.DefenceStatus = 11;
             Local.DefenceDelayFlag = 1;    //������ʱ��־
             Local.DefenceDelayTime = 0;    //����

             DisplayCancelFortifyWindow(NOREFRESH);

              #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
               //SendHostAssiOrder(LocalCfg.DefenceStatus, 1);
              #endif
            }
           break;
    case 1: //�ڼҲ���
           if((LocalCfg.DefenceStatus != 2)&&(LocalCfg.DefenceStatus != 12))
            {
             PlayDefenceDelayWav(); //���� ������ʱ�� ��ʾ

             LocalCfg.DefenceStatus = 12;
             Local.DefenceDelayFlag = 1;    //������ʱ��־
             Local.DefenceDelayTime = 0;    //����

             DisplayCancelFortifyWindow(NOREFRESH);

              #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
              // SendHostAssiOrder(LocalCfg.DefenceStatus, 1);
              #endif
            }
           break;
    default:
          OperateQuickmenu(wintype, 101);
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchFortifyWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<2; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&fortify_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   {
    TouchNum = CheckBigmenuFunc(XLCD, YLCD);  //��ݰ�ť����
    if(TouchNum >= 100)
      isInTouch = 1;
   }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
void PlayDefenceDelayWav(void) //���� ������ʱ�� ��ʾ
{
  if((LocalCfg.VoiceHint == 1)&&(Local.Status == 0))
   {
    WaitAudioUnuse(200); //�ȴ���������
    StartPlayWav(defencedelay_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//--------------------------------------------------------------------------
void PlayCancelAlarmWav(void)
{
  if((LocalCfg.VoiceHint == 1)&&(Local.Status == 0))
   {
    StopPlayWavFile();
    WaitAudioUnuse(500); //�ȴ���������
    StartPlayWav(cancelalarm_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//--------------------------------------------------------------------------
void PlayAlarmDelayWav(void) //���� ������ʱ�� ��ʾ
{
  if(Local.Status == 0)
   if(Local.CurrentWindow != 34)
    {
     WaitAudioUnuse(200); //�ȴ���������
     StartPlayWav(alarmdelay_wav.Faddr, NO_CYCLE_PLAY);
    }
}
//--------------------------------------------------------------------------
void PlayAlarmWav(void) //���� ���� ����
{
    WaitAudioUnuse(200); //�ȴ���������
    StartPlayWav(alarm_wav.Faddr, CYCLE_PLAY);
}
//--------------------------------------------------------------------------
void DisplayAlarmWindow(uint8_t RefreshType)   //��ʾ��������
{
  uint8_t i, j;
  char tmp_str[10];

  if(Local.LcdLightFlag == 0)
   {
    //����LCD����
    Lcd_Power(1, LocalCfg.BrightVolume);
   }
  Local.LcdLightFlag = 1; //LCD�����־
  Local.LcdLightTime = 0; //ʱ��
    
  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

  DisplayImage(&alarm_main_image, FROMFILE, FULLPAGE);

  StopPlayWavFile();
  WaitAudioUnuse(2000); //�ȴ���������
  
  /*playflag = 1;
  if((LocalCfg.DefenceInfo[6][3] != 0)||(LocalCfg.DefenceInfo[7][3] != 0))
  	playflag = 0;
  if(playflag)*/
  	PlayAlarmWav(); //���� ���� ����

  for(i=0; i<MAXDEFENCENUM; i++)
   if(LocalCfg.DefenceInfo[i][3] != 0)
    {
     for(j=0; j<10; j++)
       tmp_str[j] = '\0';
     sprintf(tmp_str, "%d", i+1);
     display_text(310+24*i, 432, 0, COLOR_BLACK, tmp_str, 0, 24, FULLPAGE);
     xtm_printf("i = %d, tmp_str = %s\n", i, tmp_str);
    }

  Local.CurrentWindow = 34;
  AlarmImageNo = 1;
  AlarmImageFlag = 1;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealAlarmWindow(uint8_t wintype)    //��������Ԥ��������������Ч��
{
}
//--------------------------------------------------------------------------
void OperateAlarmWindow(uint8_t wintype, uint8_t currwindow)    //�������ڲ���
{
  switch(wintype)
   {
    case 0:  //ȡ������
           AlarmImageNo = 0;
           StopPlayWavFile();  //�رձ�����
           WaitAudioUnuse(200); //�ȴ���������
           DisplayCancelAlarmWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
           break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchAlarmWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
    {
     isInTouch = CheckTSInImage(&alarm_image[0], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = 0;
      }
    }

  if(isInTouch == 1)
   {
    OperateAlarmWindow(TouchNum, 34);
   }
}
//--------------------------------------------------------------------------
void ShowAlarmImage(void)
{
  if(Local.CurrentWindow != 34)
    AlarmImageFlag = 0;
  if(AlarmImageFlag == 1)
   {
    DisplayImage(&alarm_image[AlarmImageNo], FROMFILE, NOFULLPAGE);
    if(AlarmImageNo == 0)
      AlarmImageNo = 1;
    else
      AlarmImageNo = 0;
   }
}
//--------------------------------------------------------------------------
void DisplayCancelAlarmWindow(uint8_t RefreshType)    //��ʾȡ����������
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

  DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, FULLPAGE);

  DisplayImage(&setup_key_image, FROMFILE, FULLPAGE);

  display_text(cancelfortify_edit.xLeft, cancelfortify_edit.yTop - 32, 0, COLOR_BLACK, "����������ȡ������:", 0, 24, FULLPAGE);

  strcpy(num_button[12].buttonname, "ɾ��");
  strcpy(num_button[13].buttonname, "ȷ��");

  //��ť
  for(i=12; i<14; i++)
   {
    num_button[i].FontType = 24;
    num_button[i].FontColor = COLOR_BLACK;
    num_button[i].Text_xLeft = (num_button[i].width - strlen(num_button[i].buttonname)*12)/2;
    num_button[i].Text_yTop = (num_button[i].height - 24)/2;
    //xtm_printf("%d   %d\n", num_button[i].Text_xLeft, num_button[i].Text_yTop);
   }

  for(i=12; i<14; i++)
    DisplayImageButton_Text(&num_button[i], num_button[i].FontColor, 2, FULLPAGE);
  Local.CurrentWindow = 35;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealCancelAlarmWindow(uint8_t wintype)    //ȡ����������Ԥ��������������Ч��
{
  if((wintype >= 0)&&(wintype <= 14))
   {
    DisplayImageButton(&num_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  if((wintype >= 12)&&(wintype <= 13))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//--------------------------------------------------------------------------
void OperateCancelAlarmWindow(uint8_t wintype, uint8_t currwindow)    //ȡ���������ڲ���
{
  uint8_t i, j;
  char str[5];
  int Special;
  char SpecialPass[10];
  if(wintype < 14)
    DisplayImageButton(&num_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
  if((wintype >= 12)&&(wintype <= 13))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
           if(cancelfortify_edit.BoxLen < cancelfortify_edit.MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
             strcat(cancelfortify_edit.Text, str);
             str[0] = '*';
             str[1] = '\0';

             display_text(cancelfortify_edit.xLeft + cancelfortify_edit.CursorX + cancelfortify_edit.BoxLen*setup_pass_edit.fWidth,
                     cancelfortify_edit.yTop + cancelfortify_edit.CursorY - 2, cancelfortify_edit.Space, cancelfortify_edit.CursorCorlor, str, 0, 24, NOFULLPAGE);

             cancelfortify_edit.BoxLen ++;
            }
           break;
    case 10: //���
    case 12:
          cancelfortify_edit.Text[0] = 0;
          cancelfortify_edit.BoxLen = 0;
          DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
          break;
    case 11: //ȷ��
    case 13: //ȷ��
          xtm_printf("cancelfortify_edit.BoxLen = %d\n", cancelfortify_edit.BoxLen);
          cancelfortify_edit.Text[cancelfortify_edit.BoxLen] = '\0';
          LocalCfg.EngineerPass[4] = '\0';
          Special = atoi(LocalCfg.EngineerPass);
          Special ++;
          if(Special > 9999)
            Special = 0;
          for(i=0; i<10; i++)
            SpecialPass[i] = '\0';
          sprintf(SpecialPass, "%04d", Special);
          xtm_printf("OperateCancelAlarmWindow::SpecialPass = %s\n", SpecialPass);

          if((strcmp(cancelfortify_edit.Text, LocalCfg.EngineerPass) == 0)||(strcmp(cancelfortify_edit.Text, SpecialPass) == 0))
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

              //�ٳֱ���
              if(strcmp(cancelfortify_edit.Text, SpecialPass) == 0)
               {
                SendAlarmFunc(0x01, Local.AlarmByte);
                #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
                 //SendAlarmToHost(37);
                #endif
               }
              CancelAlarmDeal(); //ȡ���������洦��
           }
          else
           {
              PlayPassErrorWav();

              //��������
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
           }
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchCancelAlarmWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<14; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&num_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
void CancelAlarmDeal(void) //ȡ���������洦��
{
#if 0
  SetGpio(GPIOA_OUT_DATA, ALARM_OUT_LOW, SET_LOW); //�ر������

  //��������
  cancelfortify_edit.Text[0] = 0;
  cancelfortify_edit.BoxLen = 0;

  Local.CurrentWindow = 0;
  DisplayFirstWindow();
#endif  
}
//--------------------------------------------------------------------------
void CheckDefenceDelay_Func(void) //��鲼����ʱ����
{
   int i, j;
   if(Local.DefenceDelayFlag == 1)    //������ʱ��־
    {

     if((Local.DefenceDelayTime % TIMERPERSEC) == 0)

      {

       /*if((Local.DefenceDelayTime % (TIMERPERSEC*2)) == 0)

         SetGpio(GPIOD_OUT_DATA, FORTIFY_LED_LOW, SET_LOW); //������LED

       else

         SetGpio(GPIOD_OUT_DATA, FORTIFY_LED_HIGH, SET_HIGH); //�ز���LED*/

      }


     Local.DefenceDelayTime ++;    //����

     if(Local.DefenceDelayTime >= (LocalCfg.Out_DelayTime*TIMERPERSEC))    //����

      {

       Local.DefenceDelayFlag = 0;

       Local.DefenceDelayTime = 0;


       xtm_printf("�����ɹ�\n");

       PlayFortifySuccWav();

         

       switch(LocalCfg.DefenceStatus)

        {

         case 11://���������ʱ��

         case 12://�ڼҲ�����ʱ��

         case 13://���޲�����ʱ��                           

         case 14://�𴲲�����ʱ��

         case 15://�Զ��岼����ʱ��

               for(j=0; j<5; j++)

                for(i=0; i<MAXDEFENCENUM; i++)
                 {
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


                LocalCfg.DefenceStatus = LocalCfg.DefenceStatus - 10;
                FortifyFinishDeal(); //������ɽ��洦��
                break;
        }
      }

     //ÿ3����ʾ,������ʱ��

     if(Local.DefenceDelayFlag == 1)

      if((Local.DefenceDelayTime % (5*TIMERPERSEC)) == 0)

       if(Local.DefenceDelayTime >= (5*TIMERPERSEC))

        {

         xtm_printf("CheckDefenceDelay_Func::Local.DefenceDelayTime = %d, LocalCfg.Out_DelayTime = %d\n", Local.DefenceDelayTime, LocalCfg.Out_DelayTime);

         PlayDefenceDelayWav(); //���� ������ʱ�� ��ʾ

        }

      if((LocalCfg.DefenceStatus < 11)&&(LocalCfg.DefenceStatus > 15))

       {

        Local.DefenceDelayFlag = 0;

        Local.DefenceDelayTime = 0;

       }

    }
}
//--------------------------------------------------------------------------
void FortifyFinishDeal(void) //������ɽ��洦��
{
  //д���������ļ�
  WriteCfgFile();
  
  //SetGpio(GPIOD_OUT_DATA, FORTIFY_LED_LOW, SET_LOW); //������LED
  Local.DefenceDelayFlag = 0;
  Local.DefenceDelayTime = 0;

  if(Local.CurrentWindow == 32)
    DisplayCancelFortifyWindow(REFRESH_MIDDLE | REFRESH_BOTTOM); 
}
//--------------------------------------------------------------------------
void CheckDelayAlarm(void)  //�����ʱ��������
{
  uint8_t i;
  uint8_t AlarmByte[2];
  uint8_t tmpbyte;

  uint8_t dDelay[7] = {0, 5, 10, 15, 20, 25, 60};   //��ʱʱ��

  for(i=0; i<2; i++)
    AlarmByte[i] = 0x0;

  tmpbyte = 0x01;

  for(i=0; i<MAXDEFENCENUM; i++)
  {
    if(Local.AlarmDelayFlag[i] == 1)
    {
      /*if((Local.AlarmDelayTime[i] % TIMERPERSEC) == 0)
      {
        if((Local.AlarmDelayTime[i] % (TIMERPERSEC*2)) == 0)
          SetGpio((unsigned long)GPIO_DATA_REG, ALARM_LED, SET_LOW);  //������ LED
        else
          SetGpio((unsigned long)GPIO_DATA_REG, ALARM_LED, SET_HIGH);  //�ر��� LED
      }*/

      Local.AlarmDelayTime[i] ++;    //����
      if(Local.AlarmDelayTime[i] > (dDelay[LocalCfg.DefenceSetup_dDelay[i]]*TIMERPERSEC))    //����
      {
        //20101125 xu �����ں��й����� ��ʱʱ�䵽 ����������
        if(Local.Status == 0)
        {
          StopPlayWavFile();  //�رձ�����ʱ��ʾ��
          //usleep(200*1000);  //��ʱ��Ϊ�˵ȴ�����������ɣ������������
        } 

        Local.AlarmDelayFlag[i] = 0;     //������ʱ��־
        Local.AlarmDelayTime[i] = 0;
        if(LocalCfg.DefenceInfo[i][3] != 2)
        {
          AlarmByte[i/8] |= (tmpbyte << i);
          LocalCfg.DefenceInfo[i][3] = 1;  //�б���
        } 
      }
      if((AlarmByte[0] != 0x0)||(AlarmByte[1] != 0x0))
      {
        //SetGpio((unsigned long)GPIO_DATA_REG, ALARM_LED, SET_LOW);  //������ LED
        xtm_printf("AlarmByte[0] = 0x%X, AlarmByte[1] = 0x%X\n", AlarmByte[0], AlarmByte[1]);
        //SetGpio(GPIOA_OUT_DATA, ALARM_OUT_HIGH, SET_HIGH); //���������
        SendAlarmFunc(0, AlarmByte);
        for(i=0; i<2; i++)
          Local.AlarmByte[i] = AlarmByte[i];
        
        //�ڶԽ�״̬�£�����ʾ
        if(Local.Status == 0)
          DisplayAlarmWindow(REFRESH_TOP);
      }

      //ÿ5����ʾ,������ʱ��
      if(Local.AlarmDelayFlag[i] == 1)
      {  
        if((Local.AlarmDelayTime[i] % (5*TIMERPERSEC)) == 0)
        {
          if(Local.AlarmDelayTime[i] >= (5*TIMERPERSEC))
          {
            PlayAlarmDelayWav(); //���� ������ʱ�� ��ʾ
          }
        }
      }
    }
  }
}

//--------------------------------------------------------------------------
void SendAlarmFunc(uint8_t SpecialByte, uint8_t AlarmByte[2]) //������������
{
  uint16_t sendlength;
  uint8_t sendb[128];
  char RemoteIP[20];

  //ͷ��
  memcpy(sendb, UdpPackageHead, 6);
  //����
  sendb[6] = ALARM;

  sendb[7]=ASK;        //����
  //������
  memcpy(sendb+8, LocalCfg.Addr, 20);
  memcpy(sendb+28, LocalCfg.Mac_Addr, 6);

  sendb[34] = LocalCfg.DefenceStatus;
  sendb[35] = LocalCfg.DefenceNum;

  sendb[34] = LocalCfg.DefenceStatus;     //����״̬
  sendb[35] = SpecialByte;                //���ⱨ��
  sendb[36] = LocalCfg.DefenceNum;        //����ģ�����
  sendb[37] = AlarmByte[0];
  sendb[38] = AlarmByte[1];
  sendb[39] = 0x0;
  sendb[40] = 0x0;

  sendlength = 41;
  sprintf(RemoteIP, "%d.%d.%d.%d", LocalCfg.IP_Server[0], LocalCfg.IP_Server[1], LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
  CreateMultiUdpBuff(UDP_DataSocket, MULTI_SEND, RemoteIP, LocalPort[0], sendb, sendlength, 0);
}
//--------------------------------------------------------------------------
void SendCancelAlarmFunc(void) //ȡ����������
{
  uint16_t sendlength;
  uint8_t sendb[128];
  char RemoteIP[20];

  //ͷ��
  memcpy(sendb, UdpPackageHead, 6);
  //����
  sendb[6] = CANCELALARM;

  sendb[7]=ASK;        //����
  //������
  memcpy(sendb+8, LocalCfg.Addr, 20);

  sendb[28] = LocalCfg.DefenceStatus;
  sendb[29] = LocalCfg.DefenceNum;

  sendlength = 30;
  sprintf(RemoteIP, "%d.%d.%d.%d", LocalCfg.IP_Server[0], LocalCfg.IP_Server[1], LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
  CreateMultiUdpBuff(UDP_DataSocket, MULTI_SEND, RemoteIP, LocalPort[0], sendb, sendlength, 0);
}
//--------------------------------------------------------------------------
void InitDefence(void)
{
  uint8_t i;
  for(i=0; i<MAXCOUNT; i++)
  {
    Defence[i].Count = 0;
    Defence[i].Flag = 0;
    Defence[i].eLevel = 1; //Ĭ��Ϊ�ߵ�ƽ
    Defence[i].Old_eLevel = 1; //ǰһ�ε�ƽ
  }
}
//---------------------------------------------------------------------------
void CheckDefence(unsigned char DefenceByte)    //����������
{
#if 0
  uint8_t    i;
  uint8_t tmpvalue;
  uint8_t tmpbyte;
  uint8_t AlarmByte[2];  //����
  uint8_t DefenceNo;
  uint8_t StatusNo;
  uint8_t MaxDefenceCount = 8;
  uint8_t NoPerByte = 8;
  uint8_t CountNum;

  if(Local.isHost != '0')  //����, ����������Ч
    return;
#if 1   //20110825 ��Ϊ����
     tmpvalue = DefenceByte;
     if(tmpvalue != 0)
      {
       for(i=0; i<NoPerByte; i++)
        {
         DefenceNo = i;

         if((tmpvalue & 0x01) == 1)
          {
           //xtm_printf("i = %d, DefenceNo = %d,  Old_eLevel = %d\n", i, DefenceNo, Defence[DefenceNo].Old_eLevel);

           if(Defence[DefenceNo].Old_eLevel == 0)
            {
             Defence[DefenceNo].Count ++;
             //xtm_printf("Defence[DefenceNo].Count = %d, DefenceByte = 0x%X\n", Defence[DefenceNo].Count, DefenceByte);
            }
           Defence[DefenceNo].eLevel = 1;
           //xtm_printf("DefenceNo = %d\n", DefenceNo);
          }
         else
          {
           Defence[DefenceNo].Count = 0;
           Defence[DefenceNo].Flag = 0;
           Defence[DefenceNo].eLevel = 0;
           Defence[DefenceNo].Old_eLevel = 0;
           //xtm_printf("Defence[DefenceNo].Count = %d, Defence[%d].Old_eLevel = %d\n", Defence[DefenceNo].Count, DefenceNo, Defence[DefenceNo].Old_eLevel);
          }
         tmpvalue = tmpvalue >> 1;
        }
      }
     else
       for(i=0; i<NoPerByte; i++)

        {
         DefenceNo = i;
         Defence[DefenceNo].Count = 0;
         Defence[DefenceNo].Flag = 0;
         Defence[DefenceNo].eLevel = 0;
         Defence[DefenceNo].Old_eLevel = 0;
         //xtm_printf("DefenceNo = %d, tmpvalue = 0x%X, AlarmByte[0] = 0x%X, AlarmByte[1] = 0x%X\n", 
         //        DefenceNo, tmpvalue, AlarmByte[0], AlarmByte[1]);
        }

#else    //����
     tmpvalue = DefenceByte;
     //printf("tmpvalue = 0x%02X\n", tmpvalue);
     if(tmpvalue != 0xff)
      {
       for(i=0; i<NoPerByte; i++)
        {
         DefenceNo = i;
         if((tmpvalue & 0x01) == 0)
          {
           Defence[DefenceNo].Count ++;
           xtm_printf("DefenceNo = %d\n", DefenceNo);
          }
         else
          {
           Defence[DefenceNo].Count = 0;
           Defence[DefenceNo].Flag = 0;
          }
         tmpvalue = tmpvalue >> 1;
        }
      }
     else
       for(i=0; i<NoPerByte; i++)

        {
         DefenceNo = i;
         Defence[DefenceNo].Count = 0;
         Defence[DefenceNo].Flag = 0;
         //xtm_printf("DefenceNo = %d, tmpvalue = 0x%X, AlarmByte[0] = 0x%X, AlarmByte[1] = 0x%X\n", 
         //            DefenceNo, tmpvalue, AlarmByte[0], AlarmByte[1]);
        }

#endif    

   //return;

#if 1

   for(i=0; i<2; i++)

     AlarmByte[i] = 0;

   for(i=0; i<MAXDEFENCENUM; i++)

    {

     //xtm_printf("DefenceByte[0] = 0x%02X, DefenceByte[1] = 0x%02X\n", DefenceByte[0], DefenceByte[1]);

     //xtm_printf("Defence[%d].Count = %d, Defence[%d].Flag = %d\n", i, Defence[i].Count, i, Defence[i].Flag);

     CountNum = 4;

     if((Defence[i].Count > CountNum)&&(Defence[i].Flag == 0))
      {
       Defence[i].Flag = 1;
       Defence[i].Old_eLevel = 1;
       xtm_printf("Defnece i = %d, dType = %d, DefenceStatus = %d\n", i + 1, LocalCfg.DefenceSetup_dType[i], LocalCfg.DefenceStatus);
       tmpbyte = 0x01;

       if(i >= MAXDEFENCENUM)
         return;       
/*struct DefenceSetup1{          //��������
               unsigned char dType;   //��������     0 - �շ���  1 - ҹ����  2 - 24Сʱ����  3 - �Զ������
               unsigned char dDelay;   //��ʱʱ��    0 - 0��   1 - 5��  2 - 10��  3 - 15��  4 - 20��  5 - 25��  6 - 60��
               unsigned char tType;   //̽ͷ����     0 - �̸�  1 - ȼ��  2 - ����  3 - ����  4 - ����  5 - �Ŵ�  6 - ����
               };
unsigned char DefenceStatus;       //����״̬      0 - ����  1 - ���   2 - �ڼ�   3 - ����   4 - ��   5 - �Զ���
    */                                              //                        11 - ���   12 - �ڼ�   13 - ����   14 - ��   15 - �Զ���     ��ʱ
       switch(LocalCfg.DefenceSetup_dType[i])
        {
         case 3: //�Զ������
                switch(LocalCfg.DefenceStatus)
                 {
                  case 1:
                  case 2:
                  case 3:
                  case 4:
                  case 5:
                  //case 11:    //�ڲ�����ʱ�У���������������
                  //case 12:
                  //case 13:
                  //case 14:
                  //case 15:
                          if((LocalCfg.DefenceStatus >= 1)&&(LocalCfg.DefenceStatus <= 5))
                            StatusNo = LocalCfg.DefenceStatus - 1;
                          else
                            StatusNo = LocalCfg.DefenceStatus - 11;
                          xtm_printf("StatusNo = %d, LocalCfg.Scene_Defence[%d][%d] = %d, LocalCfg.DefenceInfo[%d][3] = %d\n", 
                              StatusNo, StatusNo, i, LocalCfg.Scene_Defence[StatusNo][i], i, LocalCfg.DefenceInfo[i][3]);
                          if(LocalCfg.Scene_Defence[StatusNo][i] == 1)
                           {
                            switch(LocalCfg.DefenceSetup_dDelay[i])
                             {
                              case 0:
                                     if(LocalCfg.DefenceInfo[i][3] != 2)
                                      {
                                       AlarmByte[i/8] |= (tmpbyte << i);
                                       LocalCfg.DefenceInfo[i][3] = 1;  //�б���
                                      }
                                     break;
                              default:
                                     Local.AlarmDelayFlag[i] = 1;     //������ʱ��־
                                     Local.AlarmDelayTime[i] = 0;    //����

                                     PlayAlarmDelayWav(); //���� ������ʱ�� ��ʾ
                                     break;
                             }
                           }
                          break;
                 }
                break;
         case 2: //24Сʱ����
                switch(LocalCfg.DefenceSetup_dDelay[i])
                 {
                  case 0:
                         if(LocalCfg.DefenceInfo[i][3] != 2)
                          {
                           AlarmByte[i/8] |= (tmpbyte << i);
                           LocalCfg.DefenceInfo[i][3] = 1;  //�б���
                          }
                         break;
                  default:
                         Local.AlarmDelayFlag[i] = 1;     //������ʱ��־
                         Local.AlarmDelayTime[i] = 0;    //����

                         PlayAlarmDelayWav(); //���� ������ʱ�� ��ʾ
                         break;
                 }
                break;
        }
     }
    }
  if((AlarmByte[0] != 0x0)||(AlarmByte[1] != 0x0))
   {
    xtm_printf("AlarmByte[0] = 0x%X, AlarmByte[1] = 0x%X\n", AlarmByte[0], AlarmByte[1]);

    SetGpio(GPIOA_OUT_DATA, ALARM_OUT_HIGH, SET_HIGH); //���������
    SendAlarmFunc(0, AlarmByte);
    for(i=0; i<2; i++)
      Local.AlarmByte[i] = AlarmByte[i];
    //�ڶԽ�״̬�£�����ʾ
    if(Local.Status == 0)
      DisplayAlarmWindow(REFRESH_TOP);
   }

#endif   
#endif	
}
//---------------------------------------------------------------------------