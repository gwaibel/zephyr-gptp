#include <stdio.h>

#include "gptp_trg_ptp.h"



int ptp_clock_get(struct device *dev, struct net_ptp_time *tm)
{
   tm->second = 0;
   tm->nanosecond = 0;
   printf("%s: sec=%lu nsec=%u", __FUNCTION__, tm->second, tm->nanosecond);
   return 0;
}

int ptp_clock_set(struct device *dev, struct net_ptp_time *tm)
{
   printf("%s: sec=%lu nsec=%u", __FUNCTION__, tm->second, tm->nanosecond);
   return 0;
}

int ptp_clock_rate_adjust(struct device *dev, float rate)
{
   printf("%s: rate=%f", __FUNCTION__, rate);
   return 0;
}

int ptp_clock_adjust(struct device *dev, int increment)
{
   printf("%s: inc=%u", __FUNCTION__, increment);
   return 0;
}