



#define CommonH
#include "common.h"   

#ifdef _ADDTABLE_SUPPORT  //地址


//读地址表文件
void ReadAddrTableFile(void);
//写地址表文件
void WriteAddrTableFile(void);


uint32_t AddrTablePos = 0;

//从地址表 查询地址
uint8_t  QueryAddrTableFile(char *DestAddr, uint8_t *DestIP);
uint8_t QueryAddrTableMemory(char *DestAddr, uint8_t *DestIP);
uint8_t CaptureAddrFromLine(char *a_Addr, uint8_t *a_IP, char *LineText);

//从地址表 读取一行
uint8_t ReadAddrTableLine(char *LineText);
//---------------------------------------------------------------------------
//读地址表文件
void ReadAddrTableFile(void)
{
  uint16_t i, TotalNum;
  uint8_t  j, addr_len;
  char     LineText[50];
  uint32_t FAddr;
  char     a_Addr[20], tmp_addr[20];
  uint8_t  a_IP[4];
  uint8_t  ParaNum;
  uint16_t MemPara[5];  //最多5段内存, 每行32个字节，1段2048行, 存储在 0x00580000 位置
  uint16_t NumPerPara;
  uint16_t ReadLen;
  NumPerPara = 2048;
  AddrTablePos = 0;
  ReadAddrTableLine(LineText);

  AddrTable.TotalNum = 0;
  if(strcmp(LineText, "ADDRTABLE") == 0)
  {
    ReadAddrTableLine(LineText);  //版本号
    xtm_printf("ReadAddrTableFile::AddrTable Version = %s\n", LineText);
    strcpy(AddrTable.Version, LineText);
    ReadAddrTableLine(LineText);  //数量
    TotalNum = atoi(LineText);
    if(TotalNum > ADDR_MAXNUM)
      TotalNum = ADDR_MAXNUM;
    
    AddrTable.TotalNum = TotalNum;
    xtm_printf("ReadAddrTableFile::AddrTable TotalNum = %d\n", TotalNum);

    if((AddrTable.TotalNum % NumPerPara) == 0)
      ParaNum = AddrTable.TotalNum / NumPerPara;
    else
      ParaNum = AddrTable.TotalNum / NumPerPara + 1;
    
    for(i=0; i<5; i++)
      MemPara[i] = 0;
     
    for(i=0; i<ParaNum; i++)
    {
      if(i == (ParaNum - 1))
        MemPara[i] = AddrTable.TotalNum % NumPerPara;
      else
        MemPara[i] = NumPerPara;
      
      xtm_printf("ReadAddrTableFile:: ParaNum = %d, MemPara[%d] = %d\n", ParaNum, i, MemPara[i]);
      outportb(CONTROL_REG2, inportb(CONTROL_REG2)|0x08);
      outportb(MEMORY_WINDOW_BASE_H, 0x58 + i);
      FAddr =  ADDRTABLE_ADDR + AddrTablePos;
      //xtm_printf("ReadAddrTableFile:: FAddr = 0x%X %X\n", (INT16U)((FAddr & 0xffff0000)>>16), (INT16U)(FAddr & 0xffff));
      if(MemPara[i] < NumPerPara)
        xtm_read25x32data(FAddr, Memory6_Stream, MemPara[i] * 32);
      else
      {
        xtm_read25x32data(FAddr, Memory6_Stream, 0x8000);
        xtm_read25x32data(FAddr + 0x8000, Memory6_Stream + 0x8000, 0x8000);
      }
      outportb(CONTROL_REG2, inportb(CONTROL_REG2)&0xf7);

      //xtm_printf("AddrTablePos 1 = 0x%X\n", AddrTablePos);
      //xtm_printf(" MemPara[i] = 0x%X\n",  MemPara[i]);
       AddrTablePos = AddrTablePos + (INT32U)(MemPara[i]) * 32;
       ClearWatchDog();
       //xtm_printf("AddrTablePos 2 = 0x%X\n", AddrTablePos);
    }
    
    xtm_printf("ReadAddrTableFile:: finish\n");
    /*   
    for(i=0; i<AddrTable.TotalNum; i++)                       //  AddrTable.TotalNum      
    {
      ClearWatchDog();
      outportb(CONTROL_REG2, inportb(CONTROL_REG2)|0x08);
      outportb(MEMORY_WINDOW_BASE_H, 0x58 + i/NumPerPara);
      xtm_memcopy(LineText, Memory6_Stream + (i % NumPerPara)*32, 32);
      LineText[32] = '\0';
      outportb(CONTROL_REG2, inportb(CONTROL_REG2)&0xf7);
      xtm_printf("ReadAddrTableFile::LineText[%d] = %s\n", i, LineText);
    }*/
  }
  else
    xtm_printf("ReadAddrTableFile::no AddrTableFile\n");
}
//--------------------------------------------------------------------------
//写地址表文件
void WriteAddrTableFile(void)
{
}
//--------------------------------------------------------------------------
//从地址表内存 查询地址
uint8_t QueryAddrTableMemory(char *DestAddr, uint8_t *DestIP)
{
  uint16_t i, TotalNum;
  uint8_t  j, addr_len;
  char     LineText[50];
  char     a_Addr[20];
  uint8_t  a_IP[4];
  uint16_t NumPerPara, mem_pos, AddrNum, MultiAddr;
  uint16_t ParaPos, addr_ok;
  NumPerPara = 2048;

  xtm_printf("QueryAddrTableMemory::Version = %s, TotalNum = %d, DestAddr = %s\n", AddrTable.Version, AddrTable.TotalNum, DestAddr);

  AddrNum = 0;
  ParaPos = 0;
  outportb(CONTROL_REG2, inportb(CONTROL_REG2)|0x08);
  outportb(MEMORY_WINDOW_BASE_H, 0x58 + ParaPos);
  for(i=0; i<AddrTable.TotalNum; i++)
  {
      addr_len = 12;
      switch(DestAddr[0])
      {
        case 'S':
            addr_len = 11;   //12
            MultiAddr = 1;
          break;
        case 'B':
            addr_len = 5;   //6
            MultiAddr = 1;
          break;
        case 'M':
            addr_len = 8;
            MultiAddr = 0;
          break;
        case 'W':
        case 'P':
            addr_len = 5;
            MultiAddr = 0;
          break;
        case 'H':
            addr_len = 12;
            MultiAddr = 0;
          break;
        case 'Z':
            addr_len = 5;
            MultiAddr = 0;
          break;
      }
      //xtm_printf("i = %d, TotalNum = %d\n", i, TotalNum);

      if(ParaPos != i/NumPerPara)
      {
        ParaPos = i/NumPerPara;
        outportb(MEMORY_WINDOW_BASE_H, 0x58 + ParaPos);
      }

      //
      mem_pos = (i % NumPerPara) * 32;
      addr_ok = 1;

      /*
      xtm_printf("i = %d, addr_len = %d, mem_pos = %d\n", i, addr_len, mem_pos);
      xtm_memcopy(LineText, Memory6_Stream + mem_pos, addr_len);
      LineText[addr_len] = '\0';
      xtm_printf("LineText = %s\n", LineText);    */

      for(j=0; j<addr_len; j++)
        if(Memory6_Stream[mem_pos + j] != DestAddr[j])
        {
          addr_ok = 0;
          break;
        }
      
      if(addr_ok == 1)
      {
        //xtm_printf("i = %d, LineText = %s\n", i, LineText);
        xtm_memcopy(LineText, Memory6_Stream + mem_pos, 32);
        CaptureAddrFromLine(a_Addr, a_IP, LineText);

        xtm_memcopy(Remote.Addr[AddrNum], a_Addr, 20);
        xtm_memcopy(Remote.IP[AddrNum], a_IP, 4);
        xtm_printf("a_Addr = %s, DestIP = %d.%d.%d.%d\n", Remote.Addr[AddrNum], Remote.IP[AddrNum][0], Remote.IP[AddrNum][1], Remote.IP[AddrNum][2], Remote.IP[AddrNum][3]);
        AddrNum ++;

        if(MultiAddr == 0)
          break;
       }

       ClearWatchDog();
       //xtm_printf("a_Addr = %s, DestAddr = %s\n", a_Addr, DestAddr);
   }
  outportb(CONTROL_REG2, inportb(CONTROL_REG2)&0xf7);
  return AddrNum;
}
//--------------------------------------------------------------------------
uint8_t QueryAddrTableFile(char *DestAddr, uint8_t *DestIP)
{
  uint16_t i, TotalNum;
  uint8_t  j, addr_len;
  char     LineText[50];
  char     a_Addr[20], tmp_addr[20];
  uint8_t  a_IP[4];
  AddrTablePos = 0;
  ReadAddrTableLine(LineText);
  xtm_printf("QueryAddrTableFile , %s, DestAddr = %s\n", LineText, DestAddr);
  if(strcmp(LineText, "ADDRTABLE") == 0)
  {
    ReadAddrTableLine(LineText);  //版本号
    xtm_printf("AddrTable Version = %s\n", LineText);
    ReadAddrTableLine(LineText);  //数量
    TotalNum = atoi(LineText);
    
    if(TotalNum > ADDR_MAXNUM)
      TotalNum = ADDR_MAXNUM;
    xtm_printf("AddrTable TotalNum = %d\n", TotalNum);

    for(i=0; i<TotalNum; i++)
    {
      //xtm_printf("i = %d, TotalNum = %d\n", i, TotalNum);
      strcpy(tmp_addr, DestAddr);
      ReadAddrTableLine(LineText);  //地址
      //xtm_printf("i = %d, LineText = %s\n", i, LineText);
      CaptureAddrFromLine(a_Addr, a_IP, LineText);
      switch(a_Addr[0])
      {
        case 'S':
            a_Addr[12] = '\0';
            tmp_addr[12] = '\0';
          break;
        case 'B':
            a_Addr[6] = '\0';
            tmp_addr[6] = '\0';
          break;
        case 'M':
            a_Addr[8] = '\0';
            tmp_addr[8] = '\0';
          break;
        case 'W':
        case 'P':
            a_Addr[5] = '\0';
            tmp_addr[5] = '\0';
          break;
        case 'H':
            a_Addr[12] = '\0';
            tmp_addr[12] = '\0';
          break;
        case 'Z':
            a_Addr[5] = '\0';
            tmp_addr[5] = '\0';
          break;
      }
      
      ClearWatchDog(); 
      //xtm_printf("a_Addr = %s, DestAddr = %s\n", a_Addr, DestAddr);
      if(strcmp(a_Addr, tmp_addr) == 0)
      {
        xtm_memcopy(DestIP, a_IP, 4);
        xtm_printf("a_Addr = %s, DestIP = %d.%d.%d.%d\n", a_Addr, DestIP[0], DestIP[1], DestIP[2], DestIP[3]);
        return 1;
      }
      //xtm_printf("RecordsContent[%d].Length = %d, RecordsBuff[%d] = 0x%X 0x%X\n", i, RecordsContent[i].Length, i, (INT16U)((RecordsBuff[i] & 0xffff0000) >> 16), (INT16U)(RecordsBuff[i] & 0xffff));
    }
  }
  else
  {
    return 0;
  }
}

//--------------------------------------------------------------------------
uint8_t CaptureAddrFromLine(char *a_Addr, uint8_t *a_IP, char *LineText)
{
  uint8_t j, addr_len, Addr_pos, ip_len;
  char ip_text[20];
  char TmpText[4][5];

  addr_len = strlen(LineText);
  Addr_pos = 0;
  for(j=0; j<addr_len; j++)
  {
    if((LineText[j] == '-')&&(LineText[j+1] == '-')&&(LineText[j+2] == '-'))
    {
      Addr_pos = j;
      break;
    }
  }
  xtm_memcopy(a_Addr, LineText, Addr_pos);
  a_Addr[Addr_pos] = '\0';

  //xtm_printf("addr_len = %d, Addr_pos = %d\n", addr_len, Addr_pos);
  ip_len = addr_len - Addr_pos - 3;
  
  if(ip_len > 15)
    ip_len = 15;
  
  xtm_memcopy(ip_text, LineText + Addr_pos + 3, ip_len);
  ip_text[ip_len] = '\0';

  xtm_memcopy(TmpText[0], ip_text, 3);
  TmpText[0][3] = '\0';
  xtm_memcopy(TmpText[1], ip_text + 4, 3);
  TmpText[1][3] = '\0';
  xtm_memcopy(TmpText[2], ip_text + 8, 3);
  TmpText[2][3] = '\0';
  xtm_memcopy(TmpText[3], ip_text + 12, 3);
  TmpText[3][3] = '\0';       

  for(j=0; j<4; j++)
    a_IP[j] = atoi(TmpText[j]);    
  //xtm_printf("a_Addr = %s, ip_text = %s\n", a_Addr, ip_text);
}
//--------------------------------------------------------------------------
//从地址表 读取一行
uint8_t ReadAddrTableLine(char *LineText)
{
  uint8_t LinePos;
  LinePos = 0;
  while(1)
  {
    xtm_read25x32data(ADDRTABLE_ADDR + AddrTablePos, LineText + LinePos, 1);
    AddrTablePos ++;
    if(LineText[LinePos] == '\r')
    {
      LineText[LinePos] = '\0';
      AddrTablePos ++;
      break;
    }
    LinePos ++;

    if(LinePos >= 39)
    {
      LineText[LinePos] = '\0';
      break;
    }
  }
  //xtm_printf("LinePos = %d\n", LinePos);
  return 1;
}

#endif

