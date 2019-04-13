#ifndef x_info_h
#define x_info_h

extern void DisplayInfoWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //��ʾ��Ϣ����
extern void PreDealInfoWindow(uint8_t wintype);    //��Ϣ����Ԥ������������Ч��
extern void OperateInfoWindow(uint8_t wintype, uint8_t currwindow);    //��Ϣ���ڲ���
extern void CheckTouchInfoWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplayInfoContentWindow(uint8_t infotype, uint8_t infono);    //��ʾ�鿴��Ϣ����
extern void PreDealInfoContentWindow(uint8_t wintype);    //�鿴��Ϣ����Ԥ������������Ч��
extern void OperateInfoContentWindow(uint8_t wintype, uint8_t currwindow);    //�鿴��Ϣ���ڲ���
extern void CheckTouchInfoContentWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern uint8_t Init_InfoVar(void); //��ʼ�� Info �ṹ
extern uint8_t Uninit_InfoVar(void); //�ͷ� Info �ṹ
//����Ϣ�ļ�
extern void ReadInfoFile(void);
//д��Ϣ�ļ�
extern void WriteInfoFile(void);

extern void ResetInfoCfg(void);    //��λ��Ϣ�����ļ�
extern void WriteInfoCfgFile(void);     //д��Ϣ�����ļ�

extern void Recv_NewInfo(void);
#endif