from pydynamind import *

import paramiko
import os
import time
import datetime

class MikeWhisperer(Module):
        display_name = "Coupling Mike Urban"
        group_name = "Performance Assessment"

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

            self.createParameter("simulation", STRING)
            self.simulation = ""

            self.createParameter("result_folder", STRING)
            self.result_folder = ""

            self.createParameter("output_folder", STRING)
            self.output_folder = ""

            self.createParameter("step", INT)
            self.step = 1

            self.dummy = ViewContainer("city", COMPONENT, MODIFY)
            self.registerViewContainers([self.dummy])

            self.transport = None
            self.sftp = None

        def check_for_done(self,  folder, expected):
            self.connect()
            log("check " + str(datetime.datetime.now()) + " " + str(folder), Standard)
            try:
                for e in expected:
                    print e
                    self.sftp.listdir(folder + "/" + e)
                log("all done", Standard)
                self.close()
                return True
            except:
                log("continue waiting", Standard)
                self.close()
                return False

        def get_files(self, folder, expected, outputfolder):
            self.connect()
            for e in expected:
                folder_name = folder + "/" + e
                print folder_name
                files = self.sftp.listdir(folder_name)
                if not os.path.exists(outputfolder + "/" + e):
                    os.makedirs(outputfolder + "/" + e)

                for f in files:
                    log("copy " +  str((folder_name + "/" + f)), Standard)
                    self.sftp.get(folder_name + "/" + f, outputfolder + "/" + e + "/" + f)
            self.close()

        def connect(self):
            establised = False
            while not establised:
                try:
                    log(str(self.host) + " " + str(self.port) + " " + str(self.username) + " " + str(self.password), Standard)
                    self.transport = paramiko.Transport((self.host, self.port))

                    self.transport.connect(username=self.username, password=self.password)
                    establised = True
                except:
                    log("Connection failed, try again", Warning)
                    time.sleep(60)

            log("connected", Standard)
            self.sftp = paramiko.SFTPClient.from_transport(self.transport)

        def close(self):
            log("close connection", Standard)
            self.sftp.close()
            self.transport.close()

            self.sftp = None
            self.transport = None

        def run(self):
            log("try to connect", Standard)
            print self.get_group_counter()
            if self.get_group_counter() != -1 and (self.get_group_counter() -  1) % self.step != 0:
                return


            result_folder = self.result_folder
            if self.get_group_counter() != -1:
                result_folder = result_folder + "_" + str(self.get_group_counter())
            while not self.check_for_done(result_folder, self.simulation.split(",")):
                time.sleep(60)

            self.get_files(result_folder, self.simulation.split(","), self.output_folder)








