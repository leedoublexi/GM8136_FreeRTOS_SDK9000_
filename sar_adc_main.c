#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "platform_io.h"
#include "lib_sar_adc.h"

#define SAR_ADC_POLLING_INTERVAL    10

sar_adc_cfg_t sar_adc_config;

#define KEY0	0x20
#define KEY1	0x40
#define KEY2	0x70
#define KEY3	0x90
#define KEY4	0xB0
#define KEY5	0xE0
#define KEY6	0xA0

#define SCANCODE_ENTER                                      28
#define SCANCODE_TAB                                            15
#define SCANCODE_CURSORBLOCKUP              103    /* Cursor key block */
#define SCANCODE_CURSORBLOCKDOWN        108    /* Cursor key block */
#define SCANCODE_ESCAPE                                     1

typedef struct{
	u32 value;
	u32 event;	
}keytranslate_t;

static keytranslate_t value_to_event[5] = {
		{KEY0,SCANCODE_ENTER},
		{KEY1,SCANCODE_TAB},
		{KEY2,SCANCODE_CURSORBLOCKUP},						
		{KEY3,SCANCODE_CURSORBLOCKDOWN},					
		{KEY4,SCANCODE_ESCAPE},						
};

extern xQueueHandle queue_for_ui_key;

void sar_adc_main_task(void *pvParameters)
{
    sar_adc_pub_data new_data;
    u32 gui_key;
    printf("sar_adc_main_task\n");
	
    for ( ;; ){
        if (gm_sar_adc_get_data(&new_data)){
            if(new_data.status & KEY_XAIN_0){
                u32 i;

                for(i=0;i<(sizeof(value_to_event)/sizeof(keytranslate_t));i++){
                    if(value_to_event[i].value == (new_data.adc_val & 0xF0)){
                        gui_key = value_to_event[i].event;
                        if(xQueueSendToBack(queue_for_ui_key, (void *)&gui_key, 10) != pdPASS){
                            printf("queue_for_ui_key is full!!!\n");
                        }
                    }
                }
            }
        }
        vTaskDelay(SAR_ADC_POLLING_INTERVAL);
    }
}

int sar_adc_main(void)
{
    printf("sar_adc_main\n");

    sar_adc_config.gain0_int = 0;
    sar_adc_config.run_mode = SAR_ADC_RUNING_MODE_ALL;
    sar_adc_config.check_interval_gain0 = 10;
    sar_adc_config.check_interval_gain1 = 1000;
    sar_adc_config.check_interval_gain2 = 1000;
    sar_adc_config.short_key_time = 30;
    sar_adc_config.long_key_time = 500;
    sar_adc_config.sar_adc_callback = NULL;
    gm_sar_adc_init(&sar_adc_config);

    xTaskCreate(sar_adc_main_task, "SAR_ADC_Main Task", 500, NULL, 3, NULL );

    return 0;
}
