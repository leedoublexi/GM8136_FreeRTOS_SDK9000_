
#include "stdlib.h"

#define x_first_h

#define CommonH
#include "common.h"

void InterfaceInit(void);

void DisplayFirstWindow(void);    //显示主窗口
void PreDealFirstWindow(uint8_t wintype);    //主窗口预处理，按键按下效果
void OperateFirstWindow(uint8_t wintype, uint8_t currwindow);    //主窗口操作
void CheckTouchFirstWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

void CheckFirstWindowFlag(void);  //检查返回首页标志

void RefreshBackWindow(uint8_t RefreshType, uint8_t FullPageType); //刷新背景
void DisplayStateBar(void);      //状态条
void PlaySoundTip(void); //播放提示音

void PreDealBigmenuButton(uint8_t wintype);  //快捷按键 预处理
void ShowPressQuickmenu_Button(uint8_t wintype);  //快捷按键   弹起
void OperateQuickmenu(uint8_t wintype, uint8_t currtype);

void DisplayInfoStateImage(uint8_t NetState);    //显示信息图标
void DisplayPictureStateImage(uint8_t NetState);    //显示留影图标

//供lib 调用
uint32_t GetLocal_fb_addr(void);
uint8_t GetLocal_fb_page(void);
uint32_t GetLocal_fb1_addr(void);

//uint8_t *Get_jpeg_bitstream(void);
//uint8_t Get_Mjpeg_DecodeError(void);
void Set_Local_DecodeJpegFlag(uint8_t disp_DecodeJpegFlag);
uint8_t Get_Local_DecodeStreamFlag(void);
//---------------------------------------------------------------------------
//当前窗口  0 -- 开机界面  1 -- 服务介绍   2 -- 设备管理   3 -- 安防界面  4 -- 通讯功能界面  5 -- 便捷服务  6 -- 多媒体界面   7 -- 设置界面   8 -- 情景模式


//          12 -- 视频呼叫
//          13 -- 监视门口
//          14 -- 记录查询
//          16 -- 对讲图像窗口
//          21 -- 通话记录窗口
//          22 -- 查看通话记录窗口

//          31 -- 撤防
//          32 -- 布防
//          34 -- 报警窗口
//          35 -- 取消报警窗口

//          111 -- 社区服务窗口  多媒体
//          112 -- 信息内容显示窗口

// 	    152 -- 设置主窗口
//          149 -- Digital KB1窗口
//          150 -- KB1窗口
//          151 -- LAN地址设置
// 	    154 -- 房号设置
// 	    158 -- 版本信息
// 	    159 -- 系统升级
//          165 -- 屏幕设置
//          166 -- 声音设置
//          167 -- 时间设置
//          168 -- 密码设置
//          169 -- 语言设置

// 	    160 -- 振铃音乐选择

//          188 -- 语音提示、按键音设置窗口
//          190 -- 校准触摸屏窗口
//          191 -- 校准触摸屏提示窗口
//          192 -- 测试窗口
//以下不能为局部变量　

//主界面
uint32_t main_image_Faddr = 0x00200000UL;
uint32_t logo_image_Faddr = 0x0020A000UL;

uint32_t menu_button_Faddr[5][2] = {
                                    {0x0020C000UL, 0x0020E000UL},
                                    {0x00210000UL, 0x00212000UL},
                                    {0x00215000UL, 0x00217000UL},
                                    {0x0021A000UL, 0x0021C000UL},
                                    {0x0021E000UL, 0x00220000UL}
                                   };

uint32_t state_image_Faddr[6] = {
                                    0x00222000UL,
                                    0x00223000UL,
                                    0x00224000UL,
                                    0x00225000UL,
                                    0x00226000UL,
                                    0x00227000UL
                                   };

uint32_t top_image_Faddr = 0x00242000UL;
uint32_t middle_image_Faddr = 0x00245000UL;
uint32_t bottom_image_Faddr = 0x00230000UL;
uint32_t bigmenu_button_Faddr[7][2] = {
                                    {0x00234000UL, 0x00235000UL},
                                    {0x00236000UL, 0x00237000UL},
                                    {0x00238000UL, 0x00239000UL},
                                    {0x0023A000UL, 0x0023B000UL},
                                    {0x0023C000UL, 0x0023D000UL},
                                    {0x0023E000UL, 0x0023F000UL},
                                    {0x00240000UL, 0x00241000UL}
                                   };

//进入设置
uint32_t setup_pass_edit_Faddr = 0x002A0000UL;
uint32_t setup_key_image_Faddr = 0x002A3000UL;

uint32_t num_button_Faddr[15][2] = {
                                    {0x002AA000UL, 0x002AB000UL},
                                    {0x002AC000UL, 0x002AD000UL},
                                    {0x002AE000UL, 0x002AF000UL},
                                    {0x002B0000UL, 0x002B1000UL},
                                    {0x002B2000UL, 0x002B3000UL},
                                    {0x002B4000UL, 0x002B5000UL},
                                    {0x002B6000UL, 0x002B7000UL},
                                    {0x002B8000UL, 0x002B9000UL},
                                    {0x002BA000UL, 0x002BB000UL},
                                    {0x002BC000UL, 0x002BD000UL},
                                    {0x002BE000UL, 0x002BF000UL},
                                    {0x002C0000UL, 0x002C1000UL},
                                    {0x002C2000UL, 0x002C3000UL},
                                    {0x002C5000UL, 0x002C6000UL},
                                    {0x002C8000UL, 0x002C9000UL}
                                   };

//设置主窗口
uint32_t setup_button_Faddr[5][2] = {
                                    {0x00320000UL, 0x00322000UL},
                                    {0x00324000UL, 0x00326000UL},
                                    {0x00328000UL, 0x0032A000UL},
                                    {0x0032C000UL, 0x0032E000UL},
                                    {0x00330000UL, 0x00332000UL}
                                   };

//键盘窗口
uint32_t kb_image_Faddr = 0x00370000UL;
uint32_t kb_edit_Faddr = 0x0037C000UL;

uint32_t kb_button_Faddr[15][2] = {
                                    {0x0037E000UL, 0x0037F000UL},
                                    {0x00380000UL, 0x00381000UL},
                                    {0x00382000UL, 0x00383000UL},
                                    {0x00384000UL, 0x00385000UL},
                                    {0x00386000UL, 0x00387000UL},
                                    {0x00388000UL, 0x00389000UL},
                                    {0x0038A000UL, 0x0038B000UL},
                                    {0x0038C000UL, 0x0038D000UL},
                                    {0x0038E000UL, 0x0038F000UL},
                                    {0x00390000UL, 0x00391000UL},
                                    {0x00392000UL, 0x00393000UL},
                                    {0x00394000UL, 0x00395000UL},
                                    {0x00396000UL, 0x00397000UL},
                                    {0x00399000UL, 0x0039A000UL},
                                    {0x0039C000UL, 0x0039D000UL}
                                   };

//LAN设置窗口
uint32_t lan_image_Faddr = 0x0033B000UL;
uint32_t lan_button_Faddr[2][2] = {
                                    {0x00335000UL, 0x00336000UL},
                                    {0x00338000UL, 0x00339000UL}
                                   };
uint32_t lan_edit_Faddr[2] = {
                                    0x00345000UL, 0x00346000UL
                                   };

//房号设置窗口
uint32_t addr_image_Faddr = 0x00350000UL;
uint32_t addr_button_Faddr[2][2] = {
                                    {0x0035B000UL, 0x0035C000UL},
                                    {0x0035E000UL, 0x0035F000UL}
                                   };

//修改密码窗口
uint32_t pass_image_Faddr = 0x003C0000UL;

//屏幕调节窗口
uint32_t screen_image_Faddr = 0x003D0000UL;
uint32_t screen_button_Faddr[3][2] = {
                                    {0x003D9000UL, 0x003DA000UL},
                                    {0x003DC000UL, 0x003DD000UL},
                                    {0x003DE000UL, 0x003DF000UL}
                                   };
uint32_t brightness_image_Faddr[6] = {
                                    0x003E0000UL, 0x003E2000UL, 0x003E4000UL, 0x003E6000UL, 0x003E8000UL, 0x003EA000UL
                                   };

//音量调节窗口
uint32_t voice_image_Faddr = 0x003F0000UL;
uint32_t voice_button_Faddr[5][2] = {
                                    {0x003FA000UL, 0x003FB000UL},
                                    {0x003FD000UL, 0x003FE000UL},
                                    {0x003FF000UL, 0x00400000UL},
                                    {0x00401000UL, 0x00402000UL},
                                    {0x00403000UL, 0x00404000UL}
                                   };
uint32_t spk_image_Faddr[6] = {
                                    0x00405000UL, 0x00407000UL, 0x00409000UL, 0x0040B000UL, 0x0040D000UL, 0x0040F000UL
                                   };
uint32_t mic_image_Faddr[6] = {
                                    0x00411000UL, 0x00413000UL, 0x00415000UL, 0x00417000UL, 0x00419000UL, 0x0041B000UL
                                   };

//关于窗口
uint32_t version_button_Faddr[2][2] = {
                                    {0x003A0000UL, 0x003A1000UL},
                                    {0x003A2000UL, 0x003A3000UL}
                                   };

//触摸屏校准窗口
uint32_t calibrate_image_Faddr = 0x003B0000UL;
uint32_t cross_button_Faddr[1][2] = {
                                    {0x003B2000UL, 0x003B3000UL}
                                   };

//呼入窗口
uint32_t talkpic_image_Faddr = 0x002E0000UL;
uint32_t talkpic_button_Faddr[3][2] = {
                                    {0x002E4000UL, 0x002E5000UL},
                                    {0x002E6000UL, 0x002E7000UL},
                                    {0x002E8000UL, 0x002E9000UL}
                                   };
uint32_t talkwatchpic_image_Faddr  = 0x00250000UL;


//监视视频窗口
uint32_t watchpic_image_Faddr = 0x002EA000UL;
uint32_t watchpic_button_Faddr[1][2] = {
                                    {0x002E4000UL, 0x002E5000UL}
                                    };

//监视窗口
uint32_t watchtarget_image_Faddr = 0x00310000UL;
uint32_t watch_Faddr[3][2] = {
                                    {0x00312000UL, 0x00313000UL},
                                    {0x00314000UL, 0x00315000UL},
                                    {0x00316000UL, 0x00318000UL}
                                   };

//布防窗口
uint32_t fortify_button_Faddr[2][2] = {
                                    {0x002F0000UL, 0x002F2000UL},
                                    {0x002F4000UL, 0x002F6000UL}
                                   };

//报警图像
uint32_t alarm_main_image_Faddr = 0x00306000;
uint32_t alarm_image_Faddr[2] = {0x002F8000UL, 0x002FE000UL};

//信息窗口
uint32_t info_image_Faddr = 0x00420000UL;
uint32_t info_button_Faddr[3][2] = {
                                    {0x00429000UL, 0x0042A000UL},
                                    {0x0042B000UL, 0x0042C000UL},
                                    {0x0042D000UL, 0x0042E000UL}
                                   };
uint32_t inforow_button_Faddr[INFONUMPERPAGE][2] = {
                                    {0x0042F000UL, 0x00431000UL},
                                    {0x00433000UL, 0x00435000UL},
                                    {0x00437000UL, 0x00439000UL},
                                    {0x0043B000UL, 0x0043D000UL},
                                    {0x0043F000UL, 0x00441000UL}
                                   };

//信息查看窗口
uint32_t infowin_image_Faddr = 0x00450000UL;
uint32_t infowin_button_Faddr[3][2] = {
                                    {0x00454000UL, 0x00455000UL},
                                    {0x00456000UL, 0x00457000UL},
                                    {0x00458000UL, 0x00459000UL}
                                   };
uint32_t infocontent_image_Faddr = 0x00453000UL;

//通话记录窗口
uint32_t records_pic_icon_Faddr[RECORDNUMPERPAGE] = {0x0046A000UL, 0x0046B000UL, 0x0046C000UL, 0x0046D000UL, 0x0046E000UL};
//  struct TImage records_image;
//  struct TImageButton records_button[3];
//  struct TImageButton recordsrow_button[RECORDNUMPERPAGE];

  //通话记录查看窗口
uint32_t picwin_image_Faddr = 0x00480000UL;
uint32_t pic_blank_image_Faddr = 0x00484000UL;
uint32_t picwin_button_Faddr[3][2] = {
                                    {0x00486000UL, 0x00487000UL},
                                    {0x00488000UL, 0x00489000UL},
                                    {0x0048A000UL, 0x0048B000UL}
                                   };

void InterfaceInit(void)     //初始化界面资源
{
  uint8_t i;

  //主界面
  main_image.Faddr = main_image_Faddr;
  logo_image.Faddr = logo_image_Faddr;
  PreDealImage(&main_image);
  PreDealImage(&logo_image);
    
  for(i=0; i<5; i++)
   {
    menu_button[i].Faddr[0] = menu_button_Faddr[i][0];
    menu_button[i].Faddr[1] = menu_button_Faddr[i][1];
    PreDealImageButton(&menu_button[i], IMAGEUP);
    PreDealImageButton(&menu_button[i], IMAGEDOWN);
   }

  for(i=0; i<6; i++)
   {
    state_image[i].Faddr = state_image_Faddr[i];
    PreDealImage(&state_image[i]);
   }

  for(i=0; i<7; i++)
   {
    bigmenu_button[i].Faddr[0] = bigmenu_button_Faddr[i][0];
    bigmenu_button[i].Faddr[1] = bigmenu_button_Faddr[i][1];
    PreDealImageButton(&bigmenu_button[i], IMAGEUP);
    PreDealImageButton(&bigmenu_button[i], IMAGEDOWN);
   }

  top_image.Faddr = top_image_Faddr;
  middle_image.Faddr = middle_image_Faddr;
  bottom_image.Faddr = bottom_image_Faddr;
  PreDealImage(&top_image);
  PreDealImage(&middle_image);
  PreDealImage(&bottom_image);
#if 1
  //进入设置
  setup_pass_edit.Faddr[0] = setup_pass_edit_Faddr;
  PreDealEdit(&setup_pass_edit, IMAGEUP);
  setup_pass_edit.FontType = 24;
  setup_pass_edit.CursorX = 40;//20;
  setup_pass_edit.CursorY = 36;//16;
  if(setup_pass_edit.FontType == 16)
   {
    setup_pass_edit.CursorHeight = 12;
    setup_pass_edit.fWidth = 8;
    setup_pass_edit.Space = 0;
   }
  else
   {
    setup_pass_edit.CursorHeight = 24;
    setup_pass_edit.fWidth = 16;
    setup_pass_edit.Space = 2;
   }
  setup_pass_edit.CursorCorlor = COLOR_WHITE;

  setup_pass_edit.Text[0] = '\0';
  setup_pass_edit.BoxLen = 0;         //文本框当前输入长度
  setup_pass_edit.MaxLen = 4;         //文本框最大输入长度
  setup_pass_edit.Focus = 0;        //显示输入光标
  setup_pass_edit.Visible = 0;


  setup_key_image.Faddr = setup_key_image_Faddr;
  PreDealImage(&setup_key_image);

  for(i=0; i<15; i++)
   {
    num_button[i].Faddr[0] = num_button_Faddr[i][0];
    num_button[i].Faddr[1] = num_button_Faddr[i][1];
    PreDealImageButton(&num_button[i], IMAGEUP);
    PreDealImageButton(&num_button[i], IMAGEDOWN);
   }

  //设置主窗口
  for(i=0; i<5; i++)
   {
    setup_button[i].Faddr[0] = setup_button_Faddr[i][0];
    setup_button[i].Faddr[1] = setup_button_Faddr[i][1];
    PreDealImageButton(&setup_button[i], IMAGEUP);
    PreDealImageButton(&setup_button[i], IMAGEDOWN);
   }

  //键盘界面
  kb_image.Faddr = kb_image_Faddr;
  PreDealImage(&kb_image);

  kb_edit.Faddr[0] = kb_edit_Faddr;
  PreDealEdit(&kb_edit, IMAGEUP);
  kb_edit.FontType = 24;
  kb_edit.CursorX = 40;//20;
  kb_edit.CursorY = 36;//16;
  if(kb_edit.FontType == 16)
   {
    kb_edit.CursorHeight = 12;
    kb_edit.fWidth = 8;
    kb_edit.Space = 0;
   }
  else
   {
    kb_edit.CursorHeight = 24;
    kb_edit.fWidth = 16;
    kb_edit.Space = 2;
   }
  kb_edit.CursorCorlor = COLOR_WHITE;

  kb_edit.Text[0] = '\0';
  kb_edit.BoxLen = 0;         //文本框当前输入长度
  kb_edit.MaxLen = 4;         //文本框最大输入长度
  kb_edit.Focus = 0;        //显示输入光标
  kb_edit.Visible = 0;


  for(i=0; i<15; i++)
   {
    kb_button[i].Faddr[0] = kb_button_Faddr[i][0];
    kb_button[i].Faddr[1] = kb_button_Faddr[i][1];
    PreDealImageButton(&kb_button[i], IMAGEUP);
    PreDealImageButton(&kb_button[i], IMAGEDOWN);
   }

  //LAN设置窗口
  lan_image.Faddr = lan_image_Faddr;
  PreDealImage(&lan_image);
  for(i=0; i<2; i++)
   {
    lan_button[i].Faddr[0] = lan_button_Faddr[i][0];
    lan_button[i].Faddr[1] = lan_button_Faddr[i][1];
    PreDealImageButton(&lan_button[i], IMAGEUP);
    PreDealImageButton(&lan_button[i], IMAGEDOWN);
   }

  for(i=0; i<5; i++)
   {
    lan_edit[i] = setup_pass_edit;
    lan_edit[i].FontType = 16;
    lan_edit[i].MaxLen = 15;
   }

   lan_edit[0].Faddr[0] = lan_edit_Faddr[0];
   PreDealEdit(&lan_edit[0], IMAGEUP);
   lan_edit[0].Faddr[1] = lan_edit_Faddr[1];
   PreDealEdit(&lan_edit[0], IMAGEDOWN);

  for(i=1; i<5; i++)
    lan_edit[i] = lan_edit[0];

  lan_edit[0].xLeft = 180;
  lan_edit[0].yTop = 122;
  lan_edit[1].xLeft = 534;
  lan_edit[1].yTop = 122;
  lan_edit[2].xLeft = 180;
  lan_edit[2].yTop = 190;
  lan_edit[3].xLeft = 534;
  lan_edit[3].yTop = 190;
  lan_edit[4].xLeft = 180;
  lan_edit[4].yTop = 256;

  //房号设置窗口
  addr_image.Faddr = addr_image_Faddr;
  PreDealImage(&addr_image);

  for(i=0; i<2; i++)
   {
    addr_button[i].Faddr[0] = addr_button_Faddr[i][0];
    addr_button[i].Faddr[1] = addr_button_Faddr[i][1];
    PreDealImageButton(&addr_button[i], IMAGEUP);
    PreDealImageButton(&addr_button[i], IMAGEDOWN);
   }

  for(i=0; i<8; i++)
    addr_edit[i] = lan_edit[0];

  addr_edit[0].xLeft = 180;
  addr_edit[0].yTop = 100;
  addr_edit[1].xLeft = 180;
  addr_edit[1].yTop = 160;
  addr_edit[2].xLeft = 180;
  addr_edit[2].yTop = 220;
  addr_edit[3].xLeft = 180;
  addr_edit[3].yTop = 280;

  addr_edit[4].xLeft = 534;
  addr_edit[4].yTop = 100;
  addr_edit[5].xLeft = 534;
  addr_edit[5].yTop = 160;
  addr_edit[6].xLeft = 534;
  addr_edit[6].yTop = 220;
  addr_edit[7].xLeft = 534;
  addr_edit[7].yTop = 280;

  //修改密码窗口
  pass_image.Faddr = pass_image_Faddr;
  PreDealImage(&pass_image);

  for(i=0; i<2; i++)
   {
    pass_button[i] = lan_button[i];
   }

  for(i=0; i<3; i++)
    pass_edit[i] = lan_edit[0];

  pass_edit[0].xLeft = 366;
  pass_edit[0].yTop = 122;
  pass_edit[1].xLeft = 366;
  pass_edit[1].yTop = 190;
  pass_edit[2].xLeft = 366;
  pass_edit[2].yTop = 256;
  pass_edit[0].FontType = 24;
  pass_edit[1].FontType = 24;
  pass_edit[2].FontType = 24;

  //屏幕调节窗口
  screen_image.Faddr = screen_image_Faddr;
  PreDealImage(&screen_image);

  for(i=0; i<3; i++)
   {
    screen_button[i].Faddr[0] = screen_button_Faddr[i][0];
    screen_button[i].Faddr[1] = screen_button_Faddr[i][1];
    PreDealImageButton(&screen_button[i], IMAGEUP);
    PreDealImageButton(&screen_button[i], IMAGEDOWN);
   }

  for(i=0; i<6; i++)
   {
    brightness_image[i].Faddr = brightness_image_Faddr[i];
    PreDealImage(&brightness_image[i]);
   }
  //音量调节窗口
  voice_image.Faddr = voice_image_Faddr;
  PreDealImage(&voice_image);

  for(i=0; i<5; i++)
   {
    voice_button[i].Faddr[0] = voice_button_Faddr[i][0];
    voice_button[i].Faddr[1] = voice_button_Faddr[i][1];
    PreDealImageButton(&voice_button[i], IMAGEUP);
    PreDealImageButton(&voice_button[i], IMAGEDOWN);
   }

  for(i=0; i<6; i++)
   {
    spk_image[i].Faddr = spk_image_Faddr[i];
    PreDealImage(&spk_image[i]);
   }

  for(i=0; i<6; i++)
   {
    mic_image[i].Faddr = mic_image_Faddr[i];
    PreDealImage(&mic_image[i]);
   }

  //关于窗口
  for(i=0; i<2; i++)
   {
    version_button[i].Faddr[0] = version_button_Faddr[i][0];
    version_button[i].Faddr[1] = version_button_Faddr[i][1];
    PreDealImageButton(&version_button[i], IMAGEUP);
    PreDealImageButton(&version_button[i], IMAGEDOWN);
   }

  //触摸屏校准窗口
  calibrate_image.Faddr = calibrate_image_Faddr;
  PreDealImage(&calibrate_image);

  for(i=0; i<1; i++)
   {
    cross_button[i].Faddr[0] = cross_button_Faddr[i][0];
    cross_button[i].Faddr[1] = cross_button_Faddr[i][1];
    PreDealImageButton(&cross_button[i], IMAGEUP);
    PreDealImageButton(&cross_button[i], IMAGEDOWN);
   }

  //呼出窗口
  call_image.Faddr = setup_key_image.Faddr;
  PreDealImage(&call_image);

  r2r_edit = setup_pass_edit;
  r2r_edit.BoxLen = 0;         //文本框当前输入长度
  r2r_edit.MaxLen = 10;         //文本框最大输入长度

  //呼入窗口
  talkpic_image.Faddr = talkpic_image_Faddr;
  PreDealImage(&talkpic_image);

  for(i=0; i<3; i++)
   {
    talkpic_button[i].Faddr[0] = talkpic_button_Faddr[i][0];
    talkpic_button[i].Faddr[1] = talkpic_button_Faddr[i][1];
    PreDealImageButton(&talkpic_button[i], IMAGEUP);
    PreDealImageButton(&talkpic_button[i], IMAGEDOWN);
   }
 
  talkwatchpic_image.Faddr = talkwatchpic_image_Faddr;
  PreDealImage(&talkwatchpic_image); 	

  //监视视频窗口
  watchpic_image.Faddr = watchpic_image_Faddr;
  PreDealImage(&watchpic_image);

  for(i=0; i<1; i++)
   {
    watchpic_button[i].Faddr[0] = watchpic_button_Faddr[i][0];
    watchpic_button[i].Faddr[1] = watchpic_button_Faddr[i][1];
    PreDealImageButton(&watchpic_button[i], IMAGEUP);
    PreDealImageButton(&watchpic_button[i], IMAGEDOWN);
   }

  //监视窗口
  watchtarget_image.Faddr = watchtarget_image_Faddr;
  PreDealImage(&watchtarget_image);

  for(i=0; i<3; i++)
   {
    watch_button[i].Faddr[0] = watch_Faddr[i][0];
    watch_button[i].Faddr[1] = watch_Faddr[i][1];
    PreDealImageButton(&watch_button[i], IMAGEUP);
    PreDealImageButton(&watch_button[i], IMAGEDOWN);
   }

  //布防窗口
  for(i=0; i<2; i++)
   {
    fortify_button[i].Faddr[0] = fortify_button_Faddr[i][0];
    fortify_button[i].Faddr[1] = fortify_button_Faddr[i][1];
    PreDealImageButton(&fortify_button[i], IMAGEUP);
    PreDealImageButton(&fortify_button[i], IMAGEDOWN);
   }

  //撤防窗口
  //密码窗口文本框
  cancelfortify_edit = setup_pass_edit;
  cancelfortify_edit.BoxLen = 0;         //文本框当前输入长度
  cancelfortify_edit.MaxLen = 4;         //文本框最大输入长度

  //报警图像
  alarm_main_image.Faddr = alarm_main_image_Faddr;
  PreDealImage(&alarm_main_image);
  
  for(i=0; i<2; i++)
   {
    alarm_image[i].Faddr = alarm_image_Faddr[i];
    PreDealImage(&alarm_image[i]);
   }

  //信息窗口
  info_image.Faddr = info_image_Faddr;
  PreDealImage(&info_image);

  for(i=0; i<3; i++)
   {
    info_button[i].Faddr[0] = info_button_Faddr[i][0];
    info_button[i].Faddr[1] = info_button_Faddr[i][1];
    PreDealImageButton(&info_button[i], IMAGEUP);
    PreDealImageButton(&info_button[i], IMAGEDOWN);
   }

  for(i=0; i<INFONUMPERPAGE; i++)
   {
    inforow_button[i].Faddr[0] = inforow_button_Faddr[i][0];
    inforow_button[i].Faddr[1] = inforow_button_Faddr[i][1];
    PreDealImageButton(&inforow_button[i], IMAGEUP);
    PreDealImageButton(&inforow_button[i], IMAGEDOWN);
   }

  //信息查看窗口
  infowin_image.Faddr = infowin_image_Faddr;
  PreDealImage(&infowin_image);

  for(i=0; i<3; i++)
   {
    infowin_button[i].Faddr[0] = infowin_button_Faddr[i][0];
    infowin_button[i].Faddr[1] = infowin_button_Faddr[i][1];
    PreDealImageButton(&infowin_button[i], IMAGEUP);
    PreDealImageButton(&infowin_button[i], IMAGEDOWN);
   }

  infocontent_image.Faddr = infocontent_image_Faddr;
  PreDealImage(&infocontent_image);

  //通话记录窗口
  records_image = info_image;
  for(i=0; i<3; i++)
    records_button[i] = info_button[i];
  for(i=0; i<INFONUMPERPAGE; i++)
    recordsrow_button[i] = inforow_button[i];


  for(i=0; i<RECORDNUMPERPAGE; i++)
   {
    records_pic_icon[i].Faddr = records_pic_icon_Faddr[i];
    PreDealImage(&records_pic_icon[i]);
   }

  //通话记录查看窗口
  picwin_image.Faddr = picwin_image_Faddr;
  PreDealImage(&picwin_image);

  for(i=0; i<3; i++)
   {
    picwin_button[i].Faddr[0] = picwin_button_Faddr[i][0];
    picwin_button[i].Faddr[1] = picwin_button_Faddr[i][1];
    PreDealImageButton(&picwin_button[i], IMAGEUP);
    PreDealImageButton(&picwin_button[i], IMAGEDOWN);
   }

  pic_blank_image.Faddr = pic_blank_image_Faddr;
  PreDealImage(&pic_blank_image);



  soundtip_wav.Faddr =      0x00768000UL;
  ring_wav.Faddr =          0x00700000UL;//0x0076C000UL;//0x00700000UL;//0x0076C000UL;
  modisucc_wav.Faddr =      0x0070D000UL;
  modifail_wav.Faddr =      0x00714000UL;
  passerror_wav.Faddr =     0x0071B000UL;
  cancelfortify_wav.Faddr = 0x0072F000UL;
  defencedelay_wav.Faddr =  0x00735000UL;
  fortify_wav.Faddr =       0x00732000UL;
  cancelalarm_wav.Faddr =   0x0072B000UL;
  alarmdelay_wav.Faddr =    0x00721000UL;
  alarm_wav.Faddr =         0x00727000UL;

  inputerror_wav.Faddr =    0x00738000UL;

  backring_wav.Faddr =          0x0073C000UL;

  null_wav.Faddr = 0x00769000UL;
  
  icn8505_wav.Faddr = 0x000DE000UL;

  wav_1k[0].Faddr = 0x00600000UL;   //1K 0db
  wav_1k[1].Faddr = 0x0060C000UL;   //1K -3db
  wav_1k[2].Faddr = 0x00616000UL;   //1K -6db
  wav_1k[3].Faddr = 0x00620000UL;   //1K -9db
  wav_1k[4].Faddr = 0x0062A000UL;   //1K -12db
  wav_1k[5].Faddr = 0x00632000UL;   //1K -15db
  wav_1k[6].Faddr = 0x0063C000UL;   //1K -18db
  wav_1k[7].Faddr = 0x00646000UL;   //1K -21db
  wav_1k[8].Faddr = 0x00650000UL;   //1K -24db
  wav_1k[9].Faddr = 0x0065A000UL;   //1K -27db
  wav_1k[10].Faddr = 0x00664000UL;   //1K -30db
  wav_1k[11].Faddr = 0x0066E000UL;   //1K -33db
  wav_1k[12].Faddr = 0x00678000UL;   //1K -36db
  wav_1k[13].Faddr = 0x00682000UL;   //1K -39db
  wav_1k[14].Faddr = 0x0068C000UL;   //1K -42db
  wav_1k[15].Faddr = 0x00696000UL;   //1K -45db
  wav_1k[16].Faddr = 0x006A0000UL;   //1K -48db
  wav_1k[17].Faddr = 0x006AA000UL;   //1K -51db
  wav_1k[18].Faddr = 0x006B4000UL;   //1K -54db
  wav_1k[19].Faddr = 0x006BE000UL;   //1K -57db
  wav_1k[20].Faddr = 0x006C8000UL;   //1K -60db
#endif  
}
//---------------------------------------------------------------------------
//供lib 调用
uint32_t GetLocal_fb_addr(void)
{
  return Local.fb_addr;
}
//---------------------------------------------------------------------------
uint8_t GetLocal_fb_page(void)
{
  return Local.fb_page;
}
//---------------------------------------------------------------------------
uint32_t GetLocal_fb1_addr(void)
{
  return Local.fb1_addr;
}
//---------------------------------------------------------------------------

/*uint8_t *Get_jpeg_bitstream(void)
{
  return jpeg_bitstream;
}*/
//---------------------------------------------------------------------------
/*uint8_t Get_Mjpeg_DecodeError(void)
{
  return Mjpeg_DecodeError;
}*/
//---------------------------------------------------------------------------
void Set_Local_DecodeJpegFlag(uint8_t disp_DecodeJpegFlag)
{
  Local.DecodeJpegFlag = disp_DecodeJpegFlag;
}
//---------------------------------------------------------------------------
uint8_t Get_Local_DecodeStreamFlag(void)
{
  return Local.DecodeStreamFlag;
}
//---------------------------------------------------------------------------
void DisplayFirstWindow(void)    //显示主窗口
{
  uint8_t i;
  char ip_text[20];
  xtm_printf("DisplayFirstWindow::...\n");

  DisplayImage(&main_image, FROMFILE, FULLPAGE);       // FROMFILE


 DisplayImage(&logo_image, FROMFILE, FULLPAGE);


  for(i=0; i<5; i++)
   {     
     DisplayImageButton(&menu_button[i], IMAGEUP, FROMFILE, FULLPAGE);

  }
  //显示状态条
  DisplayStateBar();   


  Local.CurrentWindow = 0;

  Local.PreStatus = 0;  //20130115 预状态，防止同时的操作

  //sprintf(ip_text, "%d.%d.%d.%d", LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);
  //display_text(100, 100, 0, COLOR_BLACK, ip_text, 0, 16, FULLPAGE);
  //display_text(100, 120, 0, COLOR_RED, "ABCDefg123", 0, 16, FULLPAGE);
  //display_text(100, 150, 0, COLOR_BLACK, "地址请输入地址密码", 0, 24, FULLPAGE);
  //display_text(100, 180, 0, COLOR_RED, "ABCDefg123", 0, 24, FULLPAGE); 
  //display_text(100, 150, 0, COLOR_BLACK, "地", 0, 24, FULLPAGE);
  //display_text(100, 150, 0, COLOR_BLACK, "地址请输入地址密码", 0, 24, FULLPAGE);

  //StartPlayWav(modisucc_wav.Faddr, NO_CYCLE_PLAY);
	// PlaySoundTip();

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
  
  /*vTaskDelay(3000);
  xtm_clear_disp_ram(Local.fb_page, COLOR_RED);
  vTaskDelay(3000);
  xtm_clear_disp_ram(Local.fb_page, COLOR_GREEN);
  vTaskDelay(3000);
  xtm_clear_disp_ram(Local.fb_page, COLOR_BLUE);
  vTaskDelay(3000);
  xtm_clear_disp_ram(Local.fb_page, COLOR_WHITE);
  vTaskDelay(3000);
  xtm_clear_disp_ram(Local.fb_page, COLOR_BLACK);
  vTaskDelay(3000);
  xtm_clear_disp_ram(Local.fb_page, COLOR_YELLOW);
  vTaskDelay(3000);
  xtm_clear_disp_ram(Local.fb_page, COLOR_CYAN);  */

  Local.FirstWindowFlag = 1;     //返回首页标志
  Local.FirstWindowTime = 0;     //返回首页 计数

  Local.NoBreak = 0;
  xtm_printf("DisplayFirstWindowIP = %d.%d.%d.%d\n", 
  	LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);

  //StartPlayWav(soundtip_wav.Faddr, NO_CYCLE_PLAY);//NO_CYCLE_PLAY);       // soundtip_wav.Faddr

//  SoundSetSpeakerVolume(LocalCfg.SpkVolume);
   
}
//---------------------------------------------------------------------------
//检查返回首页标志
void CheckFirstWindowFlag(void)
{
  if(Local.CurrentWindow == 0)
   if(Local.FirstWindowFlag == 1)     //返回首页标志
    {
     Local.FirstWindowTime ++;     //返回首页 计数
     if(Local.FirstWindowTime > 3*TIMERPERSEC)
      {
       Local.FirstWindowFlag = 0;
       Local.FirstWindowTime = 0;
       StopPlayWavFile();
      }
    }
}
//---------------------------------------------------------------------------
void PreDealFirstWindow(uint8_t wintype)    //主窗口预处理，按键按下效果
{
  if((wintype >= 0)&&(wintype <= 4))
    DisplayImageButton(&menu_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
}
//---------------------------------------------------------------------------
void OperateFirstWindow(uint8_t wintype, uint8_t currwindow)    //主窗口操作
{
  if(wintype < 5)
    DisplayImageButton(&menu_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
  printf("OperateFirstWindow::wintype = %d, currwindow = %d, Local.isHost = %d\n", wintype, currwindow, Local.isHost);
  switch(wintype)
   {
    case 0:
             if(Local.isHost != '0')  //副机, 安防功能无效
                break;    
             switch(LocalCfg.DefenceStatus)
              {
               case 0://未布防
                      if(Local.CurrentWindow != 32)
                        DisplayFortifyWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
                      break;
               case 1://外出布防
               case 2://在家布防
               case 11://外出布防延时中
               case 12://在家布防延时中
                      if(Local.CurrentWindow != 31)
                        DisplayCancelFortifyWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
                      break;
              }
           break;
    case 1://显示对讲呼出窗口
           DisplayTalkWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
           break;
    case 2://显示监视窗口
           DisplayWatchWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);    //刷新 中 下
           break;
    case 3://显示信息窗口
           DisplayInfoWindow(REFRESH_MIDDLE | REFRESH_BOTTOM, 0);    //刷新 中 下
           break;
    case 4://显示通话记录窗口
           DisplayRecordsWindow(REFRESH_MIDDLE | REFRESH_BOTTOM, 0);    //刷新 中 下
           break;
    case 5: //设置
           DisplaySetupWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
           break;

   }
}
//---------------------------------------------------------------------------
void CheckTouchFirstWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<5; i++)  //窗口按钮
    {
     isInTouch = CheckTSInButton(&menu_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   for(i=2; i<6; i+=2)  //窗口按钮
    {
     isInTouch = CheckTSInImage(&state_image[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = 8 + i;
       break;
      }
    }
    
  if(isInTouch == 0)
    {
     isInTouch = CheckTSInImage(&logo_image, XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = 5;
      }
    }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//---------------------------------------------------------------------------
void RefreshBackWindow(uint8_t RefreshType, uint8_t FullPageType) //刷新背景
{
  //if(RefreshType & REFRESH_TOP)
   {
    DisplayImage(&top_image, FROMFILE, FullPageType);
    DisplayImage(&logo_image, FROMFILE, FullPageType);
   }
  //if(RefreshType & REFRESH_MIDDLE)
    DisplayImage(&middle_image, FROMFILE, FullPageType);
  //if(RefreshType & REFRESH_BOTTOM)
    DisplayImage(&bottom_image, FROMFILE, FullPageType);
}
//---------------------------------------------------------------------------
void DisplayStateBar(void)
{
  //0--网络断线
  //1--网络正常
  //2--无新短信
  //3--有新短信
  //4--无新留影
  //5--有新留影
  uint8_t i;

  if(Local.isHost != '0')  //副机, 布撤防无效
    LocalCfg.DefenceStatus = 0;

  if((LocalCfg.DefenceStatus != 0)&&(LocalCfg.DefenceStatus != 1)&&(LocalCfg.DefenceStatus != 2)&&(LocalCfg.DefenceStatus != 11)&&(LocalCfg.DefenceStatus != 12))
    LocalCfg.DefenceStatus = 0;    
      
  if(Local.NetLanStatus == 1)  //网络状态 0 断开  1 接通
    DisplayImage(&state_image[1], FROMFILE, FULLPAGE);
  else
    DisplayImage(&state_image[0], FROMFILE, FULLPAGE);

  //有新信息
  if(Local.NewInfo == 1)
    DisplayImage(&state_image[3], FROMFILE, FULLPAGE);
  else
    DisplayImage(&state_image[2], FROMFILE, FULLPAGE);

  //有新留影
  if(Local.NewRecPic == 1)
    DisplayImage(&state_image[5], FROMFILE, FULLPAGE);
  else
    DisplayImage(&state_image[4], FROMFILE, FULLPAGE);

/*  //布防状态
  if((LocalCfg.DefenceStatus == 1)||(LocalCfg.DefenceStatus == 2)||(LocalCfg.DefenceStatus == 3)||(LocalCfg.DefenceStatus == 4)||(LocalCfg.DefenceStatus == 5))
    DisplayImage(&state_image[3], MAINYUVFBPAGE, FROMMEMORY);
  else
    DisplayImage(&state_image[2], MAINYUVFBPAGE, FROMMEMORY);      */






/*  DisplayClock(REFRESH); //显示时钟
  if(Local.interface_init_flag == 1)
    ShowWeather();  //显示天气     */
}
//---------------------------------------------------------------------------
void PlaySoundTip(void) //播放提示音
{
  if(LocalCfg.VoiceHint == 1)
   {
    WaitAudioUnuse(200); //等待声卡空闲               //  wav_1k[5].Faddr
    StartPlayWav(soundtip_wav.Faddr, NO_CYCLE_PLAY);       // soundtip_wav.Faddr
   }
}
//---------------------------------------------------------------------------
void PreDealBigmenuButton(uint8_t wintype)  //快捷按键 预处理
{
  if((wintype >= 100)&&(wintype <= 106))
    DisplayImageButton(&bigmenu_button[wintype-100], IMAGEDOWN, FROMFILE, NOFULLPAGE);
}
//---------------------------------------------------------------------------
void ShowPressQuickmenu_Button(uint8_t wintype)  //快捷按键  弹起
{
  if((wintype >= 100)&&(wintype <= 106))
    DisplayImageButton(&bigmenu_button[wintype-100], IMAGEUP, FROMFILE, NOFULLPAGE);
}
//---------------------------------------------------------------------------
void OperateQuickmenu(uint8_t wintype, uint8_t currtype)
{
  switch(wintype)
   {
    case 100:  //首页
           if(currtype != wintype)
             DisplayFirstWindow();
           break;
    case 101:  //安防
          if(Local.isHost != '0')  //副机, 安防功能无效
             break;
          if(currtype != wintype)
             switch(LocalCfg.DefenceStatus)
              {
               case 0://未布防
                      if(Local.CurrentWindow != 32)
                        DisplayFortifyWindow(REFRESH_MIDDLE);
                      break;
               case 1://外出布防
               case 2://在家布防
               case 11://外出布防延时中
               case 12://在家布防延时中
                      if(Local.CurrentWindow != 31)
                        DisplayCancelFortifyWindow(REFRESH_MIDDLE);
                      break;
              }
           break;
    case 102:  //呼叫
           if(currtype != wintype)
             DisplayTalkWindow(REFRESH_MIDDLE);
           break;
    case 103:  //监视
           if(currtype != wintype)
             DisplayWatchWindow(REFRESH_MIDDLE);    //刷新 中 
           break;
    case 104: //信息
           if(currtype != wintype)
             DisplayInfoWindow(REFRESH_MIDDLE, 0);    //刷新 中
           break;
    case 105:  //记录
           if(currtype != wintype)
               DisplayRecordsWindow(REFRESH, 0);    //显示照片窗口（一级）
           break;
    case 106:  //系统设置
           if(currtype != wintype)
             DisplaySetupWindow(REFRESH_MIDDLE);
           break;
   }
}
//---------------------------------------------------------------------------
void DisplayInfoStateImage(uint8_t State)    //显示信息图标
{
  if(Local.CurrentWindow == 0)
     {
      if(State == 0)
        DisplayImage(&state_image[2], FROMFILE, NOFULLPAGE);
      else
        DisplayImage(&state_image[3], FROMFILE, NOFULLPAGE);
     }
}
//---------------------------------------------------------------------------
void DisplayPictureStateImage(uint8_t State)    //显示留影图标
{
  if(Local.CurrentWindow == 0)
     {
      if(State == 0)
        DisplayImage(&state_image[4], FROMFILE, NOFULLPAGE);
      else
        DisplayImage(&state_image[5], FROMFILE, NOFULLPAGE);
     }
}
//---------------------------------------------------------------------------

