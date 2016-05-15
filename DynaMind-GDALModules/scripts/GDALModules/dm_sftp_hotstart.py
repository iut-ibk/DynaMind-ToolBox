from pydynamind import *


import paramiko
import time
from osgeo import ogr
import uuid
import os


class DM_Hoststart_SFTP(Module):

        display_name = "Hotstart Simulation from SFTP"
        group_name = "Data Import and Export"


        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("username", STRING)
            self.username = ""

            self.createParameter("password", STRING)
            self.password = ""

            self.createParameter("port", INT)
            self.port = 22

            self.createParameter("host", STRING)
            self.host = ""


            # self.createParameter("folder_name", STRING)
            # self.folder_name = ""

            self.createParameter("file_name", STRING)
            self.file_name = ""

            self.createParameter("step", INT)
            self.step = 1 #export every x step

            self.transport = None
            self.sftp = None

            self.downloaded = False

            self.real_file_name = ""

        def init(self):

            if self.downloaded:
                return

            if not self.connect():
                return

            print "get file"

            file_name = self.get_file(self.file_name)

            self.close()

            self.downloaded = True

            ds = ogr.Open(file_name)

            if ds is None:
                print "Open failed.\n"
                return

            lyr = ds.GetLayerByName("dynamind_table_definitions")
            lyr.ResetReading()

            views = {}
            view_type = {}
            for feat in lyr:
                view_name = feat.GetFieldAsString("view_name")
                attribute_name = feat.GetFieldAsString("attribute_name")
                datatype =  feat.GetFieldAsString("data_type")

                if attribute_name == "DEFINITION":
                    view_type[view_name] = datatype
                    continue
                if view_name not in views.keys():
                    views[view_name] = {}

                views[view_name][attribute_name] = datatype

            self.view_containers = []
            for view in view_type.keys():
                v = ViewContainer(view, self.StringToDMDataType(view_type[view]), WRITE)
                self.view_containers.append(v)

                if  view not in views.keys():
                    continue

                for attribute in views[view].keys():
                    v.addAttribute(attribute, self.StringToDMDataType(views[view][attribute]), WRITE )


            ds = None
            self.registerViewContainers(self.view_containers)


        def StringToDMDataType(self, type):

            if type ==  "COMPONENT":
                return COMPONENT

            if type == "NODE":

                return NODE

            if type == "EDGE":
                return EDGE

            if type == "FACE":
                return FACE

            if type == "SUBSYSTEM":
                return SUBSYSTEM

            if type ==  "INTEGER":
                return Attribute.INT

            if type == "DOUBLE":
                return Attribute.DOUBLE

            if type == "DOUBLEVECTOR":
                return Attribute.DOUBLEVECTOR

            if type == "STRING":
                return Attribute.STRING

            if type == "DATE":
                return Attribute.DATE

            if type == "LINK":
                return Attribute.LINK


        def connect(self):
            established = False
            while not established:
                try:
                    log(str(self.host) + " " + str(self.port) + " " + str(self.username) + " " + str(self.password), Standard)
                    self.transport = paramiko.Transport((self.host, self.port))

                    self.transport.connect(username=self.username, password=self.password)
                    established = True
                except:
                    return False

            log("connected", Standard)
            self.sftp = paramiko.SFTPClient.from_transport(self.transport)
            return True


        def close(self):
            log("close connection", Standard)
            self.sftp.close()
            self.transport.close()

            self.sftp = None
            self.transport = None

        def get_file(self, file_name):
            self.real_file_name = "/tmp/" + str(uuid.uuid4())+".sqlite"
            self.sftp.get(file_name, self.real_file_name)

            return self.real_file_name

        def run(self):
            db = self.getGDALData("city")
            db.setGDALDatabase(self.real_file_name)
            os.remove(self.real_file_name)






