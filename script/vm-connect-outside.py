#!/usr/bin/python

"""
This example shows how to create an empty Mininet object
(without a topology object) and add nodes to it manually.
"""
import subprocess
import os
from mininet.net import Mininet
from mininet.node import Controller, OVSController
from mininet.link import TCLink
from mininet.cli import CLI
from mininet.log import setLogLevel, info

class server():
    def __init__(self, dst_ip, dst_port):
        self.dst_ip = dst_ip
        self.dst_port = dst_port

class client():
    def __init__(self, src_ip, src_port):
        self.src_ip = src_ip
        self.src_port = src_port

def GoOutsideNet():

    "Create an network connecting outside and add nodes to it."

    net = Mininet(link=TCLink, controller=OVSController,
                  autoSetMacs=True, xterms=True)

    info('*** Adding controller\n')
    net.addController('c0')

    info('*** Adding hosts\n')
    h1 = net.addHost('h1', ip='192.168.2.111')
    h2 = net.addHost('h2', ip='192.168.2.112')

    info( '*** Adding switch\n' )
    s1 = net.addSwitch( 's1' )

    info( '*** Creating links\n' )
    net.addLink(h1, s1)
    net.addLink(h2, s1)

    info( '*** Starting network\n')
    net.start()

    info('***connect outside ***\n')
    connect_outside()

    info('** add some qdisc **\n')
    add_htb('eno2')

    info('** add flow entries ** \n')
    add_flow_entry()

    info('** exec some command in net namespace **\n')
    h5 = server("192.168.2.5", "12345")

    exec_cmd(h1, h5)

    info( '*** Running CLI\n' )
    CLI( net )

    info('*** exit the mininet **\n')
    exit_mininet()

    info( '*** Stopping network' )
    net.stop()

    info('** exec mn -c **\n')
    os.popen('mn -c')

def connect_outside():
    # info('*** add route ***\n')
    # route_ret_code = os.popen('ip route add 192.168.2.0/24 via 192.168.2.110 dev eno2')
    info('*** add if eno2 to ovs s1 ***\n')
    attach_ret_code = os.popen('ovs-vsctl add-port s1 eno2')

def add_htb(interface):
    info('** add qdisc to interface **\n')
    qos_ret_code = os.popen('ovs-vsctl set port %s qos=@newqos -- \
                            --id=@newqos create qos type=linux-htb \
                                    other-config:max-rate=1000000000 \
                                    queues:1=@p0queue \
                                    queues:2=@p1queue -- \
                            --id=@p0queue create queue other-config:min-rate=200000000 -- \
                            --id=@p1queue create queue other-config:min-rate=800000000'%interface)
    info('** add some qdisc to ovs interface **\n')
    os.popen('ethtool -K s1-eth1 rx off tx off tso off gso off')

def add_flow_entry():
    info('** enqueue some packets to corresponding queues **\n')
    os.popen('ovs-ofctl add-flow s1 in_port=1,ip,nw_src=192.168.2.111,priority=2,actions=set_queue:2,normal')
    os.popen('ovs-ofctl add-flow s1 in_port=3,ip,nw_dst=192.168.2.111,priority=2,actions=output:1')
    os.popen('ovs-ofctl add-flow s1 in_port=2,ip,nw_src=192.168.2.112,priority=2,actions=set_queue:1,normal')
    os.popen('ovs-ofctl add-flow s1 in_port=3,ip,nw_dst=192.168.2.112,priority=2,actions=output:2')
    os.popen('ovs-ofctl add-flow s1 arp,actions=output:all')

def exec_cmd(host, server):
    info('** exec the iperf command **\n')
    host.popen('iperf -c '+ server.dst_ip + ' -p '+server.dst_port +' -t 60 -i 1')
    info('** turn off the tso and gso func ** \n')
    host.popen('ethtool -K ' + host.name + '-eth0' + ' rx off tx off tso off gso off')

def exit_mininet():
    # info('*** del route  ***\n')
    # route_ret_code = os.popen('ip route delete 192.168.2.0/24 via 192.168.2.110 dev eno2')
    info('** del the qos and queue in ovs **\n')
    clear_qos_ret_code = os.popen('ovs-vsctl clear port eno2 qos ')
    info('** del the qos in ovs **\n')
    del_qos_ret_code = os.popen('ovs-vsctl --all destroy qos')
    info('** del the queue in ovs **\n')
    del_queue_ret_code = os.popen('ovs-vsctl --all destroy queue')
    info('*** del if eno2 to ovs s1 ***\n')
    attach_ret_code = os.popen('ovs-vsctl del-port s1 eno2')


if __name__ == '__main__':
    setLogLevel( 'info' )
    GoOutsideNet()
