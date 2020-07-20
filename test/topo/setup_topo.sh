#!/usr/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
vppctl=`which vppctl`
vpp=`which vpp`
systemctl stop vpp

pgrep "vpp"|xargs kill -9

conf1="$DIR/vpp1.conf"
conf2="$DIR/vpp2.conf"
$vpp -c $conf1
$vpp -c $conf2

sock1="/run/vpp/cli-vpp1.sock"
sock2="/run/vpp/cli-vpp2.sock"

vppctl2="${vppctl} -s ${sock2}"
vppctl1="${vppctl} -s ${sock1}"



sleep 1

ip link del vppout1
ip link del vppout2
ip link del vpp1vpp2

ip netns del host1
ip netns del host2

ip netns add host1
ip netns add host2

ip netns exec host1 ip link set lo up
ip netns exec host2 ip link set lo up 

ip link add vppout1 type veth peer name vpphost1
ip link set vppout1 up
ip link set vpphost1 up 
ip link set vpphost1 netns host1
ip netns exec host1 ip link set vpphost1 up 
ip netns exec host1 ip addr add 10.0.1.1/24 dev vpphost1 
ip netns exec host1 ip route add 10.0.3.0/24 via 10.0.1.2


ip link add vppout2 type veth peer name vpphost2
ip link set vppout2 up 
ip link set vpphost2 up 
ip link set vpphost2 netns host2
ip netns exec host2 ip link set vpphost2 up
ip netns exec host2 ip addr add 10.0.3.1/24 dev vpphost2
ip netns exec host2 ip route add 10.0.1.0/24 via 10.0.3.2


$vppctl1 create host-interface name vppout1 
$vppctl1 set int state host-vppout1 up
$vppctl1 set int ip address host-vppout1 10.0.1.2/24

$vppctl2 create host-interface name vppout2
$vppctl2 set int state host-vppout2 up
$vppctl2 set int ip address host-vppout2 10.0.3.2/24

ip link add vpp1vpp2 type veth peer vpp2vpp1 
ip link set vpp1vpp2 up 
ip link set vpp2vpp1 up 

$vppctl1 create host-interface name vpp1vpp2 
$vppctl1 set int state host-vpp1vpp2 up 
$vppctl1 set int ip address host-vpp1vpp2 10.0.2.1/24
#to be configured by sweetcomb
#$vppctl1 ip route add 10.0.3.0/24 via 10.0.2.2

$vppctl2 create host-interface name vpp2vpp1
$vppctl2 set int state host-vpp2vpp1 up
$vppctl2 set int ip address host-vpp2vpp1 10.0.2.2/24
$vppctl2 ip route add 10.0.1.0/24 via 10.0.2.1




