#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>

#include <logging/log.h>
#include <gptp_trg_sys.h>






void log_print(const char *format, ...)
{
   va_list args;
   va_start(args, format);

   vprintf(format, args);
   va_end(args);
}


uint32_t sys_rand32_get(void)
{
   const long int number = mrand48();

   return (number & 0xFFFFFFFF);
}
