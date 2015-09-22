## Sensornet ##

Small sensors often don't have the required ressources to know an entire sensor network to perform global routing protocols like the link-state routing protocol. To solve this problem it is possible to use local routing protocols. To use them it is required for each sensor to collect information about its immediate neighbourhood and possibly also about sensors that are reachable by up to k-hops from this specific sensor.

This repository contains an implementation of a local routing protocol and a k-hop neighbourhood discovery for TMote-Sky-Sensors using the Contiki OS.
The k-hop neighbourhood discovery broadcasts for each sensor periodically its currently known information network as far as (k-1)-hops.
The implemented local routing protocol is simple and sends the information to the direct neighbour, which is calculated based on the euklidian distance between the local x,y position of the sensors. The local position has to be written manually into an array before compiling the firmware currently, as the TMote-Sky-Sensors can't determind their position themselves (no GPS or similar).
