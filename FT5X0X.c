#define CommonH
#include "common.h"
#include "lib_i2c.h"
#include "FT5X0X.h"

#define FT5X0X_IIC_ADDR         0x70 >> 1
#define FT_CMD_WR   0X70    //写命令
#define FT_CMD_RD   0X71    //读命令

#define FT_DEVIDE_MODE          0x00    //FT5426模式控制寄存器
#define FT_ID_G_MODE            0xA4    //FT5426中断模式控制寄存器
#define FT_ID_G_THGROUP         0x80    //触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE    0x88    //激活状态周期设置寄存器
#define FT_ID_G_LIB_VERSION     0xA1    //版本号
#define FT_REG_NUM_FINGER       0x02    //触摸状态寄存器

#define FT_TP1_REG  0X03    //第一个触摸点数据地址
#define FT_TP2_REG  0X09    //第二个触摸点数据地址
#define FT_TP3_REG  0X0F    //第三个触摸点数据地址
#define FT_TP4_REG  0X15    //第四个触摸点数据地址
#define FT_TP5_REG  0X1B    //第五个触摸点数据地址
const u16 FT5426_TPX_TBL[5]={FT_TP1_REG,FT_TP2_REG,FT_TP3_REG,FT_TP4_REG,FT_TP5_REG};//5个触摸点
int ft5x0x_init(void)
{
  uint8_t temp[2];
  int err;
  int ret = 0;

  temp[0]=0;
  gm_i2c_write_me_one(FT5X0X_IIC_ADDR, FT_DEVIDE_MODE, temp, 1);//进入正常操作模式
  temp[0]=1;
  gm_i2c_write_me_one(FT5X0X_IIC_ADDR, FT_ID_G_MODE, temp, 1);//中断模式
  temp[0]=22;
  gm_i2c_write_me_one(FT5X0X_IIC_ADDR, FT_ID_G_THGROUP, temp, 1);//触摸有效值
  temp[0]=12; 
  gm_i2c_write_me_one(FT5X0X_IIC_ADDR, FT_ID_G_PERIODACTIVE, temp, 1);//激活周期

  //读取版本号，参考值：0X3003||0X0001||0X0002
  err = gm_i2c_read_me_one(FT5X0X_IIC_ADDR, FT_ID_G_LIB_VERSION, temp, 2);//给定的触摸点
  
  if(err<0)
    ret=0;
  else
    ret=1;
    
  xtm_printf("FT5X0X Version = 0x%x%x\n",temp[0],temp[1]);//Version = 0x02
  xtm_printf("ret = %d\n",ret);
  return ret;
}

void ft5x0x_read_touch_x_y(uint16_t *x,uint16_t *y)
{
  int mode = 0;
  uint8_t temp;
  uint8_t buf[4];
  int i;
  
  gm_i2c_read_me_one(FT5X0X_IIC_ADDR, FT_REG_NUM_FINGER, &mode, 1);//读取触摸点的状态
  if((mode&0X0F)&&((mode&0X0F)<6))//有触摸点按下
  {
    gm_i2c_read_me_one(FT5X0X_IIC_ADDR, FT5426_TPX_TBL[0], buf, 4);//读取XY坐标值
    *x = ((uint16_t)(buf[0]&0X0F)<<8)+buf[1];
    *y = ((uint16_t)(buf[2]&0X0F)<<8)+buf[3];
    xtm_printf("FT5X0X scan_x = %d   scan_y=%d\n",*x,*y);
  }
}
