//#ifndef x_disp_h
//#define x_disp_h

#define FROMFILE 0
#define FROMMEMORY 1

struct file_head1
  {
   uint32_t Faddr;    //Flash 地址
   uint32_t Filelen;            //文件大小  实际大小
   uint32_t FLen;               //Flash存储大小
   uint16_t FileType;         //文件类型 0 -- 主程序  1 -- 图片   2 -- 字库   3 -- 声音
   uint16_t Width;
   uint16_t Height;
   uint16_t xLeft;
   uint16_t yTop;
   uint16_t TotalPackage;      //总包数
   uint16_t CurrPackage;       //当前包数
   uint16_t Datalen;           //数据长度
  };
  
struct TImage{
               uint32_t Faddr;    //Flash 地址
               uint32_t Jpegaddr;    //Flash Jpeg Data地址
               uint32_t Maddr;    //内存 地址
               uint32_t FLen;     //Flash 传送长度   256的倍数
               uint32_t Len;
               uint16_t width;
               uint16_t height;
               uint16_t xLeft;
               uint16_t yTop;
              };

struct TImageButton{
               //0--up  1--down
               uint32_t Faddr[2];     //Flash 地址
               uint32_t Jpegaddr[2];  //Flash Jpeg Data地址
               uint32_t Maddr[2];     //内存 地址
               uint32_t FLen[2];      //Flash 传送长度   256的倍数
               uint32_t Len[2];
               uint16_t width;
               uint16_t height;
               uint16_t xLeft;
               uint16_t yTop;
               uint16_t Text_xLeft;
               uint16_t Text_yTop;
               char buttonname[30];
               uint8_t FontColor; 
               uint8_t FontType; //字体 16  24
               uint8_t Visible;
              };

struct TEdit{
               //0--up  1--down
               uint32_t Faddr[2];     //Flash 地址
               uint32_t Jpegaddr[2];  //Flash Jpeg Data地址
               uint32_t Maddr[2];     //内存 地址
               uint32_t FLen[2];      //Flash 传送长度  256的倍数
               uint32_t Len[2];
               uint16_t width;
               uint16_t height;
               uint16_t xLeft;
               uint16_t yTop;
               uint16_t Text_xLeft;
               uint16_t Text_yTop;               
               uint8_t FontType; //字体 16  24
               uint8_t Space;    //字距
               uint16_t CursorX; //光标X坐标
               uint16_t CursorY; //光标Y坐标
               uint16_t CursorHeight; //光标高度
               uint16_t CursorCorlor;
               uint16_t fWidth;     //字宽度
               char Text[20];
               uint8_t BoxLen;         //文本框当前输入长度
               uint8_t MaxLen;         //文本框最大输入长度
               uint8_t ValidLen;       //有效长度
               uint8_t Focus;        //显示输入光标
               uint8_t Visible;
              };
              
#ifndef x_disp_h
#define x_disp_h
extern void jpeg_dec_init(void);

extern void xtm_clear_disp_ram(uint8_t PageNo, uint16_t color);
extern void clear_area(uint16_t x,uint16_t y,uint16_t Hi,uint16_t Wi,uint16_t color_val,uint8_t PageNo);

extern void selectym(uint8_t ye);
extern void gra_dispcn16(uint8_t *ch, int x, int y,uint16_t color_val,uint8_t FullPageType);
extern void gra_dispcn24(uint8_t *ch, int x, int y,uint16_t color_val,uint8_t FullPageType);
extern void display_text(uint16_t x, uint16_t y, uint8_t wd, uint16_t clr, char *s, uint8_t pass, uint8_t fonttype, uint8_t FullPageType); //显示点阵汉字

//预处理 Image 文件
extern uint8_t PreDealImage(struct TImage *t_image);
extern uint8_t DisplayImage(struct TImage *t_image, uint8_t FromType, uint8_t FullPageType);

//预处理 ImageButton 文件
extern uint8_t PreDealImageButton(struct TImageButton *t_button, uint8_t buttontype);
extern uint8_t DisplayImageButton(struct TImageButton *t_button, uint8_t buttontype, uint8_t FromType, uint8_t FullPageType);
//显示图形按键文本
extern void DisplayImageButton_Text(struct TImageButton *t_button, uint16_t color, uint8_t wd, uint8_t FullPageType);

//预处理 Edit 文件
extern uint8_t PreDealEdit(struct TEdit *t_edit, uint8_t buttontype);
extern uint8_t DisplayEdit(struct TEdit *t_edit, uint8_t buttontype, uint8_t FromType, uint8_t FullPageType);
extern void DisplayEdit_Text(struct TEdit *t_edit, uint16_t color, uint8_t wd, uint8_t FullPageType);
extern void DisplayPassEdit_Text(struct TEdit *t_edit, uint16_t color, uint8_t wd, uint8_t FullPageType);

extern void WaitDecodeFinish(void);
extern void DecoceVideo(uint8_t *Video_Maddr, uint32_t Video_Len);

#endif
//#endif

