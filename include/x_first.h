#ifndef x_first_h
#define x_first_h

extern void InterfaceInit(void);

extern void DisplayFirstWindow(void);    //��ʾ������
extern void PreDealFirstWindow(uint8_t wintype);    //������Ԥ������������Ч��
extern void OperateFirstWindow(uint8_t wintype, uint8_t currwindow);    //�����ڲ���
extern void CheckTouchFirstWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern void CheckFirstWindowFlag(void);  //��鷵����ҳ��־

extern void RefreshBackWindow(uint8_t RefreshType, uint8_t FullPageType); //ˢ�±���
extern void DisplayStateBar(void);      //״̬��
extern void PlaySoundTip(void); //������ʾ��

extern void PreDealBigmenuButton(uint8_t wintype);  //��ݰ��� Ԥ����
extern void ShowPressQuickmenu_Button(uint8_t wintype);  //��ݰ���   ����
extern void OperateQuickmenu(uint8_t wintype, uint8_t currtype);

extern void DisplayFortifyStateImage(uint8_t State);    //��ʾ����ͼ��
extern void DisplayInfoStateImage(uint8_t State);    //��ʾ��Ϣͼ��
extern void DisplayPictureStateImage(uint8_t State);    //��ʾ��Ӱͼ��

//��lib ����
extern uint32_t GetLocal_fb_addr(void);
extern uint8_t GetLocal_fb_page(void);
extern uint32_t GetLocal_fb1_addr(void);

extern void Set_Local_DecodeJpegFlag(uint8_t disp_DecodeJpegFlag);
extern uint8_t Get_Local_DecodeStreamFlag(void);
#endif