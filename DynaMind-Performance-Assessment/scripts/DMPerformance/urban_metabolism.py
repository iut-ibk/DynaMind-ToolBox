import sys
import logging
import time


from pydynamind import *
from osgeo import ogr
from datetime import datetime

logging.basicConfig(level=logging.INFO)
from wbmodel import WaterCycleModel, Streams, LotStream, SoilParameters, UnitFlows, DemandProfile, annual_sum

class UrbanMetabolismModel(Module):

    display_name = "Urban Metabolism Model"
    group_name = "Performance Assessment"

    def __init__(self):

        Module.__init__(self)
        self._templates = {}
        self._storages = {}
        self.setIsGDALModule(True)

        self.createParameter("from_rain_station", DM.BOOL)
        self.from_rain_station = False

    def init(self):

        self.lot = ViewContainer('wb_lot', DM.COMPONENT, DM.READ)
        self.lot.addAttribute("persons", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("roof_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("outdoor_imp", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("garden_area", DM.Attribute.DOUBLE, DM.READ)
        # self.lot.addAttribute("tree_cover", DM.Attribute.DOUBLE, DM.READ)

        for i in range(1, 10):
            self.lot.addAttribute(f"wb_sub_catchment_id_{i}", DM.Attribute.INT, DM.READ)

        self.lot.addAttribute("demand", DM.Attribute.DOUBLE, DM.WRITE)
        self.lot.addAttribute("wb_lot_template_id", DM.Attribute.INT, DM.READ)
        self.lot.addAttribute("provided_volume", DM.Attribute.DOUBLE, DM.WRITE)
        self.lot.addAttribute("wb_soil_id", DM.Attribute.INT, DM.READ)
        self.lot.addAttribute("green_roof_id", DM.Attribute.INT, DM.READ)
        self.lot.addAttribute("wb_demand_profile_id", DM.Attribute.INT, DM.READ)

        for s in LotStream:
            self.lot.addAttribute(str(s).split(".")[1], DM.Attribute.DOUBLE, DM.WRITE)

        self.wb_lot_storages=ViewContainer("wb_lot_storages", DM.COMPONENT, DM.WRITE)
        self.wb_lot_storages.addAttribute('provided_volume', DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.wb_lot_storages.addAttribute('storage_behaviour', DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.wb_lot_storages.addAttribute('spills', DM.Attribute.INT, DM.WRITE)
        self.wb_lot_storages.addAttribute('dry', DM.Attribute.INT, DM.WRITE)
        self.wb_lot_storages.addAttribute('parcel_id', DM.Attribute.INT, DM.WRITE)
        self.wb_lot_storages.addAttribute('storage_id', DM.Attribute.INT, DM.WRITE)

        self.wb_lot_template = ViewContainer('wb_lot_template', DM.COMPONENT, DM.READ)

        self.wb_lot_streams = ViewContainer('wb_lot_streams', DM.COMPONENT, DM.READ)
        self.wb_lot_streams.addAttribute('wb_lot_template_id', DM.Attribute.INT, DM.READ)
        self.wb_lot_streams.addAttribute('outflow_stream_id', DM.Attribute.INT, DM.READ)
        self.wb_lot_streams.addAttribute('lot_stream_id', DM.Attribute.INT, DM.READ)

        self.wb_storages = ViewContainer('wb_storages', DM.COMPONENT, DM.READ)
        self.wb_storages.addAttribute("wb_lot_template_id", DM.Attribute.INT, DM.READ)
        self.wb_storages.addAttribute("inflow_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_storages.addAttribute("demand_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_storages.addAttribute("demand_stream_1_id", DM.Attribute.INT, DM.READ)
        self.wb_storages.addAttribute("demand_stream_2_id", DM.Attribute.INT, DM.READ)
        self.wb_storages.addAttribute("volume", DM.Attribute.DOUBLE, DM.READ)
        self.wb_storages.addAttribute('provided_volume', DM.Attribute.DOUBLE, DM.WRITE)

        self.wb_sub_catchments = ViewContainer('wb_sub_catchment', DM.COMPONENT, DM.READ)
        self.wb_sub_catchments.addAttribute('stream', DM.Attribute.INT, DM.READ)
        self.wb_sub_catchments.addAttribute('annual_flow', DM.Attribute.DOUBLE, DM.WRITE)
        self.wb_sub_catchments.addAttribute('daily_flow', DM.Attribute.DOUBLEVECTOR, DM.WRITE)

        self.wb_sub_storages = ViewContainer('wb_sub_storages', DM.COMPONENT, DM.READ)
        self.wb_sub_storages.addAttribute("inflow_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_sub_storages.addAttribute("demand_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_sub_storages.addAttribute("demand_stream_1_id", DM.Attribute.INT, DM.READ)
        self.wb_sub_storages.addAttribute("demand_stream_2_id", DM.Attribute.INT, DM.READ)
        self.wb_sub_storages.addAttribute("volume", DM.Attribute.DOUBLE, DM.READ)
        self.wb_sub_storages.addAttribute('provided_volume', DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.wb_sub_storages.addAttribute('storage_behaviour', DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.wb_sub_storages.addAttribute('spills', DM.Attribute.INT, DM.WRITE)
        self.wb_sub_storages.addAttribute('dry', DM.Attribute.INT, DM.WRITE)

        # self.wb_lot_to_sub_catchments = ViewContainer('wb_lot_to_sub_catchments', DM.COMPONENT, DM.READ)
        # self.wb_lot_to_sub_catchments.addAttribute('wb_sub_catchment_id', DM.Attribute.LINK, DM.READ)
        # self.wb_lot_to_sub_catchments.addAttribute('parcel_id', DM.Attribute.LINK, DM.READ)

        self.green_roofs = ViewContainer('green_roof', DM.COMPONENT, DM.READ)
        self.green_roofs.addAttribute("wb_soil_id", DM.Attribute.INT, DM.READ)

        self.wb_soil_parameters = ViewContainer('wb_soil', DM.COMPONENT, DM.READ)

        for s in SoilParameters:
            self.wb_soil_parameters.addAttribute(str(s).split(".")[1], DM.Attribute.DOUBLE, DM.READ)

        self.wb_unit_flows = ViewContainer('wb_unit_flow', DM.COMPONENT, DM.WRITE)
        for s in UnitFlows:
            self.wb_unit_flows.addAttribute("wb_soil_id", DM.Attribute.INT, DM.WRITE)
            self.wb_unit_flows.addAttribute("station_id", DM.Attribute.INT, DM.WRITE)
            self.wb_unit_flows.addAttribute("wb_demand_profile_id", DM.Attribute.INT, DM.WRITE)
            self.wb_unit_flows.addAttribute(str(s).split(".")[1], DM.Attribute.DOUBLEVECTOR, DM.WRITE)

        self.wb_demand_profile = ViewContainer('wb_demand_profile', DM.COMPONENT, DM.READ)
        for s in DemandProfile:
            self.wb_demand_profile.addAttribute(str(s).split(".")[1], DM.Attribute.DOUBLE, DM.READ)

        # need multiple streams per lot
        # lot can be part of multiple sub catchment (1 for each stream)
        # key = (sub_catchment, stream)

        view_register = [self.lot,
                         self.wb_soil_parameters,
                         self.wb_lot_template,
                         self.wb_storages,
                         self.wb_sub_storages,
                         self.wb_sub_catchments,
                         # self.wb_lot_to_sub_catchments,
                         self.wb_lot_streams,
                         self.wb_lot_storages,
                         self.wb_demand_profile,
                         self.green_roofs,
                         self.wb_unit_flows
                         ]

        if self.from_rain_station:
            self.timeseries = ViewContainer("timeseries", DM.COMPONENT, DM.READ)
            self.timeseries.addAttribute("data", DM.Attribute.DOUBLEVECTOR, DM.READ)
            self.timeseries.addAttribute("station_id", DM.Attribute.INT, DM.READ)
            self.timeseries.addAttribute("type", DM.Attribute.STRING, DM.READ)
            self.timeseries.addAttribute("start", Attribute.STRING, READ)
            self.timeseries.addAttribute("end", Attribute.STRING, READ)
            self.lot.addAttribute("station_id", DM.Attribute.INT, DM.READ)

            view_register.append(self.timeseries)
        self.registerViewContainers(view_register)

    def run(self):
        lots = {}
        # collect all lot data
        lot_streams = {}

        soils = {}
        for s in self.wb_soil_parameters:
            soil_id = s.GetFID()
            soil = {}
            for p in SoilParameters:
                soil[p] = s.GetFieldAsDouble(str(p).split(".")[1])
            soils[soil_id] = soil

        demand_profile = {}
        for s in self.wb_demand_profile:
            demand_id = s.GetFID()
            profile = {}
            for p in DemandProfile:
                profile[p] = s.GetFieldAsDouble(str(p).split(".")[1])
            demand_profile[demand_id] = profile
        self.wb_demand_profile.finalise()

        for s in self.wb_lot_streams:
            s: ogr.Feature
            wb_template_id = s.GetFieldAsInteger("wb_lot_template_id")
            out_stream = Streams(s.GetFieldAsInteger("outflow_stream_id"))
            lot_stream = LotStream(s.GetFieldAsInteger("lot_stream_id"))

            if s.GetFieldAsInteger("wb_lot_template_id") not in lot_streams:
                lot_streams[wb_template_id] = {}

            if out_stream not in lot_streams[wb_template_id]:
                lot_streams[wb_template_id][out_stream] = []
            lot_streams[wb_template_id][out_stream].append(lot_stream)
        self.wb_lot_streams.finalise()

        self._templates = {}

        for template in self.wb_lot_template:
            template : ogr.Featured
            self._templates[template.GetFID()] = {"streams": lot_streams[template.GetFID()]}

        self.wb_lot_template.finalise()

        sub_catchments = {}
        sub_catchments_lots = {}

        for s in self.wb_sub_catchments:
            s: ogr.Feature
            sub_catchments[s.GetFID()] = Streams(s.GetFieldAsInteger("stream"))
            sub_catchments_lots[s.GetFID()] = []

        # Lot scale storages
        for s in self.wb_storages:
            s : ogr.Feature
            template_id = s.GetFieldAsInteger("wb_lot_template_id")

            if template_id not in self._storages:
                self._storages[template_id] = []

            storage = {"inflow": LotStream(s.GetFieldAsInteger("inflow_stream_id")),
                       "volume": s.GetFieldAsInteger("volume"),
                       "id": s.GetFID()}
            if s.GetFieldAsInteger("demand_stream_id") > 0:
                storage["demand"] = LotStream(s.GetFieldAsInteger("demand_stream_id"))
            if s.GetFieldAsInteger("demand_stream_1_id") > 0:
                storage["demand_1"] = LotStream(s.GetFieldAsInteger("demand_stream_1_id"))
            if s.GetFieldAsInteger("demand_stream_2_id") > 0:
                storage["demand_2"] = LotStream(s.GetFieldAsInteger("demand_stream_2_id"))

            storages = self._storages[template_id]
            storages.append(storage)

        wb_sub_storages = {}
        for s in self.wb_sub_storages:
            s: ogr.Feature
            storage = {"inflow": "sub_" + str(s.GetFieldAsInteger("inflow_stream_id")),
                       "volume": s.GetFieldAsInteger("volume"),
                       "id" : s.GetFID()}

            if s.GetFieldAsInteger("demand_stream_id") > 0:
                storage["demand"] = "sub_" + str(s.GetFieldAsInteger("demand_stream_id"))
            if s.GetFieldAsInteger("demand_stream_1_id") > 0:
                storage["demand_1"] = "sub_" + str(s.GetFieldAsInteger("demand_stream_1_id"))
            if s.GetFieldAsInteger("demand_stream_2_id") > 0:
                storage["demand_2"] = "sub_" + str(s.GetFieldAsInteger("demand_stream_2_id"))

            wb_sub_storages[s.GetFID()] = storage

        green_roofs = {}
        for s in self.green_roofs:
            s: ogr.Feature
            green_roof = {"soil_id": s.GetFieldAsInteger("wb_soil_id")}
            green_roofs[s.GetFID()] = green_roof
        self.green_roofs.finalise()

        for l in self.lot:
            l : ogr.Feature

            if l.GetFieldAsInteger("wb_lot_template_id") not in self._storages:
                self._storages[l.GetFieldAsInteger("wb_lot_template_id")] = []

            green_roof = None
            if l.GetFieldAsInteger("green_roof_id") > 0:
                green_roof = green_roofs[l.GetFieldAsInteger("green_roof_id")]
            station_id = 1
            if self.from_rain_station:
                station_id = l.GetFieldAsInteger("station_id")
            lot = {
                "persons": l.GetFieldAsDouble("persons"),
                "units": l.GetFieldAsInteger("units"),
                "roof_area": l.GetFieldAsDouble("roof_area"),
                "impervious_area": l.GetFieldAsDouble("outdoor_imp"),
                "irrigated_garden_area": l.GetFieldAsDouble("garden_area"),
                "area": l.GetFieldAsDouble("area"),
                "soil_id": l.GetFieldAsDouble("wb_soil_id"),
                "wb_demand_profile_id": l.GetFieldAsInteger("wb_demand_profile_id"),
                "streams":
                    self._templates[l.GetFieldAsInteger("wb_lot_template_id")]["streams"],
                "storages": self._storages[l.GetFieldAsInteger("wb_lot_template_id")],
                "green_roof": green_roof,
                "station_id": station_id,
            }

            lots[l.GetFID()] = lot

        self.lot.reset_reading()
        for lot_sub_catchments in self.lot:
            lot_sub_catchments: ogr.Feature
            parcel_id = lot_sub_catchments.GetFID()
            for i in range(1, 10):
                wb_sub_catchment_id = lot_sub_catchments.GetFieldAsInteger(f"wb_sub_catchment_id_{i}")
                if wb_sub_catchment_id == 0:
                    wb_sub_catchment_id = i
                sub_catchments_lots[wb_sub_catchment_id].append(parcel_id)


        stations, dates = self._load_station()


        wb = WaterCycleModel(lots=lots,
                             sub_catchments=sub_catchments,
                             wb_lot_to_sub_catchments=sub_catchments_lots,
                             wb_sub_storages=wb_sub_storages,
                             wb_demand_profile = demand_profile,
                             soils=soils,
                             stations=stations,
                             dates=dates,
                             library_path=self.getSimulationConfig().getDefaultLibraryPath())

        self.wb_soil_parameters.finalise()

        self.wb_sub_catchments.reset_reading()
        for s in self.wb_sub_catchments:
            daily_flow = wb.get_sub_daily_flow(s.GetFID())
            if not daily_flow:
                continue

            logging.info(
                f"{s.GetFID()} {str(Streams(s.GetFieldAsInteger('stream')))} {format(sum(daily_flow), '.2f')}")

            s.SetField("annual_flow", annual_sum(daily_flow))
            dm_set_double_list(s, 'daily_flow', daily_flow)
        self.wb_sub_catchments.finalise()

        # Write lot storages
        self.wb_storages.reset_reading()
        for s in self.wb_storages:
            s : ogr.Feature
            s.SetField("provided_volume", wb.get_internal_storage_volumes(s.GetFID()))

        self.wb_storages.finalise()

        self.wb_sub_storages.reset_reading()
        for s in self.wb_sub_storages:
            s : ogr.Feature
            storage = wb.get_storage(s.GetFID())

            s.SetField('spills', storage.get_state_value_as_int('spills'))
            s.SetField('dry', storage.get_state_value_as_int('dry'))
            dm_set_double_list(s, 'provided_volume', storage.get_state_value_as_double_vector('provided_volume'))
            dm_set_double_list(s, 'storage_behaviour', storage.get_state_value_as_double_vector('storage_behaviour'))

            logging.debug(
                f"{s.GetFID()} provided_volume: {format(sum(storage.get_state_value_as_double_vector('provided_volume')), '.2f')}")
            logging.debug(
                f"{s.GetFID()} storage_behaviour: {format(sum(storage.get_state_value_as_double_vector('storage_behaviour')), '.2f')}")
            logging.debug(
                f"{s.GetFID()} spills: {format(storage.get_state_value_as_int('spills'), '.2f')}")
            logging.debug(
                f"{s.GetFID()} dry: {format(storage.get_state_value_as_int('dry'), '.2f')}")

        self.wb_sub_storages.finalise()
        self.lot.reset_reading()

        for keys, item in wb.get_standard_values().items():
            f: ogr.Feature
            soil_id = keys[0]
            station_id = keys[1]
            wb_demand_profile_id = keys[2]

            f = self.wb_unit_flows.create_feature()
            f.SetField("wb_soil_id", soil_id)
            f.SetField("station_id", station_id)
            f.SetField("wb_demand_profile_id", wb_demand_profile_id)
            for s in UnitFlows:
                dm_set_double_list(f, str(s).split(".")[1],
                                   item[s])
        self.wb_unit_flows.finalise()

        for l in self.lot:
            l.SetField("provided_volume", wb.get_internal_storage_volumes_lot(l.GetFID()))
            l_id = l.GetFID()
            for key, storage in wb.get_internal_storages(l_id).items():
                s = self.wb_lot_storages.create_feature()
                s.SetField('parcel_id', l.GetFID())
                s.SetField('storage_id', key)
                s.SetField('spills', storage.get_state_value_as_int('spills'))
                s.SetField('dry', storage.get_state_value_as_int('dry'))
                dm_set_double_list(s, 'provided_volume', storage.get_state_value_as_double_vector('provided_volume'))
                dm_set_double_list(s, 'storage_behaviour', storage.get_state_value_as_double_vector('storage_behaviour'))
                logging.debug(
                    f"{s.GetFID()} provided_volume: {format(sum(storage.get_state_value_as_double_vector('provided_volume')), '.2f')}")
                logging.debug(
                    f"{s.GetFID()} storage_behaviour: {format(sum(storage.get_state_value_as_double_vector('storage_behaviour')), '.2f')}")
                logging.debug(
                    f"{s.GetFID()} spills: {format(storage.get_state_value_as_int('spills'), '.2f')}")
                logging.debug(
                    f"{s.GetFID()} dry: {format(storage.get_state_value_as_int('dry'), '.2f')}")
            for stream in LotStream:
                l.SetField(str(stream).split(".")[1], float(wb.get_parcel_internal_stream_volumes(l.GetFID(), stream)))
        self.lot.finalise()
        self.wb_lot_storages.finalise()

    def _load_station(self):
        """
        Load rainfall stations from database
        :return: dict with timeseries key is the station id
        """
        stations = {}

        if not self.from_rain_station:
            stations[1] = {
                "rainfall intensity": self._load_rainfall(),
                "evapotranspiration": self._load_eta()
            }

        start_date = "2001-Jan-01 00:00:00"
        end_date = "2002-Jan-01 00:00:00"

        if self.from_rain_station:
            for t in self.timeseries:
                t: ogr.Feature
                series = [0. if v < 0. else v/1000. for v in DM.dm_get_double_list(t, "data")]

                type = t.GetFieldAsString("type")
                station_id = t.GetFieldAsInteger("station_id")
                if station_id == 0:
                    continue
                if station_id in stations:
                    station = stations[station_id]
                else:
                    station = {}
                station[type] = series
                stations[station_id] = station
                start_date = datetime.strptime(t.GetFieldAsString("start"), '%d.%m.%Y %H:%M:%S').strftime('%Y-%b-%d %H:%M:%S')
                end_date = datetime.strptime(t.GetFieldAsString("end"), '%d.%m.%Y %H:%M:%S').strftime('%Y-%b-%d %H:%M:%S')
            self.timeseries.finalise()

        return stations, (start_date, end_date)

    def get_default_folder(self):
        return self.getSimulationConfig().getDefaultLibraryPath()

    def _load_rainfall(self):
        with open(self.get_default_folder() + '/Data/Raindata/melb_rain_24.ixx') as f:
            rainfall = f.read()
        return [float(r.split("\t")[4]) / 1000. for r in rainfall.splitlines()]

    def _load_eta(self):
        with open(self.get_default_folder() + '/Data/Raindata/melb_eva_24.ixx') as f:
            rainfall = f.read()
        return [float(r.split("\t")[1]) / 1000. for r in rainfall.splitlines()]


def _create_lot(id):
        """
        User for testing
        :param id:
        :return:
        """
        return {
            "id": id,
            "persons": 5,
            "roof_area": 200,
            "impervious_area": 200,
            "irrigated_garden_area": 200,
            "streams": {
                Streams.potable_demand: [LotStream.potable_demand],
                # Streams.non_potable_demand: [LotStream.non_potable_demand],
                # Streams.outdoor_demand: [LotStream.outdoor_demand],
                # Streams.sewerage: [LotStream.black_water],
                # Streams.grey_water: [LotStream.grey_water],
                # Streams.stormwater_runoff: [LotStream.roof_runoff, LotStream.impervious_runoff],
                # Streams.infiltration: [LotStream.infiltration],
                # Streams.evapotranspiration: [LotStream.evapotranspiration]
        }
            # "storages": [{"inflow": LotStream.roof_runoff, "demand": LotStream.outdoor_demand, "volume": 5},
            #              {"inflow": LotStream.grey_water, "demand": LotStream.non_potable_demand, "volume": 0.5}]
        }


if __name__ == '__main__':
    sys.path.insert(0, "/Users/christianurich/Documents/dynamind/build/output/")

    for s in [10, 100]:
        start = time.time()
        lots = {}
        for i in range(s):
            lots[i] = (_create_lot(i))
        WaterCycleModel(lots=lots, library_path="/Users/christianurich/Documents/dynamind/build/output/")
        print(s, time.time() - start)


