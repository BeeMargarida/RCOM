#!/bin/bash
#switch_config

del flash:vlan.dat
copy flash:tux5-clean startup-config <t04gFixe>
reload

copy running-config flash:<t04gFixe>

configure terminal
no vlan 50
end
configure terminal
no vlan 51
end

configure terminal
vlan 50
end
configure terminal
vlan 51
end

configure terminal
interface fastethernet 0/1
switchport mode access
switchport access vlan 50
end

configure terminal
interface fastethernet 0/4
switchport mode access
switchport access vlan 50
end

configure terminal
interface fastethernet 0/2
switchport mode access
switchport access vlan 51
end

configure terminal
interface fastethernet 0/5
switchport mode access
switchport access vlan 51
end

configure terminal
interface fastethernet 0/7
switchport mode access
switchport access vlan 51
end