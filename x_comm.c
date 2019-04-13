
#include "stdlib.h"
#include "lib_pwm.h"


#define x_comm_h

#define CommonH
#include "common.h"

#define _COMM_OPERATE   1

void dcomm1(void);
void dcomm2(void);
void CommSendBuff(uint8_t comm_fd, uint8_t *comm_buf, uint16_t send_len);

void KeyTouchOperate(uint8_t TouchNum); //按键响应
void KeyPreDeal(void); //按键预处理，按下效果
void KeyBackDeal(void); //按键后处理，弹起效果 并处理

uint8_t CheckKeyPressAndLcd(void); //检查按键并点亮LCD
//---------------------------------------------------------------------------
void CommSendBuff(uint8_t comm_fd, uint8_t *comm_buf, uint16_t send_len)
{
#if 0
  uint16_t i;
  for (i=0; i<send_len; i++)
   {
    if(comm_fd == 0)
     {
      while((inportb(UART0_TX_STATUS)&0x1)!=0x1);	// 等待发送保持器为空
      outportb(UART0_TX_DATA, comm_buf[i]);				// 发送字符
     }
    if(comm_fd == 1)
     {
      while((inportb(UART1_TX_STATUS)&0x1)!=0x1);	// 等待发送保持器为空
      outportb(UART1_TX_DATA, comm_buf[i]);				// 发送字符
     }
   }
#endif  
}
//---------------------------------------------------------------------------
void InitComm(void)  
{
	xTaskCreate(dcomm1, "dcomm1", 4096, NULL, 2, NULL ); 
}
//---------------------------------------------------------------------------

uint8_t ErrorStopDecode = 0;
void dcomm1(void)
{
    	int i;
		uint16_t color;
		uint8_t eeprom_bin[512];

	   u8 get_uart_data = 0;
	   color = 0x8000;
	   while(1){
		   printf("dcomm1...\n");
		   //xtm_clear_disp_ram(Local.fb_page, color);
		   //color = color >> 1; 

			   get_uart_data = gm_uart_getc(0);
		
			   switch(get_uart_data)
				{
#if _COMM_OPERATE
				 case 'A':
    					
							/*printf("color = 0x%X\n", color);
							xtm_clear_disp_ram(Local.fb_page, color);
							color = color >> 1;	
							if(color == 0)
								color = 0x8000;*/
						  break;
				 case 'B':		
				 	DisplayCalibrateWindow();	//校准触摸屏
						  break;
				 case 'C':
						  xtm_printf("C\n");				 	
						  if(LocalCfg.BrightVolume < 5)
							LocalCfg.BrightVolume ++;							
						  else
						  	LocalCfg.BrightVolume = 0;
						  Lcd_Power(1, LocalCfg.BrightVolume);
						  //gm_lcd_enable_screen_tv(1, 0);

						  break;
				 case 'D':
						  xtm_printf("D\n");
							Lcd_Power(0, 0);
							//gm_lcd_enable_screen_tv(0, 0);
						  break;
				 case 'E':
						  xtm_printf("E\n");
						  Local.HavePicRecorded = 1;
						  TmpRecordsContent.HavePic = 1;  //有无图像标志  0   1 -- 640x480  2 -- 320x240
           				  TmpRecordsContent.Length = 20000;
						  WritePicFunc();
						  break;
				 case 'F':
						  xtm_printf("F\n");
						  /*LocalCfg.SpkVolume = 5;
						  SoundSetSpeakerVolume(LocalCfg.SpkVolume);
						  StartPlayWav(soundtip_wav.Faddr, CYCLE_PLAY);*/
						  break;
				 case 'G':
						  break;
				 case 'H':
						  Local.GpioWatchDogStarted = 0;  //在GPIO检测线程中启动WatchDog
						  break;
				 case 'I':
						  StopPlayWavFile();
						  break;
				 case 'J':
						  xtm_printf("J\n");
						  break;
		
				 case 'K':
						  xtm_printf("K\n");
						  GT911_Init();   //初始化电容触摸屏
						  break;
				 case 'L':
				          if(Local.Status == 0)
				           {
				            DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, WATCHWINTYPE);
				            Watch_Func();
				           }				 	
						  break;
				 case 'T':
				 		  gm_gpio_set_value(NET_IO, SET_LOW); 
						  xtm_printf("T\n");
						  break;
				 case 'M':
				 		  gm_gpio_set_value(NET_IO, SET_HIGH); 
						  xtm_printf("M\n");
						  break;
				 case 'N':
						  break;
				 case 'O':
						  DisplayCalibrateWindow();
						  break;
				 case 'P':
				 		  xtm_printf("P\n");
						  //StopPlayWavFile();
						  //WaitAudioUnuse(1000); //等待声卡空闲	
				 		  //StartPlayWav(alarm_wav.Faddr, CYCLE_PLAY);
				 		  StartPlayWav(soundtip_wav.Faddr, NO_CYCLE_PLAY);
						  break;
				 case 'R':
						 break;
				 case 'S':
						  xtm_printf("S\n");
						  StopPlayWavFile();
						  break;
				 case 'U':
				 		  StartPlayWav(ring_wav.Faddr, NO_CYCLE_PLAY);
						  xtm_printf("U\n");
				 case 'W':
				 	      xtm_printf("W\n");
				 		  ReadFromFlash(0x001F0000, eeprom_bin, 512);
		     #ifdef _XTM8900_SUPPORT  //XTM8900网卡支持
    			net_write_eeprom(eeprom_bin);
		  	 #endif
		  	 #ifdef _XTM9000_SUPPORT  //XTM9000网卡支持
				net_write_eeprom_9000(eeprom_bin);
		  	 #endif						  
						  break;
				 case 'Y':
						  break;
				 case 'Z':
						  break;
				 case 'a':	//0
				 case 'b':	//1
				 case 'c':	//2
				 case 'd':	//3
				 case 'e':	//4
				 case 'f':	//5
				 case 'g':	//6
				 case 'h':	//7
				 case 'i':	//8
				 case 'j':	//9
				 case 'k':	//10
				 case 'l':	//11
				 case 'm':	//12
				 case 'n':	//13
				 case 'o':	//14
						  
						  KeyTouchOperate(get_uart_data - 'a');
						  xtm_printf("dcomm1::read, %d\n", get_uart_data - 'a');
						  break;
				 case '0':
				 case '1':
				 case '2':
				 case '3':
				 case '4':
				 case '5':
				 case '6':
						  KeyTouchOperate(100 + get_uart_data - '0');
						  break;
		
				 case '7':
						  break;
				 case '8':
						  break;
				 case '9':
						  break;
#endif
		
				 }

	   	}
	   vTaskDelete(NULL);
}
//---------------------------------------------------------------------------
void KeyTouchOperate(uint8_t TouchNum) //按键响应
{
#if 1
  if(CheckKeyPressAndLcd()) //检查按键并点亮LCD
    return;
  printf("KeyTouchOperate::1\n");
  if(Local.KeyTouchNo == 0)
   {
    if(Local.Status == 0)
      PlaySoundTip(); //播放提示音
    //vTaskDelay(100);
    Local.KeyTouchNo = TouchNum + 1; //按钮按下标志
    KeyPreDeal(); //按键预处理，按下效果
    Local.KeyTouchTime = KEYDELAYTIME;
   }
#endif  
}
//---------------------------------------------------------------------------
void KeyPreDeal(void) //按键预处理，按下效果
{
  xtm_printf("KeyPreDeal::Local.CurrentWindow = %d\n", Local.CurrentWindow);
  switch(Local.CurrentWindow)
   {
    case 0:     //主窗口预处理，按键按下效果
           PreDealFirstWindow(Local.KeyTouchNo - 1);
           break;
    case 5:    //进入设置窗口预处理，按键按下效果
           PreDealSetupWindow(Local.KeyTouchNo - 1);
           break;
    case 12://呼出窗口
           PreDealTalkWindow(Local.KeyTouchNo - 1);
           break;
    case 13://监视窗口
           PreDealWatchWindow(Local.KeyTouchNo - 1);
           break;
    case 16:    //对讲呼入窗口预处理，按键按下效果
           PreDealTalkWatchPicWindow(Local.KeyTouchNo - 1);
           break;
    case 31:    //撤防窗口预处理，按键按下效果
           PreDealCancelFortifyWindow(Local.KeyTouchNo - 1);
           break;
    case 32:    //布防窗口预处理，按键按下效果
           PreDealFortifyWindow(Local.KeyTouchNo - 1);
           break;
    case 35:    //取消报警窗口预处理，按键按下效果
           PreDealCancelAlarmWindow(Local.KeyTouchNo - 1);
           break;
    case 21:    //通话记录窗口预处理，按键按下效果
           PreDealRecordsWindow(Local.KeyTouchNo - 1);
           break;
    case 22:    //查看通话记录窗口预处理，按键按下效果
           PreDealRecordsContentWindow(Local.KeyTouchNo - 1);
           break;
    case 111:    //信息窗口预处理，按键按下效果
           PreDealInfoWindow(Local.KeyTouchNo - 1);
           break;
    case 112:    //信息查看窗口预处理，按键按下效果
           PreDealInfoContentWindow(Local.KeyTouchNo - 1);
           break;
    case 152:    //设置主窗口预处理，按键按下效果
           PreDealSetupMainWindow(Local.KeyTouchNo - 1);
           break;
    case 149:    //键盘窗口预处理，按键按下效果
           PreDealKbWindow(Local.KeyTouchNo - 1);
           break;
    case 151:    //Lan设置窗口预处理，按键按下效果
           PreDealLanWindow(Local.KeyTouchNo - 1);
           break;
    case 154:    //房号设置窗口预处理，按键按下效果
           PreDealAddrWindow(Local.KeyTouchNo - 1);
           break;
    case 165:    //屏幕设置窗口预处理，按键按下效果
           PreDealScreenSetupWindow(Local.KeyTouchNo - 1);
           break;
    case 166:    //音量调节窗口预处理，按键按下效果
           PreDealVoiceSetupWindow(Local.KeyTouchNo - 1);
           break;
    case 168:    //修改密码窗口预处理，按键按下效果
           PreDealPassSetupWindow(Local.KeyTouchNo - 1);
           break;
    case 158:    //版本窗口预处理，按键按下效果
           PreDealVersionWindow(Local.KeyTouchNo - 1);
           break;
    case 190:    //校准触摸屏窗口预处理，按键按下效果
           PreDealCalibrateWindow(Local.KeyTouchNo - 1);
           break;
   }
}
//---------------------------------------------------------------------------
void KeyBackDeal(void) //按键后处理，弹起效果 并处理
{
  switch(Local.CurrentWindow)
   {
    case 0: //主界面
           OperateFirstWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 5: //进入设置
           OperateSetupWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 12://呼出窗口
           OperateTalkWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 13://监视窗口
           OperateWatchWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 16://呼入窗口
           OperateTalkWatchPicWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 31:    //撤防窗口
           OperateCancelFortifyWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 32:    //布防窗口
           OperateFortifyWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 35:    //取消报警窗口
           OperateCancelAlarmWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 21:    //通话记录窗口
           OperateRecordsWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 22:    //查看通话记录窗口
           OperateRecordsContentWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 111:    //信息窗口
           OperateInfoWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 112:    //信息查看窗口
           OperateInfoContentWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 152: //设置主窗口
           OperateSetupMainWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 149: //键盘窗口
           OperateKbWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 151: //LAN设置窗口
           OperateLanWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 154: //房号设置窗口
           OperateAddrWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 165:    //屏幕设置窗口预处理，按键按下效果
           OperateScreenSetupWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 166:    //音量调节窗口预处理，按键按下效果
           OperateVoiceSetupWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 168: //修改密码窗口
           OperatePassSetupWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 158: //版本窗口
           OperateVersionWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 190: //校准触摸屏操作
           OperateCalibrateWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
   }
}
//---------------------------------------------------------------------------
uint8_t CheckKeyPressAndLcd(void) //检查按键并点亮LCD
{
  Local.KeyPressTime = 0;
  Local.LcdLightTime = 0;
  if(Local.LcdLightFlag == 0)
   {
    printf("CheckKeyPressAndLcd::Local.LcdLightFlag = %d\n", Local.LcdLightFlag);
    //点亮LCD背光
    Lcd_Power(1, LocalCfg.BrightVolume);
    Local.LcdLightFlag = 1; //LCD背光标志
    Local.LcdLightTime = 0; //时间
    
    return 1;
   }
  return 0;
}
//---------------------------------------------------------------------------
void dcomm2(void)
{
#if 0
      uint16_t comlen;
      uint8_t buf[512];
      comlen = 0;
      while (comm2.RP!=comm2.WP)
       {
        #ifdef _XTM_ADD
         //xtm_printf("dcomm2::comm2.RP = %d, comm2.WP = %d, comm2.Rbuf[comm2.RP] = 0x%X\n", comm2.RP, comm2.WP, comm2.Rbuf[comm2.RP]);
        #endif
        buf[comlen] = comm2.Rbuf[comm2.RP];
        comm2.RP ++;
        if(comm2.RP >= COMBUFFLEN)
          comm2.RP = 0;
        comlen ++;
       }
      if(comlen > 0)
       {
        buf[comlen] = '\0';
        xtm_printf("dcomm2::len = %d, buf = %s, comm2.RP = %d, comm2.WP = %d\n", comlen, buf, comm2.RP, comm2.WP);
       }
#endif	  
}
//---------------------------------------------------------------------------
