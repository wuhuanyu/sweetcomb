#!/bin/bash

ip link del vppout1
ip link del vppout2
ip link del vpp1vpp2

ip netns del host1
ip netns del host2

systemctl stop vpp

pgrep vpp|xargs kill -9
