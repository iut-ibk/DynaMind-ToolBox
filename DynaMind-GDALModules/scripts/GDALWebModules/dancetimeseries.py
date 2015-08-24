
from osgeo import ogr, osr
from pydynamind import *
import psycopg2

class LoadDAnCETimeseries(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("host", STRING)
            self.host = "localhost"

            self.createParameter("database", STRING)
            self.database = "danceplatform"

            self.createParameter("username", STRING)
            self.username = ""

            self.createParameter("password", STRING)
            self.password = ""

            self.createParameter("deltaT", INT)
            self.deltaT = 0


            self.view_name = "station"

        def init(self):
            self.node_station = ViewContainer(self.view_name, NODE, READ)
            self.node_station.addAttribute("station_id", Attribute.INT, READ)
            self.node_station.addAttribute("values", Attribute.DOUBLEVECTOR, WRITE)
            self.node_station.addAttribute("values", Attribute.DOUBLEVECTOR, WRITE)
            self.node_station.addAttribute("start", Attribute.STRING, WRITE)
            self.node_station.addAttribute("end", Attribute.STRING, WRITE)
            self.node_station.addAttribute("timestep", Attribute.INT, WRITE)
            #self.node_station.addAttribute("station_id", Attribute.DOUBLEVECTOR, WRITE)
            self.registerViewContainers([self.node_station])

        def run(self):
            try:
                conn = psycopg2.connect("dbname=" + str(self.database) +  " user='" + str(self.username) +  "' host='" + str(self.host) +  "' password='" + str(self.password) +  "'")
            except:
                print "I am unable to connect to the database"

            cur = conn.cursor()

            for station in self.node_station:
                station_id = station.GetFieldAsInteger("station_id")

                # Find start and end date
                cur.execute("SELECT date from measurment WHERE station_id = " + str(station_id) + " ORDER BY date DESC LIMIT 1 ")
                rows = cur.fetchall()
                for r in rows:
                    station.SetField("end", r[0].strftime('%d.%m.%Y %H:%M:%S'))

                cur.execute("SELECT date  from measurment WHERE station_id = " + str(station_id) + " ORDER BY date ASC LIMIT 1 ")

                rows = cur.fetchall()
                for r in rows:
                    station.SetField("start", r[0].strftime('%d.%m.%Y %H:%M:%S'))

                cur.execute("SELECT date from measurment WHERE station_id = " + str(station_id) + " ORDER BY date ASC LIMIT 2 ")
                rows = cur.fetchall()
                times = []
                for r in rows:
                    print r[0]
                    times.append(r[0])

                db_timestep = (times[1] - times[0]).dt.total_seconds()


                delta_t = self.deltaT
                print db_timestep
                if self.deltaT < db_timestep:
                    log( "used timestep " + str(db_timestep), Warning)
                    delta_t = db_timestep

                if self.deltaT % db_timestep != 0:
                    log("non integer mutiplyer for timestep use nearest timestep " + str(int(delta_t / db_timestep) * db_timestep), Warning)

                skipper = int(delta_t / db_timestep)

                station.SetField("timestep",db_timestep * skipper)

                cur.execute("SELECT date, value from measurment WHERE station_id = " + str(station_id) +  " ORDER BY date ASC")
                rows = cur.fetchall()
                counter = 0
                r = 0
                result_rain = []
                inserted = 0
                for entry in rows:
                    if counter % skipper == 0:
                        result_rain.append(r)
                        r = 0
                        inserted+=1
                    r += float(entry[1])

                    counter+=1
                print counter, inserted
                station.SetField("values", ' '.join(str(d) for d in result_rain))

            self.node_station.finalise()


#
# import datetime
#
#

#
#
# cur = conn.cursor()
#
#
# cur.execute("""SELECT date  from measurment WHERE station_id = 5ORDER BY date DESC LIMIT 1 """)
#
# rows = cur.fetchall()
# for r in rows:
#      print r
#
# cur.execute("""SELECT date  from measurment WHERE station_id = 5ORDER BY date ASC LIMIT 1 """)
#
# rows = cur.fetchall()
# for r in rows:
#      print r
#
# cur.execute("""SELECT date,  value from measurment WHERE station_id = 5 and date >= '2000-1-1 00:00:00' and date < '2010-1-1 00:00:00'""")
#
# rows = cur.fetchall()