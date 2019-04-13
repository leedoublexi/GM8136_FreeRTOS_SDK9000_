
#ifndef x_main_h
#define x_main_h
extern void Init_LocalVar(void); //��ʼ�� Local ��

extern void ResetLocalCfg(void);    //��λ�����ļ�

//�������ļ�
extern void ReadCfgFile(void);
//д�����ļ�
extern void WriteCfgFile(void);
//д���ݵ�Flash
extern void SaveToFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length);
//��Flash������
extern void ReadFromFlash(uint32_t FlashAddr, uint8_t *buf, uint16_t length);

extern uint8_t *Flash_Stream;      //Flash�洢
#endif