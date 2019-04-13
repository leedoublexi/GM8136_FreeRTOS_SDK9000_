#define CommonH
#include "common.h"

#include "lib_i2c.h"
#include "gt911.h"



#define GT911_PORT 0x02dc
#define GT911_PORT_IN 0x02de
#define GT911_PORT_OEN 0x02dd


#define GT911_SCL 0x80     // GPIOD.7
#define GT911_SDA 0x20     // GPIOD.5


//#define _H8K1S


#define GT911_ACKTIMES 0xff
#define CONFIG_REG_SIZE sizeof(GTP911_CFG_DATA)

uint8_t count[5];
uint16_t lastpos[5][2];		//���һ�ε�����


//0x00,0x04,0x58,0x02---0x60(1024*600)
//0x20,0x03,0xe0,0x01---0xba(800*480)
//���������ò���(�����������ṩ)
const uint8_t GTP911_CFG_DATA[]=
{
	//* h8���ڻ�  7��
#ifdef _H8K1S
	0x5A,0x20,0x03,0xE0,0x01,0x05,0x0D,0x00,0x01,
	0x08,0x28,0x05,0x50,0x32,0x03,0x05,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x8C,0x2E,0x0E,0x22,0x24,0x31,0x0D,0x00,0x00,
	0x00,0x99,0x03,0x1D,0x00,0x00,0x00,0x00,0x00,
	0x03,0x64,0x32,0x00,0x00,0x00,0x1C,0x41,0x94,
	0xC5,0x02,0x07,0x00,0x00,0x04,0x9C,0x11,0x00,
	0x76,0x17,0x00,0x5B,0x1F,0x00,0x47,0x2A,0x00,
	0x39,0x38,0x00,0x39,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,
	0x0C,0x0E,0x10,0x12,0x14,0x16,0x18,0x1A,0x1C,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
	0x04,0x06,0x08,0x0A,0x0C,0x0F,0x10,0x12,0x13,
	0x14,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,
	0x22,0x24,0x26,0x28,0x29,0x2A,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x80,0x01
#else
	//H4���ڻ� 6.95�紥����
	0x50,0x20,0x03,0xE0,0x01,0x05,0x0D,0x00,0x01,
	0x0A,0x28,0x0F,0x50,0x32,0x03,0x05,0x00,0x00,
	0x00,0x00,0x00,0x00,0x08,0x17,0x19,0x1C,0x14,
	0x87,0x29,0x0A,0x4E,0x50,0xEB,0x04,0x00,0x00,
	0x00,0x00,0x02,0x11,0x00,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x46,0x64,0x94,
	0xC5,0x02,0x07,0x00,0x00,0x04,0x9E,0x48,0x00,
	0x8D,0x4D,0x00,0x7F,0x53,0x00,0x73,0x59,0x00,
	0x67,0x60,0x00,0x67,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,
	0x0C,0x0E,0x10,0x12,0x14,0xFF,0xFF,0xFF,0xFF,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
	0x04,0x06,0x08,0x0A,0x0C,0x1D,0x1E,0x1F,0x20,
	0x21,0x22,0x24,0x26,0x28,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xB6,0x01
#endif

};	


//���ݴ�����������
_my_touchp_dev ctp_dev=
{
	//GT911_Init,
	//GT911_Scan,
	0,
	0,
	0,
	0,
	0,
};



//��GT911д��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t GT911_WR_Reg(uint16_t address,uint8_t *buf,uint8_t len)
{
/*	uint8_t i;
	uint8_t ret=0;
	GT911_START();	
 	GT911_send_1byte(GT_CMD_WR);   	//����д���� 	 
	GT911_send_1byte(address>>8);   	//���͸�8λ��ַ	 										  		   
	GT911_send_1byte(address&0XFF);   	//���͵�8λ��ַ
	for(i=0;i<len;i++)
	{	   
    	GT911_send_1byte(buf[i]);  	//������
	}
    GT911_STOP();					//����һ��ֹͣ����
	return ret; */
	return 0;
}

//��GT911����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���

void GT911_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
/*	uint8_t i;
 	GT911_START();	
 	GT911_send_1byte(GT_CMD_WR);   	//����д���� 	 
 	GT911_send_1byte(reg>>8);   	//���͸�8λ��ַ	 										  		   
 	GT911_send_1byte(reg&0XFF);   	//���͵�8λ��ַ
	//�������ﻹ�и�ֹͣ�� �ź�
	GT911_STOP();//����һ��ֹͣ����
 	GT911_START();  	 	   
	GT911_send_1byte(GT_CMD_RD);    //���Ͷ�����		   
	#if 1
	//���һ������Ӧ��
	for(i=0;i<len;i++)
	{	   
		buf[i] = GT911_receive_1byte();

    	if(i==(len-1))
			GT911_NOACK();
		else
			GT911_ACK();
	}
	#endif
    GT911_STOP();//����һ��ֹͣ����	    */
}


//��GT911�������ò���
//buf:���ò�����ָ��
//cfg_len:���ò�������
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t GT911_Send_Cfg(uint8_t * buf,uint16_t cfg_len)
{
	uint8_t ret=0;
	uint8_t retry=0;
	for(retry=0;retry<5;retry++)
	{
		ret=gm_i2c_write_me (GT_I2C_ADDR, GT_CONFIG_REG, buf, cfg_len);
		xtm_printf("GT911_Send_Cfg::cfg_len = %d, ret = %x\r\n", cfg_len, ret);
		if(ret==0) 
			break;
		vTaskDelay(10);
	
	}
	return ret;
}

uint8_t GT911_Dynamic_Cfg(uint16_t Startaddr, uint8_t * buf,uint16_t cfg_len)
{	
	uint8_t ret=0;
	uint8_t retry=0;
	uint32_t tt=0;
	uint8_t buffer[CONFIG_REG_SIZE-2]={0};
	uint8_t flashbuf[2];
	for(retry=0;retry<5;retry++)
	{
		ret=GT911_WR_Reg(Startaddr,buf,cfg_len);
		xtm_printf("*************mycount%x\r\n",ret);
		if(ret==0)break;
		vTaskDelay(10);
	}

	GT911_RD_Reg(GT_CONFIG_REG,buffer,CONFIG_REG_SIZE-2);
	for(retry=0;retry <sizeof(buffer);retry++)
	{
		tt=tt+buffer[retry];
	}

	tt=tt&0x000000ff;
	tt=~tt;
	tt=tt+0x00000001;
	tt=tt&0x000000ff;
	xtm_printf("########ttcount=%x\r\n",tt);

	#if 1

	flashbuf[0]=(uint8_t)tt&0xff;
	flashbuf[1]=1;
	for(retry=0;retry<5;retry++)
	{
		ret=GT911_WR_Reg(0x80ff,flashbuf,2);
		if(ret==0)break;
		vTaskDelay(10);
	}
	
	#endif
}


void GT911_Enter_Sleep(void)
{
	#if 0
	uint8_t buf=5;
	outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) & ~(1<<5)); //�������
	outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA)& ~(1<<5));//INT ����
	vTaskDelay(5);
	if(GT911_WR_Reg(GT_SLEEP_REG,&buf,1))
	{
		 xtm_printf("&&&&&&Enter Sleep Failed!!! 0\n");
		return ;
	}
	vTaskDelay(60);	
	outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA)|(1<<5));//INT ����
	vTaskDelay(3);
	outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) |(1<<5)); //�������
	 xtm_printf("&&&&&&&Enter Sleep Succeed!!!\n");

	#endif

}

//GT811����
void GT911_Wakeup_Sleep(void)
{
#if 0
	//outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA) & ~(1<<5));
	vTaskDelay(10);
	//outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA) |(1<<5));//
	vTaskDelay(200);
#endif	
}



void GT911_Rest(void){//�趨��ַΪ 0xBA/0xBB ��ʱ��
#if 0
#if 1
	//outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) & ~(1<<5)); //�������
	//outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA)  |(1<<5));
	
	outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) & ~(1<<5)); //�������
	outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA)& ~(1<<5));//INT ����
	//��100us ��
	asm{

		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop

	
	}
	//res ����
	//outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) & ~(1<<5)); //�������
	//outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA) |(1<<5));//
	//�ӳ�5ms ��INT �������ģʽ
	vTaskDelay(15);
	outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) |(1<<5)); //�������
	//outportb(GPIOD_OUT_DATA, inportb(GPIOD_OUT_DATA)|(1<<5));//INT ����

#else
	//outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) & ~(1<<5)); //�������
	//outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA) |(1<<5));
	//outportb(GPIOD_OEN_DATA, inportb(GPIOD_OEN_DATA) |(1<<5)); //������
	outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) & ~(1<<5)); //�������
	outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA)& ~(1<<5));//INT ����
	//outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA)|(1<<5));//INT ����
	//��100us ��
	asm{

		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop


	}
	//res ����
	//outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) & ~(1<<5)); //�������
	//outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA) |(1<<5));//
	//�ӳ�5ms ��INT �������ģʽ
	outportb(GPIOA_OUT_DATA,inportb(GPIOA_OUT_DATA)| (1<<5));//INT ����
	vTaskDelay(6);

	outportb(GPIOA_OEN_DATA, inportb(GPIOA_OEN_DATA) |(1<<5)); //�������

	//outportb(GPIOA_OUT_DATA, inportb(GPIOA_OUT_DATA) |(1<<5));//INT ����
	//outportb(GPIOD_OUT_DATA, inportb(GPIOD_OUT_DATA)|(1<<5));//INT ����
	vTaskDelay(15);

#endif
#endif
}


//GT911��ʼ��
//����ֵ:��ʼ�����
//0,��ʼ���ɹ�;
//1,�������ò�������
//2,�汾����
uint8_t GT911_Init(void)
{

	uint16_t version=0;
	uint8_t mm,i;
	uint8_t temp, buf[1024];

	printf("GT911_Init::gm_i2c_read, \n");
	
	

//1���Ͻ��и�λ
	GT911_Rest();

// 2���ӳ�50ms ���ȡ�̼���İ汾����Ϣ
	vTaskDelay(60);


// 3������������Ϣ
	gm_i2c_read_me(GT_I2C_ADDR, 0X8144, &temp, 1);			//��ȡ�汾�߰�λ
	printf("GT911_Init::gm_i2c_read 0X8144, temp = 0x%X\n", temp); 
	version=(uint16_t)temp<<8; 
	gm_i2c_read_me(GT_I2C_ADDR, 0X8145, &temp, 1);				//��ȡ�汾�Ͱ�λ 
	printf("GT911_Init::gm_i2c_read 0X8145, temp = 0x%X\n", temp); 
	version|=temp;
	xtm_printf("version:%x\r\n",version); 

	if(version==0X6010)							//�汾��ȷ,�������ò���
	{ 
		temp=GT911_Send_Cfg((uint8_t*)GTP911_CFG_DATA,sizeof(GTP911_CFG_DATA));//�������ò���
		temp = 1;
	}
	else 
		temp=0;	//�汾����

	//for(i=0; i<10; i++){
	//	gm_i2c_read_me(GT_I2C_ADDR, 0x8047 + i, &buf[0], 1);				//��ȡ�汾�Ͱ�λ 
	//	printf("GT911_Init::gm_i2c_read 0x%X, buf[0] = 0x%X\n", 0x8047 + i, buf[0]); 
	//}
	for(i=0;i<5;i++)
	{
		lastpos[i][0]=0XFFFF;	//ȫ������Ϊ�Ƿ�ֵ
		lastpos[i][1]=0XFFFF;
		count[i]=0;	   			//��ʱ������
	}
	//mm=0xAA;
	//GT911_WR_Reg(0x8041,&mm,1);
	return temp; 

  
}





//ɨ��GT911
//�õ��Ľ��������ctp_dev�ṹ����								   									 
void GT911_Scan(void)
{	



// 1���ȶ�ȡ�Ĵ��� 0x814E��
//����ǰ buffer��buffer status Ϊ 1������׼���ã�
//��������ָ������������״̬ȡ��Ӧ���������ꡢ������Ϣ��
// 2������ 1 �з��� buffer ���ݣ�buffer status Ϊ 0��δ׼���ã���ȴ� 1ms �ٽ��ж�ȡ��
	uint8_t buf[41];//һ�ζ�ȡ34�ֽ�
	//if(GT911_RD_Reg(GT_READ_XY_REG,buf,1))
	//if((ctp_dev.tpsta&0X80)==0)return;			//����Ч����,���ȡ����,����ֱ���˳�
  	//GT911_RD_Reg(GT_READ_XY_REG,buf,41);		//һ�ζ�ȡ34���ֽ�
  	gm_i2c_read_me(GT_I2C_ADDR, GT_READ_XY_REG, buf, 41);		
  	if(buf[0]&0x80==0)return;
	ctp_dev.tpsta=buf[0]&0X4F;					//��������λ,ͬʱ�����Ч�������
#if 0								//��תXY
	ctp_dev.y[0]=480-(((uint16_t)buf[5]<<8)+buf[4]);	//������0����
	ctp_dev.x[0]=((uint16_t)buf[3]<<8)+buf[2];	  
	ctp_dev.ppr[0]=buf[1];
	
	ctp_dev.y[1]=480-(((uint16_t)buf[13]<<8)+buf[12]);	//������1����
	ctp_dev.x[1]=((uint16_t)buf[11]<<8)+buf[10];	  
	ctp_dev.ppr[1]=buf[9];
	
	ctp_dev.y[2]=480-(((uint16_t)buf[21]<<8)+buf[20]);//������2����
	ctp_dev.x[2]=((uint16_t)buf[19]<<8)+buf[18];	  
	ctp_dev.ppr[2]=buf[17];
	
	ctp_dev.y[3]=480-(((uint16_t)buf[29]<<8)+buf[28]);//������3����
	ctp_dev.x[3]=((uint16_t)buf[27]<<8)+buf[26];	  
	ctp_dev.ppr[3]=buf[25];
	
	ctp_dev.y[4]=480-(((uint16_t)buf[37]<<8)+buf[36]);//������4����
	ctp_dev.x[4]=((uint16_t)buf[35]<<8)+buf[34];	  
	ctp_dev.ppr[4]=buf[33];
	
#else 
	#if 1
#ifdef _H8K1S
	ctp_dev.psize[0]=((uint16_t)(buf[7]<<8)+buf[6]);
	ctp_dev.y[0]=((uint16_t)(buf[5]<<8)+buf[4]);	//������0����
	ctp_dev.x[0]=((uint16_t)(buf[3]<<8)+buf[2]);	  
	ctp_dev.ppr[0]=buf[1];

	ctp_dev.psize[1]=((uint16_t)(buf[15]<<8)+buf[14]);
	ctp_dev.y[1]=((uint16_t)(buf[13]<<8)+buf[12]);	//������1����
	ctp_dev.x[1]=((uint16_t)(buf[11]<<8)+buf[10]);	  
	ctp_dev.ppr[1]=buf[9];

	ctp_dev.psize[2]=((uint16_t)(buf[23]<<8)+buf[22]);
	ctp_dev.y[2]=((uint16_t)(buf[21]<<8)+buf[20]);//������2����
	ctp_dev.x[2]=((uint16_t)(buf[19]<<8)+buf[18]);	  
	ctp_dev.ppr[2]=buf[17];

	ctp_dev.psize[3]=((uint16_t)(buf[31]<<8)+buf[30]);
	ctp_dev.y[3]=((uint16_t)(buf[29]<<8)+buf[28]);//������3����
	ctp_dev.x[3]=((uint16_t)(buf[27]<<8)+buf[26]);	  
	ctp_dev.ppr[3]=buf[25];

	ctp_dev.psize[4]=((uint16_t)(buf[39]<<8)+buf[38]);
	ctp_dev.y[4]=((uint16_t)(buf[37]<<8)+buf[36]);//������4����
	ctp_dev.x[4]=((uint16_t)(buf[35]<<8)+buf[34]);	  
	ctp_dev.ppr[4]=buf[33];
#else
	ctp_dev.psize[0]=((uint16_t)(buf[7]<<8)+buf[6]);
	ctp_dev.y[0]=((uint16_t)(buf[5]<<8)+buf[4]-20);	//������0����
	ctp_dev.x[0]=((uint16_t)(buf[3]<<8)+buf[2]-10);	  
	ctp_dev.ppr[0]=buf[1];

	ctp_dev.psize[1]=((uint16_t)(buf[15]<<8)+buf[14]);
	ctp_dev.y[1]=((uint16_t)(buf[13]<<8)+buf[12]-20);	//������1����
	ctp_dev.x[1]=((uint16_t)(buf[11]<<8)+buf[10]-10);	  
	ctp_dev.ppr[1]=buf[9];

	ctp_dev.psize[2]=((uint16_t)(buf[23]<<8)+buf[22]);
	ctp_dev.y[2]=((uint16_t)(buf[21]<<8)+buf[20]-20);//������2����
	ctp_dev.x[2]=((uint16_t)(buf[19]<<8)+buf[18]-10);	  
	ctp_dev.ppr[2]=buf[17];

	ctp_dev.psize[3]=((uint16_t)(buf[31]<<8)+buf[30]);
	ctp_dev.y[3]=((uint16_t)(buf[29]<<8)+buf[28]-20);//������3����
	ctp_dev.x[3]=((uint16_t)(buf[27]<<8)+buf[26]-10);	  
	ctp_dev.ppr[3]=buf[25];

	ctp_dev.psize[4]=((uint16_t)(buf[39]<<8)+buf[38]);
	ctp_dev.y[4]=((uint16_t)(buf[37]<<8)+buf[36]-20);//������4����
	ctp_dev.x[4]=((uint16_t)(buf[35]<<8)+buf[34]-10);	  
	ctp_dev.ppr[4]=buf[33];
#endif
	#else 

	ctp_dev.psize[0]=((uint16_t)(buf[7]<<8)+buf[6]);
	ctp_dev.y[0]=(uint16_t)(((uint16_t)(buf[5]<<8)+buf[4])*25/32);	//������0����
	ctp_dev.x[0]=(uint16_t)(((uint16_t)(buf[3]<<8)+buf[2])*12/15);	
	ctp_dev.ppr[0]=buf[1];

	ctp_dev.psize[1]=((uint16_t)(buf[15]<<8)+buf[14]);
	ctp_dev.y[1]=(uint16_t)(((uint16_t)(buf[13]<<8)+buf[12])*25/32);	//������1����
	ctp_dev.x[1]=(uint16_t)(((uint16_t)(buf[11]<<8)+buf[10])*12/15);	  
	ctp_dev.ppr[1]=buf[9];

	ctp_dev.psize[2]=((uint16_t)(buf[23]<<8)+buf[22]);
	ctp_dev.y[2]=(uint16_t)(((uint16_t)(buf[21]<<8)+buf[20])*25/32);//������2����
	ctp_dev.x[2]=(uint16_t)(((uint16_t)(buf[19]<<8)+buf[18])*12/15);	  
	ctp_dev.ppr[2]=buf[17];

	ctp_dev.psize[3]=((uint16_t)(buf[31]<<8)+buf[30]);
	ctp_dev.y[3]=(uint16_t)(((uint16_t)(buf[29]<<8)+buf[28])*25/32);//������3����
	ctp_dev.x[3]=(uint16_t)(((uint16_t)(buf[27]<<8)+buf[26])*12/15);	  
	ctp_dev.ppr[3]=buf[25];

	ctp_dev.psize[4]=((uint16_t)(buf[39]<<8)+buf[38]);
	ctp_dev.y[4]=(uint16_t)(((uint16_t)(buf[37]<<8)+buf[36])*25/32);//������4����
	ctp_dev.x[4]=(uint16_t)(((uint16_t)(buf[35]<<8)+buf[34])*12/15);	  
	ctp_dev.ppr[4]=buf[33];
	#endif

#endif	 		    
}


uint8_t MainScan(uint16_t *x,uint16_t *y)
{
	uint8_t res=0, dat=0,i, ret;

	gm_i2c_read_me(GT_I2C_ADDR, 0x814E, &dat, 1);	
	//xtm_printf("0x814E -- dat = 0x%X\n", dat);
	if(dat&0x80){
		
		GT911_Scan();
		dat=0;
		ret=gm_i2c_write_me (GT_I2C_ADDR, 0x814E, &dat,1);
	}
	
	if(ctp_dev.tpsta&0x4f)
	{
		for(i=0;i<5;i++)
		{
			if(ctp_dev.tpsta&(1<<i))
			{
				if(ctp_dev.x[i]<SCREEN_WIDTH&&ctp_dev.y[i]<SCREEN_HEIGHT)
				{
					if(lastpos[i][0]==0XFFFF)
					{
						*x=ctp_dev.x[i];
						*y=ctp_dev.y[i];
						
						xtm_printf("ctp_dev.x[%d]=%d ---->ctp_dev.y[%d]=%d -->>Psize=%d\r\n",i,*x,i,*y,ctp_dev.psize[i]);
						xtm_printf("ctp_dev.PStau[%d]=%x\r\n",i,ctp_dev.tpsta);
						lastpos[i][0]=ctp_dev.x[i];
						lastpos[i][1]=ctp_dev.y[i];
						count[i]=0;
						res=1;
						break;
					}
				}
			}
		}
		
		ctp_dev.tpsta =0;
	
	}else{

		for(i=0;i<5;i++)
		{
			count[i]++;
			if(count[i]>1)
			{
				count[i]=0;
				lastpos[i][0]=0XFFFF;
			}
		}

	}
	return res;

}



