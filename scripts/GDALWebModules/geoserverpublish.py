import sys
sys.path.append("../")
from geoserver.catalog import Catalog, FailedRequestError
from geoserver.catalog import Catalog
from string import Template
import subprocess
import geoserver.util
import sys
import ast
import re #regular expression
import os.path
import psycopg2

if os.path.isfile('../config.py'):
    from config import *
else:
    GEOSERVERURL = "http://localhost:8080/geoserver/" 
    GEOSERVERUSERNAME = 'admin'
    GEOSERVERPW = 'iut07030201'
    POSTGREPW ='iut0703'
    POSTGREUSERNAME='postgres'
    POSTGREIP='localhost'

class GeoserverHelper:
    def __init__(self, geoserverUrl = "", geoserverUserName = "", geoserverPW = "", geoserverWorkSpace = "",
        postgreIP = "", postgreUserName = "", postgrePW = ""):

        """use the constructor given arguments if used"""
        self.geoserverUrl = geoserverUrl if geoserverUrl != "" else GEOSERVERURL
        self.geoserverUserName = geoserverUserName if geoserverUserName != "" else GEOSERVERUSERNAME
        self.geoserverPW = geoserverPW if geoserverPW != "" else GEOSERVERPW
        self.geoserverWorkSpace = geoserverWorkSpace if geoserverWorkSpace != "" else "crc"
        self.postgreIP = postgreIP if postgreIP != "" else POSTGREIP
        self.postgreUserName = postgreUserName if postgreUserName != "" else POSTGREUSERNAME
        self.postgrePW = postgrePW if postgrePW != "" else POSTGREPW

        if self.geoserverUrl[-1] != '/':
            raise Exception("GeoserverUrl must end with a slash ('/')")

        self.catalog = Catalog(self.geoserverUrl+"rest/")
        self.catalog.http.add_credentials(self.geoserverUserName,self.geoserverPW)
        try:
            workspaces = self.catalog.get_workspaces()
        except:
            e = sys.exc_info()[0]
            print e
        self.cWorkSpace = self.catalog.get_workspace(self.geoserverWorkSpace)

    def getLayers(self):
        return self.cWorkSpace.catalog.get_layers()

    def insertShapeIntoPostGis(self, shapeFile, databaseName, tableName, encoding=3857):
        '''returns the returnCode of the execution of the insert script, e.g:
        helper.insertShapeIntoPostGis('/home/c815/gsTest/test.shp','crc','testingHelper2')'''

        if not os.path.isfile(shapeFile):
            print "Shape file not found"
            return -1
            cmds = "PGPASSWORD={pgPW} ./createWSFTFromSHP.sh -s {shapeFileF} -d {databaseNameF} -t {tableNameF} -u {postgreUsername} -i {postgreIP}".format(pgPW=self.postgrePW,
                shapeFileF=shapeFile,databaseNameF=databaseName, tableNameF=tableName, postgreUsername=self.postgreUserName, postgreIP=self.postgreIP)
            return subprocess.call(cmds, shell=True)

    def uploadShapeFile(self, shapeFile, storeName):
        shpPlusSidcars = geoserver.util.shapefile_and_friends(shapeFile[:-3])
        shpPlusSidcars
        self.ft = self.catalog.create_featurestore(storeName, shpPlusSidcars, self.cWorkSpace)

    def getStyles(self):
        return self.catalog.get_styles()

    def uploadStyleFile(self, sldFile, styleName, overWrite, workSpace = None):
        f = open(sldFile,'r')
        styleSrc = f.read()
        uploadStyle(styleSrc, styleName, overWrite, workSpace)
        f.close()

    def uploadStyle(self, sldSrc, styleName, overWrite, workSpace = None):
        self.catalog.create_style(styleName,sldSrc, overWrite, workSpace)

    def publishPostGISLayer(self,postGISLayerName, storeName, crs='EPSG:3857'):
        '''cat.publish_featuretype('testingstuff',crcStore,native_crs='EPSG:3857')'''

        store = self.catalog.get_store(storeName)
        if store != None:
            self.catalog.publish_featuretype(postGISLayerName, store, crs)

    def setDefaultStyleForLayer(self, layerName, styleName):
        l = self.catalog.get_layer(layerName)

        sNames = [ i.name for i in self.getStyles() ]
        if styleName not in sNames:
            split = styleName.split(':')
            if len(split) == 2:
                workSpace = styleName.split(':')[0]
                newStyleName = styleName.split(':')[1]
            else:
                return -1
            style = self.catalog.get_style(newStyleName, workSpace)
            if style is None:
                return -1
            if l != None:
                l._set_default_style(styleName)
                self.catalog.save(l)
                return 0

    def createPostGISDataStore(self, storeName, postGisPassword, postGisUser, postGisHost,
        postGisDatabase, workSpace = None):

        #check if connection parameter are valid
        try:
            conn = psycopg2.connect("dbname='{dbName}' user='{dbUser}' host='{Host}' password='{password}'".format(dbName=postGisDatabase,
                                                                                                                   dbUser=postGisUser,
                                                                                                                   Host=postGisHost,
                                                                                                                   password=postGisPassword))
        except:
            return False

        w = self.catalog.create_datastore(storeName, workSpace)
        template = Template("""{'validate connections': 'true', 'port': '5432', 
        'Support on the fly geometry simplification': 'true', 
        'create database': 'false', 'dbtype': 'postgis', 
        'Connection timeout': '20', 'namespace': 'http://www.crcproject.com', 
        'Max connection idle time': '300', 'Expose primary keys': 'false', 
        'min connections': '1', 'Max open prepared statements':'50', 
        'passwd': '$passwd', 
        'encode functions': 'false', 
        'max connections': '10', 'Evictor tests per run': '3', 'Loose bbox': 'true', 
        'Evictor run periodicity': '300', 'Estimated extends': 'true', 
        'database': '$database', 
        'fetch size': '1000', 'Test while idle': 'true', 
        'host': '$host', 
        'preparedStatements': 'false', 'schema': 'public', 
        'user': '$user'}""")
        dic = ast.literal_eval(template.substitute(
            passwd=postGisPassword,
            user=postGisUser,
            host=postGisHost,
            database=postGisDatabase))
        #'passwd': 'crypt1:Bsaz2AUI8T+6Pj43krA7kg==', 
        #'user': 'postgres'}
        #'database': 'crc', 
        #'host': 'localhost', 
        w.connection_parameters = dic
        self.catalog.save(w)
        return True

import unittest

class TestGeoserverHelper(unittest.TestCase):
    def setUp(self):
        self.gh = GeoserverHelper("http://web01-c815.uibk.ac.at:8080/geoserver/")

    def createWorkSpace(self):
        self.gh.catalog.create_workspace('testWS','http://web01-c815.uibk.ac.at:8080/geoserver/rest/workspaces/testWS.xml')
        ws = self.gh.catalog.get_workspace('testWS')
        assert ws is not None

    def test_uploadStyle(self):
        #self.uploadStyle()
        sldSrc = """<?xml version="1.0" encoding="UTF-8"?><sld:StyledLayerDescriptor xmlns="http://www.opengis.net/sld" xmlns:sld="http://www.opengis.net/sld" xmlns:ogc="http://www.opengis.net/ogc" xmlns:gml="http://www.opengis.net/gml" version="1.0.0">
  <sld:NamedLayer>
    <sld:Name>test</sld:Name>
    <sld:UserStyle>
      <sld:Name>test</sld:Name>
      <sld:FeatureTypeStyle>
        <sld:Name>name</sld:Name>
        <sld:Rule>
          <sld:Name>Single symbol</sld:Name>
          <sld:LineSymbolizer>
            <sld:Stroke>
              <sld:GraphicStroke>
                <sld:Graphic>
                  <sld:Mark>
                    <sld:WellKnownName>circle</sld:WellKnownName>
                    <sld:Fill/>
                    <sld:Stroke/>
                  </sld:Mark>
                  <sld:Size>2</sld:Size>
                </sld:Graphic>
              </sld:GraphicStroke>
            </sld:Stroke>
          </sld:LineSymbolizer>
        </sld:Rule>
      </sld:FeatureTypeStyle>
    </sld:UserStyle>
  </sld:NamedLayer>
</sld:StyledLayerDescriptor>
        """
        self.gh.uploadStyle(sldSrc,'testStyle2',True, 'crc')
        s = self.gh.catalog.get_style('testStyle2','crc')
        assert s is not None
        self.gh.catalog.delete(s)
        s = self.gh.catalog.get_style('testStyle2','crc')
        assert s is None

    def deleteWorkSpace(self):
        ws = self.gh.catalog.get_workspace('testWS')
        assert ws is not None
        self.gh.catalog.delete(ws)
        ws = self.gh.catalog.get_workspace('testWS')
        assert ws is None

    def createPostGisDataStore(self):
        self.gh.createPostGISDataStore('test4', 'iut0703', 'postgres','localhost', 'test', 'crc')

    def publishPostGISLayer(self):
        self.gh.publishPostGISLayer('test', 'test4')

    def deletelayer(self):
        l = self.gh.catalog.get_layer('test')
        assert l is not None
        self.gh.catalog.delete(l)
        l = self.gh.catalog.get_layer('test')
        assert l is None

    def deletePostGisDataStore(self):
        s = self.gh.catalog.get_store('test4')
        assert s is not None
        r = s.get_resources()
        for i in r:
            self.gh.catalog.delete(i)
        self.gh.catalog.delete(s)

    def test_workSpace(self):
        self.createWorkSpace()
        self.deleteWorkSpace()

    def test_createPostGisDataStore(self):
        self.createPostGisDataStore()
        self.publishPostGISLayer()
        self.deletelayer()
        self.deletePostGisDataStore()

if __name__ == '__main__':
    unittest.main()
    #import doctest
    #doctest.testmod(extraglobs={'gh': GeoserverHelper("http://web01-c815.uibk.ac.at:8080/geoserver/")})