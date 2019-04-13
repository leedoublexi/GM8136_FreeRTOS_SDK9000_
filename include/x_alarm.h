#ifndef x_alarm_h
#define x_alarm_h

extern void DisplayCancelFortifyWindow(uint8_t RefreshType);    //��ʾ��������
extern void PreDealCancelFortifyWindow(uint8_t wintype);    //��������Ԥ������������Ч��
extern void OperateCancelFortifyWindow(uint8_t wintype, uint8_t currwindow);    //�������ڲ���
extern void CheckTouchCancelFortifyWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������
extern void CancelFortifyDeal(void); //�������洦��
extern void PlayCancelFortifyWav(void);
extern void PlayDefenceDelayWav(void); //���� ������ʱ�� ��ʾ

extern void PlayCancelAlarmWav(void);

extern void DisplayFortifyWindow(uint8_t RefreshType);    //��ʾ��������
extern void PreDealFortifyWindow(uint8_t wintype);    //��������Ԥ������������Ч��
extern void OperateFortifyWindow(uint8_t wintype, uint8_t currwindow);    //�������ڲ���
extern void CheckTouchFortifyWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplayAlarmWindow(uint8_t RefreshType);    //��ʾ��������
extern void PreDealAlarmWindow(uint8_t wintype);    //��������Ԥ������������Ч��
extern void OperateAlarmWindow(uint8_t wintype, uint8_t currwindow);    //�������ڲ���
extern void CheckTouchAlarmWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void ShowAlarmImage(void);

extern void DisplayCancelAlarmWindow(uint8_t RefreshType);    //��ʾȡ����������
extern void PreDealCancelAlarmWindow(uint8_t wintype);    //ȡ����������Ԥ������������Ч��
extern void OperateCancelAlarmWindow(uint8_t wintype, uint8_t currwindow);    //ȡ���������ڲ���
extern void CheckTouchCancelAlarmWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������
extern void CancelAlarmDeal(void); //ȡ���������洦��

extern void CheckDefenceDelay_Func(void); //��鲼����ʱ����
extern void FortifyFinishDeal(void); //������ɽ��洦��
extern void CheckDelayAlarm(void);  //�����ʱ��������
extern void SendAlarmFunc(uint8_t SpecialByte, uint8_t AlarmByte[2]); //������������
extern void SendCancelAlarmFunc(void); //ȡ����������

extern void InitDefence(void);
extern void CheckDefence(unsigned char DefenceByte);    //����������
#endif