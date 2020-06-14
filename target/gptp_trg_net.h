/*
 * Copyright (c) 2020, Sensor-Technik Wiedemann GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPTP_TRG_NET_H
#define GPTP_TRG_NET_H

#include "gptp_trg_types.h"
#include "gptp_trg_sys.h"
#include "gptp_trg_timer.h"
#include <net/ptp_time.h>

#include "gptp_trg_target.h"


#define NET_ASSERT(cond, ...) __ASSERT(cond, "" __VA_ARGS__)

#define NET_ETH_MINIMAL_FRAME_SIZE	60

/** Network packet priority settings described in IEEE 802.1Q Annex I.1 */
enum net_priority {
	NET_PRIORITY_BK = 1, /**< Background (lowest)                */
	NET_PRIORITY_BE = 0, /**< Best effort (default)              */
	NET_PRIORITY_EE = 2, /**< Excellent effort                   */
	NET_PRIORITY_CA = 3, /**< Critical applications (highest)    */
	NET_PRIORITY_VI = 4, /**< Video, < 100 ms latency and jitter */
	NET_PRIORITY_VO = 5, /**< Voice, < 10 ms latency and jitter  */
	NET_PRIORITY_IC = 6, /**< Internetwork control               */
	NET_PRIORITY_NC = 7  /**< Network control                    */
};

/**
 * @brief Net Verdict
 */
enum net_verdict {
	/** Packet has been taken care of. */
	NET_OK,
	/** Packet has not been touched, other part should decide about its
	 * fate.
	 */
	NET_CONTINUE,
	/** Packet must be dropped. */
	NET_DROP,
};


struct net_eth_addr {
	uint8_t addr[6];
};

struct net_eth_hdr {
	struct net_eth_addr dst;
	struct net_eth_addr src;
	uint16_t type;
} __packed;
#define NET_ETH_HDR(pkt) ((struct net_eth_hdr *)((uint8_t*)pkt->pbuf->payload - sizeof(struct net_eth_hdr)))   /* ((struct net_eth_hdr *)net_pkt_data(pkt)) */

/**
 *  @brief Hardware link address structure
 *
 *  Used to hold the link address information
 */
struct net_linkaddr {
   /** The array of byte representing the address */
   uint8_t *addr;

   /** Length of that address array */
   uint8_t len;

   /** What kind of address is this for */
   uint8_t type;
};

/**
 * @brief Network packet.
 *
 * Note that if you add new fields into net_pkt, remember to update
 * net_pkt_clone() function.
 */
struct net_pkt {

   /* This must be the first entry */
   sys_snode_t node;

   struct pbuf *pbuf;
   uint16_t cursor;

   /** Network interface */
   struct net_if *iface;

   /** Timestamp if available. */
   struct net_ptp_time timestamp;

   /** Network packet priority, can be left out in which case packet
    * is not prioritised.
    */
   uint8_t priority;

   /* VLAN TCI (Tag Control Information). This contains the Priority
    * Code Point (PCP), Drop Eligible Indicator (DEI) and VLAN
    * Identifier (VID, called more commonly VLAN tag). This value is
    * kept in host byte order.
    */
   uint16_t vlan_tci;
};

/**
 * @brief Network Interface structure
 *
 * Used to handle a network interface on top of a net_if_dev instance.
 * There can be many net_if instance against the same net_if_dev instance.
 *
 */
struct net_if {
	int tx_pending;
}; //__net_if_align;

/**
 * @typedef net_if_timestamp_callback_t
 * @brief Define callback that is called after a network packet
 *        has been timestamped.
 * @param "struct net_pkt *pkt" A pointer on a struct net_pkt which has
 *        been timestamped after being sent.
 */
typedef void (*net_if_timestamp_callback_t)(struct net_pkt *pkt);


/**
 * @brief Timestamp callback handler struct.
 *
 * Stores the timestamp callback information. Caller must make sure that
 * the variable pointed by this is valid during the lifetime of
 * registration. Typically this means that the variable cannot be
 * allocated from stack.
 */
struct net_if_timestamp_cb {
	/** Node information for the slist. */
	sys_snode_t node;

	/** Packet for which the callback is needed.
	 *  A NULL value means all packets.
	 */
	struct net_pkt *pkt;

	/** Net interface for which the callback is needed.
	 *  A NULL value means all interfaces.
	 */
	struct net_if *iface;

	/** Timestamp callback */
	net_if_timestamp_callback_t cb;
};

/**
 * @brief Get an network interface's link address
 *
 * @param iface Pointer to a network interface structure
 *
 * @return a pointer to the network link address
 */
struct net_linkaddr *net_if_get_link_addr(struct net_if *iface);

/**
 * @brief Return gPTP port number attached to this interface.
 *
 * @param iface Network interface
 *
 * @return Port number, no such port if < 0
 */
int net_eth_get_ptp_port(struct net_if *iface);

void net_pkt_set_lladdr_dest(struct net_pkt *pkt, struct net_linkaddr *lladdr);
void net_pkt_set_lladdr_src(struct net_pkt *pkt, struct net_linkaddr *lladdr);
void net_pkt_set_lltype(struct net_pkt *pkt, uint16_t type);

static inline uint8_t *net_pkt_data(struct net_pkt *pkt)
{
	return pkt->pbuf->payload;
}

static inline size_t net_pkt_get_len(struct net_pkt *pkt)
{
	return pkt->pbuf->len;
}

static inline struct net_if *net_pkt_iface(struct net_pkt *pkt)
{
	return pkt->iface;
}

static inline struct net_ptp_time *net_pkt_timestamp(struct net_pkt *pkt)
{
	return &pkt->timestamp;
}

static inline void net_pkt_set_priority(struct net_pkt *pkt,
					uint8_t priority)
{
	pkt->priority = priority;
}

static inline void net_pkt_set_overwrite(struct net_pkt *pkt, bool overwrite)
{
	/* nothing to do here */
   (void)overwrite;
}

/**
 * @brief Increase the packet ref count
 *
 * @details Mark the packet to be used still.
 *
 * @param pkt Network packet to ref.
 *
 * @return Network packet if successful, NULL otherwise.
 */
struct net_pkt *net_pkt_ref(struct net_pkt *pkt);

/**
 * @brief Place packet back into the available packets slab
 *
 * @details Releases the packet to other use. This needs to be
 * called by application after it has finished with the packet.
 *
 * @param pkt Network packet to release.
 *
 */
void net_pkt_unref(struct net_pkt *pkt);

/**
 * @brief Allocate a network packet and buffer at once
 *
 * @param iface   The network interface the packet is supposed to go through.
 * @param size    The size of buffer.
 * @param family  The family to which the packet belongs.
 * @param proto   The IP protocol type (can be 0 for none).
 * @param timeout Maximum time to wait for an allocation.
 *
 * @return a pointer to a newly allocated net_pkt on success, NULL otherwise.
 */
struct net_pkt *net_pkt_alloc_with_buffer(struct net_if *iface,
					  size_t size,
					  unsigned short int family,
					  int proto,
					  k_timeout_t timeout);

/**
 * @brief Initialize net_pkt cursor
 *
 * @details This will initialize the net_pkt cursor from its buffer.
 *
 * @param pkt The net_pkt whose cursor is going to be initialized
 */
void net_pkt_cursor_init(struct net_pkt *pkt);

/**
 * @brief Skip some data from a net_pkt
 *
 * @details net_pkt's cursor should be properly initialized
 *          Cursor position will be updated after the operation.
 *          Depending on the value of pkt->overwrite bit, this function
 *          will affect the buffer length or not. If it's true, it will
 *          advance the cursor to the requested length. If it's false,
 *          it will do the same but if the cursor was already also at the
 *          end of existing data, it will increment the buffer length.
 *          So in this case, its behavior is just like net_pkt_write or
 *          net_pkt_memset, difference being that it will not affect the
 *          buffer content itself (which may be just garbage then).
 *
 * @param pkt    The net_pkt whose cursor will be updated to skip given
 *               amount of data from the buffer.
 * @param length Amount of data to skip in the buffer
 *
 * @return 0 in success, negative errno code otherwise.
 */
int net_pkt_skip(struct net_pkt *pkt, size_t length);

/**
 * @brief Write data into a net_pkt
 *
 * @details net_pkt's cursor should be properly initialized and,
 *          if needed, positioned using net_pkt_skip.
 *          Cursor position will be updated after the operation.
 *
 * @param pkt    The network packet where to write
 * @param data   Data to be written
 * @param length Length of the data to be written
 *
 * @return 0 on success, negative errno code otherwise.
 */
int net_pkt_write(struct net_pkt *pkt, const void *data, size_t length);


/**
 * @brief Return PTP clock that is tied to this ethernet network interface.
 *
 * @param iface Network interface
 *
 * @return Pointer to PTP clock if found, NULL if not found or if this
 * ethernet interface does not support PTP.
 */
struct device *net_eth_get_ptp_clock(struct net_if *iface);

/**
 * @brief Set gPTP port number attached to this interface.
 *
 * @param iface Network interface
 * @param port Port number to set
 */
void net_eth_set_ptp_port(struct net_if *iface, int port);



/**
 * @typedef net_if_cb_t
 * @brief Callback used while iterating over network interfaces
 *
 * @param iface Pointer to current network interface
 * @param user_data A valid pointer to user data or NULL
 */
typedef void (*net_if_cb_t)(struct net_if *iface, void *user_data);

/**
 * @brief Go through all the network interfaces and call callback
 * for each interface.
 *
 * @param cb User-supplied callback function to call
 * @param user_data User specified data
 */
void net_if_foreach(net_if_cb_t cb, void *user_data);


/**
 * @brief Unregister a timestamp callback.
 *
 * @param handle Caller specified handler for the callback.
 */
void net_if_unregister_timestamp_cb(struct net_if_timestamp_cb *handle);

/**
 * @brief Register a timestamp callback.
 *
 * @param handle Caller specified handler for the callback.
 * @param pkt Net packet for which the callback is registered. NULL for all
 * 	      packets.
 * @param iface Net interface for which the callback is. NULL for all
 *		interfaces.
 * @param cb Callback to register.
 */
void net_if_register_timestamp_cb(struct net_if_timestamp_cb *handle,
				  struct net_pkt *pkt,
				  struct net_if *iface,
				  net_if_timestamp_callback_t cb);

/**
 * @brief Queue a packet to the net interface TX queue
 *
 * @param iface Pointer to a network interface structure
 * @param pkt Pointer to a net packet to queue
 */
void net_if_queue_tx(struct net_if *iface, struct net_pkt *pkt);


extern char *net_sprint_ll_addr_buf(const uint8_t *ll, uint8_t ll_len,
				    char *buf, int buflen);



#endif