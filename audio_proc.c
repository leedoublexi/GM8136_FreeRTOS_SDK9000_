#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <ftuart.h>
#include "lib_board.h"
#include "lib_sdc.h"
#include "lib_fs.h"
#include "lib_mm.h"
#include "lib_pmu.h"
#include "lib_dma.h"
#include "lib_ssp.h"
#include "lib_au_ssp.h"
#include "lib_adda.h"
#include "lib_i2c.h"
#include "ftiic010.h"
#include "lib_aac.h"
#include "lib_timer.h"
#include "lib_uart.h"
#include "audio_proc.h"
DMA_INT_CBFUN_t record_cb_fun = NULL;
DMA_INT_CBFUN_t play_cb_fun = NULL;
static void sspdma_record_isr(void *data)
{
	//record
	gm_dma_clear_channel_int_status(PCM_DMA_RX_CH);
	//RecordChunk_PCMPush_Finish();
	if(record_cb_fun)
	{
		record_cb_fun(data);	
	}
}

static void sspdma_play_isr(void *data)
{
	//play
	//printf("play...\n");
	gm_dma_clear_channel_int_status(PCM_DMA_TX_CH);
	//PlayChunk_PCMPop_Finish();
	if(play_cb_fun){
		play_cb_fun(data);
	}
}

void sspdma_rec_init(int SSP_idx, u32 dma_link_addr, u32 buf_addr, DMA_INT_CBFUN_t cb_fun)
{
	u32 i;
 	AHB_DMA_LLD *RX_LLP;
	record_cb_fun = cb_fun;
	gm_dma_reset_channel(PCM_DMA_RX_CH);
	
	RX_LLP = (AHB_DMA_LLD *)dma_link_addr;
	for (i=0; i<PCM_FRAME_BUF_NUM; i++){
		RX_LLP[i].src_addr = (u32)(SSP_BASE(SSP_idx)+0x18);					//SSP Transmit/Receive Data Register
    	RX_LLP[i].dst_addr = (u32)(buf_addr + i * (PCM_FRAME_SIZE));	//8 is the size of AVI chunk header
    	RX_LLP[i].llp.value = ((u32)&RX_LLP[(i+1)%PCM_FRAME_BUF_NUM]) | 1;	//8136, DDR is in master 1
    	RX_LLP[i].llp_ctrl.value = 0;
    	RX_LLP[i].llp_ctrl.bits.tc_msk = 0;
    	RX_LLP[i].llp_ctrl.bits.src_width = DMA_WIDTH_16BITS;  //1;     // source transfer size
    	RX_LLP[i].llp_ctrl.bits.dst_width = DMA_WIDTH_16BITS;  //1;     // destination transfer size
    	RX_LLP[i].llp_ctrl.bits.src_ctrl = DMA_ADDR_FIXED;     //2;     // source increment, decrement or fix
    	RX_LLP[i].llp_ctrl.bits.dst_ctrl = DMA_ADDR_INC;       //0;     // destination increment, decrement or fix
    	RX_LLP[i].llp_ctrl.bits.src_sel = 0;       // source M0
    	RX_LLP[i].llp_ctrl.bits.dst_sel = 1;       // destination M1
    	RX_LLP[i].llp_ctrl.bits.ff_th = 0;
    	RX_LLP[i].size = (PCM_FRAME_SIZE)>>RX_LLP[i].llp_ctrl.bits.src_width;
	}

	gm_dma_reset_channel(PCM_DMA_RX_CH);
	gm_dma_set_int(PCM_DMA_RX_CH, ENABLE, ENABLE, ENABLE);
	gm_dma_register_int_callback(PCM_DMA_RX_CH, sspdma_record_isr, 0);
}


void sspdma_play_init(int SSP_idx, u32 dma_link_addr, u32 buf_addr, DMA_INT_CBFUN_t cb_fun)
{
	u32 i;
	AHB_DMA_LLD *TX_LLP;
	play_cb_fun = cb_fun;
	gm_dma_reset_channel(PCM_DMA_TX_CH);
	 
	TX_LLP = (AHB_DMA_LLD *)dma_link_addr;
	for (i=0; i<PCM_FRAME_BUF_NUM; i++){
		TX_LLP[i].dst_addr = (u32)(SSP_BASE(SSP_idx)+0x18);					//SSP Transmit/Receive Data Register
		TX_LLP[i].src_addr = (u32)(buf_addr + i * (PCM_FRAME_SIZE));
		TX_LLP[i].llp.value = ((u32)&TX_LLP[(i+1)%PCM_FRAME_BUF_NUM]) | 1;	//8136, DDR is in master 1
		TX_LLP[i].llp_ctrl.value = 0;
		TX_LLP[i].llp_ctrl.bits.tc_msk = 0;
		TX_LLP[i].llp_ctrl.bits.src_width = DMA_WIDTH_16BITS;  //1;     // source transfer size
		TX_LLP[i].llp_ctrl.bits.dst_width = DMA_WIDTH_16BITS;  //1;     // destination transfer size
		TX_LLP[i].llp_ctrl.bits.src_ctrl = DMA_ADDR_INC;       //0;     // source increment, decrement or fix
		TX_LLP[i].llp_ctrl.bits.dst_ctrl = DMA_ADDR_FIXED;     //2;     // destination increment, decrement or fix
		TX_LLP[i].llp_ctrl.bits.src_sel = 1;       //DDR, M1
		TX_LLP[i].llp_ctrl.bits.dst_sel = 0;       //SSP, M0
		TX_LLP[i].llp_ctrl.bits.ff_th = 0;
		TX_LLP[i].size = (PCM_FRAME_SIZE)>>TX_LLP[i].llp_ctrl.bits.src_width;
	} 

    gm_dma_reset_channel(PCM_DMA_TX_CH);
	gm_dma_set_int(PCM_DMA_TX_CH, ENABLE, ENABLE, ENABLE);
	gm_dma_register_int_callback(PCM_DMA_TX_CH, sspdma_play_isr, 0);
}


void sspdma_rec_start(int SSP_idx, u32 dma_link_addr, u32 buf_addr)
{
	AHB_DMA_LLD *llp =  (AHB_DMA_LLD *)dma_link_addr;

	gm_ssp_set_dma_enable_rx(SSP_idx, ENABLE);		//enable RX
	gm_dma_link_mode_hw(PCM_DMA_RX_CH,   // channel
                            (u32)(llp), //u32 LinkAddr,  // Link-List address
                            (u32)(SSP_BASE(SSP_idx)+0x18),	//SSP Transmit/Receive Data Register 
                            buf_addr,      // dest begin address
                            PCM_FRAME_SIZE,       // Total bytes
                            DMA_WIDTH_16BITS,     // source width 8/16/32 bits -> 0/1/2
                            DMA_WIDTH_16BITS,     // dest width 8/16/32 bits -> 0/1/2
                            DMA_BURST_1,          // source burst size, 0 -> 1 unit
                            DMA_ADDR_FIXED,       // source address change : Inc/dec/fixed --> 0/1/2
                            DMA_ADDR_INC,         // dest address change : Inc/dec/fixed --> 0/1/2
                            DMA_PRIO_HIGHEST,     // priority for this chaanel 0(low)/1/2/3(high)
                            0,	//src sel, M0
                            1,	//dst sel, M1
                            DMA_MODE_NORMAL,      // Dest Normal/Hardwire,   0/1
                            0,                    // Dest req num
                            DMA_MODE_HW,          // Src Normal/Hardwire,   0/1
                            (DMA_REQACK_SSP0_RX+SSP_idx*2)    // Src req num
                           );
   gm_dma_enable_channel(PCM_DMA_RX_CH);

   gm_ssp_enable(SSP_idx);
}


void sspdma_play_start(u32 SSP_idx, u32 offset, u32 dma_link_addr, u32 buf_addr)
{
	AHB_DMA_LLD *llp =  (AHB_DMA_LLD *)dma_link_addr;
    
	offset&=0xfffffffc;
	gm_ssp_set_dma_enable_tx(SSP_idx, DISABLE); 

	gm_dma_link_mode_hw(PCM_DMA_TX_CH,   // channel
                            (u32)(llp),
                            buf_addr + offset,      // source begin address
                            (u32)(SSP_BASE(SSP_idx)+0x18),      // dest begin address
                            PCM_FRAME_SIZE,       // Total bytes
                            DMA_WIDTH_16BITS, //1,         // source width 8/16/32 bits -> 0/1/2
                            DMA_WIDTH_16BITS, //1,         // dest width 8/16/32 bits -> 0/1/2
                            DMA_BURST_1,      //0,     // source burst size, 0 -> 1 unit
                            DMA_ADDR_INC,     //0,     // source address change : Inc/dec/fixed --> 0/1/2
                            DMA_ADDR_FIXED,   //2,     // dest address change : Inc/dec/fixed --> 0/1/2
                            DMA_PRIO_HIGHEST, //3,     // priority for this chaanel 0(low)/1/2/3(high)
                            1,	//src sel, M1
                            0,	//dst sel, M0                            
                            DMA_MODE_HW,      //1,     // Dest Normal/Hardwire,   0/1
                            (DMA_REQACK_SSP0_TX+SSP_idx*2),        // Dest req num
                            DMA_MODE_NORMAL,  //0,     // Src Normal/Hardwire,   0/1
                            0                         // Src req num
                            );
	gm_dma_enable_channel(PCM_DMA_TX_CH);
    
	gm_ssp_set_dma_enable_tx(SSP_idx, ENABLE); 
    
	gm_ssp_enable(SSP_idx);
}


void sspdma_rec_stop(int SSP_idx)
{
    gm_dma_reset_channel(PCM_DMA_RX_CH);
    gm_ssp_set_dma_enable_rx(SSP_idx, DISABLE); 
    gm_ssp_fifo_clr_rx(SSP_idx);  
}


void sspdma_play_stop(int SSP_idx)
{
    gm_dma_reset_channel(PCM_DMA_TX_CH);
    gm_ssp_set_dma_enable_tx(SSP_idx, DISABLE); 
    gm_ssp_fifo_clr_tx(SSP_idx);
}

int sspdma_get_playfifo_len(int SSP_idx)
{
    return gm_ssp_get_txlen(SSP_idx);
}

//This function is used to transfer fix address and len.
//User should check ssp dma ready before next play.
void sspdma_play_by_fixed_addr(u32 address, u32 len)
{
	gm_ssp_set_dma_enable_tx(0, DISABLE);

    gm_dma_normal_mode_fix_dest_speed(PCM_DMA_TX_CH, address, (u32)(SSP_BASE(0)+0x18), len);
    gm_dma_enable_channel(PCM_DMA_TX_CH);
    gm_ssp_set_dma_enable_tx(0, ENABLE);
}


