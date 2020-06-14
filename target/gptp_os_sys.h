/*
 * Copyright (c) 2020, Sensor-Technik Wiedemann GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPTP_OS_SYS_H
#define GPTP_OS_SYS_H

/*
 * This header provides basic OS types for the gptp stack.
 * This file shall be deployed with the gptp API headers.
 */

#include "gptp_os_types.h"


typedef struct _snode sys_snode_t;
struct _snode {
	struct _snode *next;
	struct _snode *prev;
   char name[16]; // For debug!!
};


typedef struct _slist sys_slist_t;
struct _slist {
   sys_snode_t *head;
   sys_snode_t *tail;
};

/**
 * @brief Find and remove a node from a list
 *
 * This and other sys_slist_*() functions are not thread safe.
 *
 * @param list A pointer on the list to affect
 * @param node A pointer on the node to remove from the list
 *
 * @return true if node was removed
 */
static inline bool sys_slist_find_and_remove(sys_slist_t *list, sys_snode_t *node)
{
   bool found = false;
   sys_snode_t *curr = list->head;
   sys_snode_t *prev = list->head;

   while (curr != NULL)
   {
      if (curr == node)
      {
         if (curr == list->head)
         {
            list->head = curr->next;
            if (list->head != NULL)
            {
               list->head->prev = NULL;
            }
         }
         else
         {
            prev->next = curr->next;
            if (curr->next != NULL)
            {
               curr->next->prev = prev;
            }
         }

         if (list->head == NULL)
         {
            list->tail = NULL;
         }
         else if (curr->next == NULL)
         {
            list->tail = prev;
         }

         found = true;
         break;
      }
      prev = curr;
      curr = curr->next;
   }

   return found;
}

/**
 * @brief Prepend a node to the given list
 *
 * This and other sys_slist_*() functions are not thread safe.
 *
 * @param list A pointer on the list to affect
 * @param node A pointer on the node to prepend
 */
static inline void sys_slist_prepend(sys_slist_t *list, sys_snode_t *node)
{
   node->prev = NULL;
   node->next = list->head;
   list->head = node;
   if (node->next != NULL)
   {
      node->next->prev = node;
   }
   if (list->tail == NULL)
   {
      list->tail = node;
   }
}

/**
 * @brief Return the tail node of the list and remove it from the list
 *
 * This and other sys_slist_*() functions are not thread safe.
 *
 * @param list A pointer on the list to affect
 *
 * @return A pointer to the last node from the list
 */
static inline sys_snode_t * sys_slist_get_tail(sys_slist_t *list)
{
   sys_snode_t *node = list->tail;
   if (list->tail != NULL)
   {
      list->tail = list->tail->prev;
      if (list->tail != NULL)
      {
         list->tail->next = NULL;
      }
   }

   return node;
}

/**
 * @brief Peek the first node from the list
 *
 * @param list A point on the list to peek the first node from
 *
 * @return A pointer on the first node of the list (or NULL if none)
 */
static inline sys_snode_t *sys_slist_peek_head(sys_slist_t *list)
{
	return list->head;
}

/**
 * @brief Peek the next node from current node
 *
 * @param node A pointer on the node where to peek the next node
 *
 * @return a pointer on the next node (or NULL if none)
 */
static inline sys_snode_t *sys_slist_peek_next(sys_snode_t *node)
{
	return (node) ? node->next : NULL;
}

#define Z_GENLIST_FOR_EACH_NODE_SAFE(__lname, __l, __sn, __sns) \
   for (__sn = sys_ ## __lname ## _peek_head(__l), \
           __sns = sys_ ## __lname ## _peek_next(__sn); \
        __sn != NULL ; __sn = __sns, \
           __sns = sys_ ## __lname ## _peek_next(__sn))
#define SYS_SLIST_FOR_EACH_NODE_SAFE(__sl, __sn, __sns) \
   Z_GENLIST_FOR_EACH_NODE_SAFE(slist, __sl, __sn, __sns)



/**
 * @brief Return a 32-bit random value that should pass general
 * randomness tests.
 *
 * @note The random value returned is not a cryptographically secure
 * random number value.
 *
 * @return 32-bit random value.
 */
extern uint32_t sys_rand32_get(void);



#endif