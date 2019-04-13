#ifndef define_H_
#define define_H_


#define _PON_SUPPORT   //PON ֧�� 20140314
#define _ROOM_SEARCH_SUPPORT      //���ڻ����� С�ſڻ���4.3���ڻ�

#define _XTM8900_SUPPORT  //XTM8900����֧��
//#define _XTM9000_SUPPORT  //XTM9000����֧��

#define VIDEO_VGA  0
#define VIDEO_QVGA  1

#define LOCAL_VIDEO   0
#define REMOTE_VIDEO  1
//#define _TCPM  //20150727 ������֧��
#define _TCPM_70A                     //7������� ����10��������л�  0902

//#define _REMOTECALLTEST          //Զ�̺��в���
#define _ADDTABLE_SUPPORT      //��ַ��֧��
#define _MULTI_MACHINE_SUPPORT   //��ֻ�֧��
//#define _CAMERA_SUPPORT        //�粻������ͷ��GPIOB6��7ΪComm1, GPIOC ��Ϊ8������ʹ��   ����ģʽ
//#define _SEND_TO_SELF_TEST     //�������ݰ����Լ�
#define _REMOTE_FORTIFY_SUPPORT  //Զ�̲�����֧��
#define _MULTI_CENTER_SUPPORT    //�����Ļ�֧��   20130613


#define ELEVATORCONTROL_HANGZHOU     //20120822
#ifdef ELEVATORCONTROL_HANGZHOU      //20120822
  #define ELEVATR_ORDER1  181
#endif

//Flash secter ɾ��ÿ��Ϊ4K�� ���Դ洢��ַ��Ϊ 4K�ı���
#define INFO_ADDR            0x7f8000   //��Ϣ�洢��ַ   ��һ�� 512 Ϊ����  ��20��512 Ϊ��Ϣ��
#define LOCALCFG_ADDR        0x7ff000   //�����ļ��洢��ַ
#define LOCALCFG_ADDR1       0x7fe000   //�����ļ��洢��ַ2  ������

#define RECORDCFG_ADDR       0x7f0000   //ͨ����¼�洢��ַ   ��һ�� 512 Ϊ����  ��20��512 Ϊͨ����¼��
#define RECORDPIC_ADDR       0x570000   //ͨ����¼��Ƭ�洢��ַ   ÿ64K һ����Ƭ, ���20��

#ifdef _ADDTABLE_SUPPORT  //��ַ��֧��
 #define ADDRTABLE_ADDR       0x7A6000   //��ַ��   ���� 0x4A000  ��  0x7effff
 #define ADDR_MAXNUM          9000      //��ַ���������
 #define READ_ADDRTABLE       230       //����ַ����Ϣ
#endif

#define FLAGTEXT   "xtm8130sdkkkkdfdsIM"
#define HARDWAREVER "S-HW VER 4.0.2 For 8130"    //Ӳ���汾
#define SOFTWAREVER "S-SW70A VER 4.2.0"   //����汾
#ifdef _TCPM  //20150727 ������֧��
 #define SERIALNUM "20151117A70C"    //��Ʒ���к�
#else
 #define SERIALNUM "20151117A70"    //��Ʒ���к�
#endif

//#define _RGB


#define IMAGEUP    0
#define IMAGEDOWN  1

#define TOUCH_WRITE_ADDR     0x90      //������I2C��ַ read:10010001 write 10010000
#define TOUCH_READ_ADDR      0x91      //������I2C��ַ read:10010001 write 10010000

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

#define SCREEN_WIDTH		800//1024//480//800            //��Ļ���
#define SCREEN_HEIGHT		480//600//272//480            //��Ļ�߶�
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


// #define FORTIFY_SEC_IO0     0           //������GPIO0 0
// #define FORTIFY_IO0     1               //������GPIO0 1
// #define FORTIFY_IO1     2               //������GPIO0 2
// #define FORTIFY_IO2     3               //������GPIO0 3
// #define FORTIFY_IO3     4               //������GPIO0 4
// #define FORTIFY_IO4     28              //������GPIO0 28
// #define FORTIFY_IO5     29              //������GPIO0 29
// #define FORTIFY_IO6     30              //������GPIO0 30
// #define FORTIFY_IO7     31              //������GPIO0 31

#define FORTIFY_IO_CLK        1       // 
#define FORTIFY_IO_PL         2       //   
#define FORTIFY_IO_DATA       9       //


#define TOUCH_IO        17      //������GPIO0 17
#define ZT2083_TOUCH_IO 17      //������GPIO0 17
#define WATCHDOG_IO     57      //���Ź�GPIO1 25
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


#define DELTACALIB      50   //������У׼ʮ��λ��

#define REFRESH_TOP     0x01
#define REFRESH_MIDDLE  0x02
#define REFRESH_BOTTOM  0x04

#define MAXCOUNT       8      //�����������
#define SUBMAXNUM      3      //�����������
#define MAXDEFENCENUM  8      //����������
#define UDPPACKMAX     1200
#define UDPSENDMAX     50     //UDP��η��ͻ������ֵ
#define AUDIOSENDMAX   10     //Audio UDP��η��ͻ������ֵ
#define COMMSENDMAX    10     //COMM��η��ͻ������ֵ
#define MAXSENDNUM     6      //����ʹ���
#define MAXVIDEOBUF    2      //�����Ƶ����
#define MAXPACKNUM     30     //֡������ݰ�����

#define TIMERTIME      100    //500 ms
#define TIMERPERSEC    2      //ÿ��2���߳�
#define TIMER2TIME     1      //5 ms
#define GPIOTIME       4      //20 ms  GPIO���
#define GPIOPERSEC     50     //ÿ��50���߳�

#define IDLE_TIME      30     //���з���������ʱ��
#define CLOSELCD_TIME  60//10 //60      //������LCDʱ��
#define POWERSTAY_TIME 30//500 // 30      //��LCD�� ����͹���ʱ��

#define NO_CYCLE_PLAY   0   //��Ƶ���β���
#define CYCLE_PLAY      1   //��Ƶѭ������
#define KEYDELAYTIME  120   //��ťѹ��ʱ��  150ms


//����Ϣ
#define INFOMAXITEM   20  //����Ϣ�������
#define INFOMAXSIZE   402 //����Ϣ�����������
#define INFONUMPERPAGE  5  //һҳ��ʾ��Ϣ��

#define INFOROWLEN    44    //��Ϣÿ�г���
#define INFOROWPIXEL  528    //��Ϣÿ�����ء�44*12
#define MAXROW  12          //�������
#define TOTALPAGE  3          //��ҳ��
#define PAGEPERROW  4          //ҳ����
#define VERSIONPERROW  10          //ҳ����

//ͨ����¼
#define RECORDMAXITEM   20  //ͨ����¼�������
#define RECORDNUMPERPAGE  5  //һҳ��ʾͨ����¼��

#define NSMULTIADDR  "238.9.9.1"
#define DIRECTCALLTIME  600                 //ֱ�Ӻ���ÿ��ʱ��
#define WATCHTIMEOUT  (30*TIMERPERSEC)    //(65500*TIMERPERSEC) //�����ʱ��   600
#define CALLTIMEOUT  (30*TIMERPERSEC)     //�����ʱ��   250
#define TALKTIMEOUT  (120*TIMERPERSEC)    //130  30*20   //ͨ���ʱ��      600

#define LANGUAGEMAXNUM   2   //�������

//���� ��������
#define ALARM         1    //����
#define CANCELALARM   2    //ȡ������
#define SENDMESSAGE   3   //������Ϣ
#define REPORTSTATUS  4   //�豸��ʱ����״̬
#define QUERYSTATUS   5   //�������Ĳ�ѯ�豸״̬
#define REMOTEDEFENCE   20   //Զ�̲���
#define RESETPASS       30   //��λ����
#define SEARCHALLEQUIP  252  //���������豸���������ģ����豸��
#define WRITEEQUIPADDR      254  //д�豸��ַ���������ģ����豸��

//�Խ�
#define VIDEOTALK      150 //���������ӶԽ�
#define VIDEOWATCH     152 //���������
#define NSORDER        154 //�����������������ڹ㲥��
#define NSSERVERORDER  155 //����������(NS������)
#define FINDEQUIP      170 //�����豸
#define ASK              1     //�������� ����
#define REPLY            2     //�������� Ӧ��
#define SINGLE_SEND           1   //����
#define MULTI_SEND            2     //���

#define CALL             1     //����
#define LINEUSE          2     //ռ��
#define QUERYFAIL        3      //ͨ��ʧ��
#define CALLANSWER       4     //����Ӧ��
#define CALLSTART        6     //��ʼͨ��

#define CALLUP           7     //ͨ������1�����з�->���з���
#define CALLDOWN         8     //ͨ������2�����з�->���з���
#define CALLCONFIRM      9     //ͨ������ȷ�ϣ����շ����ͣ��Ա㷢�ͷ�ȷ�����ߣ�
#define REMOTEOPENLOCK   10     //Զ�̿���

#define JOINGROUP        22     //�����鲥�飨���з�->���з������з�Ӧ��
#define LEAVEGROUP       23     //�˳��鲥�飨���з�->���з������з�Ӧ��
#define TURNTALK         24     //ת�ӣ����з�->���з������з�Ӧ��
#define CENTERTURNTALK         25     //�����˹�ת��
#define TRUSTEESHIP_TALK  26    //�й�

#define CALLEND          30     //ͨ������

#ifdef _REMOTE_FORTIFY_SUPPORT  //Զ�̲�����֧��
 #define REMOTE_FORTIFY   160    //Զ�̲�����
#endif

#ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
 #define HOSTTOSUB  222 //�����븱��ͨ��
 #define SYNCSUB        1   //ͬ��״̬����������������
 #define SUBDEFENCE     2   //��������������������
 #define SUBALARM       3   //��������������������
 #define SUBALARMTIP    5   //������ʾ����������������
 #define SUBDEFENCETIP    6   //������ʾ����������������
 #define SUBCONTROL        10    //�ҵ���ƣ���������������
 #define SUBCONTROLTIP     11    //�ҵ������ʾ����������������
 #define SUBFIND        255   //���Ҹ�������������������

 #define SUBFIND_START_NUM  4  //���Ҹ��� ���ʹ���   6 - 4 = 2
#endif

#define UPLOADFILE  225    //�ϴ�
#define UPLOADYUV  227    //�ϴ�YUV
#define READFLASH  228    //��FLASH����
#define REMOTERESET  229    //Զ��������

#define UDP_TEST  245

#define DOWNLOADFILE  224    //����
#define STARTDOWN  1       //��ʼ����һ��
#define DOWN       2       //����
#define DOWNFINISHONE       3  //�������һ��
#define STOPDOWN       10      //ֹͣ����
#define DOWNFAIL         21 //����ʧ��  �豸������������

#define REMOTEDEBUGINFO      253   //����Զ�̵�����Ϣ
#define STARTDEBUG  1    //��ʼ
#define STOPDEBUG  0    //ֹͣ
#define DEBUGDATA  2    //��������
#define REMOTE_REBOOT  3    //������

#ifdef _REMOTECALLTEST  //Զ�̺��в���
 #define REMOTETEST      200   //����Զ�̺��в���
 #define STARTTEST  1    //��ʼ
 #define ENTERTESTMODE  2    //�������ģʽ
 #define STOPCALL       3    //ֹͣ����ͨ�� 
#endif

#define SEND_PCM  226      //����PCM����
#define _REMOTEWRITEOV7725REGISTER   //Զ��дOV7725�Ĵ���  ������
#ifdef _REMOTEWRITEOV7725REGISTER   //Զ��дOV7725�Ĵ���  ������
 #define OV7725_OPERATE   227       //��дOV7725
 #define OV7725READREGISTER  1
 #define OV7725WRITEREGISTER 2
#endif

#ifdef _SEND_TO_SELF_TEST    //�������ݰ����Լ�
 #define SEND_SELF  251
#endif

#define _PACK_1024

#ifdef _ROOM_SEARCH_SUPPORT 	 //���ڻ����� С�ſڻ���4.3���ڻ�
 UINT8 RoomSearchFlag;	  //0 ����	1 ����	2 д��
 UINT8 RoomSearchTime; 
 #define MAXCARDNUM	   50			   //�����
 struct RoomSearch1 RoomSearch;
 struct RoomSearch1 RoomSearchWrite;
 
	
#endif


#ifdef _PON_SUPPORT
   #define PORT_NUM  3
   #define PONSEARCHALLEQUIP  240  //���������豸���������ģ����豸��
   #define PONWRITEEQUIPADDR      241  //д�豸��ַ���������ģ����豸��
  
#else
  #define PORT_NUM  2
#endif

#define SMALLESTSIZE  18  //512  //UDP��С����С    20121102 512 -- 18 , MAC��СΪ60
#define PACKDATALEN  1200   //���ݰ���С

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

#ifdef _ROOM_SEARCH_SUPPORT      //���ڻ����� С�ſڻ���4.3���ڻ�
struct RoomSearch1{
               char Addr[20];             //��ַ����
               char AddrVerifyMa[10];         //������ͬ����
               UINT8 Mac_Addr[6]; //������ַ
               UINT8 IP[4];       //IP��ַ
               UINT8 IP_Mask[4];  //��������
               UINT8 IP_Gate[4];  //���ص�ַ
               UINT8 IP_Server[4];  //����������ַ����NS��������Ϊͬһ����

               char EngineerPass[10];             //��������

               UINT8 MultiMachineSupport;   //��ֻ�֧��
               UINT8 DenMaxNum;             //�ֻ��������
               UINT8 Language;              //����
               UINT8 PassOpenLock;          //���뿪��
               UINT8 CardOpenLock;         //ˢ������
               UINT8 DoorButton;            //���Ű�ť
               UINT8 DoorDetect;            //�Ŵż��
               UINT8 OpenLockTime ;         //����ʱ��  0 500ms  1 3s  3 5s
               UINT8 DelayLockTime;         //��ʱ����  0 0s  1 3s  2 5s  3 10s

               UINT8 MicDecValue; //չ�䡢չ��ģʽ,���С Mic����
               UINT8 CallDest;    //����Ŀ��  0 -- ����   1 -- ����
               UINT8 EncodeFrameRate; //����֡��

               UINT8 villa_ext_dev_type;     //�����ſڻ���������     0 -- ����   1 -- ˢ��
               UINT8 villa_ext_dev_function; //�����ſڻ����蹦��  0 -- ����   1 -- ������

               UINT32 CardNum;
               UINT8 SN[MAXCARDNUM*4];
              };
#endif

#endif
