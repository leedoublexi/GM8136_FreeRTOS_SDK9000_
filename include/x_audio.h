struct TAudio{
               uint32_t Faddr;    //Flash ��ַ
               uint32_t WavAddr;    //Flash Data��ַ
               uint32_t Len;
              };

#ifndef x_audio_h
#define x_audio_h
extern void InitPcm(void);    //20120104 xu

extern void CreateAudioSend_Stream(void);

extern void StartRecAudio(void);
extern void StopRecAudio(void);
extern void SetPlayAudioVariable(void);
extern void StartPlayAudio(void);
extern void StopPlayAudio(void);
extern void sendAudioRecvMessage(void);

extern void CheckPlayAudioBuf(void);

extern void StartPlayWav(uint32_t FlashAddr, uint8_t PlayFlag); //0 ���β���  1 ѭ������
extern void StopPlayWavFile(void);
extern void WaitAudioUnuse(uint16_t delayms); //�ȴ���������


//��ȡ wav �ļ�
extern uint8_t ReadWavFile(uint32_t FlashAddr);

extern uint8_t *PcmO_Stream;
extern uint8_t *PcmI_Stream;
extern uint8_t *Rec_Stream;

extern uint32_t PcmO_Len;
extern uint32_t PcmO_iPut;
extern uint32_t PcmO_iGet;

extern uint8_t *AudioSend_Stream;       //����Ƶ����Stream

extern void SoundSetSpeakerVolume(int volume);
extern void SoundSetMicVolume(int volume);

extern void SetOldMicAdc(uint8_t old_value);    //��lib ����
extern void SetOldSpkDac(uint8_t old_value);    //��lib ����
extern void SetOldSpkVolume(uint8_t old_value);    //��lib ����


extern uint8_t Video_Send_Flag;
extern uint8_t Audio_Send_Flag;

extern void SetMicVolume(void);
extern void StartNoiseDect(void);
extern void GetIdlePlayValue(uint8_t *PlayBuf);

extern void RestoreSpeakerDac(void);      //x_udp.c  ����
#endif

