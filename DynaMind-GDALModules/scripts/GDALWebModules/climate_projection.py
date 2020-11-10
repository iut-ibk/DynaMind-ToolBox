from pydynamind import *
from netCDF4 import Dataset
import pandas as pd
# import xarray as xr
import requests
from io import StringIO
import datetime
from datetime import timedelta

class ClimateProjection(Module):
    display_name = "Climate Projection"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("from_temperature_station", DM.BOOL)
        self.from_temperature_station = False

        self.createParameter("from_projection", DM.BOOL)
        self.from_projection = True


        self.createParameter("fraction", DM.DOUBLE)
        self.fraction = 0.0

        self.createParameter("mean_temperature", DM.STRING)
        #self.mean_temperature =  '/Users/christianurich/Documents/rainfall/climate/tscr_aveAdjust.daily.ccam10-awap_ACCESS1-0Q_rcp85.nc'
        self.mean_temperature = 'https://dap.tern.org.au/thredds/ncss/CMIP5QLD/CMIP5_Downscaled_CCAM_QLD10/RCP85/daily_adjusted/MeanTemperature/tscr_aveAdjust.daily.ccam10-awap_CSIRO-Mk3-6-0Q_rcp85.nc'

        self.createParameter("max_temperature", DM.STRING)
        #self.max_temperature = '/Users/christianurich/Documents/rainfall/climate/tmaxscrAdjust.daily.ccam10-awap_ACCESS1-0Q_rcp85.nc'
        self.max_temperature = 'https://dap.tern.org.au/thredds/ncss/CMIP5QLD/CMIP5_Downscaled_CCAM_QLD10/RCP85/daily_adjusted/MaximumTemperature/tmaxscrAdjust.daily.ccam10-awap_CSIRO-Mk3-6-0Q_rcp85.nc'

        self.createParameter("min_temperature", DM.STRING)
        #self.min_temperature = '/Users/christianurich/Documents/rainfall/climate/tminscrAdjust.daily.ccam10-awap_ACCESS1-0Q_rcp85.nc'
        self.min_temperature ='https://dap.tern.org.au/thredds/ncss/CMIP5QLD/CMIP5_Downscaled_CCAM_QLD10/RCP85/daily_adjusted/MinimumTemperature/tminscrAdjust.daily.ccam10-awap_CSIRO-Mk3-6-0Q_rcp85.nc'

        self.city = ViewContainer("city", DM.COMPONENT, DM.MODIFY)

        self.registerViewContainers([self.city])


    def init(self):
        datastream = []

        self.city = ViewContainer("city", DM.COMPONENT, DM.MODIFY)
        self.city.addAttribute("long", DM.Attribute.DOUBLE, DM.READ)
        self.city.addAttribute("lat", DM.Attribute.DOUBLE, DM.READ)

        self.city.addAttribute("cs_mean_temperature", DM.Attribute.STRING, DM.READ)
        self.city.addAttribute("cs_max_temperature", DM.Attribute.STRING, DM.READ)
        self.city.addAttribute("cs_min_temperature", DM.Attribute.STRING, DM.READ)
        self.city.addAttribute("temperature_exceedance_fraction", DM.Attribute.DOUBLE, DM.READ)

        self.city.addAttribute("start_period", DM.Attribute.INT, DM.READ)
        self.city.addAttribute("end_period", DM.Attribute.INT, DM.READ)

        if not self.from_projection:
            self.city.addAttribute("cs_mean_temperatures", DM.Attribute.STRING, DM.READ)
            self.city.addAttribute("cs_max_temperatures", DM.Attribute.STRING, DM.READ)
            self.city.addAttribute("cs_min_temperatures", DM.Attribute.STRING, DM.READ)
            self.city.addAttribute("cs_start_date", DM.Attribute.STRING, DM.READ)

        self.registerViewContainers([self.city])

    def load_temperature_data(self):
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
            return t

        log("Load data from station " + str(station_id), Standard)

        self.timeseries.setAttributeFilter("temperature_station_id = " + str(station_id))
        self.timeseries.reset_reading()
        data = {}
        for t in self.timeseries:
            if t.GetFieldAsInteger("temperature_station_id") == station_id:
                data[t.GetFieldAsString("type")] = dm_get_double_list(t, "data")


        return data["temperature"]

    def write_temperature_data(self, start_date, end_date, climate_data):
        if not self.from_temperature_station:
            log("No temperature station defined loading default values", Warning)
            return

        station_id = -1
        self.temperature_station.reset_reading()
        for s in self.temperature_station:
            station_id = s.GetFID()

        if station_id < 0:
            log("No temperature station defined loading default values", Warning)
            # self.timeseries.finalise()
            # self.temperature_station.finalise()


        log("Load data from station " + str(station_id), Standard)

        self.timeseries.setAttributeFilter("temperature_station_id = " + str(station_id))
        self.timeseries.reset_reading()
        for t in self.timeseries:
            if t.GetFieldAsInteger("temperature_station_id") == station_id:
                if t.GetFieldAsString("type") == "temperature":
                    t.SetField("temperature_station_id", -1)

        timeseries = self.timeseries.create_feature()
        timeseries.SetField("start", str(start_date))
        timeseries.SetField("end", str(end_date))
        timeseries.SetField("temperature_station_id", station_id)
        print(climate_data)
        dm_set_double_list(timeseries, "data", climate_data)
        timeseries.SetField("type", "temperature")
        timeseries.SetField("timestep", 60*30)

        self.timeseries.finalise()
        self.temperature_station.finalise()

    def init(self):

        datastream = []

        self.temperature_station = ViewContainer("temperature_station", DM.COMPONENT, DM.READ)
        self.temperature_station.addAttribute("dance_station_id", DM.Attribute.INT, DM.READ)

        self.timeseries = ViewContainer("timeseries", DM.COMPONENT, DM.READ)
        self.timeseries.addAttribute("data", DM.Attribute.DOUBLEVECTOR, DM.MODIFY)
        self.timeseries.addAttribute("temperature_station_id", DM.Attribute.INT, DM.READ)
        self.timeseries.addAttribute("start", DM.Attribute.STRING, DM.MODIFY)

        datastream.append(self.city)
        datastream.append(self.timeseries)
        datastream.append(self.temperature_station)

        self.registerViewContainers(datastream)

        self.datasets = {}


        self.datasets[
            "tscr_aveAdjust"] = self.mean_temperature
        self.datasets[
            "tminscrAdjust"] = self.min_temperature
        self.datasets[
            "tmaxscrAdjust"] = self.max_temperature


    def extract_mean_timeseries(self, key, long, lat):

        url = f"{self.datasets[key]}?var={key}&latitude={lat}&longitude={long}&horizStride=1&time_start=1980-01-01T15%3A00%3A00Z&time_end=2099-12-31T15%3A00%3A00Z&timeStride=1&accept=CSV"
        log(url, Standard)
        r = requests.get(url)
        log(r.status_code, Standard)
        log("returned request", Standard)
        return pd.read_csv(StringIO(r.text)).iloc[:, [3]].to_numpy() - 273.15

    def get_3day_average(self, df, start_year, end_year, fraction):
        sorted_values = df.loc[(df[0] > f'{start_year}-01-01') & (df[0] < f'{end_year}-12-30')].sort_values(by='rolling_mean2',
                                                                                                   ascending=False)
        row = int(fraction * len(sorted_values))
        return sorted_values.iloc[row]

    def characteristics_day(self, c, d):
        min_value = c[d * 48:(d + 1) * 48].min()
        max_value = c[d * 48:(d + 1) * 48].max()
        return (c[d * 48:(d + 1) * 48] - min_value) / (max_value - min_value)

    def generate_from_min_max(self, c, min_values, max_values, start_date):

        time_series_data = []
        for i in range(3):
            unit_type = self.characteristics_day(c, i)
            unit_type = pd.DataFrame(unit_type).reset_index()
            print( min_values[i], max_values[i])
            time_series_data.append([unit_type, min_values[i], max_values[i]])
        start = datetime.datetime.strptime(start_date, "%Y-%m-%d")
        return str(start), str(start + timedelta(days=3)), self.generate(time_series_data)

    def extract_extreme_event(self, climate_data, c, start, end):
        hihgest = self.get_3day_average(climate_data, start, end, self.fraction)
        days = climate_data.loc[(climate_data[0] > hihgest[0] - pd.DateOffset(days=3)) & (climate_data[0] <= hihgest[0])]
        time_series_data = []

        for i in range(3):
            unit_type = self.characteristics_day(c, i)

            unit_type = pd.DataFrame(unit_type).reset_index()
            print("unit_type", days.iloc[i]['tminscrAdjust'], days.iloc[i]['tmaxscrAdjust'])
            time_series_data.append([unit_type, days.iloc[i]['tminscrAdjust'], days.iloc[i]['tmaxscrAdjust']])
        return str(hihgest[0] - pd.DateOffset(days=3)), str(hihgest[0]), self.generate(time_series_data)

    def genreate_unit_curve(self, unit_curve, min_value, max_value):
        return (unit_curve * (max_value - min_value) + min_value)[0].tolist()

    def generate(self, time_series_data):
        timeseries_new = None
        for ts in time_series_data:
            df = pd.DataFrame(self.genreate_unit_curve(ts[0], ts[1], ts[2]))
            if timeseries_new is None:
                timeseries_new = df
                continue
            timeseries_new = timeseries_new.append(df)
        return timeseries_new[0].tolist()

    def to_vector(self, st):
        st = st.replace("[", "").replace("]", "")
        print(st)
        return [float(d) for d in st.split(",")]

    def run(self):
        dr = pd.date_range(start='1/1/1980', end='12/31/2099', freq='1d')
        dates = dr[(dr.day != 29) | (dr.month != 2)]

        data = {}
        for t in self.city:
            lat = t.GetFieldAsDouble("lat")
            long = t.GetFieldAsDouble("long")
            start_period = t.GetFieldAsInteger("start_period")
            end_period = t.GetFieldAsInteger("end_period")
            self.fraction = t.GetFieldAsDouble("temperature_exceedance_fraction")

            self.datasets[
                "tscr_aveAdjust"] = t.GetFieldAsString("cs_mean_temperature")
            self.datasets[
                "tminscrAdjust"] = t.GetFieldAsString("cs_min_temperature")
            self.datasets[
                "tmaxscrAdjust"] = t.GetFieldAsString("cs_max_temperature")

            cs_mean_temperatures = None
            cs_max_temperatures = None
            cs_min_temperatures = None
            cs_start_date = None

            cs_mean_temperatures = self.to_vector(t.GetFieldAsString("cs_mean_temperatures"))
            cs_max_temperatures = self.to_vector(t.GetFieldAsString("cs_max_temperatures"))
            cs_min_temperatures = self.to_vector(t.GetFieldAsString("cs_min_temperatures"))
            cs_start_date = t.GetFieldAsString("cs_start_date")


        self.city.finalise()

        if start_period < 1900:
            self.timeseries.finalise()
            self.temperature_station.finalise()
            print("return because irrelevant")
            return

        temperature_data = self.load_temperature_data()
        c = pd.DataFrame(temperature_data)

        if len(cs_max_temperatures) == 3:
            print(cs_min_temperatures,cs_max_temperatures)
            start_date, end_date, data =  self.generate_from_min_max(c, cs_min_temperatures,cs_max_temperatures,  cs_start_date)
            self.write_temperature_data(start_date, end_date, data)
            return

        climate_data = pd.DataFrame(dates)
        climate_data = climate_data.assign(tscr_aveAdjust=self.extract_mean_timeseries("tscr_aveAdjust", long, lat))
        climate_data = climate_data.assign(tminscrAdjust=self.extract_mean_timeseries("tminscrAdjust", long, lat))
        climate_data = climate_data.assign(tmaxscrAdjust=self.extract_mean_timeseries("tmaxscrAdjust", long, lat))

        # other statistics
        climate_data['rolling_mean2'] = climate_data['tscr_aveAdjust'].rolling(window=3).mean()

        start_date, end_date, data = self.extract_extreme_event(climate_data, c, start_period, end_period)
        self.write_temperature_data(start_date, end_date, data)

