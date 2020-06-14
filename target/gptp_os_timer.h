/*
 * Copyright (c) 2020, Sensor-Technik Wiedemann GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPTP_OS_TIMER_H
#define GPTP_OS_TIMER_H

#include "gptp_os_types.h"
#include "gptp_os_sys.h"
#include "gptp_os_thread.h"


#define USEC_PER_SEC                (1000U * 1000U)
#define MSEC_PER_SEC                (1000U)
#define NSEC_PER_USEC               (1000U)
#define USEC_PER_MSEC               (1000U)


#define K_NO_WAIT 0

/**
 * @brief Generate timeout delay from milliseconds.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a ms milliseconds to perform the requested operation.
 *
 * @param ms Duration in milliseconds.
 *
 * @return Timeout delay value.
 */
#define K_MSEC(ms)     ms //Z_TIMEOUT_MS(ms)

struct k_timer;

/**
 * @typedef k_timer_expiry_t
 * @brief Timer expiry function type.
 *
 * A timer's expiry function is executed by the system clock interrupt handler
 * each time the timer expires. The expiry function is optional, and is only
 * invoked if the timer has been initialized with one.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
typedef void (*k_timer_expiry_t)(struct k_timer *timer);

/**
 * @typedef k_timer_stop_t
 * @brief Timer stop function type.
 *
 * A timer's stop function is executed if the timer is stopped prematurely.
 * The function runs in the context of the thread that stops the timer.
 * The stop function is optional, and is only invoked if the timer has been
 * initialized with one.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
typedef void (*k_timer_stop_t)(struct k_timer *timer);


struct k_timer
{
   sys_snode_t node;

   /* runs in ISR context */
   void (*expiry_fn)(struct k_timer *timer);

   /* runs in the context of the thread that calls k_timer_stop() */
   void (*stop_fn)(struct k_timer *timer);

   /* timer period [ms] */
   k_timeout_t period;
   /* timer initial offset [ms] */
   k_timeout_t duration;

   /* next time the timer triggers [ms] */
   int64_t next_time;

   /* user-specific data, also used to support legacy features */
   void *user_data;
};


/**
 * @brief Get time remaining before a timer next expires.
 *
 * This routine computes the (approximate) time remaining before a running
 * timer next expires. If the timer is not running, it returns zero.
 *
 * @param timer     Address of timer.
 *
 * @return Remaining time (in milliseconds).
 */
extern uint32_t k_timer_remaining_get(struct k_timer *timer);

/**
 * @brief Initialize a timer.
 *
 * This routine initializes a timer, prior to its first use.
 *
 * @param timer     Address of timer.
 * @param expiry_fn Function to invoke each time the timer expires.
 * @param stop_fn   Function to invoke if the timer is stopped while running.
 *
 * @return N/A
 */
extern void k_timer_init(struct k_timer *timer,
			 k_timer_expiry_t expiry_fn,
			 k_timer_stop_t stop_fn);

/**
 * @brief Start a timer.
 *
 * This routine starts a timer, and resets its status to zero. The timer
 * begins counting down using the specified duration and period values.
 *
 * Attempting to start a timer that is already running is permitted.
 * The timer's status is reset to zero and the timer begins counting down
 * using the new duration and period values.
 *
 * @param timer     Address of timer.
 * @param duration  Initial timer duration.
 * @param period    Timer period.
 *
 * @return N/A
 */
void k_timer_start(struct k_timer *timer,
			     k_timeout_t duration, k_timeout_t period);


/**
 * @brief Stop a timer.
 *
 * This routine stops a running timer prematurely. The timer's stop function,
 * if one exists, is invoked by the caller.
 *
 * Attempting to stop a timer that is not running is permitted, but has no
 * effect on the timer.
 *
 * @note Can be called by ISRs.  The stop handler has to be callable from ISRs
 * if @a k_timer_stop is to be called from ISRs.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
void k_timer_stop(struct k_timer *timer);


/**
 * @brief Get system uptime.
 *
 * This routine returns the elapsed time since the system booted,
 * in milliseconds.
 *
 * @note
 *    @rst
 *    While this function returns time in milliseconds, it does
 *    not mean it has millisecond resolution. The actual resolution depends on
 *    :option:`CONFIG_SYS_CLOCK_TICKS_PER_SEC` config option.
 *    @endrst
 *
 * @return Current uptime in milliseconds.
 */
int64_t k_uptime_get(void);

#endif