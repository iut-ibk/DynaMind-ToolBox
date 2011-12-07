import sys
sys.path.append('/home/csae6550/work/VIBe2Core/build/Release')


"""
@author  Chrisitan Urich <christian.urich@gmail.com>
@version 1.0
@section LICENSE

This file is part of VIBe2
Copyright (C) 2011  Christian Urich

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

from pydynamite import *
import pydynamite
import shapefile

class ImportShapeFile(Module):
	"""Import Shapefiles
	
	Import for Shapfiles. All data and attributes are imported. The data are saved in the Dynamite Sytem format. 
	The elements are saved with Identifier_ID
	@ingroup Modules
	@author Christian Urich
	"""
        def __init__(self):
            Module.__init__(self)
            view = pydynamite.View("Shape")
            view.addComponent(pydynamite.SUBSYSTEM)
            
            self.FileName = "/home/csae6550/Desktop/pipes"
            self.Identifier = ""
                
            views = pydynamite.viewvector()
            views.push_back(view)
            
            self.addData("Network",views)
            
            #self.addParameter(self,"FileName", pydynamite.VIBe2.FILENAME, pointer(self.FileName) ,"")
            #self.addParameter(self, "Identifier", VIBe2.STRING)

	def getClassName(self):
            return self.__class__.__name__

        def getFileName(self):
            return self.__module__.split(".")[0]
            
            
            
        def run(self):
            print "DAS IST EIN TEST############"
            vec = self.getData("Network")
            sourcePath =  self.FileName
            sf = shapefile.Reader(sourcePath)
            shaperecords = sf.shapeRecords()
            fields = sf.fields
            
            for r in shaperecords:
                shp = r.shape.points
                subsys = vec.createSubSystem("newsystem","Shape")
                points = pydynamite.nodevector()
                attr = r.record
                
                #Add all attribute at the current shape
                for index, value in enumerate(attr):
                    name = fields[index+1]
                    newattr = pydynamite.Attribute(name[0],name[0])
                    
                    if name[1]=='C':
                        newattr.setString(str(attr[index]))
                        
                    if name[1]=='N':
                        newattr.setDouble(float(attr[index]))
                        
                    subsys.addAttribute(newattr)
                
                #Add this state only save points
                for point in shp:
                    points.push_back(subsys.addNode(point[0],point[1],0,""))
                
                if r.shape.shapeType != shapefile.POINT:
                    for index, p1 in enumerate(points):
                        if(points.__len__()>index+1):
                            #Add this state no differenct between POLYLINE and POLYGON shape type
                            p2 = points[index+1]
                            subsys.addEdge(p1,p2)
                        else:
                            #If shape type is POLYGON close the current shape  
                            if r.shape.shapeType == shapefile.POLYGON:
                                p2 = points[0]
                                subsys.addEdge(p1,p2)
            
            pydynamite.log("Imported " + str(vec.getAllSubSystems().__len__()) + " shapes",pydynamite.Standard)
                
                                        
def main():
    #pydynamite.Log.init()
    pydynamite.initlog()
    test = ImportShapeFile()
    test.run()

if __name__ == "__main__":
    main()
