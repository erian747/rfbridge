#!/bin/bash
echo Setting up SLIP interface

sudo home/erian747/tunslip -s /dev/ttyUSB0 -t sltun0 192.168.4.1 255.255.255.0 &
sudo ifconfig sltun0 192.168.4.1 pointopoint 192.168.4.2
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
sudo iptables -A FORWARD -i sltun0 -o eth0 -j ACCEPT
