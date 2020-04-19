import sys
import logging
import time


from pydynamind import *
from osgeo import ogr

logging.basicConfig(level=logging.INFO)
from wbmodel import WaterCycleModel, Streams, LotStream, SoilParameters, UnitFlows



class UrbanMetabolismModel(Module):

    display_name = "Urban Metabolism Model"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self._templates = {}
        self._storages = {}
        self.setIsGDALModule(True)

    def init(self):
        self.lot = ViewContainer('parcel', DM.COMPONENT, DM.READ)
        self.lot.addAttribute("persons", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("roof_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("outdoor_imp", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("garden_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("demand", DM.Attribute.DOUBLE, DM.WRITE)
        self.lot.addAttribute("wb_lot_template_id", DM.Attribute.INT, DM.READ)
        self.lot.addAttribute("provided_volume", DM.Attribute.DOUBLE, DM.WRITE)
        self.lot.addAttribute("wb_soil_id", DM.Attribute.INT, DM.WRITE)
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
        self.wb_storages.addAttribute("volume", DM.Attribute.DOUBLE, DM.READ)
        self.wb_storages.addAttribute('provided_volume', DM.Attribute.DOUBLE, DM.WRITE)

        self.wb_sub_catchments = ViewContainer('wb_sub_catchment', DM.COMPONENT, DM.READ)
        self.wb_sub_catchments.addAttribute('stream', DM.Attribute.INT, DM.READ)
        self.wb_sub_catchments.addAttribute('annual_flow', DM.Attribute.DOUBLE, DM.WRITE)
        self.wb_sub_catchments.addAttribute('daily_flow', DM.Attribute.DOUBLEVECTOR, DM.WRITE)

        self.wb_sub_storages = ViewContainer('wb_sub_storages', DM.COMPONENT, DM.READ)
        self.wb_sub_storages.addAttribute("inflow_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_sub_storages.addAttribute("demand_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_sub_storages.addAttribute("volume", DM.Attribute.DOUBLE, DM.READ)
        self.wb_sub_storages.addAttribute("provided_volume", DM.Attribute.DOUBLE, DM.WRITE)

        self.wb_lot_to_sub_catchments = ViewContainer('wb_lot_to_sub_catchments', DM.COMPONENT, DM.READ)
        self.wb_lot_to_sub_catchments.addAttribute('wb_sub_catchment_id', DM.Attribute.LINK, DM.READ)
        self.wb_lot_to_sub_catchments.addAttribute('parcel_id', DM.Attribute.LINK, DM.READ)

        self.wb_soil_parameters = ViewContainer('wb_soil', DM.COMPONENT, DM.READ)

        for s in SoilParameters:
            self.wb_soil_parameters.addAttribute(str(s).split(".")[1], DM.Attribute.DOUBLE, DM.READ)

        for s in UnitFlows:
            self.wb_soil_parameters.addAttribute(str(s).split(".")[1], DM.Attribute.DOUBLEVECTOR, DM.WRITE)

        # need multiple streams per lot
        # lot can be part of multiple sub catchment (1 for each stream)
        # key = (sub_catchment, stream)


        view_register = [self.lot,
                         self.wb_soil_parameters,
                         self.wb_lot_template,
                         self.wb_storages,
                         self.wb_sub_storages,
                         self.wb_sub_catchments,
                         self.wb_lot_to_sub_catchments,
                         self.wb_lot_streams,
                         self.wb_lot_storages
                         ]
        #
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
            self._templates[template.GetFID()] = { "streams": lot_streams[template.GetFID()]}

        self.wb_lot_template.finalise()

        sub_catchments = {}
        sub_catchments_lots = {}

        for s in self.wb_sub_catchments:
            s: ogr.Feature
            sub_catchments[s.GetFID()] = Streams(s.GetFieldAsInteger("stream"))
            sub_catchments_lots[s.GetFID()] = []
        # self.wb_sub_catchments.finalise()

        for lot_sub_catchments in self.wb_lot_to_sub_catchments:
            lot_sub_catchments: ogr.Feature
            parcel_id = lot_sub_catchments.GetFieldAsInteger("parcel_id")
            wb_sub_catchment_id = lot_sub_catchments.GetFieldAsInteger("wb_sub_catchment_id")
            sub_catchments_lots[wb_sub_catchment_id].append(parcel_id)
        self.wb_lot_to_sub_catchments.finalise()

        for s in self.wb_storages:
            s : ogr.Feature
            template_id = s.GetFieldAsInteger("wb_lot_template_id")

            if template_id not in self._storages:
                self._storages[template_id] = []

            storage = {"inflow": LotStream(s.GetFieldAsInteger("inflow_stream_id")),
                       "demand": LotStream(s.GetFieldAsInteger("demand_stream_id")),
                       "volume": s.GetFieldAsInteger("volume"),
                       "id": s.GetFID()}

            storages = self._storages[template_id]
            storages.append(storage)


        wb_sub_storages = {}
        for s in self.wb_sub_storages:
            s: ogr.Feature
            storage = {"inflow": "sub_" + str(s.GetFieldAsInteger("inflow_stream_id")),
                       "demand": "sub_" + str(s.GetFieldAsInteger("demand_stream_id")),
                       "volume": s.GetFieldAsInteger("volume"),
                       "id" : s.GetFID()}
            wb_sub_storages[s.GetFID()] = storage

        for l in self.lot:
            l : ogr.Feature

            if l.GetFieldAsInteger("wb_lot_template_id") not in self._storages:
                self._storages[l.GetFieldAsInteger("wb_lot_template_id")] = []
            lot = {
                "persons": l.GetFieldAsDouble("persons"),
                "roof_area": l.GetFieldAsDouble("roof_area"),
                "impervious_area": l.GetFieldAsDouble("outdoor_imp"),
                "irrigated_garden_area": l.GetFieldAsDouble("garden_area"),
                "area": l.GetFieldAsDouble("area"),
                "soil_id": l.GetFieldAsDouble("wb_soil_id"),
                "streams":
                    self._templates[l.GetFieldAsInteger("wb_lot_template_id")]["streams"],
                "storages": self._storages[l.GetFieldAsInteger("wb_lot_template_id")]
            }

            lots[l.GetFID()] = lot

        wb = WaterCycleModel(lots=lots,
                             sub_catchments=sub_catchments,
                             wb_lot_to_sub_catchments=sub_catchments_lots,
                             wb_sub_storages=wb_sub_storages,
                             soils=soils,
                             library_path=self.getSimulationConfig().getDefaultLibraryPath())

        self.wb_soil_parameters.reset_reading()
        for s in self.wb_soil_parameters:
            standard = wb.get_standard_values(s.GetFID())
            for f in UnitFlows:
                dm_set_double_list(s, str(f).split(".")[1], standard[f])
        self.wb_soil_parameters.finalise()

        self.wb_sub_catchments.reset_reading()
        for s in self.wb_sub_catchments:
            daily_flow = wb.get_sub_daily_flow(s.GetFID())
            if not daily_flow:
                continue

            logging.info(
                f"{s.GetFID()} {str(Streams(s.GetFieldAsInteger('stream')))} {format(sum(daily_flow), '.2f')}")

            s.SetField("annual_flow", sum(daily_flow))
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
            s.SetField("provided_volume", wb.get_storage_volumes(s.GetFID()))
        self.wb_sub_storages.finalise()
        self.lot.reset_reading()

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
            for stream in LotStream:
                l.SetField(str(stream).split(".")[1], float(wb.get_parcel_internal_stream_volumes(l.GetFID(), stream)))
        self.lot.finalise()
        self.wb_lot_storages.finalise()






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


