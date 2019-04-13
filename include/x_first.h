#ifndef x_first_h
#define x_first_h

extern void InterfaceInit(void);

extern void DisplayFirstWindow(void);    //显示主窗口
extern void PreDealFirstWindow(uint8_t wintype);    //主窗口预处理，按键按下效果
extern void OperateFirstWindow(uint8_t wintype, uint8_t currwindow);    //主窗口操作
extern void CheckTouchFirstWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void CheckFirstWindowFlag(void);  //检查返回首页标志

extern void RefreshBackWindow(uint8_t RefreshType, uint8_t FullPageType); //刷新背景
extern void DisplayStateBar(void);      //状态条
extern void PlaySoundTip(void); //播放提示音

extern void PreDealBigmenuButton(uint8_t wintype);  //快捷按键 预处理
extern void ShowPressQuickmenu_Button(uint8_t wintype);  //快捷按键   弹起
extern void OperateQuickmenu(uint8_t wintype, uint8_t currtype);

extern void DisplayFortifyStateImage(uint8_t State);    //显示布防图标
extern void DisplayInfoStateImage(uint8_t State);    //显示信息图标
extern void DisplayPictureStateImage(uint8_t State);    //显示留影图标

//供lib 调用
extern uint32_t GetLocal_fb_addr(void);
extern uint8_t GetLocal_fb_page(void);
extern uint32_t GetLocal_fb1_addr(void);

extern void Set_Local_DecodeJpegFlag(uint8_t disp_DecodeJpegFlag);
extern uint8_t Get_Local_DecodeStreamFlag(void);
#endif