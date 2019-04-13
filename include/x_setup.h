#ifndef x_setup_h
#define x_setup_h


extern void *xtm_memcopy(void *dest, void *src, UINT16 n);
extern void DisplaySetupWindow(uint8_t RefreshType);    //��ʾ�������ô���
extern void PreDealSetupWindow(uint8_t wintype);    //�������ô���Ԥ������������Ч��
extern void OperateSetupWindow(uint8_t wintype, uint8_t currwindow);    //�������ô��ڲ���
extern void CheckTouchSetupWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplaySetupMainWindow(uint8_t RefreshType);    //��ʾ����������
extern void PreDealSetupMainWindow(uint8_t wintype);    //����������Ԥ������������Ч��
extern void OperateSetupMainWindow(uint8_t wintype, uint8_t currwindow);    //���������ڲ���
extern void CheckTouchSetupMainWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void PlayInputErrorWav(void);
extern void PlayPassErrorWav(void);
extern void PlayModiSuccWav(void);
extern void PlayModiFailWav(void);

extern void DisplayKbWindow(uint8_t EditType);    //��ʾ���̴���
extern void PreDealKbWindow(uint8_t wintype);    //���̴���Ԥ������������Ч��
extern void OperateKbWindow(uint8_t wintype, uint8_t currwindow);    //���̴��ڲ���
extern void CheckTouchKbWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void WriteNullKb_Edit(uint8_t FullPageType);
extern uint8_t Kb_Edit_Deal(void);  //Kb �ı�����Ϣ����


extern void DisplayLanWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //LAN���ô���
extern void PreDealLanWindow(uint8_t wintype);    //LAN���ô���Ԥ������������Ч��
extern void OperateLanWindow(uint8_t wintype, uint8_t currwindow);    //LAN���ô��ڲ���
extern void CheckTouchLanWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplayAddrWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //�������ô���
extern void PreDealAddrWindow(uint8_t wintype);    //�������ô���Ԥ������������Ч��
extern void OperateAddrWindow(uint8_t wintype, uint8_t currwindow);    //�������ô��ڲ���
extern void CheckTouchAddrWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplayPassSetupWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //�޸����봰��
extern void PreDealPassSetupWindow(uint8_t wintype);    //�޸����봰��Ԥ������������Ч��
extern void OperatePassSetupWindow(uint8_t wintype, uint8_t currwindow);    //�޸����봰�ڲ���
extern void CheckTouchPassSetupWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplayScreenSetupWindow(uint8_t RefreshType);    //��Ļ���ڴ���
extern void PreDealScreenSetupWindow(uint8_t wintype);    //��Ļ���ڴ���Ԥ������������Ч��
extern void OperateScreenSetupWindow(uint8_t wintype, uint8_t currwindow);    //��Ļ���ڴ��ڲ���
extern void CheckTouchScreenSetupWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplayVoiceSetupWindow(uint8_t RefreshType);    //�������ڴ���
extern void PreDealVoiceSetupWindow(uint8_t wintype);    //�������ڴ���Ԥ������������Ч��
extern void OperateVoiceSetupWindow(uint8_t wintype, uint8_t currwindow);    //�������ڴ��ڲ���
extern void CheckTouchVoiceSetupWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void DisplayVersionWindow(uint8_t RefreshType);    //��ʾ�汾����
extern void PreDealVersionWindow(uint8_t wintype);    //�汾����Ԥ������������Ч��
extern void OperateVersionWindow(uint8_t wintype, uint8_t currwindow);    //�汾����
extern void CheckTouchVersionWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������
extern void RefreshVersionWindow(uint8_t FullPageType);    //ˢ�°汾��Ϣ����

extern void DisplayCalibrateWindow(void);    //У׼����������
extern void PreDealCalibrateWindow(uint8_t wintype);    //У׼����������Ԥ������������Ч��
extern void OperateCalibrateWindow(uint8_t wintype, uint8_t currwindow);  //У׼����������
extern void CheckTouchCalibrateWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern uint16_t Calib_X[4];
extern uint16_t Calib_Y[4];
extern uint16_t Calib_OldX[4];
extern uint16_t Calib_OldY[4];

extern void PonSearch_Result(void);
extern void PonWrite_Result(void);
#endif
