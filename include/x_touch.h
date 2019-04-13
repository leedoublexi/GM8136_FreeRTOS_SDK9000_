#ifndef x_touch_h
#define x_touch_h

extern uint8_t CheckTouchDelayTime(void);  //触摸屏处理时检查延迟

extern void TouchScr_Func(uint8_t ScrType, uint16_t touch_x, uint16_t touch_y); //触摸屏输入
extern uint8_t CheckTSInButton(struct TImageButton *t_button, uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该按钮内
extern uint8_t CheckTSInEdit(struct TEdit *t_edit, uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该Edit内
extern uint8_t CheckTSInImage(struct TImage *t_image, uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该Image内
extern uint8_t CheckTSInVideoScreen(uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该视频窗口内
extern uint8_t CheckTSInPicScreen(uint16_t XLCD, uint16_t YLCD); //检查触摸点是否在该照片窗口内
extern uint8_t CheckBigmenuFunc(uint16_t XLCD, uint16_t YLCD);  //快捷按钮操作
#endif