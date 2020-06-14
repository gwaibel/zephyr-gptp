#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/gptp.h>
#include <logging/log.h>
LOG_MODULE_DECLARE(net_gptp, CONFIG_NET_GPTP_LOG_LEVEL);

#include "gptp_trg_net.h"
#include "gptp_trg_queue.h"


K_FIFO_DEFINE(queue);

typedef struct
{
   sys_snode_t node;
   char name[16];
} T_Element;


void sys_list_print(sys_slist_t *list)
{
   sys_snode_t *sn, *sns;

	SYS_SLIST_FOR_EACH_NODE_SAFE(list, sn, sns)
   {
      printf("%s ", sn->name);
   }
   printf("(end)\n");
}


void TimerFunc(struct k_timer *timer)
{
   printf("%i: %s", (int)k_uptime_get(), (char*)timer->user_data);
}


int main()
{
   bool ret;
   sys_slist_t list = { NULL };
   sys_snode_t node1, node2, node3;

   strcpy(node1.name, "node1");
   strcpy(node2.name, "node2");
   strcpy(node3.name, "node3");

   LOG_INF("Hello from gptp\n");

   sys_list_print(&list);
   sys_slist_prepend(&list, &node1);
   sys_list_print(&list);
   sys_slist_prepend(&list, &node2);
   sys_list_print(&list);
   sys_slist_prepend(&list, &node3);
   sys_list_print(&list);

   ret = sys_slist_find_and_remove(&list, &node3);
   sys_list_print(&list);
   ret = sys_slist_find_and_remove(&list, &node1);
   sys_list_print(&list);
   ret = sys_slist_find_and_remove(&list, &node2);
   sys_list_print(&list);
   ret = sys_slist_find_and_remove(&list, &node1);
   sys_list_print(&list);
   ret = sys_slist_find_and_remove(&list, &node3);
   sys_list_print(&list);


   T_Element at_Element[5];

   T_Element *pt_Element;

   strcpy(at_Element[0].name, "item0");
   strcpy(at_Element[1].name, "item1");
   strcpy(at_Element[2].name, "item2");
   strcpy(at_Element[3].name, "item3");
   strcpy(at_Element[4].name, "item4");

   k_fifo_put(&queue, &at_Element[0]);
   k_fifo_put(&queue, &at_Element[1]);
   k_fifo_put(&queue, &at_Element[2]);
   k_fifo_put(&queue, &at_Element[3]);
   k_fifo_put(&queue, &at_Element[4]);

   pt_Element = k_fifo_get(&queue, 0);
   printf("%s\n", pt_Element->name);
   pt_Element = k_fifo_get(&queue, 0);
   printf("%s\n", pt_Element->name);
   pt_Element = k_fifo_get(&queue, 1);
   printf("%s\n", pt_Element->name);
   pt_Element = k_fifo_get(&queue, 1);
   printf("%s\n", pt_Element->name);
   pt_Element = k_fifo_get(&queue, 1);
   printf("%s\n", pt_Element->name);
   pt_Element = k_fifo_get(&queue, 1);
   printf("%p\n", pt_Element);


   struct k_timer timer1, timer2, timer3, timer4;
   static char acn_Timer1String[] = "timer1 (2000/1000ms)\n";
   static char acn_Timer2String[] = "timer2 (3000/500ms)\n";
   static char acn_Timer3String[] = "timer3 (1000/1500ms)\n";
   static char acn_Timer4String[] = "timer4 (1500/2000ms)\n";

   k_timer_init(&timer1, TimerFunc, NULL);
   k_timer_init(&timer2, TimerFunc, NULL);
   k_timer_init(&timer3, TimerFunc, NULL);
   k_timer_init(&timer4, TimerFunc, NULL);

   timer1.user_data = acn_Timer1String;
   timer2.user_data = acn_Timer2String;
   timer3.user_data = acn_Timer3String;
   timer4.user_data = acn_Timer4String;

   usleep(1000*1000);

   printf("%i: Timer1 start\n", (int)k_uptime_get());
   k_timer_start(&timer1, 2000, 1000);
   usleep(500*1000);
   printf("%i: Timer2 start\n", (int)k_uptime_get());
   k_timer_start(&timer2, 3000, 500);
   printf("%i: Timer3 start\n", (int)k_uptime_get());
   k_timer_start(&timer3, 1000, 1500);
   printf("%i: Timer4 start\n", (int)k_uptime_get());
   k_timer_start(&timer4, 1500, 2000);

   usleep(5*1000*1000);

   printf("Timer1 stop\n");
   printf("Timer2 stop\n");
   k_timer_stop(&timer1);
   k_timer_stop(&timer2);
   usleep(5*1000*1000);
   printf("Timer3 stop\n");
   printf("Timer4 stop\n");
   k_timer_stop(&timer3);
   k_timer_stop(&timer4);

   printf("%i: Timer1 start\n", (int)k_uptime_get());
   k_timer_start(&timer1, 2000, 1000);

   usleep(5*1000*1000);

   return 0;
}
