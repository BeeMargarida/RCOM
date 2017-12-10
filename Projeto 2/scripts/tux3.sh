#!/bin/bash
#tux3_config

ifconfig eth0 down

ifconfig eth0 up
ifconfig eth0 172.16.50.1/24
arp -d 172.16.50.1
arp -d 172.16.50.254

route add default gw 172.16.50.254