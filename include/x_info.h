#ifndef x_info_h
#define x_info_h

extern void DisplayInfoWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //显示信息窗口
extern void PreDealInfoWindow(uint8_t wintype);    //信息窗口预处理，按键按下效果
extern void OperateInfoWindow(uint8_t wintype, uint8_t currwindow);    //信息窗口操作
extern void CheckTouchInfoWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplayInfoContentWindow(uint8_t infotype, uint8_t infono);    //显示查看信息窗口
extern void PreDealInfoContentWindow(uint8_t wintype);    //查看信息窗口预处理，按键按下效果
extern void OperateInfoContentWindow(uint8_t wintype, uint8_t currwindow);    //查看信息窗口操作
extern void CheckTouchInfoContentWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern uint8_t Init_InfoVar(void); //初始化 Info 结构
extern uint8_t Uninit_InfoVar(void); //释放 Info 结构
//读信息文件
extern void ReadInfoFile(void);
//写信息文件
extern void WriteInfoFile(void);

extern void ResetInfoCfg(void);    //复位信息配置文件
extern void WriteInfoCfgFile(void);     //写信息设置文件

extern void Recv_NewInfo(void);
#endif