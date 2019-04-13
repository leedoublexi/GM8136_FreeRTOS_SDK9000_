#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "lib_board.h"

#include "common.h"

#ifdef _XTM8900_SUPPORT  //XTM8900网卡支持
	#include "lib_xtm8900.h"
#endif
#ifdef _XTM9000_SUPPORT  //XTM9000网卡支持
	#include "lib_xtm9000.h"
#endif



#if ( configUSE_IDLE_HOOK == 1 )
/* Declare a variable that will be incremented by the hook function. */
volatile unsigned long ulIdleCycleCount = 0UL;
volatile unsigned long ulShowIdleCount = 0UL;
#endif

//网卡回调函数
void NetStatusFunc(int type) 
{
    printf("NetStatusFunc::type = %d\n", type);
	switch(type){
		case NET_PROBE_OK://NET_PROBE_OK:
		     //net_refresh_addr(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);
		     #ifdef _XTM8900_SUPPORT  //XTM8900网卡支持
    			net_get_mac(LocalCfg.Mac_Addr);
		  	 #endif
		  	 #ifdef _XTM9000_SUPPORT  //XTM9000网卡支持
				net_get_mac_9000(LocalCfg.Mac_Addr);
		  	 #endif
		     
			 printf("LocalCfg.Mac_Addr = %02X:%02X:%02X:%02X:%02X:%02X\n", LocalCfg.Mac_Addr[0], LocalCfg.Mac_Addr[1], LocalCfg.Mac_Addr[2],
			 	LocalCfg.Mac_Addr[3], LocalCfg.Mac_Addr[4], LocalCfg.Mac_Addr[5]);
			 Init_Udp();
			 
			break;
		case NET_LINKUP://NET_PROBE_OK:
		    xtm_printf("Link up\n");;
			Local.NetLanStatus = 1;   //网络状态 0 断开  1 接通
			break;
		case NET_LINKDOWN://NET_PROBE_OK:
		    xtm_printf("Link down\n");
			Local.NetLanStatus = 0;   //网络状态 0 断开  1 接通
			break;	
		case NET_USBERROR://NET_PROBE_OK:
		     xtm_printf("NET_USBERROR\n");
			break;
		case NET_DISCONNECT://NET_PROBE_OK:
			UnInit_Udp();
		    xtm_printf("NET_DISCONNECT\n");
			break;			
	}
}



void main_task(void *pvParameters)
{
  int i;
   
	printf("main_task::1\n");
 	
	InitPcm();	

  	disp_init();
	//Decode
	jpeg_dec_init();

	InitGpio();  //初始化GPIO	
	ClearWatchDog();

  	Init_LocalVar(); //初始化 Local 结构

  	printf("main::1, NetStatusFunc = 0x%X, IP = %d.%d.%d.%d\n", NetStatusFunc, LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);

	#ifdef _XTM8900_SUPPORT  //XTM8900网卡支持
    net_set_addr(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);
	net_init(NetStatusFunc);	
	#endif
	#ifdef _XTM9000_SUPPORT  //XTM9000网卡支持
		net_set_addr_9000(LocalCfg.IP, LocalCfg.IP_Mask, LocalCfg.IP_Gate, 0);
		net_init_9000(NetStatusFunc);
	#endif
	
	InitDefence();   //初始化防区
  	Init_InfoVar(); //初始化 Info 结构
  	Init_RecordsVar(); //初始化 Records 结构
	InterfaceInit(); //初始化界面资源
	
	DisplayFirstWindow();	 //显示主窗口
	Lcd_Power(1, LocalCfg.BrightVolume);
	timer_init();
	InitComm();
	
	//ace_init();
    StartPlayWav(soundtip_wav.Faddr, NO_CYCLE_PLAY);//soundtip_wav  null_wav

  for ( ;; )
  {
    #if ( configUSE_IDLE_HOOK == 1 )      
    	//printf("idle=%ld\n", ulIdleCycleCount - ulShowIdleCount);
    	ulIdleCycleCount++;//avoid no counting
    	ulShowIdleCount = ulIdleCycleCount;
    #endif   
    
    vTaskDelay(3000/portTICK_RATE_MS);
  }
}


int main(void)
{
   BOARD_PARAMETER_T board_params;
   board_params.uart_tx_int_mode = 0;
   board_params.uart_rx_int_mode = 1;
   board_params.noncache_heap_size = 0x00A00000;   //8M bytes
   gm_board_init(&board_params);




   xTaskCreate(main_task, "Main Task", 3072, NULL, 3, NULL );   //MAIN_PRIORITY
    
   vTaskStartScheduler();
    
   return 0;
}


void vApplicationIdleHook( void )
{
   /* This hook function does nothing but increment a counter. */
#if ( configUSE_IDLE_HOOK == 1 )
	ulIdleCycleCount++;
#endif
}


void vApplicationStackOverflowHook( xTaskHandle xTask, char *pcTaskName )
{       
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
   function is called if a stack overflow is detected. */
   printf("%s stack overflow\n",pcTaskName);
   taskDISABLE_INTERRUPTS();
   for( ;; );     
}
