
#include "stdlib.h"

#define x_info_h

#define CommonH
#include "common.h"

void DisplayInfoWindow(uint8_t RefreshType, uint8_t Curr_Sec);    //显示信息窗口
void PreDealInfoWindow(uint8_t wintype);    //信息窗口预处理，按键按下效果
void OperateInfoWindow(uint8_t wintype, uint8_t currwindow);    //信息窗口操作
void CheckTouchInfoWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

void ShowInfoList(uint8_t infotype, uint8_t refreshflag, uint8_t FullPageType);    //信息显示方框（一级）  信息列表

uint8_t Info_CurrBox;
void DisplayInfoContentWindow(uint8_t infotype, uint8_t infono);    //显示查看信息窗口
void PreDealInfoContentWindow(uint8_t wintype);    //查看信息窗口预处理，按键按下效果
void OperateInfoContentWindow(uint8_t wintype, uint8_t currwindow);    //查看信息窗口操作
void CheckTouchInfoContentWindow(uint16_t XLCD, uint16_t YLCD); //检查触摸屏操作

void ShowInfoContent(uint8_t infotype, uint8_t infono, uint8_t pageno, uint8_t FullPageType);    // 信息内容
void CreateContentRow(char *Content, uint16_t nLength, char create_row[MAXROW][INFOROWLEN + 2]);  //产生要显示的行内容

uint8_t Init_InfoVar(void); //初始化 Info 结构
uint8_t Uninit_InfoVar(void); //释放 Info 结构
//读信息文件
void ReadInfoFile(void);
//写信息文件
void WriteInfoFile(void);

void ResetInfoCfg(void);    //复位信息配置文件
void WriteInfoCfgFile(void);     //写信息设置文件

void Recv_NewInfo(void);

struct displayinfo1 displayinfo;
//---------------------------------------------------------------------------
void DisplayInfoWindow(uint8_t RefreshType, uint8_t Curr_Sec)    //显示信息窗口
{
  uint8_t j;
  RefreshBackWindow(RefreshType, FULLPAGE); //刷新背景

  DisplayImage(&info_image, FROMFILE, FULLPAGE);

  Local.NewInfo = 0;
//  SetGpio(GPIOD_OUT_DATA, INFO_LED_HIGH, SET_HIGH);  //关信息 LED
  Info.CurrNo = Curr_Sec;            //当前信息序号  0---n-1
  Info.CurrentInfoPage = Info.CurrNo /INFONUMPERPAGE + 1;     //当前信息页

  ShowInfoList(0, REFRESH, FULLPAGE);

  //for(j=0; j<Info[0].TotalNum; j++)
  //  xtm_printf("DisplayInfoWindow::InfoBuff[%d] = %s\n", j, InfoBuff[j]);
  Local.CurrentWindow = 111;

  Local.fb_page = !Local.fb_page;
  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealInfoWindow(uint8_t wintype)    //信息窗口预处理，按键按下效果
{
  if((wintype >= 0)&&(wintype <= 2))
   {
    DisplayImageButton(&info_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //快捷按键 预处理
}
//--------------------------------------------------------------------------
void OperateInfoWindow(uint8_t wintype, uint8_t currwindow)    //信息窗口操作
{
  uint8_t i;
  uint8_t TmpInfoNo;
  uint16_t tmp;
  if(wintype < 3)
    DisplayImageButton(&info_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //快捷按键  弹起

  switch(wintype)
   {
    case 0://上翻       a
             if(Info.CurrentInfoPage > 1)
              {
               Info.CurrentInfoPage --;
               xtm_printf("OperateInfoWindow1:: Info.CurrNo = %d, Info.CurrentInfoPage = %d\n",  Info.CurrNo, Info.CurrentInfoPage);
               tmp = (uint16_t)INFONUMPERPAGE * (Info.CurrentInfoPage - 1);
               Info.CurrNo = tmp;
               xtm_printf("OperateInfoWindow2:: Info.CurrNo = %d, Info.CurrentInfoPage = %d\n",  Info.CurrNo, Info.CurrentInfoPage);
               ShowInfoList(0, REFRESH, NOFULLPAGE);
              }
           break;
    case 1://下翻     b
             //xtm_printf("Info.CurrentInfoPage = %d, Info.TotalInfoPage = %d\n", Info.CurrentInfoPage, Info.TotalInfoPage);
             if(Info.CurrentInfoPage < Info.TotalInfoPage)
              {
               Info.CurrentInfoPage ++;
               xtm_printf("OperateInfoWindow3:: Info.CurrNo = %d, Info.CurrentInfoPage = %d\n",  Info.CurrNo, Info.CurrentInfoPage);
               tmp = (uint16_t)INFONUMPERPAGE * (Info.CurrentInfoPage - 1);
               Info.CurrNo = tmp;
               xtm_printf("OperateInfoWindow4:: Info.CurrNo = %d, Info.CurrentInfoPage = %d\n",  Info.CurrNo, Info.CurrentInfoPage);
               ShowInfoList(0, REFRESH, NOFULLPAGE);
              }
           break;
    case 2://删除           c
               if(Info.CurrNo < Info.TotalNum)
                {
                 //顺序前移
                 for(i = Info.CurrNo; i < (Info.TotalNum - 1); i ++)
                  {
                   memcpy(&InfoContent[i], &InfoContent[i+1], sizeof(struct InfoContent1));
                   strcpy(InfoBuff[i], InfoBuff[i+1]);
                  }
                 Info.TotalNum --;
                 xtm_printf("Info.CurrNo = %d, Info.TotalNum = %d\n", Info.CurrNo, Info.TotalNum);
                 if(Info.CurrNo > (Info.TotalNum - 1))
                   Info.CurrNo = Info.TotalNum - 1;
                 ShowInfoList(0, NOREFRESH, NOFULLPAGE);
                 WriteInfoCfgFile();     //写信息设置文件
                 WriteInfoFile();       //写信息文件
                }
             break;
    case 40://信息1行           c
    case 41://信息2行           c
    case 42://信息3行           c
    case 43://信息4行           c
    case 44://信息5行           c
           //当前信息序号
             TmpInfoNo = (Info.CurrentInfoPage-1)*INFONUMPERPAGE+(wintype - 40);
             xtm_printf("TmpInfoNo = %d, Info.TotalNum = %d \n", TmpInfoNo,
                    Info.TotalNum);
             if(Info.CurrNo == TmpInfoNo)
              {
               if(TmpInfoNo < (Info.TotalNum))
                {
                 Info.CurrNo = TmpInfoNo;
                 DisplayInfoContentWindow(0, Info.CurrNo);
                }
              }
             else
              {
               if(TmpInfoNo < (Info.TotalNum))
                {
                 Info.CurrNo = TmpInfoNo;
                 ShowInfoList(0, NOREFRESH, NOFULLPAGE);
                }
              }
           break;
    default:
          OperateQuickmenu(wintype, 104);
          break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchInfoWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //窗口按钮
    {
     isInTouch = CheckTSInButton(&info_button[i], XLCD, YLCD);
     if(isInTouch == 1)
      {
       TouchNum = i;
       break;
      }
    }

  if(isInTouch == 0)
   for(i=0; i<INFONUMPERPAGE; i++)  //窗口按钮
    {
     isInTouch = CheckTSInButton(&inforow_button[i], XLCD, YLCD);
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
void ShowInfoList(uint8_t infotype, uint8_t refreshflag, uint8_t FullPageType)    //信息显示方框（一级）  信息列表
{
  uint8_t i, j;
  uint8_t PageTotalNum;  //当页信息数量
  uint8_t CurrNo;
  char tmp_con[20];
  uint16_t tmp_len;  //预览长度
  uint8_t NoInPage;
  uint16_t ascnum, tmp;
  uint8_t DeltaHei;
  uint16_t FontColor;

  tmp_len = 10;

  switch(infotype)
   {
    case 0: //普通信息
           if(Info.TotalNum > 0)
            {
             //总页数
             if((Info.TotalNum % INFONUMPERPAGE) == 0)
               tmp = Info.TotalNum /INFONUMPERPAGE;
             else
               tmp = Info.TotalNum /INFONUMPERPAGE + 1;
             Info.TotalInfoPage = tmp;

             //当前页
             tmp = Info.CurrNo / INFONUMPERPAGE + 1;
             Info.CurrentInfoPage = tmp;
             if(Info.CurrentInfoPage < Info.TotalInfoPage)
               PageTotalNum = INFONUMPERPAGE;
             else
               PageTotalNum = Info.TotalNum - (Info.CurrentInfoPage - 1)*INFONUMPERPAGE;
             //当前信息在本页中的位置
             NoInPage = (Info.CurrNo)%INFONUMPERPAGE;

             xtm_printf("ShowInfoList::Info.CurrNo = %d, Info.TotalNum = %d, Info.CurrentInfoPage = %d\n", Info.CurrNo, Info.TotalNum, Info.CurrentInfoPage);
             for(i = 0; i < INFONUMPERPAGE; i++)
              {
               if(i == NoInPage)
                 DisplayImageButton(&inforow_button[i], IMAGEDOWN, FROMFILE, FullPageType);
               else
                 DisplayImageButton(&inforow_button[i], IMAGEUP, FROMFILE, FullPageType);
              }

             for(i = 0; i < PageTotalNum; i++)
              {
               if(i == NoInPage)
                 FontColor = COLOR_WHITE;
               else
                 FontColor = COLOR_BLACK;
                 
               //xtm_printf("FontColor = 0x%X\n", FontColor);
               CurrNo = (Info.CurrentInfoPage - 1)*INFONUMPERPAGE + i;

               DeltaHei = (inforow_button[i].height - 24)/2;
               //序号
               sprintf(tmp_con, "%02d\0", (Info.CurrentInfoPage-1)*INFONUMPERPAGE+i+1);
               tmp_con[2] = '\0';
               //xtm_printf("tmp_con = %s\n", tmp_con);
               display_text(inforow_button[i].xLeft+10, inforow_button[i].yTop + DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);

               //xtm_printf("InfoBuff[%d] = %s\n", CurrNo, InfoBuff[CurrNo]);
               //内容
               if(InfoContent[CurrNo].Length >= tmp_len)
                {
                 memcpy(tmp_con, InfoBuff[CurrNo], tmp_len);
                 tmp_con[tmp_len] = '\0';
                 for(j=0; j<tmp_len; j++)
                  if(tmp_con[j] == '\r')
                    tmp_con[j] = '\0';
                 if((uint8_t)tmp_con[8] < 0xA1)
                   tmp_con[9] = '\0';

                 ascnum = 0;
                 for(j=0; j<strlen(tmp_con); j++)
                  {
                   if((uint8_t)tmp_con[j] < 0xA1)
                     ascnum ++;
                   else
                     j ++;
                  }
                 if((ascnum % 2) != 0)
                   tmp_con[9] = '\0';
                }
               else
                {
                 memcpy(tmp_con, InfoBuff[CurrNo], InfoContent[CurrNo].Length);
                 tmp_con[InfoContent[CurrNo].Length] = '\0';
                 for(j=0; j<InfoContent[CurrNo].Length; j++)
                  if(tmp_con[j] == '\r')
                    tmp_con[j] = '\0';
                }
               display_text(inforow_button[i].xLeft+60, inforow_button[i].yTop+DeltaHei, 0, FontColor, tmp_con, 0, 24, FullPageType);

               //时间
             /*  memcpy(tmp_con, InfoContent[infotype][CurrNo].Time, tmp_len);
                 tmp_con[tmp_len] = '\0';
               outxy24(inforow_button[i].xLeft+200, inforow_button[i].yTop+DeltaHei, 1,
                       fontcolor, 1, 1, tmp_con, 0, Local.CurrFbPage);         */

               display_text(inforow_button[i].xLeft+350, inforow_button[i].yTop+DeltaHei, 0, FontColor, "管理处", 0, 24, FullPageType);
              }
            }
           else
             for(i = 0; i < INFONUMPERPAGE; i++)
               DisplayImageButton(&inforow_button[i], IMAGEUP, FROMFILE, FullPageType);
           break;
   }
}
//--------------------------------------------------------------------------
void DisplayInfoContentWindow(uint8_t infotype, uint8_t infono)    //显示查看信息窗口
{
  DisplayImage(&infowin_image, FROMFILE, NOFULLPAGE);

  Info_CurrBox = infono;
  xtm_printf("InfoContent[infono].Length= %d\n", InfoContent[infono].Length);
  displayinfo.totalpage = TOTALPAGE;
  displayinfo.pageno = 0;
  CreateContentRow(InfoBuff[infono], InfoContent[infono].Length, displayinfo.content_row);
  ShowInfoContent(0, infono, 0, NOFULLPAGE);
  
  Local.CurrentWindow = 112;
  
//  Local.fb_page = !Local.fb_page;
//  selectym(Local.fb_page);
}
//--------------------------------------------------------------------------
void PreDealInfoContentWindow(uint8_t wintype)    //查看信息窗口预处理，按键按下效果
{
  if((wintype >= 0)&&(wintype <= 2))
   {
    DisplayImageButton(&infowin_button[wintype], IMAGEDOWN, FROMFILE, NOFULLPAGE);
   }

  PreDealBigmenuButton(wintype);  //快捷按键 预处理
}
//--------------------------------------------------------------------------
void OperateInfoContentWindow(uint8_t wintype, uint8_t currwindow)    //查看信息窗口操作
{
  uint8_t i;
  uint8_t TmpInfoNo;
  if(wintype < 3)
    DisplayImageButton(&infowin_button[wintype], IMAGEUP, FROMFILE, NOFULLPAGE);

  ShowPressQuickmenu_Button(wintype);  //快捷按键  弹起

  switch(wintype)
   {
    case 0://上翻       a
            if(displayinfo.pageno > 0)
             {
              displayinfo.pageno --;
              ShowInfoContent(0, Info_CurrBox, displayinfo.pageno, NOFULLPAGE);
             }
             break;
    case 1://下翻     b
             if(displayinfo.pageno < (displayinfo.totalpage - 1))
              {
               displayinfo.pageno ++;
               ShowInfoContent(0, Info_CurrBox, displayinfo.pageno, NOFULLPAGE);
              }      
             break;
    case 2://返回
             DisplayInfoWindow(REFRESH_MIDDLE, Info_CurrBox);    //刷新 中
             break;
    default:
             OperateQuickmenu(wintype, 104);
             break;
   }
}
//--------------------------------------------------------------------------
void CheckTouchInfoContentWindow(uint16_t XLCD, uint16_t YLCD) //检查触摸屏操作
{
  uint8_t i;
  uint8_t isInTouch;
  uint8_t TouchNum;
  isInTouch = 0;

  if(isInTouch == 0)
   for(i=0; i<3; i++)  //窗口按钮
    {
     isInTouch = CheckTSInButton(&infowin_button[i], XLCD, YLCD);
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
void ShowInfoContent(uint8_t infotype, uint8_t infono, uint8_t pageno, uint8_t FullPageType)    // 信息内容
{
  int xLeft,yTop,yHeight;
  int i, j;
  char tmp_con[50];
  int tmp_len;  //预览长度
  int tmp_row;
  int numperpage;

  DisplayImage(&infocontent_image, FROMFILE, FullPageType);
  tmp_len = 20;

  xLeft = infocontent_image.xLeft;
  yTop = infocontent_image.yTop + 6;
  yHeight = 38;
  if(infono <= (Info.TotalNum - 1))
   {
    for(i=0; i<50; i++)
      tmp_con[i] = '\0';
    sprintf(tmp_con, "第 %d 条" , infono + 1);
    display_text(xLeft+16, yTop+0*yHeight, 0, COLOR_BLACK, tmp_con, 0, 24, FullPageType);

    for(i=0; i<50; i++)
      tmp_con[i] = '\0';
    sprintf(tmp_con, "第 %d 页" , displayinfo.pageno + 1);
    display_text(xLeft+150, yTop+0*yHeight, 0, COLOR_BLACK, tmp_con, 0, 24, FullPageType);

  //  memcpy(tmp_con, InfoContent[infotype][infono].Time, tmp_len);
  //  tmp_con[tmp_len] = '\0';
  //  display_text(xLeft + 100, yTop+0*yHeight, 0, COLOR_BLACK, tmp_con, 0, 24);


    yTop = yTop+1*yHeight;

    if((displayinfo.totalrow % PAGEPERROW) == 0)
      displayinfo.totalpage = displayinfo.totalrow /PAGEPERROW;
    else
      displayinfo.totalpage = displayinfo.totalrow /PAGEPERROW + 1;

    if(displayinfo.totalpage == (displayinfo.pageno + 1))
      numperpage = displayinfo.totalrow - displayinfo.pageno*PAGEPERROW;
    else
      numperpage = PAGEPERROW;

    for(i=displayinfo.pageno*PAGEPERROW; i<(displayinfo.pageno*PAGEPERROW + numperpage); i++)
     {
      display_text(xLeft + 16, yTop+(i-displayinfo.pageno*PAGEPERROW)*yHeight, 0, COLOR_BLACK, displayinfo.content_row[i], 0, 24, FullPageType);
      xtm_printf("displayinfo.content_row[i] = %s\n", displayinfo.content_row[i]);
     } 
   }
}
//--------------------------------------------------------------------------
void CreateContentRow(char *Content, uint16_t nLength, char create_row[MAXROW][INFOROWLEN + 2])  //产生要显示的行内容
{
  int i,j;
  int row;
  int asciinum;
  int row_pixel;
  row = 0;
  for(i=0; i<MAXROW; i++)
   for(j=0; j<(INFOROWLEN + 2); j++)
    create_row[i][j] = '\0';
  j = 0;
  asciinum = 0;
  row_pixel = 0;
  for(i=0; i<nLength; i++)
   {
    if((Content[i] == '\r')&&(Content[i+1] == '\n'))
     {
      create_row[row][j] = '\0';
      row ++;
      asciinum = 0;
      j = 0;
      row_pixel = 0;
      i++;
     }
    else
     {
      if((uint8_t)Content[i] >= 0xA1)
       {
        create_row[row][j] = Content[i];
        i ++;
        j ++;
        create_row[row][j] = Content[i];
        j ++;
        row_pixel += 24;
       }
      else
       {
          create_row[row][j] = Content[i];
          j ++;
          row_pixel += 16;
          asciinum ++;
       }
      if((j >= (INFOROWLEN-1)) || (row_pixel >= INFOROWPIXEL))
       {
    //    printf("j = %d, asciinum = %d, i = %d\n", j, asciinum, i);
        if((asciinum % 2) != 0)
         {
          create_row[row][j] = ' ';
         }
        create_row[row][j+1] = '\0';
        row ++;
        asciinum = 0;
        j = 0;
        row_pixel = 0;
       }
     }
    if(row >= MAXROW)
      break;
   }
  displayinfo.totalrow = row+1;
  if((displayinfo.totalrow % PAGEPERROW) == 0)
    displayinfo.totalpage = displayinfo.totalrow / PAGEPERROW;
  else
    displayinfo.totalpage = displayinfo.totalrow / PAGEPERROW + 1;          

  //printf("0 1 2 3 4 5 6 7 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", Content[0], Content[1], Content[2], Content[3], Content[4], Content[5]);
  //for(i=0; i<displayinfo.totalrow; i++)
  //  printf("create_row[row] = %s, len = %d\n", create_row[i], strlen(create_row[i]));      
}
//--------------------------------------------------------------------------
uint8_t Init_InfoVar(void) //初始化 Info 结构
{
  uint16_t i;
  //信息内容，如更改了短信息最大条数，要注意此处的内存分配
  for(i=0; i<INFOMAXITEM; i++)
   {
    InfoBuff[i] = INFO_BUF + i*INFOMAXSIZE;   
   }
	
  //初始化短信息结构
  Info.MaxNum = INFOMAXITEM;

  //读信息文件
  ReadInfoFile();

  Info.MaxNum = INFOMAXITEM;   //最大信息数
  xtm_printf("Info.TotalNum = %d, Info.NoReadedNum = %d, Info.MaxNum = %d\n", Info.TotalNum, Info.NoReadedNum, Info.MaxNum);
  Info.CurrentInfoPage = 1;

  return 0;
}
//--------------------------------------------------------------------------
uint8_t Uninit_InfoVar(void) //释放 Info 结构
{
}
//--------------------------------------------------------------------------
//读信息文件
void ReadInfoFile(void)
{
  uint16_t i;
  printf("ReadInfoFile::1\n");
  ReadFromFlash(INFO_ADDR, Flash_Stream, 0x200);
  memcpy(&Info, Flash_Stream, sizeof(struct Info1));
  if((Info.FlashHead[0] == 'I')&&(Info.FlashHead[1] == 'N')&&(Info.FlashHead[2] == 'F')&&(Info.FlashHead[3] == 'O'))
   {
     if(Info.TotalNum > INFOMAXITEM)
       Info.TotalNum = 0;
	 printf("ReadInfoFile::2, Info.TotalNum = %d\n", Info.TotalNum);
     for(i=0; i<Info.TotalNum; i++)
      {
       ReadFromFlash(INFO_ADDR + 0x200 + i*0x200, Flash_Stream, 0x200);
       memcpy(&InfoContent[i], Flash_Stream, sizeof(struct InfoContent1));
       memcpy(InfoBuff[i], Flash_Stream + 80, INFOMAXSIZE);
       //xtm_printf("InfoContent[%d].Length = %d, %s\n", i, InfoContent[i].Length, InfoBuff[i]);
      }
   }
  else
   {
    ResetInfoCfg();    //复位信息配置文件
    WriteInfoCfgFile();     //写信息设置文件
   }
}
//--------------------------------------------------------------------------
//写信息文件
void WriteInfoFile(void)
{
  uint16_t i;
  for(i=0; i<Info.TotalNum; i++)
   {
    memcpy(Flash_Stream + i*0x200, &InfoContent[i], sizeof(struct InfoContent1));
    memcpy(Flash_Stream + i*0x200 + 80, InfoBuff[i], INFOMAXSIZE);    
   }
  SaveToFlash(INFO_ADDR + 0x200, Flash_Stream, Info.TotalNum * 0x200);
}
//--------------------------------------------------------------------------
void ResetInfoCfg(void)    //复位信息配置文件
{
  strcpy(Info.FlashHead, "INFO"); //Flash已存标志
  Info.MaxNum = INFOMAXITEM;   //最大信息数
  Info.TotalNum = 0; //信息总数
  Info.NoReadedNum = 0; //未读信息总数
  Info.TotalInfoPage = 0;   //总信息页数
  Info.CurrentInfoPage = 0; //当前信息页
  Info.CurrNo = 0;    //当前信息序号
  Info.CurrPlayNo = 0;  //当前播放序号
}
//--------------------------------------------------------------------------
void WriteInfoCfgFile(void)     //写信息设置文件
{
  memcpy(Flash_Stream, &Info, sizeof(struct Info1));
  SaveToFlash(INFO_ADDR, Flash_Stream, 0x200);
}
//--------------------------------------------------------------------------
void Recv_NewInfo(void)
{
  Local.NewInfo = 1;  //有新信息
  DisplayInfoStateImage(Local.NewInfo);    //显示信息图标
  //SetGpio(GPIOD_OUT_DATA, INFO_LED_LOW, SET_LOW);  //开信息 LED
}
//--------------------------------------------------------------------------

