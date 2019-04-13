
#include "stdlib.h"
#include "lib_pwm.h"


#define x_comm_h

#define CommonH
#include "common.h"

#define _COMM_OPERATE   1

void dcomm1(void);
void dcomm2(void);
void CommSendBuff(uint8_t comm_fd, uint8_t *comm_buf, uint16_t send_len);

void KeyTouchOperate(uint8_t TouchNum); //������Ӧ
void KeyPreDeal(void); //����Ԥ��������Ч��
void KeyBackDeal(void); //������������Ч�� ������

uint8_t CheckKeyPressAndLcd(void); //��鰴��������LCD
//---------------------------------------------------------------------------
void CommSendBuff(uint8_t comm_fd, uint8_t *comm_buf, uint16_t send_len)
{
#if 0
  uint16_t i;
  for (i=0; i<send_len; i++)
   {
    if(comm_fd == 0)
     {
      while((inportb(UART0_TX_STATUS)&0x1)!=0x1);	// �ȴ����ͱ�����Ϊ��
      outportb(UART0_TX_DATA, comm_buf[i]);				// �����ַ�
     }
    if(comm_fd == 1)
     {
      while((inportb(UART1_TX_STATUS)&0x1)!=0x1);	// �ȴ����ͱ�����Ϊ��
      outportb(UART1_TX_DATA, comm_buf[i]);				// �����ַ�
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
				 	DisplayCalibrateWindow();	//У׼������
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
						  TmpRecordsContent.HavePic = 1;  //����ͼ���־  0   1 -- 640x480  2 -- 320x240
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
						  Local.GpioWatchDogStarted = 0;  //��GPIO����߳�������WatchDog
						  break;
				 case 'I':
						  StopPlayWavFile();
						  break;
				 case 'J':
						  xtm_printf("J\n");
						  break;
		
				 case 'K':
						  xtm_printf("K\n");
						  GT911_Init();   //��ʼ�����ݴ�����
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
						  //WaitAudioUnuse(1000); //�ȴ���������	
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
		     #ifdef _XTM8900_SUPPORT  //XTM8900����֧��
    			net_write_eeprom(eeprom_bin);
		  	 #endif
		  	 #ifdef _XTM9000_SUPPORT  //XTM9000����֧��
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
void KeyTouchOperate(uint8_t TouchNum) //������Ӧ
{
#if 1
  if(CheckKeyPressAndLcd()) //��鰴��������LCD
    return;
  printf("KeyTouchOperate::1\n");
  if(Local.KeyTouchNo == 0)
   {
    if(Local.Status == 0)
      PlaySoundTip(); //������ʾ��
    //vTaskDelay(100);
    Local.KeyTouchNo = TouchNum + 1; //��ť���±�־
    KeyPreDeal(); //����Ԥ��������Ч��
    Local.KeyTouchTime = KEYDELAYTIME;
   }
#endif  
}
//---------------------------------------------------------------------------
void KeyPreDeal(void) //����Ԥ��������Ч��
{
  xtm_printf("KeyPreDeal::Local.CurrentWindow = %d\n", Local.CurrentWindow);
  switch(Local.CurrentWindow)
   {
    case 0:     //������Ԥ������������Ч��
           PreDealFirstWindow(Local.KeyTouchNo - 1);
           break;
    case 5:    //�������ô���Ԥ������������Ч��
           PreDealSetupWindow(Local.KeyTouchNo - 1);
           break;
    case 12://��������
           PreDealTalkWindow(Local.KeyTouchNo - 1);
           break;
    case 13://���Ӵ���
           PreDealWatchWindow(Local.KeyTouchNo - 1);
           break;
    case 16:    //�Խ����봰��Ԥ������������Ч��
           PreDealTalkWatchPicWindow(Local.KeyTouchNo - 1);
           break;
    case 31:    //��������Ԥ������������Ч��
           PreDealCancelFortifyWindow(Local.KeyTouchNo - 1);
           break;
    case 32:    //��������Ԥ������������Ч��
           PreDealFortifyWindow(Local.KeyTouchNo - 1);
           break;
    case 35:    //ȡ����������Ԥ������������Ч��
           PreDealCancelAlarmWindow(Local.KeyTouchNo - 1);
           break;
    case 21:    //ͨ����¼����Ԥ������������Ч��
           PreDealRecordsWindow(Local.KeyTouchNo - 1);
           break;
    case 22:    //�鿴ͨ����¼����Ԥ������������Ч��
           PreDealRecordsContentWindow(Local.KeyTouchNo - 1);
           break;
    case 111:    //��Ϣ����Ԥ������������Ч��
           PreDealInfoWindow(Local.KeyTouchNo - 1);
           break;
    case 112:    //��Ϣ�鿴����Ԥ������������Ч��
           PreDealInfoContentWindow(Local.KeyTouchNo - 1);
           break;
    case 152:    //����������Ԥ������������Ч��
           PreDealSetupMainWindow(Local.KeyTouchNo - 1);
           break;
    case 149:    //���̴���Ԥ������������Ч��
           PreDealKbWindow(Local.KeyTouchNo - 1);
           break;
    case 151:    //Lan���ô���Ԥ������������Ч��
           PreDealLanWindow(Local.KeyTouchNo - 1);
           break;
    case 154:    //�������ô���Ԥ������������Ч��
           PreDealAddrWindow(Local.KeyTouchNo - 1);
           break;
    case 165:    //��Ļ���ô���Ԥ������������Ч��
           PreDealScreenSetupWindow(Local.KeyTouchNo - 1);
           break;
    case 166:    //�������ڴ���Ԥ������������Ч��
           PreDealVoiceSetupWindow(Local.KeyTouchNo - 1);
           break;
    case 168:    //�޸����봰��Ԥ������������Ч��
           PreDealPassSetupWindow(Local.KeyTouchNo - 1);
           break;
    case 158:    //�汾����Ԥ������������Ч��
           PreDealVersionWindow(Local.KeyTouchNo - 1);
           break;
    case 190:    //У׼����������Ԥ������������Ч��
           PreDealCalibrateWindow(Local.KeyTouchNo - 1);
           break;
   }
}
//---------------------------------------------------------------------------
void KeyBackDeal(void) //������������Ч�� ������
{
  switch(Local.CurrentWindow)
   {
    case 0: //������
           OperateFirstWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 5: //��������
           OperateSetupWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 12://��������
           OperateTalkWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 13://���Ӵ���
           OperateWatchWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 16://���봰��
           OperateTalkWatchPicWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 31:    //��������
           OperateCancelFortifyWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 32:    //��������
           OperateFortifyWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 35:    //ȡ����������
           OperateCancelAlarmWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 21:    //ͨ����¼����
           OperateRecordsWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 22:    //�鿴ͨ����¼����
           OperateRecordsContentWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 111:    //��Ϣ����
           OperateInfoWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 112:    //��Ϣ�鿴����
           OperateInfoContentWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 152: //����������
           OperateSetupMainWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 149: //���̴���
           OperateKbWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 151: //LAN���ô���
           OperateLanWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 154: //�������ô���
           OperateAddrWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 165:    //��Ļ���ô���Ԥ������������Ч��
           OperateScreenSetupWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 166:    //�������ڴ���Ԥ������������Ч��
           OperateVoiceSetupWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 168: //�޸����봰��
           OperatePassSetupWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 158: //�汾����
           OperateVersionWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
    case 190: //У׼����������
           OperateCalibrateWindow(Local.KeyTouchNo - 1, Local.CurrentWindow);
           break;
   }
}
//---------------------------------------------------------------------------
uint8_t CheckKeyPressAndLcd(void) //��鰴��������LCD
{
  Local.KeyPressTime = 0;
  Local.LcdLightTime = 0;
  if(Local.LcdLightFlag == 0)
   {
    printf("CheckKeyPressAndLcd::Local.LcdLightFlag = %d\n", Local.LcdLightFlag);
    //����LCD����
    Lcd_Power(1, LocalCfg.BrightVolume);
    Local.LcdLightFlag = 1; //LCD�����־
    Local.LcdLightTime = 0; //ʱ��
    
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
