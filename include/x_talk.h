#ifndef x_talk_h
#define x_talk_h

extern void Talk_Func(void);       //通话 接听
extern void CallCenter_Func(char *CenterAddr);  //呼叫中心
extern void CallR2R_Func(uint8_t *DestAddr);    //户户对讲
extern void Watch_Func(void);       //监视
extern void TalkEnd_Func(void);
extern void WatchEnd_Func(void);
extern void CallTimeOut_Func(void); //呼叫超时
extern void OpenLock_Func(void);  //开锁

extern void CreateTalkBuff(uint8_t *sendb, uint8_t Order1, uint8_t Order2, uint8_t Order3, uint8_t *Remote_Addr, uint8_t *Remote_IP);

extern void CreateNSBuff(uint8_t *sendb, uint8_t Order1, uint8_t Order2);

extern void DisplayTalkWindow(uint8_t RefreshType);    //显示对讲呼出窗口
extern void PreDealTalkWindow(uint8_t wintype);    //对讲呼出窗口预处理，按键按下效果
extern void OperateTalkWindow(uint8_t wintype, uint8_t currwindow);    //对讲呼出窗口操作
extern void CheckTouchTalkWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作


extern void DisplayWatchWindow(uint8_t RefreshType);    //显示监视窗口
extern void PreDealWatchWindow(uint8_t wintype);    //监视窗口预处理，按键按下效果
extern void OperateWatchWindow(uint8_t wintype, uint8_t currwindow);    //监视窗口操作
extern void CheckTouchWatchWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作


extern void DisplayTalkWatchPicWindow(uint8_t ReturnType, uint8_t WinType);    //显示对讲呼入窗口
extern void PreDealTalkWatchPicWindow(uint8_t wintype);    //对讲呼入窗口预处理，按键按下效果
extern void OperateTalkWatchPicWindow(uint8_t wintype, uint8_t currwindow);    //对讲呼入窗口操作
extern void CheckTouchTalkWatchPicWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern struct TalkCall1 TalkCall;
#endif