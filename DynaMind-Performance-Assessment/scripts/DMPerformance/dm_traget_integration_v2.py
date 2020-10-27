__author__ = 'christianurich'

from pydynamind import *
import sqlite3
import sys
import os
import ogr

import numpy as np
import uuid
import datetime
import subprocess
from netCDF4 import Dataset
import math

from timezonefinder import TimezoneFinder
from dateutil import tz
from scipy.optimize import curve_fit


class Sun:

    def getSunriseTime(self, coords, date):
        return self.calcSunTime(coords, date, True)

    def getSunsetTime(self, coords, date):
        return self.calcSunTime(coords, date, False)

    def getCurrentUTC(self):
        now = datetime.datetime.now()
        return [now.day, now.month, now.year]

    def calcSunTime(self, coords, date, isRiseTime, zenith=90.8):

        # isRiseTime == False, returns sunsetTime

        # day, month, year = self.getCurrentUTC()

        day = date["day"]
        month = date["month"]
        year = date["year"]

        longitude = coords['longitude']
        latitude = coords['latitude']

        TO_RAD = math.pi / 180

        # 1. first calculate the day of the year
        N1 = math.floor(275 * month / 9)
        N2 = math.floor((month + 9) / 12)
        N3 = (1 + math.floor((year - 4 * math.floor(year / 4) + 2) / 3))
        N = N1 - (N2 * N3) + day - 30

        # 2. convert the longitude to hour value and calculate an approximate time
        lngHour = longitude / 15

        if isRiseTime:
            t = N + ((6 - lngHour) / 24)
        else:  # sunset
            t = N + ((18 - lngHour) / 24)

        # 3. calculate the Sun's mean anomaly
        M = (0.9856 * t) - 3.289

        # 4. calculate the Sun's true longitude
        L = M + (1.916 * math.sin(TO_RAD * M)) + (0.020 * math.sin(TO_RAD * 2 * M)) + 282.634
        L = self.forceRange(L, 360)  # NOTE: L adjusted into the range [0,360)

        # 5a. calculate the Sun's right ascension

        RA = (1 / TO_RAD) * math.atan(0.91764 * math.tan(TO_RAD * L))
        RA = self.forceRange(RA, 360)  # NOTE: RA adjusted into the range [0,360)

        # 5b. right ascension value needs to be in the same quadrant as L
        Lquadrant = (math.floor(L / 90)) * 90
        RAquadrant = (math.floor(RA / 90)) * 90
        RA = RA + (Lquadrant - RAquadrant)

        # 5c. right ascension value needs to be converted into hours
        RA = RA / 15

        # 6. calculate the Sun's declination
        sinDec = 0.39782 * math.sin(TO_RAD * L)
        cosDec = math.cos(math.asin(sinDec))

        # 7a. calculate the Sun's local hour angle
        cosH = (math.cos(TO_RAD * zenith) - (sinDec * math.sin(TO_RAD * latitude))) / (
                    cosDec * math.cos(TO_RAD * latitude))

        if cosH > 1:
            return {'status': False, 'msg': 'the sun never rises on this location (on the specified date)'}

        if cosH < -1:
            return {'status': False, 'msg': 'the sun never sets on this location (on the specified date)'}

        # 7b. finish calculating H and convert into hours

        if isRiseTime:
            H = 360 - (1 / TO_RAD) * math.acos(cosH)
        else:  # setting
            H = (1 / TO_RAD) * math.acos(cosH)

        H = H / 15

        # 8. calculate local mean time of rising/setting
        T = H + RA - (0.06571 * t) - 6.622

        # 9. adjust back to UTC
        UT = T - lngHour
        UT = self.forceRange(UT, 24)  # UTC time in decimal format (e.g. 23.23)

        # 10. Return
        hr = self.forceRange(int(UT), 24)
        min = round((UT - int(UT)) * 60, 0)

        return {
            'status': True,
            'decimal': UT,
            'hr': hr,
            'min': min
        }

    def forceRange(self, v, max):
        # force v to be >= 0 and < max
        if v < 0:
            return v + max
        elif v >= max:
            return v - max

        return v

class TargetInegrationv2(Module):
    display_name = "Targetv2"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("from_temperature_station", DM.BOOL)
        self.from_temperature_station = False

        self.createParameter("grid_size_from_city", BOOL)
        self.grid_size_from_city = False

        self.createParameter("grid_size", DOUBLE)
        self.grid_size = 20.0

        self.createParameter("reporting_timestep", DM.INT)
        self.reporting_timestep = 4

        self.internal_grid_size = 0.0

        self.air_temperature_data = ViewContainer("micro_climate_grid", DM.FACE, DM.READ)
        self.air_temperature_data.addAttribute("time_stamp", DM.Attribute.STRING, DM.WRITE)

        self.createParameter("target_path", STRING)
        # /Users/christianurich/Downloads/mothlight-target_java-fbfddc596fdc/src
        self.target_path = ""

        self.city = ViewContainer("city", DM.COMPONENT, DM.READ)
        self.city.addAttribute("region", DM.Attribute.STRING, DM.READ)
        self.city.addAttribute("lat", DM.Attribute.DOUBLE, DM.READ)
        self.city.addAttribute("long", DM.Attribute.DOUBLE, DM.READ)


        self.registerViewContainers([self.city, self.air_temperature_data])

        self.kd = {}

        self.kd["townsville"] = [999.24, 1017.83, 937.01666667, 844.53333333, 776.14,
                 715.83666667, 748.51666667, 847.73, 912.15666667, 979.73666667,
                 998.11666667, 1001.08333333]

        self.kd["melbourne"] = [999.24, 1017.83, 937.01666667, 844.53333333, 776.14,
                 715.83666667, 748.51666667, 847.73, 912.15666667, 979.73666667,
                 998.11666667, 1001.08333333]

        self.lat = 144.9631
        self.long = -37.8136
        self.region = "melbourne"

    def init(self):

        datastream = []

        self.micro_climate_grid = ViewContainer("micro_climate_grid", DM.COMPONENT, DM.READ)
        self.micro_climate_grid.addAttribute("roof_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("road_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("water_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("concrete_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("tree_cover_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("grass_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("irrigated_grass_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("target_h", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("target_w", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("taf", DM.Attribute.DOUBLE, DM.WRITE)
        self.micro_climate_grid.addAttribute("taf_period", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.micro_climate_grid.addAttribute("ts", DM.Attribute.DOUBLE, DM.WRITE)
        self.micro_climate_grid.addAttribute("ucan", DM.Attribute.DOUBLE, DM.WRITE)
        self.micro_climate_grid.addAttribute("utci_period", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.micro_climate_grid.addAttribute("utci", DM.Attribute.DOUBLE, DM.WRITE)

        for t in range(143):
            if t % self.reporting_timestep == 0:
                self.micro_climate_grid.addAttribute(f"taf_{t}", DM.Attribute.DOUBLE, DM.WRITE)
                self.micro_climate_grid.addAttribute(f"utci_{t}", DM.Attribute.DOUBLE, DM.WRITE)
        if self.from_temperature_station:

            self.temperature_station = ViewContainer("temperature_station", DM.COMPONENT, DM.READ)
            self.temperature_station.addAttribute("dance_station_id", DM.Attribute.INT, DM.READ)

            self.timeseries = ViewContainer("timeseries", DM.COMPONENT, DM.READ)
            self.timeseries.addAttribute("data", DM.Attribute.DOUBLEVECTOR, DM.READ)
            self.timeseries.addAttribute("temperature_station_id", DM.Attribute.INT, DM.READ)
            self.timeseries.addAttribute("start", DM.Attribute.STRING, DM.READ)

            datastream.append(self.timeseries)
            datastream.append(self.temperature_station)

            if self.grid_size_from_city:
                self.city.addAttribute("grid_size", DM.Attribute.DOUBLE, READ)

        datastream.append(self.city)
        datastream.append(self.air_temperature_data)
        datastream.append(self.micro_climate_grid)

        self.registerViewContainers(datastream)

    def fixNulls(self, text):
        if (text == None):
            text = 0.0
        return round(text, 2)

    def convert_time(self,timezone, time, date):
        seconds = time * 3600
        m, s = divmod(seconds, 60)
        h, m = divmod(m, 60)
        time_to_string = "%d:%02d:%02d" % (h, m, s)
        # METHOD 1: Hardcode zones:
        from_zone = tz.gettz('UTC')

        to_zone = tz.gettz(timezone)

        utc = datetime.datetime.strptime(f'{date["year"]}-{date["month"]}-{date["day"]} {time_to_string}', '%Y-%m-%d %H:%M:%S')
        utc = utc.replace(tzinfo=from_zone)
        central = utc.astimezone(to_zone)
        return central

    def sunset_sunrise(self, long, lat, day, month, year):
        # from Sun import Sun
        coords = {'longitude': long, 'latitude': lat}
        date = {'day': day, 'month': month, 'year': year}
        sun = Sun()

        tf = TimezoneFinder()
        timezone = tf.timezone_at(lng=long, lat=lat)

        # Sunrise time UTC (decimal, 24 hour format)
        sunrise = self.convert_time(timezone, sun.getSunriseTime(coords, date)['decimal'], date)

        # Sunset time UTC (decimal, 24 hour format)
        sunset = self.convert_time(timezone, sun.getSunsetTime(coords, date)['decimal'], date)


        return sunrise, sunset

    def load_temperature_data(self, region, long, lat):
        t = [24.7, 24.5, 22.3, 22.3, 21.7, 21.7, 21.9, 20.6, 20.6, 21.3, 21.3, 19.6, 18.8, 18.7, 19.9, 22.9, 25.2, 27.3,
             29.4, 31, 33, 35, 36.4, 38, 37.9, 39.1, 39, 40.2, 39.8, 40.3, 41.2, 41.2, 42.3, 42.6, 42.4, 42.4, 42.2,
             41.9, 40.8, 39.9, 36.6, 34.5, 32.1, 31.1, 30.6, 29.5, 29.7, 28.4, 27.3, 26.4, 26.5, 26.3, 24.6, 24.5, 24.5,
             23.6, 23.7, 22.3, 22.9, 23, 22, 23, 23.7, 23.7, 26, 29.3, 32.2, 33.4, 36.2, 37.8, 39.3, 40.3, 42.4, 42.8,
             43.9, 44, 44.4, 44.8, 44.3, 45.5, 43.6, 44.6, 45.5, 44.6, 43.7, 44.1, 42.9, 41.5, 38.2, 36.3, 34.9, 33.8,
             32.5, 32.6, 32.2, 31.7, 32.4, 31.3, 30.9, 30.1, 30.7, 30.5, 30.3, 29.6, 32.8, 30.8, 27.8, 26.8, 26.8, 24.4,
             26.8, 27.7, 30.1, 32.6, 34.6, 35.4, 37.1, 37.7, 39.3, 40.5, 41.3, 42.9, 42.9, 43, 41.5, 42.7, 41.3, 39.6,
             37, 34.7, 34.2, 32.1, 32.3, 30.4, 28.2, 27, 26.6, 25.9, 24.9, 23.7, 21.8, 19.9, 18.3, 17.9]
        rh = [52, 51, 59, 61, 60, 59, 57, 62, 64, 64, 63, 68, 71, 69, 66, 54, 47, 42, 38, 34, 30, 26, 23, 22, 21, 20,
              18, 17, 18, 16, 16, 16, 16, 15, 15, 15, 14, 15, 15, 16, 22, 25, 31, 33, 34, 36, 34, 38, 41, 44, 45, 47,
              55, 54, 52, 60, 55, 61, 60, 60, 60, 58, 59, 61, 49, 42, 32, 30, 24, 21, 18, 18, 15, 13, 12, 12, 11, 11,
              11, 11, 11, 10, 10, 10, 11, 11, 11, 12, 16, 19, 21, 24, 26, 25, 25, 26, 25, 26, 27, 28, 27, 28, 28, 27,
              22, 24, 29, 32, 32, 39, 33, 34, 29, 24, 20, 18, 17, 17, 17, 15, 15, 13, 12, 10, 12, 15, 21, 24, 25, 26,
              27, 31, 27, 25, 26, 30, 30, 29, 28, 25, 26, 31, 44, 49]
        ws = [0, 8, 8, 0, 11, 11, 9, 0, 9, 13, 17, 9, 8, 8, 9, 9, 9, 11, 8, 5, 9, 9, 17, 15, 17, 21, 21, 21, 13, 13, 18,
              11, 8, 9, 18, 13, 13, 11, 11, 2, 0, 8, 2, 0, 5, 5, 8, 4, 0, 0, 0, 0, 0, 0, 8, 9, 8, 5, 9, 5, 5, 9, 9, 8,
              8, 8, 9, 5, 8, 13, 17, 18, 26, 24, 24, 21, 21, 24, 18, 22, 26, 18, 17, 24, 17, 21, 17, 15, 11, 9, 0, 5, 5,
              5, 8, 8, 9, 9, 8, 0, 9, 13, 13, 11, 17, 11, 9, 8, 8, 15, 4, 8, 0, 8, 13, 17, 18, 21, 22, 26, 31, 37, 46,
              46, 46, 42, 39, 42, 46, 50, 42, 39, 39, 31, 31, 28, 24, 21, 22, 28, 24, 26, 24, 22]
        p = [1011.2, 1010.9, 1010.8, 1010.6, 1010.7, 1010.6, 1010.3, 1010.2, 1010.4, 1010.6, 1010.7, 1011, 1011.1,
             1011.3, 1011.5, 1011.8, 1012.1, 1012.4, 1012.2, 1012.4, 1012.5, 1012.6, 1012.4, 1012, 1011.6, 1011.1,
             1010.8, 1010.4, 1009.9, 1009.5, 1008.9, 1008.4, 1008, 1007.6, 1007.3, 1007, 1006.7, 1006.5, 1006.6, 1006.6,
             1006.8, 1007, 1007.2, 1007.5, 1007.8, 1007.9, 1007.9, 1007.9, 1007.7, 1007.6, 1007.6, 1007.3, 1007.2, 1007,
             1007, 1006.8, 1006.9, 1007.1, 1007.2, 1007.4, 1007.7, 1008.1, 1008.5, 1008.6, 1008.9, 1009.3, 1009.4,
             1009.1, 1009.1, 1008.8, 1008.7, 1008.4, 1008, 1007.6, 1007.3, 1006.7, 1006.2, 1005.8, 1005.2, 1004.7,
             1004.2, 1003.9, 1003.6, 1003.6, 1003.2, 1003.2, 1003.2, 1003.3, 1003.3, 1003.4, 1003.5, 1003.6, 1003.6,
             1003.5, 1003.3, 1003, 1002.7, 1002.5, 1002.3, 1001.9, 1001.4, 1001.1, 1000.9, 1000.9, 1001, 1000.8, 1000.5,
             1000.5, 1000.6, 1000.8, 1001, 1001.2, 1001.3, 1001.4, 1001.4, 1001.2, 1001, 1000.9, 1000.6, 1000.3, 1000,
             999.4, 999, 998.5, 998.3, 998.3, 998.4, 998.7, 999.6, 1000.6, 1001.2, 1001.8, 1002.4, 1003.4, 1004.2,
             1004.6, 1004.9, 1005.3, 1005.9, 1006.7, 1007.1, 1007.5, 1008.1, 1008.2]
        kd = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 58.02, 439.81, 623.46, 727.19, 756.89, 832.55, 879.93, 906.69, 932.18,
              942.08, 953.24, 959.77, 959.21, 965.06, 960.33, 953.96, 961.62, 943.24, 38.85, 392.37, 333.84, 663.53,
              2.06, 127.39, 582.78, 205.56, 81.55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              180.71, 411.46, 0.77, 0.81, 304.55, 1.96, 142.28, 858.39, 886.99, 771.1, 468.91, 275.67, 939.21, 921.69,
              616.83, 383, 0.17, 919.02, 901.65, 874.89, 27.61, 0, 0.14, 645.28, 13.21, 0.17, 54.21, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8, 306.92, 605.16, 642.92, 777.19, 835.82, 878.07, 894.85,
              280.47, 727.82, 0.59, 50.37, 0, 0.22, 1.09, 0, 0.69, 1.27, 0.93, 0.52, 0.38, 0.21, 0.31, 0.32, 14.66,
              440.46, 74.87, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        ld = [411.291, 406.59, 405.57, 404.84, 404.62, 403.12, 398.33, 399.02, 397.5, 393.81, 390.37, 389.02, 386.16,
              387.23, 390.15, 397.19, 402.64, 408.22, 415.83, 425.52, 432, 438.17, 440.09, 441.62, 445.94, 449.32,
              450.43, 452.99, 454.9, 459.95, 467.09, 458.9, 461.12, 468.16, 474.66, 458.21, 462.78, 452.61, 450.3,
              440.35, 438.29, 431.37, 442.93, 442.8, 455.24, 429.55, 447.35, 434.543, 434.84, 429.98, 443.57, 448.32,
              442.41, 410.45, 400.25, 400.05, 403.23, 398.8, 397.47, 391.32, 391.22, 423.95, 425.35, 423.75, 435.87,
              435.24, 436.49, 427.57, 432.55, 439.28, 466.71, 466.65, 454.74, 453.61, 461.85, 460.28, 471.99, 455.3,
              457.28, 461.48, 459.96, 483.84, 474.83, 442.22, 460.63, 436.29, 456.65, 430.52, 456.93, 439.88, 432.1,
              429.82, 440.94, 430.42, 423.66, 425.97, 422.723, 403.06, 408.67, 410.65, 402.85, 388.6, 392.75, 397.03,
              404.76, 395.56, 397.86, 397.42, 388.61, 380.97, 382.48, 385.02, 385.01, 385.14, 380.7, 387.47, 394.23,
              377.98, 412.55, 409.48, 405.85, 397.27, 386.73, 379.68, 374.1, 366.85, 359.76, 355.92, 353.39, 349.88,
              346.86, 344.82, 340.59, 334.22, 321.98, 307.86, 304.93, 303.01, 303.65, 300.52, 297.96, 296.58, 295.26,
              293.12]

        if not self.from_temperature_station:
            log("No temperature station defined loading default values", Warning)
            return t, rh, ws, p, kd, ld

        station_id = -1
        for s in self.temperature_station:
            station_id = s.GetFID()

        if station_id < 0:
            log("No temperature station defined loading default values", Warning)
            self.timeseries.finalise()
            self.temperature_station.finalise()
            return t, rh, ws, p, kd, ld

        log("Load data from station " + str(station_id), Standard)

        self.timeseries.setAttributeFilter("temperature_station_id = " + str(station_id))
        self.timeseries.reset_reading()
        data = {}

        start_date = datetime.datetime.strptime("5/02/2009 00:00", "%d/%m/%Y %H:%M")
        for t in self.timeseries:
            if t.GetFieldAsInteger("temperature_station_id") == station_id:
                type = t.GetFieldAsString("type")
                data[type] = dm_get_double_list(t, "data")
                if type == "temperature":
                    start_date = datetime.datetime.strptime(t.GetFieldAsString("start"), "%Y-%m-%d %H:%M:%S")

        self.timeseries.finalise()
        self.temperature_station.finalise()
        data["relative_humidity"] = []
        for idx, t in enumerate(data["temperature"]):  # Convert to m/s
            td = data["dew_point_temperature"][idx]
            u = 100. * math.exp(1.8096 + (17.2694 * td) / (237.3 + td)) / math.exp(
                (1.8096) + (17.2694 * t) / (237.3 + t))
            d = data["relative_humidity"]
            d.append(u)
            data["relative_humidity"] = d
        wind_speed = []
        for idx, w in enumerate(data["wind_speed"]):  # Convert to m/s
            wind_speed.append( w * 1000. / 24 / 60 / 60 )
        data["wind_speed"] = wind_speed

        if region in self.kd:
            kd = self.create_solar_radiation(self.kd[region][start_date.month], long, lat, start_date.day, start_date.month, start_date.year).tolist()

        return data["temperature"], data["relative_humidity"], data["wind_speed"], data["pressure"], kd, ld

    def square_func(self, x, a, b, c):
        return a * x * x + b * x + c

    def create_solar_radiation(self, watt, long, lat, day, month, year):
        sunrise, sunset = self.sunset_sunrise(long, lat, day, month, year)
        start_time_step = (sunrise.hour * 60 + sunrise.minute) / 30
        end_time_step = (sunset.hour * 60 + sunset.minute) / 30
        g_popt, g_pcov = curve_fit(self.square_func, [start_time_step, (end_time_step + start_time_step) / 2, end_time_step],
                                   [0, watt, 0])
        curve = np.array([self.square_func(x, *g_popt) for x in range(0, 48, 1)])
        curve[curve < 0] = 0
        kd = np.append(curve, np.append(curve, curve))
        return kd

    def write_climate_file(self, cimate_file, region, long, lat):
        t, rh, ws, p, kd, ld = self.load_temperature_data(region, long, lat)

        d = datetime.datetime.strptime("5/02/2009 00:00", "%d/%m/%Y %H:%M")

        with open(cimate_file, "w") as f:
            f.write("datetime,Ta,RH,WS,P,Kd,Ld\n")

            for idx, temperature in enumerate(t):
                if idx > 143:
                    break
                f.write(d.strftime("%d/%m/%Y %H:%M"))
                f.write(",")
                f.write(str(temperature))
                f.write(",")
                f.write(str(rh[idx]))
                f.write(",")
                f.write(str(ws[idx]))
                f.write(",")
                f.write(str(p[idx]))
                f.write(",")
                f.write(str(kd[idx]))
                f.write(",")
                f.write(str(ld[idx]))
                f.write(str("\n"))
                d = d + datetime.timedelta(minutes=30)

    def read_output_file(self, output):
        # print "read me"
        dataset = Dataset(output, 'r')
        self.city.reset_reading()

        for c in self.city:
            min_lat = c.GetFieldAsDouble("min_lat")
            min_long = c.GetFieldAsDouble("min_long")
        self.micro_climate_grid.reset_reading()
        data = dataset.variables['TEMP_AIR'][:]
        data_utci = dataset.variables['UTCI'][:]
        for (idx, m) in enumerate(self.micro_climate_grid):
            element_y = int(round((self.fixNulls(m.GetFieldAsDouble("lat_cart")) - min_lat) / self.internal_grid_size))
            element_x = int(
                round((self.fixNulls(m.GetFieldAsDouble("long_cart")) - min_long) / self.internal_grid_size))
            at = []
            utci = []
            for t in range(143):
                air_temperature = data[t]
                utc = data_utci[t]
                at.append(air_temperature[element_y][element_x])
                utci.append(utc[element_y][element_x])
                if t % self.reporting_timestep == 0:
                    m.SetField(f"taf_{t}", air_temperature[element_y][element_x])
                    m.SetField(f"utci_{t}", utc[element_y][element_x])
            m.SetField("taf", np.array(at).max())
            m.SetField("utci", np.array(utci).max())
            dm_set_double_list(m, "taf_period", at)
            dm_set_double_list(m, "utci_period", utci)
        self.micro_climate_grid.finalise()

    def write_config(self, config_file, output_uuid, climate_file, landuse_file, cols, rows):
        with open(config_file, "w") as f:
            f.write("site_name=Mawson\n")
            f.write("run_name=" + str(output_uuid) + "\n")
            f.write("inpt_met_file=" + str(climate_file) + str("\n"))
            f.write("inpt_lc_file=" + str(landuse_file) + str("\n"))
            f.write("output_dir=/tmp" + str("\n"))
            f.write("date_fmt=%d/%m/%Y %H:%M   " + str("\n"))
            f.write("timestep=1800S   " + str("\n"))
            f.write("include roofs=Y   " + str("\n"))
            f.write("SpinUp=2009,2,5,0	   " + str("\n"))
            f.write("StartDate =2009,2,5,0	   " + str("\n"))
            f.write("EndDate =2009,2,7,24   " + str("\n"))

            f.write("mod_ldwn=N" + str("\n"))
            f.write("lat=-37.8136" + str("\n"))
            f.write("domainDim=" + str(cols) + "," + str(rows) + str("\n"))
            f.write("latEdge=-34.79829" + str("\n"))
            f.write("lonEdge=138.79829" + str("\n"))
            f.write("latResolution=0.00088" + str("\n"))
            f.write("lonResolution=0.00110" + str("\n"))
            f.write("disableOutput=Fid,tb,TsurfWall,TsurfCan,TsurfHorz,Ucan,Pet" + str("\n"))

    def run(self):
        sys.path.append(self.target_path + "/Toolkit2-Runs/bin/")
        output_uuid = str(uuid.uuid4())

        landuse_file = "/tmp/" + str(uuid.uuid4()) + "lc.csv"
        config_file = "/tmp/" + str(uuid.uuid4()) + "conf.txt"
        climate_file = "/tmp/" + str(uuid.uuid4()) + "climate.csv"

        cols, rows = self.write_input_file(landuse_file)

        self.write_climate_file(climate_file, self.region, self.long, self.lat)
        self.write_config(config_file, output_uuid, climate_file, landuse_file, cols, rows)
        self.run_target(config_file)
        self.read_output_file(str("/tmp/" + str(output_uuid) + str(".nc")))
        self.city.finalise()

        os.remove(str("/tmp/" + str(output_uuid) + str(".nc")))
        os.remove(landuse_file)
        os.remove(climate_file)
        os.remove(config_file)

    def run_target(self, config_file):
        subprocess.call(
            "javac -cp ../../netcdfAll-4.6.11.jar:../../slf4j-jdk14-1.7.14.jar:. *.java HTC/*.java -encoding UTF-8",
            shell=True, cwd=self.target_path + "/Target")
        subprocess.call("java -cp ../netcdfAll-4.6.11.jar:. Target.RunToolkit " + str(config_file),
                        shell=True, cwd=self.target_path)

    def write_input_file(self, landuse_file):
        text_file = open(landuse_file, "w")
        text_file.write("FID,roof,road,watr,conc,Veg,dry,irr,H,W" + '\n')

        self.internal_grid_size = self.grid_size

        for c in self.city:
            min_lat = c.GetFieldAsDouble("min_lat")
            min_long = c.GetFieldAsDouble("min_long")
            self.lat = c.GetFieldAsDouble("lat")
            self.long = c.GetFieldAsDouble("long")
            self.region = c.GetFieldAsString("region")
            print(self.lat, self.long,self.region )
            height = c.GetFieldAsDouble("max_lat") - c.GetFieldAsDouble("min_lat")
            length = c.GetFieldAsDouble("max_long") - c.GetFieldAsDouble("min_long")

            if self.grid_size_from_city:
                self.internal_grid_size = c.GetFieldAsDouble("grid_size")

        log(f"Grid Size {self.grid_size}, {self.internal_grid_size}", Standard)

        current_pos = -1

        for m in self.micro_climate_grid:
            element_y = round((self.fixNulls(m.GetFieldAsDouble("lat_cart")) - min_lat) / self.internal_grid_size)
            element_x = round((self.fixNulls(m.GetFieldAsDouble("long_cart")) - min_long) / self.internal_grid_size)

            new_pos = element_y * round(length / self.internal_grid_size + 1) + element_x
            for i in range(int(round(new_pos - (current_pos + 1)))):
                row_text = str(current_pos + i + 1) + ',' + str(0) + ',' + str(0) + ',' + str(
                    0) + ',' + str(0) + ',' + str(0) + ',' + str(
                    1) + ',' + str(0) + ',' + str(4) + ',' + str(30) + '\n'
                text_file.write(row_text)

            current_pos = new_pos

            ogc_fid = self.fixNulls(m.GetFID())
            roof_fraction = self.fixNulls(m.GetFieldAsDouble("roof_fraction"))
            road_fraction = self.fixNulls(m.GetFieldAsDouble("road_fraction"))
            water_fraction = self.fixNulls(m.GetFieldAsDouble("water_fraction"))
            concrete_fraction = self.fixNulls(m.GetFieldAsDouble("concrete_fraction"))
            tree_cover_fraction = self.fixNulls(m.GetFieldAsDouble("tree_cover_fraction"))
            grass_fraction = self.fixNulls(m.GetFieldAsDouble("grass_fraction"))
            irrigated_grass_fraction = self.fixNulls(m.GetFieldAsDouble("irrigated_grass_fraction"))
            target_h = self.fixNulls(m.GetFieldAsDouble("target_h"))
            target_w = self.fixNulls(m.GetFieldAsDouble("target_w"))

            # H = '4'
            # W = '30'
            #
            # if (roof_fraction > 0.3):
            #     H = '8'
            #     W = '10'
            # else:
            #     if (grass_fraction + irrigated_grass_fraction > 0.5):
            #         H = '4'
            #         W = '30'

            # row_text =fixNulls(row[0]) + ',' + fixNulls(row[1]) + ',' + fixNulls(row[2])+ ',' + fixNulls(row[3])+ ',' + fixNulls(row[4])+ ',' + fixNulls(row[5])+ ',' + fixNulls(row[6])+ ',' + fixNulls(row[7]) +  ',' + H +  ',' + W + '\n'
            row_text = str(current_pos) + ',' + str(roof_fraction) + ',' + str(road_fraction) + ',' + str(
                water_fraction) + ',' + str(concrete_fraction) + ',' + str(tree_cover_fraction) + ',' + str(
                grass_fraction) + ',' + str(irrigated_grass_fraction) + ',' + str(target_h) + ',' + str(target_w) + '\n'
            text_file.write(row_text)

        # print length/self.internal_grid_size, height/self.internal_grid_size
        last_pos = (length / self.internal_grid_size + 1) * (height / self.internal_grid_size + 1)

        for i in range(int(round(last_pos - (current_pos + 1)))):
            # print "fill", current_pos + i + 1
            row_text = str(current_pos + i + 1) + ',' + str(0) + ',' + str(0) + ',' + str(
                0) + ',' + str(0) + ',' + str(0) + ',' + str(
                1) + ',' + str(0) + ',' + str(4) + ',' + str(30) + '\n'
            text_file.write(row_text)

        text_file.close()
        # self.micro_climate_grid.finalise()

        return int(round(length / self.internal_grid_size + 1)), int(round(height / self.internal_grid_size + 1))

