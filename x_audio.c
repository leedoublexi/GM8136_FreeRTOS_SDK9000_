


#include "stdlib.h"




#define x_audio_h

#define CommonH
#include "common.h"

#include "lib_pmu.h"
#include "lib_dma.h"
#include "lib_ssp.h"
#include "lib_au_ssp.h"
#include "lib_adda.h"
#include "lib_i2c.h"
#include "ftiic010.h"
#include "audio_proc.h"
#include "g711core.h"
#include "abtx_init.h"




#define _REC_NO  1   //录音缓冲区 No

void Pcm_Enable(uint8_t uFlag);    //20120305 xu

void InitPcm(void);    //20120104 xu

uint8_t AudioRecIsStart = 0;
uint8_t AudioPlayIsStart = 0;
uint8_t WavRecFlag = 0;
uint8_t WavPlayFlag = 0;

#define AUDIO_CHANNEL_NUM           1
int pcm_out_buf_wptr = -1;
int pcm_in_buf_wptr = -16;
xQueueHandle   queue_for_audio_record;
xQueueHandle   queue_for_audio_playback;
int start_streaming = 0;
int start_audio_playing = 0;


uint32_t audio_timestamp;      //音视频发送Stream
uint16_t Audio_Frameno;

#define MIC_VOLUME  0x40 //0x40 //0x88       24 21 18 15 12
#define MIC_VOLUME_DEC  0x20 //0x88
void StartRecAudio(void);
void StopRecAudio(void);
void SetPlayAudioVariable(void);
void StartPlayAudio(void);
void StopPlayAudio(void);

void sendAudioRecvMessage(void);
void CreateAudioSend_Stream(void);      //音频发送Stream


uint32_t WavFileLen = 0;
uint32_t WavPlayPos = 0;
uint32_t RecWavFileLen = 0;
uint8_t RecWavPlayFlag = 0;
void StartPlayWav(uint32_t FlashAddr, uint8_t PlayFlag); //0 单次播放  1 循环播放
void StopPlayWavFile(void);
void WaitAudioUnuse(uint16_t delayms); //等待声卡空闲


//读取 wav 文件
uint8_t ReadWavFile(uint32_t FlashAddr);


uint8_t pcmi_bn;
uint8_t pcmo_bn;   //pcm 输出块位置
uint8_t SendNullFlag;
uint8_t rec_null_data[PCM_FRAME_SIZE];
void send_rec_audio_data(uint8_t *audio_buf, uint16_t length);


uint8_t  WavPlayMode;  //音频播放模式　　单次　　循环
uint32_t PcmO_Len;
uint32_t PcmO_iPut;  //加到缓冲区位置
uint32_t PcmO_iGet;  //取位置
uint8_t *PcmO_Stream;
#ifdef _AUDIOECHO_SUPPORT     //回声清除支持
  int PcmO_Echo_iput;
  int PcmO_Echo_n; // 环形缓冲区中的元素总数量
  short ref_echoin[PCM_FRAME_SIZE/2];
#endif


uint16_t PcmI_Len;
uint16_t PcmI_iPut;  //加到缓冲区位置
uint16_t PcmI_iGet;  //取位置
uint8_t *PcmI_Stream;




uint8_t *AudioSend_Stream;       //音频发送包头Stream

//int SpeakerVolume[6] = {-24, -18, -12, -6, 0, 6};//-40 ~ +6 dB
//L channel output digital Gain, -50 ~ 30 dB

int SpeakerVolume[6] = {-12, -6, 0, 6, 12, 18};//-50 ~ 30 dB

//int MicVolume[6] = {-12, -12, -6, 0, 10, 20};//-27 ~ +36 dB
int MicVolume[6] = {-21, -18, -15, -12, -9, 0};//-27 ~ +36 dB


void SoundSetSpeakerVolume(int volume);
void SoundSetMicVolume(int volume);


uint8_t OldMicAdc = 0;
void SetOldMicAdc(uint8_t old_value);    //供lib 调用
uint8_t OldSpkDac = 0;
void SetOldSpkDac(uint8_t old_value);    //供lib 调用
uint8_t OldSpkVolume = 0;
void SetOldSpkVolume(uint8_t old_value);    //供lib 调用


uint8_t Video_Send_Flag = 0;
uint8_t Audio_Send_Flag = 0;


uint16_t Audio_Count = 0;


static void audio_isr_cb(void *data);
static void audio_play_isr_cb(void *data);

uint8_t FirstPlayFlag = 1;
//void RestoreSpeakerDac(void);      //x_udp.c  调用
//---------------------------------------------------------------------------
void Pcm_Enable(uint8_t uFlag)    //20120305 xu
{
#if 0
  if (uFlag)
    outportb(CONTROL_REG, inportb(CONTROL_REG)|0x18);  //000 11 000  PCM使能
  //  outportb(CONTROL_REG, inportb(CONTROL_REG)|0x10);  //000 10 000  PCMO使能
  else
   {
    outportb(CONTROL_REG, inportb(CONTROL_REG)&0xe7);  //111 00 111  PCM禁止
 //   outportb(CONTROL_REG, inportb(CONTROL_REG)&0xef);  //111 01 111  PCMO禁止
   }

  xtm_printf("Pcm_Enable::uFlag = %d, CONTROL_REG = 0x%X\n", uFlag, inportb(CONTROL_REG));
#endif  
}
//---------------------------------------------------------------------------
void InitPcm(void)    //20120104 xu
{
	ADDA_PARAMS_T           adda_param;    
	AUDIO_DRIVER_PARAM_t    audio_param;	
	int value;
	
	printf("InitPcm::1 PimMux, pmu 0x64 = 0x%08X\n", gm_pmu_read_reg(0x64));

	//init dma 	
	gm_dma_init(FALSE, FALSE, 0xFF);  
	printf("InitPcm::2 PimMux, pmu 0x64 = 0x%08X\n", gm_pmu_read_reg(0x64));
	//init PMU, clock source, DA source select and ADDA308 registers	
	adda_param.input_mode      = 0;     
	//analog MIC	
	adda_param.ssp0_enable     = 1;     
	//AD and DA are from SSP0	
	adda_param.ssp1_enable     = 0;  	
	adda_param.is_master_mode  = 1;		
	gm_adda_init(&adda_param);	
	printf("InitPcm::3 PimMux, pmu 0x64 = 0x%08X\n", gm_pmu_read_reg(0x64));

	LocalCfg.MicVolume = 4;
    SoundSetMicVolume(LocalCfg.MicVolume);

	//L channel input digital Gain, -50 ~ 30 dB
	value = gm_adda_get_ladv();
	xtm_printf("InitPcm::gm_adda_get_ladv = %d\n", value);	

	//L channel output digital Gain, -50 ~ 30 dB
	value = gm_adda_get_ldav();
	xtm_printf("InitPcm::gm_adda_get_ldav = %d\n", value);
	//gm_adda_set_ldav(10);

    //speaker output Gain, -40 ~ +6 dB
	gm_adda_set_spv(6);
	LocalCfg.SpkVolume = 5;
	SoundSetSpeakerVolume(LocalCfg.SpkVolume);
	//init audio SSP	
	audio_param.sample_rate   = 8000;	
	audio_param.total_channel = 1;   	
	audio_param.is_master     = 0;	
	audio_param.is_stereo     = 0;	
	gm_au_ssp_init(&audio_param);

	memset(rec_null_data, 0, PCM_FRAME_SIZE);

	PcmO_Stream = (uint8_t *)AUDIO_PLAY_BUF;
	PcmI_Stream = (uint8_t *)AUDIO_RECORD_BUF;

	AudioSend_Stream = (uint8_t *)malloc(1024);       //音频发送包头Stream

    printf("SSP_DMA_LINK_REC = 0x%X, SSP_PCM_OUT_BUF = 0x%X, SSP_PCM_OUT_BUF_END = 0x%X,  sizeof(AHB_DMA_LLD) = %d\n", 
		SSP_DMA_LINK_REC, SSP_PCM_OUT_BUF, SSP_PCM_OUT_BUF_END,  sizeof(AHB_DMA_LLD));

	printf("SSP_DMA_LINK_PLAY = 0x%X, SSP_PCM_IN_BUF = 0x%X, SSP_PCM_IN_BUF_END = 0x%X\n", 
		SSP_DMA_LINK_PLAY, SSP_PCM_IN_BUF, SSP_PCM_IN_BUF_END);
	
#ifdef _AUDIOECHO_SUPPORT   //回声消除支持20180222
	InitAudioEcho(8000);
#endif

    //sspdma_rec_init(0, SSP_DMA_LINK_REC, SSP_PCM_OUT_BUF, audio_isr_cb);  	
	//sspdma_play_init(0, SSP_DMA_LINK_PLAY, SSP_PCM_IN_BUF, audio_play_isr_cb);	

}
//---------------------------------------------------------------------------
void SoundSetSpeakerVolume(int volume)
{
	//speaker output Gain, -40 ~ +6 dB  
	//gm_adda_set_spv(SpeakerVolume[volume]); 
	gm_adda_set_ldav(SpeakerVolume[volume]); 
	printf("SoundSetSpeakerVolume::%d\n", SpeakerVolume[volume]);
}
//---------------------------------------------------------------------------

void SoundSetMicVolume(int volume)
{
	//L channel input analog Gain, from -27 ~ +36 dB
	gm_adda_set_liv(MicVolume[volume]); 
	printf("SoundSetMicVolume::%d\n", MicVolume[volume]);
}
//---------------------------------------------------------------------------

//Record Code

static void audio_isr_cb(void *data)
{   
	u32 pcm_message[3];   
	if(start_streaming&&(pcm_out_buf_wptr>=0))
	{      
		pcm_message[0] = (u32)(SSP_PCM_OUT_BUF + pcm_out_buf_wptr * PCM_FRAME_SIZE);      
		pcm_message[1] = PCM_FRAME_SIZE;      
		pcm_message[2] = gm_timer_get_tick(0, 0);            
		if(xQueueSendToBackFromISR(queue_for_audio_record, (void *)&pcm_message, NULL ) != pdPASS)
		{         
			printf("queue_for_audio_record is full!!!\n");      
		}   
	}   
	pcm_out_buf_wptr = (pcm_out_buf_wptr+1)%PCM_FRAME_BUF_NUM;
}
//---------------------------------------------------------------------------

void audio_record_task(void *pvParameters)
{	
	u32 pcm_message[3];	
	unsigned int pcm_addr;
	unsigned int t1, t2;

    unsigned int pcm_len;
	queue_for_audio_record = xQueueCreate(PCM_FRAME_BUF_NUM, sizeof(u32)*3);  	      	
	sspdma_rec_init(0, SSP_DMA_LINK_REC, SSP_PCM_OUT_BUF, audio_isr_cb);  	
	pcm_out_buf_wptr = -1;
	start_streaming = 1;	
	sspdma_rec_start(0, SSP_DMA_LINK_REC, SSP_PCM_OUT_BUF);		
	printf("audio_record_task::init...1\n");
	for ( ;; ){		
		if(xQueueReceive( queue_for_audio_record, &pcm_message, 1000 ) != pdPASS)
		{			
			printf("fail record pcm data\n");			
			continue;		
		}
	    if(AudioRecIsStart == 0)		
		{					
			break;	
		}		
		//t1 = gm_timer_get_tick(0, 0);
		//printf("%d, %d\n", t1 - t2, pcm_len);
		//t2 = t1;
		pcm_addr = pcm_message[0];
     		
		pcm_len = pcm_message[1];	

	  #ifdef _AUDIOECHO_SUPPORT 	//回声清除支持
					 short echo_out[PCM_FRAME_SIZE];
					 AudioEchoProcess(ref_echoin, (short *)pcm_addr, echo_out, PCM_FRAME_SIZE/2, 50);
					 memcpy(PcmI_Stream + PcmI_iPut, echo_out, PCM_FRAME_SIZE);
 	  #else
					 memcpy(PcmI_Stream + PcmI_iPut, pcm_addr, PCM_FRAME_SIZE);	
	  #endif		

		PcmI_iPut += PCM_FRAME_SIZE;
    	if(PcmI_iPut >= PCM_BUF_MAX)
      		PcmI_iPut = 0;

	    if(PcmI_iPut >= PcmI_iGet)
	      PcmI_Len = (PcmI_iPut - PcmI_iGet);
	    else
	      PcmI_Len = (PcmI_iPut + PCM_BUF_MAX - PcmI_iGet);
	
		send_rec_audio_data((uint8_t *)pcm_addr, pcm_len);
	    if(AudioRecIsStart == 0)		
		{					
			break;	
		}	
	}
	start_streaming = 0;
	printf("audio_record_task::exit...\n");
	sspdma_rec_stop(0);
	vQueueDelete(queue_for_audio_record);
	vTaskDelete(NULL);
}

void StartRecAudio(void)
{
#if 1
	PcmI_Stream = (uint8_t *)AUDIO_RECORD_BUF;
	PcmI_Len = 0;
	PcmI_iPut = 0;  //加到缓冲区位置
	PcmI_iGet = 0;  //取位置
	
	AudioRecIsStart = 1;

	xTaskCreate(audio_record_task, "RECORD", 4096, NULL, AUDIORECORD_PRIORITY, NULL );  
#endif	
}
//---------------------------------------------------------------------------
void StopRecAudio(void)
{
	u32 pcm_message[3]; 


	AudioRecIsStart = 0;
	printf("StopRecAudio::1\n");

	if(start_streaming)
			if(xQueueSendToBack(queue_for_audio_record, (void *)&pcm_message, NULL ) != pdPASS)	
			{			
				printf("queue_for_audio_record is full 2, %d!!!\n", start_streaming);		
			}
}
//---------------------------------------------------------------------------
void SetOldMicAdc(uint8_t old_value)    //供lib 调用
{
  OldMicAdc = old_value;
}
//---------------------------------------------------------------------------
void SetOldSpkDac(uint8_t old_value)    //供lib 调用
{
  OldSpkDac = old_value;
}
//---------------------------------------------------------------------------
void SetOldSpkVolume(uint8_t old_value)    //供lib 调用
{
  OldSpkVolume = old_value;
}
//---------------------------------------------------------------------------
// Playback code
static void audio_play_isr_cb(void *data)
{	
	u32 pcm_message;
//	if(WavPlayFlag)
		if(start_audio_playing)
		{		    		
		    //printf("PcmO_Len = %d, PcmO_iPut = %d, PcmO_iGet = %d\n", PcmO_Len, PcmO_iPut, PcmO_iGet); 
			if(xQueueSendToBackFromISR(queue_for_audio_playback, &pcm_message, NULL ) != pdPASS)
			{			
				printf("queue_for_audio_playback is full 1, %d!!!\n", start_audio_playing);		
			}	
		}	
}
//---------------------------------------------------------------------------
//xQueueSendToBackFromISR
void sendAudioRecvMessage(void)
{
	u32 pcm_message;
	if(start_audio_playing)
		//if(PcmO_Len == PCM_FRAME_SIZE)
			if(xQueueSendToBack(queue_for_audio_playback, &pcm_message, NULL ) != pdPASS)
			{			
				printf("queue_for_audio_playback is full 2, %d!!!\n", start_audio_playing);		
			}	

}

void audio_playback_task(void *pvParameters)
{		
	int len;	
	u32 pcm_message;
	u32 i,refill_addr;	
	start_audio_playing = 1;
	printf("audio_playback_task::init..., WavPlayFlag = %d, AudioPlayIsStart = %d\n", WavPlayFlag, AudioPlayIsStart);
	queue_for_audio_playback = xQueueCreate(160, sizeof(u32));  		
	sspdma_play_init(0, SSP_DMA_LINK_PLAY, SSP_PCM_IN_BUF, audio_play_isr_cb);	
	gm_adda_set_speaker_on(1);	

	//opEN();

	refill_addr = (u32)(SSP_PCM_IN_BUF);	
	memset(refill_addr, 0, PCM_BUF_MAX);
	if(!WavPlayFlag){
		while(AudioPlayIsStart){
			if(PcmO_Len > (PCM_FRAME_SIZE *4)){
				PcmO_iGet =  4*PCM_FRAME_SIZE;
				memcpy(refill_addr, PcmO_Stream, PcmO_iGet);
				PcmO_Len = PcmO_Len - PcmO_iGet;
				printf("audio_playback_task::init...2\n");
				break;
			}
			vTaskDelay(2);
		}
	}
				
	
	pcm_in_buf_wptr = 0;
	sspdma_play_start(0, 0, SSP_DMA_LINK_PLAY, SSP_PCM_IN_BUF);	
	//printf("audio_playback_task::init...3\n");
	for ( ;; ){		
		if(AudioPlayIsStart == 0)		
		{	
		    //printf("audio_playback_task::AudioPlayIsStart 1 = %d\n", AudioPlayIsStart);
			break;	
		}		
		if(xQueueReceive( queue_for_audio_playback, &pcm_message, 1000 ) != pdPASS)
		{			
			printf("fail play pcm data\n");	
			if(FirstPlayFlag)
				Local.GpioWatchDogStarted = 0;  //在GPIO检测线程中启动WatchDog
			continue;		
		}		
		//printf("f = %d\n", sspdma_get_playfifo_len(0));
		FirstPlayFlag = 0;
		if(AudioPlayIsStart == 0)		
		{				
			//printf("audio_playback_task::AudioPlayIsStart 2 = %d\n", AudioPlayIsStart);
			break;	
		}

		refill_addr = (u32)(SSP_PCM_IN_BUF + PcmO_iGet);
		if(WavPlayFlag)
		{			
			len = WavFileLen - (WavPlayPos);// + PCM_FRAME_SIZE);
			if(len > PCM_FRAME_SIZE){
				len = PCM_FRAME_SIZE;
				memcpy(refill_addr, PcmO_Stream + WavPlayPos, len);
				//printf("WavPlayPos 1 = %d, WavFileLen = %d, PcmO_iGet = %d, len = %d\n", WavPlayPos, WavFileLen, PcmO_iGet, len);	
			}
			else
			{
			    memcpy(refill_addr, PcmO_Stream + WavPlayPos, len);
				//printf("WavPlayPos 2 = %d, WavFileLen = %d, PcmO_iGet = %d, len = %d\n", WavPlayPos, WavFileLen, PcmO_iGet, len);	
				if(WavPlayMode == NO_CYCLE_PLAY)  //音频播放模式　　单次　　循环
				{
					StopPlayWavFile();
					break;
				}
				else
				{
					WavPlayPos = 0;
				}

			}
			
			//printf("0x%X\n", refill_addr);
				
			WavPlayPos += len;
			PcmO_iGet += len;
			if(PcmO_iGet >= (PCM_FRAME_SIZE *PCM_FRAME_BUF_NUM))
				PcmO_iGet = 0;	
    	}
		else
		{
		    if(Local.SendLockFlag)    //20180608 通话过程中开锁
		    {
		      SetPlayAudioVariable();	
		      Local.SendLockFlag = 0;
		      sspdma_play_start(0, 0, SSP_DMA_LINK_PLAY, SSP_PCM_IN_BUF);	
		    }
		    //printf("PcmO_iGet = %d\n", PcmO_iGet);
		    if(PcmO_Len >= PCM_FRAME_SIZE){
    			memcpy(refill_addr, PcmO_Stream + PcmO_iGet, PCM_FRAME_SIZE);
				//sspdma_play_by_fixed_addr(refill_addr, PCM_FRAME_SIZE);

#ifdef _AUDIOECHO_SUPPORT     //回声清除支持
			  	if (PcmO_Echo_n < PCM_BUF_MAX)
					  PcmO_Echo_n += PCM_FRAME_SIZE;
				if (PcmO_Echo_n >= PCM_FRAME_SIZE)
				  {
					  memcpy(ref_echoin, PcmO_Stream +PcmO_Echo_iput, PCM_FRAME_SIZE);
					  AudioEchoSetRef(ref_echoin, PCM_FRAME_SIZE / 2);
					  if ((PcmO_Echo_iput + PCM_FRAME_SIZE) >= PCM_BUF_MAX)
						  PcmO_Echo_iput = 0;
					  else
						  PcmO_Echo_iput += PCM_FRAME_SIZE;
					  PcmO_Echo_n -= PCM_FRAME_SIZE;
				  }
#endif

				//printf("0x%X\n", refill_addr);
    			PcmO_iGet += PCM_FRAME_SIZE;
    			if(PcmO_iGet >= PCM_BUF_MAX)
      				PcmO_iGet = 0;		
				if(PcmO_Len > 0)
          			PcmO_Len -= PCM_FRAME_SIZE;			
		    }
			//else
			//	printf("PcmO_Len = %d\n", PcmO_Len);
		}
  	
	}
	printf("audio_playback_task::exit...\n");
	sspdma_play_stop(0);
	vQueueDelete(queue_for_audio_playback);	
	//opDS();


	start_audio_playing = 0;
	SetPlayAudioVariable();

	vTaskDelete(NULL);
}
//---------------------------------------------------------------------------
void SetPlayAudioVariable(void)
{
	PcmO_Stream = (uint8_t *)AUDIO_PLAY_BUF;
	PcmO_Len = 0;
	PcmO_iPut = 0;  //加到缓冲区位置
	PcmO_iGet = 0;  //取位置
	
#ifdef _AUDIOECHO_SUPPORT     //回声消除支持
	PcmO_Echo_iput = 0;
    PcmO_Echo_n = 0; // 环形缓冲区中的元素总数量
#endif    

}

void StartPlayAudio(void)
{
#if 1
	WaitAudioUnuse(1000);

	AudioPlayIsStart = 1;

	xTaskCreate(audio_playback_task, "Audio Playback", 4096, NULL, AUDIOPLAY_PRIORITY, NULL );	
#endif	
}
//---------------------------------------------------------------------------
void StopPlayAudio(void)
{
	u32 pcm_message;
	AudioPlayIsStart = 0;
	printf("StopPlayAudio::start_audio_playing = %d\n", start_audio_playing);
	if(start_audio_playing)
		if(xQueueSendToBack(queue_for_audio_playback, &pcm_message, NULL ) != pdPASS)
		{			
			printf("queue_for_audio_playback is full!!!\n");		
		}	
}
//---------------------------------------------------------------------------
void StartPlayWav(uint32_t FlashAddr, uint8_t PlayFlag) //0 单次播放  1 循环播放
{
#if 1
  uint16_t i, len;
  uint32_t refill_addr;
  if(AudioPlayIsStart == 0)
   {
    WavPlayFlag = 1;
    AudioPlayIsStart = 1;

    WavPlayMode = PlayFlag;
    ReadWavFile(FlashAddr);	

	SetPlayAudioVariable();	
	
	//prefill data to buffer	
	refill_addr = (u32)(SSP_PCM_IN_BUF);	
	if(WavFileLen > 4*PCM_FRAME_SIZE)
		PcmO_iGet =  4*PCM_FRAME_SIZE;
	else 
		PcmO_iGet =  WavFileLen;
	memcpy(refill_addr, PcmO_Stream, PcmO_iGet);
	WavPlayPos = PcmO_iGet;

    PcmO_Len = WavFileLen;
    PcmO_iPut = PcmO_Len;	
	xtm_printf("StartPlayWav::1 AudioPlayIsStart = %d, WavFileLen = %d\n", AudioPlayIsStart, WavFileLen);
    StartPlayAudio();


    xtm_printf("StartPlayWav::AudioPlayIsStart = %d, WavFileLen = %d\n", AudioPlayIsStart, WavFileLen);
   }
  else
    xtm_printf("StartPlayWav::AudioPlayIsStart = %d\n", AudioPlayIsStart); 
#endif  
}
//---------------------------------------------------------------------------
void StopPlayWavFile(void)
{

  AudioPlayIsStart = 0;
  WavPlayFlag = 0;
  RecWavPlayFlag = 0;
  StopPlayAudio();  
  xtm_printf("StopPlayWavFile::AudioPlayIsStart = %d, start_audio_playing = %d\n", AudioPlayIsStart, start_audio_playing);
}
//---------------------------------------------------------------------------
void WaitAudioUnuse(uint16_t delayms) //等待声卡空闲
{
  uint16_t  tmp;
  tmp = 0;
  while(start_audio_playing == 1)
   {
	vTaskDelay(1);
    tmp += 1;
    if(tmp > delayms)
      break;
   }
}
//---------------------------------------------------------------------------
void CreateAudioSend_Stream(void)      //音频发送Stream
{
#if 1
    struct talkdata1 talkdata;
    //头部
    memcpy(AudioSend_Stream, UdpPackageHead, 6);
    //命令
    AudioSend_Stream[6] = VIDEOTALK;

    AudioSend_Stream[7]= ASK;        //主叫
    if((Local.Status == 1)||(Local.Status == 3)||(Local.Status == 5))  //本机为主叫方
     {
      AudioSend_Stream[8] = CALLUP;
      memcpy(talkdata.HostAddr, LocalCfg.Addr, 20);
      memcpy(talkdata.HostIP, LocalCfg.IP, 4);
      memcpy(talkdata.AssiAddr, Remote.Addr[0], 20);
      memcpy(talkdata.AssiIP, Remote.IP[0], 4);
     }
    if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6))  //本机为被叫方
     {
      AudioSend_Stream[8] = CALLDOWN;
      memcpy(talkdata.HostAddr, Remote.Addr[0], 20);
      memcpy(talkdata.HostIP, Remote.IP[0], 4);
      memcpy(talkdata.AssiAddr, LocalCfg.Addr, 20);
      memcpy(talkdata.AssiIP, LocalCfg.IP, 4);
     }

    //时间戳
    talkdata.timestamp = 0;
      //数据类型
    talkdata.DataType = 1;
    //帧序号
    talkdata.Frameno = 0;
    //帧数据长度
    talkdata.Framelen = PCM_FRAME_SIZE/2;
    //总包数
    talkdata.TotalPackage = 1;
    //当前包
    talkdata.CurrPackage = 1;
    //数据长度
    talkdata.Datalen = PCM_FRAME_SIZE/2;
    talkdata.PackLen = PACKDATALEN;
    memcpy(AudioSend_Stream + 9, &talkdata, sizeof(talkdata));

    xtm_printf("CreateAudioSend_Stream::Local.Status = %d, Remote.IP[0] = %d.%d.%d.%d, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, AudioSend_Stream[8] = 0x%X\n", 
           Local.Status, Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3], 
           AudioSend_Stream[0], AudioSend_Stream[1], AudioSend_Stream[2], AudioSend_Stream[3], AudioSend_Stream[4], AudioSend_Stream[5], AudioSend_Stream[6], AudioSend_Stream[7], AudioSend_Stream[8]);
#endif	
}
//---------------------------------------------------------------------------

uint8_t SendAudioNum = 0;
void send_rec_audio_data(uint8_t *audio_buf, uint16_t length)
{

  uint8_t encode_buf[AUDIOBLK_8/2];
  uint16_t sendlength;
  char RemoteIP[20];


    length = AUDIOBLK_8/2;
	while(PcmI_Len >= AUDIOBLK_8){

	  //时间戳
	  memcpy(AudioSend_Stream + 57, &audio_timestamp, sizeof(uint32_t));
	  //数据类型
	  AudioSend_Stream[61] = 1;
	  AudioSend_Stream[62] = 0;
	  //帧序号
	  memcpy(AudioSend_Stream + 63, &Audio_Frameno, sizeof(uint16_t));
	  //帧数据长度
	  AudioSend_Stream[65] = AUDIOBLK_8/2;
	  AudioSend_Stream[66] = 0;
	  AudioSend_Stream[67] = 0;
	  AudioSend_Stream[68] = 0;
	  //总包数
	  AudioSend_Stream[69] = 1;
	  AudioSend_Stream[70] = 0;
	  //当前包
	  AudioSend_Stream[71] = 1;
	  AudioSend_Stream[72] = 0;
	  //数据长度
	  AudioSend_Stream[73] = AUDIOBLK_8/2;
	  AudioSend_Stream[74] = 0;
#ifdef _HALF_DUPLEX    //半双工
	  AudioSend_Stream[77] = Idle_Record_Value & 0xff;
	  AudioSend_Stream[78] = (Idle_Record_Value >> 8) & 0xff;
	
	  AudioSend_Stream[79] = Send_Record_Value & 0xff;
	  AudioSend_Stream[80] = (Send_Record_Value >> 8) & 0xff;
#endif		
	
	  //数据
	  if(SendNullFlag == 0){
		  G711Encoder(PcmI_Stream + PcmI_iGet, encode_buf, AUDIOBLK_8/2, 0);
		  memcpy(AudioSend_Stream + 86, encode_buf, length);
	  }  
	  else
		memcpy(AudioSend_Stream + 86, rec_null_data, length);

	  PcmI_iGet += AUDIOBLK_8;
	  if(PcmI_iGet >= PCM_BUF_MAX)
		PcmI_iGet = 0;
	
	  if(PcmI_iPut >= PcmI_iGet)
	    PcmI_Len = (PcmI_iPut - PcmI_iGet);
	  else
	    PcmI_Len = (PcmI_iPut + PCM_BUF_MAX - PcmI_iGet);
	
	  sendlength = 86 + length;
	
 #if 1
	  sprintf(RemoteIP, "%d.%d.%d.%d", Remote.IP[0][0], Remote.IP[0][1], Remote.IP[0][2], Remote.IP[0][3]);
	  CreateMultiUdpBuff(UDP_VideoSocket, SINGLE_SEND, RemoteIP, LocalPort[1], AudioSend_Stream, sendlength, 0);
	  //UdpSendBuff(UDP_VideoSocket, RemoteIP, LocalPort[1], AudioSend_Stream, sendlength);
 #else
	  Audio_Send_Flag = 1;
	  UdpSendBuff_1(LocalPort[1], Remote.DenMac, Remote.DenIP, AudioSend_Stream, sendlength);
	  Audio_Send_Flag = 0;
	  SendAudioNum ++;
 #endif
	  //xtm_printf("send_rec_audio_data::Remote.DenIP = %d.%d.%d.%d, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, AudioSend_Stream[8] = 0x%X\n", 
	  //	   Remote.DenIP[0], Remote.DenIP[1], Remote.DenIP[2], Remote.DenIP[3], \
	  //	   AudioSend_Stream[0], AudioSend_Stream[1], AudioSend_Stream[2], AudioSend_Stream[3], AudioSend_Stream[4], AudioSend_Stream[5], AudioSend_Stream[6], AudioSend_Stream[7], AudioSend_Stream[8]);
	
	  audio_timestamp += 8; 	 //音视频发送Stream
	  Audio_Frameno ++;
	
	  if(Local.Status == 0)
	   if(WavRecFlag == 0)
		 xtm_printf("send_rec_audio_data but Local.Status = %d\n", Local.Status);

	}
 
}
//---------------------------------------------------------------------------
//读取 wav 文件
uint8_t ReadWavFile(uint32_t FlashAddr)
{
  uint8_t read_spi[512];
  struct file_head1 file_head;
  uint32_t WavAddr;

  WavAddr = FlashAddr;

  gm_spi_flash_data_read(WavAddr, sizeof(struct file_head1), read_spi);
  memcpy(&file_head, read_spi, sizeof(struct file_head1));

  WavFileLen = file_head.Filelen;

  WavAddr += 256;
  gm_spi_flash_data_read(WavAddr, WavFileLen, AUDIO_READ_BUF);
  G711Decoder(PcmO_Stream, AUDIO_READ_BUF, WavFileLen, 0);
  WavFileLen = WavFileLen *2 ;
  WavPlayPos = 0;
  
  xtm_printf("WavAddr = 0x%X, WavFileLen = %d\n", WavAddr - 256, WavFileLen);

}
//---------------------------------------------------------------------------
void RestoreSpeakerDac(void)      //x_udp.c  调用
{
  //SoundSetSpeakerDac(0);//DB_HIGH);    //  SPK_DB_MIDDLE
}
//---------------------------------------------------------------------------
