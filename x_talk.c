

#include "stdlib.h"

#define x_talk_h

#define CommonH
#include "common.h"

void Talk_Func(void);       //ͨ�� ����
void CallCenter_Func(char *CenterAddr);  //��������
void CallR2R_Func(char *DestAddr);    //�����Խ�
void Watch_Func(void);       //����
void TalkEnd_Func(void);
void WatchEnd_Func(void);
void CallTimeOut_Func(void); //���г�ʱ
void OpenLock_Func(void);  //����

void CreateTalkBuff(uint8_t *sendb, uint8_t Order1, uint8_t Order2, uint8_t Order3, uint8_t *Remote_Addr, uint8_t *Remote_IP);

void CreateNSBuff(uint8_t *sendb, uint8_t Order1, uint8_t Order2);

void DisplayTalkWindow(uint8_t RefreshType);    //��ʾ�Խ���������
void PreDealTalkWindow(uint8_t wintype);    //�Խ���������Ԥ������������Ч��
void OperateTalkWindow(uint8_t wintype, uint8_t currwindow);    //�Խ��������ڲ���
void CheckTouchTalkWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

struct TalkCall1 TalkCall;
void ClearTalkText(void);
void CheckBuildingText(uint8_t tmplen); //��鴱������

void DisplayWatchWindow(uint8_t RefreshType);    //��ʾ���Ӵ���
void PreDealWatchWindow(uint8_t wintype);    //���Ӵ���Ԥ������������Ч��
void OperateWatchWindow(uint8_t wintype, uint8_t currwindow);    //���Ӵ��ڲ���
void CheckTouchWatchWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

#ifdef ELEVATORCONTROL_HANGZHOU
 void ElevatorControl_Func(void);  //���Ƶ���
#endif

  char S_WatchTarget[LANGUAGEMAXNUM][6][20] = {
                                                {"��Ԫ1", "��Ԫ2", "��Ԫ3", "��Ԫ4", "����1", "����2"},
                                                {"Unit 1", "Unit 2", "Unit 3", "Unit 4", "Second 1", "Second 2"}
                                               };

  char B_WatchTarget[LANGUAGEMAXNUM][2][20] = {
                                                {"����1", "����2"},
                                                {"Second 1", "Second 2"}
                                               };
void ShowWatchTarget(uint8_t FullPageType);

void DisplayTalkWatchPicWindow(uint8_t ReturnType, uint8_t WinType);    //��ʾ�Խ����봰��
void PreDealTalkWatchPicWindow(uint8_t wintype);    //�Խ����봰��Ԥ������������Ч��
void OperateTalkWatchPicWindow(uint8_t wintype, uint8_t currwindow);    //�Խ����봰�ڲ���
void CheckTouchTalkWatchPicWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

extern uint8_t Udp_InitFlag;

//---------------------------------------------------------------------------
void CreateTalkBuff(uint8_t *sendb, uint8_t Order1, uint8_t Order2, uint8_t Order3, uint8_t *Remote_Addr, uint8_t *Remote_IP)
{
    //ͷ��
    memcpy(sendb, UdpPackageHead, 6);
    //����  ,�����㲥����
    sendb[6] = Order1;
    //Multi_Udp_Buff[i].CurrOrder = Multi_Udp_Buff[i].buf[6];
    sendb[7] = Order2;    //����
    sendb[8] = Order3;


    //����Ϊ���з�
    if((Local.Status == 1)||(Local.Status == 3)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9)
       ||(Local.Status == 0))
     {
      memcpy(sendb+9, LocalCfg.Addr, 20);
      memcpy(sendb+29, LocalCfg.IP, 4);
      memcpy(sendb+33, Remote_Addr, 20);
      memcpy(sendb+53, Remote_IP, 4);
     }
    //����Ϊ���з�
    if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
     {
      memcpy(sendb+9, Remote_Addr, 20);
      memcpy(sendb+29, Remote_IP, 4);
      memcpy(sendb+33, LocalCfg.Addr, 20);
      memcpy(sendb+53, LocalCfg.IP, 4);
     }
}
//---------------------------------------------------------------------------
void CreateNSBuff(uint8_t *sendb, uint8_t Order1, uint8_t Order2)
{
    //ͷ��
    memcpy(sendb, UdpPackageHead, 6);
    //����  ,�����㲥����
    sendb[6] = Order1;
    sendb[7] = Order2;    //����

    //����Ϊ���з�
    memcpy(sendb+8, LocalCfg.Addr, 20);
    memcpy(sendb+28, LocalCfg.IP, 4);
    memcpy(sendb+32, Remote.Addr[0], 20);
    Remote.IP[0][0] = 0;
    Remote.IP[0][1] = 0;
    Remote.IP[0][2] = 0;
    Remote.IP[0][3] = 0;
    memcpy(sendb+52, Remote.IP[0], 4);
}
//---------------------------------------------------------------------------
void Talk_Func(void)       //ͨ�� ����
{
  uint8_t sendb[128];
  uint16_t sendlength;
  char RemoteIP[20];

  if(Local.Status == 2)  //״̬Ϊ���Խ�
   {
    
    StopPlayWavFile();  //�ر�����
    if(LocalCfg.EncodeFrameSize == VIDEO_VGA)
      Local.VideoType = 0x10;
    else
      Local.VideoType = 0x08;
    CreateTalkBuff(sendb, VIDEOTALK, Local.MachineType | Local.VideoType | ASK, CALLSTART, Remote.Addr[0], Remote.IP[0]);

    sendlength = 57;
	sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
	xtm_printf("Talk_Func, RemoteIP = %s\n", RemoteIP);
    CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, 0);
	
   }
}
//---------------------------------------------------------------------------
void TalkEnd_Func(void)
{
  uint8_t sendb[128];
  uint16_t sendlength;
  char RemoteIP[20];
  uint8_t j;
  if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
    ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))  //״̬Ϊ�Խ�
     {
      xtm_printf("TalkEnd_Func, Remote.DenNum = %d, Remote.IP[0] = %d.%d.%d.%d\n", Remote.DenNum, Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
      for(j=0; j<Remote.DenNum; j++)
       {
        CreateTalkBuff(sendb, VIDEOTALK, Local.MachineType | ASK, CALLEND, Remote.Addr[j], Remote.IP[j]);

        sendlength = 57;
		sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2], Remote.IP[j][3]);

        CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, 0);
       }
     }
}
//---------------------------------------------------------------------------
void Watch_Func(void)       //����
{
  uint8_t sendb[128];
  uint16_t sendlength;
  uint8_t Order;
  char RemoteIP[20];

  if(Udp_InitFlag == 0)
  	return;

  Local.ForceEndWatch = 0;  //�к���ʱ��ǿ�ƹؼ���
  memcpy(Remote.Addr[0], NullAddr, 20);
  switch(LocalCfg.Addr[0])
   {
    case 'S':
             switch(Local.WatchTarget)
                {
                  case 0:   //��Ԫ1
                  case 1:   //��Ԫ2
                  case 2:   //��Ԫ3
                  case 3:   //��Ԫ4
                         Remote.Addr[0][0] = 'M';
                         memcpy(Remote.Addr[0] + 1, LocalCfg.Addr + 1, 6);
                         Remote.Addr[0][7] = Local.WatchTarget + '0';
                         break;
                  case 4:   //����1
                  case 5:   //����2
                         Remote.Addr[0][0] = 'H';
                         memcpy(Remote.Addr[0] + 1, LocalCfg.Addr + 1, 10);
                         Remote.Addr[0][11] = Local.WatchTarget - 4 + '0';
                         break;
                }
             break;
    case 'B':
             switch(Local.WatchTarget)
                {
                 case 0:  //����1
                 case 1:  //����2
                        Remote.Addr[0][0] = 'H';
                        memcpy(Remote.Addr[0] + 1, LocalCfg.Addr + 1, 4);
                        Remote.Addr[0][11] = Local.WatchTarget + '0';
                        break;
                 case 2:
                 case 3:
                 case 4:
                 case 5:                 
                        return;
                }
             break;
   }
  //strcpy(Remote.Addr[0], "H00010108090");
  Remote.Addr[0][12] = '\0';
  xtm_printf("Remote.Addr[0] = %s\n", Remote.Addr[0]);
  if((Local.Status == 0)&&(PicStatBuf.Flag == 0))
   {

     #if 1
      CreateNSBuff(sendb, NSORDER, ASK);

      sendlength = 56;
	  sprintf(RemoteIP, "%d.%d.%d.%d", Local.NsMultiAddr[0], Local.NsMultiAddr[1], Local.NsMultiAddr[2], Local.NsMultiAddr[3]);

      CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, VIDEOWATCH);
     #else
      xtm_printf("Watch_Func::Remote.Addr[0] = %s\n", Remote.Addr[0]);
      if(QueryAddrTableFile(Remote.Addr[0], Remote.IP[0]) == 1)
       {
           Remote.DenNum = 1;
           Remote.GroupIP[0] = 236;
           Remote.GroupIP[1] = LocalCfg.IP[1];
           Remote.GroupIP[2] = LocalCfg.IP[2];
           Remote.GroupIP[3] = LocalCfg.IP[3];
           Order = Local.MachineType | ASK;
           CreateTalkBuff(sendb, VIDEOWATCH, Order, CALL, Remote.Addr[0], Remote.IP[0]);

           sendlength = 62;
		   sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
           CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, 0);
           xtm_printf("���ҵ�ַ��ɹ�,���ڷ������ %d.%d.%d.%d\n", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
       }
      else
       {
        xtm_printf("���ҵ�ַ��ʧ��\n");
        PicStatBuf.Type = Local.CurrentWindow;
        PicStatBuf.Time = 0;
        PicStatBuf.Flag = 1;
       }
     #endif
   }
  else
   {
    xtm_printf("������æ,�޷�����\n");
   }
}
//---------------------------------------------------------------------------
void WatchEnd_Func(void)
{
  uint8_t sendb[128];
  uint16_t sendlength;
  char RemoteIP[20];
  if((Local.Status == 3)||(Local.Status == 4))  //״̬Ϊ����
   {
    xtm_printf("WatchEnd_Func \n");
    CreateTalkBuff(sendb, VIDEOWATCH, Local.MachineType | ASK, CALLEND, Remote.Addr[0], Remote.IP[0]);

    sendlength = 57;
	sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
    CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, 0);
   }
}
//---------------------------------------------------------------------------
void CallCenter_Func(char *CenterAddr)  //��������
{
  uint8_t sendb[128];
  uint16_t sendlength;
  char RemoteIP[20];

  if(Udp_InitFlag == 0)
  	return;
  
  Local.ForceEndWatch = 0;  //�к���ʱ��ǿ�ƹؼ���
  memcpy(Remote.Addr[0], NullAddr, 20);
  memcpy(Remote.Addr[0], CenterAddr, 12);
  Remote.Addr[0][12] = '\0';
  xtm_printf("Remote.Addr[0] = %s\n", Remote.Addr[0]);
  if((Local.Status == 0)&&(PicStatBuf.Flag == 0))
   {
      CreateNSBuff(sendb, NSORDER, ASK);

      sendlength = 56;
	  sprintf(RemoteIP, "%d.%d.%d.%d", Local.NsMultiAddr[0], Local.NsMultiAddr[1], Local.NsMultiAddr[2], Local.NsMultiAddr[3]);

      CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, VIDEOTALK);
   }
  else
   {
    xtm_printf("������æ,�޷�����\n");
   }
}
//---------------------------------------------------------------------------
void CallR2R_Func(char *DestAddr)    //�����Խ�
{
  int i;
  uint8_t sendb[128];
  uint16_t sendlength;
  char RemoteIP[20];

  if(Udp_InitFlag == 0)
  	return;
  
  Local.ForceEndWatch = 0;  //�к���ʱ��ǿ�ƹؼ���
  memcpy(Remote.Addr[0], NullAddr, 20);
  xtm_printf("CallR2R_Func::DestAddr = %s\n", DestAddr);
  if(strlen(DestAddr) == 4)
   { //Ϊ������
    Remote.Addr[0][0] = 'B';
    memcpy(Remote.Addr[0] + 1, DestAddr, 4);
    Remote.Addr[0][5] = '0';
   }
  else
   { //Ϊ��ͨ���ڻ�
    Remote.Addr[0][0] = 'S';
    memcpy(Remote.Addr[0] + 1, DestAddr, 10);
    Remote.Addr[0][11] = '0';
   }
  xtm_printf("Remote.Addr[0] = %s, PicStatBuf.Flag = %d\n", Remote.Addr[0], PicStatBuf.Flag);
  if((Local.Status == 0)&&(PicStatBuf.Flag == 0))
   {
      CreateNSBuff(sendb, NSORDER, ASK);

      sendlength = 56;
	  sprintf(RemoteIP, "%d.%d.%d.%d", Local.NsMultiAddr[0], Local.NsMultiAddr[1], Local.NsMultiAddr[2], Local.NsMultiAddr[3]);
      CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, VIDEOTALK);
   }
  else
   {
    xtm_printf("������æ,�޷�����\n");
   }
}
//---------------------------------------------------------------------------
//���г�ʱ
void CallTimeOut_Func(void)
{
  //StopPlayWavFile();  //�ر�����
  //�籾��Ϊ���з�
  //�鿴SD�Ƿ���ں���Ӱ��־ �� ���з���ַΪΧǽ�����ſڻ��ͱ����ſڻ�
  xtm_printf("Local.Status = %d, LocalCfg.RecPhoto = %d, Remote.Addr[0][0] = %c\n", \
         Local.Status, LocalCfg.RecPhoto, Remote.Addr[0][0]);

  if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
    ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))
      TalkEnd_Func();
  if(Local.Status == 0)
    Local.OnlineFlag = 0;

  xtm_printf("���г�ʱ\n");
}
//---------------------------------------------------------------------------
void OpenLock_Func(void)  //����
{
  uint8_t sendb[128];
  uint16_t sendlength;
  char RemoteIP[20];
  if((Remote.Addr[0][0] != 'M')&&(Remote.Addr[0][0] != 'W')&&(Remote.Addr[0][0] != 'H'))
    return;
  CreateTalkBuff(sendb, VIDEOTALK, Local.MachineType | ASK, REMOTEOPENLOCK, Remote.Addr[0], Remote.IP[0]);

  sendlength = 57;
  sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
  CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, 0);
}
//--------------------------------------------------------------------------
void DisplayTalkWindow(uint8_t RefreshType)    //��ʾ�Խ���������
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

  DisplayEdit(&r2r_edit, IMAGEUP, FROMFILE, FULLPAGE);

  DisplayImage(&call_image, FROMFILE, FULLPAGE);

  display_text(r2r_edit.xLeft, r2r_edit.yTop - 32, 0, COLOR_BLACK, "�������ַ,\"#\"����,\"*\"ȡ��:", 0, 24, FULLPAGE);

  strcpy(num_button[12].buttonname, "��");
  strcpy(num_button[13].buttonname, "��Ԫ");
  strcpy(num_button[14].buttonname, "����");
  //��ť
  for(i=12; i<15; i++)
   {
    num_button[i].FontType = 24;
    num_button[i].FontColor = COLOR_BLACK;
    num_button[i].Text_xLeft = (num_button[i].width - strlen(num_button[i].buttonname)*12)/2;
    num_button[i].Text_yTop = (num_button[i].height - 24)/2;
    //xtm_printf("%d   %d\n", num_button[i].Text_xLeft, num_button[i].Text_yTop);
   }
   
  for(i=12; i<15; i++)
    DisplayImageButton_Text(&num_button[i], num_button[i].FontColor, 2, FULLPAGE);
  Local.CurrentWindow = 12;
  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);  

  TalkCall.BuildFlag = 1;   //��
  TalkCall.Build[0] = '\0';
  TalkCall.BuildMaxlen = 4;
  TalkCall.UnitFlag = 0;    //��Ԫ
  TalkCall.Unit[0] = '\0';
  TalkCall.UnitMaxlen = 2;
  TalkCall.RoomFlag = 0;    //����
  TalkCall.Room[0] = '\0';
  TalkCall.RoomMaxlen = 4;
  TalkCall.Len = 0;
  r2r_edit.BoxLen = 0;

  TalkCall.TalkText[0] = '\0';
}
//--------------------------------------------------------------------------
void PreDealTalkWindow(uint8_t wintype)    //�Խ���������Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 14))
   {
    DisplayImageButton(&num_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  if((wintype >= 11)&&(wintype <= 14))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//--------------------------------------------------------------------------
void OperateTalkWindow(uint8_t wintype, uint8_t currwindow)    //�Խ��������ڲ���
{
  uint8_t i;
  uint8_t InputOK;
  int tmplen;
  char tmpchar[10];  
  char str[5];
  if(wintype < 15)
    DisplayImageButton(&num_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
  if((wintype >= 11)&&(wintype <= 14))
   {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
   }

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:

         //  if(r2r_edit[CurrBox].BoxLen < r2r_edit[CurrBox].MaxLen)
            {
             if((TalkCall.BuildFlag == 0)&&(TalkCall.UnitFlag == 0)&&(TalkCall.RoomFlag == 0))
               break;

             str[0] = '0' + wintype;
             str[1] = '\0';

             if(TalkCall.BuildFlag == 1)   //��
              {
               if(TalkCall.Len >= TalkCall.BuildMaxlen)
                 break;
               strcat(TalkCall.Build, str);
               TalkCall.Len ++;
              }
             if(TalkCall.UnitFlag == 1)   //��Ԫ
              {
               if(TalkCall.Len >= TalkCall.UnitMaxlen)
                 break;
               strcat(TalkCall.Unit, str);
               TalkCall.Len ++;
              }
             if(TalkCall.RoomFlag == 1)       //����
              {
               if(TalkCall.Len >= TalkCall.RoomMaxlen)
                 break;
               strcat(TalkCall.Room, str);
               TalkCall.Len ++;
              }

             strcat(r2r_edit.Text, str);
             xtm_printf("str = %s, r2r_edit.BoxLen = %d\n", str, r2r_edit.BoxLen);
             display_text(r2r_edit.xLeft + r2r_edit.CursorX + r2r_edit.BoxLen*r2r_edit.fWidth,
                     r2r_edit.yTop + r2r_edit.CursorY, r2r_edit.Space, r2r_edit.CursorCorlor, str, 0, 24, NOFULLPAGE);

             r2r_edit.BoxLen ++;
            }
           break;
    case 10: //���
          ClearTalkText();
          break;
    case 11: //����
       #if 0
           if(LocalCfg.Addr[10] != '9')
            {
             strcpy(Remote.Addr[0], "S00010308020");
             strcpy(TalkCall.TalkText, "0001030802");

             DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, TALKWINTYPE);
             CallR2R_Func(TalkCall.TalkText);
             break;
            }
       #endif
          InputOK = 0;
          if(TalkCall.RoomFlag == 1)   //����
           {
            tmplen = strlen(TalkCall.Room);
            if(tmplen == 0)
              break;           
            TalkCall.RoomFlag = 0;
            
            if(strlen(TalkCall.Room) != TalkCall.RoomMaxlen)
             {
              strcpy(tmpchar, TalkCall.Room);
              switch(tmplen)
               {
                case 1:
                       sprintf(TalkCall.Room, "000%s", tmpchar);
                       break;
                case 2:
                       sprintf(TalkCall.Room, "00%s", tmpchar);
                       break;
                case 3:
                       sprintf(TalkCall.Room, "0%s", tmpchar);
                       break;
               }
             }
            TalkCall.Len = 0;
            DisplayEdit(&r2r_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
            sprintf(r2r_edit.Text, "%s��%s��Ԫ%s��", TalkCall.Build, TalkCall.Unit, TalkCall.Room);
            r2r_edit.BoxLen = strlen(r2r_edit.Text);

            display_text(r2r_edit.xLeft + r2r_edit.CursorX ,
                     r2r_edit.yTop + r2r_edit.CursorY, r2r_edit.Space, r2r_edit.CursorCorlor, r2r_edit.Text, 0, 24, NOFULLPAGE);
           }
          tmplen = strlen(TalkCall.Build);
          if((tmplen > 0)&&(tmplen < 4)&&(strlen(TalkCall.Unit) == 0)&&(strlen(TalkCall.Room) == 0))
            CheckBuildingText(tmplen); //��鴱������
          if((strlen(TalkCall.Build) == 4)&&(strlen(TalkCall.Unit) == 0)&&(strlen(TalkCall.Room) == 0))
            InputOK = 1;
          if((strlen(TalkCall.Build) == 4)&&(strlen(TalkCall.Unit) == 2)&&(strlen(TalkCall.Room) == 4))
            InputOK = 1;

           for(i=0; i<20; i++)
             TalkCall.TalkText[i] = '\0';
           sprintf(TalkCall.TalkText, "%s%s%s", TalkCall.Build, TalkCall.Unit, TalkCall.Room);
           xtm_printf("TalkCall.TalkText = %s, TalkCall.Build = %s, TalkCall.Unit = %s, TalkCall.Room = %s\n", TalkCall.TalkText, TalkCall.Build, TalkCall.Unit, TalkCall.Room);
           if(strlen(TalkCall.TalkText) == 4)
            { //Ϊ������
             memcpy(Remote.Addr[0],"000000000000",12);
             Remote.Addr[0][0] = 'B';
             memcpy(Remote.Addr[0] + 1, TalkCall.TalkText, 4);
             Remote.Addr[0][11] = '0';
             Remote.Addr[0][12] = '\0';
            }
           else
            { //Ϊ��ͨ���ڻ�
             Remote.Addr[0][0] = 'S';
             memcpy(Remote.Addr[0] + 1, TalkCall.TalkText, 10);
             Remote.Addr[0][11] = '0';
             Remote.Addr[0][12] = '\0';
            }
            
          //20101030   �޸�Ϊ���ܺ��б���
         #if 0
          strcpy(DestAddr, Remote.Addr[0]);
          DestAddr[Local.AddrLen - 1] = '0';
          strcpy(LocalAddr, LocalCfg.Addr);
          LocalAddr[Local.AddrLen - 1] = '0';
          if(strcmp(DestAddr, LocalAddr) == 0)
           {
            P_Debug("Ŀ���ַΪ�������޷�����\n");
            InputOK = 0;
            ClearTalkText();
            break;
           }
         #endif

          if(InputOK == 0)   //��Ԫ
            {
              if(LocalCfg.VoiceHint == 1)
               {
                PlayInputErrorWav();
               }
             break;
            }

           DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, TALKWINTYPE);
           CallR2R_Func(TalkCall.TalkText);
          break;
    case 12: //��
          //xtm_printf("TalkCall.BuildFlag = %d\n", TalkCall.BuildFlag);
          if(TalkCall.BuildFlag == 1)   //��
           {
            tmplen = strlen(TalkCall.Build);
            if(tmplen == 0)
              break;
            TalkCall.BuildFlag = 0;
            CheckBuildingText(tmplen); //��鴱������

            TalkCall.UnitFlag = 1;    //��Ԫ
            TalkCall.Len = 0;
           }
          break;
    case 13: //��Ԫ
          if(TalkCall.UnitFlag == 1)   //��Ԫ
           {
            tmplen = strlen(TalkCall.Unit);
            if(tmplen == 0)
              break;
            TalkCall.UnitFlag = 0;

            if(strlen(TalkCall.Unit) != TalkCall.UnitMaxlen)
             {
              strcpy(tmpchar, TalkCall.Unit);
              switch(tmplen)
               {
                case 1:
                       TalkCall.Unit[0] = '0';
                       TalkCall.Unit[1] = tmpchar[0];
                       TalkCall.Unit[2] = '\0';
                       break;
               }
             }
            TalkCall.RoomFlag = 1;    //����
            TalkCall.Len = 0;
            DisplayEdit(&r2r_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
            strcpy(r2r_edit.Text, TalkCall.Build);
            if(LocalCfg.Language == 2)  //Ӣ��
             {
              strcat(r2r_edit.Text, " B");
              strcat(r2r_edit.Text, TalkCall.Unit);
              strcat(r2r_edit.Text, " U");
              r2r_edit.BoxLen = 10;
             }
            else
             {
              strcat(r2r_edit.Text, "��");
              strcat(r2r_edit.Text, TalkCall.Unit);
              strcat(r2r_edit.Text, "��Ԫ");
              r2r_edit.BoxLen = 12;
             }

           // r2r_edit.BoxLen = strlen(r2r_edit.Text);
            xtm_printf("r2r_edit.BoxLen = %d\n", r2r_edit.BoxLen);
            if(LocalCfg.Language == 2)  //Ӣ��
              Local.Text_Space = 0;
            else
              Local.Text_Space = 3;
            display_text(r2r_edit.xLeft + r2r_edit.CursorX,
                     r2r_edit.yTop + r2r_edit.CursorY, r2r_edit.Space, r2r_edit.CursorCorlor, r2r_edit.Text, 0, 24, NOFULLPAGE);
           }
          break;
    case 14: //����
          if((Local.Status == 0)&&(Local.CurrentWindow != 16))
           {
            memcpy(Remote.Addr[0],"Z00010000000",12);
            Remote.Addr[0][12] = '\0';
            DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, TALKWINTYPE);
            CallCenter_Func("Z00010000000");
            break;
           }
          break;
    default:
          OperateQuickmenu(wintype, 102);
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchTalkWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<15; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&num_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   {
    TouchNum = CheckBigmenuFunc(XLCD, YLCD);  //��ݰ�ť����
    if(TouchNum >= 100)
      isInTouch = 1;
   }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
void ClearTalkText(void)
{
  TalkCall.BuildFlag = 1;   //��
  TalkCall.Build[0] = '\0';
  TalkCall.UnitFlag = 0;    //��Ԫ
  TalkCall.Unit[0] = '\0';
  TalkCall.RoomFlag = 0;    //����
  TalkCall.Room[0] = '\0';

  TalkCall.Len = 0;

  DisplayEdit(&r2r_edit, IMAGEUP, FROMFILE, NOFULLPAGE);

  r2r_edit.BoxLen = 0;
  r2r_edit.Text[r2r_edit.BoxLen] = '\0';
}
//--------------------------------------------------------------------------
void CheckBuildingText(uint8_t tmplen) //��鴱������
{
  char tmpchar[10];
  
  if(tmplen != TalkCall.BuildMaxlen)
   {
    strcpy(tmpchar, TalkCall.Build);
    switch(tmplen)
     {
      case 1:
             TalkCall.Build[0] = '0';
             TalkCall.Build[1] = '0';
             TalkCall.Build[2] = '0';
             TalkCall.Build[3] = tmpchar[0];
             break;
      case 2:
             TalkCall.Build[0] = '0';
             TalkCall.Build[1] = '0';
             TalkCall.Build[2] = tmpchar[0];
             TalkCall.Build[3] = tmpchar[1];
             break;
      case 3:
             TalkCall.Build[0] = '0';
             TalkCall.Build[1] = tmpchar[0];
             TalkCall.Build[2] = tmpchar[1];
             TalkCall.Build[3] = tmpchar[2];
             break;
     }
   }
  TalkCall.Build[4] = '\0'; 
  DisplayEdit(&r2r_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
  strcpy(r2r_edit.Text, TalkCall.Build);
  if(LocalCfg.Language == 2)  //Ӣ��
    strcat(r2r_edit.Text, " B");
  else
    strcat(r2r_edit.Text, "��");

  r2r_edit.BoxLen = 6;
  //r2r_edit.BoxLen = strlen(r2r_edit.Text);
  if(LocalCfg.Language == 2)  //Ӣ��
    Local.Text_Space = 0;
  else
    Local.Text_Space = 3;
  display_text(r2r_edit.xLeft + r2r_edit.CursorX,
               r2r_edit.yTop + r2r_edit.CursorY, r2r_edit.Space, r2r_edit.CursorCorlor, r2r_edit.Text, 0, 24, NOFULLPAGE);
}
//--------------------------------------------------------------------------
void DisplayWatchWindow(uint8_t RefreshType)    //��ʾ���Ӵ���
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

  for(i=0; i<3; i++)
    DisplayImageButton(&watch_button[i], IMAGEUP, FROMFILE, FULLPAGE);

  Local.WatchTarget = 0;  
  ShowWatchTarget(FULLPAGE);
  Local.CurrentWindow = 13;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);  
}
//--------------------------------------------------------------------------
void PreDealWatchWindow(uint8_t wintype)    //���Ӵ���Ԥ������������Ч��
{
  xtm_printf("PreDealWatchWindow:: wintype = %d\n", wintype);
  if((wintype >= 0)&&(wintype <= 2))
   {
    DisplayImageButton(&watch_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//--------------------------------------------------------------------------
void ShowWatchTarget(uint8_t FullPageType)
{
  uint16_t xLeft, yTop;
  char WatchTarget_Text[30];

  //xtm_printf("ShowWatchTarget 1\n");
  DisplayImage(&watchtarget_image, FROMFILE, FullPageType);
  //xtm_printf("ShowWatchTarget 2\n");
  switch(LocalCfg.Addr[0])
   {
    case 'S':
             Local.WatchTargetMaxNum = 6; //����Ŀ���������
             strcpy(WatchTarget_Text, S_WatchTarget[Local.LanguageNo][Local.WatchTarget]);
             break;
    case 'B':
             Local.WatchTargetMaxNum = 2; //����Ŀ���������
             strcpy(WatchTarget_Text, B_WatchTarget[Local.LanguageNo][Local.WatchTarget]);
             break;
   }
  xtm_printf("ShowWatchTarget::WatchTarget_Text = %s, Local.WatchTarget = %d, Local.LanguageNo = %d\n", WatchTarget_Text, Local.WatchTarget, Local.LanguageNo);
  xLeft = watchtarget_image.xLeft + (watchtarget_image.width - strlen(WatchTarget_Text)*12)/2;;
  yTop = watchtarget_image.yTop + (watchtarget_image.height - 24)/2;
  display_text(xLeft,
                     yTop, 2, COLOR_WHITE, WatchTarget_Text, 0, 24, FullPageType);
}
//---------------------------------------------------------------------------
void OperateWatchWindow(uint8_t wintype, uint8_t currwindow)    //���Ӵ��ڲ���
{
  uint8_t i;
  xtm_printf("OperateWatchWindow:: wintype = %d\n", wintype);
  if(wintype < 3)
   {
    DisplayImageButton(&watch_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
   } 

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����
  xtm_printf("OperateWatchWindow:: 2 wintype = %d\n", wintype);
  switch(wintype)
   {
    case 0: //��
           if(Local.Status == 0)
            {
             if(Local.WatchTarget > 0)
              {
               Local.WatchTarget --;
               ShowWatchTarget(NOFULLPAGE);
              }
             else
              {
               Local.WatchTarget = Local.WatchTargetMaxNum - 1;
               ShowWatchTarget(NOFULLPAGE);
              }
            }
           break;
    case 1: //��
           if(Local.Status == 0)
            {
             if(Local.WatchTarget < (Local.WatchTargetMaxNum - 1))
              {
               Local.WatchTarget ++;
               ShowWatchTarget(NOFULLPAGE);
              }
             else
              {
               Local.WatchTarget = 0;
               ShowWatchTarget(NOFULLPAGE);
              }
           }
           break;
    case 2:
          if(Local.Status == 0)
           {
            DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, WATCHWINTYPE);
            Watch_Func();
           }
          break;
    case 3:
          xtm_printf("OperateWatchWindow Local.Status = %d\n", Local.Status);
          if(Local.Status == 0)
           {
            DisplayTalkWatchPicWindow(TALKWATCHRETURNTYPE, WATCHWINTYPE);
            Watch_Func();
           }
          break;
    default:
          OperateQuickmenu(wintype, 103);
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchWatchWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&watch_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   {
    TouchNum = CheckBigmenuFunc(XLCD, YLCD);  //��ݰ�ť����
    if(TouchNum >= 100)
      isInTouch = 1;
   }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
//ReturnType 0 --  ������   1 -- Talk Watch ����   WinType  0 -- Talk����  1 -- Watch����
void DisplayTalkWatchPicWindow(uint8_t ReturnType, uint8_t WinType)    //��ʾ�Խ����봰��
{
	if(Udp_InitFlag == 0)
	  return;

  xtm_printf("DisplayTalkWatchPicWindow\n");
  CheckKeyPressAndLcd(); //��鰴��������LCD
  DisplayImage(&talkwatchpic_image, FROMFILE, FB_PAGE2);
  DisplayImage(&talkwatchpic_image, FROMFILE, FULLPAGE);
  if(WinType == TALKWINTYPE)
    DisplayImage(&talkpic_image, FROMFILE, FULLPAGE);
  else
    DisplayImage(&watchpic_image, FROMFILE, FULLPAGE);
  Local.TalkWatchWinType = WinType;
  Local.CurrentWindow = 16;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealTalkWatchPicWindow(uint8_t wintype)    //�Խ����봰��Ԥ������������Ч��
{
   if(Local.TalkWatchWinType == TALKWINTYPE)
    {
     if((wintype >= 0)&&(wintype <= 2))
      {
       DisplayImageButton(&talkpic_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
      }
    }
   else
     if(wintype < 1)
      {
       DisplayImageButton(&watchpic_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
      }
}
//--------------------------------------------------------------------------
void OperateTalkWatchPicWindow(uint8_t wintype, uint8_t currwindow)    //�Խ����봰�ڲ���
{
   xtm_printf("Local.Status = %d, Local.TalkWatchWinType = %d 11111111111\n", Local.Status, Local.TalkWatchWinType);
   if(Local.TalkWatchWinType == TALKWINTYPE)
    {
     if((wintype >= 0)&&(wintype <= 2))
      {
       DisplayImageButton(&talkpic_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
      }
    }
   else
     if(wintype < 1)
      {
       DisplayImageButton(&watchpic_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
      }
  switch(wintype)
   {
    case 0:
           xtm_printf("Local.Status = %d, Local.TalkWatchWinType = %d\n", Local.Status, Local.TalkWatchWinType);
           switch(Local.TalkWatchWinType)
            {
             case WATCHWINTYPE: //�Ҷ�
                      if(Local.Status == 3)  //״̬Ϊ����
                        WatchEnd_Func();
                      break;
             case TALKWINTYPE: //����
                      if(Local.Status == 2)  //״̬Ϊ���Խ�
                        Talk_Func();
                      break;
            }
           break;
    case 1:  //����
            if((Local.Status == 2)||(Local.Status == 6))  //״̬Ϊ���Խ� �򱻽�ͨ��
            //if(Local.Status == 6)  //״̬Ϊ���Խ�
            {
             #ifdef ELEVATORCONTROL_HANGZHOU
              if(Remote.Addr[0][0] == 'S')
               {
                 memcpy(Local.Elevator_Addr, Remote.Addr[0], 20);
                 memcpy(Local.Elevator_IP, Remote.IP[0], 4);               
                 ElevatorControl_Func();  //���Ƶ���
                 break;
               }  
             #endif
             OpenLock_Func();
            }
           break;
    case 2:  //�Ҷ�
           switch(Local.TalkWatchWinType)
            {
             case TALKWINTYPE: //�Ҷ�

                        if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
                          ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))
                          TalkEnd_Func();
                      break;
            }
           break;
    default:
          OperateQuickmenu(wintype, 102);
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchTalkWatchPicWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&talkpic_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
#ifdef ELEVATORCONTROL_HANGZHOU
void ElevatorControl_Func(void)  //���Ƶ���
{
  uint8_t i;
  uint8_t sendb[128];
  uint16_t sendlength;
  char Door_Addr[21];
  uint8_t Door_IP[4];
  char RemoteIP[20];
  memcpy(Door_Addr, NullAddr, 20);
  Door_Addr[0] = 'M';
  memcpy(Door_Addr + 1, LocalCfg.Addr + 1, 6);
  Door_Addr[7] = '0';

  if(Local.Elevator_Addr[0] != 'S')
    return;
  //ͬһ��Ԫ�ڲſɿ��Ƶ���  
  if((LocalCfg.Addr[1] == Local.Elevator_Addr[1])&&(LocalCfg.Addr[2] == Local.Elevator_Addr[2])&&(LocalCfg.Addr[3] == Local.Elevator_Addr[3])&&(LocalCfg.Addr[4] == Local.Elevator_Addr[4])&&(LocalCfg.Addr[5] == Local.Elevator_Addr[5])&&(LocalCfg.Addr[6] == Local.Elevator_Addr[6]))
   {
      //ͷ��
      memcpy(sendb, UdpPackageHead, 6);
      //����  ,�����㲥����
      sendb[6] = NSORDER;
      sendb[7] = ASK;    //����

      //����Ϊ���з�
      memcpy(sendb+8, LocalCfg.Addr, 20);
      memcpy(sendb+28, LocalCfg.IP, 4);
      memcpy(sendb+32, Door_Addr, 20);
      Door_IP[0] = 0;
      Door_IP[1] = 0;
      Door_IP[2] = 0;
      Door_IP[3] = 0;
      memcpy(sendb+52, Door_IP, 4);

      sendlength = 56;
	  sprintf(RemoteIP, "%d.%d.%d.%d", Local.NsMultiAddr[0], Local.NsMultiAddr[1], Local.NsMultiAddr[2], Local.NsMultiAddr[3]);

      CreateMultiUdpBuff(UDP_VideoSocket, MULTI_SEND, RemoteIP, LocalPort[1], sendb, sendlength, ELEVATR_ORDER1);
    }
  else
    xtm_printf("��ͬ��Ԫ���ɿ��Ƶ���\n");
}
#endif
//--------------------------------------------------------------------------
