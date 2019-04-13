#ifndef x_setup_h
#define x_setup_h


extern void *xtm_memcopy(void *dest, void *src, UINT16 n);
extern void DisplaySetupWindow(uint8_t RefreshType);    //显示进入设置窗口
extern void PreDealSetupWindow(uint8_t wintype);    //进入设置窗口预处理，按键按下效果
extern void OperateSetupWindow(uint8_t wintype, uint8_t currwindow);    //进入设置窗口操作
extern void CheckTouchSetupWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplaySetupMainWindow(uint8_t RefreshType);    //显示设置主窗口
extern void PreDealSetupMainWindow(uint8_t wintype);    //设置主窗口预处理，按键按下效果
extern void OperateSetupMainWindow(uint8_t wintype, uint8_t currwindow);    //设置主窗口操作
extern void CheckTouchSetupMainWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void PlayInputErrorWav(void);
extern void PlayPassErrorWav(void);
extern void PlayModiSuccWav(void);
extern void PlayModiFailWav(void);

extern void DisplayKbWindow(uint8_t EditType);    //显示键盘窗口
extern void PreDealKbWindow(uint8_t wintype);    //键盘窗口预处理，按键按下效果
extern void OperateKbWindow(uint8_t wintype, uint8_t currwindow);    //键盘窗口操作
extern void CheckTouchKbWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void WriteNullKb_Edit(uint8_t FullPageType);
extern uint8_t Kb_Edit_Deal(void);  //Kb 文本框信息处理


extern void DisplayLanWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //LAN设置窗口
extern void PreDealLanWindow(uint8_t wintype);    //LAN设置窗口预处理，按键按下效果
extern void OperateLanWindow(uint8_t wintype, uint8_t currwindow);    //LAN设置窗口操作
extern void CheckTouchLanWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplayAddrWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //房间设置窗口
extern void PreDealAddrWindow(uint8_t wintype);    //房间设置窗口预处理，按键按下效果
extern void OperateAddrWindow(uint8_t wintype, uint8_t currwindow);    //房间设置窗口操作
extern void CheckTouchAddrWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplayPassSetupWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //修改密码窗口
extern void PreDealPassSetupWindow(uint8_t wintype);    //修改密码窗口预处理，按键按下效果
extern void OperatePassSetupWindow(uint8_t wintype, uint8_t currwindow);    //修改密码窗口操作
extern void CheckTouchPassSetupWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplayScreenSetupWindow(uint8_t RefreshType);    //屏幕调节窗口
extern void PreDealScreenSetupWindow(uint8_t wintype);    //屏幕调节窗口预处理，按键按下效果
extern void OperateScreenSetupWindow(uint8_t wintype, uint8_t currwindow);    //屏幕调节窗口操作
extern void CheckTouchScreenSetupWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplayVoiceSetupWindow(uint8_t RefreshType);    //音量调节窗口
extern void PreDealVoiceSetupWindow(uint8_t wintype);    //音量调节窗口预处理，按键按下效果
extern void OperateVoiceSetupWindow(uint8_t wintype, uint8_t currwindow);    //音量调节窗口操作
extern void CheckTouchVoiceSetupWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplayVersionWindow(uint8_t RefreshType);    //显示版本窗口
extern void PreDealVersionWindow(uint8_t wintype);    //版本窗口预处理，按键按下效果
extern void OperateVersionWindow(uint8_t wintype, uint8_t currwindow);    //版本操作
extern void CheckTouchVersionWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作
extern void RefreshVersionWindow(uint8_t FullPageType);    //刷新版本信息窗口

extern void DisplayCalibrateWindow(void);    //校准触摸屏窗口
extern void PreDealCalibrateWindow(uint8_t wintype);    //校准触摸屏窗口预处理，按键按下效果
extern void OperateCalibrateWindow(uint8_t wintype, uint8_t currwindow);  //校准触摸屏操作
extern void CheckTouchCalibrateWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern uint16_t Calib_X[4];
extern uint16_t Calib_Y[4];
extern uint16_t Calib_OldX[4];
extern uint16_t Calib_OldY[4];

extern void PonSearch_Result(void);
extern void PonWrite_Result(void);
#endif
