#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include "gptp_trg_thread.h"
#include "gptp_trg_timer.h"


static pthread_t mt_TimerThread;
static pthread_mutex_t mt_TimerLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mt_WakeupLock = PTHREAD_MUTEX_INITIALIZER;
static sys_slist_t mt_TimerList;
static pthread_mutex_t mt_IrqLock = PTHREAD_MUTEX_INITIALIZER;


unsigned int irq_lock(void)
{
   pthread_mutex_lock(&mt_IrqLock);
   return 0;
}

void irq_unlock(unsigned int key)
{
   (void)key;
   pthread_mutex_unlock(&mt_IrqLock);
}



static void *k_timer_thread(void *ptr)
{
	sys_snode_t * pt_Node;
	sys_snode_t * pt_NodeNext;
   struct timespec t_Timeout;

   while (1)
   {
      int64_t s64_Now = k_uptime_get();
      int64_t s64_NextTime = __INT64_MAX__;
      int64_t s64_Sleep;

      pthread_mutex_lock(&mt_IrqLock);
      pthread_mutex_lock(&mt_TimerLock);
      SYS_SLIST_FOR_EACH_NODE_SAFE(&mt_TimerList, pt_Node, pt_NodeNext)
      {
         struct k_timer * pt_Timer = CONTAINER_OF(pt_Node, struct k_timer, node);

         if (pt_Timer->next_time > 0)
         {
            if (pt_Timer->next_time <= s64_Now)
            {
               // Unlock mutex to a void deadlock if timer function calls k_timer_stop()
               pthread_mutex_unlock(&mt_TimerLock);
               pt_Timer->expiry_fn(pt_Timer);
               pthread_mutex_lock(&mt_TimerLock);

               if ((pt_Timer->next_time > 0) && (pt_Timer->period > 0))
               {
                  // Timer is still active and is not a one shot timer
                  pt_Timer->next_time = s64_Now + pt_Timer->period;
               }
               else
               {
                  pt_Timer->next_time = 0;
               }
            }

            if (pt_Timer->next_time > 0)
            {
               s64_NextTime = (pt_Timer->next_time < s64_NextTime) ? pt_Timer->next_time : s64_NextTime;
            }
         }
      }
      pthread_mutex_unlock(&mt_TimerLock);
      pthread_mutex_unlock(&mt_IrqLock);

      s64_Sleep = s64_NextTime - s64_Now;
      clock_gettime(CLOCK_REALTIME, &t_Timeout);
      t_Timeout.tv_sec += s64_Sleep / 1000;
      t_Timeout.tv_nsec += (s64_Sleep % 1000) * 1000 * 1000;
      pthread_mutex_timedlock(&mt_WakeupLock,  &t_Timeout);
   }

   return NULL;
}


void k_timer_init(struct k_timer *timer, k_timer_expiry_t expiry_fn, k_timer_stop_t stop_fn)
{
   static bool q_FirstRun = true;

   if (q_FirstRun == true)
   {
      pthread_create(&mt_TimerThread, NULL, k_timer_thread, NULL);
      q_FirstRun = false;
   }

   pthread_mutex_lock(&mt_TimerLock);

   timer->expiry_fn = expiry_fn;
   timer->stop_fn = stop_fn;
   timer->next_time = 0;

   // Stop function not implemented
   assert((stop_fn == NULL));

   pthread_mutex_unlock(&mt_TimerLock);
}


void k_timer_start(struct k_timer *timer, k_timeout_t duration, k_timeout_t period)
{
   sys_snode_t * pt_Node, *sns;

   pthread_mutex_lock(&mt_TimerLock);
   timer->period = period;
   timer->duration = duration;
   timer->next_time = k_uptime_get() + duration;
   sys_slist_prepend(&mt_TimerList, &timer->node);
   pthread_mutex_unlock(&mt_TimerLock);

   // Wake timer thread
   pthread_mutex_unlock(&mt_WakeupLock);
}


void k_timer_stop(struct k_timer *timer)
{
   pthread_mutex_lock(&mt_TimerLock);
   timer->next_time = 0;
   sys_slist_find_and_remove(&mt_TimerList, &timer->node);
   pthread_mutex_unlock(&mt_TimerLock);
}


uint32_t k_timer_remaining_get(struct k_timer *timer)
{
   uint32_t uint32_time = 0;

   pthread_mutex_lock(&mt_TimerLock);
   if (timer->next_time > 0)
   {
      int64_t s64_Now = k_uptime_get();
      int64_t int64_time = timer->next_time - s64_Now;

      uint32_time = (int64_time > 0) ? (uint32_t)int64_time : 0;
   }
   pthread_mutex_unlock(&mt_TimerLock);

   return uint32_time;
}


int64_t k_uptime_get(void)
{
   int32_t s32_Error;
   uint64_t uint64_time_us = 0;
   struct timespec t_Time;

   s32_Error = clock_gettime(CLOCK_MONOTONIC, &t_Time);
   if (s32_Error == 0)
   {
      uint64_time_us = ((uint64_t)t_Time.tv_sec * 1000000) + ((t_Time.tv_nsec + 500) / 1000);
   }
   return (int64_t)(uint64_time_us / 1000u);
}


