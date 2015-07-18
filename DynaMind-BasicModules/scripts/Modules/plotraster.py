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
from numpy import *
from scipy import *
from matplotlib.pyplot import *
import os
import tempfile

class PlotRaster(Module):        
        def __init__(self):
                Module.__init__(self)
                self.vmin = 0
                self.vmax = 0                
                self.createParameter("RasterDataName", STRING, "" )
                self.RasterDataName = ""
                self.vec = View("dummy", SUBSYSTEM, MODIFY)
                self.offsetX = 0
                self.OffsetY = 0                
                views = []
                views.append(self.vec)
                self.addData("System", views)
                self.counter = 0
                self.createParameter("Folder", STRING, "")
                self.Folder = ""


                   
        def run(self):
                fig = figure()
                index = 1

                f = fig.add_subplot(1,1 ,1)  
                r = self.getRasterData("System",View(self.RasterDataName, RASTERDATA, READ))
                f = fig.add_subplot(1,index,1)
                f.set_title(self.RasterDataName)
                a = array([])    
                b = [] 
                nameMap = ""
                
                PlotStyle = ""
                width = r.getWidth()
                height = r.getHeight()                            
               
                val = []
                cval = array([])
                
                a.resize(height, width)
                for i in range(width):
                    for j in range(height): 
                            a[j,i] = r.getValue(i,j) * 1
                
                imshow(a, origin='lower', extent=[0,width,0,height], interpolation='nearest')  
                colorbar(ax = f,   orientation='horizontal')
                filename = "plot_" 
                filename+=str(self.counter).zfill(4)
                filename+=".png"
                savefig(str(self.Folder)+'/'+filename, dpi=720)
                #fig.show()
                close()
                self.counter+=1


