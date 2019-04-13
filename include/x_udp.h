#ifndef x_udp_h
#define x_udp_h
extern void xtm_printf(char *fmt, ...);
extern int Init_Udp(void);    //��ʼ��UDP
extern int UDP_DataSocket;
extern int UDP_VideoSocket;

extern void CheckAudioUdpBuff(void);   //���Audio UDP���ͻ���
extern void CheckMultiUdpBuff(void);   //���UDP���ͻ���

extern void CreateMultiUdpBuff(int s_socket, uint8_t SendMode, char *RemoteIP, uint16_t DestPort, uint8_t *udpdata, uint16_t nlength, uint8_t CurrOrder);;
extern void CreateMultiUdpBuff_Audio(int s_socket, uint8_t SendMode, char *RemoteIP, uint16_t DestPort, uint8_t *udpdata, uint16_t nlength, uint8_t CurrOrder); //����Ƶ�жϵ���


extern uint16_t video_decode_no;  //��Ƶ�������
extern uint16_t old_mjpeg_stream_no;  //��Ƶ�������

extern uint8_t *Memory6_Stream;//��������
#endif