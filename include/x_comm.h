#ifndef x_comm_h
#define x_comm_h
extern void InitComm(void);
extern void dcomm1(void);
extern void dcomm2(void);
extern void CommSendBuff(uint8_t comm_fd, uint8_t *comm_buf, uint16_t send_len);

extern void KeyTouchOperate(uint8_t TouchNum); //������Ӧ
extern void KeyPreDeal(void); //����Ԥ��������Ч��
extern void KeyBackDeal(void); //������������Ч�� ������

extern uint8_t CheckKeyPressAndLcd(void); //��鰴��������LCD
#endif