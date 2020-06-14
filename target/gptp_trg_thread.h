/*
 * Copyright (c) 2020, Sensor-Technik Wiedemann GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPTP_TRG_THREAD_H
#define GPTP_TRG_THREAD_H

#include <pthread.h>
#include "gptp_trg_types.h"


#define K_THREAD_STACK_DEFINE(name,size)     static char name[size]
#define K_THREAD_STACK_SIZEOF(name)          sizeof(name)
#define K_PRIO_COOP(prio)                    (prio)

typedef int32_t     k_timeout_t;
typedef char      k_thread_stack_t;

typedef void (*k_thread_entry_t)(void);


struct k_thread
{
	pthread_t t_Thread;
	char acn_Name[64];
};
typedef struct k_thread*    k_tid_t;


/**
 * @brief Set current thread name
 *
 * Set the name of the thread to be used when THREAD_MONITOR is enabled for
 * tracing and debugging.
 *
 * @param thread_id Thread to set name, or NULL to set the current thread
 * @param value Name string
 * @retval 0 on success
 * @retval -EFAULT Memory access error with supplied string
 * @retval -ENOSYS Thread name configuration option not enabled
 * @retval -EINVAL Thread name too long
 */
int k_thread_name_set(k_tid_t thread_id, const char *value);


/**
 * @brief Create a thread.
 *
 * This routine initializes a thread, then schedules it for execution.
 *
 * The new thread may be scheduled for immediate execution or a delayed start.
 * If the newly spawned thread does not have a delayed start the kernel
 * scheduler may preempt the current thread to allow the new thread to
 * execute.
 *
 * Thread options are architecture-specific, and can include K_ESSENTIAL,
 * K_FP_REGS, and K_SSE_REGS. Multiple options may be specified by separating
 * them using "|" (the logical OR operator).
 *
 * Historically, users often would use the beginning of the stack memory region
 * to store the struct k_thread data, although corruption will occur if the
 * stack overflows this region and stack protection features may not detect this
 * situation.
 *
 * @param new_thread Pointer to uninitialized struct k_thread
 * @param stack Pointer to the stack space.
 * @param stack_size Stack size in bytes.
 * @param entry Thread entry function.
 * @param p1 1st entry point parameter.
 * @param p2 2nd entry point parameter.
 * @param p3 3rd entry point parameter.
 * @param prio Thread priority.
 * @param options Thread options.
 * @param delay Scheduling delay, or K_NO_WAIT (for no delay).
 *
 * @return ID of new thread.
 *
 */
k_tid_t k_thread_create(struct k_thread *new_thread,
				  k_thread_stack_t *stack,
				  size_t stack_size,
				  k_thread_entry_t entry,
				  void *p1, void *p2, void *p3,
				  int prio, uint32_t options, k_timeout_t delay);


unsigned int irq_lock(void);

void irq_unlock(unsigned int key);


#endif