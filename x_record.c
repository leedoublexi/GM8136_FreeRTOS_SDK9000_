
#include "stdlib.h"

#define x_record_h

#define CommonH
#include "common.h"

void DisplayRecordsWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //显示通话记录窗口
void PreDealRecordsWindow(uint8_t wintype);    //通话记录窗口预处理，按键按下效果
void OperateRecordsWindow(uint8_t wintype, uint8_t currwindow);    //通话记录窗口操作
void CheckTouchRecordsWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

void ShowRecordsList(uint8_t refreshflag, uint8_t FullPageType);    //通话记录列表
void CheckRecordsRemoteAddr(char *RemoteAddr, char *tmp_con);

uint8_t Records_CurrBox;
void DisplayRecordsContentWindow(uint8_t recordsno);    //通话记录查看信息窗口
void PreDealRecordsContentWindow(uint8_t wintype);    //查看通话记录窗口预处理，按键按下效果
void OperateRecordsContentWindow(uint8_t wintype, uint8_t currwindow);    //查看通话记录窗口操作
void CheckTouchRecordsContentWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

void ShowRecordsContent(uint8_t recordsno, uint8_t FullPageType);    // 通话记录内容

uint8_t Init_RecordsVar(void); //初始化 Records 结构
uint8_t Uninit_RecordsVar(void); //释放 Records 结构

xSemaphoreHandle *save_pic_sem;
void SavePicFunc(void);


//读通话记录文件
void ReadRecordsFile(void);
//写通话记录文件
void WriteRecordsFile(void);

void ResetRecordsCfg(void);    //复位通话记录配置文件
void WriteRecordsCfgFile(void);     //写通话记录设置文件

void Recv_NewRecords(void);
//写照片函数
void WritePicFunc(void);
//---------------------------------------------------------------------------
void DisplayRecordsWindow(uint8_t RefreshType, uint8_t Curr_Sec)    //显示通话记录窗口
{
  RefreshBackWindow(RefreshType, FULLPAGE); //刷新背景

  DisplayImage(&records_image, FROMFILE, FULLPAGE);

  Records.CurrNo = Curr_Sec;            //当前通话记录序号  0---n-1
  Records.CurrentInfoPage = Records.CurrNo /RECORDNUMPERPAGE + 1;     //当前通话记录页

  ShowRecordsList(REFRESH, FULLPAGE);

  Local.CurrentWindow = 21;
  Local.NewRecPic = 0;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealRecordsWindow(uint8_t wintype)    //通话记录窗口预处理，按键按下效果
{
  if((wintype >= 0)&&(wintype <= 2))
   {
    DisplayImageButton(&records_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //快捷按键 预处理
}
//--------------------------------------------------------------------------
void OperateRecordsWindow(uint8_t wintype, uint8_t currwindow)    //通话记录窗口操作
{
  uint8_t i;
  uint8_t CurrNo;
  uint16_t tmp;
  if(wintype < 3)
    DisplayImageButton(&records_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //快捷按键  弹起

  switch(wintype)
   {
    case 0://上翻       a
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
    case 1://下翻     b
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
    case 2://删除           c
               if(Records.CurrNo < Records.TotalNum)
                {
                 //顺序前移
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
   
                 WriteRecordsFile();      //写通话记录设置文件  //写通话记录文件
                }
             break;
    case 40://通话记录1行           c
    case 41://通话记录2行           c
    case 42://通话记录3行           c
    case 43://通话记录4行           c
    case 44://通话记录5行           c
           //当前信息序号
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
void CheckTouchRecordsWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //窗口按钮
    {
     isInTouch = CheckTSInButton(&records_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   for(i=0; i<RECORDNUMPERPAGE; i++)  //窗口按钮
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
    TouchNum = CheckBigmenuFunc(XLCD, YLCD);  //快捷按钮操作
    if(TouchNum >= 100)
      isInTouch = 1;
   }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
void ShowRecordsList(uint8_t refreshflag, uint8_t FullPageType)    //通话记录显示方框 通话记录列表
{
  uint8_t i, j;
  uint8_t PageTotalNum;  //当页信息数量
  uint8_t CurrNo;
  char tmp_con[80];
  uint16_t tmp_len;  //预览长度
  uint8_t NoInPage;
  uint16_t ascnum, tmp;
  uint8_t DeltaHei;
  uint16_t FontColor;

  tmp_len = 10;

           if(Records.TotalNum > 0)
            {
             //总页数
             if((Records.TotalNum % INFONUMPERPAGE) == 0)
               tmp = Records.TotalNum /INFONUMPERPAGE;
             else
               tmp = Records.TotalNum /INFONUMPERPAGE + 1;
             Records.TotalInfoPage = tmp;

             //当前页
             tmp = Records.CurrNo / INFONUMPERPAGE + 1;
             Records.CurrentInfoPage = tmp;
             if(Records.CurrentInfoPage < Records.TotalInfoPage)
               PageTotalNum = INFONUMPERPAGE;
             else
               PageTotalNum = Records.TotalNum - (Records.CurrentInfoPage - 1)*RECORDNUMPERPAGE;
             //当前信息在本页中的位置
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
               //序号
               sprintf(tmp_con, "%02d", (Records.CurrentInfoPage-1)*RECORDNUMPERPAGE+i+1);
               tmp_con[2] = '\0';
               //xtm_printf("tmp_con = %s\n", tmp_con);
               //display_text(recordsrow_button[i].xLeft+10, recordsrow_button[i].yTop + DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);
               display_text(recordsrow_button[i].xLeft+2, recordsrow_button[i].yTop + DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);
               switch(RecordsContent[CurrNo].Type)
                {
                 case 0:
                        strcpy(tmp_con,  "主叫");
                        break;
                 case 1:
                        strcpy(tmp_con,  "未接听");
                        break;
                 case 2:
                        strcpy(tmp_con,  "已接听");
                        break;
                 default:
                        strcpy(tmp_con,  "未接听");
                        break;
                }

               //display_text(recordsrow_button[i].xLeft+100, recordsrow_button[i].yTop+DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);
               //CheckRecordsRemoteAddr(RecordsContent[CurrNo].RemoteAddr);
               //display_text(recordsrow_button[i].xLeft+280, recordsrow_button[i].yTop+DeltaHei, 0, FontColor, RecordsContent[CurrNo].RemoteAddr, 0, 24, FullPageType);


               display_text(recordsrow_button[i].xLeft+50, recordsrow_button[i].yTop+DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);
               CheckRecordsRemoteAddr(RecordsContent[CurrNo].RemoteAddr, tmp_con);
               display_text(recordsrow_button[i].xLeft+138, recordsrow_button[i].yTop+DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);

               //有无照片Icon
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
            sprintf(tmp_con, "门口 %s幢%s单元 %s", Build, Unit, Equiq);
            RemoteAddr[8] = '\0';
            break;
    case 'W':
            memcpy(Build, RemoteAddr + 1, 4);
            Build[4] = '\0';
            sprintf(tmp_con, "围墙 %s", Build);
            RemoteAddr[5] = '\0';
            break;
    case 'Z':
            memcpy(Build, RemoteAddr + 1, 4);
            Build[4] = '\0';
            sprintf(tmp_con, "中心 %s", Build);
            RemoteAddr[5] = '\0';
            break;
    case 'B':
            memcpy(Build, RemoteAddr + 1, 4);
            Build[4] = '\0';

            memcpy(Equiq, RemoteAddr + 7, 1);
            Equiq[1] = '\0';
            sprintf(tmp_con, "别墅 %s幢 %s", Build, Equiq);

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
            sprintf(tmp_con, "%s幢%s单元%s层%s房 设备%s", Build, Unit, Storey, Room, Equiq);
            RemoteAddr[12] = '\0';
            break;
    case 'H':
            memcpy(Equiq, RemoteAddr + 11, 1);
            Equiq[1] = '\0';
            sprintf(tmp_con, "二次门口 %s", Equiq);            
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
void DisplayRecordsContentWindow(uint8_t recordsno)    //显示查看通话记录窗口
{
  DisplayImage(&picwin_image, FROMFILE, FULLPAGE);

  Records_CurrBox = recordsno;
  ShowRecordsContent(recordsno, FULLPAGE);

  Local.CurrentWindow = 22;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealRecordsContentWindow(uint8_t wintype)    //查看通话记录窗口预处理，按键按下效果
{
  if((wintype >= 0)&&(wintype <= 2))
   {
    DisplayImageButton(&picwin_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //快捷按键 预处理
}
//--------------------------------------------------------------------------
void OperateRecordsContentWindow(uint8_t wintype, uint8_t currwindow)    //查看通话记录窗口操作
{
  INT8 i;
  if(wintype < 3)
    DisplayImageButton(&picwin_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //快捷按键  弹起

  switch(wintype)
   {
    case 0://上翻       a
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
    case 1://下翻     b
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
    case 2://返回
             DisplayRecordsWindow(REFRESH_TOP | REFRESH_MIDDLE | REFRESH_BOTTOM, Records_CurrBox);    //刷新 中
             break;
    default:
             OperateQuickmenu(wintype, 105);
             break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchRecordsContentWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //窗口按钮
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
    TouchNum = CheckBigmenuFunc(XLCD, YLCD);  //快捷按钮操作
    if(TouchNum >= 100)
      isInTouch = 1;
   }

  if(isInTouch == 1)
   {
    KeyTouchOperate(TouchNum);
   }
}
//--------------------------------------------------------------------------
void ShowRecordsContent(uint8_t recordsno, uint8_t FullPageType)    // 通话记录内容
{
  int xLeft,yTop;
  struct TImage tmp_image;
  char tmp_con[80];

  xLeft = 16;
  yTop = 10;
  if(recordsno <= (Records.TotalNum - 1))
   {
    tmp_image.Jpegaddr = RecordsBuff[recordsno];
    switch(RecordsContent[recordsno].HavePic)  //有无图像标志  0   1 -- 640x480  2 -- 320x240
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


    //序号
    sprintf(tmp_con, "%02d", recordsno+1);
    tmp_con[2] = '\0';
    display_text(xLeft,yTop, 0, COLOR_BLACK, tmp_con, 0, 24, FullPageType);

    CheckRecordsRemoteAddr(RecordsContent[recordsno].RemoteAddr, tmp_con);
    display_text(xLeft + 50, yTop, 0, COLOR_BLACK, tmp_con, 0, 24, FullPageType);
   }
}
//--------------------------------------------------------------------------
uint8_t Init_RecordsVar(void) //初始化 Records 结构
{
  uint8_t i;
  //初始化通话记录结构
  Records.MaxNum = RECORDMAXITEM;

  //读通话记录文件
  ReadRecordsFile();

  Records.MaxNum = RECORDMAXITEM;   //最大通话记录数
  xtm_printf("Records.TotalNum = %d, Records.NoReadedNum = %d, Records.MaxNum = %d\n", Records.TotalNum, Records.NoReadedNum, Records.MaxNum);
  Records.CurrentInfoPage = 1;

  save_pic_sem = xSemaphoreCreateCounting(10, 0);
  xTaskCreate(SavePicFunc, "SavePic", 2048, NULL, NORMAL_PRIORITY, NULL ); 
  return 0;
}
//--------------------------------------------------------------------------
uint8_t Uninit_RecordsVar(void) //释放 Info 结构
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
      //如信息未到最大数量，顺序后移，将新信息插入到头部
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
      else   //丢弃最后一条，顺序后移，将新信息插入到头部
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

      RecordsContent[0].isValid = TmpRecordsContent.isValid;  //有效，未删除标志   1
      RecordsContent[0].isReaded = TmpRecordsContent.isReaded; //已读标志    1
      RecordsContent[0].HavePic = TmpRecordsContent.HavePic;  //有无图像标志  0   1 -- 640x480  2 -- 320x240
      strcpy(RecordsContent[0].RemoteAddr, TmpRecordsContent.RemoteAddr);
      strcpy(RecordsContent[0].Time, TmpRecordsContent.Time);    //接收时间    32
      RecordsContent[0].Type = TmpRecordsContent.Type;     //类型        1    信息类型或事件类型
      RecordsContent[0].Length = TmpRecordsContent.Length;       //长度        4

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

      //如当前为通话记录窗口，刷新屏幕
      if(Local.CurrentWindow == 21)
       {
          ShowRecordsList(REFRESH, NOFULLPAGE);
       }
      //写通话记录设置文件//写通话记录文件
      WriteRecordsFile();   
	  ClearWatchDog();
      Recv_NewRecords();
      if(RecordsContent[0].HavePic != 0)
       {
          Local.NewRecPic = 1;  //有新留影
          
          SaveToFlash(RecordsBuff[0], Memory6_Stream, RecordsContent[0].Length);
       }   	
  }
}
//读通话记录文件
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
    ResetRecordsCfg();    //复位通话记录配置文件
    WriteRecordsCfgFile();     //写通话记录设置文件
   }
}
//--------------------------------------------------------------------------
//写通话记录文件
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
void ResetRecordsCfg(void)    //复位通话记录配置文件
{
  strcpy(Records.FlashHead, "RECO"); //Flash已存标志
  Records.MaxNum = RECORDMAXITEM;   //最大信息数
  Records.TotalNum = 0; //信息总数
  Records.NoReadedNum = 0; //未读信息总数
  Records.TotalInfoPage = 0;   //总信息页数
  Records.CurrentInfoPage = 0; //当前信息页
  Records.CurrNo = 0;    //当前信息序号
  Records.CurrPlayNo = 0;  //当前播放序号
}
//--------------------------------------------------------------------------
void WriteRecordsCfgFile(void)     //写通话记录设置文件
{
  memcpy(Flash_Stream, &Records, sizeof(struct Info1));
  SaveToFlash(RECORDCFG_ADDR, Flash_Stream, 0x200);
}
//--------------------------------------------------------------------------
void Recv_NewRecords(void)
{
}
//--------------------------------------------------------------------------
//写照片函数
void WritePicFunc(void)
{
	xSemaphoreGive(save_pic_sem);

}
//--------------------------------------------------------------------------
