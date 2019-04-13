#ifndef x_talk_h
#define x_talk_h

extern void Talk_Func(void);       //ͨ�� ����
extern void CallCenter_Func(char *CenterAddr);  //��������
extern void CallR2R_Func(uint8_t *DestAddr);    //�����Խ�
extern void Watch_Func(void);       //����
extern void TalkEnd_Func(void);
extern void WatchEnd_Func(void);
extern void CallTimeOut_Func(void); //���г�ʱ
extern void OpenLock_Func(void);  //����

extern void CreateTalkBuff(uint8_t *sendb, uint8_t Order1, uint8_t Order2, uint8_t Order3, uint8_t *Remote_Addr, uint8_t *Remote_IP);

extern void CreateNSBuff(uint8_t *sendb, uint8_t Order1, uint8_t Order2);

extern void DisplayTalkWindow(uint8_t RefreshType);    //��ʾ�Խ���������
extern void PreDealTalkWindow(uint8_t wintype);    //�Խ���������Ԥ������������Ч��
extern void OperateTalkWindow(uint8_t wintype, uint8_t currwindow);    //�Խ��������ڲ���
extern void CheckTouchTalkWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������


extern void DisplayWatchWindow(uint8_t RefreshType);    //��ʾ���Ӵ���
extern void PreDealWatchWindow(uint8_t wintype);    //���Ӵ���Ԥ������������Ч��
extern void OperateWatchWindow(uint8_t wintype, uint8_t currwindow);    //���Ӵ��ڲ���
extern void CheckTouchWatchWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������


extern void DisplayTalkWatchPicWindow(uint8_t ReturnType, uint8_t WinType);    //��ʾ�Խ����봰��
extern void PreDealTalkWatchPicWindow(uint8_t wintype);    //�Խ����봰��Ԥ������������Ч��
extern void OperateTalkWatchPicWindow(uint8_t wintype, uint8_t currwindow);    //�Խ����봰�ڲ���
extern void CheckTouchTalkWatchPicWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern struct TalkCall1 TalkCall;
#endif