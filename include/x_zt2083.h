#ifndef x_zt2083_h
#define x_zt2083_h

extern void Write_i2c(uint8_t WriteDeviceAddress, uint8_t Wdata);
extern uint16_t Read_i2c(uint8_t ReadDeviceAddress);

extern void readtp(void);

extern void test_i2c(void);
#endif