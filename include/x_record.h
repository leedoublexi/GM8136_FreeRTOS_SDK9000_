#ifndef x_record_h
#define x_record_h

extern void DisplayRecordsWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //显示通话记录窗口
extern void PreDealRecordsWindow(uint8_t wintype);    //通话记录窗口预处理，按键按下效果
extern void OperateRecordsWindow(uint8_t wintype, uint8_t currwindow);    //通话记录窗口操作
extern void CheckTouchRecordsWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

extern void DisplayRecordsContentWindow(uint8_t recordsno);    //通话记录查看信息窗口
extern void PreDealRecordsContentWindow(uint8_t wintype);    //查看通话记录窗口预处理，按键按下效果
extern void OperateRecordsContentWindow(uint8_t wintype, uint8_t currwindow);    //查看通话记录窗口操作
extern void CheckTouchRecordsContentWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作


extern uint8_t Init_RecordsVar(void); //初始化 Records 结构
extern uint8_t Uninit_RecordsVar(void); //释放 Records 结构
//读通话记录文件
extern void ReadRecordsFile(void);
//写通话记录文件
extern void WriteRecordsFile(void);

extern void ResetRecordsCfg(void);    //复位通话记录配置文件
extern void WriteRecordsCfgFile(void);     //写通话记录设置文件

extern void Recv_NewRecords(void);
//写照片函数
extern void WritePicFunc(void);
#endif