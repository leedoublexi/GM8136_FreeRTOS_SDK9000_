
#ifndef x_main_h
#define x_main_h
extern void Init_LocalVar(void); //初始化 Local 结

extern void ResetLocalCfg(void);    //复位配置文件

//读设置文件
extern void ReadCfgFile(void);
//写设置文件
extern void WriteCfgFile(void);
//写数据到Flash
extern void SaveToFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length);
//从Flash读数据
extern void ReadFromFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length);

extern uint8_t *Flash_Stream;      //Flash存储
#endif