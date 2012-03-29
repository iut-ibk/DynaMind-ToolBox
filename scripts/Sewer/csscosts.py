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



class CSSCosts(Module):
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
        self.conduits.getAttribute("Diameter")
        self.conduits.getAttribute("Length")
        self.conduits.getAttribute("Strahler")
        self.conduits.addAttribute("ConstructionCost")
        self.conduits.addAttribute("ConstructionCostPerMeter")
        
        self.inlets = View("JUNCTION", NODE, READ)
        self.inlets.getAttribute("D")

        self.storage1 = View("STORAGE", NODE, READ)
        self.storage1.getAttribute("StorageV")
        self.storage1.addAttribute("ConstructionCost")
        self.storage1.addAttribute("ConstructionCostPerCubicMeter")
	self.storage1.addAttribute("Storages")

        self.globals = View("GLOBALS_SEWER", NODE, READ)
        self.globals.addAttribute("TotalConstructionCost")
        self.globals.addAttribute("TotalConstructionStorage")
        
        viewvector = []
        viewvector.append(self.conduits)
        viewvector.append(self.inlets)  
        viewvector.append(self.storage1) 
        viewvector.append(self.globals)  
        
        

        #diameter-related coefficients
        self.createParameter("m_alpha", DOUBLE, "diameter-related coefficients")
        self.m_alpha = 0.11
        self.createParameter("m_beta", DOUBLE, "diameter-related coefficients")
        self.m_beta = 1.2

        #diameter-independet cost coefficients
        self.createParameter("n_alpha", DOUBLE, "diameter-independet cost coefficients")
        self.n_alpha = 127.
        self.createParameter("n_beta", DOUBLE, "diameter-independet cost coefficients")
        self.n_beta = -35.

        #reduction factors considering the reduced costs for green-field construction compared
        #with under street construction
        self.createParameter("f_SF1", DOUBLE, "reduction factors considering the reduced costs for green-field construction compared with under street construction")
        self.f_SF1 = 0.846
        self.createParameter("f_SF2", DOUBLE, "reduction factors considering the reduced costs for green-field construction compared with under street construction")
        self.f_SF2 = 8.196

        #maximum depth for open trench construction
        self.createParameter("D_b", DOUBLE, "maximum depth for open trench construction")
        self.D_b = 7.0

        #reduction factor taking differen construction methods below the Db into account
        self.createParameter("f_Db", DOUBLE, "reduction factor taking differen construction methods below the Db into account")
        self.f_Db = 0.0723
        
        self.addData("Sewer", viewvector)
    def run(self):
	try:
		sewer = self.getData("Sewer")
		CostsTotal = 0
		LengthTot  = 0
		names = sewer.getNamesOfComponentsInView(self.conduits)
		for nc in names:
		    c = sewer.getEdge(nc)
		    if c.getAttribute("Strahler").getDouble() < 1.9:
		        continue            
		    DN = c.getAttribute("Diameter").getDouble()
		    startNode = sewer.getNode(c.getStartpointName())
		    D = startNode.getAttribute("D").getDouble()
		    endNode = sewer.getNode(c.getEndpointName())
		    if endNode.getAttribute("D").getDouble() > D:
		        D = endNode.getAttribute("D").getDouble()            
		    #Add Diameter to D. the layout generation needs to be changed first
		    D = D+DN/1000.
		    CostPerMeter = self.calculateConstructionCostPerMeter(DN, D)            
		    length =  c.getAttribute("Length").getDouble()            
		    Costs = CostPerMeter*length
		    print Costs
		    LengthTot += length
		    c.addAttribute("ConstructionCost", Costs)
		    c.addAttribute("ConstructionCostPerMeter", CostPerMeter)
		    CostsTotal += Costs

		names = sewer.getNamesOfComponentsInView(self.storage1)
		StorageTotal = 0
		StorageV = 0

		for nc in names:
		    c = sewer.getNode(nc)
		    V = c.getAttribute("StorageV").getDouble()
		    c.addAttribute("ConstructionCost", self.calculateStorageCosts(V)*V)
		    c.addAttribute("ConstructionCostPerCubicMeter", self.calculateStorageCosts(V)) 
		    Costs = self.calculateStorageCosts(V)*V
		    StorageTotal += Costs
		    StorageV += V
	
		Storages = len(names)
		names = sewer.getNamesOfComponentsInView(self.globals)
		for nc in names:
		    c = sewer.getNode(nc)
		    c.addAttribute("TotalConstructionCost", CostsTotal)
		    c.addAttribute("TotalStorageCost", StorageTotal)
		    c.addAttribute("TotalLength", LengthTot)
		    c.addAttribute("TotalStorageV", StorageV)
		    c.addAttribute("Storages", Storages)
	except Exception, e:
	        print e
		print "Unexpected error:"
	     


    def calculateStorageCosts(self, V):
        """Calulcate storage costs
        V -- Diameter of the pipe in (m3)
        """
        if V < 100:
            return 1565
        if V >= 100 and V < 2000:
            return 10822*V**(-0.42)
        if V >= 2000:
            return 445

    def calculateConstructionCostPerMeter(self, DN, D):
        """Calulcate construction cost per meter
        DN -- Diameter of the pipe in (mm)
        D  -- Construction depth in (m) 
        """
        #depth-related cost coefficients as a function of pipe diameter US$m-2
        alpha = self.m_alpha * DN + self.n_alpha
        print DN
        beta = self.f_SF1 * self.m_beta * DN + self.f_SF2 * self.n_beta
        print alpha
        print beta
        #Open Trench Construction
        if D <= self.D_b:
            return alpha * D + beta
        #No Open Trench Construction
        else:
            return self.f_Db * alpha * D + beta + (1 - self.f_Db) * alpha * self.D_b

    


    
        
        

		
