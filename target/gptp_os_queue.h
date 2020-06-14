/*
 * Copyright (c) 2020, Sensor-Technik Wiedemann GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPTP_OS_FIFO_H
#define GPTP_OS_FIFO_H

#include <unistd.h>
#include "gptp_os_types.h"
#include "gptp_os_sys.h"


#define K_FIFO_DEFINE(name) struct k_queue name


struct k_queue {
	sys_slist_t t_List;

};
/**
 * @brief Add an element to a FIFO queue.
 *
 * This routine adds a data item to @a fifo. A FIFO data item must be
 * aligned on a word boundary, and the first word of the item is reserved
 * for the kernel's use.
 *
 * @note Can be called by ISRs.
 *
 * @param fifo Address of the FIFO.
 * @param data Address of the data item.
 *
 * @return N/A
 */
#define k_fifo_put(fifo, data) \
	k_queue_append((fifo), data) //k_queue_append(&(fifo)->_queue, data)

/**
 * @brief Get an element from a FIFO queue.
 *
 * This routine removes a data item from @a fifo in a "first in, first out"
 * manner. The first word of the data item is reserved for the kernel's use.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param fifo Address of the FIFO queue.
 * @param timeout Waiting period to obtain a data item,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @return Address of the data item if successful; NULL if returned
 * without waiting, or waiting period timed out.
 */
#define k_fifo_get(fifo, timeout) \
	k_queue_get((fifo), timeout) //k_queue_get(&(fifo)->_queue, timeout)



/**
 * @brief Append an element to the end of a queue.
 *
 * This routine appends a data item to @a queue. A queue data item must be
 * aligned on a word boundary, and the first word of the item is reserved
 * for the kernel's use.
 *
 * @note Can be called by ISRs.
 *
 * @param queue Address of the queue.
 * @param data Address of the data item.
 *
 * @return N/A
 */
static inline void k_queue_append(struct k_queue *queue, void *data)
{
   sys_slist_prepend(&queue->t_List, data);
}

/**
 * @brief Get an element from a queue.
 *
 * This routine removes first data item from @a queue. The first word of the
 * data item is reserved for the kernel's use.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param queue Address of the queue.
 * @param timeout Non-negative waiting period to obtain a data item
 *                or one of the special values K_NO_WAIT and
 *                K_FOREVER.
 *
 * @return Address of the data item if successful; NULL if returned
 * without waiting, or waiting period timed out.
 */
static inline void *k_queue_get(struct k_queue *queue, k_timeout_t timeout)
{
	sys_snode_t *node;

   node = sys_slist_get_tail(&queue->t_List);

   while ((node == NULL) && (timeout > 0))
   {
      usleep(1000);
      timeout--;
      node = sys_slist_get_tail(&queue->t_List);
   }

	return node;
}


#endif