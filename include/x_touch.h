#ifndef x_touch_h
#define x_touch_h

extern uint8_t CheckTouchDelayTime(void);  //����������ʱ����ӳ�

extern void TouchScr_Func(uint8_t ScrType, uint16_t touch_x, uint16_t touch_y); //����������
extern uint8_t CheckTSInButton(struct TImageButton *t_button, uint16_t XLCD, uint16_t YLCD); //��鴥�����Ƿ��ڸð�ť��
extern uint8_t CheckTSInEdit(struct TEdit *t_edit, uint16_t XLCD, uint16_t YLCD); //��鴥�����Ƿ��ڸ�Edit��
extern uint8_t CheckTSInImage(struct TImage *t_image, uint16_t XLCD, uint16_t YLCD); //��鴥�����Ƿ��ڸ�Image��
extern uint8_t CheckTSInVideoScreen(uint16_t XLCD, uint16_t YLCD); //��鴥�����Ƿ��ڸ���Ƶ������
extern uint8_t CheckTSInPicScreen(uint16_t XLCD, uint16_t YLCD); //��鴥�����Ƿ��ڸ���Ƭ������
extern uint8_t CheckBigmenuFunc(uint16_t XLCD, uint16_t YLCD);  //��ݰ�ť����
#endif