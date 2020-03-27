import sys
import logging
import time

# sys.path.insert(0, "/Users/christianurich/Documents/dynamind/build/output/")

from pydynamind import *
# from osgeo import ogr

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

        self.wb_lot_template = ViewContainer('wb_lot_template', DM.COMPONENT, DM.READ)
        self.wb_lot_template.addAttribute("potable_demand_stream", DM.Attribute.DOUBLEVECTOR, DM.READ)
        self.wb_lot_template.addAttribute("stormwater_runoff_stream", DM.Attribute.DOUBLEVECTOR, DM.READ)

        self.wb_storages = ViewContainer('wb_storages', DM.COMPONENT, DM.READ)
        self.wb_storages.addAttribute("wb_lot_template_id", DM.Attribute.INT, DM.READ)
        self.wb_storages.addAttribute("inflow_stream_id", DM.Attribute.INT, DM.READ)
        self.wb_storages.addAttribute("demand_stream_id", DM.Attribute.INT, DM.READ)
        # self.wb_storages.addAttribute("type", DM.Attribute.STRING, DM.READ)
        self.wb_storages.addAttribute("volume", DM.Attribute.DOUBLE, DM.READ)



        view_register = [self.lot, self.wb_lot_template, self.wb_storages]

        self.registerViewContainers(view_register)

    def run(self):
        lots = {}
        # collect all lot data
        self._templates = {}
        for template in self.wb_lot_template:
            template : ogr.Feature
            potable_demand = dm_get_double_list(template, "potable_demand_stream")
            stormwater_runoff = dm_get_double_list(template, "stormwater_runoff_stream")
            self._templates[template.GetFID()] = { "streams": {Streams.potable_demand: [ LotStream(s) for s in potable_demand ],
                                                               Streams.stormwater_runoff:  [ LotStream(s) for s in stormwater_runoff ]}}

        self.wb_lot_template.finalise()

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
        print(self._storages)
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

        wb = WaterCycleModel(lots=lots, library_path=self.getSimulationConfig().getDefaultLibraryPath())

        self.lot.finalise()


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
    for s in [10, 100]:
        start = time.time()
        lots = {}
        for i in range(s):
            lots[i] = (_create_lot(i))
        WaterCycleModel(lots=lots, library_path="/Users/christianurich/Documents/dynamind/build/output/")

        print(s, time.time() - start)


