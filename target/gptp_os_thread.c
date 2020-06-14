#include <string.h>
#include <pthread.h>
#include "gptp_os_thread.h"


static void *k_thread_thread(void *ptr)
{
   k_thread_entry_t pf_Func = ptr;

   pf_Func();
}

k_tid_t k_thread_create(struct k_thread *new_thread, k_thread_stack_t *stack,
                        size_t stack_size, k_thread_entry_t entry,
                        void *p1, void *p2, void *p3,
                        int prio, uint32_t options, k_timeout_t delay)
{
   pthread_t mt_TimerThread;

   new_thread->acn_Name[0] = '\0';

   pthread_create(&new_thread->t_Thread, NULL, k_thread_thread, entry);

   return new_thread;
}

int k_thread_name_set(k_tid_t thread_id, const char *value)
{
   strncpy(thread_id->acn_Name, value, sizeof(thread_id->acn_Name));
}