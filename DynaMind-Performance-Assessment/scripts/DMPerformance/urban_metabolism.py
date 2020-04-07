import sys
import logging
import time


from pydynamind import *
from osgeo import ogr

logging.basicConfig(level=logging.INFO)
from wbmodel import WaterCycleModel, Streams, LotStream



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
        self.lot.addAttribute("roof_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("impervious_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("irrigated_garden_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("demand", DM.Attribute.DOUBLE, DM.WRITE)
        self.lot.addAttribute("wb_lot_template_id", DM.Attribute.INT, DM.READ)
        self.lot.addAttribute("wb_sub_catchment_id", DM.Attribute.INT, DM.READ)

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

        self.wb_sub_catchments = ViewContainer('wb_sub_catchment', DM.COMPONENT, DM.READ)
        self.wb_sub_catchments.addAttribute('stream', DM.Attribute.INT, DM.READ)

        self.wb_sub_catchments = ViewContainer('wb_sub_catchment', DM.COMPONENT, DM.READ)
        self.wb_sub_catchments.addAttribute('stream', DM.Attribute.INT, DM.READ)
        self.wb_sub_catchments.addAttribute('annual_flow', DM.Attribute.DOUBLE, DM.WRITE)
        self.wb_sub_catchments.addAttribute('daily_flow', DM.Attribute.DOUBLEVECTOR, DM.WRITE)

        self.wb_sub_storages = ViewContainer('wb_sub_storages', DM.COMPONENT, DM.READ)
        self.wb_sub_storages.addAttribute("inflow_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_sub_storages.addAttribute("demand_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_sub_storages.addAttribute("volume", DM.Attribute.DOUBLE, DM.READ)

        self.wb_lot_to_sub_catchments = ViewContainer('wb_lot_to_sub_catchments', DM.COMPONENT, DM.READ)
        self.wb_lot_to_sub_catchments.addAttribute('wb_sub_catchment_id', DM.Attribute.LINK, DM.READ)
        self.wb_lot_to_sub_catchments.addAttribute('parcel_id', DM.Attribute.LINK, DM.READ)



        # need multiple streams per lot
        # lot can be part of multiple sub catchment (1 for each stream)
        # key = (sub_catchment, stream)


        view_register = [self.lot,
                         self.wb_lot_template,
                         self.wb_storages,
                         self.wb_sub_storages,
                         self.wb_sub_catchments,
                         self.wb_lot_to_sub_catchments,
                         self.wb_lot_streams]
        #
        self.registerViewContainers(view_register)

    def run(self):
        lots = {}
        # collect all lot data
        lot_streams = {}

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

        # print(lot_streams)

        self._templates = {}
        for template in self.wb_lot_template:
            template : ogr.Feature
            self._templates[template.GetFID()] = { "streams":lot_streams[template.GetFID()]}

        self.wb_lot_template.finalise()

        sub_catchments = {}
        sub_catchments_lots = {}

        for s in self.wb_sub_catchments:
            s: ogr.Feature
            sub_catchments[s.GetFID()] = Streams(s.GetFieldAsInteger("stream"))
            sub_catchments_lots[s.GetFID()] = []
        self.wb_sub_catchments.finalise()

        for lot_sub_catchments in self.wb_lot_to_sub_catchments:
            lot_sub_catchments: ogr.Feature
            parcel_id = lot_sub_catchments.GetFieldAsInteger("parcel_id")
            wb_sub_catchment_id = lot_sub_catchments.GetFieldAsInteger("wb_sub_catchment_id")

            sub_catchments_lots[wb_sub_catchment_id].append(parcel_id)
        self.wb_lot_to_sub_catchments.finalise()

        for s in self.wb_storages:
            s : ogr.Feature
            template_id = s.GetFieldAsInteger("wb_lot_template_id")

            if s not in self._storages:
                self._storages[template_id] = []

            storage = {"inflow" : LotStream(s.GetFieldAsInteger("inflow_stream_id")),
                       "demand" : LotStream(s.GetFieldAsInteger("demand_stream_id")),
                       "volume" : s.GetFieldAsInteger("volume")}

            storages = self._storages[template_id]
            storages.append(storage)
        # self.wb_storages.finalise()

        wb_sub_storages = {}
        for s in self.wb_sub_storages:
            s: ogr.Feature
            storage = {"inflow" : "sub_" + str(s.GetFieldAsInteger("inflow_stream_id")),
                       "demand" : "sub_" + str(s.GetFieldAsInteger("demand_stream_id")),
                       "volume" : s.GetFieldAsInteger("volume")}
            wb_sub_storages[s.GetFID()] = storage
        self.wb_sub_storages.finalise()

        for l in self.lot:
            l : ogr.Feature

            if l.GetFieldAsInteger("wb_lot_template_id") not in self._storages:
                self._storages[l.GetFieldAsInteger("wb_lot_template_id")] = []

            lot = {
                "persons": l.GetFieldAsDouble("persons"),
                "roof_area": l.GetFieldAsDouble("roof_area"),
                "impervious_area": l.GetFieldAsDouble("impervious_area"),
                "irrigated_garden_area": l.GetFieldAsDouble("irrigated_garden_area"),
                "streams":
                    self._templates[l.GetFieldAsInteger("wb_lot_template_id")]["streams"],
                "storages": self._storages[l.GetFieldAsInteger("wb_lot_template_id")]
            }

            lots[l.GetFID()] = lot
        self.lot.finalise()

        wb = WaterCycleModel(lots=lots,
                             sub_catchments=sub_catchments,
                             wb_lot_to_sub_catchments=sub_catchments_lots,
                             wb_sub_storages=wb_sub_storages,
                             library_path=self.getSimulationConfig().getDefaultLibraryPath())

        self.wb_sub_catchments.reset_reading()
        for s in self.wb_sub_catchments:
            daily_flow = wb.get_sub_daily_flow(s.GetFID())

            # self.wb_sub_catchments.addAttribute('annual_flow', DM.Attribute.DOUBLE, DM.WRITE)
            # self.wb_sub_catchments.addAttribute('daily_flow', DM.Attribute.DOUBLE, DM.WRITE)

            s.SetField("annual_flow", sum(daily_flow))
            dm_set_double_list(s, 'daily_flow', daily_flow)

        self.wb_sub_catchments.finalise()

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


