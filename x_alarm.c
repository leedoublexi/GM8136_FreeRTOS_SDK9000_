
#include "stdlib.h"

#define x_alarm_h

#define CommonH
#include "common.h"

void DisplayCancelFortifyWindow(uint8_t RefreshType);    //显示撤防窗口
void PreDealCancelFortifyWindow(uint8_t wintype);    //撤防窗口预处理，按键按下效果
void OperateCancelFortifyWindow(uint8_t wintype, uint8_t currwindow);    //撤防窗口操作
void CheckTouchCancelFortifyWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作
void CancelFortifyDeal(void); //撤防界面处理
void PlayFortifySuccWav(void); //播放 布防成功 提示
void PlayCancelFortifyWav(void);
void PlayDefenceDelayWav(void); //播放 布防延时中 提示

void PlayCancelAlarmWav(void);
void PlayAlarmDelayWav(void); //播放 报警延时中 提示
void PlayAlarmWav(void); //播放 报警 铃声

void DisplayFortifyWindow(uint8_t RefreshType);    //显示布防窗口
void PreDealFortifyWindow(uint8_t wintype);    //布防窗口预处理，按键按下效果
void OperateFortifyWindow(uint8_t wintype, uint8_t currwindow);    //布防窗口操作
void CheckTouchFortifyWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

void DisplayAlarmWindow(uint8_t RefreshType);    //显示报警窗口
void PreDealAlarmWindow(uint8_t wintype);    //报警窗口预处理，按键按下效果
void OperateAlarmWindow(uint8_t wintype, uint8_t currwindow);    //报警窗口操作
void CheckTouchAlarmWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作
uint8_t AlarmImageFlag = 0;
uint8_t AlarmImageNo;
void ShowAlarmImage(void);

void DisplayCancelAlarmWindow(uint8_t RefreshType);    //显示取消报警窗口
void PreDealCancelAlarmWindow(uint8_t wintype);    //取消报警窗口预处理，按键按下效果
void OperateCancelAlarmWindow(uint8_t wintype, uint8_t currwindow);    //取消报警窗口操作
void CheckTouchCancelAlarmWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作
void CancelAlarmDeal(void); //取消报警界面处理

void CheckDefenceDelay_Func(void); //检查布防延时函数
void FortifyFinishDeal(void); //布防完成界面处理
void CheckDelayAlarm(void);  //检查延时报警函数
void SendAlarmFunc(uint8_t SpecialByte, uint8_t AlarmByte[2]); //防区报警函数
void SendCancelAlarmFunc(void); //取消报警函数

struct Defence1{
               uint8_t Count;  //防区触发计数
               uint8_t Flag;   //防区触发标志
               uint8_t eLevel; //电平
               uint8_t Old_eLevel; //前一次电平               
               }Defence[MAXCOUNT];
void InitDefence(void);
void CheckDefence(unsigned char DefenceByte);    //检测防区函数
//--------------------------------------------------------------------------
void DisplayCancelFortifyWindow(uint8_t RefreshType)    //显示撤防窗口
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //刷新背景

  DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, FULLPAGE);

  DisplayImage(&setup_key_image, FROMFILE, FULLPAGE);

  display_text(cancelfortify_edit.xLeft, cancelfortify_edit.yTop - 32, 0, COLOR_BLACK, "请输入密码撤防:", 0, 24, FULLPAGE);

  strcpy(num_button[12].buttonname, "删除");
  strcpy(num_button[13].buttonname, "确认");
  //按钮
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
void PreDealCancelFortifyWindow(uint8_t wintype)    //撤防窗口预处理，按键按下效果
{
  if((wintype >= 0)&&(wintype <= 13))
   {
    DisplayImageButton(&num_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  if((wintype >= 12)&&(wintype <= 14))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //快捷按键 预处理
}
//--------------------------------------------------------------------------
void OperateCancelFortifyWindow(uint8_t wintype, uint8_t currwindow)    //撤防窗口操作
{
  uint8_t i;
  char str[5];
  if(wintype < 14)
    DisplayImageButton(&num_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
  if((wintype >= 12)&&(wintype <= 13))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  ShowPressQuickmenu_Button(wintype);  //快捷按键  弹起

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
    case 10: //清除
    case 12:
          cancelfortify_edit.Text[0] = 0;
          cancelfortify_edit.BoxLen = 0;
          DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
          break;
    case 11: //确认
    case 13: //确认
          xtm_printf("cancelfortify_edit.BoxLen = %d\n", cancelfortify_edit.BoxLen);
          cancelfortify_edit.Text[cancelfortify_edit.BoxLen] = '\0';
          LocalCfg.EngineerPass[4] = '\0';
          if(strcmp(cancelfortify_edit.Text, LocalCfg.EngineerPass) == 0)
           {
              PlayCancelFortifyWav();

              //清除输入框
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);

              CancelFortifyDeal(); //撤防界面处理

              #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
               //SendHostAssiOrder(0, 0);
              #endif
           }
          else
           {
              PlayPassErrorWav();

              //清除输入框
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
void CheckTouchCancelFortifyWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<14; i++)  //窗口按钮
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
    TouchNum = CheckBigmenuFunc(XLCD, YLCD);  //快捷按钮操作
    if(TouchNum >= 100)
      isInTouch = 1;
   }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
void CancelFortifyDeal(void) //撤防界面处理
{
  int i;
  for(i=0; i<MAXDEFENCENUM; i++)
   if(Local.AlarmDelayFlag[i] == 1)
    {
     StopPlayWavFile();  //关闭报警延时提示音
    }
  for(i=0; i<MAXDEFENCENUM; i++)
   {
    Local.AlarmDelayFlag[i] = 0;     //报警延时标志
    Local.AlarmDelayTime[i] = 0;
   }
  
  //SetGpio(GPIOD_OUT_DATA, FORTIFY_LED_HIGH, SET_HIGH); //关布防LED

  LocalCfg.DefenceStatus = 0;
  Local.DefenceDelayFlag = 0;    //布防延时标志
  Local.DefenceDelayTime = 0;    //计数

  //21011118  xu
  if((Local.CurrentWindow == 31)||(Local.CurrentWindow == 35))
    DisplayFortifyWindow(REFRESH_MIDDLE);

  //写本地设置文件
  WriteCfgFile();  
}
//---------------------------------------------------------------------------
void PlayFortifySuccWav(void) //播放 布防成功 提示
{
  if((LocalCfg.VoiceHint == 1)&&(Local.Status == 0))
   {
    WaitAudioUnuse(200); //等待声卡空闲
    StartPlayWav(fortify_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//--------------------------------------------------------------------------
void PlayCancelFortifyWav(void)
{
  if(LocalCfg.VoiceHint == 1)
   {
    /*WaitAudioUnuse(200); //等待声卡空闲*/
    StartPlayWav(cancelfortify_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//--------------------------------------------------------------------------
void DisplayFortifyWindow(uint8_t RefreshType)    //显示布防窗口
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //刷新背景

  for(i=0; i<2; i++)
    DisplayImageButton(&fortify_button[i], IMAGEUP, FROMFILE, FULLPAGE);
  Local.CurrentWindow = 32;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealFortifyWindow(uint8_t wintype)    //布防窗口预处理，按键按下效果
{
  if((wintype >= 0)&&(wintype <= 1))
   {
    DisplayImageButton(&fortify_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //快捷按键 预处理
}
//--------------------------------------------------------------------------
void OperateFortifyWindow(uint8_t wintype, uint8_t currwindow)    //布防窗口操作
{
  uint8_t i;
  int tmplen;
  char tmpchar[10];
  char str[5];
  if(wintype < 2)
    DisplayImageButton(&fortify_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //快捷按键  弹起

  switch(wintype)
   {
    case 0: //外出布防
           if((LocalCfg.DefenceStatus !=1)&&(LocalCfg.DefenceStatus !=11))
            {
             PlayDefenceDelayWav(); //播放 布防延时中 提示

             LocalCfg.DefenceStatus = 11;
             Local.DefenceDelayFlag = 1;    //布防延时标志
             Local.DefenceDelayTime = 0;    //计数

             DisplayCancelFortifyWindow(NOREFRESH);

              #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
               //SendHostAssiOrder(LocalCfg.DefenceStatus, 1);
              #endif
            }
           break;
    case 1: //在家布防
           if((LocalCfg.DefenceStatus != 2)&&(LocalCfg.DefenceStatus != 12))
            {
             PlayDefenceDelayWav(); //播放 布防延时中 提示

             LocalCfg.DefenceStatus = 12;
             Local.DefenceDelayFlag = 1;    //布防延时标志
             Local.DefenceDelayTime = 0;    //计数

             DisplayCancelFortifyWindow(NOREFRESH);

              #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
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
void CheckTouchFortifyWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<2; i++)  //窗口按钮
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
    TouchNum = CheckBigmenuFunc(XLCD, YLCD);  //快捷按钮操作
    if(TouchNum >= 100)
      isInTouch = 1;
   }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
void PlayDefenceDelayWav(void) //播放 布防延时中 提示
{
  if((LocalCfg.VoiceHint == 1)&&(Local.Status == 0))
   {
    WaitAudioUnuse(200); //等待声卡空闲
    StartPlayWav(defencedelay_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//--------------------------------------------------------------------------
void PlayCancelAlarmWav(void)
{
  if((LocalCfg.VoiceHint == 1)&&(Local.Status == 0))
   {
    StopPlayWavFile();
    WaitAudioUnuse(500); //等待声卡空闲
    StartPlayWav(cancelalarm_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//--------------------------------------------------------------------------
void PlayAlarmDelayWav(void) //播放 报警延时中 提示
{
  if(Local.Status == 0)
   if(Local.CurrentWindow != 34)
    {
     WaitAudioUnuse(200); //等待声卡空闲
     StartPlayWav(alarmdelay_wav.Faddr, NO_CYCLE_PLAY);
    }
}
//--------------------------------------------------------------------------
void PlayAlarmWav(void) //播放 报警 铃声
{
    WaitAudioUnuse(200); //等待声卡空闲
    StartPlayWav(alarm_wav.Faddr, CYCLE_PLAY);
}
//--------------------------------------------------------------------------
void DisplayAlarmWindow(uint8_t RefreshType)   //显示报警窗口
{
  uint8_t i, j;
  char tmp_str[10];

  if(Local.LcdLightFlag == 0)
   {
    //点亮LCD背光
    Lcd_Power(1, LocalCfg.BrightVolume);
   }
  Local.LcdLightFlag = 1; //LCD背光标志
  Local.LcdLightTime = 0; //时间
    
  RefreshBackWindow(RefreshType, FULLPAGE); //刷新背景

  DisplayImage(&alarm_main_image, FROMFILE, FULLPAGE);

  StopPlayWavFile();
  WaitAudioUnuse(2000); //等待声卡空闲
  
  /*playflag = 1;
  if((LocalCfg.DefenceInfo[6][3] != 0)||(LocalCfg.DefenceInfo[7][3] != 0))
  	playflag = 0;
  if(playflag)*/
  	PlayAlarmWav(); //播放 报警 铃声

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
void PreDealAlarmWindow(uint8_t wintype)    //报警窗口预处理，按键按下效果
{
}
//--------------------------------------------------------------------------
void OperateAlarmWindow(uint8_t wintype, uint8_t currwindow)    //报警窗口操作
{
  switch(wintype)
   {
    case 0:  //取消报警
           AlarmImageNo = 0;
           StopPlayWavFile();  //关闭报警音
           WaitAudioUnuse(200); //等待声卡空闲
           DisplayCancelAlarmWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
           break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchAlarmWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
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
void DisplayCancelAlarmWindow(uint8_t RefreshType)    //显示取消报警窗口
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //刷新背景

  DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, FULLPAGE);

  DisplayImage(&setup_key_image, FROMFILE, FULLPAGE);

  display_text(cancelfortify_edit.xLeft, cancelfortify_edit.yTop - 32, 0, COLOR_BLACK, "请输入密码取消报警:", 0, 24, FULLPAGE);

  strcpy(num_button[12].buttonname, "删除");
  strcpy(num_button[13].buttonname, "确认");

  //按钮
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
void PreDealCancelAlarmWindow(uint8_t wintype)    //取消报警窗口预处理，按键按下效果
{
  if((wintype >= 0)&&(wintype <= 14))
   {
    DisplayImageButton(&num_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  if((wintype >= 12)&&(wintype <= 13))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //快捷按键 预处理
}
//--------------------------------------------------------------------------
void OperateCancelAlarmWindow(uint8_t wintype, uint8_t currwindow)    //取消报警窗口操作
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

  ShowPressQuickmenu_Button(wintype);  //快捷按键  弹起

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
    case 10: //清除
    case 12:
          cancelfortify_edit.Text[0] = 0;
          cancelfortify_edit.BoxLen = 0;
          DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
          break;
    case 11: //确认
    case 13: //确认
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
                SendCancelAlarmFunc(); //取消报警函数

              for(i=0; i<MAXDEFENCENUM; i++)
               {
                Local.AlarmDelayFlag[i] = 0;     //报警延时标志
                Local.AlarmDelayTime[i] = 0;
               }

              for(i=0; i<2; i++)
                Local.AlarmByte[i] = 0;

              PlayCancelAlarmWav();

              //劫持报警
              if(strcmp(cancelfortify_edit.Text, SpecialPass) == 0)
               {
                SendAlarmFunc(0x01, Local.AlarmByte);
                #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
                 //SendAlarmToHost(37);
                #endif
               }
              CancelAlarmDeal(); //取消报警界面处理
           }
          else
           {
              PlayPassErrorWav();

              //清除输入框
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
           }
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchCancelAlarmWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<14; i++)  //窗口按钮
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
void CancelAlarmDeal(void) //取消报警界面处理
{
#if 0
  SetGpio(GPIOA_OUT_DATA, ALARM_OUT_LOW, SET_LOW); //关报警输出

  //清除输入框
  cancelfortify_edit.Text[0] = 0;
  cancelfortify_edit.BoxLen = 0;

  Local.CurrentWindow = 0;
  DisplayFirstWindow();
#endif  
}
//--------------------------------------------------------------------------
void CheckDefenceDelay_Func(void) //检查布防延时函数
{
   int i, j;
   if(Local.DefenceDelayFlag == 1)    //布防延时标志
    {

     if((Local.DefenceDelayTime % TIMERPERSEC) == 0)

      {

       /*if((Local.DefenceDelayTime % (TIMERPERSEC*2)) == 0)

         SetGpio(GPIOD_OUT_DATA, FORTIFY_LED_LOW, SET_LOW); //开布防LED

       else

         SetGpio(GPIOD_OUT_DATA, FORTIFY_LED_HIGH, SET_HIGH); //关布防LED*/

      }


     Local.DefenceDelayTime ++;    //计数

     if(Local.DefenceDelayTime >= (LocalCfg.Out_DelayTime*TIMERPERSEC))    //计数

      {

       Local.DefenceDelayFlag = 0;

       Local.DefenceDelayTime = 0;


       xtm_printf("布防成功\n");

       PlayFortifySuccWav();

         

       switch(LocalCfg.DefenceStatus)

        {

         case 11://外出布防延时中

         case 12://在家布防延时中

         case 13://就寝布防延时中                           

         case 14://起床布防延时中

         case 15://自定义布防延时中

               for(j=0; j<5; j++)

                for(i=0; i<MAXDEFENCENUM; i++)
                 {
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


                LocalCfg.DefenceStatus = LocalCfg.DefenceStatus - 10;
                FortifyFinishDeal(); //布防完成界面处理
                break;
        }
      }

     //每3秒提示,布防延时中

     if(Local.DefenceDelayFlag == 1)

      if((Local.DefenceDelayTime % (5*TIMERPERSEC)) == 0)

       if(Local.DefenceDelayTime >= (5*TIMERPERSEC))

        {

         xtm_printf("CheckDefenceDelay_Func::Local.DefenceDelayTime = %d, LocalCfg.Out_DelayTime = %d\n", Local.DefenceDelayTime, LocalCfg.Out_DelayTime);

         PlayDefenceDelayWav(); //播放 布防延时中 提示

        }

      if((LocalCfg.DefenceStatus < 11)&&(LocalCfg.DefenceStatus > 15))

       {

        Local.DefenceDelayFlag = 0;

        Local.DefenceDelayTime = 0;

       }

    }
}
//--------------------------------------------------------------------------
void FortifyFinishDeal(void) //布防完成界面处理
{
  //写本地设置文件
  WriteCfgFile();
  
  //SetGpio(GPIOD_OUT_DATA, FORTIFY_LED_LOW, SET_LOW); //开布防LED
  Local.DefenceDelayFlag = 0;
  Local.DefenceDelayTime = 0;

  if(Local.CurrentWindow == 32)
    DisplayCancelFortifyWindow(REFRESH_MIDDLE | REFRESH_BOTTOM); 
}
//--------------------------------------------------------------------------
void CheckDelayAlarm(void)  //检查延时报警函数
{
  uint8_t i;
  uint8_t AlarmByte[2];
  uint8_t tmpbyte;

  uint8_t dDelay[7] = {0, 5, 10, 15, 20, 25, 60};   //延时时间

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
          SetGpio((unsigned long)GPIO_DATA_REG, ALARM_LED, SET_LOW);  //开报警 LED
        else
          SetGpio((unsigned long)GPIO_DATA_REG, ALARM_LED, SET_HIGH);  //关报警 LED
      }*/

      Local.AlarmDelayTime[i] ++;    //计数
      if(Local.AlarmDelayTime[i] > (dDelay[LocalCfg.DefenceSetup_dDelay[i]]*TIMERPERSEC))    //计数
      {
        //20101125 xu 避免在呼叫过程中 延时时间到 就无振铃了
        if(Local.Status == 0)
        {
          StopPlayWavFile();  //关闭报警延时提示音
          //usleep(200*1000);  //延时是为了等待声音播放完成，否则会有杂音
        } 

        Local.AlarmDelayFlag[i] = 0;     //报警延时标志
        Local.AlarmDelayTime[i] = 0;
        if(LocalCfg.DefenceInfo[i][3] != 2)
        {
          AlarmByte[i/8] |= (tmpbyte << i);
          LocalCfg.DefenceInfo[i][3] = 1;  //有报警
        } 
      }
      if((AlarmByte[0] != 0x0)||(AlarmByte[1] != 0x0))
      {
        //SetGpio((unsigned long)GPIO_DATA_REG, ALARM_LED, SET_LOW);  //开报警 LED
        xtm_printf("AlarmByte[0] = 0x%X, AlarmByte[1] = 0x%X\n", AlarmByte[0], AlarmByte[1]);
        //SetGpio(GPIOA_OUT_DATA, ALARM_OUT_HIGH, SET_HIGH); //开报警输出
        SendAlarmFunc(0, AlarmByte);
        for(i=0; i<2; i++)
          Local.AlarmByte[i] = AlarmByte[i];
        
        //在对讲状态下，不显示
        if(Local.Status == 0)
          DisplayAlarmWindow(REFRESH_TOP);
      }

      //每5秒提示,报警延时中
      if(Local.AlarmDelayFlag[i] == 1)
      {  
        if((Local.AlarmDelayTime[i] % (5*TIMERPERSEC)) == 0)
        {
          if(Local.AlarmDelayTime[i] >= (5*TIMERPERSEC))
          {
            PlayAlarmDelayWav(); //播放 报警延时中 提示
          }
        }
      }
    }
  }
}

//--------------------------------------------------------------------------
void SendAlarmFunc(uint8_t SpecialByte, uint8_t AlarmByte[2]) //防区报警函数
{
  uint16_t sendlength;
  uint8_t sendb[128];
  char RemoteIP[20];

  //头部
  memcpy(sendb, UdpPackageHead, 6);
  //命令
  sendb[6] = ALARM;

  sendb[7]=ASK;        //主叫
  //子命令
  memcpy(sendb+8, LocalCfg.Addr, 20);
  memcpy(sendb+28, LocalCfg.Mac_Addr, 6);

  sendb[34] = LocalCfg.DefenceStatus;
  sendb[35] = LocalCfg.DefenceNum;

  sendb[34] = LocalCfg.DefenceStatus;     //布防状态
  sendb[35] = SpecialByte;                //特殊报警
  sendb[36] = LocalCfg.DefenceNum;        //防区模块个数
  sendb[37] = AlarmByte[0];
  sendb[38] = AlarmByte[1];
  sendb[39] = 0x0;
  sendb[40] = 0x0;

  sendlength = 41;
  sprintf(RemoteIP, "%d.%d.%d.%d", LocalCfg.IP_Server[0], LocalCfg.IP_Server[1], LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
  CreateMultiUdpBuff(UDP_DataSocket, MULTI_SEND, RemoteIP, LocalPort[0], sendb, sendlength, 0);
}
//--------------------------------------------------------------------------
void SendCancelAlarmFunc(void) //取消报警函数
{
  uint16_t sendlength;
  uint8_t sendb[128];
  char RemoteIP[20];

  //头部
  memcpy(sendb, UdpPackageHead, 6);
  //命令
  sendb[6] = CANCELALARM;

  sendb[7]=ASK;        //主叫
  //子命令
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
    Defence[i].eLevel = 1; //默认为高电平
    Defence[i].Old_eLevel = 1; //前一次电平
  }
}
//---------------------------------------------------------------------------
void CheckDefence(unsigned char DefenceByte)    //检测防区函数
{
#if 0
  uint8_t    i;
  uint8_t tmpvalue;
  uint8_t tmpbyte;
  uint8_t AlarmByte[2];  //报警
  uint8_t DefenceNo;
  uint8_t StatusNo;
  uint8_t MaxDefenceCount = 8;
  uint8_t NoPerByte = 8;
  uint8_t CountNum;

  if(Local.isHost != '0')  //副机, 安防功能无效
    return;
#if 1   //20110825 改为常闭
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

#else    //常开
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
/*struct DefenceSetup1{          //防区设置
               unsigned char dType;   //防区类型     0 - 日防区  1 - 夜防区  2 - 24小时防区  3 - 自定义防区
               unsigned char dDelay;   //延时时间    0 - 0秒   1 - 5秒  2 - 10秒  3 - 15秒  4 - 20秒  5 - 25秒  6 - 60秒
               unsigned char tType;   //探头类型     0 - 烟感  1 - 燃气  2 - 防拆  3 - 紧急  4 - 红外  5 - 门磁  6 - 窗磁
               };
unsigned char DefenceStatus;       //布防状态      0 - 撤防  1 - 外出   2 - 在家   3 - 就寝   4 - 起床   5 - 自定义
    */                                              //                        11 - 外出   12 - 在家   13 - 就寝   14 - 起床   15 - 自定义     延时
       switch(LocalCfg.DefenceSetup_dType[i])
        {
         case 3: //自定义防区
                switch(LocalCfg.DefenceStatus)
                 {
                  case 1:
                  case 2:
                  case 3:
                  case 4:
                  case 5:
                  //case 11:    //在布防延时中，触发防区不报警
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
                                       LocalCfg.DefenceInfo[i][3] = 1;  //有报警
                                      }
                                     break;
                              default:
                                     Local.AlarmDelayFlag[i] = 1;     //报警延时标志
                                     Local.AlarmDelayTime[i] = 0;    //计数

                                     PlayAlarmDelayWav(); //播放 报警延时中 提示
                                     break;
                             }
                           }
                          break;
                 }
                break;
         case 2: //24小时防区
                switch(LocalCfg.DefenceSetup_dDelay[i])
                 {
                  case 0:
                         if(LocalCfg.DefenceInfo[i][3] != 2)
                          {
                           AlarmByte[i/8] |= (tmpbyte << i);
                           LocalCfg.DefenceInfo[i][3] = 1;  //有报警
                          }
                         break;
                  default:
                         Local.AlarmDelayFlag[i] = 1;     //报警延时标志
                         Local.AlarmDelayTime[i] = 0;    //计数

                         PlayAlarmDelayWav(); //播放 报警延时中 提示
                         break;
                 }
                break;
        }
     }
    }
  if((AlarmByte[0] != 0x0)||(AlarmByte[1] != 0x0))
   {
    xtm_printf("AlarmByte[0] = 0x%X, AlarmByte[1] = 0x%X\n", AlarmByte[0], AlarmByte[1]);

    SetGpio(GPIOA_OUT_DATA, ALARM_OUT_HIGH, SET_HIGH); //开报警输出
    SendAlarmFunc(0, AlarmByte);
    for(i=0; i<2; i++)
      Local.AlarmByte[i] = AlarmByte[i];
    //在对讲状态下，不显示
    if(Local.Status == 0)
      DisplayAlarmWindow(REFRESH_TOP);
   }

#endif   
#endif	
}
//---------------------------------------------------------------------------
