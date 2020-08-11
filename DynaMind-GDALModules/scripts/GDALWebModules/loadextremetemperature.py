from osgeo import ogr, osr
from pydynamind import *
import psycopg2
import datetime

from pydynamind import ViewContainer


class LoadExtremeTemperature(Module):
        display_name = "Load Extreme Temperature"
        group_name = "DAnCE Platform"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("host", STRING)
            self.host = "localhost"

            self.createParameter("database", STRING)
            self.database = "danceplatform"

            self.createParameter("table", STRING)
            self.table = "measurment"

            self.createParameter("username", STRING)
            self.username = ""

            self.createParameter("password", STRING)
            self.password = ""

            # date format
            self.createParameter("start", STRING)
            self.start = ""

            self.createParameter("end", STRING)
            self.end = ""

            self.createParameter("deltaT", INT)
            self.deltaT = 0

            # self.createParameter("from_city", BOOL)
            # self.from_city = False

            self.createParameter("view_name", STRING)
            self.view_name = "station"

            self.temperature_time_series = None

        def init(self):
            datastream = []
            self.node_station = ViewContainer(self.view_name, COMPONENT, READ)
            self.node_station.addAttribute("dance_station_id", Attribute.INT, READ)

            self.timeseries = ViewContainer("timeseries", COMPONENT, WRITE)
            self.timeseries.addAttribute(self.view_name+"_id", Attribute.INT, WRITE)
            self.timeseries.addAttribute("data", Attribute.DOUBLEVECTOR, WRITE)
            self.timeseries.addAttribute("start", Attribute.STRING, WRITE)
            self.timeseries.addAttribute("end", Attribute.STRING, WRITE)
            self.timeseries.addAttribute("timestep", Attribute.INT, WRITE)
            self.timeseries.addAttribute("type", Attribute.STRING, WRITE)

            self.heatwave = ViewContainer("heatwave", COMPONENT, WRITE)
            self.heatwave.addAttribute(self.view_name + str("_id"), Attribute.LINK, WRITE)
            self.heatwave.addAttribute("rank", Attribute.INT, WRITE)
            self.heatwave.addAttribute("start", Attribute.STRING, WRITE)
            self.heatwave.addAttribute("end", Attribute.STRING, WRITE)
            self.heatwave.addAttribute("min_temp", Attribute.DOUBLE, WRITE)
            self.heatwave.addAttribute("max_temp", Attribute.DOUBLE, WRITE)
            self.heatwave.addAttribute("avg_temp", Attribute.DOUBLE, WRITE)

            datastream.append(self.node_station)
            datastream.append(self.timeseries)
            datastream.append(self.heatwave)

            if self.isViewInStream("city", "temperature_timeseries"):
                self.temperature_time_series = ViewContainer("temperature_timeseries", DM.COMPONENT, DM.READ)
                self.temperature_time_series.addAttribute("start_date", DM.Attribute.STRING, DM.READ)
                # self.temperature_time_series.addAttribute("end_date", DM.Attribute.STRING, DM.READ)
                datastream.append(self.temperature_time_series)

            self.registerViewContainers(datastream)

        def measurement_types(self, cur):
            measurment_types = []
            cur.execute("SELECT id, name from measurment_type")
            rows = cur.fetchall()
            for r in rows:
                measurment_types.append((r[0], r[1]))
            return measurment_types

        def get_extreme_date(self, cur, station_id, number_of_events=20):
            # log("Select  *, avg(at) over(rows 2 preceding) as rolling_avg  from (SELECT  date_trunc('day', date) AS d, avg(value) as at,max(value) as mt, min(value) as mint, count(value) from measurment where station_id = " + str(station_id) + " and value < 9999 and measurment_type_id = 6 GROUP BY d order by d) temp order by rolling_avg DESC LIMIT " + str(number_of_events), Standard)
            # cur.execute("Select  *, avg(at) over(rows 2 preceding) as rolling_avg  from (SELECT  date_trunc('day', date) AS d, avg(value) as at,max(value) as mt, min(value) as mint, count(value) from measurment where station_id = " + str(station_id) + " and value < 9999 and measurment_type_id = 6 GROUP BY d order by d) temp order by rolling_avg DESC LIMIT " + str(number_of_events))
            cur.execute("Select min(d) over (rows 2 preceding) as start_date, max(d) over (rows 2 preceding) as end_date, avg(at) over (rows 2 preceding) as rolling_avg, max(mt) over (rows 2 preceding) as max_t, min(mint) over (rows 2 preceding) as min_t from (SELECT date_trunc('day', date) AS d, avg(value) as at, max(value) as mt, min(value) as mint, count(value) as co from measurment where station_id = " + str(station_id) + " and value < 9999 and measurment_type_id = 6 GROUP BY d having count(value) = 48 order by d ) temp order by rolling_avg DESC LIMIT " + str(number_of_events))
            rows = cur.fetchall()
            events = []
            for r in rows:
                # print(r[0])
                # return end date, min, max, rolling average
                events.append([r[1], r[4], r[3], r[2]])
            return events

        def run(self):
            try:
                conn = psycopg2.connect("dbname=" + str(self.database) + " user='" + str(self.username) + "' host='" + str(self.host) +  "' password='" + str(self.password) + "'")
            except:
                log("Unable to connect to database ", Error)
                self.setStatus(MOD_CHECK_ERROR)
                return
            cur = conn.cursor()

            start_date = None #self.start
            end_date = None #self.end

            if self.temperature_time_series:
                for c in self.temperature_time_series:
                    start_date = c.GetFieldAsString("start_date")
                    # end_date = c.GetFieldAsString("end_date")


            # for id, name in self.measurement_types(cur):
            self.createTimeseries(cur, start_date)
            self.node_station.finalise()

            if self.temperature_time_series:
                self.temperature_time_series.finalise()
            self.node_station.finalise()
            self.timeseries.finalise()

        def createTimeseries(self, cur, start_date):
            filter_query = ""
            if self.node_station.get_attribute_filter_sql_string(""):
                filter_query = " AND " + self.node_station.get_attribute_filter_sql_string("")

            log(filter_query, Debug)

            for station in self.node_station:
                station_id = station.GetFieldAsInteger("dance_station_id")

                for idx, event in enumerate(self.get_extreme_date(cur, station_id)):
                    ed = event[0]
                    sd = ed - datetime.timedelta(days=3)

                    e = self.heatwave.create_feature()
                    e.SetField(self.view_name + str("_id"), station.GetFID())
                    e.SetField("rank", idx+1)
                    e.SetField("start", str(sd))
                    e.SetField("end", str(ed))
                    e.SetField("min_temp",  event[1])
                    e.SetField("max_temp",  event[2])
                    e.SetField("avg_temp",  event[3])

                if not start_date:
                    end_date = self.get_extreme_date(cur, station_id)[0][0]
                    start_date = end_date - datetime.timedelta(days=3)
                else:
                    end_date = datetime.datetime.strptime(start_date, "%Y-%m-%d %H:%M:%S") + datetime.timedelta(days=3)


                # 2010-1-1 00:00:00
                if start_date:
                    filter_query += " AND date >= '" + str(start_date) + "'"

                if end_date:
                    filter_query += " AND date < '" + str(end_date) + "'"

                log(str(filter_query), Standard)



                # 6, temperature, C
                # 7, dew_point_temperature, C
                # 8, wind_speed, km / h
                # 9, wind_direction, degree
                # 10, windgust, km / h
                # 11, pressure, hPa


                data = {"temperature": 6, "dew_point_temperature": 7, "wind_speed": 8, "pressure": 11}

                for key, id in data.items():
                    cur.execute("SELECT date, value from " + str(self.table) + " WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(id) + filter_query + " ORDER BY date ASC")
                    rows = cur.fetchall()
                    result_temp = []
                    inserted = 0
                    val = 9999
                    for entry in rows:
                        if float(entry[1]) < 9999:
                            val = float(entry[1])
                        result_temp.append(val)
                        inserted+=1
                    timeseries = self.timeseries.create_feature()
                    timeseries.SetField("start", str(start_date))
                    timeseries.SetField("end", str(end_date))
                    timeseries.SetField(self.view_name+"_id", station.GetFID())

                    log(str(inserted), Standard)
                    dm_set_double_list(timeseries, "data", result_temp)
                    timeseries.SetField("type", key)
                    timeseries.SetField("timestep", 60*30)


