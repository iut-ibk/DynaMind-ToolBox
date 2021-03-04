from pydynamind import *
from netCDF4 import Dataset
import pandas as pd
# import xarray as xr
import requests
from io import StringIO
import os
import subprocess
from datetime import datetime, timedelta
import datetime

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

    def get_monthly_evapotranspiration(self, x, y):
        """
        Returns evapotranspiration time series from BOM data. X and Y are in 4326 coordinates
        :param x:
        :param y:
        :return:
        """
        files = ["Data/bom_data/etaajan.tif",
                 "Data/bom_data/etaafeb.tif",
                 "Data/bom_data/etaamar.tif",
                 "Data/bom_data/etaaapr.tif",
                 "Data/bom_data/etaamay.tif",
                 "Data/bom_data/etaajun.tif",
                 "Data/bom_data/etaajul.tif",
                 "Data/bom_data/etaaaug.tif",
                 "Data/bom_data/etaasep.tif",
                 "Data/bom_data/etaaoct.tif",
                 "Data/bom_data/etaanov.tif",
                 "Data/bom_data/etaadec.tif"
                 ]
        data = []
        rootfolder = self.getSimulationConfig().getDefaultModulePath()
        for f in files:
            cmd = "gdallocationinfo -valonly  -wgs84 " + f"{rootfolder}/{f}" + " " + str(x) + " " + str(y)
            data.append(float(subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE).stdout.read()))
        return data

    def create_evapotranspiration(self, longitude, latitude, start, end, delta):
        monthy_evapo = self.get_monthly_evapotranspiration(longitude, latitude)
        print(monthy_evapo)
        counter = 0
        current_date = start
        current_month = -1
        values = []
        while current_date < end:
            counter += 1
            if current_month != current_date.month:
                this_month = datetime.datetime(current_date.year, current_date.month, current_date.day)
                if current_date.month < 12:
                    next_month = datetime.datetime(current_date.year, current_date.month + 1, current_date.day)
                else:
                    next_month = datetime.datetime(current_date.year + 1, 1, current_date.day)
                days_in_this_month = next_month - this_month

                delta_eva = monthy_evapo[current_month - 1] / float(days_in_this_month.days) / (
                            24. * 60. * 60.) * delta.total_seconds()

                current_month = current_date.month
            values.append(delta_eva)

            current_date += delta

        return values

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

        # Exceedance fraction is 0, 0.99 if set to -1 to will extract the time series defined in the period
        self.city.addAttribute("rainfall_exceedance_fraction", DM.Attribute.DOUBLE, DM.READ)



        self.city.addAttribute("rain_start_period", DM.Attribute.INT, DM.READ)
        self.city.addAttribute("rain_end_period", DM.Attribute.INT, DM.READ)

        self.city.addAttribute("netcdf_start_year", DM.Attribute.INT, DM.READ)
        self.city.addAttribute("netcdf_end_year", DM.Attribute.INT, DM.READ)

        self.city.addAttribute("leap_year", DM.Attribute.INT, DM.READ)

        self.city.addAttribute("wb_start_date", DM.Attribute.STRING, DM.WRITE)
        self.city.addAttribute("wb_end_date", DM.Attribute.STRING, DM.WRITE)


        datastream.append(self.city)
        datastream.append(self.timeseries)
        datastream.append(self.station)

        self.registerViewContainers(datastream)

        self.datasets = {}
        self.datasets[
            "rnd24Adjust"] = self.daily_rainfall

    def convert_long_lat(self, d, x, y):
        lon = d.variables["lon"]
        lat = d.variables["lat"]
        min_lon = min(lon[:])
        max_lon = max(lon[:])
        min_lat = min(lat[:])
        max_lat = max(lat[:])

        total_l = (max_lon - min_lon) / lon.size
        total_h = (max_lat - min_lat) / lat.size

        offset_x = x - min_lon
        offset_y = y - min_lat

        return offset_x / total_l, offset_y / total_h

    def extract_mean_timeseries_netcdf(self, key, long, lat):
        os.environ['HDF5_USE_FILE_LOCKING'] = "FALSE"
        try:
            d = Dataset(self.datasets[key])
        except Exception as err:
            log(f"Can't connect to database {self.datasets[key]}", Error)
            log(f"Error: {type(err).__name_}", Error)

        lo, la = self.convert_long_lat(d, long, lat)
        variable_name = list(d.variables.keys())[-1]

        return pd.DataFrame(d[variable_name][:, int(la), int(lo)]).to_numpy() * 60. * 60. * 24.


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

        for t in self.city:
            lat = t.GetFieldAsDouble("lat")
            long = t.GetFieldAsDouble("long")
            start_period = t.GetFieldAsInteger("rain_start_period")
            end_period = t.GetFieldAsInteger("rain_end_period")
            fraction = t.GetFieldAsDouble("rainfall_exceedance_fraction")

            self.city.addAttribute("netcdf_start_year", DM.Attribute.INT, DM.READ)
            self.city.addAttribute("netcdf_end_year", DM.Attribute.INT, DM.READ)

            self.city.addAttribute("leap_year", DM.Attribute.INT, DM.READ)

            netcdf_start_year = t.GetFieldAsInteger("netcdf_start_year")
            netcdf_end_year = t.GetFieldAsInteger("netcdf_end_year")
            leap_year = t.GetFieldAsInteger("leap_year")

            if fraction < 0:
                start_export = start_period
                end_export = end_period + 1
            else:
                start_export = 2001
                end_export = start_export + 1


            t.SetField("wb_start_date", f"\"{start_export}-Jan-01 00:00:00\"")
            t.SetField("wb_end_date", f"\"{end_export}-Jan-01 00:00:00\"")



            self.datasets[
                "rnd24Adjust"] = t.GetFieldAsString("cs_rainfall")

        self.city.finalise()

        dr = pd.date_range(start=f'1/1/{netcdf_start_year}', end=f'12/31/{netcdf_end_year}', freq='1d')
        if leap_year == 0:
            dates = dr
        else:
            dates = dr[(dr.day != 29) | (dr.month != 2)]


        if start_period < 1900:
            self.timeseries.finalise()
            self.station.finalise()
            print("return because irrelevant")
            return

        climate_data = pd.DataFrame(dates)

        if "http://" in self.datasets["rnd24Adjust"]:
            # assume to download data from the api
            climate_data = climate_data.assign(rnd24Adjust=self.extract_mean_timeseries("rnd24Adjust", long, lat))
        else:
            climate_data = climate_data.assign(rnd24Adjust=self.extract_mean_timeseries_netcdf("rnd24Adjust", long,lat))

        climate_data = climate_data.set_index(0)

        if fraction < 0:
            year = start_period
            end_year = end_period+1
        else:
            year = self.get_year(climate_data, start_period, end_period, fraction)
            end_year = year+1

        start = datetime.datetime.strptime(f'{year}-01-01', "%Y-%m-%d")
        end = datetime.datetime.strptime(f'{end_year}-01-01', "%Y-%m-%d")

        rain = climate_data.loc[(climate_data.index >= start) & (climate_data.index < end)]['rnd24Adjust'].to_list()

        # dr = pd.date_range(start=f'{year}-01-01', end=f'{year}-12-31', freq='1d')


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
        timeseries.SetField("start", f'01.01.{start_export} 00:00:00')
        timeseries.SetField("end", f'01.01.{end_export} 00:00:00')
        timeseries.SetField("station_id", station_id)

        log(f"{len(rain)}; {start_period}, {end_period}; start: 01.01.{start_export}; 00:00:00; 01.01.{end_export} 00:00:00",
            Standard)
        dm_set_double_list(timeseries, "data", rain)
        timeseries.SetField("type", "rainfall intensity")
        timeseries.SetField("timestep", 86400)

        evapo = self.create_evapotranspiration(long, lat, datetime.datetime(start_export, 1, 1, 0, 0, 0), datetime.datetime(end_export, 1, 1, 0, 0, 0), timedelta(seconds=60*60*24))
        timeseries = self.timeseries.create_feature()
        timeseries.SetField("start", f'01.01.{start_export} 00:00:00')
        timeseries.SetField("end", f'01.01.{end_export} 00:00:00')
        timeseries.SetField("station_id", station_id)
        dm_set_double_list(timeseries, "data", evapo)
        timeseries.SetField("type", "evapotranspiration")
        timeseries.SetField("timestep", 86400)

        log(
            f"{len(evapo)}; {start_period}, {end_period}; start: 01.01.{start_export}; 00:00:00; 01.01.{end_export} 00:00:00",
            Standard)



        self.timeseries.finalise()
        self.station.finalise()
