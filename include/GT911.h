#ifndef __GT911_H__
#define	__GT911_H__  

#include <FreeRTOS.h>


//相关IO和结构体的定义
 
//#define CT_RST    		PBout(2)//复位引脚
//#define CT_INT    		PBin(10)//中断引脚	
//////////////////////////////////////////////////////////////////////////////////	 
//#define GT_I2C_ADDR		0XBA >> 1	//写数据命令

#define GT_I2C_ADDR		0x28 >> 1	//写数据命令

//#define GT_CMD_WR		0X28	//写数据命令
//#define GT_CMD_RD   	0X29	//读数据命令

#define GT_EXCHG_XY		0		//调转XY坐标

#define GT_MAX_TOUCH    5		//电容触摸屏最大支持的点数
#define GT_READ_XY_REG 	0x814E  	//读取坐标寄存器 
#define GT_CONFIG_REG   0x8047	//配置参数起始寄存器
#define GT_SLEEP_REG 	0x8940

//电容触摸屏控制器
typedef struct
{
	//uint8_t   (*init)(void);			//初始化触摸屏控制器
	//void (*scan)(void);			//扫描触摸屏 	 
	uint16_t x[GT_MAX_TOUCH];		//触摸X坐标
	uint16_t y[GT_MAX_TOUCH];		//触摸Y坐标
 	uint8_t ppr[GT_MAX_TOUCH];		//触摸点的压力
 	uint16_t psize[GT_MAX_TOUCH];
	uint8_t tpsta;					//触摸屏状态
								//[0]:0,触摸点0无效;1,触摸点0有效
								//[1]:0,触摸点1无效;1,触摸点1有效
								//[2]:0,触摸点2无效;1,触摸点2有效
								//[3]:0,触摸点3无效;1,触摸点3有效
								//[4]:0,触摸点4无效;1,触摸点4有效
								//[5:6]:保留
								//[7]:0,没有有效触摸;1,有有效触摸,可以读出数据
				   	    	  
}_my_touchp_dev;
extern _my_touchp_dev ctp_dev;

uint8_t GT911_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);//向GT911写入一次数据						    
uint8_t GT911_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);//从GT911读出一次数据
uint8_t GT911_Send_Cfg(uint8_t * buf,uint16_t cfg_len);//向GT911发送配置参数
void GT911_Wakeup_Sleep(void);			//唤醒GT911
uint8_t GT911_Init(void);	    			//初始化GT911						 
void GT911_Scan(void);					//扫描GT911

uint8_t MainScan(uint16_t *x, uint16_t *y);

void GT911_Enter_Sleep(void);
#endif


