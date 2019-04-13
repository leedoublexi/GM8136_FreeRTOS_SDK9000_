#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>

#include "lib_lcd.h"

#include "lib_pwm.h"

#include "lib_panel.h"



#include "common.h"
#define HBP_480x272    24
#define HFP_480x272    210
#define HW_480x272     20
#define VFP_480x272    22  
#define VW_480x272     2//10
#define VBP_480x272     12
#define FREQ_480x272   10


//LCD 1
#define HBP_800x480    24
#define HFP_800x480    210
#define HW_800x480     20
#define VFP_800x480    22
#define VW_800x480     10
#define VBP_800x480    12
#define FREQ_800x480   33


//LCD 2
#define HBP_1024x600    140//24
#define HFP_1024x600    160
#define HW_1024x600     20
#define VFP_1024x600    12
#define VW_1024x600     2
#define VBP_1024x600    20
#define FREQ_1024x600   36//39


#define LCD_POWER 	PWM0


void Lcd_Power(uint8_t uFlag, uint8_t level);    //20110608 xu
void Disp_Enable(uint8_t uFlag);    //20120305 xu

void disp_init(void);

//freq;  duty_steps;  duty_ratio;
pwm_set_t pwm_lcd_on[6]  	= {
 								{20000,100,40}, 
								{20000,100,50},									
								{20000,100,60}, 
								{20000,100,70}, 
								{20000,100,80}, 
								{20000,100,90}
							  };	
pwm_set_t pwm_lcd_off 	= {100000,100,0};

//---------------------------------------------------------------------------
void Lcd_Power(uint8_t uFlag, uint8_t level)    //20110608 xu    1 -- 开背光   0 -- 关背光
{
#if 1
  xtm_printf("Lcd_Power::uFlag = %d, level = %d\n", uFlag, level);

   //printf("pwm_set_pinmux::4 PimMux, pmu 0x64 = 0x%08X\n", gm_pmu_read_reg(0x64));

   if (uFlag)
    {
     Disp_Enable(1);    //开显示
    }
   else
    {
     Disp_Enable(0);    //关显示
    }

   if (uFlag)
    {
       //开背光
	 gm_pwm_set(LCD_POWER, &pwm_lcd_on[level]);
	 //  gm_gpio_direction_output(LCDPOWER_IO, 1);
    }
   else
    {
       //关背光
	 gm_pwm_set(LCD_POWER, &pwm_lcd_off);   
	 //  gm_gpio_direction_output(LCDPOWER_IO, 0);
    }
#endif
}
//---------------------------------------------------------------------------
void Disp_Enable(uint8_t uFlag)    //20120305 xu
{
#if 0
  if (uFlag)
   {
    CloseDispClockFlag = 0;
    Set_Disp_Clock(1); //操作显示时钟  1  开   0  关
    outportb(CONTROL_REG, inportb(CONTROL_REG)|0x01);  //0 0000 001  显示使能
    
    setvect(18, VInt);
    outportb(DE_J_CONTROL, inportb(DE_J_CONTROL)|0x20);//使能V中断
   }
  else
   {
    outportb(DE_J_CONTROL, inportb(DE_J_CONTROL)&0xdf);//禁止V中断
    //CloseDispClockFlag = 1;
    //outportb(CONTROL_REG, inportb(CONTROL_REG)&0xfe);  //1 1111 110  显示禁止    0xfe
    //Set_Disp_Clock(0); //操作显示时钟  1  开   0  关
   }
#endif  
}
//---------------------------------------------------------------------------
static void LcdIsr()
{
    //gm_lcd_set_frame_buf(LCD_FRAME_BUF_0, lcd_buffer_addr[0]);
	//gm_lcd_set_frame_buf(LCD_FRAME_BUF_1, lcd_buffer_addr[1]);
}
//---------------------------------------------------------------------------
void 	disp_init(void)
{
	lcd_buffer0_addr[0] = GUI_BUF0;   //fb0 显示界面
	lcd_buffer0_addr[1] = GUI_BUF1;   //fb1
	lcd_buffer1_addr = GUI_BUF2;      //pip 1 显示视频

    Local.fb_page = 0;
	xtm_clear_disp_ram(Local.fb_page, 0);

    gm_pwmtmr_init(); 
	gm_pwm_request(LCD_POWER);

	gm_panel_init(2);
  if ((DISPLAY_SRC_WIDTH_MAX == 1024)&&(DISPLAY_SRC_HEIGHT_MAX == 600))
  {
    gm_lcd_init(DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX, FREQ_1024x600);
  }
  else if((DISPLAY_SRC_WIDTH_MAX == 800)&&(DISPLAY_SRC_HEIGHT_MAX == 480))
  {
    gm_lcd_init(DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX, FREQ_800x480);
  }
  else if((DISPLAY_SRC_WIDTH_MAX == 480)&&(DISPLAY_SRC_HEIGHT_MAX == 272))
  {
    gm_lcd_init(DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX, FREQ_480x272);
  }

#ifdef _RGB
	gm_lcd_set_input_params(LCD_INPUT_RGB565, LCD_L_BYTE_L_PX);
#else
	gm_lcd_set_input_params(LCD_INPUT_YCBCR422, LCD_L_BYTE_L_PX);
#endif

	//gm_lcd_set_output_params(TV_COMPOSITE_PAL);
	gm_lcd_ScreenEn(1);
	//gm_lcd_set_paneltypeEn(1);
	//gm_lcd_set_LcdEn(1);	//0x200
	//gm_lcd_Lcd480x272Timing(DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX);
  if((DISPLAY_SRC_WIDTH_MAX == 1024)&&(DISPLAY_SRC_HEIGHT_MAX == 600))
  {
    gm_lcd_Timing_User(DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX, 
      HBP_1024x600, HFP_1024x600, HW_1024x600, VFP_1024x600, VW_1024x600, VBP_1024x600);
  }
  else if((DISPLAY_SRC_WIDTH_MAX == 800)&&(DISPLAY_SRC_HEIGHT_MAX == 480))
  {
    gm_lcd_Timing_User(DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX, 
      HBP_800x480, HFP_800x480, HW_800x480, VFP_800x480, VW_800x480, VBP_800x480);
  }
  else if((DISPLAY_SRC_WIDTH_MAX == 480)&&(DISPLAY_SRC_HEIGHT_MAX == 272))
  {
    gm_lcd_Timing_User(DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX, 
      HBP_480x272, HFP_480x272, HW_480x272, VFP_480x272, VW_480x272, VBP_480x272);
  }	
  gm_lcd_set_scaler_params(DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX, DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX);
	gm_lcd_set_intr(1, LCD_INTR_VSTATUS, LCD_INTR_VFPORCH_START, (void*)LcdIsr);
	gm_lcd_enable_screen_tv(1, 0);
	gm_lcd_set_pip_position(1, 100, 100);
	gm_lcd_set_pip_size(1, 100, 100);//DISPLAY_SRC_WIDTH_MAX, DISPLAY_SRC_HEIGHT_MAX);
	gm_lcd_set_pip_priority(0xE4);
#ifdef _RGB
	gm_lcd_set_pip_format1(0x44);
#else
	gm_lcd_set_pip_format1(0xCC);
#endif
	gm_lcd_set_pip_mode(0, 0);//1); // 1 pip, global alpha blending	
	gm_lcd_set_frame_buf(LCD_FRAME_BUF_0, lcd_buffer0_addr[0]);
	gm_lcd_set_frame_buf(LCD_FRAME_BUF_1, lcd_buffer1_addr);
	//gm_lcd_reg_value();		
	
  Local.fb_page = 0;
  Local.fb_addr[0] = GUI_BUF0;
  Local.fb_addr[1] = GUI_BUF1;
	Local.fb1_addr = GUI_BUF2;
	
  selectym(Local.fb_page);	
	
	printf("disp_init::1\n");
}
//---------------------------------------------------------------------------
