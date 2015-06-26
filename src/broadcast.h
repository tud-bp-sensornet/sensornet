#ifndef __BROADCAST_H__
#define __BROADCAST_H__

struct p_broadcast_conn;

#define BROADCAST_ATTRIBUTES  { PACKETBUF_ADDR_SENDER, PACKETBUF_ADDRSIZE }, ABC_ATTRIBUTES

/**
 * \brief			Connection for broadcast including callback function
 * \param bc		A pointer to a struct p_broadcast_conn
 * \param sender	The rimeaddr_t of the sender of this data
 * \param data		A pointer to the data part of the packetbuf
 * \param length	The size of the data part of the packetbuf
 */
struct p_broadcast_conn {
    struct abc_conn abc;

    void (*received)(const struct p_broadcast_conn *bc, const rimeaddr_t *sender, void *data, size_t length);
};

/**
 * \brief			Set up an identified best-effort broadcast connection
 * \param c			A pointer to a struct p_broadcast_conn including a function pointer to a function that will be called when a packet has been received
 * \param channel	The channel on which the connection will operate
 *
 * 					This function sets up a broadcast connection on the
 * 					specified channel. The caller must have allocated the
 * 					memory for the struct broadcast_conn, usually by declaring it
 * 					as a static variable.
 *
 * 					The struct p_broadcast_conn pointer must point to a structure
 * 					containing a pointer to a function that will be called
 * 					when a packet arrives on the channel.
 *
 */
void p_broadcast_open(struct p_broadcast_conn *c, uint16_t channel);

/**
 * \brief	Close a broadcast connection
 * \param c	A pointer to a struct p_broadcast_conn
 *
 * 			This function closes a broadcast connection that has
 * 			previously been opened with p_broadcast_open().
 *
 * 			This function typically is called as an exit handler.
 *
 */
void p_broadcast_close(struct p_broadcast_conn *c);

/**
 * \brief			Send an identified best-effort broadcast packet
 * \param c			The broadcast connection on which the packet should be sent
 * \param data		A pointer to the data to be sent
 * \param length	The length of the data to be sent
 * \retval			Non-zero if the packet could be sent, zero otherwise
 *
 * 					This function sends an identified best-effort broadcast
 * 					packet. It will manage the packetbuf.
 *
 * 					The parameter c must point to a broadcast connection that
 * 					must have previously been set up with p_broadcast_open().
 *
 */
int p_broadcast_send(struct p_broadcast_conn *c, void *data, size_t length);

#endif /* __BROADCAST_H__ */
