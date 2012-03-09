"""
@file
@author  Chrisitan Urich <christian.urich@gmail.com>
@version 1.0
@section LICENSE

This file is part of DynaMind
Copyright (C) 2011-2012  Christian Urich

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

import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.lines as lines
import pylab
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
from matplotlib.collections import LineCollection
import numpy as np
import numpy as np

import os
import tempfile


class PlotVectorData(Module):       
    """Plots VectorData
    @ingroup Modules
    @author Christian Urich
    """
    def __init__(self):
        Module.__init__(self)            
        self.view = View("CONDUIT", EDGE, READ)
        self.views = []
        self.views.append(self.view)
        self.addData("Vec", self.views)
            
                

    def run(self):
        print "HUHUPLOT"
        city = self.getData("Vec")
        for year in range(1941, 2011, 5):
            patches = []
            linescol = []
            linescolWithC = []
            linewidths = []
            linecolors = []
            print "Run Plot"
            fig = plt.figure()
            fig.set_size_inches(46.8/4, 33.1/4)
            ax = fig.add_subplot(111,  aspect='equal')
            index = 0
            for i in range(len(self.views)):
                view = self.views[i]
                
                Type = "Edge"
                    
                #find LayerNames
                names = []
                names =  city.getNamesOfComponentsInView(view)
    
    
                for n in names:  
                    if str(Type) == "Edge":
                        edge = city.getEdge(n)
                        p1 = city.getNode(edge.getStartpointName())
                        p2 = city.getNode(edge.getEndpointName())
                        if index == 0:
                            xmax = p1.getX()
                            xmin = p1.getX()
                            ymax = p1.getY()
                            ymin = p1.getY() 
                            index+=1
                    if str(Type) == "Edge":
                        edge = city.getEdge(n)  
                        poly = []
                        p1 = city.getNode(edge.getStartpointName())
                        p2 = city.getNode(edge.getEndpointName())
                        if edge.getAttribute("PLAN_DATE").getDouble() < 1900:
                            continue
                        if edge.getAttribute("PLAN_DATE").getDouble() > year:
                            continue
                        poly.append([p1.getX(), p1.getY()])
                        poly.append([p1.getX(), p2.getY()])
    
                        x1 = p1.getX()
                        x2 = p2.getX()
                        x = [x1, x2]
    
                        y1 = p1.getY()
                        y2 = p2.getY()
                        y = [y1, y2]    
    
                        xmax = max( x1, x2, xmax )
                        ymax = max( y1, y2, ymax )
                        xmin = min( x1, x2, xmin )                    
                        ymin = min( y1, y2, ymin )                        
                                
                        definedStyle = False
                                
                                        
                        if definedStyle == False:
                            line = lines.Line2D(x,y)                                
                            linescol.append(((x[0], y[0]),(x[1], y[1])) )
                            #ax.add_line(line)
    
       
            if(len(patches) > 0):
                p = PatchCollection(patches, True)
                ax.add_collection(p)  
            if(len(linescol) > 0):
                pl = LineCollection(linescol)
                ax.add_collection(pl)
            if(len(linescolWithC) > 0):
                pl = LineCollection(linescolWithC, linewidths, linecolors)
                ax.add_collection(pl)      
            '''
            if self.xmin != -1 and self.xmax != -1:
                xmin = self.xmin 
                xmax = self.xmax
            if self.ymin != -1 and self.ymax != -1:
                ymin = self.ymin 
                ymax = self.ymax
            '''
            print xmax
            print ymax
            plt.xlim(xmin, xmax)
            plt.ylim(ymin, ymax)
            '''   
            if ( self.SaveToFile == "true"):
                filename = "plot" 
                filename+=str(self.getT())+"_"+str(self.getID())
                #filename+=".svg"
                #plt.savefig(filename+".svg")
                plt.savefig(filename+".png")
                #plt.savefig(filename+".eps")
                plt.savefig(filename+".pdf")
                #plt.savefig(filename+".emf")
                
            if ( self.ShowFigure == "true"):
                plt.show()
            else:
                plt.close()
            '''
            filename = "plot" 
            filename+= str(year) 
            filename+=".png"
            plt.savefig(tempfile.gettempdir()+'/'+filename)
            plt.close()
            self.sendImageToResultViewer(tempfile.gettempdir()+'/'+filename)   
