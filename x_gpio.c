#include "stdlib.h"

#include "GT911.h"
#include "lib_i2c.h"
#include "Define.h"

#define x_gpio_h

#define CommonH
#include "common.h"                      



uint8_t WatchDogFlag = SET_LOW;

uint8_t have_zt2083  = 0;
uint8_t have_gt911   = 0;
uint8_t have_icn85xx = 0;     //���ICN85XX ֧��, 20180720 By zz
uint8_t have_gslx680 = 0;     //���GSLX680 ֧��, 20180824 By zz
uint8_t have_ft5x0x  = 0;     //���FT5X0X ֧��,  20180917 By zz



#define IO_S1        26             //������GPIO0 29
#define IO_S2        27             //������GPIO0 28
#define IO_S3        24             //������GPIO0 24
#define IO_S4        25             //������GPIO0 25


struct gpio_key1                    //���������
{
  uint8_t curr_input;               //����״̬
  uint8_t downnum;                  //��⵽���������
  uint8_t press_flag;
};
struct gpio_key1 gpio_key[6];
void get_key_value(uint8_t gpiovalue);

uint16_t openlockpressnum = 0;      //��������������
uint16_t touchpressnum = 0;         //��������������
uint16_t longtouchpressflag = 0;    //�������������

struct touch_io1         //���������
{
  uint8_t curr_input;   //����״̬
  uint8_t downnum;      //��⵽���������
  uint8_t press_flag;
  uint8_t touch_count;  //���������¼���
};
struct touch_io1 touch_io;

uint16_t timenum = 0;
void InitGpio(void);    //��ʼ��GPIO
void CheckGpio(void);    //���GPIO����

void InitKeyDelay(void);
void CheckKeyDelay(uint16_t timenum);

uint8_t zt2083_init(void);
uint8_t Touch_Read_XY(void); //������I2C��ȡ����

void ClearWatchDog(void);

#ifdef _PON_SUPPORT   //PON ֧�� 20140314
 #define IR_IO  0x80
 #define GPIOC2_IO  0x04
 #define GPIOC4_IO  0x10
 #define GPIOC5_IO  0x20
 UINT16 Pon_Check_Num = 0;
 struct pon_io1
  {
   UINT8 gpioc3_high;
   UINT8 gpioc3_low;
   UINT8 gpioc5_high;
   UINT8 gpioc5_low;
  };
 struct pon_io1 pon_io;

 void StartPonPower(void);  //����PON��Դ
 void Check_Pon_Status(void); //���PON״̬
#endif


//---------------------------------------------------------------------------
void InitGpio(void)  //��ʼ��GPIO
{
  uint8_t i, reg, temp;
  I2C010_PARAMS_T         i2c_param; 
  s32 ret;

  xtm_printf("InitGpio...\n");
  
  //initiate I2C   
  i2c_param.interrupt_mode = 0;   
  i2c_param.timeout = 100;  
  gm_i2c_init(&i2c_param); 

#if 1  
  for(i=0; i<6; i++)
  {
    gpio_key[i].curr_input = 0;
    gpio_key[i].press_flag = 0;
    gpio_key[i].downnum = 0;
  } 

  /*
  //����  
  gm_gpio_direction_output(FORTIFY_SEC_IO0, SET_LOW);
  
  gm_gpio_direction_input(FORTIFY_IO0);
  gm_gpio_pull_enable(FORTIFY_IO0);
  gm_gpio_pulltype_up(FORTIFY_IO0);  
  
  gm_gpio_direction_input(FORTIFY_IO1);
  gm_gpio_pull_enable(FORTIFY_IO1);
  gm_gpio_pulltype_up(FORTIFY_IO1);    
  
  gm_gpio_direction_input(FORTIFY_IO2);
  gm_gpio_pull_enable(FORTIFY_IO2);
  gm_gpio_pulltype_up(FORTIFY_IO2);  
  
  gm_gpio_direction_input(FORTIFY_IO3);
  gm_gpio_pull_enable(FORTIFY_IO3);
  gm_gpio_pulltype_up(FORTIFY_IO3);  

  gm_gpio_direction_input(FORTIFY_IO4);
  gm_gpio_pull_enable(FORTIFY_IO4);
  gm_gpio_pulltype_up(FORTIFY_IO4);  

  gm_gpio_direction_input(FORTIFY_IO5);
  gm_gpio_pull_enable(FORTIFY_IO5);
  gm_gpio_pulltype_up(FORTIFY_IO5);  

  gm_gpio_direction_input(FORTIFY_IO6);
  gm_gpio_pull_enable(FORTIFY_IO6);
  gm_gpio_pulltype_up(FORTIFY_IO6);  

  gm_gpio_direction_input(FORTIFY_IO7);
  gm_gpio_pull_enable(FORTIFY_IO7);
  gm_gpio_pulltype_up(FORTIFY_IO7);    
  */
 
//-------------------------������ �ж�GPIO-------------------------
  gm_gpio_direction_input(TOUCH_IO);//������GPIO0 17
  gm_gpio_pull_enable(TOUCH_IO);
  gm_gpio_pulltype_down(TOUCH_IO);

  gm_gpio_direction_input(ZT2083_TOUCH_IO);

  gm_gpio_direction_output(WATCHDOG_IO, WatchDogFlag);
  gm_gpio_direction_output(NET_IO, SET_HIGH);

  //PON ��Դ
  gm_gpio_direction_output(PON_PWR_IO, SET_LOW );

  // gm_gpio_direction_input(IO_S1);
  // gm_gpio_pull_enable(IO_S1);
  // gm_gpio_pulltype_up(IO_S1);

  // gm_gpio_direction_input(IO_S2);
  // gm_gpio_pull_enable(IO_S2);
  // gm_gpio_pulltype_up(IO_S2);

  // gm_gpio_direction_input(IO_S3);
  // gm_gpio_pull_enable(IO_S3);
  // gm_gpio_pulltype_up(IO_S3);

  // gm_gpio_direction_input(IO_S4);
  // gm_gpio_pull_enable(IO_S4);
  // gm_gpio_pulltype_up(IO_S4);  

  //��ʼ��������������
//  have_zt2083 = zt2083_init();
//  printf("InitGpio, have_zt2083 = %d\n", have_zt2083);

  #ifdef  TOUCH_GT911_ENABLE
    have_gt911 = GT911_Init();   //��ʼ�����ݴ�����
    printf("InitGpio, have_gt911 = %d\n", have_gt911);
  #endif

  #ifdef  TOUCH_ICN85XX_ENABLE
    have_icn85xx = ICN85XX_Init();
    printf("InitGpio, have_icn85xx = %d\n", have_icn85xx);
  #endif  
  
  #ifdef  TOUCH_GSLX680_ENABLE
    have_gslx680 = gslx680_Test_I2C();
    printf("InitGpio, have_gslx680 = %d\n", have_gslx680);
  #endif  

  #ifdef  TOUCH_FT5X0X_ENABLE
    have_ft5x0x = ft5x0x_init();
    printf("InitGpio, have_ft5x0x = %d\n", have_ft5x0x);
  #endif  

  temp = gm_gpio_get_value(TOUCH_IO);
  printf("InitGpio, touch_io = 0x%X \n", temp);
  //0-4 ����
  //outportb(GPIOD_OEN_DATA, inportb(GPIOD_OEN_DATA) | 0x1F);
  //5-7 ���  LED1��2��3    ��Ϣ������������ 

  InitKeyDelay();

  /*
  gm_gpio_set_value(INFO_LED_HIGH, SET_HIGH); //����ϢLED
  gm_gpio_set_value(NET_LED_HIGH, SET_HIGH); //������LED
  
  if(LocalCfg.DefenceStatus != 0)
    gm_gpio_set_value(FORTIFY_LED_LOW, SET_LOW); //������LED
  else
    gm_gpio_set_value(FORTIFY_LED_HIGH, SET_HIGH); //�ز���LED

  gm_gpio_set_value(ALARM_OUT_LOW, SET_LOW); //�ر������*/
  //SetGpio(GPIOA_OUT_DATA, ALARM_OUT_HIGH, SET_HIGH); //���������
  ClearWatchDog();
#endif
  
  //
  printf("start checkgpio task....\n");
  xTaskCreate(CheckGpio, 
              "CheckGpio", 
              4096, 
              NULL, 
              NORMAL_PRIORITY, 
              NULL ); 
}
//---------------------------------------------------------------------------
#if 1
void InitKeyDelay(void)
{
  Local.WatchKeyTouched = 0;  //���Ӽ� ��Ӧ
  Local.WatchKeyTouchedTime = 0;  //���Ӽ� ��Ӧ ����

  Local.CenterKeyTouched = 0;  //�������ļ� ��Ӧ
  Local.CenterKeyTouchedTime = 0;  //�������ļ� ��Ӧ ����
}
//---------------------------------------------------------------------------
void CheckKeyDelay(uint16_t timenum)
{
  if(Local.WatchKeyTouched == 1)      //���Ӽ� ��Ӧ     20101029  xu
  {
    if((timenum % (GPIOPERSEC))==0)
    {
      Local.WatchKeyTouchedTime ++;      //���Ӽ� ��Ӧ ����
      xtm_printf("���Ӽ� ��ʱ��...  %d\n", Local.WatchKeyTouchedTime);
    }
    if(Local.WatchKeyTouchedTime >= 2)
    {
      Local.WatchKeyTouched = 0;      //���Ӽ� ��Ӧ ��־     20101029  xu
      Local.WatchKeyTouchedTime = 0;      //���Ӽ� ��Ӧ ����
    }
  }

  if(Local.CenterKeyTouched == 1)      //�������ļ� ��Ӧ     20101029  xu
  {
    if((timenum % (GPIOPERSEC))==0)
    {
      Local.CenterKeyTouchedTime ++;      //�������ļ� ��Ӧ ����
      xtm_printf("�������ļ� ��ʱ��...  %d\n", Local.CenterKeyTouchedTime);
    }
    if(Local.CenterKeyTouchedTime >= 2)
    {
      Local.CenterKeyTouched = 0;      //�������ļ� ��Ӧ ��־     20101029  xu
      Local.CenterKeyTouchedTime = 0;      //�������ļ� ��Ӧ ����
    }
  }
}
#endif
//---------------------------------------------------------------------------
void CheckGpio(void)    //���GPIO����
{
  uint8_t gpio_value;
  uint8_t tmpvalue = 1, tmpvalue1 = 1, i;

  while(1) {
    #if 1  	
		  ClearWatchDog();
		
		  tmpvalue = 1;
		  tmpvalue1 = 1;
		  gpio_value = 0;
	  
		  CheckKeyDelay(timenum);
		  timenum ++;
#ifdef _PON_SUPPORT   //PON ֧�� 20140314
  		//if(LocalCfg.Infrared_Support == 1)
  		//xtm_printf("timenum is %d\n",timenum);
   		if((timenum % 25)==0)
			xtm_printf("Check_Pon_Status Start...\n");
  			Check_Pon_Status(); //���PON״̬
#endif 
      
      //����
		  //CheckDefence(0xff);
      #if 1
        if(have_zt2083)
          //tmpvalue = gm_gpio_get_value(ZT2083_TOUCH_IO);
          tmpvalue = gm_gpio_get_value(TOUCH_IO);

        #ifdef  TOUCH_GT911_ENABLE
          if(have_gt911)
            tmpvalue1 = gm_gpio_get_value(TOUCH_IO);    
        #endif

        #ifdef TOUCH_ICN85XX_ENABLE
          if(have_icn85xx)
            tmpvalue1 = gm_gpio_get_value(TOUCH_IO);
        #endif  

        #ifdef  TOUCH_GSLX680_ENABLE
          if(have_gslx680)
            tmpvalue1 = gm_gpio_get_value(TOUCH_IO);
        #endif

        #ifdef  TOUCH_FT5X0X_ENABLE
          if(have_ft5x0x)
            tmpvalue1 = gm_gpio_get_value(TOUCH_IO);
        #endif 
	  	  
		   // xtm_printf("tmpvalue = 0x%X, tmpvalue1 = 0x%X, have_gt911 = %d\n", tmpvalue, tmpvalue1, have_gt911);
		    touch_io.curr_input = 0;
		    if(gpio_value == 0)
		    {
          if((tmpvalue == 0) || (tmpvalue1 == 0)) 
		      {
		        //���������
		        touch_io.curr_input = 1; //����״̬
		      }
        }
		 
        if( (have_gt911)||(have_icn85xx)||(have_gslx680)||(have_ft5x0x) )
        {
          if(touch_io.curr_input == 1)
            Touch_Read_XY();
        }
        else if(have_zt2083)
        {
          if(touch_io.curr_input == 1)
            touchpressnum ++;       //��������������
          else
            touchpressnum = 0;
			 
			    //����������8S����
			    if((touchpressnum > (GPIOPERSEC * 8))&&(Local.CurrentWindow != 190)&&(Local.CurrentWindow != 191))
			    {
			      touchpressnum = 0;
			      if((Local.Status == 0)&&(Local.AlarmByte[0] == 0)&&(Local.AlarmByte[1] == 0))
				      DisplayCalibrateWindow();	 //У׼������
			    }
			 
			    //���������
			    if(touch_io.curr_input == 0) //����״̬
			    {
			      touch_io.downnum = 0;  //��⵽���������
			      touch_io.press_flag = 0;
			      touch_io.touch_count = 0; //���������¼���
			    } 
			    else
			    {
			      //xtm_printf("tmpvalue = 0x%X\n", tmpvalue);
			      touch_io.downnum ++;  //��⵽���������
			      touch_io.touch_count ++;
			      if(touch_io.press_flag == 0)
				      if(touch_io.downnum >= 5)
				      {
				        touch_io.press_flag = 1;
				        xtm_printf("tmpvalue1 = 0x%X\n", tmpvalue1);
				        touch_io.downnum = 0;
			 
				        if(touch_io.touch_count >= 2)
                {
					        touch_io.touch_count = 0;
					        Touch_Read_XY();
				        }  
				        vTaskDelay(20/portTICK_RATE_MS);
				        continue;
				      }
			    }		
		 	  }
      #endif

      #if 0
		  //tmpvalue = gm_gpio_get_value(TOUCH_IO);
		  tmpvalue = 1;

		  tmpvalue = gm_gpio_get_value(IO_S1);
		  gpio_key[0].curr_input = 0;
		  if(gpio_value == 0)
		    if(tmpvalue == 0)
		    {
		      gpio_key[0].curr_input = 1; //����״̬
		      gpio_value = 1;
		    }

	    tmpvalue = gm_gpio_get_value(IO_S2);
		  gpio_key[1].curr_input = 0;
		  if(gpio_value == 0)
		    if(tmpvalue == 0)
		    {
		      gpio_key[1].curr_input = 1; //����״̬
		      gpio_value = 1;
		    }

	    tmpvalue = gm_gpio_get_value(IO_S3);
		  gpio_key[2].curr_input = 0;
		  if(gpio_value == 0)
		    if(tmpvalue == 0)
		    {
		      gpio_key[2].curr_input = 1; //����״̬
		      gpio_value = 1;
		    }

	    tmpvalue = gm_gpio_get_value(IO_S4);
		  gpio_key[3].curr_input = 0;
		  if(gpio_value == 0)
		    if(tmpvalue == 0)
		    {
		      gpio_key[3].curr_input = 1; //����״̬
		      gpio_value = 1;
		    }
	  
		  /*
      gpio_key[4].curr_input = 0;
		  if(gpio_value == 0)
		    if(!(tmpvalue & IO_S5) )
		    {
		      gpio_key[4].curr_input = 1; //����״̬
		      gpio_value = 1;
		    }
      */
	    #endif

		  //��������������  
		  if(gpio_key[1].curr_input == 1)
		    openlockpressnum ++;
		  else
		    openlockpressnum = 0;
	  
		  //����������3S����
		  if((openlockpressnum > (GPIOPERSEC * 3))&&(Local.CurrentWindow != 190)&&(Local.CurrentWindow != 191))
		  {
		    openlockpressnum = 0;
		    if((Local.Status == 0)&&(Local.AlarmByte[0] == 0)&&(Local.AlarmByte[1] == 0))
			    DisplayCalibrateWindow();	//У׼������
		  }
	  
		  //IO���
		  for(i=0; i<4; i++)
		  {
		    if(gpio_key[i].curr_input == 0) //����״̬
		    {
			    if(gpio_key[i].press_flag == 0)
			    {
			      if(gpio_key[i].downnum >= 3)
			      {
				      gpio_key[i].press_flag = 1;
				      get_key_value(i);
				      gpio_key[i].downnum = 0;
			      }
			    }
			    else
			    {
			      gpio_key[i].downnum = 0;	//��⵽���������
			      gpio_key[i].press_flag = 0;
			    } 
		    }
		    else
		    {
			    gpio_key[i].downnum ++;  //��⵽���������
		    }
		  }
    #endif	

		if(Local.KeyTouchNo) //��ť���±�־
		{
		  //printf("CheckGpio::Local.KeyTouchTime = %d\n", Local.KeyTouchTime);
		  if (Local.KeyTouchTime <= 0)
		  {
		    Local.KeyTouchTime = KEYDELAYTIME;
		    KeyBackDeal(); //������������Ч�� ������
		    Local.KeyTouchNo = 0; //��ť���±�־
		  } 	
		  else
		 	  Local.KeyTouchTime -= 20;
		}
		vTaskDelay(20/portTICK_RATE_MS);
  }
}

//---------------------------------------------------------------------------
uint8_t wav_num = 20;
void get_key_value(uint8_t gpiovalue)
{
  uint8_t val;
  //�Ѿ����ڽ���ͨ���л�����״̬
  if(PicStatBuf.Flag != 0)
  {
    xtm_printf("get_key_value::PicStatBuf.Flag = %d\n", PicStatBuf.Flag);
    return;
  }
  
  Local.LcdLightTime = 0;
  if(Local.Status == 0)
  {
    PlaySoundTip(); //������ʾ��
  }

  if(Local.LcdLightFlag == 0)
  {
    //����LCD����
    Lcd_Power(1, LocalCfg.BrightVolume);
    Local.LcdLightFlag = 1; //LCD�����־
    Local.LcdLightTime = 0; //ʱ��
  }
  else
  {
    #if 1
      switch(gpiovalue)
      {
        case 0:   //0  //ͨ��
            xtm_printf("get_key_value::IO_S1\n");
            if((Local.CurrentWindow == 16)&&(Local.Status != 0))  //ͨ��
            {
              switch(Local.Status)
              {
                  case 2: //���жԽ�
                      OperateTalkWatchPicWindow(0, 16);
                      break;
                  case 1:                             
                  case 5: //ͨ��
                  case 6: //ͨ��
                      OperateTalkWatchPicWindow(2, 16);
                      break;
              }
              Local.RecordPic = 0;   //��������Ƭ
              break;
            }
          break;
        case 1:     // ����
            xtm_printf("get_key_value::IO_S2\n");
            #if 1
              #ifdef _ZHUHAIJINZHEN      //�麣����  ����ʱ�ɿ���
                if((Local.Status == 2)||(Local.Status == 6))  //״̬Ϊ���Խ� �򱻽�ͨ��
              #else
                if(Local.Status == 6)  //״̬Ϊ���Խ�
              #endif
                {
                  OpenLock_Func();
                }
            #else
              strcpy(Remote.Addr[0], "S00010308090");
              strcpy(TalkCall.TalkText, "0001030809");

              if(Local.Status == 0)
              {
                if(Local.CurrentWindow == 16)
                  break;
                DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, TALKWINTYPE);
                CallR2R_Func(TalkCall.TalkText);
              }
            #endif
          break;
        case 2:   //1     ����
            xtm_printf("get_key_value::IO_S3\n");
            // DisplayImage(&main_image11111, FROMFILE, NOFULLPAGE);
            if(Local.WatchKeyTouched == 1)  //���Ӽ� ��Ӧ
              break;

            Local.WatchKeyTouched = 1;  //���Ӽ� ��Ӧ
            Local.WatchKeyTouchedTime = 0;  //���Ӽ� ��Ӧ ����
                                
            //���ڱ�����ȡ������������
            if((Local.CurrentWindow != 34)&&(Local.CurrentWindow != 35))
            {
              if((Local.Status == 0)&&(Local.CurrentWindow != 16))
              {
                DisplayTalkWatchPicWindow(MAINRETURNTYPE, WATCHWINTYPE);
                Watch_Func();
              }
              else
                if((Local.Status == 0)&&(Local.CurrentWindow == 13))
                  Watch_Func();
              
              if(Local.Status == 3)
              {
                WatchEnd_Func();
              }
            }
          break;			  
        /* 
        case 3:  //��Ӱ
            xtm_printf("get_key_value::IO_S4\n");
            if((Local.Status == 2)||(Local.Status == 6))
            {
              Local.ManualRecordFlag = 1; //�ֶ�¼��
              Local.IFrameCount = 30;
              break;
            }

            //���ڱ�����ȡ������������
            if((Local.CurrentWindow != 34)&&(Local.CurrentWindow != 35))
              if((Local.Status == 0)&&(Local.CurrentWindow != 16))
                if(Local.CurrentWindow != 21)
                {
                  DisplayRecordsWindow(REFRESH_MIDDLE | REFRESH_BOTTOM, 0);    //ˢ�� �� ��
                }

          break;
        case 4:   //��Ϣ
            xtm_printf("get_key_value::IO_S5\n");

            //���ڱ�����ȡ������������
            if((Local.CurrentWindow != 34)&&(Local.CurrentWindow != 35))
              if((Local.Status == 0)&&(Local.CurrentWindow != 16))
                if(Local.CurrentWindow != 53)
                {
                  DisplayInfoWindow(REFRESH_MIDDLE | REFRESH_BOTTOM, 0);    //ˢ�� �� ��
                }

          break;
          */
        case 3:  //����
            xtm_printf("get_key_value::IO_S4\n");
            if(Local.CenterKeyTouched == 1)  //�������ļ� ��Ӧ
              break;
            Local.CenterKeyTouched = 1;  //�������ļ� ��Ӧ
            Local.CenterKeyTouchedTime = 0;  //�������ļ� ��Ӧ ����

            //���ڱ�����ȡ������������
            if((Local.CurrentWindow != 34)&&(Local.CurrentWindow != 35))
            {
              if((Local.Status == 0)&&(Local.CurrentWindow != 16))
              {
                memcpy(Remote.Addr[0], "Z00010000000", 12);
                Remote.Addr[0][12] = '\0';
                DisplayTalkWatchPicWindow(MAINRETURNTYPE, TALKWINTYPE);
                CallCenter_Func("Z00010000000");
              }
            }
          break;
      }
    #endif
  }
}
//---------------------------------------------------------------------------
#ifdef _TCPM  //20150727 ������֧��
 uint8_t iicbufWR[8];
 uint8_t iicbufRD[8];
 uint16_t lastpointx;
 uint16_t lastpointy;
#endif
uint8_t zt2083_init(void)
{
	uint8_t i, ret;
  uint8_t tmp = 0;
	uint8_t reg = 0xC0;
	for(i=0; i<6; i++)
  {
	  ret = gm_i2c_write_zt2083(TOUCH_WRITE_ADDR >> 1, reg);
	  if(ret == 0)
    {
		  tmp = 1;
		  break;
	  }
	}
  return tmp;
}

uint8_t Touch_Read_XY(void) //������I2C��ȡ����
{
  uint8_t touch_ok = 0;
  uint16_t touch_cal_x;
  uint16_t touch_cal_y; 
  
  if(have_gt911)
  {
    //xtm_printf("Touch_Read_XY::22\n");
    touch_ok = MainScan(&touch_cal_x, &touch_cal_y);
    if(touch_ok == 1)
      TouchScr_Func(GT911_SCR, touch_cal_x, touch_cal_y);     
  }else if(have_icn85xx)
  {
    if(icn85xx_readEventId() == 3)
    {
      touchpressnum++;
      //����������8S����
      if((touchpressnum > (20))&&(Local.CurrentWindow == 0))
      {
        touchpressnum = 0;
        longtouchpressflag = 1 ;
        xtm_printf("get touch long press\n");
        TouchScr_Func(ICN85XX_SCR, icn85xx_readPointX(), icn85xx_readPointY());
      } 
    } else if(icn85xx_readEventId() == 4)    // ����������̧��ʱ����
    {
      touchpressnum = 0;        // �������������ͷ�ʱ �����������
      longtouchpressflag = 0;   // �������������ͷ�ʱ ����������
      TouchScr_Func(ICN85XX_SCR, icn85xx_readPointX(), icn85xx_readPointY());
    } 
  }else if(have_gslx680)
  {
    //ɨ�败������
    //touch_ok = gslx680_read_touch_x_y(&touch_cal_x, &touch_cal_y);
    //if(touch_ok == 1)
    //TouchScr_Func(GT911_SCR, touch_cal_x, touch_cal_y);
    gslx680_read_touch_x_y(&touch_cal_x, &touch_cal_y);
    TouchScr_Func(GSLX680_SCR, touch_cal_x, touch_cal_y);
  }else if (have_ft5x0x)
  {
    ft5x0x_read_touch_x_y(&touch_cal_x, &touch_cal_y);
    TouchScr_Func(FT5X0X_SCR, touch_cal_x, touch_cal_y);
  }else if(have_zt2083)
  {
		uint8_t i, j;
		uint8_t tmpvalue;
		uint8_t reg = 0;
		uint16_t touch_x[10];
		uint16_t touch_y[10];
		uint8_t tmp_x;
		uint16_t tmp_value[10];
		uint8_t result_i;
		uint16_t result_value[10];
		uint16_t tmptouch;
		uint8_t dec_max;
		uint8_t dec_tmp_x[2];

		 
		uint8_t key_count, buf[2];
		
		key_count = 0;
		for(i=0; i<5; i++)
		{    
		  reg = 0xC0;
		  gm_i2c_write_zt2083(TOUCH_WRITE_ADDR >> 1, reg);
		  gm_i2c_read_zt2083(TOUCH_WRITE_ADDR >> 1, buf, 2);	
		
		  touch_x[i] = buf[0];
		  touch_x[i] = touch_x[i] << 4;
		  touch_x[i] += ((buf[1] & 0xf0) >> 4);
		
		  reg = 0xD0;
		  gm_i2c_write_zt2083(TOUCH_WRITE_ADDR >> 1, reg);
		  gm_i2c_read_zt2083(TOUCH_WRITE_ADDR >> 1, buf, 2);  
		  touch_y[i] = buf[0];
		  touch_y[i] = touch_y[i] << 4;
		  touch_y[i] += ((buf[1] & 0xf0) >> 4);   
		   
		  if(touch_x[i] == 0x00)
			  key_count ++;
		   //xtm_printf("touch_x[%d] = %d, touch_y[%d] = %d\n", i, touch_x[i], i, touch_y[i]);
		}
		
		tmpvalue = gm_gpio_get_value(ZT2083_TOUCH_IO);
		if((tmpvalue == 1))
		{
		  xtm_printf("read touch but IO is high\n");
		  return 1;
		}
		
		//if(key_count >= 8)
		//  return 0;
		//for(i=0; i<10; i++)
		//  xtm_printf("touch_x[%d] = 0x%X, touch_y[%d] = 0x%X\n", i, touch_x[i], i, touch_y[i]);
		
		//���� X
		dec_max = 5;
		result_i = 0;
		for(j=0; j<5; j++)
		{
		  tmptouch = touch_x[0];
		  dec_tmp_x[0] = 0;
		  for(i=1; i<dec_max; i++)
			  if(touch_x[i] < tmptouch)
			  {
			    tmptouch = touch_x[i];
			    dec_tmp_x[0] = i;
			  }

		  tmp_x = 0;
		  for(i=0; i<dec_max; i++)
			  if((i != dec_tmp_x[0]))
			  {
			    tmp_value[tmp_x] = touch_x[i];
			    tmp_x ++;
			  }
		  
      result_value[result_i] = touch_x[dec_tmp_x[0]];
		  result_i ++;
		  dec_max --;
		  for(i=0; i<dec_max; i++)
			  touch_x[i] = tmp_value[i];
		}
		
		tmptouch = 0;
		for(i=1; i<4; i++)
			tmptouch += result_value[i];
		
    touch_cal_x = tmptouch/3;
		
		//for(i=4; i<7; i++)
		//  xtm_printf("x[%d] = 0x%X\n", i, result_value[i]);
		
		//���� Y
		dec_max = 5;
		result_i = 0;
		for(j=0; j<5; j++)
		{
		  tmptouch = touch_y[0];
		  dec_tmp_x[0] = 0;
		  for(i=1; i<dec_max; i++)
			  if(touch_y[i] < tmptouch)
			  {
			    tmptouch = touch_y[i];
			    dec_tmp_x[0] = i;
			  }
		  
      tmp_x = 0;
		  for(i=0; i<dec_max; i++)
			  if((i != dec_tmp_x[0]))
			  {
			    tmp_value[tmp_x] = touch_y[i];
			    tmp_x ++;
			  }

		  result_value[result_i] = touch_y[dec_tmp_x[0]];
		  result_i ++;
		  dec_max --;
		  for(i=0; i<dec_max; i++)
			  touch_y[i] = tmp_value[i];
		}
		
		tmptouch = 0;
		for(i=1; i<4; i++)
			tmptouch += result_value[i];
		
    touch_cal_y = tmptouch/3;
		
		if((touch_cal_x != 0x0)&&(touch_cal_y != 0xFFF0))
			touch_ok = 1;
		else
			xtm_printf("read i2c error %d\n", i);
   	
    if(touch_ok == 1)
     	TouchScr_Func(ZT2083_SCR, touch_cal_x, touch_cal_y);		  
	}

  return touch_ok;
}

#ifdef _PON_SUPPORT   //PON ֧�� 20140314
void StartPonPower(void)
{ 
  gm_gpio_set_value(PON_PWR_IO, SET_HIGH);
  DelayMs(500);
  gm_gpio_set_value(PON_PWR_IO, SET_LOW);
}

//--------------------------------------------------------------------------


void Check_Pon_Status(void) //���PON״̬
{
  UINT8 tmpvalue1, tmpvalue2,gpioc3_status, gpioc5_status;
  UINT8 OldStatus;

  if(Pon_Check_Num == 0)
   {
    pon_io.gpioc3_high = 0;
    pon_io.gpioc3_low = 0;
    pon_io.gpioc5_high = 0;
    pon_io.gpioc5_low = 0;
   }
  
  //tmpvalue = inportb(GPIOC_IN_DATA);
  tmpvalue1 = gm_gpio_get_value(CHECK_PON_STATUS1_IO);
  tmpvalue2 = gm_gpio_get_value(CHECK_PON_STATUS2_IO);
  xtm_printf("step2::check pon status gpio0_30 = %d,check pon status gpio0_31 = %d\n", tmpvalue1, tmpvalue2);

  if(!tmpvalue1)
  //if(!(tmpvalue & GPIOC4_IO))
    pon_io.gpioc3_low ++;
  else
    pon_io.gpioc3_high ++;
  if(!tmpvalue2)
  //if(!(tmpvalue & GPIOC5_IO))
    pon_io.gpioc5_low ++;
  else
    pon_io.gpioc5_high ++;
  
  xtm_printf("check_pon_status step1::Pon_Check_Num = %d\n", Pon_Check_Num);
  Pon_Check_Num ++;
  if(Pon_Check_Num > 100)
   {
    Pon_Check_Num = 0;
    gpioc3_status = 0;
    if(pon_io.gpioc3_high == 0)
      gpioc3_status = 0;   //��
    if(pon_io.gpioc3_low == 0)
      gpioc3_status = 1;   //��
    if((pon_io.gpioc3_high != 0)&&(pon_io.gpioc3_low != 0))
      gpioc3_status = 2;   //��

    if(pon_io.gpioc5_high == 0)
      gpioc5_status = 0;   //��
    if(pon_io.gpioc5_low == 0)
      gpioc5_status = 1;   //��
    if((pon_io.gpioc5_high != 0)&&(pon_io.gpioc5_low != 0))
      gpioc5_status = 2;   //��

    OldStatus = PonSearch.Pon_Status;
    PonSearch.Pon_Status = 1; //
	 if((gpioc3_status == 1)&&(gpioc5_status == 0)) {
		PonSearch.Pon_Status = 1;     //�����
	 }else if((gpioc3_status == 0)&&(gpioc5_status == 0)){
		 PonSearch.Pon_Status = 0;	 //δ�ϵ�
	 }else if((gpioc3_status == 0) && (gpioc5_status == 2)){	
		PonSearch.Pon_Status = 2;	  //δ�����
	 }
	/*
    if((gpioc3_status == 0)&&(gpioc5_status == 0))  //4 - ��   5 - ��
      PonSearch.Pon_Status = 0;  // 0 - ONUδ����  1 - δ�����  2 - ע�����  3 - δע��  4 - ע��ɹ�

    if((gpioc3_status == 1)&&(gpioc5_status == 2))  //4 - ��   5 - ��˸
      PonSearch.Pon_Status = 1; 
    if((gpioc3_status == 2)&&(gpioc5_status == 1))  //4 - ��˸   5 - ��
      PonSearch.Pon_Status = 2;  
    if((gpioc3_status == 1)&&(gpioc5_status == 1))  //4 - ��   5 - ��
      PonSearch.Pon_Status = 3;  
    if((gpioc3_status == 0)&&(gpioc5_status == 1))  //4 - ��   5 - ��
      PonSearch.Pon_Status = 4; 
      */
    if(PonSearch.Pon_Status == 0)
     {
      PonSearch.Status0_Num ++;
      if(PonSearch.Status0_Num >= 60)
       {
        StartPonPower();  //����PON��Դ
        PonSearch.Status0_Num = 0;
        xtm_printf("Check_Pon_Status::PonSearch.Pon_Status = %d, PonSearch.Status0_Num = %d, StartPonPower()\n", PonSearch.Pon_Status, PonSearch.Status0_Num);
       }
     }
    else
      PonSearch.Status0_Num = 0;

    if(OldStatus != PonSearch.Pon_Status)
       xtm_printf("Check_Pon_Status::OldStatus = %d, PonSearch.Pon_Status = %d\n", OldStatus, PonSearch.Pon_Status);

    xtm_printf("pon_io.gpioc4_high = %d, pon_io.gpioc4_low = %d, pon_io.gpioc5_high = %d, pon_io.gpioc5_low = %d\n", pon_io.gpioc3_high, pon_io.gpioc3_low, pon_io.gpioc5_high, pon_io.gpioc5_low);
    xtm_printf("PonSearch.Pon_Status = %d, gpioc4_status = %d, gpioc5_status = %d\n", PonSearch.Pon_Status, gpioc3_status, gpioc5_status);
   }
}
#endif

//--------------------------------------------------------------------------
void ClearWatchDog(void)
{
  uint8_t tmp_data;
  if(Local.GpioWatchDogStarted)  //��GPIO����߳�������WatchDog
  {    
    WatchDogFlag = !WatchDogFlag;
	  gm_gpio_set_value(WATCHDOG_IO, WatchDogFlag);
  }
}
//--------------------------------------------------------------------------
