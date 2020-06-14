#include "gptp_trg_net.h"
#include "gptp_trg_sys.h"



static sys_slist_t mt_FreeNetPktList;
static struct net_pkt mat_NetPkt[CONFIG_NET_GPTP_NETPKT_COUNT];


static void m_net_pkt_allocator_init(void)
{
   uint16_t u16_Idx;

   for (u16_Idx = 0; u16_Idx < CONFIG_NET_GPTP_NETPKT_COUNT; u16_Idx++)
   {
      sys_slist_prepend(&mt_FreeNetPktList, &mat_NetPkt[u16_Idx].node);
   }
}

static struct net_pkt * m_net_pkt_struct_alloc(void)
{
   struct net_pkt *pt_NetPkt = NULL;
   sys_snode_t *pt_Node = sys_slist_get_tail(&mt_FreeNetPktList)

   if (pt_Node != NULL)
   {
      pt_NetPkt = CONTAINER_OF(pt_Node, struct net_pkt, node);
   }

   return pt_NetPkt;
}

static void m_net_pkt_struct_free(struct net_pkt * pt_NetPkt)
{
   sys_slist_prepend(&mt_FreeNetPktList, pt_NetPkt->node);
}


struct net_pkt *net_pkt_alloc_with_buffer(struct net_if *iface, size_t size,
                                          unsigned short int family, int proto, k_timeout_t timeout)
{
   static bool q_Firstrun = true;
   struct net_pkt *pt_NetPkt;
   struct pbuf *pt_Pbuf;

   (void)proto; // ??
   (void)family; // ??
   (void)timeout; // ??

   if (q_Firstrun == true)
   {
      m_net_pkt_allocator_init();
      q_Firstrun = false;
   }


   pt_NetPkt = m_net_pkt_struct_alloc();
   if (pt_NetPkt == NULL)
   {
      goto out;
   }

   pt_Pbuf = pbuf_alloc(PBUF_LINK, size, PBUF_POOL);
   if (pt_Pbuf == NULL)
   {
      goto out_free_netpkt;
   }

   pt_NetPkt->pbuf = pt_Pbuf;
   pt_NetPkt->iface = iface;
   pt_NetPkt->cursor = 0;

out_free_netpkt:
   m_net_pkt_struct_free(pt_NetPkt);
out:
   return pkt;
}

struct net_pkt *net_pkt_ref(struct net_pkt *pkt)
{
   pbuf_ref(pkt->pbuf);
   return pkt;
}

void net_pkt_unref(struct net_pkt *pkt)
{
   uint8_t u8_Ret;

   u8_Ret = pbuf_free(pkt->pbuf);
   if (u8_Ret > 0)
   {
      // pbuf has been freed, free ent_pkt struct
      m_net_pkt_struct_free(pkt);
   }
}

int net_pkt_skip(struct net_pkt *pkt, size_t length)
{
   // TODO: Check pbuf size
   pkt->cursor += length;
   return 0;
}

void net_pkt_cursor_init(struct net_pkt *pkt)
{
   pkt->cursor = 0;
}

int net_pkt_write(struct net_pkt *pkt, const void *data, size_t length)
{
   err_t t_Error;

   t_Error = pbuf_take_at(pkt->pbuf, data, length, pkt->cursor);
   if (t_Error == ERR_OK)
   {
      pkt->cursor += length;
   }

   return (t_Error == ERR_OK) ? 0 : -1;
}

void net_pkt_set_lladdr_dest(struct net_pkt *pkt, struct net_linkaddr *lladdr)
{
   struct net_eth_hdr *ethhdr = NET_ETH_HDR(pkt);
   memcpy(ethhdr->dst.addr, lladdr->addr, 6);
}

void net_pkt_set_lladdr_src(struct net_pkt *pkt, struct net_linkaddr *lladdr)
{
   struct net_eth_hdr *ethhdr = NET_ETH_HDR(pkt);
   memcpy(ethhdr->src.addr, lladdr->addr, 6);
}

void net_pkt_set_lltype(struct net_pkt *pkt, uint16_t type)
{
   struct net_eth_hdr *ethhdr = NET_ETH_HDR(pkt);
   ethhdr->type = type;
}






int net_eth_get_ptp_port(struct net_if *iface)
{
   return 0;
}

struct device *net_eth_get_ptp_clock(struct net_if *iface)
{
   return NULL;
}

void net_eth_set_ptp_port(struct net_if *iface, int port)
{

}

void net_if_queue_tx(struct net_if *iface, struct net_pkt *pkt)
{

}

void net_if_register_timestamp_cb(struct net_if_timestamp_cb *handle, struct net_pkt *pkt,
                                  struct net_if *iface, net_if_timestamp_callback_t cb)
{

}


void net_if_unregister_timestamp_cb(struct net_if_timestamp_cb *handle)
{

}

struct net_linkaddr *net_if_get_link_addr(struct net_if *iface)
{
   return NULL;
}

char *net_sprint_ll_addr_buf(const uint8_t *ll, uint8_t ll_len, char *buf, int buflen)
{
   return NULL;
}

void net_if_foreach(net_if_cb_t cb, void *user_data)
{

}