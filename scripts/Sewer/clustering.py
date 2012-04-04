# -*- coding: utf-8 -*- 

"""
@file
@author  Chrisitan Urich <christian.urich@gmail.com>
@version 1.0
@section LICENSE

This file is part of DynaMind
Copyright (C) 2012  Christian Urich

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
"""


from pydynamind import *
from pydmtoolbox import *
import networkx
import numpy, matplotlib
from scipy.cluster import hierarchy
from scipy.spatial import distance



class Clustering(Module):

    def __init__(self):
        Module.__init__(self)
        self.conduits = View("CONDUIT", EDGE, READ)
       
        viewvector = []
        viewvector.append(self.conduits)        
  
        
        self.addData("Sewer", viewvector)
    def run(self):
	try:
                g = networkx.Graph()
		sewer = self.getData("Sewer")
		CostsTotal = 0
		LengthTot  = 0
		names = sewer.getNamesOfComponentsInView(self.conduits)
                pointnamelist = []
		for nc in names:                    
		    c = sewer.getEdge(nc)                    
                    startNode = c.getStartpointName()
                    endNode = c.getEndpointName()
                    if startNode not in pointnamelist:
                        pointnamelist.append(startNode)
                    if endNode not in pointnamelist:
                        pointnamelist.append(endNode)
                    g.add_edge(pointnamelist.index(startNode), pointnamelist.index(endNode))
                path_length=networkx.all_pairs_shortest_path_length(g)
                n = len(g.nodes())
                distances=numpy.zeros((n,n))
                for u,p in path_length.iteritems():
                    for v,d in p.iteritems():
                                     distances[int(u)-1][int(v)-1] = d
                sd = distance.squareform(distances)

                hier = hierarchy.average(sd)		
                hierarchy.dendrogram(hier)
                matplotlib.pylab.savefig("tree.png",format="png")
                

                partition = community.best_partition(g)
                print partition

                for i in set(partition.values()):
                    print "Community", i
                    members = list_nodes = [nodes for nodes in partition.keys() if partition[nodes] == i]
                print members
	except Exception, e:
	        print e
		print "Unexpected error:"

        
        

		
