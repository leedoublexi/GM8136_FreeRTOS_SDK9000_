
#include "stdlib.h"

#define x_first_h

#define CommonH
#include "common.h"

void InterfaceInit(void);

void DisplayFirstWindow(void);    //��ʾ������
void PreDealFirstWindow(uint8_t wintype);    //������Ԥ��������������Ч��
void OperateFirstWindow(uint8_t wintype, uint8_t currwindow);    //�����ڲ���
void CheckTouchFirstWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void CheckFirstWindowFlag(void);  //��鷵����ҳ��־

void RefreshBackWindow(uint8_t RefreshType, uint8_t FullPageType); //ˢ�±���
void DisplayStateBar(void);      //״̬��
void PlaySoundTip(void); //������ʾ��

void PreDealBigmenuButton(uint8_t wintype);  //��ݰ��� Ԥ����
void ShowPressQuickmenu_Button(uint8_t wintype);  //��ݰ���   ����
void OperateQuickmenu(uint8_t wintype, uint8_t currtype);

void DisplayInfoStateImage(uint8_t NetState);    //��ʾ��Ϣͼ��
void DisplayPictureStateImage(uint8_t NetState);    //��ʾ��Ӱͼ��

//��lib ����
uint32_t GetLocal_fb_addr(void);
uint8_t GetLocal_fb_page(void);
uint32_t GetLocal_fb1_addr(void);

//uint8_t *Get_jpeg_bitstream(void);
//uint8_t Get_Mjpeg_DecodeError(void);
void Set_Local_DecodeJpegFlag(uint8_t disp_DecodeJpegFlag);
uint8_t Get_Local_DecodeStreamFlag(void);
//---------------------------------------------------------------------------
//��ǰ����  0 -- ��������  1 -- �������   2 -- �豸����   3 -- ��������  4 -- ͨѶ���ܽ���  5 -- ��ݷ���  6 -- ��ý�����   7 -- ���ý���   8 -- �龰ģʽ


//          12 -- ��Ƶ����
//          13 -- �����ſ�
//          14 -- ��¼��ѯ
//          16 -- �Խ�ͼ�񴰿�
//          21 -- ͨ����¼����
//          22 -- �鿴ͨ����¼����

//          31 -- ����
//          32 -- ����
//          34 -- ��������
//          35 -- ȡ����������

//          111 -- �������񴰿�  ��ý��
//          112 -- ��Ϣ������ʾ����

// 	    152 -- ����������
//          149 -- Digital KB1����
//          150 -- KB1����
//          151 -- LAN��ַ����
// 	    154 -- ��������
// 	    158 -- �汾��Ϣ
// 	    159 -- ϵͳ����
//          165 -- ��Ļ����
//          166 -- ��������
//          167 -- ʱ������
//          168 -- ��������
//          169 -- ��������

// 	    160 -- ��������ѡ��

//          188 -- ������ʾ�����������ô���
//          190 -- У׼����������
//          191 -- У׼��������ʾ����
//          192 -- ���Դ���
//���²���Ϊ�ֲ�������

//������
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

//��������
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

//����������
uint32_t setup_button_Faddr[5][2] = {
                                    {0x00320000UL, 0x00322000UL},
                                    {0x00324000UL, 0x00326000UL},
                                    {0x00328000UL, 0x0032A000UL},
                                    {0x0032C000UL, 0x0032E000UL},
                                    {0x00330000UL, 0x00332000UL}
                                   };

//���̴���
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

//LAN���ô���
uint32_t lan_image_Faddr = 0x0033B000UL;
uint32_t lan_button_Faddr[2][2] = {
                                    {0x00335000UL, 0x00336000UL},
                                    {0x00338000UL, 0x00339000UL}
                                   };
uint32_t lan_edit_Faddr[2] = {
                                    0x00345000UL, 0x00346000UL
                                   };

//�������ô���
uint32_t addr_image_Faddr = 0x00350000UL;
uint32_t addr_button_Faddr[2][2] = {
                                    {0x0035B000UL, 0x0035C000UL},
                                    {0x0035E000UL, 0x0035F000UL}
                                   };

//�޸����봰��
uint32_t pass_image_Faddr = 0x003C0000UL;

//��Ļ���ڴ���
uint32_t screen_image_Faddr = 0x003D0000UL;
uint32_t screen_button_Faddr[3][2] = {
                                    {0x003D9000UL, 0x003DA000UL},
                                    {0x003DC000UL, 0x003DD000UL},
                                    {0x003DE000UL, 0x003DF000UL}
                                   };
uint32_t brightness_image_Faddr[6] = {
                                    0x003E0000UL, 0x003E2000UL, 0x003E4000UL, 0x003E6000UL, 0x003E8000UL, 0x003EA000UL
                                   };

//�������ڴ���
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

//���ڴ���
uint32_t version_button_Faddr[2][2] = {
                                    {0x003A0000UL, 0x003A1000UL},
                                    {0x003A2000UL, 0x003A3000UL}
                                   };

//������У׼����
uint32_t calibrate_image_Faddr = 0x003B0000UL;
uint32_t cross_button_Faddr[1][2] = {
                                    {0x003B2000UL, 0x003B3000UL}
                                   };

//���봰��
uint32_t talkpic_image_Faddr = 0x002E0000UL;
uint32_t talkpic_button_Faddr[3][2] = {
                                    {0x002E4000UL, 0x002E5000UL},
                                    {0x002E6000UL, 0x002E7000UL},
                                    {0x002E8000UL, 0x002E9000UL}
                                   };
uint32_t talkwatchpic_image_Faddr  = 0x00250000UL;


//������Ƶ����
uint32_t watchpic_image_Faddr = 0x002EA000UL;
uint32_t watchpic_button_Faddr[1][2] = {
                                    {0x002E4000UL, 0x002E5000UL}
                                    };

//���Ӵ���
uint32_t watchtarget_image_Faddr = 0x00310000UL;
uint32_t watch_Faddr[3][2] = {
                                    {0x00312000UL, 0x00313000UL},
                                    {0x00314000UL, 0x00315000UL},
                                    {0x00316000UL, 0x00318000UL}
                                   };

//��������
uint32_t fortify_button_Faddr[2][2] = {
                                    {0x002F0000UL, 0x002F2000UL},
                                    {0x002F4000UL, 0x002F6000UL}
                                   };

//����ͼ��
uint32_t alarm_main_image_Faddr = 0x00306000;
uint32_t alarm_image_Faddr[2] = {0x002F8000UL, 0x002FE000UL};

//��Ϣ����
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

//��Ϣ�鿴����
uint32_t infowin_image_Faddr = 0x00450000UL;
uint32_t infowin_button_Faddr[3][2] = {
                                    {0x00454000UL, 0x00455000UL},
                                    {0x00456000UL, 0x00457000UL},
                                    {0x00458000UL, 0x00459000UL}
                                   };
uint32_t infocontent_image_Faddr = 0x00453000UL;

//ͨ����¼����
uint32_t records_pic_icon_Faddr[RECORDNUMPERPAGE] = {0x0046A000UL, 0x0046B000UL, 0x0046C000UL, 0x0046D000UL, 0x0046E000UL};
//  struct TImage records_image;
//  struct TImageButton records_button[3];
//  struct TImageButton recordsrow_button[RECORDNUMPERPAGE];

  //ͨ����¼�鿴����
uint32_t picwin_image_Faddr = 0x00480000UL;
uint32_t pic_blank_image_Faddr = 0x00484000UL;
uint32_t picwin_button_Faddr[3][2] = {
                                    {0x00486000UL, 0x00487000UL},
                                    {0x00488000UL, 0x00489000UL},
                                    {0x0048A000UL, 0x0048B000UL}
                                   };

void InterfaceInit(void)     //��ʼ��������Դ
{
  uint8_t i;

  //������
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
  //��������
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
  setup_pass_edit.BoxLen = 0;         //�ı���ǰ���볤��
  setup_pass_edit.MaxLen = 4;         //�ı���������볤��
  setup_pass_edit.Focus = 0;        //��ʾ������
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

  //����������
  for(i=0; i<5; i++)
   {
    setup_button[i].Faddr[0] = setup_button_Faddr[i][0];
    setup_button[i].Faddr[1] = setup_button_Faddr[i][1];
    PreDealImageButton(&setup_button[i], IMAGEUP);
    PreDealImageButton(&setup_button[i], IMAGEDOWN);
   }

  //���̽���
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
  kb_edit.BoxLen = 0;         //�ı���ǰ���볤��
  kb_edit.MaxLen = 4;         //�ı���������볤��
  kb_edit.Focus = 0;        //��ʾ������
  kb_edit.Visible = 0;


  for(i=0; i<15; i++)
   {
    kb_button[i].Faddr[0] = kb_button_Faddr[i][0];
    kb_button[i].Faddr[1] = kb_button_Faddr[i][1];
    PreDealImageButton(&kb_button[i], IMAGEUP);
    PreDealImageButton(&kb_button[i], IMAGEDOWN);
   }

  //LAN���ô���
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

  //�������ô���
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

  //�޸����봰��
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

  //��Ļ���ڴ���
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
  //�������ڴ���
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

  //���ڴ���
  for(i=0; i<2; i++)
   {
    version_button[i].Faddr[0] = version_button_Faddr[i][0];
    version_button[i].Faddr[1] = version_button_Faddr[i][1];
    PreDealImageButton(&version_button[i], IMAGEUP);
    PreDealImageButton(&version_button[i], IMAGEDOWN);
   }

  //������У׼����
  calibrate_image.Faddr = calibrate_image_Faddr;
  PreDealImage(&calibrate_image);

  for(i=0; i<1; i++)
   {
    cross_button[i].Faddr[0] = cross_button_Faddr[i][0];
    cross_button[i].Faddr[1] = cross_button_Faddr[i][1];
    PreDealImageButton(&cross_button[i], IMAGEUP);
    PreDealImageButton(&cross_button[i], IMAGEDOWN);
   }

  //��������
  call_image.Faddr = setup_key_image.Faddr;
  PreDealImage(&call_image);

  r2r_edit = setup_pass_edit;
  r2r_edit.BoxLen = 0;         //�ı���ǰ���볤��
  r2r_edit.MaxLen = 10;         //�ı���������볤��

  //���봰��
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

  //������Ƶ����
  watchpic_image.Faddr = watchpic_image_Faddr;
  PreDealImage(&watchpic_image);

  for(i=0; i<1; i++)
   {
    watchpic_button[i].Faddr[0] = watchpic_button_Faddr[i][0];
    watchpic_button[i].Faddr[1] = watchpic_button_Faddr[i][1];
    PreDealImageButton(&watchpic_button[i], IMAGEUP);
    PreDealImageButton(&watchpic_button[i], IMAGEDOWN);
   }

  //���Ӵ���
  watchtarget_image.Faddr = watchtarget_image_Faddr;
  PreDealImage(&watchtarget_image);

  for(i=0; i<3; i++)
   {
    watch_button[i].Faddr[0] = watch_Faddr[i][0];
    watch_button[i].Faddr[1] = watch_Faddr[i][1];
    PreDealImageButton(&watch_button[i], IMAGEUP);
    PreDealImageButton(&watch_button[i], IMAGEDOWN);
   }

  //��������
  for(i=0; i<2; i++)
   {
    fortify_button[i].Faddr[0] = fortify_button_Faddr[i][0];
    fortify_button[i].Faddr[1] = fortify_button_Faddr[i][1];
    PreDealImageButton(&fortify_button[i], IMAGEUP);
    PreDealImageButton(&fortify_button[i], IMAGEDOWN);
   }

  //��������
  //���봰���ı���
  cancelfortify_edit = setup_pass_edit;
  cancelfortify_edit.BoxLen = 0;         //�ı���ǰ���볤��
  cancelfortify_edit.MaxLen = 4;         //�ı���������볤��

  //����ͼ��
  alarm_main_image.Faddr = alarm_main_image_Faddr;
  PreDealImage(&alarm_main_image);
  
  for(i=0; i<2; i++)
   {
    alarm_image[i].Faddr = alarm_image_Faddr[i];
    PreDealImage(&alarm_image[i]);
   }

  //��Ϣ����
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

  //��Ϣ�鿴����
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

  //ͨ����¼����
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

  //ͨ����¼�鿴����
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
//��lib ����
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
void DisplayFirstWindow(void)    //��ʾ������
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
  //��ʾ״̬��
  DisplayStateBar();   


  Local.CurrentWindow = 0;

  Local.PreStatus = 0;  //20130115 Ԥ״̬����ֹͬʱ�Ĳ���

  //sprintf(ip_text, "%d.%d.%d.%d", LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);
  //display_text(100, 100, 0, COLOR_BLACK, ip_text, 0, 16, FULLPAGE);
  //display_text(100, 120, 0, COLOR_RED, "ABCDefg123", 0, 16, FULLPAGE);
  //display_text(100, 150, 0, COLOR_BLACK, "��ַ�������ַ����", 0, 24, FULLPAGE);
  //display_text(100, 180, 0, COLOR_RED, "ABCDefg123", 0, 24, FULLPAGE); 
  //display_text(100, 150, 0, COLOR_BLACK, "��", 0, 24, FULLPAGE);
  //display_text(100, 150, 0, COLOR_BLACK, "��ַ�������ַ����", 0, 24, FULLPAGE);

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

  Local.FirstWindowFlag = 1;     //������ҳ��־
  Local.FirstWindowTime = 0;     //������ҳ ����

  Local.NoBreak = 0;
  xtm_printf("DisplayFirstWindowIP = %d.%d.%d.%d\n", 
  	LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);

  //StartPlayWav(soundtip_wav.Faddr, NO_CYCLE_PLAY);//NO_CYCLE_PLAY);       // soundtip_wav.Faddr

//  SoundSetSpeakerVolume(LocalCfg.SpkVolume);
   
}
//---------------------------------------------------------------------------
//��鷵����ҳ��־
void CheckFirstWindowFlag(void)
{
  if(Local.CurrentWindow == 0)
   if(Local.FirstWindowFlag == 1)     //������ҳ��־
    {
     Local.FirstWindowTime ++;     //������ҳ ����
     if(Local.FirstWindowTime > 3*TIMERPERSEC)
      {
       Local.FirstWindowFlag = 0;
       Local.FirstWindowTime = 0;
       StopPlayWavFile();
      }
    }
}
//---------------------------------------------------------------------------
void PreDealFirstWindow(uint8_t wintype)    //������Ԥ��������������Ч��
{
  if((wintype >= 0)&&(wintype <= 4))
    DisplayImageButton(&menu_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
}
//---------------------------------------------------------------------------
void OperateFirstWindow(uint8_t wintype, uint8_t currwindow)    //�����ڲ���
{
  if(wintype < 5)
    DisplayImageButton(&menu_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
  printf("OperateFirstWindow::wintype = %d, currwindow = %d, Local.isHost = %d\n", wintype, currwindow, Local.isHost);
  switch(wintype)
   {
    case 0:
             if(Local.isHost != '0')  //����, ����������Ч
                break;    
             switch(LocalCfg.DefenceStatus)
              {
               case 0://δ����
                      if(Local.CurrentWindow != 32)
                        DisplayFortifyWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
                      break;
               case 1://�������
               case 2://�ڼҲ���
               case 11://���������ʱ��
               case 12://�ڼҲ�����ʱ��
                      if(Local.CurrentWindow != 31)
                        DisplayCancelFortifyWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
                      break;
              }
           break;
    case 1://��ʾ�Խ���������
           DisplayTalkWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
           break;
    case 2://��ʾ���Ӵ���
           DisplayWatchWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);    //ˢ�� �� ��
           break;
    case 3://��ʾ��Ϣ����
           DisplayInfoWindow(REFRESH_MIDDLE | REFRESH_BOTTOM, 0);    //ˢ�� �� ��
           break;
    case 4://��ʾͨ����¼����
           DisplayRecordsWindow(REFRESH_MIDDLE | REFRESH_BOTTOM, 0);    //ˢ�� �� ��
           break;
    case 5: //����
           DisplaySetupWindow(REFRESH_MIDDLE | REFRESH_BOTTOM);
           break;

   }
}
//---------------------------------------------------------------------------
void CheckTouchFirstWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<5; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&menu_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   for(i=2; i<6; i+=2)  //���ڰ�ť
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
void RefreshBackWindow(uint8_t RefreshType, uint8_t FullPageType) //ˢ�±���
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
  //0--�������
  //1--��������
  //2--���¶���
  //3--���¶���
  //4--������Ӱ
  //5--������Ӱ
  uint8_t i;

  if(Local.isHost != '0')  //����, ��������Ч
    LocalCfg.DefenceStatus = 0;

  if((LocalCfg.DefenceStatus != 0)&&(LocalCfg.DefenceStatus != 1)&&(LocalCfg.DefenceStatus != 2)&&(LocalCfg.DefenceStatus != 11)&&(LocalCfg.DefenceStatus != 12))
    LocalCfg.DefenceStatus = 0;    
      
  if(Local.NetLanStatus == 1)  //����״̬ 0 �Ͽ�  1 ��ͨ
    DisplayImage(&state_image[1], FROMFILE, FULLPAGE);
  else
    DisplayImage(&state_image[0], FROMFILE, FULLPAGE);

  //������Ϣ
  if(Local.NewInfo == 1)
    DisplayImage(&state_image[3], FROMFILE, FULLPAGE);
  else
    DisplayImage(&state_image[2], FROMFILE, FULLPAGE);

  //������Ӱ
  if(Local.NewRecPic == 1)
    DisplayImage(&state_image[5], FROMFILE, FULLPAGE);
  else
    DisplayImage(&state_image[4], FROMFILE, FULLPAGE);

/*  //����״̬
  if((LocalCfg.DefenceStatus == 1)||(LocalCfg.DefenceStatus == 2)||(LocalCfg.DefenceStatus == 3)||(LocalCfg.DefenceStatus == 4)||(LocalCfg.DefenceStatus == 5))
    DisplayImage(&state_image[3], MAINYUVFBPAGE, FROMMEMORY);
  else
    DisplayImage(&state_image[2], MAINYUVFBPAGE, FROMMEMORY);      */






/*  DisplayClock(REFRESH); //��ʾʱ��
  if(Local.interface_init_flag == 1)
    ShowWeather();  //��ʾ����     */
}
//---------------------------------------------------------------------------
void PlaySoundTip(void) //������ʾ��
{
  if(LocalCfg.VoiceHint == 1)
   {
    WaitAudioUnuse(200); //�ȴ���������               //  wav_1k[5].Faddr
    StartPlayWav(soundtip_wav.Faddr, NO_CYCLE_PLAY);       // soundtip_wav.Faddr
   }
}
//---------------------------------------------------------------------------
void PreDealBigmenuButton(uint8_t wintype)  //��ݰ��� Ԥ����
{
  if((wintype >= 100)&&(wintype <= 106))
    DisplayImageButton(&bigmenu_button[wintype-100], IMAGEDOWN, FROMFILE, NOFULLPAGE);
}
//---------------------------------------------------------------------------
void ShowPressQuickmenu_Button(uint8_t wintype)  //��ݰ���  ����
{
  if((wintype >= 100)&&(wintype <= 106))
    DisplayImageButton(&bigmenu_button[wintype-100], IMAGEUP, FROMFILE, NOFULLPAGE);
}
//---------------------------------------------------------------------------
void OperateQuickmenu(uint8_t wintype, uint8_t currtype)
{
  switch(wintype)
   {
    case 100:  //��ҳ
           if(currtype != wintype)
             DisplayFirstWindow();
           break;
    case 101:  //����
          if(Local.isHost != '0')  //����, ����������Ч
             break;
          if(currtype != wintype)
             switch(LocalCfg.DefenceStatus)
              {
               case 0://δ����
                      if(Local.CurrentWindow != 32)
                        DisplayFortifyWindow(REFRESH_MIDDLE);
                      break;
               case 1://�������
               case 2://�ڼҲ���
               case 11://���������ʱ��
               case 12://�ڼҲ�����ʱ��
                      if(Local.CurrentWindow != 31)
                        DisplayCancelFortifyWindow(REFRESH_MIDDLE);
                      break;
              }
           break;
    case 102:  //����
           if(currtype != wintype)
             DisplayTalkWindow(REFRESH_MIDDLE);
           break;
    case 103:  //����
           if(currtype != wintype)
             DisplayWatchWindow(REFRESH_MIDDLE);    //ˢ�� �� 
           break;
    case 104: //��Ϣ
           if(currtype != wintype)
             DisplayInfoWindow(REFRESH_MIDDLE, 0);    //ˢ�� ��
           break;
    case 105:  //��¼
           if(currtype != wintype)
               DisplayRecordsWindow(REFRESH, 0);    //��ʾ��Ƭ���ڣ�һ����
           break;
    case 106:  //ϵͳ����
           if(currtype != wintype)
             DisplaySetupWindow(REFRESH_MIDDLE);
           break;
   }
}
//---------------------------------------------------------------------------
void DisplayInfoStateImage(uint8_t State)    //��ʾ��Ϣͼ��
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
void DisplayPictureStateImage(uint8_t State)    //��ʾ��Ӱͼ��
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
