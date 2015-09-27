/**
 * \file pbroadcast.h
 * Header file for a identified broadcast Rime layer ontop of Anonymous BroadCast (abc).
 * \author tud-bp-sensornet
 */

#ifndef __PBROADCAST_H__
#define __PBROADCAST_H__

#include <stdlib.h>
#include "net/rime/rime.h"

/**
 * \def MAX_BROADCAST_PAYLOAD_SIZE
 * \brief The maximum size of the payload in one pbroadcast.
 *
 * In my experience, broadcasting will work with a payload size of up to 110 bytes (on a sky).
 * It is unclear how this limit is calculated. The following formula will give us enough leeway
 * to make sure broadcasting works, but it is not accurate. Maybe there's a good way to find
 * the maximum possible packet size.
 * The uint16_t is to account for the size of the hash of the packet.
*/
#define MAX_BROADCAST_PAYLOAD_SIZE (PACKETBUF_SIZE - PACKETBUF_HDR_SIZE - sizeof(uint16_t))

/**
 * \def PBROADCAST_ATTRIBUTES
 * \brief Pbroadcasts attributes.
*/
#define PBROADCAST_ATTRIBUTES {PACKETBUF_ADDR_SENDER, PACKETBUF_ADDRSIZE}, ABC_ATTRIBUTES

/**
 * \struct p_broadcast_conn
 * \brief Connection for broadcast including callback function.
 */
struct p_broadcast_conn
{
	struct abc_conn abc; ///< The abc_conn.
	/**
	 * \brief Function will be called on received package.
	 * \param bc A pointer to a struct p_broadcast_conn.
	 * \param sender The linkaddr_t of the sender of this data.
	 * \param data A pointer to the data part of the packetbuf.
	 * \param length The size of the data part of the packetbuf.
	 */
	void (*received)(const struct p_broadcast_conn *bc, const linkaddr_t *sender, const void *data, size_t length); ///< A pointer to a function which will be called on received package.
};

/**
 * \brief Set up an identified best-effort broadcast connection.
 * \param c A pointer to a struct p_broadcast_conn including a function.
 * The pointer to a function will be called when a packet has been received.
 * \param channel The channel on which the connection will operate.
 *
 * This function sets up a broadcast connection on the
 * specified channel. The caller must have allocated the
 * memory for the struct broadcast_conn, usually by declaring it
 * as a static variable.
 *
 * The struct p_broadcast_conn pointer must point to a structure
 * containing a pointer to a function that will be called
 * when a packet arrives on the channel.
 */
void p_broadcast_open(struct p_broadcast_conn *c, uint16_t channel);

/**
 * \brief Close a broadcast connection.
 * \param c A pointer to a struct p_broadcast_conn.
 *
 * This function closes a broadcast connection that has
 * previously been opened with p_broadcast_open().
 *
 * This function typically is called as an exit handler.
 */
void p_broadcast_close(struct p_broadcast_conn *c);

/**
 * \brief Send an identified best-effort broadcast packet.
 * \param c The broadcast connection on which the packet should be sent.
 * \param data A pointer to the data to be sent.
 * \param length The length of the data to be sent.
 * \return Non-zero if the packet could be sent, zero otherwise.
 *
 * This function sends an identified best-effort broadcast
 * packet. It will manage the packetbuf.
 *
 * The parameter c must point to a broadcast connection that
 * must have previously been set up with p_broadcast_open().
 */
int p_broadcast_send(struct p_broadcast_conn *c, const void *data, size_t length);

#endif /* __PBROADCAST_H__ */
