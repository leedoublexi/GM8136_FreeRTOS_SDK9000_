
#include "stdlib.h"

#define x_record_h

#define CommonH
#include "common.h"

void DisplayRecordsWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //��ʾͨ����¼����
void PreDealRecordsWindow(uint8_t wintype);    //ͨ����¼����Ԥ������������Ч��
void OperateRecordsWindow(uint8_t wintype, uint8_t currwindow);    //ͨ����¼���ڲ���
void CheckTouchRecordsWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void ShowRecordsList(uint8_t refreshflag, uint8_t FullPageType);    //ͨ����¼�б�
void CheckRecordsRemoteAddr(char *RemoteAddr, char *tmp_con);

uint8_t Records_CurrBox;
void DisplayRecordsContentWindow(uint8_t recordsno);    //ͨ����¼�鿴��Ϣ����
void PreDealRecordsContentWindow(uint8_t wintype);    //�鿴ͨ����¼����Ԥ������������Ч��
void OperateRecordsContentWindow(uint8_t wintype, uint8_t currwindow);    //�鿴ͨ����¼���ڲ���
void CheckTouchRecordsContentWindow(uint16_t XLCD, uint16_t YLCD); //��鴥��������

void ShowRecordsContent(uint8_t recordsno, uint8_t FullPageType);    // ͨ����¼����

uint8_t Init_RecordsVar(void); //��ʼ�� Records �ṹ
uint8_t Uninit_RecordsVar(void); //�ͷ� Records �ṹ

xSemaphoreHandle *save_pic_sem;
void SavePicFunc(void);


//��ͨ����¼�ļ�
void ReadRecordsFile(void);
//дͨ����¼�ļ�
void WriteRecordsFile(void);

void ResetRecordsCfg(void);    //��λͨ����¼�����ļ�
void WriteRecordsCfgFile(void);     //дͨ����¼�����ļ�

void Recv_NewRecords(void);
//д��Ƭ����
void WritePicFunc(void);
//---------------------------------------------------------------------------
void DisplayRecordsWindow(uint8_t RefreshType, uint8_t Curr_Sec)    //��ʾͨ����¼����
{
  RefreshBackWindow(RefreshType, FULLPAGE); //ˢ�±���

  DisplayImage(&records_image, FROMFILE, FULLPAGE);

  Records.CurrNo = Curr_Sec;            //��ǰͨ����¼���  0---n-1
  Records.CurrentInfoPage = Records.CurrNo /RECORDNUMPERPAGE + 1;     //��ǰͨ����¼ҳ

  ShowRecordsList(REFRESH, FULLPAGE);

  Local.CurrentWindow = 21;
  Local.NewRecPic = 0;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealRecordsWindow(uint8_t wintype)    //ͨ����¼����Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 2))
   {
    DisplayImageButton(&records_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//--------------------------------------------------------------------------
void OperateRecordsWindow(uint8_t wintype, uint8_t currwindow)    //ͨ����¼���ڲ���
{
  uint8_t i;
  uint8_t CurrNo;
  uint16_t tmp;
  if(wintype < 3)
    DisplayImageButton(&records_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0://�Ϸ�       a
             if(Records.CurrentInfoPage > 1)
              {
               Records.CurrentInfoPage --;
               xtm_printf("OperateRecordsWindow1:: Records.CurrNo = %d, Records.CurrentInfoPage = %d\n",  Records.CurrNo, Records.CurrentInfoPage);
               tmp = (uint16_t)RECORDNUMPERPAGE * (Records.CurrentInfoPage - 1);
               Records.CurrNo = tmp;
               xtm_printf("OperateRecordsWindow2:: Records.CurrNo = %d, Records.CurrentInfoPage = %d\n",  Records.CurrNo, Records.CurrentInfoPage);
               ShowRecordsList(REFRESH, NOFULLPAGE);
              }
           break;
    case 1://�·�     b
             if(Records.CurrentInfoPage < Records.TotalInfoPage)
              {
               Records.CurrentInfoPage ++;
               xtm_printf("OperateRecordsWindow3:: Records.CurrNo = %d, Records.CurrentInfoPage = %d\n",  Records.CurrNo, Records.CurrentInfoPage);
               tmp = (uint16_t)RECORDNUMPERPAGE * (Records.CurrentInfoPage - 1);
               Records.CurrNo = tmp;
               xtm_printf("OperateRecordsWindo4:: Records.CurrNo = %d, Records.CurrentInfoPage = %d\n",  Records.CurrNo, Records.CurrentInfoPage);
               ShowRecordsList(REFRESH, NOFULLPAGE);
              }
           break;
    case 2://ɾ��           c
               if(Records.CurrNo < Records.TotalNum)
                {
                 //˳��ǰ��
                 for(i = Records.CurrNo; i < (Records.TotalNum - 1); i ++)
                  {
                   memcpy(&RecordsContent[i], &RecordsContent[i+1], sizeof(struct InfoContent1));
                   RecordsBuff[i] = RecordsBuff[i+1];
                  }
                 Records.TotalNum --;
                 xtm_printf("Records.CurrNo = %d, Records.TotalNum = %d\n", Records.CurrNo, Records.TotalNum);
                 if(Records.CurrNo > (Records.TotalNum - 1))
                   Records.CurrNo = Records.TotalNum - 1;
                 ShowRecordsList(REFRESH, NOFULLPAGE);
   
                 WriteRecordsFile();      //дͨ����¼�����ļ�  //дͨ����¼�ļ�
                }
             break;
    case 40://ͨ����¼1��           c
    case 41://ͨ����¼2��           c
    case 42://ͨ����¼3��           c
    case 43://ͨ����¼4��           c
    case 44://ͨ����¼5��           c
           //��ǰ��Ϣ���
             CurrNo = (Records.CurrentInfoPage-1)*RECORDNUMPERPAGE+(wintype - 40);
             xtm_printf("CurrNo = %d, Records.TotalNum = %d \n", CurrNo,
                    Records.TotalNum);
             if(Records.CurrNo == CurrNo)
              {
               if(CurrNo < (Records.TotalNum))
                if(RecordsContent[CurrNo].HavePic == 1)
                 {
                  Records.CurrNo = CurrNo;
                  DisplayRecordsContentWindow(Records.CurrNo);
                 }
              }
             else
              {
               if(CurrNo < (Records.TotalNum))
                {
                 Records.CurrNo = CurrNo;
                 ShowRecordsList(REFRESH, NOFULLPAGE);
                }
              }
           break;
    default:
          OperateQuickmenu(wintype, 105);
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchRecordsWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&records_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   for(i=0; i<RECORDNUMPERPAGE; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&recordsrow_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i + 40;
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
void ShowRecordsList(uint8_t refreshflag, uint8_t FullPageType)    //ͨ����¼��ʾ���� ͨ����¼�б�
{
  uint8_t i, j;
  uint8_t PageTotalNum;  //��ҳ��Ϣ����
  uint8_t CurrNo;
  char tmp_con[80];
  uint16_t tmp_len;  //Ԥ������
  uint8_t NoInPage;
  uint16_t ascnum, tmp;
  uint8_t DeltaHei;
  uint16_t FontColor;

  tmp_len = 10;

           if(Records.TotalNum > 0)
            {
             //��ҳ��
             if((Records.TotalNum % INFONUMPERPAGE) == 0)
               tmp = Records.TotalNum /INFONUMPERPAGE;
             else
               tmp = Records.TotalNum /INFONUMPERPAGE + 1;
             Records.TotalInfoPage = tmp;

             //��ǰҳ
             tmp = Records.CurrNo / INFONUMPERPAGE + 1;
             Records.CurrentInfoPage = tmp;
             if(Records.CurrentInfoPage < Records.TotalInfoPage)
               PageTotalNum = INFONUMPERPAGE;
             else
               PageTotalNum = Records.TotalNum - (Records.CurrentInfoPage - 1)*RECORDNUMPERPAGE;
             //��ǰ��Ϣ�ڱ�ҳ�е�λ��
             NoInPage = (Records.CurrNo)%RECORDNUMPERPAGE;

             xtm_printf("ShowRecordsList::Records.CurrNo = %d, Records.TotalNum = %d, Records.CurrentInfoPage = %d\n", Records.CurrNo, Records.TotalNum, Records.CurrentInfoPage);
             for(i = 0; i < RECORDNUMPERPAGE; i++)
              {
               if(i == NoInPage)
                 DisplayImageButton(&recordsrow_button[i], IMAGEDOWN, FROMFILE, FullPageType);
               else
                 DisplayImageButton(&recordsrow_button[i], IMAGEUP, FROMFILE, FullPageType);
              }

             for(i = 0; i < PageTotalNum; i++)
              {
               if(i == NoInPage)
                 FontColor = COLOR_WHITE;
               else
                 FontColor = COLOR_BLACK;
                 
               //xtm_printf("FontColor = 0x%X\n", FontColor);
               CurrNo = (Records.CurrentInfoPage - 1)*RECORDNUMPERPAGE + i;

               DeltaHei = (recordsrow_button[i].height - 24)/2;
               //���
               sprintf(tmp_con, "%02d", (Records.CurrentInfoPage-1)*RECORDNUMPERPAGE+i+1);
               tmp_con[2] = '\0';
               //xtm_printf("tmp_con = %s\n", tmp_con);
               //display_text(recordsrow_button[i].xLeft+10, recordsrow_button[i].yTop + DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);
               display_text(recordsrow_button[i].xLeft+2, recordsrow_button[i].yTop + DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);
               switch(RecordsContent[CurrNo].Type)
                {
                 case 0:
                        strcpy(tmp_con,  "����");
                        break;
                 case 1:
                        strcpy(tmp_con,  "δ����");
                        break;
                 case 2:
                        strcpy(tmp_con,  "�ѽ���");
                        break;
                 default:
                        strcpy(tmp_con,  "δ����");
                        break;
                }

               //display_text(recordsrow_button[i].xLeft+100, recordsrow_button[i].yTop+DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);
               //CheckRecordsRemoteAddr(RecordsContent[CurrNo].RemoteAddr);
               //display_text(recordsrow_button[i].xLeft+280, recordsrow_button[i].yTop+DeltaHei, 0, FontColor, RecordsContent[CurrNo].RemoteAddr, 0, 24, FullPageType);


               display_text(recordsrow_button[i].xLeft+50, recordsrow_button[i].yTop+DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);
               CheckRecordsRemoteAddr(RecordsContent[CurrNo].RemoteAddr, tmp_con);
               display_text(recordsrow_button[i].xLeft+138, recordsrow_button[i].yTop+DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);

               //������ƬIcon
               if(RecordsContent[CurrNo].HavePic == 1)
                {
                  DisplayImage(&records_pic_icon[i], FROMFILE, FullPageType);
                }
              }
            }
           else
             for(i = 0; i < RECORDNUMPERPAGE; i++)
               DisplayImageButton(&recordsrow_button[i], IMAGEUP, FROMFILE, FullPageType);
}
//--------------------------------------------------------------------------
void CheckRecordsRemoteAddr(char *RemoteAddr, char *tmp_con)
{
  char Build[5];
  char Unit[3];
  char Storey[3];
  char Room[3];
  char Equiq[2];
  uint8_t i;
  for(i=0; i<40; i++)
    tmp_con[i] = '\0';
  switch(RemoteAddr[0])
   {
    case 'M':
            memcpy(Build, RemoteAddr + 1, 4);
            Build[4] = '\0';
            memcpy(Unit, RemoteAddr + 5, 2);
            Unit[2] = '\0';
            memcpy(Equiq, RemoteAddr + 7, 1);
            Equiq[1] = '\0';
            sprintf(tmp_con, "�ſ� %s��%s��Ԫ %s", Build, Unit, Equiq);
            RemoteAddr[8] = '\0';
            break;
    case 'W':
            memcpy(Build, RemoteAddr + 1, 4);
            Build[4] = '\0';
            sprintf(tmp_con, "Χǽ %s", Build);
            RemoteAddr[5] = '\0';
            break;
    case 'Z':
            memcpy(Build, RemoteAddr + 1, 4);
            Build[4] = '\0';
            sprintf(tmp_con, "���� %s", Build);
            RemoteAddr[5] = '\0';
            break;
    case 'B':
            memcpy(Build, RemoteAddr + 1, 4);
            Build[4] = '\0';

            memcpy(Equiq, RemoteAddr + 7, 1);
            Equiq[1] = '\0';
            sprintf(tmp_con, "���� %s�� %s", Build, Equiq);

            RemoteAddr[5] = RemoteAddr[11];
            RemoteAddr[6] = '\0';
            break;
    case 'S':
            memcpy(Build, RemoteAddr + 1, 4);
            Build[4] = '\0';
            memcpy(Unit, RemoteAddr + 5, 2);
            Unit[2] = '\0';
            memcpy(Storey, RemoteAddr + 7, 2);
            Storey[2] = '\0';
            memcpy(Room, RemoteAddr + 9, 2);
            Room[2] = '\0';
            memcpy(Equiq, RemoteAddr + 11, 1);
            Equiq[1] = '\0';
            sprintf(tmp_con, "%s��%s��Ԫ%s��%s�� �豸%s", Build, Unit, Storey, Room, Equiq);
            RemoteAddr[12] = '\0';
            break;
    case 'H':
            memcpy(Equiq, RemoteAddr + 11, 1);
            Equiq[1] = '\0';
            sprintf(tmp_con, "�����ſ� %s", Equiq);            
            if((RemoteAddr[5] == '0')&&(RemoteAddr[6] == '0')&&(RemoteAddr[7] == '0')&&(RemoteAddr[8] == '0')&&(RemoteAddr[9] == '0')&&(RemoteAddr[10] == '0'))
             {
              RemoteAddr[5] = RemoteAddr[11];
              RemoteAddr[6] = '\0';
             }
            else
              RemoteAddr[12] = '\0';

            break;
   }
}
//--------------------------------------------------------------------------
void DisplayRecordsContentWindow(uint8_t recordsno)    //��ʾ�鿴ͨ����¼����
{
  DisplayImage(&picwin_image, FROMFILE, FULLPAGE);

  Records_CurrBox = recordsno;
  ShowRecordsContent(recordsno, FULLPAGE);

  Local.CurrentWindow = 22;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealRecordsContentWindow(uint8_t wintype)    //�鿴ͨ����¼����Ԥ������������Ч��
{
  if((wintype >= 0)&&(wintype <= 2))
   {
    DisplayImageButton(&picwin_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //��ݰ��� Ԥ����
}
//--------------------------------------------------------------------------
void OperateRecordsContentWindow(uint8_t wintype, uint8_t currwindow)    //�鿴ͨ����¼���ڲ���
{
  INT8 i;
  if(wintype < 3)
    DisplayImageButton(&picwin_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //��ݰ���  ����

  switch(wintype)
   {
    case 0://�Ϸ�       a
             for(i=Records_CurrBox; i > 0; i--)
              if(Records_CurrBox > 0)
               {
                Records_CurrBox --;
                if(RecordsContent[Records_CurrBox].HavePic == 1)
                 {
                  ShowRecordsContent(Records_CurrBox, NOFULLPAGE);
                  break;
                 }
               }
             break;
    case 1://�·�     b
             for(i=Records_CurrBox; i < (Records.TotalNum -1); i++)
              if(Records_CurrBox < (Records.TotalNum -1))
               {
                Records_CurrBox ++;
                if(RecordsContent[Records_CurrBox].HavePic == 1)
                 {
                  ShowRecordsContent(Records_CurrBox, NOFULLPAGE);
                  break;
                 }
               }
             break;
    case 2://����
             DisplayRecordsWindow(REFRESH_TOP | REFRESH_MIDDLE | REFRESH_BOTTOM, Records_CurrBox);    //ˢ�� ��
             break;
    default:
             OperateQuickmenu(wintype, 105);
             break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchRecordsContentWindow(uint16_t XLCD, uint16_t YLCD) //��鴥��������
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //���ڰ�ť
    {
     isInTouch = CheckTSInButton(&picwin_button[i], XLCD, YLCD);
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
void ShowRecordsContent(uint8_t recordsno, uint8_t FullPageType)    // ͨ����¼����
{
  int xLeft,yTop;
  struct TImage tmp_image;
  char tmp_con[80];

  xLeft = 16;
  yTop = 10;
  if(recordsno <= (Records.TotalNum - 1))
   {
    tmp_image.Jpegaddr = RecordsBuff[recordsno];
    switch(RecordsContent[recordsno].HavePic)  //����ͼ���־  0   1 -- 640x480  2 -- 320x240
     {
      case 1:
             tmp_image.width = 640;
             tmp_image.height = 480;
             tmp_image.xLeft = 0;
             tmp_image.yTop = 0;
             break;
      case 2:
             tmp_image.width = 320;
             tmp_image.height = 240;
             tmp_image.xLeft = 160;
             tmp_image.yTop = 120;
             break;
     }
    tmp_image.Len = RecordsContent[recordsno].Length;
    if((tmp_image.Len % 256) == 0)
      tmp_image.FLen = tmp_image.Len;
    else
      tmp_image.FLen = (tmp_image.Len / 256)*256 + 256;

    xtm_printf("tmp_image.Jpegaddr = 0x%X, tmp_image.Len = %d\n", tmp_image.Jpegaddr, tmp_image.Len);
    DisplayImage(&tmp_image, FROMFILE, FullPageType);


    //���
    sprintf(tmp_con, "%02d", recordsno+1);
    tmp_con[2] = '\0';
    display_text(xLeft,yTop, 0, COLOR_BLACK, tmp_con, 0, 24, FullPageType);

    CheckRecordsRemoteAddr(RecordsContent[recordsno].RemoteAddr, tmp_con);
    display_text(xLeft + 50, yTop, 0, COLOR_BLACK, tmp_con, 0, 24, FullPageType);
   }
}
//--------------------------------------------------------------------------
uint8_t Init_RecordsVar(void) //��ʼ�� Records �ṹ
{
  uint8_t i;
  //��ʼ��ͨ����¼�ṹ
  Records.MaxNum = RECORDMAXITEM;

  //��ͨ����¼�ļ�
  ReadRecordsFile();

  Records.MaxNum = RECORDMAXITEM;   //���ͨ����¼��
  xtm_printf("Records.TotalNum = %d, Records.NoReadedNum = %d, Records.MaxNum = %d\n", Records.TotalNum, Records.NoReadedNum, Records.MaxNum);
  Records.CurrentInfoPage = 1;

  save_pic_sem = xSemaphoreCreateCounting(10, 0);
  xTaskCreate(SavePicFunc, "SavePic", 2048, NULL, NORMAL_PRIORITY, NULL ); 
  return 0;
}
//--------------------------------------------------------------------------
uint8_t Uninit_RecordsVar(void) //�ͷ� Info �ṹ
{
}
//--------------------------------------------------------------------------
void SavePicFunc(void)
{
	int j, i;
	uint8_t isValid = 0, isFull;
	uint16_t tmplen;
	uint32_t tmp_data;

  xtm_printf("SavePicFunc::Init...\n");
  while(1){
  	xSemaphoreTake(save_pic_sem, portMAX_DELAY);
      TmpRecordsContent.isValid = 1;
      TmpRecordsContent.isReaded = 0;

      isFull = 0;
      //����Ϣδ�����������˳����ƣ�������Ϣ���뵽ͷ��
      if(Records.TotalNum < Records.MaxNum)
       {
         if(Records.TotalNum > 0)
          for(j = (Records.TotalNum - 1); j >= 0; j --)
           {
            memcpy(&RecordsContent[j+1], &RecordsContent[j], sizeof(struct InfoContent1));
            RecordsBuff[j+1] = RecordsBuff[j];
           }
          Records.TotalNum ++;
       }
      else   //�������һ����˳����ƣ�������Ϣ���뵽ͷ��
       {
          isFull = 1;
          tmp_data = RecordsBuff[Records.TotalNum - 1];
          for(j = (Records.TotalNum - 1); j >=1; j --)
           {
            memcpy(&RecordsContent[j], &RecordsContent[j-1], sizeof(struct InfoContent1));
            RecordsBuff[j] = RecordsBuff[j-1];
           }
          RecordsBuff[0] = tmp_data;
          if((RecordsBuff[0] < RECORDPIC_ADDR)||(RecordsBuff[0] > (RECORDPIC_ADDR + 20*0x10000)))
			RecordsBuff[0] = RECORDPIC_ADDR;	
		  
       }

      RecordsContent[0].isValid = TmpRecordsContent.isValid;  //��Ч��δɾ����־   1
      RecordsContent[0].isReaded = TmpRecordsContent.isReaded; //�Ѷ���־    1
      RecordsContent[0].HavePic = TmpRecordsContent.HavePic;  //����ͼ���־  0   1 -- 640x480  2 -- 320x240
      strcpy(RecordsContent[0].RemoteAddr, TmpRecordsContent.RemoteAddr);
      strcpy(RecordsContent[0].Time, TmpRecordsContent.Time);    //����ʱ��    32
      RecordsContent[0].Type = TmpRecordsContent.Type;     //����        1    ��Ϣ���ͻ��¼�����
      RecordsContent[0].Length = TmpRecordsContent.Length;       //����        4

      if(isFull == 0)
       for(j=0; j<RECORDMAXITEM; j++)
        {
         RecordsBuff[0] = RECORDPIC_ADDR + 0x10000 * j;
		 //printf("j = %d,  RecordsBuff[0] = 0x%X\n", j,  RecordsBuff[0]);
         isValid = 1;
         for(i=1; i<Records.TotalNum; i++)
          if(RecordsBuff[0] == RecordsBuff[i])
           {
            isValid = 0;
            break;
           }
         if(isValid == 1)
           break;
        }   


      xtm_printf("WritePicFunc::isValid = %d, RecordsBuff[0] = 0x%X, RecordsContent[0].Length = %d\n", isValid, RecordsBuff[0], RecordsContent[0].Length);

      //�統ǰΪͨ����¼���ڣ�ˢ����Ļ
      if(Local.CurrentWindow == 21)
       {
          ShowRecordsList(REFRESH, NOFULLPAGE);
       }
      //дͨ����¼�����ļ�//дͨ����¼�ļ�
      WriteRecordsFile();   
	  ClearWatchDog();
      Recv_NewRecords();
      if(RecordsContent[0].HavePic != 0)
       {
          Local.NewRecPic = 1;  //������Ӱ
          
          SaveToFlash(RecordsBuff[0], Memory6_Stream, RecordsContent[0].Length);
       }   	
  }
}
//��ͨ����¼�ļ�
void ReadRecordsFile(void)
{
  uint16_t i;
  uint32_t tmp_addr;
  ReadFromFlash(RECORDCFG_ADDR, Flash_Stream, 0x200);
  
  memcpy(&Records, Flash_Stream, sizeof(struct Info1));
  if((Records.FlashHead[0] == 'R')&&(Records.FlashHead[1] == 'E')&&(Records.FlashHead[2] == 'C')&&(Records.FlashHead[3] == 'O'))
   {
     if(Records.TotalNum > RECORDMAXITEM)
       Records.TotalNum = 0;
	 xtm_printf("ReadRecordsFile::1, Records.TotalNum = %d\n", Records.TotalNum);
     for(i=0; i<Records.TotalNum; i++)
      {
       ClearWatchDog();
       tmp_addr = RECORDCFG_ADDR + 0x200 + i*0x200;
       ReadFromFlash(tmp_addr, Flash_Stream, 0x200);
       memcpy(&RecordsContent[i], Flash_Stream, sizeof(struct InfoContent1));
       memcpy(&RecordsBuff[i], Flash_Stream + 120, sizeof(uint32_t));
       xtm_printf("tmp_addr = 0x%X, Length = %d, RecordsBuff[%d] = 0x%X\n", tmp_addr, RecordsContent[i].Length, i, RecordsBuff[i]);
      }
   }
  else
   {
    ResetRecordsCfg();    //��λͨ����¼�����ļ�
    WriteRecordsCfgFile();     //дͨ����¼�����ļ�
   }
}
//--------------------------------------------------------------------------
//дͨ����¼�ļ�
void WriteRecordsFile(void)
{
  uint16_t i;
  //WriteRecordsCfgFile
  xtm_printf("Records.TotaoNum = %d\n", Records.TotalNum);
  memcpy(Flash_Stream, &Records, sizeof(struct Info1));

  //WriteRecordsFile
  for(i=0; i<RECORDMAXITEM; i++)               //   Records.TotalNum
   {
    memcpy(Flash_Stream + 0x200 + i*0x200, &RecordsContent[i], sizeof(struct InfoContent1));
    memcpy(Flash_Stream + 0x200 + i*0x200 + 120, &RecordsBuff[i], sizeof(uint32_t));
    
   }
  SaveToFlash(RECORDCFG_ADDR, Flash_Stream, 0x200 + 0x200 * RECORDMAXITEM);
}
//--------------------------------------------------------------------------
void ResetRecordsCfg(void)    //��λͨ����¼�����ļ�
{
  strcpy(Records.FlashHead, "RECO"); //Flash�Ѵ��־
  Records.MaxNum = RECORDMAXITEM;   //�����Ϣ��
  Records.TotalNum = 0; //��Ϣ����
  Records.NoReadedNum = 0; //δ����Ϣ����
  Records.TotalInfoPage = 0;   //����Ϣҳ��
  Records.CurrentInfoPage = 0; //��ǰ��Ϣҳ
  Records.CurrNo = 0;    //��ǰ��Ϣ���
  Records.CurrPlayNo = 0;  //��ǰ�������
}
//--------------------------------------------------------------------------
void WriteRecordsCfgFile(void)     //дͨ����¼�����ļ�
{
  memcpy(Flash_Stream, &Records, sizeof(struct Info1));
  SaveToFlash(RECORDCFG_ADDR, Flash_Stream, 0x200);
}
//--------------------------------------------------------------------------
void Recv_NewRecords(void)
{
}
//--------------------------------------------------------------------------
//д��Ƭ����
void WritePicFunc(void)
{
	xSemaphoreGive(save_pic_sem);

}
//--------------------------------------------------------------------------
