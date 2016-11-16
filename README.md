# LYT8266_UDP
LYT8266 lamp with UDP autodiscovery and "instant" on

This Lyt8266 sketch tries to power on the light at the specified white value as
soon as possible, connects to your wifi AP and starts sending broadcast packages
detailing its IP and listening port so you can use the accompanying python 
script to discover the lamps and add them to you app / dashboard etc.

Switchting the lamp to on still takes ~2 seconds somehow.
