from osgeo import ogr
import csv
from pydynamind import *

import pdb
import dropbox


class DM_SIM_DB_DropBox(Module):

        display_name = "Publish Simulation DB on Dropbox"
        group_name = "Data Import and Export"


        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("access_token", STRING)
            self.access_token = ""

            self.createParameter("file_name", STRING)
            self.file_name = ""

            self.createParameter("step", INT)
            self.step = 1 #export every x step

        def init(self):
            self.dummy = ViewContainer("dummy", SUBSYSTEM, MODIFY)


            self.registerViewContainers([self.dummy ])

        def run(self):
            #only exort on x step
            if self.get_group_counter() != -1 and (self.get_group_counter() % self.step != 0):
                return

            file_name = self.file_name
            if self.get_group_counter() != -1:
                file_name = self.file_name + "_" + str(self.get_group_counter())
            file_name += ".sqlite"
            try:
                dbx = dropbox.Dropbox(self.access_token)
                #print 'linked account: ', dbx.users_get_current_account()
                self.getGDALDBName()
                f = open(self.getGDALDBName(), 'rb')

                dbx.files_upload(f, file_name, autorename=True)


            except Exception, e:
                log(str(e), Error)
                return

