import networkx as nx
import fnss
import os
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from mininet.node import OVSController

G = fnss.Topology(type='2to1')
G.name = "2to1_topology"

G.add_node('h1', type='left')
G.add_node('h2', type='left')
G.add_node('ovs', type='central')
G.add_node('s1', type='right')

G.add_edge('h1', 'ovs', type='left')
G.add_edge('h2', 'ovs', type='left')
G.add_edge('ovs', 's1', type='right')

hosts = ['h1', 'h2', 's1']
switches = ['ovs']

mn_topo = fnss.to_mininet(G, switches=switches,
                         hosts=hosts, relabel_nodes=True)

net = Mininet(topo=mn_topo, link=TCLink, controller=OVSController,xterms=True)
net.start()
net.xterms()

net.stop()
