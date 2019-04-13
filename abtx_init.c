


#define CommonH
#include "common.h"

#include"abtx_init.h"



#define xDelay100ms  (100/portTICK_RATE_MS)
#define xDelay10ms   (10/portTICK_RATE_MS)
#define xDelay1ms    (1/portTICK_RATE_MS)
void testio(void);

/*初始化外部IO引脚*/
void ace_init(void)
{

	gm_gpio_direction_input(IO_S1);
	gm_gpio_direction_input(IO_S2);
	gm_gpio_direction_input(IO_S3);
	gm_gpio_direction_input(IO_S4);

	gm_gpio_direction_output(ACE_RST,1);
	gm_gpio_direction_output(ACR_POWER,1);
	gm_gpio_direction_output(PWM0IO,1);
	gm_gpio_set_value(ACR_POWER, SET_HIGH); //开启消回声电源
	gm_gpio_set_value(ACE_RST, SET_HIGH); //
	vTaskDelay(xDelay100ms );
	gm_gpio_set_value(ACE_RST, SET_LOW); //
	vTaskDelay(xDelay100ms );
	gm_gpio_set_value(ACE_RST, SET_HIGH); //
	vTaskDelay(xDelay100ms );
	gm_gpio_set_value(ACE_RST, SET_LOW); //
	gm_gpio_direction_output(PD_SPK,1);
	gm_gpio_set_value(PWM0IO, SET_HIGH); //
    opDS();
	//xTaskCreate(testio, "testio", 1024, NULL, NORMAL_PRIORITY, NULL ); //建立按键检测任务
	
}
uint8_t keyboard_scan(void)
{    uint8_t value =0;
	 uint8_t static  gpio_key[4]={0,0,0,0};
	 if(gm_gpio_get_value(IO_S1)==0){
		vTaskDelay(20/portTICK_RATE_MS);
		if((gm_gpio_get_value(IO_S1)==0)&&(gpio_key[0]==0)){
			 gpio_key[0]=1;
		    value = 1;
		 }	 
	 }
	 else{
		 gpio_key[0]=0;
	 }
	if(gm_gpio_get_value(IO_S2)==0){		 
		vTaskDelay(20/portTICK_RATE_MS);
		if((gm_gpio_get_value(IO_S2)==0)&&(gpio_key[1]==0)){
		 gpio_key[1]=1;
		  value = 3;
		 
		}	 
	 }
	 else{
		 gpio_key[1]=0;
	 }
	 
	 if(gm_gpio_get_value(IO_S3)==0){			 
		vTaskDelay(20/portTICK_RATE_MS);
		if((gm_gpio_get_value(IO_S3)==0)&&(gpio_key[2]==0)){
			 gpio_key[2]=1;
			//xtm_printf("get_key_value::IO_S3\n");
			 value = 2;
		 }	 
	 }
	 else{
		 gpio_key[2]=0;
	 }
	 if(gm_gpio_get_value(IO_S4)==0){			 
		vTaskDelay(20/portTICK_RATE_MS);
		if((gm_gpio_get_value(IO_S4)==0)&&(gpio_key[3]==0)){
			 //get_key_value(5);
			 gpio_key[3]=1;
			// xtm_printf("get_key_value::IO_S4\n");
			 value = 6;
		}	 
	 }
	 else{
		 gpio_key[3]=0;
	 }
  return value;
}



