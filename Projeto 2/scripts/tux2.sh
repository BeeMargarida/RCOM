#!/bin/bash
#tux4_config

ifconfig eth0 down
ifconfig eth1 down

ifconfig eth0 up
ifconfig eth0 172.16.50.254/24

ifconfig eth1 up
ifconfig eth1 172.16.51.253/24

arp -d 172.16.50.1
arp -d 172.16.50.254
arp -d 172.16.51.1
arp -d 172.16.51.253

route add defaut gw 172.16.51.254