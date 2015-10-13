from pydynamind import *

import paramiko
import os
import time
import datetime

class MikeWhisperer(Module):
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



        def check_for_done(self, con, folder, expected):
            print "check ", datetime.datetime.now(), folder
            try:
                for e in expected:
                    print e
                    con.listdir(folder + "/" + e)
                print "all done"
                return True
            except:
                print "continue waiting"
                return False

        def get_files(self, con, folder, expected, outputfolder):
            print expected
            for e in expected:
                folder_name = folder + "/" + e
                print folder_name
                files = con.listdir(folder_name)
                if not os.path.exists(outputfolder + "/" + e):
                    os.makedirs(outputfolder + "/" + e)

                for f in files:
                    print "copy" ,  (folder_name + "/" + f)
                    con.get(folder_name + "/" + f, outputfolder + "/" + e + "/" + f)


        def run(self):
            print "try to connect"

            if self.get_group_counter() != -1 and (self.get_group_counter() -  1) % self.step != 0:
                return


            print self.host, self.port, self.username, self.password
            transport = paramiko.Transport((self.host, self.port))

            transport.connect(username=self.username, password=self.password)

            print "connected"
            sftp = paramiko.SFTPClient.from_transport(transport)
            result_folder = self.result_folder
            if self.get_group_counter() != -1:
                result_folder = result_folder + "_" + str(self.get_group_counter())
            while not self.check_for_done(sftp, result_folder, self.simulation.split(",")):
                time.sleep(60)

            self.get_files(sftp, result_folder, self.simulation.split(","), self.output_folder)
            sftp.close()

