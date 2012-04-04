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
import sys


class ReconstructUDM(Module):
    """
    @ingroup Modules
    @brief Cost model for a combined sewer system    
    To caculate the cost for the pipes the model presented in 
    Factors affecting economies of scale in combined sewer systems Mauerer (2010)
    has been used

    @section
    ﻿Maurer, M., Wolfram, M., and Anja, H. (2010). Factors affecting economies of scale in 
    combined sewer systems. Water science and technology : a journal of the International 
    Association on Water Pollution Research, 62(1), 36-41.
    @author Christian Urich
    """
    def __init__(self):
        Module.__init__(self)
        self.conduits = View("CONDUIT", EDGE, READ)
        self.conduits.getAttribute("Strahler")
        self.conduits.getAttribute("CONYEAR")


        self.cityblock = View("CITYBLOCK", FACE, READ)
        self.cityblock.addAttribute("BuildYear")

        
        viewvector = []
        viewvector.append(self.conduits)
        viewvector.append(self.cityblock)  
        

        self.createParameter("MaxStrahler", INT, "Max strahler number")
        self.MaxStrahler = 2

        self.createParameter("BlockSize", DOUBLE, "Block Size")
        self.BlockSize = 500
        
        self.addData("City", viewvector)
        
        
    def run(self):
	try:

                self.PointMap = {}
		city = self.getData("City")

		names = city.getNamesOfComponentsInView(self.conduits)
		for nc in names:
                    condi = city.getEdge(nc)
                    
                    if (condi.getAttribute("Strahler").getDouble() > self.MaxStrahler):
                        continue
                    n = city.getNode(condi.getStartpointName())
                    idx = ('%.0f') % (n.getX()/self.BlockSize)
                    idy = ('%.0f') % (n.getY()/self.BlockSize)
                    id = idx+"/"+idy
                    existingPoints = []
                    if id in self.PointMap:
                        existingPoints = self.PointMap[id]
                    else:
                        self.PointMap[id] = existingPoints 
                    existingPoints.append(condi.getAttribute("CONYEAR").getDouble())
                    


		names = city.getNamesOfComponentsInView(self.cityblock)
                #print self.PointMap
		for nc in names:
                    f = city.getFace(nc)
                    f1 = Face(f)
                    nodes = f1.getNodes()
                    
                    n = city.getNode(nodes[0])
                    idx = ('%.0f') % ((n.getX()+1)/self.BlockSize+1)
                    idy = ('%.0f') % ((n.getY()+1)/self.BlockSize)
                    id = idx+"/"+idy
                    #print id
                    if id not in self.PointMap:
                        continue
		    existingPoints = self.PointMap[id]
                    print len(existingPoints)
                    minyear = -1
                    for i in range(len(existingPoints)):
                        if existingPoints[i] < 1:
                            continue
                        if (existingPoints[i] < minyear or minyear < 0):
                            minyear = existingPoints[i] 

                        #print minyear
                        f.addAttribute("BuildYear", minyear)
	except Exception, e:
	        print e
		print sys.exc_info()
	     



    
        
        

		
