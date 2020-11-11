from pydynamind import *
from netCDF4 import Dataset
import pandas as pd
# import xarray as xr
import requests
from io import StringIO
import datetime
from datetime import timedelta

class ClimateProjectionRainfall(Module):
    display_name = "Climate Rainfall Projection"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("from_station", DM.BOOL)
        self.from_station = False

        self.createParameter("from_projection", DM.BOOL)
        self.from_projection = True


        self.createParameter("fraction", DM.DOUBLE)
        self.fraction = 0.0

        self.createParameter("daily_rainfall", DM.STRING)
        self.daily_rainfall = 'https://dap.tern.org.au/thredds/ncss/CMIP5QLD/CMIP5_Downscaled_CCAM_QLD10/RCP85/daily_adjusted/Precipitation/rnd24Adjust.daily.ccam10-awap_ACCESS1-0Q_rcp85.nc'

        self.city = ViewContainer("city", DM.COMPONENT, DM.MODIFY)

        self.registerViewContainers([self.city])

    def init(self):

        datastream = []

        self.station = ViewContainer("station", DM.COMPONENT, DM.READ)
        self.station.addAttribute("dance_station_id", DM.Attribute.INT, DM.READ)

        self.timeseries = ViewContainer("timeseries", DM.COMPONENT, DM.READ)
        self.timeseries.addAttribute("data", DM.Attribute.DOUBLEVECTOR, DM.MODIFY)
        self.timeseries.addAttribute("station_id", DM.Attribute.INT, DM.READ)
        self.timeseries.addAttribute("start", DM.Attribute.STRING, DM.MODIFY)

        self.city = ViewContainer("city", DM.COMPONENT, DM.MODIFY)
        self.city.addAttribute("long", DM.Attribute.DOUBLE, DM.READ)
        self.city.addAttribute("lat", DM.Attribute.DOUBLE, DM.READ)

        self.city.addAttribute("cs_rainfall", DM.Attribute.STRING, DM.READ)
        self.city.addAttribute("rainfall_exceedance_fraction", DM.Attribute.DOUBLE, DM.READ)

        self.city.addAttribute("rain_start_period", DM.Attribute.INT, DM.READ)
        self.city.addAttribute("rain_end_period", DM.Attribute.INT, DM.READ)

        datastream.append(self.city)
        datastream.append(self.timeseries)
        datastream.append(self.station)

        self.registerViewContainers(datastream)

        self.datasets = {}
        self.datasets[
            "rnd24Adjust"] = self.daily_rainfall

    def extract_mean_timeseries(self, key, long, lat):
        url = f"{self.datasets[key]}?var={key}&latitude={lat}&longitude={long}&horizStride=1&time_start=1980-01-01T15%3A00%3A00Z&time_end=2099-12-31T15%3A00%3A00Z&timeStride=1&accept=CSV"
        log(url, Standard)
        r = requests.get(url)
        log(str(r.status_code), Standard)
        log("returned request", Standard)
        return pd.read_csv(StringIO(r.text)).iloc[:, [3]].to_numpy()

    def get_year(self, df, start_year, end_year, fraction):
        s2 = pd.DataFrame(df.groupby([lambda x: x.year]).sum())
        sorted_values = s2.loc[(s2.index > start_year) & (s2.index < end_year)].sort_values(by='rnd24Adjust',
                                                                                            ascending=False)
        row = int(fraction * len(sorted_values))
        return sorted_values.iloc[row].name

    def generate(self, time_series_data):
        timeseries_new = None
        for ts in time_series_data:
            df = pd.DataFrame(self.genreate_unit_curve(ts[0], ts[1], ts[2]))
            if timeseries_new is None:
                timeseries_new = df
                continue
            timeseries_new = timeseries_new.append(df)
        return timeseries_new[0].tolist()

    def run(self):
        dr = pd.date_range(start='1/1/1980', end='12/31/2099', freq='1d')
        dates = dr[(dr.day != 29) | (dr.month != 2)]
        data = {}
        for t in self.city:
            lat = t.GetFieldAsDouble("lat")
            long = t.GetFieldAsDouble("long")
            start_period = t.GetFieldAsInteger("rain_start_period")
            end_period = t.GetFieldAsInteger("rain_end_period")
            fraction = t.GetFieldAsDouble("rainfall_exceedance_fraction")

            self.datasets[
                "rnd24Adjust"] = t.GetFieldAsString("cs_rainfall")

        self.city.finalise()

        if start_period < 1900:
            self.timeseries.finalise()
            self.station.finalise()
            print("return because irrelevant")
            return

        climate_data = pd.DataFrame(dates)
        climate_data = climate_data.assign(rnd24Adjust=self.extract_mean_timeseries("rnd24Adjust", long, lat))
        climate_data = climate_data.set_index(0)
        year = self.get_year(climate_data, start_period, end_period, fraction)

        start = datetime.datetime.strptime(f'{year}-01-01', "%Y-%m-%d")
        end = datetime.datetime.strptime(f'{year+1}-01-01', "%Y-%m-%d")

        rain = climate_data.loc[(climate_data.index >= start) & (climate_data.index < end)]['rnd24Adjust'].to_list()

        dr = pd.date_range(start=f'{year}-01-01', end=f'{year}-12-31', freq='1d')
        # if len(dr) == 366:
        #     rain.append(0)

        if not self.from_station:
            log("No temperature station defined loading default values", Warning)
            return

        station_id = -1
        self.station.reset_reading()
        for s in self.station:
            station_id = s.GetFID()

        if station_id < 0:
            log("No station defined loading default values", Warning)

        log("Load data from station " + str(station_id), Standard)

        self.timeseries.setAttributeFilter("temperature_station_id = " + str(station_id))
        self.timeseries.reset_reading()
        for t in self.timeseries:
            if t.GetFieldAsInteger("station_id") == station_id:
                if t.GetFieldAsString("type") == "rainfall intensity":
                    t.SetField("station_id", 0)

        timeseries = self.timeseries.create_feature()
        timeseries.SetField("start", f'01.01.{2001} 00:00:00')
        timeseries.SetField("end", f'01.01.{2002} 00:00:00')
        timeseries.SetField("station_id", station_id)
        print(rain, len(rain))
        dm_set_double_list(timeseries, "data", rain)
        timeseries.SetField("type", "rainfall intensity")
        timeseries.SetField("timestep", 86400)

        self.timeseries.finalise()
        self.station.finalise()
