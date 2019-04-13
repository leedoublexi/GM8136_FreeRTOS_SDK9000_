#ifndef x_udp_h
#define x_udp_h
extern void xtm_printf(char *fmt, ...);
extern int Init_Udp(void);    //初始化UDP
extern int UDP_DataSocket;
extern int UDP_VideoSocket;

extern void CheckAudioUdpBuff(void);   //检查Audio UDP发送缓存
extern void CheckMultiUdpBuff(void);   //检查UDP发送缓存

extern void CreateMultiUdpBuff(int s_socket, uint8_t SendMode, char *RemoteIP, uint16_t DestPort, uint8_t *udpdata, uint16_t nlength, uint8_t CurrOrder);;
extern void CreateMultiUdpBuff_Audio(int s_socket, uint8_t SendMode, char *RemoteIP, uint16_t DestPort, uint8_t *udpdata, uint16_t nlength, uint8_t CurrOrder); //给音频中断调用


extern uint16_t video_decode_no;  //视频解码包号
extern uint16_t old_mjpeg_stream_no;  //视频解码包号

extern uint8_t *Memory6_Stream;//用于下载
#endif