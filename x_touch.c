

#include "stdlib.h"

#define CommonH
#include "common.h"

uint8_t CheckTouchDelayTime(void);  //触摸屏处理时检查延迟

void TouchScr_Func(uint8_t ScrType, uint16_t touch_x, uint16_t touch_y); //触摸屏输入
uint8_t CheckTSInButton(struct TImageButton *t_button, uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该按钮内
uint8_t CheckTSInEdit(struct TEdit *t_edit, uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该Edit内
uint8_t CheckTSInImage(struct TImage *t_image, uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该Image内
uint8_t CheckTSInVideoScreen(uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该视频窗口内
uint8_t CheckTSInPicScreen(uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该照片窗口内
uint8_t CheckBigmenuFunc(uint16_t XLCD, uint16_t YLCD);  //快捷按钮操作
//---------------------------------------------------------------------------
#if 0
void TouchScr_Func(uint8_t ScrType, uint16_t touch_x, uint16_t touch_y) //触摸屏输入
{
  INT16 X,Y, XLCD, YLCD;

  uint8_t isInTouch;

  //已经处在结束通话切换界面状态
  if(PicStatBuf.Flag == 1)
  {
    xtm_printf("TouchScr_Func::PicStatBuf.Flag = %d\n", PicStatBuf.Flag);
    return;
  }

  if((ScrType==GT911_SCR)||(ScrType==ICN85XX_SCR)||(ScrType==GSLX680_SCR)||(ScrType==FT5X0X_SCR) )
  {
    XLCD = touch_x;
    YLCD = touch_y;
  }
  else if(ScrType == ZT2083_SCR)
  {
	  if(LocalCfg.Ts_Mode == 0)
	  {
		  X = touch_x;
		  Y = touch_y;
	  }
	  else			 //1
	  {
		  X = touch_y;
		  Y = touch_x;
	  }
	  if(LocalCfg.Ts_deltaX == 0)
		  LocalCfg.Ts_deltaX = 1;
	  if(LocalCfg.Ts_deltaY == 0)
		  LocalCfg.Ts_deltaY = 1;   

	  XLCD = (INT32)(X - LocalCfg.Ts_X0)*SCREEN_WIDTH/LocalCfg.Ts_deltaX + SCREEN_WIDTH/2;
	  YLCD = (INT32)(Y - LocalCfg.Ts_Y0)*SCREEN_HEIGHT/LocalCfg.Ts_deltaY + SCREEN_HEIGHT/2;
  } 

  xtm_printf("X=%d, Y=%d, XLCD=%d, YLCD=%d, LocalCfg.Ts_X0=%d, LocalCfg.Ts_Y0=%d, LocalCfg.Ts_deltaX=%d, LocalCfg.Ts_deltaY=%d, Ts_Mode = %d\n", 
  	X, Y, XLCD, YLCD, LocalCfg.Ts_X0, LocalCfg.Ts_Y0,     
     LocalCfg.Ts_deltaX, LocalCfg.Ts_deltaY, LocalCfg.Ts_Mode);
  
  if(XLCD < 0)
    XLCD = 0;
  if(XLCD > SCREEN_WIDTH)
    XLCD = SCREEN_WIDTH;
  if(YLCD < 0)
    YLCD = 0;
  if(YLCD > SCREEN_HEIGHT)
    YLCD = SCREEN_HEIGHT;
  
  xtm_printf("Local.CurrentWindow=%d\n", Local.CurrentWindow);

  Local.KeyPressTime = 0;
  Local.LcdLightTime = 0;
  if(Local.LcdLightFlag == 0)
  {
    //点亮LCD背光
    Lcd_Power(1, LocalCfg.BrightVolume);
    Local.LcdLightFlag = 1; //LCD背光标志
    Local.LcdLightTime = 0; //时间
  }
  else
  {
    isInTouch = 0;
    switch(Local.CurrentWindow)
    {
      case 0: //主窗口
             CheckTouchFirstWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 12: //视频呼叫窗口
             CheckTouchTalkWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 13: //监视窗口
             CheckTouchWatchWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 16://显示图像操作窗口（二级）,有呼叫时自动显示
             CheckTouchTalkWatchPicWindow(XLCD, YLCD); //检查触摸屏操作
             break;
 /*     #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
      case 25://转接
             CheckTouchTalkTurnWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      #endif          */
      case 21://通话记录窗口（一级）
             CheckTouchRecordsWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 22://查看通话记录窗口（二级，弹出）
             CheckTouchRecordsContentWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 31:  //撤防界面操作（二级）
             CheckTouchCancelFortifyWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 32:  //布防界面操作（二级）
             CheckTouchFortifyWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 34://显示报警窗口（二级）
             CheckTouchAlarmWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 35:  //取消报警界面操作（二级）
             CheckTouchCancelAlarmWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 111: //信息窗口
             CheckTouchInfoWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 112://信息显示窗口（二级，弹出）
             CheckTouchInfoContentWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 5://设置窗口（一级）
             CheckTouchSetupWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 149://KB窗口
             CheckTouchKbWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 151://LAN设置窗口（二级）
             CheckTouchLanWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 152: //设置主窗口
             CheckTouchSetupMainWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 154://房号设置窗口（二级）
             CheckTouchAddrWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 165://屏幕设置窗口
             CheckTouchScreenSetupWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 166://音量调节窗口
             CheckTouchVoiceSetupWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 168://修改密码窗口
             CheckTouchPassSetupWindow(XLCD, YLCD); //检查触摸屏操作
             break;             
      case 158://版本信息窗口
             CheckTouchVersionWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 190: //校准触摸屏窗口
             PlaySoundTip(); //播放提示音

             Calib_X[Local.CalibratePos] = X;
             Calib_Y[Local.CalibratePos] = Y;
             Calib_OldX[Local.CalibratePos] = touch_x;
             Calib_OldY[Local.CalibratePos] = touch_y;
             OperateCalibrateWindow(0, 190);
             break;
       }
     }    
}
#endif

void TouchScr_Func(uint8_t ScrType, uint16_t touch_x, uint16_t touch_y) //触摸屏输入
{
  INT16 X,Y, XLCD, YLCD;

  uint8_t isInTouch;

  //已经处在结束通话切换界面状态
  if(PicStatBuf.Flag == 1)
  {
    xtm_printf("TouchScr_Func::PicStatBuf.Flag = %d\n", PicStatBuf.Flag);
    return;
  }

  if((ScrType==GT911_SCR)||(ScrType==ICN85XX_SCR)||(ScrType==GSLX680_SCR)||(ScrType==FT5X0X_SCR) )
  {
    XLCD = touch_x;
    YLCD = touch_y;
  }
  else if(ScrType == ZT2083_SCR)
  {
    if(LocalCfg.Ts_Mode == 0)
    {
      X = touch_x;
      Y = touch_y;
    }else      //1
    {
      X = touch_y;
      Y = touch_x;
    }

    if(LocalCfg.Ts_deltaX == 0)
      LocalCfg.Ts_deltaX = 1;
    if(LocalCfg.Ts_deltaY == 0)
      LocalCfg.Ts_deltaY = 1;   
    
    XLCD = (INT32)(X - LocalCfg.Ts_X0)*SCREEN_WIDTH/LocalCfg.Ts_deltaX + SCREEN_WIDTH/2;
    YLCD = (INT32)(Y - LocalCfg.Ts_Y0)*SCREEN_HEIGHT/LocalCfg.Ts_deltaY + SCREEN_HEIGHT/2;
  } 

  xtm_printf("X=%d, Y=%d, XLCD=%d, YLCD=%d, LocalCfg.Ts_X0=%d, LocalCfg.Ts_Y0=%d, LocalCfg.Ts_deltaX=%d, LocalCfg.Ts_deltaY=%d, Ts_Mode = %d\n", 
    X, Y, XLCD, YLCD, LocalCfg.Ts_X0, LocalCfg.Ts_Y0,     
     LocalCfg.Ts_deltaX, LocalCfg.Ts_deltaY, LocalCfg.Ts_Mode);
  
  if(XLCD < 0)
    XLCD = 0;
  
  if(XLCD > SCREEN_WIDTH)
    XLCD = SCREEN_WIDTH;
  
  if(YLCD < 0)
    YLCD = 0;
  
  if(YLCD > SCREEN_HEIGHT)
    YLCD = SCREEN_HEIGHT;
  
  xtm_printf("Local.CurrentWindow=%d\n", Local.CurrentWindow);

  Local.KeyPressTime = 0;
  Local.LcdLightTime = 0;
  if(Local.LcdLightFlag == 0)
  {
    //点亮LCD背光
    Lcd_Power(1, LocalCfg.BrightVolume);
    Local.LcdLightFlag = 1; //LCD背光标志
    Local.LcdLightTime = 0; //时间
  }else
  {
    isInTouch = 0;
    switch(Local.CurrentWindow)
    {
      case 0: //主窗口
             CheckTouchFirstWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 12: //视频呼叫窗口
             CheckTouchTalkWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 13: //监视窗口
             CheckTouchWatchWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 16://显示图像操作窗口（二级）,有呼叫时自动显示
             CheckTouchTalkWatchPicWindow(XLCD, YLCD); //检查触摸屏操作
             break;
 /*     #ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
      case 25://转接
             CheckTouchTalkTurnWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      #endif          */
      case 21://通话记录窗口（一级）
             CheckTouchRecordsWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 22://查看通话记录窗口（二级，弹出）
             CheckTouchRecordsContentWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 31:  //撤防界面操作（二级）
             CheckTouchCancelFortifyWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 32:  //布防界面操作（二级）
             CheckTouchFortifyWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 34://显示报警窗口（二级）
             CheckTouchAlarmWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 35:  //取消报警界面操作（二级）
             CheckTouchCancelAlarmWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 111: //信息窗口
             CheckTouchInfoWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 112://信息显示窗口（二级，弹出）
             CheckTouchInfoContentWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 5://设置窗口（一级）
             CheckTouchSetupWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 149://KB窗口
             CheckTouchKbWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 151://LAN设置窗口（二级）
             CheckTouchLanWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 152: //设置主窗口
             CheckTouchSetupMainWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 154://房号设置窗口（二级）
             CheckTouchAddrWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 165://屏幕设置窗口
             CheckTouchScreenSetupWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 166://音量调节窗口
             CheckTouchVoiceSetupWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 168://修改密码窗口
             CheckTouchPassSetupWindow(XLCD, YLCD); //检查触摸屏操作
             break;             
      case 158://版本信息窗口
             CheckTouchVersionWindow(XLCD, YLCD); //检查触摸屏操作
             break;
      case 190: //校准触摸屏窗口
             PlaySoundTip(); //播放提示音

             Calib_X[Local.CalibratePos] = X;
             Calib_Y[Local.CalibratePos] = Y;
             Calib_OldX[Local.CalibratePos] = touch_x;
             Calib_OldY[Local.CalibratePos] = touch_y;
             OperateCalibrateWindow(0, 190);
             break;
       }
     }    
}

//---------------------------------------------------------------------------
uint8_t CheckTouchDelayTime(void)  //触摸屏处理时检查延迟
{
/*  struct timeval tv;
  gettimeofday(&tv, NULL);
  Local.newtouchtime = tv.tv_sec *1000 + tv.tv_usec/1000;
  if(((Local.newtouchtime - Local.oldtouchtime) < TOUCHMAXTIME)&&(Local.newtouchtime > Local.oldtouchtime))
    return 0;
  else
    Local.oldtouchtime = Local.newtouchtime;    */
  return 1;
}
//---------------------------------------------------------------------------
uint8_t CheckTSInButton(struct TImageButton *t_button, uint16_t XLCD, uint16_t YLCD) //检查触摸点是否在该按钮内
{
  if((XLCD >= t_button->xLeft)&&(XLCD <= (t_button->xLeft + t_button->width))
    &&(YLCD >= t_button->yTop)&&(YLCD <= (t_button->yTop + t_button->height)))
    return 1;
  else
    return 0;
}
//---------------------------------------------------------------------------
uint8_t CheckTSInEdit(struct TEdit *t_edit, uint16_t XLCD, uint16_t YLCD) //检查触摸点是否在该Edit内
{
  if((XLCD >= t_edit->xLeft)&&(XLCD <= (t_edit->xLeft + t_edit->width))
    &&(YLCD >= t_edit->yTop)&&(YLCD <= (t_edit->yTop + t_edit->height)))
    return 1;
  else
    return 0;
}
//---------------------------------------------------------------------------
uint8_t CheckTSInImage(struct TImage *t_image, uint16_t XLCD, uint16_t YLCD) //检查触摸点是否在该Image内
{
  if((XLCD >= t_image->xLeft)&&(XLCD <= (t_image->xLeft + t_image->width))
    &&(YLCD >= t_image->yTop)&&(YLCD <= (t_image->yTop + t_image->height)))
    return 1;
  else
    return 0;
}
//---------------------------------------------------------------------------
uint8_t CheckTSInPicScreen(uint16_t XLCD, uint16_t YLCD) //检查触摸点是否在该照片窗口内
{
  uint8_t i;
  uint16_t Pic_X[3] = {87, 301, 509};
  uint16_t Pic_Y[3] = {127, 128, 128};
  for(i=0; i<3; i++)
   if((XLCD >= Pic_X[i])&&(XLCD <= (Pic_X[i] + 192))
    &&(YLCD >= Pic_Y[i])&&(YLCD <= (Pic_Y[i] + 144)))
    {
     return i+1;
    }
  return 0;
}
//---------------------------------------------------------------------------
uint8_t CheckBigmenuFunc(uint16_t XLCD, uint16_t YLCD)  //快捷按钮操作
{
  uint8_t i;
  uint8_t isInTouch;
  for(i=0; i<7; i++)  //大菜单按钮
   {
    isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
    if(isInTouch == 1)
     {
      return (i + 100);
      //键盘和触摸屏处理函数
      //KeyAndTouchFunc(i + 'a' + 30);
      //break;
     }
   }
  return 0;
}
//---------------------------------------------------------------------------

