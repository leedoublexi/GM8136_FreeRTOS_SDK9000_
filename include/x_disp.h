//#ifndef x_disp_h
//#define x_disp_h

#define FROMFILE 0
#define FROMMEMORY 1

struct file_head1
  {
   uint32_t Faddr;    //Flash ��ַ
   uint32_t Filelen;            //�ļ���С  ʵ�ʴ�С
   uint32_t FLen;               //Flash�洢��С
   uint16_t FileType;         //�ļ����� 0 -- ������  1 -- ͼƬ   2 -- �ֿ�   3 -- ����
   uint16_t Width;
   uint16_t Height;
   uint16_t xLeft;
   uint16_t yTop;
   uint16_t TotalPackage;      //�ܰ���
   uint16_t CurrPackage;       //��ǰ����
   uint16_t Datalen;           //���ݳ���
  };
  
struct TImage{
               uint32_t Faddr;    //Flash ��ַ
               uint32_t Jpegaddr;    //Flash Jpeg Data��ַ
               uint32_t Maddr;    //�ڴ� ��ַ
               uint32_t FLen;     //Flash ���ͳ���   256�ı���
               uint32_t Len;
               uint16_t width;
               uint16_t height;
               uint16_t xLeft;
               uint16_t yTop;
              };

struct TImageButton{
               //0--up  1--down
               uint32_t Faddr[2];     //Flash ��ַ
               uint32_t Jpegaddr[2];  //Flash Jpeg Data��ַ
               uint32_t Maddr[2];     //�ڴ� ��ַ
               uint32_t FLen[2];      //Flash ���ͳ���   256�ı���
               uint32_t Len[2];
               uint16_t width;
               uint16_t height;
               uint16_t xLeft;
               uint16_t yTop;
               uint16_t Text_xLeft;
               uint16_t Text_yTop;
               char buttonname[30];
               uint8_t FontColor; 
               uint8_t FontType; //���� 16  24
               uint8_t Visible;
              };

struct TEdit{
               //0--up  1--down
               uint32_t Faddr[2];     //Flash ��ַ
               uint32_t Jpegaddr[2];  //Flash Jpeg Data��ַ
               uint32_t Maddr[2];     //�ڴ� ��ַ
               uint32_t FLen[2];      //Flash ���ͳ���  256�ı���
               uint32_t Len[2];
               uint16_t width;
               uint16_t height;
               uint16_t xLeft;
               uint16_t yTop;
               uint16_t Text_xLeft;
               uint16_t Text_yTop;               
               uint8_t FontType; //���� 16  24
               uint8_t Space;    //�־�
               uint16_t CursorX; //���X����
               uint16_t CursorY; //���Y����
               uint16_t CursorHeight; //���߶�
               uint16_t CursorCorlor;
               uint16_t fWidth;     //�ֿ��
               char Text[20];
               uint8_t BoxLen;         //�ı���ǰ���볤��
               uint8_t MaxLen;         //�ı���������볤��
               uint8_t ValidLen;       //��Ч����
               uint8_t Focus;        //��ʾ������
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
extern void display_text(uint16_t x, uint16_t y, uint8_t wd, uint16_t clr, char *s, uint8_t pass, uint8_t fonttype, uint8_t FullPageType); //��ʾ������

//Ԥ���� Image �ļ�
extern uint8_t PreDealImage(struct TImage *t_image);
extern uint8_t DisplayImage(struct TImage *t_image, uint8_t FromType, uint8_t FullPageType);

//Ԥ���� ImageButton �ļ�
extern uint8_t PreDealImageButton(struct TImageButton *t_button, uint8_t buttontype);
extern uint8_t DisplayImageButton(struct TImageButton *t_button, uint8_t buttontype, uint8_t FromType, uint8_t FullPageType);
//��ʾͼ�ΰ����ı�
extern void DisplayImageButton_Text(struct TImageButton *t_button, uint16_t color, uint8_t wd, uint8_t FullPageType);

//Ԥ���� Edit �ļ�
extern uint8_t PreDealEdit(struct TEdit *t_edit, uint8_t buttontype);
extern uint8_t DisplayEdit(struct TEdit *t_edit, uint8_t buttontype, uint8_t FromType, uint8_t FullPageType);
extern void DisplayEdit_Text(struct TEdit *t_edit, uint16_t color, uint8_t wd, uint8_t FullPageType);
extern void DisplayPassEdit_Text(struct TEdit *t_edit, uint16_t color, uint8_t wd, uint8_t FullPageType);

extern void WaitDecodeFinish(void);
extern void DecoceVideo(uint8_t *Video_Maddr, uint32_t Video_Len);

#endif
//#endif

