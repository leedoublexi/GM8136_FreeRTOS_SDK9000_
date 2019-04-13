
#include "stdlib.h"

#include "x_udp.h"
#include "m_init.h"

int xtm_printf(const char *fmt, ...);



//---------------------------------------------------------------------------
#define DEBUG_MAX 200
int xtm_printf(const char *fmt, ...)
{
  printf(fmt, ...);
}
//---------------------------------------------------------------------------

