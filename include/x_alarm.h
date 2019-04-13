#ifndef x_alarm_h
#define x_alarm_h

extern void DisplayCancelFortifyWindow(uint8_t RefreshType);    //显示撤防窗口
extern void PreDealCancelFortifyWindow(uint8_t wintype);    //撤防窗口预处理，按键按下效果
extern void OperateCancelFortifyWindow(uint8_t wintype, uint8_t currwindow);    //撤防窗口操作
extern void CheckTouchCancelFortifyWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作
extern void CancelFortifyDeal(void); //撤防界面处理
extern void PlayCancelFortifyWav(void);
extern void PlayDefenceDelayWav(void); //播放 布防延时中 提示

extern void PlayCancelAlarmWav(void);

extern void DisplayFortifyWindow(uint8_t RefreshType);    //显示布防窗口
extern void PreDealFortifyWindow(uint8_t wintype);    //布防窗口预处理，按键按下效果
extern void OperateFortifyWindow(uint8_t wintype, uint8_t currwindow);    //布防窗口操作
extern void CheckTouchFortifyWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplayAlarmWindow(uint8_t RefreshType);    //显示报警窗口
extern void PreDealAlarmWindow(uint8_t wintype);    //报警窗口预处理，按键按下效果
extern void OperateAlarmWindow(uint8_t wintype, uint8_t currwindow);    //报警窗口操作
extern void CheckTouchAlarmWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void ShowAlarmImage(void);

extern void DisplayCancelAlarmWindow(uint8_t RefreshType);    //显示取消报警窗口
extern void PreDealCancelAlarmWindow(uint8_t wintype);    //取消报警窗口预处理，按键按下效果
extern void OperateCancelAlarmWindow(uint8_t wintype, uint8_t currwindow);    //取消报警窗口操作
extern void CheckTouchCancelAlarmWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作
extern void CancelAlarmDeal(void); //取消报警界面处理

extern void CheckDefenceDelay_Func(void); //检查布防延时函数
extern void FortifyFinishDeal(void); //布防完成界面处理
extern void CheckDelayAlarm(void);  //检查延时报警函数
extern void SendAlarmFunc(uint8_t SpecialByte, uint8_t AlarmByte[2]); //防区报警函数
extern void SendCancelAlarmFunc(void); //取消报警函数

extern void InitDefence(void);
extern void CheckDefence(unsigned char DefenceByte);    //检测防区函数
#endif