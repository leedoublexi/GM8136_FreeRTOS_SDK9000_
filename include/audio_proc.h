/*******************************************************************************
* Copyright  Grain-Media, Inc. 2015-2016.  All rights reserved.                *
*------------------------------------------------------------------------------*
* Name: audio_proc.h                                                           *
* Description: Header file for audio flow                                      *
*                                                                              *
*******************************************************************************/

#ifndef _AUDIO_PROC_H_
#define _AUDIO_PROC_H_
#include <types.h>
#include "platform_io.h"
#define _AUDIOECHO_SUPPORT   //回声消除支持20180222

//==============================================================================
//
//                              DEFINES
//
//==============================================================================
#define SSP_BASE(x) (SSP_FTSSP010_0_PA_BASE + 0x100000 * x)

#ifdef _AUDIOECHO_SUPPORT     //回声消除支持
  #define _8130_AUDIO_SUPPORT
  #define PCM_FRAME_SIZE    160//128
  #ifdef _8130_AUDIO_SUPPORT
    #define AUDIOBLK_8  128   //每帧8ms
  #else
    #define AUDIOBLK_8  160   //每帧10ms
  #endif
  #define PCM_FRAME_BUF_NUM               256
#else
  #define PCM_FRAME_SIZE 128 //128   //每帧8ms
  #define AUDIOBLK_8  128   //每帧8ms
  #define PCM_FRAME_BUF_NUM               320
#endif


//#define PCM_FRAME_SIZE                  128//128//(2*1024)	//2*1024 bytes => 1024 samples/per channel for AAC_Enc()
//#define PCM_FRAME_BUF_NUM               512

#define PCM_BUF_MAX    					40960//65536  				//PCM放音缓冲区最大值
#define PCM_WBATCH_NUM                  32

#define AVAILABLE_MEM_START_SSP      (0x2000000ul)
#define SSP_DMA_LINK_REC         ALIGN(AVAILABLE_MEM_START_SSP, 8)
#define SSP_PCM_OUT_BUF          ALIGN((SSP_DMA_LINK_REC + sizeof(AHB_DMA_LLD)*PCM_FRAME_BUF_NUM), 8)
#define SSP_PCM_OUT_BUF_END      ALIGN((SSP_PCM_OUT_BUF + PCM_FRAME_SIZE * PCM_FRAME_BUF_NUM), 8)

#define SSP_DMA_LINK_PLAY        ALIGN(SSP_PCM_OUT_BUF_END, 8)
#define SSP_PCM_IN_BUF	         ALIGN((SSP_DMA_LINK_PLAY + sizeof(AHB_DMA_LLD)*PCM_FRAME_BUF_NUM), 8)
#define SSP_PCM_IN_BUF_END       ALIGN((SSP_PCM_IN_BUF + PCM_FRAME_SIZE * PCM_FRAME_BUF_NUM), 8)

//==============================================================================
//
//                              TYPE DEFINES
//
//==============================================================================

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

//==============================================================================
//
//                              EXTERNAL VARIABLES REFERENCE
//
//==============================================================================

//==============================================================================
//
//                              EXTERNAL FUNCTION PROTOTYPES
//
//==============================================================================
extern void sspdma_play_init(int SSP_idx, u32 dma_link_addr, u32 buf_addr, DMA_INT_CBFUN_t cb_fun);
extern void sspdma_play_start(u32 SSP_idx, u32 offset, u32 dma_link_addr, u32 buf_addr);
extern void sspdma_play_stop(int SSP_idx);
extern void sspdma_rec_init(int SSP_idx, u32 dma_link_addr, u32 buf_addr, DMA_INT_CBFUN_t cb_fun);
extern void sspdma_rec_start(int SSP_idx, u32 dma_link_addr, u32 buf_addr);
extern void sspdma_rec_stop(int SSP_idx);
extern void sspdma_play_by_fixed_addr(u32 address, u32 len);
extern int sspdma_get_playfifo_len(int SSP_idx);


#endif /* _AUDIO_PROC_H_ */
