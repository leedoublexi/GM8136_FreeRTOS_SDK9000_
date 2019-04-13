#ifndef x_comm_h
#define x_comm_h
extern void InitComm(void);
extern void dcomm1(void);
extern void dcomm2(void);
extern void CommSendBuff(uint8_t comm_fd, uint8_t *comm_buf, uint16_t send_len);

extern void KeyTouchOperate(uint8_t TouchNum); //按键响应
extern void KeyPreDeal(void); //按键预处理，按下效果
extern void KeyBackDeal(void); //按键后处理，弹起效果 并处理

extern uint8_t CheckKeyPressAndLcd(void); //检查按键并点亮LCD
#endif