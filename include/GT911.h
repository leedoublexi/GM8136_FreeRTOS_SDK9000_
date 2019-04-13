#ifndef __GT911_H__
#define	__GT911_H__  

#include <FreeRTOS.h>


//���IO�ͽṹ��Ķ���
 
//#define CT_RST    		PBout(2)//��λ����
//#define CT_INT    		PBin(10)//�ж�����	
//////////////////////////////////////////////////////////////////////////////////	 
//#define GT_I2C_ADDR		0XBA >> 1	//д��������

#define GT_I2C_ADDR		0x28 >> 1	//д��������

//#define GT_CMD_WR		0X28	//д��������
//#define GT_CMD_RD   	0X29	//����������

#define GT_EXCHG_XY		0		//��תXY����

#define GT_MAX_TOUCH    5		//���ݴ��������֧�ֵĵ���
#define GT_READ_XY_REG 	0x814E  	//��ȡ����Ĵ��� 
#define GT_CONFIG_REG   0x8047	//���ò�����ʼ�Ĵ���
#define GT_SLEEP_REG 	0x8940

//���ݴ�����������
typedef struct
{
	//uint8_t   (*init)(void);			//��ʼ��������������
	//void (*scan)(void);			//ɨ�败���� 	 
	uint16_t x[GT_MAX_TOUCH];		//����X����
	uint16_t y[GT_MAX_TOUCH];		//����Y����
 	uint8_t ppr[GT_MAX_TOUCH];		//�������ѹ��
 	uint16_t psize[GT_MAX_TOUCH];
	uint8_t tpsta;					//������״̬
								//[0]:0,������0��Ч;1,������0��Ч
								//[1]:0,������1��Ч;1,������1��Ч
								//[2]:0,������2��Ч;1,������2��Ч
								//[3]:0,������3��Ч;1,������3��Ч
								//[4]:0,������4��Ч;1,������4��Ч
								//[5:6]:����
								//[7]:0,û����Ч����;1,����Ч����,���Զ�������
				   	    	  
}_my_touchp_dev;
extern _my_touchp_dev ctp_dev;

uint8_t GT911_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);//��GT911д��һ������						    
uint8_t GT911_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);//��GT911����һ������
uint8_t GT911_Send_Cfg(uint8_t * buf,uint16_t cfg_len);//��GT911�������ò���
void GT911_Wakeup_Sleep(void);			//����GT911
uint8_t GT911_Init(void);	    			//��ʼ��GT911						 
void GT911_Scan(void);					//ɨ��GT911

uint8_t MainScan(uint16_t *x, uint16_t *y);

void GT911_Enter_Sleep(void);
#endif


