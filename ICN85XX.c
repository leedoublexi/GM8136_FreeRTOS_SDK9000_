



#define CommonH
#include "common.h"

#include "lib_i2c.h"
#include "ICN85XX.h"

#define ICN85XX_TOUCH_IO         17          //触摸屏GPIO0 17
#define ICN85XX_SCL_IO           18          //
#define ICN85XX_SDA_IO           19          //  

#define ICN85XX_PROG_IIC_ADDR_W  0x60
#define ICN85XX_PROG_IIC_ADDR_R  0x61
#define ICN85XX_PROG_IIC_ADDR    0x60 >> 1


#define ICN85XX_IIC_ADDR_W       0x90        //写数据命令
#define ICN85XX_IIC_ADDR_R       0x91        //读数据命令
#define ICN85XX_IIC_ADDR         0x90 >> 1


#define FLASH_TOTAL_SIZE    0x00010000UL
#define FLASH_PAGE_SIZE     0x1000
#define FLASH_AHB_BASE_ADDR             0x00100000UL
#define FLASH_PATCH_PARA_BASE_ADDR      (FLASH_TOTAL_SIZE - FLASH_PAGE_SIZE)         //  allocate 1 page for patch para, 0xff00
#define FLASH_CODE_INFO_BASE_ADDR       (FLASH_PATCH_PARA_BASE_ADDR - FLASH_PAGE_SIZE)           //  0xfe00,allocate 1 page for system para
#define FLASH_CRC_ADDR      (FLASH_AHB_BASE_ADDR + FLASH_CODE_INFO_BASE_ADDR + 0x00) //  0xfe00
#define FLASH_CODE_LENGTH_ADDR          (FLASH_AHB_BASE_ADDR + FLASH_CODE_INFO_BASE_ADDR + 0x04) //  0xfe04


int      icn85xx_goto_progmode(void);
uint32_t icn85xx_crc_calc(uint32_t crc_in, uint8_t *buf, uint8_t len);
int      icn85xx_crc_enable(unsigned char enable);
int      icn85xx_crc_check(uint32_t crc, uint16_t len);
int      icn85xx_fw_download(uint16_t offset, uint8_t * buffer, uint8_t size);
void     FlashWriteEnable(void);
void     ClearFlashState(void);
void     EarseFlash(uint8_t erase_index, uint32_t flash_addr);
int      icn85xx_erase_flash(void);
int      icn85xx_prog_buffer(uint32_t flash_addr, uint32_t sram_addr, uint32_t copy_length, uint8_t program_type);
int      icn85xx_prog_data(uint32_t flash_addr, uint32_t data);
int      icn85xx_bootfrom_sram(void);
void     icn85xx_read_flash(uint32_t sram_address, uint32_t flash_address, uint32_t copy_length, uint8_t i2c_wire_num);
uint16_t icn85xx_readVersion(void);
uint16_t icn85xx_readEventId(void);
uint16_t icn85xx_readPointX(void);
uint16_t icn85xx_readPointY(void);
uint16_t icn85xx_readTouchDownThresold(void);
int      icn85xx_fw_update(void);

uint8_t ICN85XX_Init(void);



/***********************************************************************************************
Name    :   icn85xx_goto_progmode
Input   :
Output  :
function    :   change MCU to progmod
***********************************************************************************************/
int icn85xx_goto_progmode(void)
{
  int ret = -1;
  int retry = 3;
  uint8_t ucTemp[10];
    
	//flash_info("icn85xx_goto_progmode\n"); 
  while(retry > 0)
  {
    // icn85xx_set_prog_addr();
    ucTemp[0] = 0x5a;

    ret = gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0xCC3355, ucTemp, 1);
    if( ret < 0)
    {
      retry -- ;
      vTaskDelay(2);
    } else
      break;           
  }
  
  xtm_printf("icn85xx_goto_progmode over, ret: %d\n", ret);
  if(retry == 0)
    return -1;

  return 0;
}


#if 1
uint32_t crc32table[256] = {
 0x00000000UL, 0x04c11db7UL, 0x09823b6eUL, 0x0d4326d9UL, 0x130476dcUL, 0x17c56b6bUL,
 0x1a864db2UL, 0x1e475005UL, 0x2608edb8UL, 0x22c9f00fUL, 0x2f8ad6d6UL, 0x2b4bcb61UL,
 0x350c9b64UL, 0x31cd86d3UL, 0x3c8ea00aUL, 0x384fbdbdUL, 0x4c11db70UL, 0x48d0c6c7UL,
 0x4593e01eUL, 0x4152fda9UL, 0x5f15adacUL, 0x5bd4b01bUL, 0x569796c2UL, 0x52568b75UL,
 0x6a1936c8UL, 0x6ed82b7fUL, 0x639b0da6UL, 0x675a1011UL, 0x791d4014UL, 0x7ddc5da3UL,
 0x709f7b7aUL, 0x745e66cdUL, 0x9823b6e0UL, 0x9ce2ab57UL, 0x91a18d8eUL, 0x95609039UL,    
 0x8b27c03cUL, 0x8fe6dd8bUL, 0x82a5fb52UL, 0x8664e6e5UL, 0xbe2b5b58UL, 0xbaea46efUL,    
 0xb7a96036UL, 0xb3687d81UL, 0xad2f2d84UL, 0xa9ee3033UL, 0xa4ad16eaUL, 0xa06c0b5dUL,
 0xd4326d90UL, 0xd0f37027UL, 0xddb056feUL, 0xd9714b49UL, 0xc7361b4cUL, 0xc3f706fbUL,
 0xceb42022UL, 0xca753d95UL, 0xf23a8028UL, 0xf6fb9d9fUL, 0xfbb8bb46UL, 0xff79a6f1UL,
 0xe13ef6f4UL, 0xe5ffeb43UL, 0xe8bccd9aUL, 0xec7dd02dUL, 0x34867077UL, 0x30476dc0UL,
 0x3d044b19UL, 0x39c556aeUL, 0x278206abUL, 0x23431b1cUL, 0x2e003dc5UL, 0x2ac12072UL,
 0x128e9dcfUL, 0x164f8078UL, 0x1b0ca6a1UL, 0x1fcdbb16UL, 0x018aeb13UL, 0x054bf6a4UL,
 0x0808d07dUL, 0x0cc9cdcaUL, 0x7897ab07UL, 0x7c56b6b0UL, 0x71159069UL, 0x75d48ddeUL,
 0x6b93dddbUL, 0x6f52c06cUL, 0x6211e6b5UL, 0x66d0fb02UL, 0x5e9f46bfUL, 0x5a5e5b08UL,
 0x571d7dd1UL, 0x53dc6066UL, 0x4d9b3063UL, 0x495a2dd4UL, 0x44190b0dUL, 0x40d816baUL,
 0xaca5c697UL, 0xa864db20UL, 0xa527fdf9UL, 0xa1e6e04eUL, 0xbfa1b04bUL, 0xbb60adfcUL,
 0xb6238b25UL, 0xb2e29692UL, 0x8aad2b2fUL, 0x8e6c3698UL, 0x832f1041UL, 0x87ee0df6UL,
 0x99a95df3UL, 0x9d684044UL, 0x902b669dUL, 0x94ea7b2aUL, 0xe0b41de7UL, 0xe4750050UL,
 0xe9362689UL, 0xedf73b3eUL, 0xf3b06b3bUL, 0xf771768cUL, 0xfa325055UL, 0xfef34de2UL,
 0xc6bcf05fUL, 0xc27dede8UL, 0xcf3ecb31UL, 0xcbffd686UL, 0xd5b88683UL, 0xd1799b34UL,
 0xdc3abdedUL, 0xd8fba05aUL, 0x690ce0eeUL, 0x6dcdfd59UL, 0x608edb80UL, 0x644fc637UL,
 0x7a089632UL, 0x7ec98b85UL, 0x738aad5cUL, 0x774bb0ebUL, 0x4f040d56UL, 0x4bc510e1UL,
 0x46863638UL, 0x42472b8fUL, 0x5c007b8aUL, 0x58c1663dUL, 0x558240e4UL, 0x51435d53UL,
 0x251d3b9eUL, 0x21dc2629UL, 0x2c9f00f0UL, 0x285e1d47UL, 0x36194d42UL, 0x32d850f5UL,
 0x3f9b762cUL, 0x3b5a6b9bUL, 0x0315d626UL, 0x07d4cb91UL, 0x0a97ed48UL, 0x0e56f0ffUL,
 0x1011a0faUL, 0x14d0bd4dUL, 0x19939b94UL, 0x1d528623UL, 0xf12f560eUL, 0xf5ee4bb9UL,
 0xf8ad6d60UL, 0xfc6c70d7UL, 0xe22b20d2UL, 0xe6ea3d65UL, 0xeba91bbcUL, 0xef68060bUL,
 0xd727bbb6UL, 0xd3e6a601UL, 0xdea580d8UL, 0xda649d6fUL, 0xc423cd6aUL, 0xc0e2d0ddUL,
 0xcda1f604UL, 0xc960ebb3UL, 0xbd3e8d7eUL, 0xb9ff90c9UL, 0xb4bcb610UL, 0xb07daba7UL,
 0xae3afba2UL, 0xaafbe615UL, 0xa7b8c0ccUL, 0xa379dd7bUL, 0x9b3660c6UL, 0x9ff77d71UL,
 0x92b45ba8UL, 0x9675461fUL, 0x8832161aUL, 0x8cf30badUL, 0x81b02d74UL, 0x857130c3UL,
 0x5d8a9099UL, 0x594b8d2eUL, 0x5408abf7UL, 0x50c9b640UL, 0x4e8ee645UL, 0x4a4ffbf2UL,
 0x470cdd2bUL, 0x43cdc09cUL, 0x7b827d21UL, 0x7f436096UL, 0x7200464fUL, 0x76c15bf8UL,
 0x68860bfdUL, 0x6c47164aUL, 0x61043093UL, 0x65c52d24UL, 0x119b4be9UL, 0x155a565eUL,
 0x18197087UL, 0x1cd86d30UL, 0x029f3d35UL, 0x065e2082UL, 0x0b1d065bUL, 0x0fdc1becUL,
 0x3793a651UL, 0x3352bbe6UL, 0x3e119d3fUL, 0x3ad08088UL, 0x2497d08dUL, 0x2056cd3aUL,
 0x2d15ebe3UL, 0x29d4f654UL, 0xc5a92679UL, 0xc1683bceUL, 0xcc2b1d17UL, 0xc8ea00a0UL,
 0xd6ad50a5UL, 0xd26c4d12UL, 0xdf2f6bcbUL, 0xdbee767cUL, 0xe3a1cbc1UL, 0xe760d676UL,
 0xea23f0afUL, 0xeee2ed18UL, 0xf0a5bd1dUL, 0xf464a0aaUL, 0xf9278673UL, 0xfde69bc4UL,
 0x89b8fd09UL, 0x8d79e0beUL, 0x803ac667UL, 0x84fbdbd0UL, 0x9abc8bd5UL, 0x9e7d9662UL,
 0x933eb0bbUL, 0x97ffad0cUL, 0xafb010b1UL, 0xab710d06UL, 0xa6322bdfUL, 0xa2f33668UL,
 0xbcb4666dUL, 0xb8757bdaUL, 0xb5365d03UL, 0xb1f740b4UL
};
#endif
uint32_t icn85xx_crc_calc(uint32_t crc_in, uint8_t *buf, uint8_t len)
{
#if 1
  uint8_t i;
  uint32_t crc = crc_in;
  for(i = 0; i < len; i++)
  {
    crc = (crc << 8) ^ crc32table[((crc >> 24) ^ *buf++) & 0xFF];
  }
  return crc;
#endif
//return 0;
}

/***********************************************************************************************
Name    :   icn85xx_crc_enable
Input   :   
Output  :   
function    :control crc control
***********************************************************************************************/
int  icn85xx_crc_enable(unsigned char enable)
{
  uint8_t ucTemp[10];
  int ret = 0;

  if(enable==1)
  {
    ucTemp[0] = 1;
    ret = gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040028, ucTemp, 1);
  } 
  else if(enable==0) 
  {
    ucTemp[0] = 0;
    ret = gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040028, ucTemp, 1);
  } 
  return ret;
}


/***********************************************************************************************
Name    :   icn85xx_crc_check
Input   :   
Output  :   
function    :chec crc right or not
***********************************************************************************************/
int  icn85xx_crc_check(uint32_t crc, uint16_t len)
{
  int ret;
  uint16_t crc_len;
  uint32_t crc_result;    
  uint8_t  ucTemp[4];
  uint8_t  Order[10];
  uint8_t  i;

  for(i=0; i<4; i++)
  {
    ucTemp[i] = 0;
  }

  ret= gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0x04002C, ucTemp, 4);
  crc_result = (uint32_t)ucTemp[3]<<24 | (uint32_t)ucTemp[2]<<16 | (uint32_t)ucTemp[1] << 8 | (uint32_t)ucTemp[0];
  xtm_printf("icn85xx_crc_check::ucTemp 1 = 0x%X, 0x%X, 0x%X, 0x%X\n", ucTemp[0], ucTemp[1], ucTemp[2], ucTemp[3]);
  //flash_info("crc_result: 0x%x\n", crc_result);

  ret= gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0x040034, ucTemp, 2);
  crc_len = ucTemp[1] << 8 | ucTemp[0];
  xtm_printf("icn85xx_crc_check::ucTemp 2 = 0x%X, 0x%X\n", ucTemp[0], ucTemp[1]);
  xtm_printf("crc_len: 0x%X\n", crc_len);

  if((crc_result == crc) && (crc_len == len))
  {
    return 0;
  }
  else
  {
    xtm_printf("crc_fw: 0x%x, 0x%x, 0x%x, 0x%x\n", (uint8_t)((crc & 0xff000000) >> 24), 
    	          (uint8_t)((crc & 0xff0000) >> 16), 
    	          (uint8_t)((crc & 0xff00) >> 8), 
    	          (uint8_t)((crc & 0xff)));
    xtm_printf("crc_result: 0x%x, 0x%x, 0x%x, 0x%x\n", (uint8_t)((crc_result & 0xff000000) >> 24), 
    	          (uint8_t)((crc_result & 0xff0000) >> 16), 
    	          (uint8_t)((crc_result & 0xff00) >> 8), 
    	          (uint8_t)((crc_result & 0xff)));
    xtm_printf("crc_len: 0x%X\n", crc_len);
    return -1;
  }
}

/***********************************************************************************************
Name    :   icn85xx_fw_download
Input   :   
Output  :   
function    :   download code to sram
***********************************************************************************************/
int  icn85xx_fw_download(uint16_t offset, uint8_t * buffer, uint8_t size)
{
  uint8_t Order = 0;

  #ifdef ENABLE_BYTE_CHECK  
    int i = 0;
    uint8_t testb[B_SIZE];
  #endif

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, offset, buffer, size);

  #ifdef ENABLE_BYTE_CHECK  
    gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, offset, testb, size);
    for(i = 0; i < size; i++)
    {
      if(buffer[i] != testb[i])
      {
        xtm_printf("buffer[%d]:%x  testb[%d]:%x\n",i,buffer[i],i,testb[i]);
        return -1;
      }  
    }
  #endif 
  return 0;
}

uint8_t FlashState(uint8_t State_Index)
{
  // uint8_t Order[10];
  uint8_t ucTemp[4] = {0,0,0,0};

  ucTemp[2]=0x08;
  ucTemp[1]=0x10;
  ucTemp[0]=0x00;

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04062C, ucTemp, 3);

  if(State_Index==0)
  {
    ucTemp[0]=0x05;
  } 
  else if(State_Index==1) 
  {
    ucTemp[0]=0x35;
  }
  
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040630, ucTemp, 1);

  ucTemp[1]=0x00;
  ucTemp[0]=0x01;

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040640, ucTemp, 2);

  ucTemp[0]=1;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 2);

  while(ucTemp[0])
  {
    gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);
  }

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040648, ucTemp, 1);
  return (uint8_t)(ucTemp[0]);
}


void FlashWriteEnable(void)
{
  // uint8_t Order[10];
  uint8_t ucTemp[4] = {0,0,0,0};

  ucTemp[2]=0x00;
  ucTemp[1]=0x10;
  ucTemp[0]=0x00;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04062C, ucTemp, 3);

  ucTemp[0]=0x06;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040630, ucTemp, 1);

  ucTemp[0]=0x00;
  ucTemp[1]=0x00;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040640, ucTemp, 2);  

  ucTemp[0]=1;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);

  while(ucTemp[0])
  {
    gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);
  }
    
  ucTemp[0]=FlashState(0);
  while( (ucTemp[0]&0x02)!=0x02)
  {
    ucTemp[0]=FlashState(0);    
  }
}


void ClearFlashState(void)
{
  uint8_t ucTemp[4] = {0,0,0,0};

  gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0x040603, ucTemp, 1);

  ucTemp[0]=(ucTemp[0]|0x20);
  gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0x040603, ucTemp, 1);

  FlashWriteEnable();   
  ////////////////////////////write comd to flash
  ucTemp[2]=0x00;
  ucTemp[1]=0x10;
  ucTemp[0]=0x10;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04062C, ucTemp, 3);  

  ucTemp[0]=0x01;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040630, ucTemp, 1);

  ucTemp[0]=0x00;
  ucTemp[1]=0x00;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040640, ucTemp, 2);

  ucTemp[0]=0x00;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040638, ucTemp, 1);

  ucTemp[0]=1;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);
  while(ucTemp[0])
  {
    gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);
  }
  while(FlashState(0)&0x01);
}


void EarseFlash(uint8_t erase_index, uint32_t flash_addr)
{
  uint8_t Order[10];
  uint8_t ucTemp[4] = {0,0,0,0};
  
  FlashWriteEnable();    
  
  if(erase_index==0)            //erase the chip
  {
    ucTemp[0]=0xc7;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040630, ucTemp, 1);

    ucTemp[2]=0x00;
    ucTemp[1]=0x10;
    ucTemp[0]=0x00;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04062C, ucTemp, 3);
  } else if(erase_index==1)       //erase 32k space of the flash
  {
    ucTemp[0]=0x52;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0X040630, ucTemp, 1);

    ucTemp[2]=0x00;
    ucTemp[1]=0x13;
    ucTemp[0]=0x00;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04062C, ucTemp, 3);
  } else if(erase_index==2)     //erase 64k space of the flash
  {
    ucTemp[0]=0xd8;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040630, ucTemp, 1);

    ucTemp[2]=0x00;
    ucTemp[1]=0x13;
    ucTemp[0]=0x00;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04062C, ucTemp, 3);
  } else if(erase_index==3)
  {
    ucTemp[0]=0x20;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040630, ucTemp, 1);
    ucTemp[2]=0x00;
    ucTemp[1]=0x13;
    ucTemp[0]=0x00;       
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04062C, ucTemp, 3);
  }
  
  ucTemp[2]=(unsigned char)(flash_addr>>16);
  ucTemp[1]=(unsigned char)(flash_addr>>8);
  ucTemp[0]=(unsigned char)(flash_addr);
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040634, ucTemp, 3);

  ucTemp[1]=0x00;
  ucTemp[0]=0x00;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040640, ucTemp, 3);

  ucTemp[0]=1;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);

  while(ucTemp[0])
  {
    gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);
  }           
}


int  icn85xx_erase_flash(void)
{
  int i;
  ClearFlashState(); 

  while(FlashState(0)&0x01);
  FlashWriteEnable();
  
  EarseFlash(1,0);            
  while((FlashState(0)&0x01));  

  for(i=0; i<7; i++)
  {
    FlashWriteEnable();
    EarseFlash(3,0x8000+i*0x1000);            
    while((FlashState(0)&0x01)); 
  }
  
  for(i=0; i<4; i++)
  {
    FlashWriteEnable();
    EarseFlash(3,0x10000+i*0x1000);            
    while((FlashState(0)&0x01)); 
  }    
  return 0;
}


/***********************************************************************************************
Name    :   icn85xx_prog_buffer
Input   :   
Output  :   
function    :   progm flash
***********************************************************************************************/
int  icn85xx_prog_buffer(uint32_t flash_addr, uint32_t sram_addr, uint32_t copy_length, uint8_t program_type)
{
  uint8_t Order[10]; 
  uint8_t ucTemp[4] = {0,0,0,0};
  uint8_t prog_state=0;

  uint32_t i=0;
  uint8_t program_commond=0;
  
  if(program_type == 0)
  {
    program_commond = 0x02;
  } else if(program_type == 1)
  {
    program_commond = 0xf2;
  } else
  {
    program_commond = 0x02;
  }
       
  for(i=0; i<copy_length; )
  {
    xtm_printf("prog_buffer::i = 0x%X\n", i);
    prog_state=(FlashState(0)&0x01);
    while(prog_state)
    {
      prog_state=(FlashState(0)&0x01);
    }
    FlashWriteEnable();           

    ucTemp[2]=0;
    ucTemp[1]=0x13;
    ucTemp[0]=0;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04062C, ucTemp, 3);

    ucTemp[2]=(uint8_t)(flash_addr>>16);
    ucTemp[1]=(uint8_t)(flash_addr>>8);
    ucTemp[0]=(uint8_t)(flash_addr);

    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040634, ucTemp, 3);    

    ucTemp[2]=(uint8_t)(sram_addr>>16);
    ucTemp[1]=(uint8_t)(sram_addr>>8);
    ucTemp[0]=(uint8_t)(sram_addr);

    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04063C, ucTemp, 3);

    if((i+256) <= copy_length)
    {
      ucTemp[1]=0x01;
      ucTemp[0]=0x00;
    } else {
      ucTemp[1]=(uint8_t)((copy_length-i)>>8);
      ucTemp[0]=(uint8_t)(copy_length-i);
    }
    
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0X040640, ucTemp, 2);

    ucTemp[0]=program_commond;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040630, ucTemp, 1);

    ucTemp[0]=0x01;
    gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);

    flash_addr+=256;
    sram_addr+=256;
    i+=256;
    while(ucTemp[0])
    {
      gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040644, ucTemp, 1);
    }
  }     
  
  prog_state=(FlashState(0)&0x01);
  while(prog_state)
  {
    prog_state=(FlashState(0)&0x01);
  }
  return 0;   
}


/***********************************************************************************************
Name    :   icn85xx_prog_data
Input   :   
Output  :   
function    :   write int data to flash
***********************************************************************************************/
int  icn85xx_prog_data(uint32_t flash_addr, uint32_t data)
{
  uint8_t Order[10];
  uint8_t ucTemp[4] = {0,0,0,0};
    
  ucTemp[3]=(uint8_t)(data>>24);
  ucTemp[2]=(uint8_t)(data>>16);
  ucTemp[1]=(uint8_t)(data>>8);
  ucTemp[0]=(uint8_t)(data);

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x02A000, ucTemp, 4);
  icn85xx_prog_buffer(flash_addr , 0x02A000, 0x04,  0);
  return 0;
}


/***********************************************************************************************
Name    :   icn85xx_bootfrom_sram
Input   :   
Output  :   
function    :
***********************************************************************************************/
int  icn85xx_bootfrom_sram(void)
{
  // uint8_t Order[10];
  uint8_t ucTemp[4];
  int ret = -1;

  ucTemp[0] = 0x03;
  xtm_printf("icn85xx_bootfrom_sram\n");

  ret = gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040400, ucTemp, 1);//change bootmode from sram
  return ret;
}


/***********************************************************************************************
Name    :   icn85xx_read_flash
Input   :   
Output  :   
function    :   read data from flash to sram
***********************************************************************************************/
void  icn85xx_read_flash(uint32_t sram_address, uint32_t flash_address, uint32_t copy_length, uint8_t i2c_wire_num)
{
  uint8_t Order[10];
  uint8_t ucTemp[4] = {0,0,0,0};

  if(i2c_wire_num==1)
  {
    ucTemp[2]=0x18;
    ucTemp[1]=0x13;
    ucTemp[0]=0x00;
  }
  else if(i2c_wire_num==2)
  {
    ucTemp[2]=0x1a;
    ucTemp[1]=0x13;
    ucTemp[0]=0x01;
  }
  else if(i2c_wire_num==4)
  {
    ucTemp[2]=0x19;
    ucTemp[1]=0x13;
    ucTemp[0]=0x01;
  }
  else
  {
    ucTemp[2]=0x18;
    ucTemp[1]=0x13;
    ucTemp[0]=0x01;
  }

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04602C, ucTemp, 3);

  if(i2c_wire_num==1)
  {           
    ucTemp[0]=0x03;
  }
  else if(i2c_wire_num==2)
  {
    ucTemp[0]=0x3b;
  }
  else if(i2c_wire_num==4)
  {
    ucTemp[0]=0x6b;
  }
  else
  {
    ucTemp[0]=0x0b;
  }

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040630, ucTemp, 1);
    
  ucTemp[2]=(uint8_t)(flash_address>>16);
  ucTemp[1]=(uint8_t)(flash_address>>8);
  ucTemp[0]=(uint8_t)(flash_address);

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x040634, ucTemp, 3);

  ucTemp[2]=(uint8_t)(sram_address>>16);
  ucTemp[1]=(uint8_t)(sram_address>>8);
  ucTemp[0]=(uint8_t)(sram_address);

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0x04063C, ucTemp, 3);

  ucTemp[1]=(uint8_t)(copy_length>>8);
  ucTemp[0]=(uint8_t)(copy_length);

  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0X040640, ucTemp, 2);

  ucTemp[0]=0x01;
  gm_i2c_write_me(ICN85XX_PROG_IIC_ADDR, 0X040644, ucTemp, 1);
  
  while(ucTemp[0])
  {
    gm_i2c_read_me(ICN85XX_PROG_IIC_ADDR, 0X040644, ucTemp, 1);
  }
}


/***********************************************************************************************
Name    :   icn85xx_readVersion
Input   :   void
Output  :   
function    :   return version
***********************************************************************************************/
uint16_t icn85xx_readVersion(void)
{
  int      err = 0;
  uint8_t  tmp[2]={0x00,0x00};
  uint16_t CurVersion;
  
  err = gm_i2c_read_me(ICN85XX_IIC_ADDR, 0x000C, tmp, 2);

  if (err < 0) 
  {
    xtm_printf("icn85xx_readVersion:: failed: %d\n", err);
    return 0;
  }
  CurVersion = ((uint16_t)tmp[0]<<8) | tmp[1];
  xtm_printf("ICN85xx_CurVersion = 0x%X, tmp[0] = 0x%X, tmp[1] = 0x%X\n", CurVersion, tmp[0], tmp[1]);

  return CurVersion;
}


uint16_t icn85xx_readEventId(void)
{
  int err = 0;
  uint8_t tmp[2];
  uint16_t EventId;

  err = gm_i2c_read_me(ICN85XX_IIC_ADDR, 0x1008, tmp, 1);
  if (err < 0) 
  {
    xtm_printf("icn85xx_readVersion:: failed: %d\n", err);
    return 0;
  }
  EventId = (uint16_t)tmp[0];
  xtm_printf("EventId = 0x%X, tmp[0] = 0x%X\n", EventId, tmp[0]);
  return EventId;
}


uint16_t icn85xx_readPointX(void)
{
  int      err = 0;
  uint8_t  tmp[2];
  uint16_t PointX;

  err= gm_i2c_read_me(ICN85XX_IIC_ADDR, 0x1003, tmp, 2);
  if (err < 0) 
  {
    xtm_printf("icn85xx_readPointX:: failed: %d\n", err);
    return 0;
  }
  PointX = ((uint16_t)tmp[1]<<8) | tmp[0];
  //xtm_printf("PointX  = 0x%X, tmp[0] = 0x%X, tmp[1] = 0x%X\n", PointX, tmp[0], tmp[1]);
  return PointX;
}


uint16_t icn85xx_readPointY(void)
{
  int      err = 0;
  uint8_t  tmp[2];
  uint16_t PointY;
  
  err= gm_i2c_read_me(ICN85XX_IIC_ADDR, 0x1005, tmp, 2);
  if (err < 0) 
  {
    xtm_printf("icn85xx_readVersion:: failed: %d\n", err);
    return 0;
  }
  PointY = ((uint16_t)tmp[1]<<8) | tmp[0];
  //xtm_printf("PointY = 0x%X, tmp[0] = 0x%X, tmp[1] = 0x%X\n", PointY, tmp[0], tmp[1]);
  return PointY;
}


uint16_t icn85xx_readTouchDownThresold(void)
{
  int      err = 0;
  uint8_t  tmp[2];
  uint16_t Thresold;
  
  err=gm_i2c_read_me(ICN85XX_IIC_ADDR, 0x8076, tmp, 1);
  if (err < 0) 
  {
    xtm_printf("icn85xx_readVersion:: failed: %d\n", err);
    return 0;
  }
  Thresold = tmp[0];
  xtm_printf("Thresold = 0x%X, tmp[0] = 0x%X\n", Thresold, tmp[0]);
  return Thresold;
}


/***********************************************************************************************
Name    :   icn85xx_fw_update 
Input   :   fw
Output  :   
function    :   upgrade fw
***********************************************************************************************/
#define B_SIZE 120
int  icn85xx_fw_update(void)
{
  uint16_t file_size, last_length;
  uint16_t j, num;
  char     temp_buf[B_SIZE];
  uint32_t crc_fw;
    
  uint8_t read_spi[512];
  struct file_head1 file_head;

  gm_spi_flash_data_read(icn8505_wav.Faddr, sizeof(struct file_head1), read_spi);
  
  // xtm_memcopy(&file_head, read_spi, sizeof(struct file_head1));
  memcpy(&file_head, read_spi, sizeof(struct file_head1));
  file_size = file_head.Filelen;

  gm_spi_flash_data_read(icn8505_wav.Faddr + 256, file_size, PcmO_Stream[0]);
  xtm_printf("file_size = 0x%X\n", file_size);

  if(icn85xx_goto_progmode() != 0)
  {
    xtm_printf("icn85xx_goto_progmode() != 0 error\n");
    return -1;
  } 
 
  vTaskDelay(1);
  icn85xx_crc_enable(1);
  xtm_printf("icn85xx_fw_download start\n");
  num = file_size/B_SIZE;
  crc_fw = 0;
  for(j=0; j < num; j++)
  {
    xtm_printf("download::j = %d, num = %d\n", j, num);
    // xtm_memcopy(temp_buf, PcmO_Stream[0] + j*B_SIZE, B_SIZE);

    crc_fw = icn85xx_crc_calc(crc_fw, temp_buf, B_SIZE);
    if(icn85xx_fw_download(j*B_SIZE, temp_buf, B_SIZE) != 0)
    {
      xtm_printf("error j:%d\n",j);
      return -1;
    }
  }
  last_length = file_size - B_SIZE*j;
  if(last_length > 0)
  {
    // xtm_memcopy(temp_buf, PcmO_Stream[0] + j*B_SIZE, last_length);
    memcpy(temp_buf, PcmO_Stream[0] + j*B_SIZE, last_length);
    crc_fw = icn85xx_crc_calc(crc_fw, temp_buf, last_length);
    if(icn85xx_fw_download(j*B_SIZE, temp_buf, last_length) != 0)
    {
      xtm_printf("error last length\n");
      return -1;
    }
  }

  icn85xx_crc_enable(0);
  xtm_printf("crc_fw: 0x%x, 0x%x, 0x%x, 0x%x\n", (uint8_t)((crc_fw & 0xff000000) >> 24), 
  	         (uint8_t)((crc_fw & 0xff0000) >> 16), 
  	         (uint8_t)((crc_fw & 0xff00) >> 8), 
  	         (uint8_t)((crc_fw & 0xff)));
  if(icn85xx_crc_check(crc_fw, file_size) != 0)
  {
    xtm_printf("down fw error, crc error\n");
    return -1;
  } else {
    xtm_printf("downoad fw ok, crc ok\n");
  }
  xtm_printf("icn85xx_fw_download ok\n");
  
  #if 1
    icn85xx_erase_flash();

    FlashWriteEnable();

    icn85xx_prog_buffer( 0, 0, file_size,0);

    while((FlashState(0)&0x01));         
    FlashWriteEnable();

    icn85xx_prog_data(FLASH_CRC_ADDR, crc_fw);
    icn85xx_prog_data(FLASH_CRC_ADDR+4, file_size);

    icn85xx_crc_enable(1);   
    icn85xx_read_flash( 0,  0, file_size,  2);
    icn85xx_crc_enable(0);
    if(icn85xx_crc_check(crc_fw, file_size) != 0)
    {
      xtm_printf("read flash data error, crc error\n");
      return -1;
    }
    else
    {
      xtm_printf("read flash data ok, crc ok\n");
    }      
    while((FlashState(0)&0x01));

    // if(icn85xx_bootfrom_flash() == 0)
    if(icn85xx_bootfrom_sram() == 0)	//code already in ram
    {
      xtm_printf("icn85xx_bootfrom_flash error\n");
      return -1;
    }
  #endif
  
  vTaskDelay(50);
  xtm_printf("icn85xx upgrade ok\n");
  return 0;
}

/*
uint8_t Read_i2c_85xx(uint8_t ReadDeviceAddress, uint8_t *pRData, uint8_t length)
{
  uint8_t r_byte, i;
  uint8_t tmp_data;
  uint8_t I2CStatus;
  
  i2c_START();   //主控MCU在I2C上产生一个起始信号
  I2CStatus = i2c_send_1byte(ReadDeviceAddress); //向I2C总线传送触摸芯片的器件地址和读出方向标记位
  if(I2CStatus)
    xtm_printf("Read_i2c::addr = 0x%X, I2CStatus = %d  ----------------------------\n", ReadDeviceAddress, I2CStatus);
  else
    xtm_printf("Read_i2c::addr = 0x%X, I2CStatus = %d\n", ReadDeviceAddress, I2CStatus);
  
  if(I2CStatus)  //收到触摸芯片的正确回应
  {
    for(i=0; i <= length-1; i++)
    {
     	// delay(20);
     	vTaskDelay(20);
     	pRData[i] = i2c_receive_1byte();
      //xtm_printf("Read_i2c::pRData[%d] = 0x%X\n", i, pRData[i]);
     	if(i < length-1)
     	{
     	  i2c_ACK();
     	} else {
     	  i2c_NOACK();     	//读完数据后的最后一个字节后的第9个时钟不产生回应位，以通知总线上的从机数据读取完毕。
      }
    }
    
    // delay(1);
    vTaskDelay(1);
    i2c_STOP();        //主控MCU在I2C上产生一个停止信号，结束操作
    return _BX;		//
  } else {
    i2c_STOP();    //主控MCU在I2C上产生一个停止信号
    return(1);
  }
  //  return _BX;
}
*/

uint8_t ICN85XX_Init(void)
{
  uint16_t version=0;
  uint8_t temp;

  printf("ICN85XX_Init::gm_i2c_read, \n");

  //1、上进行复位
  //ICN85XX_Rest();

  //2、延迟50ms 后读取固件库的版本号信息
  vTaskDelay(50);

  //3、读取ICN85XX固件库版本信息
  version = icn85xx_readVersion();
  if( version != 0x0000 )    //版本信息正确
  {
    temp = 1;    //版本正确
  }else
    temp = 0;    //版本错误

  return temp;
}
