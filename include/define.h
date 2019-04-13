#ifndef define_H_
#define define_H_


#define _PON_SUPPORT   //PON 支持 20140314
#define _ROOM_SEARCH_SUPPORT      //室内机搜索 小门口机、4.3室内机

#define _XTM8900_SUPPORT  //XTM8900网卡支持
//#define _XTM9000_SUPPORT  //XTM9000网卡支持

#define VIDEO_VGA  0
#define VIDEO_QVGA  1

#define LOCAL_VIDEO   0
#define REMOTE_VIDEO  1
//#define _TCPM  //20150727 电容屏支持
#define _TCPM_70A                     //7寸电容屏 或者10寸电容屏切换  0902

//#define _REMOTECALLTEST          //远程呼叫测试
#define _ADDTABLE_SUPPORT      //地址表支持
#define _MULTI_MACHINE_SUPPORT   //多分机支持
//#define _CAMERA_SUPPORT        //如不用摄像头，GPIOB6、7为Comm1, GPIOC 作为8个防区使用   测试模式
//#define _SEND_TO_SELF_TEST     //发送数据包给自己
#define _REMOTE_FORTIFY_SUPPORT  //远程布撤防支持
#define _MULTI_CENTER_SUPPORT    //多中心机支持   20130613


#define ELEVATORCONTROL_HANGZHOU     //20120822
#ifdef ELEVATORCONTROL_HANGZHOU      //20120822
  #define ELEVATR_ORDER1  181
#endif

//Flash secter 删除每块为4K， 所以存储地址设为 4K的倍数
#define INFO_ADDR            0x7f8000   //信息存储地址   第一个 512 为配置  后20个512 为信息条
#define LOCALCFG_ADDR        0x7ff000   //配置文件存储地址
#define LOCALCFG_ADDR1       0x7fe000   //配置文件存储地址2  备份用

#define RECORDCFG_ADDR       0x7f0000   //通话记录存储地址   第一个 512 为配置  后20个512 为通话记录条
#define RECORDPIC_ADDR       0x570000   //通话记录相片存储地址   每64K 一张照片, 最多20张

#ifdef _ADDTABLE_SUPPORT  //地址表支持
 #define ADDRTABLE_ADDR       0x7A6000   //地址表   长度 0x4A000  到  0x7effff
 #define ADDR_MAXNUM          9000      //地址表最大数量
 #define READ_ADDRTABLE       230       //读地址表信息
#endif

#define FLAGTEXT   "xtm8130sdkkkkdfdsIM"
#define HARDWAREVER "S-HW VER 4.0.2 For 8130"    //硬件版本
#define SOFTWAREVER "S-SW70A VER 4.2.0"   //软件版本
#ifdef _TCPM  //20150727 电容屏支持
 #define SERIALNUM "20151117A70C"    //产品序列号
#else
 #define SERIALNUM "20151117A70"    //产品序列号
#endif

//#define _RGB


#define IMAGEUP    0
#define IMAGEDOWN  1

#define TOUCH_WRITE_ADDR     0x90      //触摸屏I2C地址 read:10010001 write 10010000
#define TOUCH_READ_ADDR      0x91      //触摸屏I2C地址 read:10010001 write 10010000

#define REFRESH    1
#define NOREFRESH  0

#define FB_PAGE0   0
#define FB_PAGE1   1
#define FULLPAGE   1
#define NOFULLPAGE 0
#define FB_PAGE2   2



#define COLOR_BLACK         1
#define COLOR_WHITE         2
#define COLOR_RED           3 //0xf800
#define COLOR_YELLOW        4 //0xFFE0
#define COLOR_GREEN         5 //0x07E0
#define COLOR_BLUE          6
#define COLOR_CYAN          7

#define SCREEN_WIDTH		800//1024//480//800            //屏幕宽度
#define SCREEN_HEIGHT		480//600//272//480            //屏幕高度
#define DISPLAY_SRC_WIDTH_MAX  SCREEN_WIDTH
#define DISPLAY_SRC_HEIGHT_MAX  SCREEN_HEIGHT

#define ALIGN(x, a)	        (((x) + (a) - 1) & ~((a) - 1))

#define AVAILABLE_MEM_START     (0x1600000ul)
#define GUI_BUF0                AVAILABLE_MEM_START//ALIGN((AVAILABLE_MEM_START, 256)
#define GUI_BUF1                (GUI_BUF0 + (DISPLAY_SRC_WIDTH_MAX * DISPLAY_SRC_HEIGHT_MAX) * 2)//ALIGN((AVAILABLE_MEM_START + (DISPLAY_SRC_WIDTH_MAX * DISPLAY_SRC_HEIGHT_MAX) * 2), 256)
#define GUI_BUF2                (GUI_BUF1 + (DISPLAY_SRC_WIDTH_MAX * DISPLAY_SRC_HEIGHT_MAX) * 2)

#define JPEG_DECODE_BUF0        (GUI_BUF2 + (DISPLAY_SRC_WIDTH_MAX * DISPLAY_SRC_HEIGHT_MAX) * 2)//ALIGN((GUI_BUF3 + (DISPLAY_SRC_WIDTH_MAX * DISPLAY_SRC_HEIGHT_MAX) * 2), 256)
#define MJPEG_STEAM_BUF  				(JPEG_DECODE_BUF0 + (DISPLAY_SRC_WIDTH_MAX * DISPLAY_SRC_HEIGHT_MAX) * 2)

#define MEMORY_STEAM_BUF0  		  (MJPEG_STEAM_BUF + 1024*1024)
#define AUDIO_READ_BUF  				(MEMORY_STEAM_BUF0 + 1024*1024)
#define AUDIO_RECORD_BUF  		  (AUDIO_READ_BUF + 1024*1024)
#define AUDIO_PLAY_BUF  				(AUDIO_RECORD_BUF + 1024*1024)

#define INFO_BUF  						  (AUDIO_PLAY_BUF + 1024*1024)

#define JPEG_MEMORY_ADDR        (0x3200000ul)

#define MAIN_PRIORITY           0
#define TIMER_PRIORITY          2
#define NORMAL_PRIORITY         3
#define MULTISEND_PRIORITY      4
#define UDPDATA_PRIORITY        4
#define UDPVIDEO_PRIORITY       5
#define UDPPON_PRIORITY         4
#define AUDIORECORD_PRIORITY    4
#define AUDIOPLAY_PRIORITY      4


// #define FORTIFY_SEC_IO0     0           //触摸屏GPIO0 0
// #define FORTIFY_IO0     1               //触摸屏GPIO0 1
// #define FORTIFY_IO1     2               //触摸屏GPIO0 2
// #define FORTIFY_IO2     3               //触摸屏GPIO0 3
// #define FORTIFY_IO3     4               //触摸屏GPIO0 4
// #define FORTIFY_IO4     28              //触摸屏GPIO0 28
// #define FORTIFY_IO5     29              //触摸屏GPIO0 29
// #define FORTIFY_IO6     30              //触摸屏GPIO0 30
// #define FORTIFY_IO7     31              //触摸屏GPIO0 31

#define FORTIFY_IO_CLK        1       // 
#define FORTIFY_IO_PL         2       //   
#define FORTIFY_IO_DATA       9       //


#define TOUCH_IO        17      //触摸屏GPIO0 17
#define ZT2083_TOUCH_IO 17      //触摸屏GPIO0 17
#define WATCHDOG_IO     57      //看门狗GPIO1 25
#define LCDPOWER_IO     60      //PWM0  GPIO1 28
#define NET_IO          61      //PWM0  GPIO1 29

#define PON_PWR_IO      28      //PON_POW_EN GPIO0 28

#define CHECK_PON_STATUS1_IO 30 //check pon status led1 GPIO0 30
#define CHECK_PON_STATUS2_IO 31 //check pon status led2 GPIO031


#define ZT2083_SCR   0
#define GT911_SCR    0
#define ICN85XX_SCR  1
#define GSLX680_SCR  0
#define FT5X0X_SCR   0

//#define   TOUCH_GT911_ENABLE
#define   TOUCH_ICN85XX_ENABLE
//#define   TOUCH_GSLX680_ENABLE
//#define   TOUCH_FT5X0X_ENABLE


#define DELTACALIB      50   //触摸屏校准十字位置

#define REFRESH_TOP     0x01
#define REFRESH_MIDDLE  0x02
#define REFRESH_BOTTOM  0x04

#define MAXCOUNT       8      //防区最大数量
#define SUBMAXNUM      3      //副机最大数量
#define MAXDEFENCENUM  8      //最大防区数量
#define UDPPACKMAX     1200
#define UDPSENDMAX     50     //UDP多次发送缓冲最大值
#define AUDIOSENDMAX   10     //Audio UDP多次发送缓冲最大值
#define COMMSENDMAX    10     //COMM多次发送缓冲最大值
#define MAXSENDNUM     6      //最大发送次数
#define MAXVIDEOBUF    2      //最大视频缓冲
#define MAXPACKNUM     30     //帧最大数据包数量

#define TIMERTIME      100    //500 ms
#define TIMERPERSEC    2      //每秒2次线程
#define TIMER2TIME     1      //5 ms
#define GPIOTIME       4      //20 ms  GPIO检测
#define GPIOPERSEC     50     //每秒50次线程

#define IDLE_TIME      30     //空闲返回主界面时间
#define CLOSELCD_TIME  60//10 //60      //待机关LCD时间
#define POWERSTAY_TIME 30//500 // 30      //关LCD后 进入低功耗时间

#define NO_CYCLE_PLAY   0   //音频单次播放
#define CYCLE_PLAY      1   //音频循环播放
#define KEYDELAYTIME  120   //按钮压下时间  150ms


//短信息
#define INFOMAXITEM   20  //短信息最大条数
#define INFOMAXSIZE   402 //短信息内容最大容量
#define INFONUMPERPAGE  5  //一页显示信息数

#define INFOROWLEN    44    //信息每行长度
#define INFOROWPIXEL  528    //信息每行像素　44*12
#define MAXROW  12          //最大行数
#define TOTALPAGE  3          //总页数
#define PAGEPERROW  4          //页行数
#define VERSIONPERROW  10          //页行数

//通话记录
#define RECORDMAXITEM   20  //通话记录最大条数
#define RECORDNUMPERPAGE  5  //一页显示通话记录数

#define NSMULTIADDR  "238.9.9.1"
#define DIRECTCALLTIME  600                 //直接呼叫每次时间
#define WATCHTIMEOUT  (30*TIMERPERSEC)    //(65500*TIMERPERSEC) //监视最长时间   600
#define CALLTIMEOUT  (30*TIMERPERSEC)     //呼叫最长时间   250
#define TALKTIMEOUT  (120*TIMERPERSEC)    //130  30*20   //通话最长时间      600

#define LANGUAGEMAXNUM   2   //最大语言

//命令 管理中心
#define ALARM         1    //报警
#define CANCELALARM   2    //取消报警
#define SENDMESSAGE   3   //发送信息
#define REPORTSTATUS  4   //设备定时报告状态
#define QUERYSTATUS   5   //管理中心查询设备状态
#define REMOTEDEFENCE   20   //远程布防
#define RESETPASS       30   //复位密码
#define SEARCHALLEQUIP  252  //搜索所有设备（管理中心－＞设备）
#define WRITEEQUIPADDR      254  //写设备地址（管理中心－＞设备）

//对讲
#define VIDEOTALK      150 //局域网可视对讲
#define VIDEOWATCH     152 //局域网监控
#define NSORDER        154 //主机名解析（子网内广播）
#define NSSERVERORDER  155 //主机名解析(NS服务器)
#define FINDEQUIP      170 //查找设备
#define ASK              1     //命令类型 主叫
#define REPLY            2     //命令类型 应答
#define SINGLE_SEND           1   //单次
#define MULTI_SEND            2     //多次

#define CALL             1     //呼叫
#define LINEUSE          2     //占线
#define QUERYFAIL        3      //通信失败
#define CALLANSWER       4     //呼叫应答
#define CALLSTART        6     //开始通话

#define CALLUP           7     //通话数据1（主叫方->被叫方）
#define CALLDOWN         8     //通话数据2（被叫方->主叫方）
#define CALLCONFIRM      9     //通话在线确认（接收方发送，以便发送方确认在线）
#define REMOTEOPENLOCK   10     //远程开锁

#define JOINGROUP        22     //加入组播组（主叫方->被叫方，被叫方应答）
#define LEAVEGROUP       23     //退出组播组（主叫方->被叫方，被叫方应答）
#define TURNTALK         24     //转接（被叫方->主叫方，主叫方应答）
#define CENTERTURNTALK         25     //中心人工转接
#define TRUSTEESHIP_TALK  26    //托管

#define CALLEND          30     //通话结束

#ifdef _REMOTE_FORTIFY_SUPPORT  //远程布撤防支持
 #define REMOTE_FORTIFY   160    //远程布撤防
#endif

#ifdef _MULTI_MACHINE_SUPPORT  //多分机支持
 #define HOSTTOSUB  222 //主机与副机通信
 #define SYNCSUB        1   //同步状态（主机－＞副机）
 #define SUBDEFENCE     2   //布防（副机－＞主机）
 #define SUBALARM       3   //报警（副机－＞主机）
 #define SUBALARMTIP    5   //报警提示（主机－＞副机）
 #define SUBDEFENCETIP    6   //布防提示（主机－＞副机）
 #define SUBCONTROL        10    //家电控制（副机－＞主机）
 #define SUBCONTROLTIP     11    //家电控制提示（主机－＞副机）
 #define SUBFIND        255   //查找副机（主机－＞副机）

 #define SUBFIND_START_NUM  4  //查找副机 发送次数   6 - 4 = 2
#endif

#define UPLOADFILE  225    //上传
#define UPLOADYUV  227    //上传YUV
#define READFLASH  228    //读FLASH内容
#define REMOTERESET  229    //远程重启动

#define UDP_TEST  245

#define DOWNLOADFILE  224    //下载
#define STARTDOWN  1       //开始下载一个
#define DOWN       2       //下载
#define DOWNFINISHONE       3  //下载完成一个
#define STOPDOWN       10      //停止下载
#define DOWNFAIL         21 //下载失败  设备－》管理中心

#define REMOTEDEBUGINFO      253   //发送远程调试信息
#define STARTDEBUG  1    //开始
#define STOPDEBUG  0    //停止
#define DEBUGDATA  2    //调试数据
#define REMOTE_REBOOT  3    //重启动

#ifdef _REMOTECALLTEST  //远程呼叫测试
 #define REMOTETEST      200   //发送远程呼叫测试
 #define STARTTEST  1    //开始
 #define ENTERTESTMODE  2    //进入测试模式
 #define STOPCALL       3    //停止呼叫通话 
#endif

#define SEND_PCM  226      //发送PCM数据
#define _REMOTEWRITEOV7725REGISTER   //远程写OV7725寄存器  调试用
#ifdef _REMOTEWRITEOV7725REGISTER   //远程写OV7725寄存器  调试用
 #define OV7725_OPERATE   227       //读写OV7725
 #define OV7725READREGISTER  1
 #define OV7725WRITEREGISTER 2
#endif

#ifdef _SEND_TO_SELF_TEST    //发送数据包给自己
 #define SEND_SELF  251
#endif

#define _PACK_1024

#ifdef _ROOM_SEARCH_SUPPORT 	 //室内机搜索 小门口机、4.3室内机
 UINT8 RoomSearchFlag;	  //0 空闲	1 搜索	2 写入
 UINT8 RoomSearchTime; 
 #define MAXCARDNUM	   50			   //最大卡数
 struct RoomSearch1 RoomSearch;
 struct RoomSearch1 RoomSearchWrite;
 
	
#endif


#ifdef _PON_SUPPORT
   #define PORT_NUM  3
   #define PONSEARCHALLEQUIP  240  //搜索所有设备（管理中心－＞设备）
   #define PONWRITEEQUIPADDR      241  //写设备地址（管理中心－＞设备）
  
#else
  #define PORT_NUM  2
#endif

#define SMALLESTSIZE  18  //512  //UDP最小包大小    20121102 512 -- 18 , MAC最小为60
#define PACKDATALEN  1200   //数据包大小

#define MAINRETURNTYPE  0
#define TALKWATCHRETURNTYPE  1
#define TALKWINTYPE  0
#define WATCHWINTYPE  1

#define _TEST_BTN_NUM  31

#define SET_LOW             0x0
#define SET_HIGH            0x1

#define FORTIFY_LED_HIGH      0x20    //GPIOD5
#define FORTIFY_LED_LOW       0xdf
#define INFO_LED_HIGH         0x40    //GPIOD6
#define INFO_LED_LOW          0xbf
#define NET_LED_HIGH       0x80    //GPIOD7
#define NET_LED_LOW        0x7f
#define ALARM_OUT_HIGH     0x20
#define ALARM_OUT_LOW      0xdf

#define memcpy  memcpy

#ifdef _ROOM_SEARCH_SUPPORT      //室内机搜索 小门口机、4.3室内机
struct RoomSearch1{
               char Addr[20];             //地址编码
               char AddrVerifyMa[10];         //主副机同步码
               UINT8 Mac_Addr[6]; //网卡地址
               UINT8 IP[4];       //IP地址
               UINT8 IP_Mask[4];  //子网掩码
               UINT8 IP_Gate[4];  //网关地址
               UINT8 IP_Server[4];  //主服务器地址（与NS服务器可为同一个）

               char EngineerPass[10];             //工程密码

               UINT8 MultiMachineSupport;   //多分机支持
               UINT8 DenMaxNum;             //分机最大数量
               UINT8 Language;              //语言
               UINT8 PassOpenLock;          //密码开锁
               UINT8 CardOpenLock;         //刷卡开锁
               UINT8 DoorButton;            //出门按钮
               UINT8 DoorDetect;            //门磁检测
               UINT8 OpenLockTime ;         //开锁时间  0 500ms  1 3s  3 5s
               UINT8 DelayLockTime;         //延时开锁  0 0s  1 3s  2 5s  3 10s

               UINT8 MicDecValue; //展箱、展板模式,需减小 Mic音量
               UINT8 CallDest;    //呼叫目标  0 -- 室内   1 -- 中心
               UINT8 EncodeFrameRate; //编码帧率

               UINT8 villa_ext_dev_type;     //二次门口机外设类型     0 -- 键盘   1 -- 刷卡
               UINT8 villa_ext_dev_function; //二次门口机外设功能  0 -- 开锁   1 -- 布撤防

               UINT32 CardNum;
               UINT8 SN[MAXCARDNUM*4];
              };
#endif

#endif
