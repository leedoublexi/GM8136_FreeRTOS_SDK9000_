#ifndef x_record_h
#define x_record_h

extern void DisplayRecordsWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //��ʾͨ����¼����
extern void PreDealRecordsWindow(uint8_t wintype);    //ͨ����¼����Ԥ������������Ч��
extern void OperateRecordsWindow(uint8_t wintype, uint8_t currwindow);    //ͨ����¼���ڲ���
extern void CheckTouchRecordsWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplayRecordsContentWindow(uint8_t recordsno);    //ͨ����¼�鿴��Ϣ����
extern void PreDealRecordsContentWindow(uint8_t wintype);    //�鿴ͨ����¼����Ԥ������������Ч��
extern void OperateRecordsContentWindow(uint8_t wintype, uint8_t currwindow);    //�鿴ͨ����¼���ڲ���
extern void CheckTouchRecordsContentWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������


extern uint8_t Init_RecordsVar(void); //��ʼ�� Records �ṹ
extern uint8_t Uninit_RecordsVar(void); //�ͷ� Records �ṹ
//��ͨ����¼�ļ�
extern void ReadRecordsFile(void);
//дͨ����¼�ļ�
extern void WriteRecordsFile(void);

extern void ResetRecordsCfg(void);    //��λͨ����¼�����ļ�
extern void WriteRecordsCfgFile(void);     //дͨ����¼�����ļ�

extern void Recv_NewRecords(void);
//д��Ƭ����
extern void WritePicFunc(void);
#endif