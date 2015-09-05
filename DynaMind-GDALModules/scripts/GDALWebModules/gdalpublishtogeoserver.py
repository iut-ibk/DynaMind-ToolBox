"""
@file
@author  Martin Schoepf <martin.schoepf@gmail.com>
@version 1.0
@section LICENSE

This file is part of DynaMind
Copyright (C) 2014  Martin Schoepf

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

from osgeo import ogr
from pydynamind import *
from geoserverpublish import *


class GDALPublishPostgisLayerInGeoserver(Module):
    display_name = "Publish PostGis to Geoserver"
    group_name = "Data Import and Export"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)
        
        ### Postgis configuration
        self.createParameter("postGisUrl", STRING, "Url of the PostGis")
        self.postGisUrl = "localhost"

        self.createParameter("postGisUsername", STRING, "Url of the PostGis")
        self.postGisUsername = "postgres"

        self.createParameter("postGisPassword", STRING, "Url of the PostGis")
        self.postGisPassword = "iut0703"

        self.createParameter("postGisDB", STRING, "Db to use")
        self.postGisDB="test"

        self.createParameter("postGisTable", STRING, "Table to be used inside the DB")
        self.postGisTable="test"


        ### Geoserver configuration
        self.createParameter("geoserverUrl", STRING, "Url of the geoserver")
        self.geoserverUrl = "http://web01-c815.uibk.ac.at:8080/geoserver/"

        self.createParameter("geoserverUserName", STRING, "User for the geoserver")
        self.geoserverUserName = "admin"

        self.createParameter("geoserverPassword", STRING, "Password for the supplied geoserver use")
        self.geoserverPassword = "iut07030201"

        self.createParameter("workspace", STRING, "Workspace to be used in the geoserver")
        self.workspace="crc"

        self.createParameter("geoserverDataStoreName", STRING, "Name of the Datastore to be used in the geoserver")
        self.geoserverDataStoreName="postgis"

        self.createParameter("EPSG", STRING, "EPSG code used for layer publish")
        self.EPSG='EPSG:3857'

        ### Use style to publish layer
        self.createParameter("styleUsed", STRING, "Style to be used to publish the Layer. \
            If the style is in a workspace use the following syntax: workspace:styleName ")
        self.styleUsed='standard'        

        
        self.dummy = ViewContainer("city", COMPONENT, MODIFY)
        self.registerViewContainers([self.dummy])
        
        self.createParameter("step", INT)
        self.step = 1 #export every x step
        
    
    def init(self):
        # self.__postgis = ViewContainer("__postgisSettings", COMPONENT, READ)
        # self.__postgis.addAttribute("url", Attribute.STRING, READ)
        # self.__postgis.addAttribute("db", Attribute.STRING, READ)
        # self.__postgis.addAttribute("table", Attribute.STRING, READ)

        # views = []
        # views.append(self.__postgis)

        # self.registerViewContainers(views)

        self.geoHelper = GeoserverHelper(self.geoserverUrl,self.geoserverUserName,self.geoserverPassword,
                                         self.geoserverWorkSpace)

    def run(self):
        #only exort on x step
        if self.get_group_counter() != -1 and (self.get_group_counter()  % self.step != 0):
            return

        #indication of the postgis-connection is valid
        ret = True
        try:
            ret = self.geoHelper.createPostGISDataStore(self.geoserverDataStoreName, self.postGisPassword,
                                              self.postGisUsername, self.postGisUrl, self.postGisDB,
                                              self.workspace)
        except FailedRequestError as e:
            log(str(e),Warning)

        if ret == False:
            log("Could not connect to PostGIS server with the supplied parameters.",Error)
            self.setStatus(MOD_EXECUTION_ERROR)
            return

        try:
            #check for counter:  "_" + str(self.get_group_counter() for postGisTable #if self.counter is set
            if self.get_group_counter() != -1:
                self.geoHelper.publishPostGISLayer(self.postGisTable+"_"+str(self.get_group_counter()), self.geoserverDataStoreName, self.EPSG)
            else:
                self.geoHelper.publishPostGISLayer(self.postGisTable, self.geoserverDataStoreName, self.EPSG)
        except FailedRequestError as e:
            log(str(e),Error)
            self.setStatus(MOD_EXECUTION_ERROR)
            return

        if self.styleUsed != 'standard':
            ret = self.geoHelper.setDefaultStyleForLayer(self.postGisTable, self.styleUsed)
            if ret != 0:
                log("Could not set the supplied style, standard style used.",Warning)
        return
