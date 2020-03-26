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
        self.setIsGDALModule(True)

    def init(self):
        self.lot = ViewContainer('parcel', DM.COMPONENT, DM.READ)
        self.lot.addAttribute("persons", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("roof_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("impervious_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("irrigated_garden_area", DM.Attribute.DOUBLE, DM.READ)
        self.lot.addAttribute("demand", DM.Attribute.DOUBLE, DM.WRITE)
        self.lot.addAttribute("wb_template_id", DM.Attribute.INT, DM.READ)

        view_register = [self.lot]

        self.registerViewContainers(view_register)

    # should lotscale be considered as part of the templates?

    # Lot template


    def run(self):
        lots = {}
        # collect all lot data
        for l in self.lot:
            l : ogr.Feature
            lot = {
                "persons": l.GetFieldAsDouble("persons"),
                "roof_area": l.GetFieldAsDouble("roof_area"),
                "impervious_area": l.GetFieldAsDouble("impervious_area"),
                "irrigated_garden_area": l.GetFieldAsDouble("irrigated_garden_area"),
                "potable_demand": [ "_potable_demand"],
                "non_potable_demand": ["_non_potable_demand"],
                "outdoor_demand": ["_outdoor_demand"],
                "sewerage": ["_black_water"],
                "grey_water": ["_grey_water"]
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
                Streams.non_potable_demand: [LotStream.non_potable_demand],
                Streams.outdoor_demand: [LotStream.outdoor_demand],
                Streams.sewerage: [LotStream.black_water],
                Streams.grey_water: [LotStream.grey_water],
                Streams.stormwater_runoff: [LotStream.roof_runoff, LotStream.impervious_runoff],
                Streams.infiltration: [LotStream.infiltration],
                Streams.evapotranspiration: [LotStream.evapotranspiration]
        },
            "storages": [{"inflow": LotStream.roof_runoff, "demand": LotStream.outdoor_demand, "volume": 5},
                         {"inflow": LotStream.grey_water, "demand": LotStream.non_potable_demand, "volume": 0.5}]
        }


if __name__ == '__main__':
    for s in [10, 100]:
        start = time.time()
        lots = {}
        for i in range(s):
            lots[i] = (_create_lot(i))
        WaterCycleModel(lots=lots, library_path="/Users/christianurich/Documents/dynamind/build/output/")

        print(s, time.time() - start)


