

#include "stdlib.h"

#define x_video_h

#define CommonH
#include "common.h"


//显示视频窗口　cType -- 0  本地   1 -- 远程
void DisplayVideoWindow(uint16_t xLeft,uint16_t width,uint16_t yTop,uint16_t height, uint8_t cType);
void CloseVideoWindow(void);
//---------------------------------------------------------------------------
//显示视频窗口　cType -- 0  本地   1 -- 远程
void DisplayVideoWindow(uint16_t xLeft,uint16_t width,uint16_t yTop,uint16_t height, uint8_t cType)
{
	gm_lcd_set_pip_position(1, xLeft, yTop);
	gm_lcd_set_pip_size(1, width, height);
	gm_lcd_set_pip_mode(1, 0);
}
//---------------------------------------------------------------------------
void CloseVideoWindow(void)
{
	gm_lcd_set_pip_mode(0, 0);

    Local.ClearVideoWindowFlag = 1;    //清视频窗口标志
}
//---------------------------------------------------------------------------

