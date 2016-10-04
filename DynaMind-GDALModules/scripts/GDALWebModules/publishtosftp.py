from pydynamind import *


import paramiko
import time


class DM_Publish_SFTP(Module):

        display_name = "Publish Simulation DB to SFTP"
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


            self.createParameter("folder_name", STRING)
            self.folder_name = ""

            self.createParameter("file_name", STRING)
            self.file_name = ""

            self.createParameter("step", INT)
            self.step = 1 #export every x step

            self.transport = None
            self.sftp = None

            self.createParameter("scenario_id_as_prefix", BOOL)
            self.scenario_id_as_prefix = False

            self.dummy = ViewContainer("dummy", SUBSYSTEM, MODIFY)

        def init(self):
            views = []
            views.append(self.dummy)

            if self.scenario_id_as_prefix:
                self.city = ViewContainer("city", COMPONENT, READ)
                self.city.addAttribute("scenario_id", Attribute.INT, READ)
                views.append(self.city)
            self.registerViewContainers(views)

        def connect(self):
            established = False
            while not established:
                try:
                    log(str(self.host) + " " + str(self.port) + " " + str(self.username) + " " + str(self.password), Standard)
                    self.transport = paramiko.Transport((self.host, self.port))

                    self.transport.connect(username=self.username, password=self.password)
                    established = True
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

        def get_files(self, folder, expected, outputfolder):
            self.connect()
            for e in expected:
                folder_name = folder + "/" + e
                print folder_name
                files = self.sftp.listdir(folder_name)
                if not os.path.exists(outputfolder + "/" + e):
                    os.makedirs(outputfolder + "/" + e)

                for f in files:
                    log("copy " + str((folder_name + "/" + f)), Standard)
                    self.sftp.get(folder_name + "/" + f, outputfolder + "/" + e + "/" + f)
            self.close()

        def run(self):

            scenario_id = ""
            if self.scenario_id_as_prefix:
                self.city.reset_reading()
                for c in self.city:
                    scenario_id = str(c.GetFieldAsInteger("scenario_id"))

            #only export on x step
            if self.get_group_counter() != -1 and ( (self.get_group_counter()-1) % self.step != 0):
                return


            prefix = ""
            if scenario_id:
                prefix = scenario_id

            file_name = prefix + self.file_name

            if self.get_group_counter() != -1:
                file_name = prefix + self.file_name + "_" + str(self.get_group_counter())


            file_name += ".sqlite"

            try:
                self.connect()
                if self.folder_name:
                    try:
                        self.sftp.chdir(self.folder_name)  # Test if remote_path exists
                    except IOError:
                        self.sftp.mkdir(self.folder_name)  # Create remote_path
                        self.sftp.chdir(self.folder_name)

                self.sftp.put(self.getGDALDBName(), file_name)
                self.close()
                if self.scenario_id_as_prefix:
                    self.city.finalise()

            except Exception, e:
                log(str(e), Error)
                self.close()
                if self.scenario_id_as_prefix:
                    self.city.finalise()
                return

