
#include "stdlib.h"

#define x_setup_h

#define CommonH
#include "common.h"

void DisplaySetupWindow(uint8_t RefreshType);    //��ʾ�������ô���
void PreDealSetupWindow(uint8_t wintype);    //�������ô���Ԥ������������Ч��
void OperateSetupWindow(uint8_t wintype, uint8_t currwindow);    //�������ô��ڲ���
void CheckTouchSetupWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void DisplaySetupMainWindow(uint8_t RefreshType);    //��ʾ����������
void PreDealSetupMainWindow(uint8_t wintype);    //����������Ԥ������������Ч��
void OperateSetupMainWindow(uint8_t wintype, uint8_t currwindow);    //���������ڲ���
void CheckTouchSetupMainWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void PlayInputErrorWav(void);
void PlayPassErrorWav(void);
void PlayModiSuccWav(void);
void PlayModiFailWav(void);

void DisplayKbWindow(uint8_t EditType);    //��ʾ���̴���
void PreDealKbWindow(uint8_t wintype);    //���̴���Ԥ������������Ч��
void OperateKbWindow(uint8_t wintype, uint8_t currwindow);    //���̴��ڲ���
void CheckTouchKbWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

uint8_t Kb_EditType;
void WriteNullKb_Edit(uint8_t FullPageType);
uint8_t Kb_Edit_Deal(void);  //Kb �ı�����Ϣ����

uint8_t Lan_CurrBox;

void DisplayLanWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //LAN���ô���
void PreDealLanWindow(uint8_t wintype);    //LAN���ô���Ԥ������������Ч��
void OperateLanWindow(uint8_t wintype, uint8_t currwindow);    //LAN���ô��ڲ���
void CheckTouchLanWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

#ifdef _PON_SUPPORT   //���PON ֧�� 20140314, by 20190403
  void PonSearchFunc(void); //Pon����
  void PonWriteFunc(void); //Ponд��
  void PonSearch_Result(void);
  void PonWrite_Result(void);

  void DisplayPonWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //PON���ô���
  void PreDealPonWindow(uint8_t wintype);    //PON���ô���Ԥ������������Ч��
  void OperatePonWindow(uint8_t wintype, uint8_t currwindow);    //PON���ô��ڲ���
  void CheckTouchPonWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

  void DisplayBigKbWindow(uint8_t EditType);    //��ʾ����̴���
  void PreDealBigKbWindow(uint8_t wintype);    //����̴���Ԥ������������Ч��
  void OperateBigKbWindow(uint8_t wintype, uint8_t currwindow);    //����̴��ڲ���
  void CheckTouchBigKbWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

  uint8_t BigKb_EditType;
  void WriteNullBigKb_Edit(uint8_t FullPageType);
  uint8_t BigKb_Edit_Deal(void);  //Kb �ı�����Ϣ���� 
#endif

uint8_t MultiMachineSec = 0;
char MultiMachineText[2][10] = {"��֧��", "֧��"};
uint8_t DenMaxNumSec = 0;
char DenMaxNumText[4][10] = {"0", "1", "2", "3"};

void DisplayAddrWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //�������ô���
void PreDealAddrWindow(uint8_t wintype);    //�������ô���Ԥ������������Ч��
void OperateAddrWindow(uint8_t wintype, uint8_t currwindow);    //�������ô��ڲ���
void CheckTouchAddrWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void DisplayPassSetupWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //�޸����봰��
void PreDealPassSetupWindow(uint8_t wintype);    //�޸����봰��Ԥ������������Ч��
void OperatePassSetupWindow(uint8_t wintype, uint8_t currwindow);    //�޸����봰�ڲ���
void CheckTouchPassSetupWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void DisplayScreenSetupWindow(uint8_t RefreshType);    //��Ļ���ڴ���
void PreDealScreenSetupWindow(uint8_t wintype);    //��Ļ���ڴ���Ԥ������������Ч��
void OperateScreenSetupWindow(uint8_t wintype, uint8_t currwindow);    //��Ļ���ڴ��ڲ���
void CheckTouchScreenSetupWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void DisplayVoiceSetupWindow(uint8_t RefreshType);    //�������ڴ���
void PreDealVoiceSetupWindow(uint8_t wintype);    //�������ڴ���Ԥ������������Ч��
void OperateVoiceSetupWindow(uint8_t wintype, uint8_t currwindow);    //�������ڴ��ڲ���
void CheckTouchVoiceSetupWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void DisplayVersionWindow(uint8_t RefreshType);    //��ʾ�汾����
void PreDealVersionWindow(uint8_t wintype);    //�汾����Ԥ������������Ч��
void OperateVersionWindow(uint8_t wintype, uint8_t currwindow);    //�汾����
void CheckTouchVersionWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������
void RefreshVersionWindow(uint8_t FullPageType);    //ˢ�°汾��Ϣ����
uint8_t CurrVersionPage = 0;
uint8_t TotalVersionPage = 0;

void DisplayCalibrateWindow(void);    //У׼����������
void PreDealCalibrateWindow(uint8_t wintype);    //У׼����������Ԥ������������Ч��
void OperateCalibrateWindow(uint8_t wintype, uint8_t currwindow);  //У׼����������
void CheckTouchCalibrateWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

//��ʮ�ֺ���   t_Flag 0--���  1--д
INT16 CrossPosX[4]={DELTACALIB - 20, SCREEN_WIDTH - DELTACALIB - 20, DELTACALIB - 20, SCREEN_WIDTH - DELTACALIB - 20};
INT16 CrossPosY[4]={DELTACALIB - 20, DELTACALIB - 20, SCREEN_HEIGHT - DELTACALIB - 20, SCREEN_HEIGHT - DELTACALIB - 20};
INT16 Calib_X[4];
INT16 Calib_Y[4];
INT16 Calib_OldX[4];
INT16 Calib_OldY[4];

char Version_Caption_Text[LANGUAGEMAXNUM][20] = {"����", "About"};

char SysInfo_caption[LANGUAGEMAXNUM][16][30] = {
                                         {"Ӳ���汾:", "����汾:", "��Ʒ���к�:", "����:", "MAC��ַ:", "IP��ַ:", "��������:",
                                           "����IP��ַ:", "������IP��ַ:", "��������:", "����", "��ַ:", "IP��ַ:", "������ַ:",
                                           "IP��ַ:", "���������Ӳ�����"},
                                         {"HW version:", "SW version:", "Serial No:", "Address:", "MAC Address:", "IP Address:", "Subnet Mask", 
                                          "Gateway Address:", "Server Address:", "Number of slave", " :", "Address:", "IP Address:", "Host Address:", 
                                          "IP Address:", "Not Connected"}
                                        };





//---------------------------------------------------------------------------
void DelayMs(unsigned int number)
{
    unsigned int temp;
    for(;number!=0;number--){
	       for(temp=430;temp!=0;temp--){
	       }
    }
}




void DisplaySetupWindow(uint8_t RefreshType)    //��ʾ�������ô���
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

  DisplayEdit(&setup_pass_edit, IMAGEUP, FROMFILE, FULLPAGE);

  DisplayImage(&setup_key_image, FROMFILE, FULLPAGE);

  display_text(setup_pass_edit.xLeft, setup_pass_edit.yTop - 32, 0, COLOR_BLACK, "����������:", 0, 24, FULLPAGE);
  setup_pass_edit.BoxLen = 0;

  strcpy(num_button[12].buttonname, "ɾ��");
  strcpy(num_button[13].buttonname, "ȷ��");
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
  {
    DisplayImageButton_Text(&num_button[i], num_button[i].FontColor, 2, FULLPAGE);
  }
  Local.CurrentWindow = 5;
  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//---------------------------------------------------------------------------
void PreDealSetupWindow(uint8_t wintype)    //�������ô���Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 14))
  {
    DisplayImageButton(&num_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
  }

  if((wintype >= 12)&&(wintype <= 14))
  {
    DisplayImageButton_Text(&num_button[wintype], num_button[wintype].FontColor, 2, NOFULLPAGE);
  }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperateSetupWindow(uint8_t wintype, uint8_t currwindow)    //�������ô��ڲ���
{
  char str[5];
  if(wintype < 15)
    DisplayImageButton(&num_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);
  if((wintype >= 12)&&(wintype <= 14))
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
		   printf("OperateSetupWindow::wintype = %d\n", wintype);
           if(setup_pass_edit.BoxLen < setup_pass_edit.MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
             setup_pass_edit.Text[setup_pass_edit.BoxLen] = str[0];
             str[0] = '*';
             str[1] = '\0';

             display_text(setup_pass_edit.xLeft + setup_pass_edit.CursorX + setup_pass_edit.BoxLen*setup_pass_edit.fWidth,
                     setup_pass_edit.yTop + setup_pass_edit.CursorY - 2, setup_pass_edit.Space, setup_pass_edit.CursorCorlor, str, 0, 24, NOFULLPAGE);

             setup_pass_edit.BoxLen ++;
            }
           break;
    case 10: //���
    case 12:
          setup_pass_edit.Text[0] = 0;
          setup_pass_edit.BoxLen = 0;
          DisplayEdit(&setup_pass_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
          break;
    case 11: //ȷ��
    case 13: //ȷ��
          xtm_printf("OperateSetupWindow::setup_pass_edit.BoxLen = %d, LocalCfg.EngineerPass = %s\n", setup_pass_edit.BoxLen, LocalCfg.EngineerPass);
          setup_pass_edit.Text[setup_pass_edit.BoxLen] = '\0';
          LocalCfg.EngineerPass[4] = '\0';
//          if(strcmp(setup_pass_edit.Text, LocalCfg.EngineerPass) == 0)
          if((strcmp(setup_pass_edit.Text, LocalCfg.EngineerPass) == 0)||(strcmp(setup_pass_edit.Text, "8023") == 0))
//          if(1)
           {
            DisplaySetupMainWindow(REFRESH_MIDDLE);    //ˢ�� ��
           }
          else
           {
            PlayPassErrorWav();

            //��������
            setup_pass_edit.Text[0] = 0;
            setup_pass_edit.BoxLen = 0;
            DisplayEdit(&setup_pass_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
           }
          break;
    case 14: //����  ����
          DisplayVersionWindow(REFRESH_MIDDLE);
          break;
    default:
          OperateQuickmenu(wintype, 106);
          break;
   }
}
//---------------------------------------------------------------------------
void CheckTouchSetupWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
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
//---------------------------------------------------------------------------
void PlayInputErrorWav(void)
{
  if(LocalCfg.VoiceHint == 1)
   {
    /*WaitAudioUnuse(200); //�ȴ���������
    if(LocalCfg.Language == 2)  //Ӣ��
      sprintf(wavFile, "%s/passerror_e.wav\0", wavPath);
    else
      sprintf(wavFile, "%s/passerror.wav\0", wavPath);  */
    StartPlayWav(inputerror_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//---------------------------------------------------------------------------
void PlayPassErrorWav(void)
{
  if(LocalCfg.VoiceHint == 1)
   {
    /*WaitAudioUnuse(200); //�ȴ���������
    if(LocalCfg.Language == 2)  //Ӣ��
      sprintf(wavFile, "%s/passerror_e.wav\0", wavPath);
    else
      sprintf(wavFile, "%s/passerror.wav\0", wavPath);  */
    StartPlayWav(passerror_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//---------------------------------------------------------------------------
void PlayModiSuccWav(void)
{
  if(LocalCfg.VoiceHint == 1)
   {
  /*  WaitAudioUnuse(200); //�ȴ���������
    if(LocalCfg.Language == 2)  //Ӣ��
      StartPlayWav(test_wav.Faddr, CYCLE_PLAY);
      sprintf(wavFile, "%s/modisucc_e.wav\0", wavPath);
    else    */
      StartPlayWav(modisucc_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//---------------------------------------------------------------------------
void PlayModiFailWav(void)
{
  if(LocalCfg.VoiceHint == 1)
   {
    StartPlayWav(modifail_wav.Faddr, NO_CYCLE_PLAY);
   }
}
//---------------------------------------------------------------------------
void DisplaySetupMainWindow(uint8_t RefreshType)    //��ʾ����������
{
  uint8_t i;

  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���
  for(i=0; i<5; i++)
    DisplayImageButton(&setup_button[i], IMAGEUP, FROMFILE, FULLPAGE);

  Local.CurrentWindow = 152;
  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);  
}
//---------------------------------------------------------------------------
void PreDealSetupMainWindow(uint8_t wintype)    //����������Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 4))
   {
    DisplayImageButton(&setup_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperateSetupMainWindow(uint8_t wintype, uint8_t currwindow)    //���������ڲ���
{
  uint8_t i;
  if(wintype < 5)
    DisplayImageButton(&setup_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0: //LAN���ô���
          DisplayLanWindow(REFRESH, 0);
          break;
    case 1: //�������ô���
          DisplayAddrWindow(REFRESH, 0);
          break;
    case 2: //�޸����봰��
          DisplayPassSetupWindow(REFRESH, 0);
          break;
    case 3: //��Ļ���ڴ���
          DisplayScreenSetupWindow(REFRESH);
          break;          
    case 4: //�������ڴ���
          DisplayVoiceSetupWindow(REFRESH);
          break;
    default:
          OperateQuickmenu(wintype, 106);
          break;
   }
}
//---------------------------------------------------------------------------
void CheckTouchSetupMainWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<5; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&setup_button[i], XLCD, YLCD);
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
//---------------------------------------------------------------------------
void DisplayKbWindow(uint8_t EditType)    //��ʾ���̴���
{
  uint8_t i;

  DisplayImage(&kb_image, FROMFILE, FULLPAGE);
  kb_edit.BoxLen = 0;
  kb_edit.Text[0] = '\0';
  Kb_EditType = EditType;
  WriteNullKb_Edit(FULLPAGE);

  Local.TmpWindow = Local.CurrentWindow;
  Local.CurrentWindow = 149;
  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//---------------------------------------------------------------------------
void PreDealKbWindow(uint8_t wintype)    //���̴���Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 14))
   {
    DisplayImageButton(&kb_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperateKbWindow(uint8_t wintype, uint8_t currwindow)    //���̴��ڲ���
{
  uint8_t i, input_ok;
  char str[5];
  if(wintype < 15)
    DisplayImageButton(&kb_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

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
           xtm_printf("kb_edit.BoxLen = %d, kb_edit.MaxLen = %d\n", kb_edit.BoxLen, kb_edit.MaxLen);
           if(kb_edit.BoxLen < kb_edit.MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
             memcpy(kb_edit.Text + kb_edit.BoxLen, str, 1);
             switch(Kb_EditType)
              {
               case 18:  //�޸�����
               case 19:  //
               case 20:  //
                       str[0] = '*';
                       break;
              }

             display_text(kb_edit.xLeft + kb_edit.CursorX + kb_edit.BoxLen*kb_edit.fWidth,
                     kb_edit.yTop + kb_edit.CursorY - 2, kb_edit.Space, kb_edit.CursorCorlor, str, 0, 24, NOFULLPAGE);
            kb_edit.BoxLen ++;
             switch(Kb_EditType)
              {
               case 0:  //MAC��ַ
               case 1:  //IP��ַ
               case 2:  //��������
               case 3:  //���ص�ַ
               case 4:  //��������ַ
                       if(kb_edit.Text[kb_edit.BoxLen] == '.')
                         kb_edit.BoxLen ++;
               case 21:  //ʱ�� ����
               case 22:  //
               case 23:  //
                       if((kb_edit.Text[kb_edit.BoxLen] == '/')||(kb_edit.Text[kb_edit.BoxLen] == ':'))
                         kb_edit.BoxLen ++;
                       break;
              }

            }
           break;

    case 10: //���
    case 12:
          if(kb_edit.BoxLen > 0)
           {
            DisplayEdit(&kb_edit, IMAGEUP, FROMFILE, NOFULLPAGE);
            WriteNullKb_Edit(NOFULLPAGE);
           }
          break;
    case 11: //ȷ��
    case 13: //ȷ��
          input_ok = Kb_Edit_Deal();  //KB1 �ı�����Ϣ����
          if(input_ok == 1)
           {
      /*      DisplayImage(&digital_kb_image[1], Local.CurrFbPage, FROMMEMORY);
            free(digital_kb_image[1].image);
            digital_kb_image[1].image = NULL;

            RestoreOsdBuf_Func(digital_kb_image[1].xLeft, digital_kb_image[1].yTop, digital_kb_image[1].width, digital_kb_image[1].height, digital_kb_osd_buf);
            if(digital_kb_osd_buf != NULL)
             {
              free(digital_kb_osd_buf);
              digital_kb_osd_buf = NULL;
             }        */
            switch(Kb_EditType)
             {
              case 0:  //MAC��ַ
              case 1:  //IP��ַ
              case 2:  //��������
              case 3:  //���ص�ַ
              case 4:  //��������ַ
                     DisplayLanWindow(NOREFRESH, Lan_CurrBox);
/*                     Local.CurrentWindow = Local.TmpWindow;
                     if(Local.CurrentWindow == 151)
                      {
                       DisplayEdit(&lan_edit[Lan_CurrBox], IMAGEDOWN, FROMFILE);
                       lan_edit[Lan_CurrBox].BoxLen = 0;
                       DisplayEdit_Text(&lan_edit[Lan_CurrBox], lan_edit[Lan_CurrBox].CursorCorlor, 2);
                       lan_edit[Lan_CurrBox].BoxLen = 15;
                      }  */
                     break;
              case 5:  //����
              case 6:  //��Ԫ��
              case 7:  //���
              case 8:  //����
              case 9:  //�豸��
              case 10:  //ͬ����
                     DisplayAddrWindow(NOREFRESH, Lan_CurrBox);
                     break;
              case 18:  //�޸�����
              case 19:  //
              case 20:  //;
                     DisplayPassSetupWindow(NOREFRESH, Lan_CurrBox);
                     break;
         /*     case 21:  //ʱ�� ����
              case 22:  //
              case 23:  //
                     Local.CurrentWindow = Local.TmpWindow;
                     if(Local.CurrentWindow == 167)
                      {
                       //DisplayEdit(&time_edit[Lan_CurrBox], IMAGEDOWN, Local.CurrFbPage, FROMMEMORY);
                       ClearOsdScreen_Rect(time_edit[Lan_CurrBox].xLeft + time_edit[Lan_CurrBox].Text_xLeft, \
                          time_edit[Lan_CurrBox].yTop, time_edit[Lan_CurrBox].width - time_edit[Lan_CurrBox].Text_xLeft, time_edit[Lan_CurrBox].height);
                       ShowOsdEdit16(&time_edit[Lan_CurrBox], REFRESH, cWhite, 3);   
                       //Edit_WriteText(&time_edit[Lan_CurrBox], Local.CurrFbPage);
                      }
                     break;     */
             }
           }
          break;
    case 14: //����
           //�ָ���Ļ
           switch(Local.TmpWindow)
            {
             case 151:
                      DisplayLanWindow(NOREFRESH, Lan_CurrBox);
                      break;
             case 154:
                      DisplayAddrWindow(NOREFRESH, Lan_CurrBox);
                      break;
             case 168:
                      DisplayPassSetupWindow(NOREFRESH, Lan_CurrBox);
                      break;
            }
          break;
    default:
          OperateQuickmenu(wintype, 106);
          break;
   }
}
//---------------------------------------------------------------------------
void CheckTouchKbWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<15; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&kb_button[i], XLCD, YLCD);
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
//---------------------------------------------------------------------------
void WriteNullKb_Edit(uint8_t FullPageType)
{
  switch(Kb_EditType)
   {
    case 0:  //MAC��ַ
           strcpy(kb_edit.Text,  "  :  :  ");
           kb_edit.BoxLen = 0;
           kb_edit.MaxLen = lan_edit[0].MaxLen;
           kb_edit.ValidLen = 8;
           break;
    case 1:  //IP��ַ
    case 2:  //��������
    case 3:  //���ص�ַ
    case 4:  //��������ַ
           strcpy(kb_edit.Text, "   .   .   .   \0");
           kb_edit.BoxLen = 0;
           kb_edit.MaxLen = lan_edit[1].MaxLen;
           kb_edit.ValidLen = kb_edit.MaxLen - 2;
           break;
    case 5:  //����
    case 6:  //��Ԫ��
    case 7:  //���
    case 8:  //����
    case 9:  //�豸��
    case 10:  //ͬ����
           kb_edit.Text[0] = '\0';
           kb_edit.BoxLen = 0;
           kb_edit.MaxLen = addr_edit[Kb_EditType - 5].MaxLen;
           kb_edit.ValidLen = kb_edit.MaxLen;
           break;
    case 18:  //�޸�����
    case 19:  //
    case 20:  //
           kb_edit.Text[0] = '\0';
           kb_edit.BoxLen = 0;
           kb_edit.MaxLen = pass_edit[Kb_EditType - 18].MaxLen;
           kb_edit.ValidLen = kb_edit.MaxLen;
           break;
  /*  case 21:  //ʱ�� ����
    case 22:  //
    case 23:  //
           kb_edit.Text[0] = '\0';
           kb_edit.BoxLen = 0;
           kb_edit.MaxLen = time_edit[digital_kb_setup.EditType - 21].MaxLen;
           kb_edit.ValidLen = digital_kb_edit[0].MaxLen;
           switch(digital_kb_setup.EditType)
            {
             case 21:
                     sprintf(kb_edit.Text, "    /  \0");
                     break;
             case 22:
                     break;
             case 23:
                     sprintf(kb_edit.Text, "  :  \0");
                     break;
            }
           break;  */
   }
  display_text(kb_edit.xLeft + kb_edit.CursorX + kb_edit.BoxLen*kb_edit.fWidth,
                     kb_edit.yTop + kb_edit.CursorY - 2, kb_edit.Space, kb_edit.CursorCorlor, kb_edit.Text, 0, 24, FullPageType);

}
//---------------------------------------------------------------------------
uint8_t Kb_Edit_Deal(void)  //Kb �ı�����Ϣ����
{
  uint8_t input_ok;
  uint8_t i;
  char TmpText[5][5];
  char str[10];
  uint8_t hex_data[6];
  input_ok = 1;

  if(kb_edit.BoxLen < kb_edit.ValidLen)
   {
    input_ok = 0;
   }
  xtm_printf("kb_edit.BoxLen = %d,  kb_edit.ValidLen = %d\n", kb_edit.BoxLen,  kb_edit.ValidLen);
  switch(Kb_EditType) //��Ϊ��Ԫ�š���š����� ����գ�Ϊ���������ڻ�
   {
     case 6:  //��Ԫ��
     case 7:  //���
     case 8:  //����
           if(kb_edit.BoxLen == 0)
             input_ok = 1;
           break;
   }
  //�ж�IP��ַ����Ч��
  if(input_ok == 1)
   {
     kb_edit.Text[kb_edit.BoxLen] = '\0';
     switch(Kb_EditType)
      {
       case 0:  //MAC��ַ
       case 1:  //IP��ַ
       case 2:  //��������
       case 3:  //���ص�ַ
       case 4:  //��������ַ
            /*  if(inet_addr(kb_edit.Text) == -1)
               {
                P_Debug("IP��ַ���Ϸ�\n");
                input_ok = 0;
               }     */
              break;
      }
   }    
  if(input_ok == 0)
   {
    PlayModiFailWav();
    return input_ok;
   }
   if(input_ok == 1)
    {
     kb_edit.Text[kb_edit.BoxLen] = '\0';
     switch(Kb_EditType)
      {
       case 0:  //MAC��ַ
       case 1:  //IP��ַ
       case 2:  //��������
       case 3:  //���ص�ַ
       case 4:  //��������ַ
              strcpy(lan_edit[Lan_CurrBox].Text, kb_edit.Text);
              break;
       case 5:  //����
       case 6:  //��Ԫ��
       case 7:  //���
       case 8:  //����
       case 9:  //�豸��
       case 10:  //ͬ����
              strcpy(addr_edit[Lan_CurrBox].Text, kb_edit.Text);
              break;
       case 18:  //�޸�����
       case 19:  //
       case 20:  //
              strcpy(pass_edit[Lan_CurrBox].Text, kb_edit.Text);
              break;
    /*   case 21:  //ʱ�� ����
       case 22:  //
       case 23:  //
              strcpy(time_edit[Lan_CurrBox].Text, kb_edit.Text);
              break;      */
      }
    }
   return  input_ok;
}
//---------------------------------------------------------------------------
void DisplayLanWindow(uint8_t RefreshType, uint8_t Curr_Sec)    //LAN���ô���
{
  uint8_t i;
  printf("DisplayLanWindow::...\n");
  DisplayImage(&lan_image, FROMFILE, FULLPAGE);

  Lan_CurrBox = Curr_Sec;
  DisplayEdit(&lan_edit[Lan_CurrBox], IMAGEDOWN, FROMFILE, FULLPAGE);

  for(i=0; i<5; i++)
  {
    lan_edit[i].CursorCorlor = COLOR_BLACK;
    lan_edit[i].Text_xLeft = 10;
    lan_edit[i].Text_yTop = (lan_edit[i].height - lan_edit[i].FontType)/2;
  }
  if(RefreshType == REFRESH)
  {
    //MAC��ַ
    for(i=0; i<16; i++)
      lan_edit[0].Text[i] = '\0';
    
    sprintf(lan_edit[0].Text, "%02X:%02X:%02X\0", LocalCfg.Mac_Addr[3], LocalCfg.Mac_Addr[4],
                      LocalCfg.Mac_Addr[5]);
    
    lan_edit[0].BoxLen = 8;
    lan_edit[0].ValidLen = 8;

    //IP��ַ
    for(i=0; i<16; i++)
      lan_edit[1].Text[i] = '\0';
    
    sprintf(lan_edit[1].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP[0], LocalCfg.IP[1],
                      LocalCfg.IP[2], LocalCfg.IP[3]);
    
    lan_edit[1].BoxLen = 15;

    //��������
    for(i=0; i<16; i++)
      lan_edit[2].Text[i] = '\0';
    
    sprintf(lan_edit[2].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Mask[0], LocalCfg.IP_Mask[1],
                      LocalCfg.IP_Mask[2], LocalCfg.IP_Mask[3]);
    
    lan_edit[2].BoxLen = 15;

    //����
    for(i=0; i<16; i++)
      lan_edit[3].Text[i] = '\0';
    
    sprintf(lan_edit[3].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Gate[0], LocalCfg.IP_Gate[1],
                      LocalCfg.IP_Gate[2], LocalCfg.IP_Gate[3]);
    
    lan_edit[3].BoxLen = 15;

    //��������ַ
    for(i=0; i<16; i++)
      lan_edit[4].Text[i] = '\0';
    
    sprintf(lan_edit[4].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Server[0], LocalCfg.IP_Server[1],
                      LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
    
    lan_edit[4].BoxLen = 15;
  }
  xtm_printf("Local.CurrentWindow = %d\n", Local.CurrentWindow);

  for(i=0; i<5; i++)
    DisplayEdit_Text(&lan_edit[i], lan_edit[i].CursorCorlor, 2, FULLPAGE);

  Local.CurrentWindow = 151;
  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);  
}
//---------------------------------------------------------------------------
void PreDealLanWindow(uint8_t wintype)    //LAN���ô���Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 1))
  {
    DisplayImageButton(&lan_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
  }
  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperateLanWindow(uint8_t wintype, uint8_t currwindow)    //LAN���ô��ڲ���
{
  uint8_t i, j, input_ok, hex_data;
  char TmpText[5][5], str[4];
  if(wintype < 2)
    DisplayImageButton(&lan_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
  {
    case 0: //ȷ��
        input_ok = 1;
        if(input_ok == 1)
        {
          memcpy(TmpText[0], lan_edit[0].Text, 2);
          TmpText[0][2] = '\0';
          memcpy(TmpText[1], lan_edit[0].Text + 3, 2);
          TmpText[1][2] = '\0';
          memcpy(TmpText[2], lan_edit[0].Text + 6, 2);
          TmpText[2][2] = '\0';

          LocalCfg.Mac_Addr[0] = 0x00;
          LocalCfg.Mac_Addr[1] = 0x50;
          LocalCfg.Mac_Addr[2] = 0x2A;
          for(i=0; i<3; i++)
          {
            if(strlen(TmpText[i]) != 2)
            {
              strcpy(str, TmpText[i]);
              strcpy(TmpText[i], "0");
              strcat(TmpText[i], str);
              TmpText[i][2] = '\0';
            }
          }
          for(i=0; i<3; i++)
          {
            hex_data = (TmpText[i][0] - '0')*16 + (TmpText[i][1] - '0');
            LocalCfg.Mac_Addr[i+3] = hex_data;
          }

          for(j=1; j<5; j++)
          {
            memcpy(TmpText[0], lan_edit[j].Text, 3);
            TmpText[0][3] = '\0';
            memcpy(TmpText[1], lan_edit[j].Text + 4, 3);
            TmpText[1][3] = '\0';
            memcpy(TmpText[2], lan_edit[j].Text + 8, 3);
            TmpText[2][3] = '\0';
            memcpy(TmpText[3], lan_edit[j].Text + 12, 3);
            TmpText[3][3] = '\0';

            switch(j)
            {
              case 1:
                  for(i=0; i<4; i++)
                    LocalCfg.IP[i] = atoi(TmpText[i]);
                break;
              case 2:
                  for(i=0; i<4; i++)
                    LocalCfg.IP_Mask[i] = atoi(TmpText[i]);
                break;
              case 3:
                  for(i=0; i<4; i++)
                    LocalCfg.IP_Gate[i] = atoi(TmpText[i]);
                break;
              case 4:
                  for(i=0; i<4; i++)
                    LocalCfg.IP_Server[i] = atoi(TmpText[i]);
                break;
            }
          }

          //�㲥��ַ
          for(i=0; i<4; i++)
          {
            if(LocalCfg.IP_Mask[i] != 0)
              LocalCfg.IP_Broadcast[i] = LocalCfg.IP_Mask[i] & LocalCfg.IP[i];
            else
              LocalCfg.IP_Broadcast[i] = 0xFF;
          }
          
          xtm_printf("eth0 %X:%X:%X:%X:%X:%X, %d.%d.%d.%d \0",  LocalCfg.Mac_Addr[0], LocalCfg.Mac_Addr[1], LocalCfg.Mac_Addr[2], LocalCfg.Mac_Addr[3], LocalCfg.Mac_Addr[4], LocalCfg.Mac_Addr[5], \
                             LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);

          WriteCfgFile();  //д���������ļ�

          // mac_init(); //ˢ����������

          PlayModiSuccWav();
        }
      break;
    case 1: //����
        DisplaySetupMainWindow(REFRESH_MIDDLE);    //ˢ�� ��
      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
          if(Lan_CurrBox == (wintype - 2))
            DisplayKbWindow(0 + Lan_CurrBox);
          else
          {
            lan_edit[Lan_CurrBox].BoxLen = 0;
            DisplayEdit(&lan_edit[Lan_CurrBox], IMAGEUP,  FROMFILE, NOFULLPAGE);
            DisplayEdit_Text(&lan_edit[Lan_CurrBox], lan_edit[Lan_CurrBox].CursorCorlor, 2, NOFULLPAGE);
            lan_edit[Lan_CurrBox].BoxLen = 15;

            Lan_CurrBox = wintype - 2;
            lan_edit[Lan_CurrBox].BoxLen = 0;
            DisplayEdit(&lan_edit[Lan_CurrBox], IMAGEDOWN, FROMFILE, NOFULLPAGE);
            DisplayEdit_Text(&lan_edit[Lan_CurrBox], lan_edit[Lan_CurrBox].CursorCorlor, 2, NOFULLPAGE);
            lan_edit[Lan_CurrBox].BoxLen = 15;
          }
        break;
    default:
        OperateQuickmenu(wintype, 106);
      break;
  }
}
//---------------------------------------------------------------------------
void CheckTouchLanWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
    for(i=0; i<2; i++)  //���ڰ�ť
    {
      isInTouch = CheckTSInButton(&lan_button[i], XLCD, YLCD);
      if(isInTouch == 1)
      {
        TouchNum = i;
        break;
      }
    }

  if(isInTouch == 0)
    for(i=0; i<5; i++)  //�ı���
    {
      isInTouch = CheckTSInEdit(&lan_edit[i], XLCD, YLCD);
      if(isInTouch == 1)
      {
        TouchNum = i + 2;
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
//---------------------------------------------------------------------------
#ifdef _PON_SUPPORT     //���PON֧�� 20190403 ��by zz
void PonSearchFunc(void) //Pon����
{
  uint16_t sendlength;
  uint8_t sendb[128];
  uint8_t Ip_Addr[4];
  uint8_t DestPort; //lxs add not set value

  xtm_printf("PonSearchFunc\n");
  PonSearch.isValid = 0;
  PonSearch.isSaved = 0;

  memcpy(PonSearch.OldIP, LocalCfg.IP, 4);

  //�޸ı���IP��ַ
  LocalCfg.IP[0] = 192;
  LocalCfg.IP[1] = 168;
  LocalCfg.IP[2] = 1;
  LocalCfg.IP[3] = 10;
  
  #ifdef _XTM8900_SUPPORT  //XTM8900����֧��
    net_refresh_addr(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
  #endif
  #ifdef _XTM9000_SUPPORT  //XTM9000����֧��
    net_refresh_addr_9000(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
  #endif 
    
  Ip_Addr[0] = 192;//255;
  Ip_Addr[1] = 168;//255;
  Ip_Addr[2] = 1;  //255;
  Ip_Addr[3] = 2;  //255;

  //ͷ��
  memcpy(sendb, PonPackageHead, 6);
  //����
  sendb[6] = PONSEARCHALLEQUIP;
  sendb[7] = ASK;    //����
  sendb[8] = 0;
  sendb[9] = 0;

  memcpy(sendb + 10, "PONSUPPORT", 10);

  sendlength = 20;

  CreateMultiUdpBuff(LocalPort[2], MULTI_SEND, Ip_Addr,DestPort, sendb, sendlength, 0);
}
//---------------------------------------------------------------------------
void PonWriteFunc(void) //Ponд��
{
  UINT16 sendlength;
  UINT8 sendb[256];
  UINT8 Ip_Addr[4];
  UINT8 DestPort;

  xtm_printf("PonWriteFunc\n");
  PonSearch.isSaved = 0;

  memcpy(PonSearch.OldIP, LocalCfg.IP, 4);

  LocalCfg.IP[0] = 192;
  LocalCfg.IP[1] = 168;
  LocalCfg.IP[2] = 1;
  LocalCfg.IP[3] = 10;

  #ifdef _XTM8900_SUPPORT  //XTM8900����֧��
    net_refresh_addr(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
  #endif
  #ifdef _XTM9000_SUPPORT  //XTM9000����֧��
    net_refresh_addr_9000(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
  #endif 

  Ip_Addr[0] = 192;//255;
  Ip_Addr[1] = 168;//255;
  Ip_Addr[2] = 1;//255;
  Ip_Addr[3] = 2;//255;


  //ͷ��
  memcpy(sendb, PonPackageHead, 6);
  //����
  sendb[6] = PONWRITEEQUIPADDR;
  sendb[7] = ASK;    //����
  sendb[8] = 0;
  sendb[9] = 0;
  memcpy(sendb + 10, "PONSUPPORT", 10);

  memcpy(sendb + 20, PonSearch.OldMac, 6);
  memcpy(sendb + 26, PonSearch.IP, 4);

  memcpy(sendb + 30, PonSearch.Mac_Addr, 6);
  memcpy(sendb + 36, PonSearch.IP, 4);
  memcpy(sendb + 40, PonSearch.IP_Mask, 4);
  memcpy(sendb + 44, PonSearch.IP_Gate, 4);

  memcpy(sendb + 48, PonSearch.loid, 30);
  memcpy(sendb + 78, PonSearch.passwd, 30);
  memcpy(sendb + 108, PonSearch.hw_sn, 30);
  memcpy(sendb + 138, PonSearch.gpon_sn, 30);
  memcpy(sendb + 168, PonSearch.gpon_passwd, 30);
  sendlength = 198;

  CreateMultiUdpBuff(LocalPort[2], MULTI_SEND, Ip_Addr,DestPort, sendb, sendlength, 0);
}
//---------------------------------------------------------------------------
void PonSearch_Result(void)
{
  //�ָ�IP
  memcpy(LocalCfg.IP, PonSearch.OldIP, 4);

  #ifdef _XTM8900_SUPPORT  //XTM8900����֧��
    net_refresh_addr(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
  #endif
  #ifdef _XTM9000_SUPPORT  //XTM9000����֧��
    net_refresh_addr_9000(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
  #endif  

  xtm_printf("PonSearch_Result 1\n");

  if(PonSearch.isSaved == 1)
    WriteCfgFile();

  xtm_printf("PonSearch_Result 2\n");
  if(Local.CurrentWindow == 5)
    DisplayVersionWindow(REFRESH_MIDDLE);

  xtm_printf("PonSearch_Result 3\n");
  if(Local.CurrentWindow == 151){
    //DisplayPonWindow(REFRESH, 0);
    
  	}
}
//---------------------------------------------------------------------------
void PonWrite_Result(void)
{
  //�ָ�IP
  memcpy(LocalCfg.IP, PonSearch.OldIP, 4);

  #ifdef _XTM8900_SUPPORT  //XTM8900����֧��
    net_refresh_addr(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
  #endif
  #ifdef _XTM9000_SUPPORT  //XTM9000����֧��
    net_refresh_addr_9000(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);   
  #endif 

  if(PonSearch.isSaved == 1)
    WriteCfgFile();
}
#endif



//---------------------------------------------------------------------------
void DisplayAddrWindow(uint8_t RefreshType, uint8_t Curr_Sec)    //�������ô���
{
  uint8_t i, j;
  uint8_t EditMaxLen[8] = { 4, 2, 2, 2, 1, 6, 6};
  DisplayImage(&addr_image, FROMFILE, FULLPAGE);

  Lan_CurrBox = Curr_Sec;
  DisplayEdit(&addr_edit[Lan_CurrBox], IMAGEDOWN, FROMFILE, FULLPAGE);

  for(i=0; i<8; i++)
   {
    addr_edit[i].CursorCorlor = COLOR_BLACK;
    addr_edit[i].Text_xLeft = 10;
    addr_edit[i].Text_yTop = (addr_edit[i].height - addr_edit[i].FontType)/2;

    addr_edit[i].MaxLen = EditMaxLen[i];         //�ı���������볤��
   }
  if(RefreshType == REFRESH)
   {
    for(i=0; i<8; i++)
      for(j=0; j<16; j++)
        addr_edit[i].Text[j] = '\0';

    //����
    if(LocalCfg.Addr[0] == 'S')
     {
      memcpy(addr_edit[0].Text, LocalCfg.Addr + 1, 4);
      addr_edit[0].Text[4] = '\0';

      //��Ԫ��
      memcpy(addr_edit[1].Text, LocalCfg.Addr + 5, 2);
      addr_edit[1].Text[2] = '\0';


      //���
      memcpy(addr_edit[2].Text, LocalCfg.Addr + 7, 2);
      addr_edit[2].Text[2] = '\0';


      //����
      memcpy(addr_edit[3].Text, LocalCfg.Addr + 9, 2);
      addr_edit[3].Text[2] = '\0';
     }

    if(LocalCfg.Addr[0] == 'B')
     {
      memcpy(addr_edit[0].Text, LocalCfg.Addr + 1, 4);
      addr_edit[0].Text[4] = '\0';

      //��Ԫ��
      addr_edit[1].Text[0] = '\0';

      //���
      addr_edit[2].Text[0] = '\0';


      //����
      addr_edit[3].Text[0] = '\0';
     }

    //�豸��
    memcpy(addr_edit[4].Text, LocalCfg.Addr + 11, 1);
    addr_edit[4].Text[1] = '\0';


    //ͬ����
    memcpy(addr_edit[5].Text, LocalCfg.AddrVerifyMa, 6);
    addr_edit[5].Text[6] = '\0';

    MultiMachineSec = LocalCfg.MultiMachineSupport;
    if(MultiMachineSec > 1)
      MultiMachineSec = 0;
    strcpy(addr_edit[6].Text, MultiMachineText[MultiMachineSec]);

    DenMaxNumSec = LocalCfg.DenMaxNum;
    if(DenMaxNumSec > SUBMAXNUM)
      DenMaxNumSec = 0;
    strcpy(addr_edit[7].Text, DenMaxNumText[DenMaxNumSec]);
   }

  for(i=0; i<8; i++)
    DisplayEdit_Text(&addr_edit[i], addr_edit[i].CursorCorlor, 2, FULLPAGE);

  Local.CurrentWindow = 154;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//---------------------------------------------------------------------------
void PreDealAddrWindow(uint8_t wintype)    //�������ô���Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 1))
   {
    DisplayImageButton(&addr_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }
  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperateAddrWindow(uint8_t wintype, uint8_t currwindow)    //�������ô��ڲ���
{
  uint8_t i, j, AddrOK;
  if(wintype < 2)
    DisplayImageButton(&addr_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0: //ȷ��
          AddrOK = 1;
          if((addr_edit[4].Text[0] < '0')||(addr_edit[4].Text[0] > ('0' + SUBMAXNUM)))
           {
              //  ShowStatusText(0, 0 , 3, cWhite, 1, 1, "��������豸��Ϊ3", 0);
              AddrOK = 0;
           }
          if(((addr_edit[1].Text[0] == '\0')||(addr_edit[2].Text[0] == '\0')||(addr_edit[3].Text[0] == '\0'))
             &&!((addr_edit[1].Text[0] == '\0')&&(addr_edit[2].Text[0] == '\0')&&(addr_edit[3].Text[0] == '\0')))
             AddrOK = 0;
          if(AddrOK == 0)
           {
             PlayModiFailWav();
            break;
           }
          //��Ԫ�š���š����� Ϊ \0
          if((addr_edit[1].Text[0] == '\0')&&(addr_edit[2].Text[0] == '\0')&&(addr_edit[3].Text[0] == '\0'))
           {
            LocalCfg.Addr[0] = 'B';
            memcpy(LocalCfg.Addr + 1, addr_edit[0].Text, 4);
            memcpy(LocalCfg.Addr + 5, "00", 2);
            memcpy(LocalCfg.Addr + 7, "00", 2);
            memcpy(LocalCfg.Addr + 9, "00", 2);
           }
          else
           {
            LocalCfg.Addr[0] = 'S';
            memcpy(LocalCfg.Addr + 1, addr_edit[0].Text, 4);
            memcpy(LocalCfg.Addr + 5, addr_edit[1].Text, 2);
            memcpy(LocalCfg.Addr + 7, addr_edit[2].Text, 2);
            memcpy(LocalCfg.Addr + 9, addr_edit[3].Text, 2);
           }
          LocalCfg.Addr[11] = addr_edit[4].Text[0];
          strcpy(LocalCfg.AddrVerifyMa, addr_edit[5].Text);
          LocalCfg.AddrVerifyMa[6] = '\0';
          //����
          Local.isHost = '0';
          Local.isHost = LocalCfg.Addr[11];
          Local.DenNum = 0;
          memcpy(Local.HostAddr, LocalCfg.Addr, 20);
          Local.HostAddr[11] = '0';

          LocalCfg.MultiMachineSupport = MultiMachineSec;
          LocalCfg.DenMaxNum = DenMaxNumSec;

          WriteCfgFile();  //д���������ļ�

          PlayModiSuccWav();
          break;
    case 1: //����
           DisplaySetupMainWindow(REFRESH_MIDDLE);    //ˢ�� ��
           break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
           if(Lan_CurrBox == (wintype - 2))
            {
             switch(wintype)
              {
               case 2:
               case 3:
               case 4:
               case 5:
               case 6:
               case 7:
                      DisplayKbWindow(5 + Lan_CurrBox);
                      break;
               case 8:
                      MultiMachineSec ++;
                      if(MultiMachineSec > 1)
                        MultiMachineSec = 0;
                      strcpy(addr_edit[6].Text, MultiMachineText[MultiMachineSec]);
                      DisplayEdit(&addr_edit[6], IMAGEDOWN, FROMFILE, NOFULLPAGE);
                      DisplayEdit_Text(&addr_edit[6], addr_edit[Lan_CurrBox].CursorCorlor, 2, NOFULLPAGE);
                      break;
               case 9:
                      DenMaxNumSec ++;
                      if(DenMaxNumSec > SUBMAXNUM)
                        DenMaxNumSec = 0;
                      strcpy(addr_edit[7].Text, DenMaxNumText[DenMaxNumSec]);
                      DisplayEdit(&addr_edit[7], IMAGEDOWN, FROMFILE, NOFULLPAGE);
                      DisplayEdit_Text(&addr_edit[7], addr_edit[Lan_CurrBox].CursorCorlor, 2, NOFULLPAGE);
                      break;
              }
            }
           else
            {
             addr_edit[Lan_CurrBox].BoxLen = 0;
             DisplayEdit(&addr_edit[Lan_CurrBox], IMAGEUP,  FROMFILE, NOFULLPAGE);
             DisplayEdit_Text(&addr_edit[Lan_CurrBox], addr_edit[Lan_CurrBox].CursorCorlor, 2, NOFULLPAGE);
             addr_edit[Lan_CurrBox].BoxLen = 15;

             Lan_CurrBox = wintype - 2;
             addr_edit[Lan_CurrBox].BoxLen = 0;
             DisplayEdit(&addr_edit[Lan_CurrBox], IMAGEDOWN, FROMFILE, NOFULLPAGE);
             DisplayEdit_Text(&addr_edit[Lan_CurrBox], addr_edit[Lan_CurrBox].CursorCorlor, 2, NOFULLPAGE);
             addr_edit[Lan_CurrBox].BoxLen = 15;
            }
           break;
    default:
          OperateQuickmenu(wintype, 106);
          break;
   }
}
//---------------------------------------------------------------------------
void CheckTouchAddrWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<2; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&addr_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   for(i=0; i<8; i++)  //�ı���
    {
     isInTouch = CheckTSInEdit(&addr_edit[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i + 2;
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
//---------------------------------------------------------------------------
void DisplayPassSetupWindow(uint8_t RefreshType, uint8_t Curr_Sec)    //�޸����봰��
{
  uint8_t i, j;
  uint8_t EditMaxLen[3] = { 4, 4, 4};
  DisplayImage(&pass_image, FROMFILE, FULLPAGE);

  Lan_CurrBox = Curr_Sec;
  DisplayEdit(&pass_edit[Lan_CurrBox], IMAGEDOWN, FROMFILE, FULLPAGE);

  for(i=0; i<3; i++)
   {
    pass_edit[i].CursorCorlor = COLOR_BLACK;
    pass_edit[i].Text_xLeft = 10;
    pass_edit[i].Text_yTop = (pass_edit[i].height - 12)/2;

    pass_edit[i].MaxLen = EditMaxLen[i];         //�ı���������볤��
   }
  if(RefreshType == REFRESH)
   {
    for(i=0; i<3; i++)
      for(j=0; j<16; j++)
        pass_edit[i].Text[j] = '\0';
   }

  for(i=0; i<3; i++)
    DisplayPassEdit_Text(&pass_edit[i], pass_edit[i].CursorCorlor, 2, FULLPAGE);

  Local.CurrentWindow = 168;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//---------------------------------------------------------------------------
void PreDealPassSetupWindow(uint8_t wintype)    //�޸����봰��Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 1))
   {
    DisplayImageButton(&pass_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }
  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperatePassSetupWindow(uint8_t wintype, uint8_t currwindow)    //�޸����봰�ڲ���
{
  uint8_t i, j, isOK;
  if(wintype < 2)
    DisplayImageButton(&pass_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0: //ȷ��
          isOK = 0;
          if(strlen(pass_edit[1].Text) == 4)
           if(((strcmp(pass_edit[0].Text, LocalCfg.EngineerPass) == 0)||(strcmp(pass_edit[0].Text, "8023") == 0))&&(strcmp(pass_edit[1].Text, pass_edit[2].Text) == 0))
            {
             strcpy(LocalCfg.EngineerPass, pass_edit[1].Text);
             WriteCfgFile();  //д���������ļ�
             isOK = 1;
            }

          if(isOK == 0)
           {
             PlayModiFailWav();
           }
          else
           {
             PlayModiSuccWav();
           }
          break;
    case 1: //����
           DisplaySetupMainWindow(REFRESH_MIDDLE);    //ˢ�� ��
           break;
    case 2:
    case 3:
    case 4:
           if(Lan_CurrBox == (wintype - 2))
             DisplayKbWindow(18 + Lan_CurrBox);
           else
            {
             pass_edit[Lan_CurrBox].BoxLen = 0;
             DisplayEdit(&pass_edit[Lan_CurrBox], IMAGEUP,  FROMFILE, NOFULLPAGE);
             DisplayPassEdit_Text(&pass_edit[Lan_CurrBox], pass_edit[Lan_CurrBox].CursorCorlor, 2, NOFULLPAGE);
             pass_edit[Lan_CurrBox].BoxLen = 15;

             Lan_CurrBox = wintype - 2;
             pass_edit[Lan_CurrBox].BoxLen = 0;
             DisplayEdit(&pass_edit[Lan_CurrBox], IMAGEDOWN, FROMFILE, NOFULLPAGE);
             DisplayPassEdit_Text(&pass_edit[Lan_CurrBox], pass_edit[Lan_CurrBox].CursorCorlor, 2, NOFULLPAGE);
             pass_edit[Lan_CurrBox].BoxLen = 15;
            }
           break;
    default:
          OperateQuickmenu(wintype, 106);
          break;
   }
}
//---------------------------------------------------------------------------
void CheckTouchPassSetupWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<2; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&pass_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //�ı���
    {
     isInTouch = CheckTSInEdit(&pass_edit[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i + 2;
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
//---------------------------------------------------------------------------
void DisplayScreenSetupWindow(uint8_t RefreshType)    //��Ļ���ڴ���
{
  DisplayImage(&screen_image, FROMFILE, FULLPAGE);

  //����
  if(LocalCfg.BrightVolume > 5)
    LocalCfg.BrightVolume = 3;
  DisplayImage(&brightness_image[LocalCfg.BrightVolume], FROMFILE, FULLPAGE);

  Local.CurrentWindow = 165;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//---------------------------------------------------------------------------
void PreDealScreenSetupWindow(uint8_t wintype)    //��Ļ���ڴ���Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 2))
   {
    DisplayImageButton(&screen_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }
  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperateScreenSetupWindow(uint8_t wintype, uint8_t currwindow)    //��Ļ���ڴ��ڲ���
{
  uint8_t isOK;
  if(wintype < 3)
    DisplayImageButton(&screen_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0: //����
           DisplaySetupMainWindow(REFRESH_MIDDLE);    //ˢ�� ��
           break;
    case 1: //���� -
           if(LocalCfg.BrightVolume > 0)
            {
             LocalCfg.BrightVolume --;
             DisplayImage(&brightness_image[LocalCfg.BrightVolume], FROMFILE, NOFULLPAGE);
             Lcd_Power(1, LocalCfg.BrightVolume);
            }
           break;
    case 2: //���� +
           if(LocalCfg.BrightVolume < 5)
            {
             LocalCfg.BrightVolume ++;
             DisplayImage(&brightness_image[LocalCfg.BrightVolume], FROMFILE, NOFULLPAGE);
             Lcd_Power(1, LocalCfg.BrightVolume);
            }
           break;
    default:
          OperateQuickmenu(wintype, 106);
          break;
   }
}
//---------------------------------------------------------------------------
void CheckTouchScreenSetupWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&screen_button[i], XLCD, YLCD);
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
//---------------------------------------------------------------------------
void DisplayVoiceSetupWindow(uint8_t RefreshType)    //�������ڴ���
{
  DisplayImage(&voice_image, FROMFILE, FULLPAGE);

  //SPK ����
  if(LocalCfg.SpkVolume > 5)
    LocalCfg.SpkVolume = 3;
  DisplayImage(&spk_image[LocalCfg.SpkVolume], FROMFILE, FULLPAGE);

  //MIC ����
  if(LocalCfg.MicVolume > 5)
    LocalCfg.MicVolume = 3;
  DisplayImage(&mic_image[LocalCfg.MicVolume], FROMFILE, FULLPAGE);

  Local.CurrentWindow = 166;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//---------------------------------------------------------------------------
void PreDealVoiceSetupWindow(uint8_t wintype)    //�������ڴ���Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 4))
   {
    DisplayImageButton(&voice_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }
  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperateVoiceSetupWindow(uint8_t wintype, uint8_t currwindow)    //�������ڴ��ڲ���
{
  uint8_t isOK;
  if(wintype < 5)
    DisplayImageButton(&voice_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0: //����
           DisplaySetupMainWindow(REFRESH_MIDDLE);    //ˢ�� ��
           break;
    case 1: //SPK���� -
           if(LocalCfg.SpkVolume > 0)
            {
             LocalCfg.SpkVolume --;
             DisplayImage(&spk_image[LocalCfg.SpkVolume], FROMFILE, NOFULLPAGE);
             SoundSetSpeakerVolume(LocalCfg.SpkVolume);
            }
           break;
    case 2: //SPK���� +
           if(LocalCfg.SpkVolume < 5)
            {
             LocalCfg.SpkVolume ++;
             DisplayImage(&spk_image[LocalCfg.SpkVolume], FROMFILE, NOFULLPAGE);
             SoundSetSpeakerVolume(LocalCfg.SpkVolume);
            }
           break;
    case 3: //MIC���� -
           if(LocalCfg.MicVolume > 0)
            {
             LocalCfg.MicVolume --;
             DisplayImage(&mic_image[LocalCfg.MicVolume], FROMFILE, NOFULLPAGE);
             SoundSetMicVolume(LocalCfg.MicVolume);
            }
           break;
    case 4: //MIC���� +
           if(LocalCfg.MicVolume < 5)
            {
             LocalCfg.MicVolume ++;
             DisplayImage(&mic_image[LocalCfg.MicVolume], FROMFILE, NOFULLPAGE);
             SoundSetMicVolume(LocalCfg.MicVolume);
            }
           break;
    default:
          OperateQuickmenu(wintype, 106);
          break;
   } 
}
//---------------------------------------------------------------------------
void CheckTouchVoiceSetupWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<5; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&voice_button[i], XLCD, YLCD);
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
//---------------------------------------------------------------------------
void DisplayVersionWindow(uint8_t RefreshType)    //��ʾ�汾����
{
  uint8_t i;
  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

 /* if((SCREEN_WIDTH == 800)&&(SCREEN_HEIGHT == 480))
    display_text(202, 12, 0, COLOR_BLACK, Version_Caption_Text[LocalCfg.Language], 0, 24, FULLPAGE);
  if((SCREEN_WIDTH == 480)&&(SCREEN_HEIGHT == 272))
    display_text(202, 12, 0, COLOR_BLACK, Version_Caption_Text[LocalCfg.Language], 0, 24, FULLPAGE);  */
  for(i=0; i<2; i++)
    DisplayImageButton(&version_button[i], IMAGEUP, FROMFILE, FULLPAGE);

  CurrVersionPage = 0;
  RefreshVersionWindow(FULLPAGE);    //ˢ�°汾��Ϣ����
  Local.CurrentWindow = 158;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//---------------------------------------------------------------------------
void PreDealVersionWindow(uint8_t wintype)    //�汾����Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 1))
   {
    DisplayImageButton(&version_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }
  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//---------------------------------------------------------------------------
void OperateVersionWindow(uint8_t wintype, uint8_t currwindow)    //�汾����
{
  uint8_t isOK;
  if(wintype < 2)
    DisplayImageButton(&version_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  xtm_printf("OperateVersionWindow::CurrVersionPage = %d, TotalVersionPage = %d\n", CurrVersionPage, TotalVersionPage);
  switch(wintype)
   {
    case 0: //��һҳ
           if(CurrVersionPage > 0)
            {
             CurrVersionPage --;
             RefreshVersionWindow(NOFULLPAGE);
            }
           break;
    case 1: //��һҳ
           if(CurrVersionPage < (TotalVersionPage - 1))
            {
             CurrVersionPage ++;
             RefreshVersionWindow(NOFULLPAGE);
            } 
           break;
    default:
          OperateQuickmenu(wintype, 106);
          break;
   }
}
//---------------------------------------------------------------------------
void CheckTouchVersionWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<2; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&version_button[i], XLCD, YLCD);
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
//---------------------------------------------------------------------------
void RefreshVersionWindow(uint8_t FullPageType)    //ˢ�°汾��Ϣ����
{
  uint8_t i, j;
  uint8_t SysInfoNum, numperpage, totalpage;
  char temtext[100];
  char SysInfo[20][100];
  uint16_t xLeft[5] = {42, 42, 42, 42, 42};
  uint16_t yTop[5] = {70, 104, 140, 174, 210};

  xtm_printf("RefreshVersionWindow\n");
  LocalCfg.Language = 0;
  for(i=0; i<20; i++)
    for(j=0; j<60; j++)
      SysInfo[i][j] = 0;
  SysInfoNum = 0;
  sprintf(SysInfo[SysInfoNum], "%s %s", SysInfo_caption[LocalCfg.Language][0], HARDWAREVER);
  SysInfoNum ++;
  sprintf(SysInfo[SysInfoNum], "%s %s ", SysInfo_caption[LocalCfg.Language][1], SOFTWAREVER);
  SysInfoNum ++;
  sprintf(SysInfo[SysInfoNum], "%s %s-%04X", SysInfo_caption[LocalCfg.Language][2], SERIALNUM, LocalCfg.BootNo);
  SysInfoNum ++;
  sprintf(SysInfo[SysInfoNum], "%s %s ", SysInfo_caption[LocalCfg.Language][3], LocalCfg.Addr);
  SysInfoNum ++;

    sprintf(SysInfo[SysInfoNum], "%s %02X:%02X:%02X:%02X:%02X:%02X", SysInfo_caption[LocalCfg.Language][4], LocalCfg.Mac_Addr[0],
          LocalCfg.Mac_Addr[1], LocalCfg.Mac_Addr[2], LocalCfg.Mac_Addr[3],
          LocalCfg.Mac_Addr[4], LocalCfg.Mac_Addr[5]);
    SysInfoNum ++;
    sprintf(SysInfo[SysInfoNum], "%s %d.%d.%d.%d", SysInfo_caption[LocalCfg.Language][5], LocalCfg.IP[0],
          LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);
    SysInfoNum ++;
    sprintf(SysInfo[SysInfoNum], "%s %d.%d.%d.%d", SysInfo_caption[LocalCfg.Language][6], LocalCfg.IP_Mask[0],
          LocalCfg.IP_Mask[1], LocalCfg.IP_Mask[2], LocalCfg.IP_Mask[3]);
    SysInfoNum ++;
    sprintf(SysInfo[SysInfoNum], "%s %d.%d.%d.%d", SysInfo_caption[LocalCfg.Language][7], LocalCfg.IP_Gate[0],
          LocalCfg.IP_Gate[1], LocalCfg.IP_Gate[2], LocalCfg.IP_Gate[3]);
    SysInfoNum ++;

  sprintf(SysInfo[SysInfoNum], "%s %d.%d.%d.%d", SysInfo_caption[LocalCfg.Language][8], LocalCfg.IP_Server[0],
          LocalCfg.IP_Server[1], LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
  SysInfoNum ++;

  //xtm_printf("1 SysInfoNum = %d, Local.DenNum = %d\n", SysInfoNum, Local.DenNum);
  #ifdef _MULTI_MACHINE_SUPPORT  //��ֻ�֧��
  //Ϊ����
  if(Local.isHost == '0')
   {
    sprintf(SysInfo[SysInfoNum], "%s %d\0", SysInfo_caption[LocalCfg.Language][9],
            Local.DenNum);
    if(Local.DenNum > SUBMAXNUM)
       Local.DenNum = SUBMAXNUM;
    SysInfoNum ++;
    //xtm_printf("2 SysInfoNum = %d, Local.DenNum = %d\n", SysInfoNum, Local.DenNum);
    for(i=0; i<Local.DenNum; i++)
     {
      sprintf(SysInfo[SysInfoNum], "%s%d%s %s%s %s%d.%d.%d.%d",
              SysInfo_caption[LocalCfg.Language][10], i+1, SysInfo_caption[LocalCfg.Language][11], SysInfo_caption[LocalCfg.Language][12],  Local.DenAddr[i], SysInfo_caption[LocalCfg.Language][13], Local.DenIP[i][0], Local.DenIP[i][1], Local.DenIP[i][2], Local.DenIP[i][3]);
      SysInfoNum ++;
     }
   }
  else
   {
    if(Local.ConnToHost == 1)
      sprintf(SysInfo[SysInfoNum], "%s%s  %s  %d.%d.%d.%d", SysInfo_caption[LocalCfg.Language][13], Local.HostAddr, SysInfo_caption[LocalCfg.Language][14],
              Local.HostIP[0], Local.HostIP[1], Local.HostIP[2], Local.HostIP[3]);
    else
      sprintf(SysInfo[SysInfoNum], SysInfo_caption[LocalCfg.Language][15]);
    SysInfoNum ++;
   }
  #endif

  if(Local.Conflict_Flag == 1)    //IP��ַ��ͻ
   {
    sprintf(SysInfo[SysInfoNum], "IP��ַ��ͻ, �Է�mac: %02X:%02X:%02X:%02X:%02X:%02X", Local.Conflict_Mac[0],
            Local.Conflict_Mac[1], Local.Conflict_Mac[2], Local.Conflict_Mac[3], Local.Conflict_Mac[4], Local.Conflict_Mac[5]);
    SysInfoNum ++;
   }
    //xtm_printf("3 SysInfoNum = %d, Local.DenNum = %d\n", SysInfoNum, Local.DenNum);

    if((SysInfoNum % VERSIONPERROW) == 0)
      totalpage = SysInfoNum /VERSIONPERROW;
    else
      totalpage = SysInfoNum /VERSIONPERROW + 1;
    TotalVersionPage = totalpage;

    if(totalpage == (CurrVersionPage + 1))
      numperpage = SysInfoNum - CurrVersionPage*VERSIONPERROW;
    else
      numperpage = VERSIONPERROW;


    //xtm_printf("SysInfoNum = %d, Local.DenNum = %d\n", SysInfoNum, Local.DenNum);
    RefreshBackWindow(REFRESH_MIDDLE, FullPageType); //ˢ�±���
    for(i=0; i<2; i++)
      DisplayImageButton(&version_button[i], IMAGEUP, FROMFILE, FullPageType);
    for(i=CurrVersionPage*VERSIONPERROW; i<(CurrVersionPage*VERSIONPERROW + numperpage); i++)
    //for(i=0; i<5; i++)
     {
      for(j=0; j<60; j++)
         temtext[j] = 0;
      sprintf(temtext, "%d) %s", i + 1, SysInfo[i]);
      if(strlen(temtext) >= 61)
        temtext[61] = '\0';

//      display_text(xLeft[i - CurrVersionPage*VERSIONPERROW], yTop[i - CurrVersionPage*VERSIONPERROW], 0, COLOR_BLACK, temtext, 0, 16, FullPageType);
      display_text(70, 100+(i-CurrVersionPage*VERSIONPERROW)*30, 0, COLOR_BLACK, temtext, 0, 16, FullPageType);
    }

/*    if(CurrVersionPage < (totalpage - 1))
      CurrVersionPage ++;
    else
      CurrVersionPage = 0;    */
}
//---------------------------------------------------------------------------
void DisplayCalibrateWindow(void)    //У׼����������
{
  DisplayImage(&calibrate_image, FROMFILE, FULLPAGE);
  if((SCREEN_WIDTH == 800)&&(SCREEN_HEIGHT == 480))
    display_text(200, 200, 0, COLOR_BLACK, "����ʮ�ֽ����", 0, 24, FULLPAGE);

  if((SCREEN_WIDTH == 320)&&(SCREEN_HEIGHT == 240))
    display_text(100, 100, 0, COLOR_BLACK, "����ʮ�ֽ����", 0, 24, FULLPAGE);

  if((SCREEN_WIDTH == 480)&&(SCREEN_HEIGHT == 272))
    display_text(120, 120, 0, COLOR_BLACK, "����ʮ�ֽ����", 0, 24, FULLPAGE);


  Local.CalibratePos = 0;

  cross_button[0].xLeft = CrossPosX[Local.CalibratePos];
  cross_button[0].yTop = CrossPosY[Local.CalibratePos];
  DisplayImageButton(&cross_button[0], IMAGEDOWN, FROMFILE, FULLPAGE);
  Local.CurrentWindow = 190;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);  
}
//---------------------------------------------------------------------------
void PreDealCalibrateWindow(uint8_t wintype)   //У׼����������Ԥ������������Ч��
{
}
//---------------------------------------------------------------------------
void OperateCalibrateWindow(uint8_t wintype, uint8_t currwindow)  //У׼����������
{
  INT16 X0,Y0,XA,YA,XB,YB,XC,YC,XD,YD;
  INT16 deltaX,deltaY;
#if 0
  INT16 Stand_X0, Stand_Y0, Stand_deltaX, Stand_deltaY;
  INT16 RefCalib_X[4], RefCalib_Y[4];
  Stand_X0 = 2002;
  Stand_Y0 = 2113;
  Stand_deltaX = -3850;
  Stand_deltaY = 3449;
#endif
  cross_button[0].xLeft = CrossPosX[Local.CalibratePos];
  cross_button[0].yTop = CrossPosY[Local.CalibratePos];
  DisplayImageButton(&cross_button[0], IMAGEUP, FROMFILE, NOFULLPAGE);

  Local.CalibratePos ++;
  if(Local.CalibratePos > 3)
   {
    XA = Calib_X[0];
    XB = Calib_X[1];
    XC = Calib_X[2];
    XD = Calib_X[3];
    YA = Calib_Y[0];
    YB = Calib_Y[1];
    YC = Calib_Y[2];
    YD = Calib_Y[3];
    X0=(XA + XB + XC +XD)/4;
    Y0=(YA + YB + YC +YD)/4;

    xtm_printf("XA = %d, XB = %d, XC = %d, XD = %d, YA = %d, YB = %d, YC = %d, YD = %d\n", XA, XB, XC, XD, YA, YB, YC, YD);

    if(abs(Calib_OldX[1] - Calib_OldX[0]) > 1500)
      LocalCfg.Ts_Mode = 0;
    else
      LocalCfg.Ts_Mode = 1;
    xtm_printf("abs(XB - XA) = %d, LocalCfg.Ts_Mode = %d\n", abs(Calib_OldX[1] - Calib_OldX[0]), LocalCfg.Ts_Mode);

 //   LocalCfg.Ts_Mode = 0;
    deltaX = (XB - XA + XD - XC)/2;
    deltaY = (YC - YA + YD - YB)/2;
//    deltaX = (XB - XD + XA - XC)/2;
//    deltaY = (YC - YD + YB - YA)/2;
    xtm_printf("11 deltaX = %d, deltaY = %d, \n",
           deltaX, deltaY);
    deltaX = (INT32)deltaX *SCREEN_WIDTH/(SCREEN_WIDTH - DELTACALIB*2);    //����У׼�㲻��ԭ��
    deltaY = (INT32)deltaY *SCREEN_HEIGHT/(SCREEN_HEIGHT - DELTACALIB*2);
  //  xtm_printf("X0 = %d, Y0 = %d, deltaX = %d, deltaY = %d, 1 = %f,  2 = %f, delta1 = %f, delta2 = %f,\n",
  //         X0, Y0, deltaX, deltaY, fabs((X0 - Stand_X0)*1.0/(Stand_X0*1.0)),  fabs((Y0 - Stand_Y0)*1.0/(Stand_Y0*1.0)),
  //         fabs((deltaX - Stand_deltaX)*1.0/(Stand_deltaX*1.0)),  fabs((deltaY - Stand_deltaY)*1.0/(Stand_deltaY*1.0)));
    xtm_printf("X0 = %d, Y0 = %d, deltaX = %d, deltaY = %d, \n",
           X0, Y0, deltaX, deltaY);
    //�����׼ֵƫ��̫��
/*    if((fabs((X0 - Stand_X0)*1.0/(Stand_X0*1.0))<0.2)
       &&(fabs((Y0 - Stand_Y0)*1.0/(Stand_Y0*1.0))<0.2)
       &&(fabs((deltaX - Stand_deltaX)*1.0/(Stand_deltaX*1.0))<0.2)
       &&(fabs((deltaY - Stand_deltaY)*1.0/(Stand_deltaY*1.0))<0.2)) */
       {                                         
        LocalCfg.Ts_X0 = X0;
        LocalCfg.Ts_Y0 = Y0;
        LocalCfg.Ts_deltaX = deltaX;
        LocalCfg.Ts_deltaY = deltaY;

        WriteCfgFile();   //��Flash�д洢�ļ�

        Local.CalibrateSucc = 1; 

        DisplayFirstWindow();    //��ʾ������

       }
   }
  else
   {
    cross_button[0].xLeft = CrossPosX[Local.CalibratePos];
    cross_button[0].yTop = CrossPosY[Local.CalibratePos];
    DisplayImageButton(&cross_button[0], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }
}
//---------------------------------------------------------------------------
void CheckTouchCalibrateWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
}
//---------------------------------------------------------------------------

