

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "lib_board.h"
#include "lib_lcd.h"
#include "io.h"

#include "lib_panel.h"
#include "lib_spi.h"
#include "lib_mm.h"
#include "lib_mcp100.h"


#define x_disp_h
#include "x_disp.h"

//#define CommonH
#include "define.h"

#define HZK24S_START 0x00110000
#define HZK24T_START 0x0018A000

#define EN16START  0x001A3000
#define EN24START  0x001E6000
#define HZK16_START 0x001A4000


void xtm_clear_disp_ram(uint8_t PageNo, uint16_t color);
void clear_area(uint16_t x,uint16_t y,uint16_t Hi,uint16_t Wi,uint16_t color_val,uint8_t PageNo);

void selectym(uint8_t ye);
void gra_dispcn16(uint8_t *ch, int x, int y,uint16_t color_val, uint8_t FullPageType);
void gra_dispcn24(uint8_t *ch, int x, int y,uint16_t color_val, uint8_t FullPageType);
void display_text(uint16_t x, uint16_t y, uint8_t wd, uint16_t clr, char *s, uint8_t pass, uint8_t fonttype, uint8_t FullPageType); //显示点阵汉字

//先把JPEG读到内存
uint32_t Malloc_Jpeg_Memory(uint32_t FLen);

//预处理 Image 文件
uint8_t PreDealImage(struct TImage *t_image);
uint8_t DisplayImage(struct TImage *t_image, uint8_t FromType, uint8_t FullPageType);

//预处理 ImageButton 文件
uint8_t PreDealImageButton(struct TImageButton *t_button, uint8_t buttontype);
uint8_t DisplayImageButton(struct TImageButton *t_button, uint8_t buttontype, uint8_t FromType, uint8_t FullPageType);
//显示图形按键文本
void DisplayImageButton_Text(struct TImageButton *t_button, uint16_t color, uint8_t wd, uint8_t FullPageType);

//预处理 Edit 文件
uint8_t PreDealEdit(struct TEdit *t_edit, uint8_t buttontype);
uint8_t DisplayEdit(struct TEdit *t_edit, uint8_t buttontype, uint8_t FromType, uint8_t FullPageType);
void DisplayEdit_Text(struct TEdit *t_edit, uint16_t color, uint8_t wd, uint8_t FullPageType);
void DisplayPassEdit_Text(struct TEdit *t_edit, uint16_t color, uint8_t wd, uint8_t FullPageType);

uint32_t CurrJpegMemory;

uint32_t *Local_fb_addr;
uint8_t Local_fb_page;
uint32_t Local_fb1_addr;

uint8_t *disp_jpeg_bitstream;
extern u32 lcd_buffer0_addr[2];
//#define _PRINTINFO

xSemaphoreHandle  jpeg_access_mutex = NULL;
void *Jpeg_Dec_Handle;


void DecoceVideo(uint8_t *Video_Maddr, uint32_t Video_Len);

//---------------------------------------------------------------------------
static void JpgdIsr()
{
    printf("JpgdIsr::...\n");
}
static void jpge_isr_cb()
{
   u32 jpeg_done;
   xSemaphoreGiveFromISR( jpeg_access_mutex, NULL );  
}

void jpeg_dec_init(void)
{   
   JPEGDEC_INIT_PARAM_T init_param;

   if(jpeg_access_mutex == NULL){
      vSemaphoreCreateBinary(jpeg_access_mutex);
   }

   gm_mcp100_init();

   init_param.output_format = 2;//2;    //0:OUTPUT_YUV, 1:OUTPUT_420_YUV, 2:OUTPUT_CbYCrY (LCD)
   gm_jpgdec_init(&Jpeg_Dec_Handle, &init_param);

   gm_jpgdec_enable_intr((PrHandler2)jpge_isr_cb);

   CurrJpegMemory = JPEG_MEMORY_ADDR;
   
	printf("jpeg_dec_init, Jpeg_Dec_Handle = 0x%X\n",  Jpeg_Dec_Handle);
	//if(ret<0)
	//	printf("jpeg_dec_init RET = %d \n", ret);   
}
//---------------------------------------------------------------------------
void xtm_clear_disp_ram(uint8_t PageNo, uint16_t color)
{
	uint8_t Y;
	uint8_t U;
	uint8_t V;	

	uint32_t i, pixsize = DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX*2;
	uint8_t *fb_mem = MEMORY_STEAM_BUF0;
#ifdef _RGB
	for(i=0; i<pixsize; i+=2)
	 {
	   *(fb_mem + i) = (color & 0xff00) >> 8;//240;     //U
	   *(fb_mem + i + 1)= (color & 0xff);//41;   //Y	   
	 }

#else	
  switch(color)
   {
    case COLOR_WHITE: 
               Y=235;
               U=128;
               V=128;
               break;
    case COLOR_YELLOW:            
    		   Y=210;               
			   U=16;
               V=146;
               break;    
	case COLOR_CYAN:               
				Y=170;               
				U=166;
               V=16;
               break;    
	case COLOR_GREEN:            
			   Y=145;               
			   U=54;
               V=34;
               break;        
	case COLOR_RED:               
				Y=81;               
				U=90;
                V=240;
                break;    
	case COLOR_BLUE:               
				Y=41;               
				U=240;
                V=110;
                break;    
	case COLOR_BLACK: 
               Y=16;
               U=128;
               V=128;
               break;
    default:
               Y=16;
               U=128;
               V=128;
               break;
   }	
	for(i=0; i<pixsize; i+=2)
	 {
	  	    for(i=0; i<pixsize; i+=4)
	         {
	           *(fb_mem + i) = U;//240;     //U
			   *(fb_mem + i + 1)= Y;//41;   //Y
	           *(fb_mem + i + 2) = V;//110; //V
			   *(fb_mem + i + 3)= Y;//41;   //Y		   
	         }
	 }
#endif	
	memcpy((void *)lcd_buffer0_addr[PageNo], fb_mem, DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX*2);

}
//---------------------------------------------------------------------------
void clear_area(uint16_t x,uint16_t y,uint16_t Hi,uint16_t Wi,uint16_t color_val,uint8_t PageNo)
{
	uint16_t i, j;
	uint8_t *addr;
	
	for(i=0;i<Hi;i++){
		addr = (((uint32_t) (y + i)*SCREEN_WIDTH +(uint32_t) x)<<1) + lcd_buffer0_addr[PageNo];	
		for(j=0; j<Wi; j+=2)
	 	{
			*(addr + j) = 0xe0;
			*(addr + j + 1 )= 0x07;
	 	}	
	}
}
//---------------------------------------------------------------------------
void selectym(uint8_t ye)
{
    switch (ye)
    {
     case 0:
	 	gm_lcd_set_frame_buf(LCD_FRAME_BUF_0, lcd_buffer0_addr[0]);		
		break;
	 case 1:
	 	gm_lcd_set_frame_buf(LCD_FRAME_BUF_0, lcd_buffer0_addr[1]);
		break;	 	
    }
}
//---------------------------------------------------------------------------
void gra_dispen(uint16_t x, uint16_t y, char ch, uint8_t FullPageType)
{
}
//---------------------------------------------------------------------------
int fb_pixel(int width, int height, int x, int y, uint16_t color, uint8_t FullPageType)
{  
  uint8_t Y;
  uint8_t U;
  uint8_t V;  
  uint8_t *fb_mem ;
  if ((x > width) || (y > height))    
  	return (-1);  

  switch(color)
   {
    case COLOR_WHITE: 
               Y=235;
               U=128;
               V=128;
               break;
    case COLOR_YELLOW:            
    		   Y=210;               
			   U=16;
               V=146;
               break;    
	case COLOR_CYAN:               
				Y=170;               
				U=166;
               V=16;
               break;    
	case COLOR_GREEN:            
			   Y=145;               
			   U=54;
               V=34;
               break;        
	case COLOR_RED:               
				Y=81;               
				U=90;
                V=240;
                break;    
	case COLOR_BLUE:               
				Y=41;               
				U=240;
                V=110;
                break;    
	case COLOR_BLACK: 
               Y=16;
               U=128;
               V=128;
               break;
    default:
               Y=16;
               U=128;
               V=128;
               break;
   }
	Local_fb_addr = GetLocal_fb_addr();
	Local_fb_page = GetLocal_fb_page(); 
	
	if(FullPageType)
	  fb_mem = (((uint32_t) y*width +(uint32_t) x)<<1) + Local_fb_addr[!Local_fb_page];	   //显示到后台页
	else
	  fb_mem = (((uint32_t) y*width +(uint32_t) x)<<1) + Local_fb_addr[Local_fb_page];	   //显示到当前页  
//printf("color = %d, Y = %d, U = %d, V = %d\n", color, Y, U, V);
	if((x%2) == 0)
		*fb_mem = U;
	else
		*fb_mem = V;
  	*(fb_mem + 1) = Y;

  return (0);
}//---------------------------------------------------------------------------

void gra_dispcn16(uint8_t *ch, int x, int y,uint16_t color_val,uint8_t FullPageType)
{
	uint8_t ccdot[16][2];
	uint8_t ascdot[16];
	int col,byte,bit,mask; 
	int cx,cy, i;

	int qu, wei;
	uint32_t addr1;

	while(*ch){
		if(*ch>0x80){
    	qu=*ch++;
    	wei=*ch;

        addr1=(qu-161)*94+(wei-161);
    	addr1<<=5;
    	addr1=HZK16_START+addr1;
		//printf("gra_dispcn16::1, addr1 = 0x%X\n", addr1);
		gm_spi_flash_data_read(addr1, 32, ccdot);
		//printf("gra_dispcn16::2\n");
		//for(i=0; i<32; i++)
		//	printf("i = %d, %X, \n", i, ccdot[i/2][i%2]);

        for(col=0;col<16;col++)
         { 
          cx=x+col;
          for(byte=0;byte<2;byte++)
           { 
            cy=y+8*byte;
            mask=0x80;
            for(bit=0;bit < 8; bit++)
             {
              if(ccdot[col][byte]&mask)
                  fb_pixel(SCREEN_WIDTH, SCREEN_HEIGHT,
                           x+8*byte+bit, y+col, color_val, FullPageType);
               mask =mask>>1;
              }
            }
         }
		
	  	x=x+16;
	  	ch++;

		}else{
			qu=*ch++;
			addr1 = qu<<4;
    		addr1=EN16START+addr1;
			gm_spi_flash_data_read(addr1, 16, ascdot);

			for(byte=0;byte < 16;byte++){
			  mask=0x80;
			  cy=y+byte+2;//*2;
			 for(bit=0;bit < 8;bit++){
			   if(ascdot[byte]&mask){
				 cx=x+bit;//*2;
				 fb_pixel(SCREEN_WIDTH, SCREEN_HEIGHT,
							  cx,cy, color_val, FullPageType);
				}
			   mask=mask>>1;
			  }
		  }

          x=x+8;		
	}
 }
}
//---------------------------------------------------------------------------
void gra_dispcn24(uint8_t *ch, int x, int y,uint16_t color_val, uint8_t FullPageType)
{
	uint8_t ccdot[24][3];
	uint8_t ascdot[24][2];

	int col,byte,bit,mask; 
	int cx,cy, i, j;

	int qu, wei;
	uint32_t addr1;

	while(*ch){
		if(*ch>0x80){
			qu=*ch++;
				wei=*ch;
				if(qu <= 0xa3)	  //制表符及全角字符
				 {
				  addr1=(qu-161)*94+(wei-161);
				  addr1*=72;
				  addr1+=HZK24T_START;
				 }
				else
				 {
				  addr1=(qu-161-15)*94+(wei-161);	 //24点阵字库中没有制表符等,故需减一偏移量
				  addr1*=72;
				  addr1+=HZK24S_START;
				 }

				gm_spi_flash_data_read(addr1, 72, ccdot);
				//printf("addr1 = 0x%X, %d, %d\n", addr1, qu, wei);
				/*for(i=0; i<24; i++)
					for(j=0; j<3; j++)
						printf("0x%X, ", ccdot[i][j]);
				printf("\n");*/
				for(col=0;col<24;col++){
						   cy=y+col;
						   for(byte=0;byte<3;byte++){
							 cx=x+8*byte;
							 mask=0x80;
							 for(bit=0;bit < 8; bit++){
							   if(ccdot[col][byte]&mask)
									 fb_pixel(SCREEN_WIDTH, SCREEN_HEIGHT,
											  cx+bit, cy, color_val, FullPageType);
							   mask =mask>>1;
							  }
							}
						  }
				x=x+24;				
	
	  	ch++;
	  
		}else{
			qu=*ch++;
			if (qu>0x19){
				addr1 = qu*48;//qu*36;
    		addr1+=EN24START;

						gm_spi_flash_data_read(addr1, 48, ascdot);
						for(col=0;col<24;col++){
								   cy=y+col;
								   for(byte=0;byte<2;byte++){
									 cx=x+8*byte;
									 mask=0x80;
									 for(bit=0;bit < 8; bit++){
									   if(ascdot[col][byte]&mask)
											 fb_pixel(SCREEN_WIDTH, SCREEN_HEIGHT,
													  cx+bit,cy, color_val, FullPageType);
									   mask =mask>>1;
									  }
									}
								  }
						x=x+12;	

 		}
	}
  }
}
//---------------------------------------------------------------------------
void display_text(uint16_t x, uint16_t y, uint8_t wd, uint16_t clr, char *s, uint8_t pass, uint8_t fonttype, uint8_t FullPageType) //显示点阵汉字
{

  if(fonttype == 24)
    gra_dispcn24(s, x, y, clr, FullPageType);//COLOR_BLUE);
  else
    gra_dispcn16(s, x, y, clr, FullPageType);//COLOR_BLUE); 
}
//---------------------------------------------------------------------------
uint32_t Malloc_Jpeg_Memory(uint32_t FLen)
{
    uint32_t Maddr;
	Maddr = CurrJpegMemory;
	CurrJpegMemory += FLen;
	return Maddr;
}

//预处理 Image 文件
uint8_t PreDealImage(struct TImage *t_image)
{
  uint8_t read_spi[512];
  struct file_head1 file_head;
#ifdef _PRINTINFO
  xtm_printf("t_image->Faddr = 0x%X %X\n", \
                                (uint16_t)((t_image->Faddr & 0xffff0000) >> 16), (uint16_t)(t_image->Faddr & 0xffff));
#endif
  gm_spi_flash_data_read(t_image->Faddr, sizeof(struct file_head1), read_spi);

#ifdef _PRINTINFO
  xtm_printf("0x%X, 0x%X, 0x%X, 0x%X\n", read_spi[0], read_spi[1], read_spi[2], read_spi[3]);
#endif
  memcpy(&file_head, read_spi, sizeof(struct file_head1));
#ifdef _PRINTINFO
  xtm_printf("读 Faddr = 0x%X , Filelen = 0x%X, Flen = 0x%X, TotalPackage = %d, CurrPackage = %d, Width = %d, Height = %d, xLeft = %d, yTop = %d, Datalen = %d\n", 
                                file_head.Faddr , 
                                file_head.Filelen , 
                                file_head.FLen, 
                                file_head.TotalPackage, file_head.CurrPackage, file_head.Width, file_head.Height, file_head.xLeft, file_head.yTop, file_head.Datalen);
#endif
  t_image->Jpegaddr = file_head.Faddr + 256;
  //t_image->Maddr = ((uint32_t)disp_jpeg_bitstream&0xffff)+(((uint32_t)disp_jpeg_bitstream&0xffff0000)>>12)|0x700000;
  t_image->xLeft = file_head.xLeft;
  t_image->yTop = file_head.yTop;
  t_image->Len = file_head.Filelen;
  t_image->FLen = file_head.FLen;
  t_image->width = file_head.Width;
  t_image->height = file_head.Height;

  //t_image->Maddr = Malloc_Jpeg_Memory(t_image->FLen);
  //gm_spi_flash_data_read(t_image->Jpegaddr, t_image->FLen, t_image->Maddr);


#ifdef _PRINTINFO
  gm_spi_flash_data_read(t_image->Faddr + 256, sizeof(struct file_head1), read_spi);
  xtm_printf("11 0x%X, 0x%X, 0x%X, 0x%X\n", read_spi[0], read_spi[1], read_spi[2], read_spi[3]);
#endif

#ifdef _PRINTINFO
  gm_spi_flash_data_read(t_image->Faddr + 0x1500, 256, read_spi);
  xtm_printf("22 0x%X, 0x%X, 0x%X, 0x%X\n", read_spi[0x28], read_spi[0x29], read_spi[0x30], read_spi[0x31]);
#endif
  return 0;
}
//---------------------------------------------------------------------------
extern uint8_t ErrorStopDecode;
uint8_t DisplayImage(struct TImage *t_image, uint8_t FromType, uint8_t FullPageType)
{
  uint16_t ReadNum, i, ret;
  uint8_t DecodeNum;
  uint8_t DecoceErr;
	uint8_t *jpeg_buf, *tmp_data, *tmp_data1;
	JPEGDEC_FRAME_PARAM_T s_dec_frame;	

  /*
  xtm_printf("读 Faddr = 0x%X , Jpegaddr = 0x%X, Filelen = 0x%X, Flen = 0X%X, Width = %d, Height = %d, xLeft = %d, yTop = %d\n", 
                                t_image->Faddr, t_image->Jpegaddr, 
                                t_image->Len, 
                                t_image->FLen, 
                                 t_image->width, t_image->height, t_image->xLeft, t_image->yTop);     
  */    
  if((t_image->width > SCREEN_WIDTH)||(t_image->height > SCREEN_HEIGHT))
  {
    xtm_printf("DisplayImage::width = 0x%X, height = 0x%X\n", t_image->width, t_image->height);
    return 0;
  }
  if((t_image->xLeft > SCREEN_WIDTH)||(t_image->yTop > SCREEN_HEIGHT))
  {
    xtm_printf("DisplayImage::xLeft = 0x%X, yTop = 0x%X\n", t_image->xLeft, t_image->yTop);
    return 0;
  }
  if(t_image->Len > 0x20000)
  {
    xtm_printf("DisplayImage::Len = 0x%X\n", t_image->Len);
    return 0;
  }			
  Local_fb_addr = GetLocal_fb_addr();
  Local_fb_page = GetLocal_fb_page();

  //xtm_printf("DisplayImage::Local_fb_addr[%d] = 0x%X %X\n", 
  //                              !Local_fb_page, (uint16_t)((Local_fb_addr[!Local_fb_page] & 0xffff0000) >> 16), (uint16_t)(Local_fb_addr[!Local_fb_page] & 0xffff));
        
  DecodeNum = 0;
	DecoceErr = 1;
	jpeg_buf = (u32 *)gm_noncache_mem_alloc(ALIGN(t_image->Len, 256), 256);
  do{
    if(FromType == FROMFILE)
    {
      ReadNum = 0;
      while(1)
      {
        gm_spi_flash_data_read(t_image->Jpegaddr, t_image->FLen, jpeg_buf);
        ReadNum ++;

        if((jpeg_buf[0] == 0xff)&&(jpeg_buf[1] == 0xd8)&&(jpeg_buf[t_image->Len - 2] == 0xff)&&(jpeg_buf[t_image->Len - 1] == 0xd9))
        {
          if(ReadNum > 1)
            xtm_printf("DisplayImage:: ReadNum = %d\n", ReadNum);
          break;
        }
        ClearWatchDog();
        if(ReadNum > 10)
        {
          xtm_printf("DisplayImage:: ReadNum = %d\n", ReadNum);
          break;
        }
      }
    }
		
		//if(FromType == FROMMEMORY)
		//	memcpy(jpeg_buf, (u8 *)t_image->Jpegaddr, t_image->Len);

		s_dec_frame.bitstream  = (u8 *)jpeg_buf;  //t_image->Maddr;// 
		s_dec_frame.bs_size  = ALIGN(t_image->Len, 256);
		s_dec_frame.thumbnail_dec = 0;
		s_dec_frame.dst_Yaddr = JPEG_DECODE_BUF0;//JPEG_DECODE_BUF0;	   Local_fb_addr[!Local_fb_page]
		s_dec_frame.dst_Uaddr = JPEG_DECODE_BUF0 + DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX;	  
		s_dec_frame.dst_Vaddr = JPEG_DECODE_BUF0 + DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX*3/2;	
		s_dec_frame.crop_enable   = 0;	
				  
		//printf("2 dst_Yaddr = 0x%X, dst_Uaddr = 0x%X, dst_Vaddr = 0x%X, bitstream = 0x%X, bs_size = %d\n", 
		//			s_dec_frame.dst_Yaddr, s_dec_frame.dst_Uaddr, s_dec_frame.dst_Vaddr, s_dec_frame.bitstream, s_dec_frame.bs_size);
					
		xSemaphoreTake(jpeg_access_mutex, portMAX_DELAY);
							
		if(ret = gm_jpgdec_decframe(Jpeg_Dec_Handle, &s_dec_frame))
    {
      printf("DisplayImage::decode JPEG Fail!!\n");
      DecoceErr = 1;
		}		
			
		//printf("DisplayImage::gm_jpgdec_decframe, ret = %d\n", ret);
		vTaskDelay(1);
		if(ret == 0)
			break;

    DecodeNum ++;
    if(DecodeNum > 10)
      break;

    ClearWatchDog();
  }while(DecoceErr == 1);

  if(DecodeNum > 1)
  {
    xtm_printf("DisplayImage::DecodeNum = %d, DecoceErr = %d\n", DecodeNum, DecoceErr);
    ErrorStopDecode = 1;
  } 

  if(FullPageType == FB_PAGE2)
  {
		Local_fb1_addr = GetLocal_fb1_addr();
		tmp_data1 = (uint8_t *)(Local_fb1_addr);
		memcpy(tmp_data1, JPEG_DECODE_BUF0, (uint32_t)t_image->width*t_image->height*2);			
  }
	else
  {
		if((t_image->width == SCREEN_WIDTH)&&(t_image->xLeft == 0))
    {
			if(!FullPageType)
				tmp_data1 = (uint8_t *)(Local_fb_addr[Local_fb_page] + (((uint32_t) (t_image->yTop)*SCREEN_WIDTH +(uint32_t)t_image->xLeft)<<1)) ;
			else
				tmp_data1 = (uint8_t *)(Local_fb_addr[!Local_fb_page] + (((uint32_t) (t_image->yTop)*SCREEN_WIDTH +(uint32_t)t_image->xLeft)<<1)) ;
			memcpy(tmp_data1, JPEG_DECODE_BUF0, (uint32_t)t_image->width*t_image->height*2);		
	  }
		else
    {
			for(i=0; i<t_image->height; i++)
			{
			  tmp_data = (uint8_t *)(JPEG_DECODE_BUF0 + (uint32_t)(i)*t_image->width*2); 
			  if(!FullPageType)
				tmp_data1 = (uint8_t *)(Local_fb_addr[Local_fb_page] + (((uint32_t) (i + t_image->yTop)*SCREEN_WIDTH +(uint32_t)t_image->xLeft)<<1)) ;
			  else
				tmp_data1 = (uint8_t *)(Local_fb_addr[!Local_fb_page] + (((uint32_t) (i + t_image->yTop)*SCREEN_WIDTH +(uint32_t)t_image->xLeft)<<1)) ;
			  memcpy(tmp_data1, tmp_data, t_image->width*2);
			}
			//printf("t_image->width = %d, t_image->height = %d\n", t_image->width, t_image->height);
    } 
	}
	
  gm_noncache_mem_free(jpeg_buf); 
  return 0;
}
//---------------------------------------------------------------------------
//预处理 ImageButton 文件
uint8_t PreDealImageButton(struct TImageButton *t_button, uint8_t buttontype)
{
#if 1
  uint8_t read_spi[512];
  struct file_head1 file_head;

  #ifdef _PRINTINFO
    xtm_printf("t_button->Faddr[%d] = 0x%X %X\n", \
                                  buttontype, (uint16_t)((t_button->Faddr[buttontype] & 0xffff0000) >> 16), (uint16_t)(t_button->Faddr[buttontype] & 0xffff));
  #endif
  
  gm_spi_flash_data_read(t_button->Faddr[buttontype], sizeof(struct file_head1), read_spi);

  #ifdef _PRINTINFO
    xtm_printf("0x%X, 0x%X, 0x%X, 0x%X\n", read_spi[0], read_spi[1], read_spi[2], read_spi[3]);
  #endif
  
  memcpy(&file_head, read_spi, sizeof(struct file_head1));

  #ifdef _PRINTINFO
    xtm_printf("读 Faddr = 0x%X%X , Filelen = 0x%X%X, Flen = 0x%X%X, TotalPackage = %d, CurrPackage = %d, Width = %d, Height = %d, xLeft = %d, yTop = %d, Datalen = %d\n", \
                                (uint16_t)((file_head.Faddr & 0xffff0000) >> 16), (uint16_t)(file_head.Faddr & 0xffff), \
                                (uint16_t)((file_head.Filelen & 0xffff0000) >> 16), (uint16_t)(file_head.Filelen & 0xffff), \
                                (uint16_t)((file_head.FLen & 0xffff0000) >> 16), (uint16_t)(file_head.FLen & 0xffff), \
                                file_head.TotalPackage, file_head.CurrPackage, file_head.Width, file_head.Height, file_head.xLeft, file_head.yTop, file_head.Datalen);
  #endif
  
  t_button->Jpegaddr[buttontype] = file_head.Faddr + 256;
  t_button->xLeft = file_head.xLeft;
  t_button->yTop = file_head.yTop;
  t_button->Len[buttontype] = file_head.Filelen;
  t_button->FLen[buttontype] = file_head.FLen;
  t_button->width = file_head.Width;
  t_button->height = file_head.Height;

  //t_button->Maddr[buttontype] = Malloc_Jpeg_Memory(t_button->FLen[buttontype]);
  //gm_spi_flash_data_read(t_button->Jpegaddr[buttontype], t_button->FLen[buttontype], t_button->Maddr[buttontype]);  
#endif
  return 0;
}
//---------------------------------------------------------------------------
uint8_t DisplayImageButton(struct TImageButton *t_button, uint8_t buttontype, uint8_t FromType, uint8_t FullPageType)
{
  uint32_t j;
  uint16_t ReadNum, i;
  uint8_t DecodeNum, ret, DecoceErr;
	uint8_t *jpeg_buf, *tmp_data, *tmp_data1;
	JPEGDEC_FRAME_PARAM_T s_dec_frame;	


 	//xtm_printf("读 Faddr = 0x%X , Filelen = 0x%X, Flen = 0x%X, Width = %d, Height = %d, xLeft = %d, yTop = %d\n", 
  //                        t_button->Faddr[buttontype], 
  //                        t_button->Len[buttontype], 
  //                        t_button->FLen[buttontype], 
  //                         t_button->width, t_button->height, t_button->xLeft, t_button->yTop);    

  if((t_button->width > SCREEN_WIDTH)||(t_button->height > SCREEN_HEIGHT))
  {
    xtm_printf("DisplayImageButton::width = 0x%X, height = 0x%X\n", t_button->width, t_button->height);
    return 0;
  }

  if((t_button->xLeft > SCREEN_WIDTH)||(t_button->yTop > SCREEN_HEIGHT))
  {
    xtm_printf("DisplayImageButton::xLeft = 0x%X, yTop = 0x%X\n", t_button->xLeft, t_button->yTop);
    return 0;
  }
		
  if(t_button->Len[buttontype] > 0x20000)
  {
    xtm_printf("DisplayImageButton::Len[%d] = 0x%X\n", buttontype, t_button->Len[buttontype]);
    return 0;
  }			
  Local_fb_addr = GetLocal_fb_addr();
  Local_fb_page = GetLocal_fb_page();

  DecodeNum = 0;
	DecoceErr = 1;
	jpeg_buf = (u32 *)gm_noncache_mem_alloc(ALIGN(t_button->Len[buttontype], 256), 256);
	
  do{
    if(FromType == FROMFILE)
    {
      #if 1
        ReadNum = 0;
        while(1)
        {
          gm_spi_flash_data_read(t_button->Jpegaddr[buttontype], t_button->FLen[buttontype], jpeg_buf);
          ReadNum ++;

          if((jpeg_buf[0] == 0xff)&&(jpeg_buf[1] == 0xd8)&&(jpeg_buf[t_button->Len[buttontype] - 2] == 0xff)&&(jpeg_buf[t_button->Len[buttontype] - 1] == 0xd9))
          {
            if(ReadNum > 1)
              xtm_printf("DisplayImageButton:: ReadNum = %d\n", ReadNum);
            break;
          }
          
          if(ReadNum > 10)
          {
            xtm_printf("DisplayImageButton:: ReadNum = %d\n", ReadNum);
            break;
          }
        }
      #endif
    }       

		s_dec_frame.bitstream  = (u8 *)jpeg_buf;   //t_button->Maddr[buttontype];/
		s_dec_frame.bs_size  = ALIGN(t_button->Len[buttontype], 256);
		s_dec_frame.thumbnail_dec = 0;
		s_dec_frame.dst_Yaddr = JPEG_DECODE_BUF0;//JPEG_DECODE_BUF0;	   Local_fb_addr[!Local_fb_page]
		s_dec_frame.dst_Uaddr = JPEG_DECODE_BUF0 + DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX;	  
		s_dec_frame.dst_Vaddr = JPEG_DECODE_BUF0 + DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX*3/2;	
		s_dec_frame.crop_enable   = 0;	
		//printf("s_dec_frame.bs_size = %d\n", s_dec_frame.bs_size);

						  
		//printf("2 dst_Yaddr = 0x%X, dst_Uaddr = 0x%X, dst_Vaddr = 0x%X, bitstream = 0x%X, bs_size = %d\n", 
		//			s_dec_frame.dst_Yaddr, s_dec_frame.dst_Uaddr, s_dec_frame.dst_Vaddr, s_dec_frame.bitstream, s_dec_frame.bs_size);
							
		xSemaphoreTake(jpeg_access_mutex, portMAX_DELAY);
									
		if(ret = gm_jpgdec_decframe(Jpeg_Dec_Handle, &s_dec_frame))
    {
			printf("DisplayImageButton::decode JPEG Fail!!\n");
			DecoceErr = 1;
		}
					
		//printf("DisplayImageButton::gm_jpgdec_decframe, ret = %d\n", ret);
		vTaskDelay(1);
		if(ret == 0)
			break;
				
    DecodeNum ++;
    if(DecodeNum > 10)
      break;
  
    ClearWatchDog();
  }while(DecoceErr == 1);

  if(DecodeNum > 1)
  {
    xtm_printf("DisplayImageButton::DecodeNum = %d\n", DecodeNum);
    ErrorStopDecode = 1;
  }

	for(i=0; i<t_button->height; i++)
	{
	  tmp_data = (uint8_t *)(JPEG_DECODE_BUF0 + (uint32_t)(i)*t_button->width*2);	
	  //tmp_data = (uint8_t *)ALIGN((JPEG_DECODE_BUF0 + (uint32_t)(i)*t_button->width*2), 256);
	
    if(!FullPageType)
	    tmp_data1 = (uint8_t *)(Local_fb_addr[Local_fb_page] + (((uint32_t) (i + t_button->yTop)*SCREEN_WIDTH +(uint32_t)t_button->xLeft)<<1)) ;
	  else
		  tmp_data1 = (uint8_t *)(Local_fb_addr[!Local_fb_page] + (((uint32_t) (i + t_button->yTop)*SCREEN_WIDTH +(uint32_t)t_button->xLeft)<<1)) ;
	  
    memcpy(tmp_data1, tmp_data, t_button->width*2);
	}

  gm_noncache_mem_free(jpeg_buf);
  return 0;  
}
//---------------------------------------------------------------------------
//显示图形按键文本
void DisplayImageButton_Text(struct TImageButton *t_button, uint16_t color, uint8_t wd, uint8_t FullPageType)
{
  if(t_button->FontType == 24)
    display_text(t_button->xLeft + t_button->Text_xLeft, t_button->yTop + t_button->Text_yTop, wd, color, t_button->buttonname, 0, 24, FullPageType);
  else
    display_text(t_button->xLeft + t_button->Text_xLeft, t_button->yTop + t_button->Text_yTop, wd, color, t_button->buttonname, 0, 16, FullPageType);
}
//---------------------------------------------------------------------------
//预处理 Edit 文件
uint8_t PreDealEdit(struct TEdit *t_edit, uint8_t buttontype)
{
  uint8_t read_spi[512];
  struct file_head1 file_head;

#ifdef _PRINTINFO
  xtm_printf("t_edit->Faddr[%d] = 0x%X %X\n", \
                                  buttontype, (uint16_t)((t_edit->Faddr[buttontype] & 0xffff0000) >> 16), (uint16_t)(t_edit->Faddr[buttontype] & 0xffff));
#endif

  gm_spi_flash_data_read(t_edit->Faddr[buttontype], sizeof(struct file_head1), read_spi);

#ifdef _PRINTINFO
  xtm_printf("0x%X, 0x%X, 0x%X, 0x%X\n", read_spi[0], read_spi[1], read_spi[2], read_spi[3]);
#endif

  memcpy(&file_head, read_spi, sizeof(struct file_head1));

#ifdef _PRINTINFO
  xtm_printf("读 Faddr = 0x%X%X , Filelen = 0x%X%X, Flen = 0x%X%X, TotalPackage = %d, CurrPackage = %d, Width = %d, Height = %d, xLeft = %d, yTop = %d, Datalen = %d\n", \
                                (uint16_t)((file_head.Faddr & 0xffff0000) >> 16), (uint16_t)(file_head.Faddr & 0xffff), \
                                (uint16_t)((file_head.Filelen & 0xffff0000) >> 16), (uint16_t)(file_head.Filelen & 0xffff), \
                                (uint16_t)((file_head.FLen & 0xffff0000) >> 16), (uint16_t)(file_head.FLen & 0xffff), \
                                file_head.TotalPackage, file_head.CurrPackage, file_head.Width, file_head.Height, file_head.xLeft, file_head.yTop, file_head.Datalen);
#endif

  t_edit->Jpegaddr[buttontype] = file_head.Faddr + 256;
  t_edit->xLeft = file_head.xLeft;
  t_edit->yTop = file_head.yTop;
  t_edit->Len[buttontype] = file_head.Filelen;
  t_edit->FLen[buttontype] = file_head.FLen;
  t_edit->width = file_head.Width;
  t_edit->height = file_head.Height;

  return 0;
}
//---------------------------------------------------------------------------
uint8_t DisplayEdit(struct TEdit *t_edit, uint8_t buttontype, uint8_t FromType, uint8_t FullPageType)
{
  uint16_t ReadNum, i;
	uint8_t DecodeNum, ret, DecoceErr;
	uint8_t *jpeg_buf, *tmp_data, *tmp_data1;
	JPEGDEC_FRAME_PARAM_T s_dec_frame;	       

  if((t_edit->width > SCREEN_WIDTH)||(t_edit->height > SCREEN_HEIGHT))
  {
    xtm_printf("DisplayEdit::width = 0x%X, height = 0x%X\n", t_edit->width, t_edit->height);
    return 0;
  }
  
  if((t_edit->xLeft > SCREEN_WIDTH)||(t_edit->yTop > SCREEN_HEIGHT))
  {
    xtm_printf("DisplayEdit::xLeft = 0x%X, yTop = 0x%X\n", t_edit->xLeft, t_edit->yTop);
    return 0;
  }
	
  if(t_edit->Len[buttontype] > 0x20000)
  {
    xtm_printf("DisplayEdit::Len[%d] = 0x%X\n", buttontype, t_edit->Len[buttontype]);
    return 0;
  }		

  Local_fb_addr = GetLocal_fb_addr();
  Local_fb_page = GetLocal_fb_page();
  
  DecodeNum = 0;      
	DecoceErr = 1;
	jpeg_buf = (u32 *)gm_noncache_mem_alloc(ALIGN(t_edit->Len[buttontype], 256), 256);	
  
  do{
    if(FromType == FROMFILE)
    {
      ReadNum = 0;
      while(1)
      {
        gm_spi_flash_data_read(t_edit->Jpegaddr[buttontype], t_edit->FLen[buttontype], jpeg_buf);
        ReadNum ++;
        if((jpeg_buf[t_edit->Len[buttontype] - 2] == 0xff)&&(jpeg_buf[t_edit->Len[buttontype] - 1] == 0xd9))
        {
          if(ReadNum > 1)
            xtm_printf("DisplayEdit:: ReadNum = %d\n", ReadNum);
          break;
        }
        if(ReadNum > 10)
        {
          xtm_printf("DisplayEdit:: ReadNum = %d\n", ReadNum);
          break;
        }
      }
    }

		s_dec_frame.bitstream  = (u8 *)jpeg_buf;   
		s_dec_frame.bs_size  = ALIGN(t_edit->Len[buttontype], 256);
		s_dec_frame.thumbnail_dec = 0;
		s_dec_frame.dst_Yaddr = JPEG_DECODE_BUF0;//JPEG_DECODE_BUF0;	   Local_fb_addr[!Local_fb_page]
		s_dec_frame.dst_Uaddr = JPEG_DECODE_BUF0 + DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX;	  
		s_dec_frame.dst_Vaddr = JPEG_DECODE_BUF0 + DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX*3/2;	
		s_dec_frame.crop_enable   = 0;	

		xSemaphoreTake(jpeg_access_mutex, portMAX_DELAY);
									
		if(ret = gm_jpgdec_decframe(Jpeg_Dec_Handle, &s_dec_frame))
    {
			printf("DisplayEdit::decode JPEG Fail!!\n");
			DecoceErr = 1;
		}			
	
		//printf("DisplayEdit::gm_jpgdec_decframe, ret = %d\n", ret);
		vTaskDelay(1);
		if(ret == 0)
			break;
				
    DecodeNum ++;
    if(DecodeNum > 10)
      break;
    
    ClearWatchDog();

  }while(DecoceErr == 1);

	for(i=0; i<t_edit->height; i++)
	{
		tmp_data = (uint8_t *)(JPEG_DECODE_BUF0 + (uint32_t)(i)*t_edit->width*2);	
		if(!FullPageType)
			tmp_data1 = (uint8_t *)(Local_fb_addr[Local_fb_page] + (((uint32_t) (i + t_edit->yTop)*SCREEN_WIDTH +(uint32_t)t_edit->xLeft)<<1)) ;
		else
			tmp_data1 = (uint8_t *)(Local_fb_addr[!Local_fb_page] + (((uint32_t) (i + t_edit->yTop)*SCREEN_WIDTH +(uint32_t)t_edit->xLeft)<<1)) ;
		
    memcpy(tmp_data1, tmp_data, t_edit->width*2);
	}		

  gm_noncache_mem_free(jpeg_buf);
	return 0;
}
//---------------------------------------------------------------------------
void DisplayEdit_Text(struct TEdit *t_edit, uint16_t color, uint8_t wd, uint8_t FullPageType)
{
  if(t_edit->FontType == 24)
    display_text(t_edit->xLeft + t_edit->Text_xLeft, t_edit->yTop + t_edit->Text_yTop, wd, color, t_edit->Text, 0, 24, FullPageType);
  else
    display_text(t_edit->xLeft + t_edit->Text_xLeft, t_edit->yTop + t_edit->Text_yTop, wd, color, t_edit->Text, 0, 16, FullPageType); 
}
//---------------------------------------------------------------------------
void DisplayPassEdit_Text(struct TEdit *t_edit, uint16_t color, uint8_t wd, uint8_t FullPageType)
{

  uint8_t text_len, i;
  char str[20];
  text_len = strlen(t_edit->Text);
  for(i=0; i<text_len; i++)
    str[i] = '*';
  str[text_len] = '\0';
  if(t_edit->FontType == 24)
    display_text(t_edit->xLeft + t_edit->Text_xLeft, t_edit->yTop + t_edit->Text_yTop, wd, color, str, 0, 24, FullPageType);
  else
    display_text(t_edit->xLeft + t_edit->Text_xLeft, t_edit->yTop + t_edit->Text_yTop, wd, color, str, 0, 16, FullPageType); 
}
//---------------------------------------------------------------------------
void DecoceVideo(uint8_t *Video_Maddr, uint32_t Video_Len)
{
	uint16_t i, ret;
	JPEGDEC_FRAME_PARAM_T s_dec_frame;	

	//printf("DecoceVideo::1, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", 
	//	Video_Maddr[0], Video_Maddr[1], Video_Maddr[2], Video_Maddr[3], Video_Maddr[Video_Len - 2], Video_Maddr[Video_Len - 1]);

	s_dec_frame.bitstream  = (u8 *)Video_Maddr;   
	s_dec_frame.bs_size  = ALIGN(Video_Len, 256);          	
	s_dec_frame.thumbnail_dec = 0;
	s_dec_frame.dst_Yaddr = GUI_BUF2;//JPEG_DECODE_BUF0;	   Local_fb_addr[!Local_fb_page]
	s_dec_frame.dst_Uaddr = GUI_BUF2 + DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX;	  
	s_dec_frame.dst_Vaddr = GUI_BUF2 + DISPLAY_SRC_WIDTH_MAX*DISPLAY_SRC_HEIGHT_MAX*3/2;	
	s_dec_frame.crop_enable   = 0;				  
				
	xSemaphoreTake(jpeg_access_mutex, portMAX_DELAY);
						
	if(ret = gm_jpgdec_decframe(Jpeg_Dec_Handle, &s_dec_frame)){
				 printf("decode JPEG Fail!!\n");
	}			
		
	//printf("DecoceVideo::gm_jpgdec_decframe, ret = %d\n", ret);
}


