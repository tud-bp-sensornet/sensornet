#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

void serialize(void (*packet_complete)(const void *packet_data, size_t length));

// updates graph with info from packet
void deserialize(const rimeaddr_t *sender, void *packet, size_t length);

#endif
