import logging
from pyexpat import model
import pycd3 as cd3

from enum import Enum

# for intergration only
import pandas as pd


class SoilParameters(Enum):
    impervious_threshold = 1
    initial_soil_storage = 2
    infiltration_capacity = 3
    infiltration_exponent = 4
    initial_groundwater_store = 5
    daily_recharge_rate = 6
    daily_drainage_rate = 7
    daily_deep_seepage_rate = 8
    soil_store_capacity = 9
    field_capacity = 10
    transpiration_capacity = 11

class SoilParameters_Irrigation(Enum):
    horton_inital_infiltration = 1
    horton_final_infiltration = 2
    horton_decay_constant = 3
    wilting_point = 4
    field_capactiy = 5
    saturation = 6
    soil_depth = 7
    intial_soil_depth = 8
    ground_water_recharge_rate = 9
    transpiration_capacity = 10
    initial_loss = 11


class UnitFlows(Enum):
    roof_runoff = 1
    impervious_runoff = 2
    outdoor_demand = 3
    possible_infiltration = 4
    actual_infiltration = 5
    groundwater_infiltration = 6
    pervious_storage = 7
    effective_evapotranspiration = 8
    pervious_runoff = 9
    rainfall = 10
    evapotranspiration = 11
    pervious_evapotranspiration = 12
    pervious_evapotranspiration_irrigated = 13
    impervious_evapotranspiration = 14
    roof_evapotranspiration = 15


class UnitParameters:
    def __init__(self,
                 start_date,
                 end_date,
                 soil_parameters: {},
                 climate_data: {},
                 crop_factor: float,
                 library_path):
        """
        Calculate standard values for per m2
        This includes
         - roof_runoff per m2 roof
         - surface_runoff per m2 surface runoff
         - outdoor_demand per m2 garden space
         - possible_infiltration per m2 non perv area
         - actual_infiltration per m2 non perv area
         - groundwater_infiltration per m2 non perv area
         - effective_evapotranspiration
         - rainfall
         - evapotranspiration
        """

        """
        Impervious threshold      1 mm
        Initial soil storage      30%
        Infiltration capacity     200 mm
        Infiltration exponent     1
        Initial groundwater store 10 mm
        Daily recharge rate       25%
        Daily drainage rate       5%
        Daily deep seepage rate   0%
        """

        print("Unit parameters")
        self._library_path = library_path
        self._standard_values = {}
        self.start_date = start_date
        self._climate_data = climate_data
        self.end_date = end_date
        self.soil = soil_parameters
        
        lot_area = 500
        perv_area_fra = 0.2
        roof_imp_fra = 0.5

        # get the rainfall out of the climate data for testing
        df_rain = pd.DataFrame({'rainfall': self._climate_data['rainfall']})
        df_rain.to_csv('/workspaces/DynaMind-ToolBox/tests/resources/rainfall.csv')

        # get the keys of the soil parameters of the model. These will be different depending on what model is being used and thus can be
        # used to identify each model
        key = list(self.soil.keys())

        # setup the inputs and reporting dicts for the model

        # old model
        if key[0] == SoilParameters.impervious_threshold:

            horton_initial_cap = 0.09
            horton_final_cap = 0.001
            horton_decay_constant = 0.06
            # perv_soil_storage_capacity = 0.03
            # daily_recharge_rate = 0.25
            # transpiration_capacity = 7

            parameters = {}
            parameters["Catchment_Area_[m^2]"] = lot_area
            parameters["Fraktion_of_Pervious_Area_pA_[-]"] = perv_area_fra
            parameters["Fraktion_of_Impervious_Area_to_Stormwater_Drain_iASD_[-]"] = 1.0 - perv_area_fra
            parameters["Fraktion_of_Impervious_Area_to_Reservoir_iAR_[-]"] = roof_imp_fra
            parameters["Outdoor_Demand_Weighing_Factor_[-]"] = 1.0

            parameters["Initial_Infiltration_Capacity_[m/h]"] = horton_initial_cap
            parameters["Final_Infiltration_Capacity_[m/h]"] = horton_final_cap
            parameters["Decay_Constant_[1/min]"] = horton_decay_constant
            parameters["Soil Storage Capacity in m"] = self.soil[SoilParameters.soil_store_capacity]
            parameters["Daily Recharge Rate"] = self.soil[SoilParameters.daily_recharge_rate]
            parameters["Transpire Capacity"] = self.soil[SoilParameters.transpiration_capacity]

            reporting = {}
            reporting[UnitFlows.roof_runoff] = {"port": "Collected_Water", "factor": (lot_area * roof_imp_fra)}
            reporting[UnitFlows.impervious_runoff] = {"port": "impervious_runoff",
                                                    "factor": (lot_area * (1 - perv_area_fra))}
            reporting[UnitFlows.outdoor_demand] = {"port": "Outdoor_Demand", "factor": (lot_area / crop_factor * (
                perv_area_fra))}  # {"port": "Outdoor_Demand", "factor" : (lot_area * ( perv_area_fra ))}

            reporting[UnitFlows.possible_infiltration] = {"port": "Possible_Infiltration",
                                                        "factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.actual_infiltration] = {"port": "Actual_Infiltration",
                                                        "factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.groundwater_infiltration] = {"port": "groundwater_infiltration",
                                                            "factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.pervious_storage] = {"port": "previous_storage", "factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.effective_evapotranspiration] = {"port": "effective_evapotranspiration",
                                                                "factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.pervious_runoff] = {"port": "pervious_runoff", "factor": (lot_area * (perv_area_fra))}

        # new model
        elif key[0] == SoilParameters_Irrigation.horton_inital_infiltration:

            parameters = {}

            parameters["Catchment_Area_[m^2]"] = lot_area
            parameters["Fraktion_of_Pervious_Area_pA_[-]"] = perv_area_fra
            parameters["Fraktion_of_Impervious_Area_to_Stormwater_Drain_iASD_[-]"] = 1.0 - perv_area_fra
            parameters["Fraktion_of_Impervious_Area_to_Reservoir_iAR_[-]"] = roof_imp_fra

            parameters["Initial_Infiltration_Capacity_[m/h]"] = self.soil[SoilParameters_Irrigation.horton_inital_infiltration]
            parameters["Final_Infiltration_Capacity_[m/h]"] = self.soil[SoilParameters_Irrigation.horton_final_infiltration]
            parameters["Decay_Constant_[1/min]"] = self.soil[SoilParameters_Irrigation.horton_decay_constant]

            parameters["Wilting_Point_[%]"] = self.soil[SoilParameters_Irrigation.wilting_point]
            parameters["Field_Capacity_[%]"] = self.soil[SoilParameters_Irrigation.field_capactiy]
            parameters["Saturation_[%]"] = self.soil[SoilParameters_Irrigation.saturation]
            
            parameters["Soil Storage Capacity [m]"] = self.soil[SoilParameters_Irrigation.soil_depth]
            parameters["Initial_Pervious_Storage_Level_[m]"] = self.soil[SoilParameters_Irrigation.intial_soil_depth]
            parameters["Wetting_Loss_[m]"] = self.soil[SoilParameters_Irrigation.initial_loss]

            parameters["Daily Recharge Rate"] = self.soil[SoilParameters_Irrigation.ground_water_recharge_rate]
            parameters["Transpire Capacity"] = self.soil[SoilParameters_Irrigation.transpiration_capacity]


            reporting = {}

            reporting[UnitFlows.roof_runoff] = {"port": "roof_runoff", "factor": (lot_area * roof_imp_fra)}
            reporting[UnitFlows.impervious_runoff] = {"port": "impervious_runoff","factor": (lot_area * (1 - perv_area_fra))}
            reporting[UnitFlows.pervious_runoff] = {"port": "pervious_runoff", "factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.outdoor_demand] = {"port": "outdoor_demand", "factor": (lot_area * perv_area_fra)} 
            reporting[UnitFlows.possible_infiltration] = {"port": "possible_infiltration", "factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.actual_infiltration] = {"port": "actual_infiltration","factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.groundwater_infiltration] = {"port": "groundwater_infiltration","factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.pervious_storage] = {"port": "pervious_storage", "factor": (lot_area * (perv_area_fra))}
            reporting[UnitFlows.effective_evapotranspiration] = {"port": "evapotranspiration", "factor": (lot_area * (perv_area_fra))}
            

        #set up the city model and register the python plugins

        flow = {'Q': cd3.Flow.flow, 'N': cd3.Flow.concentration}

        print("Init CD3")
        catchment_model = cd3.CityDrain3(
            self.start_date,
            self.end_date,
            "86400",
            flow
        )

        # Init CD3
        # Register Modules
        catchment_model.register_native_plugin(
            self.get_default_folder() + "/libcd3core")
        catchment_model.register_native_plugin(
            self.get_default_folder() + "/CD3Modules/libdance4water-nodes")
        catchment_model.register_python_path(
            self.get_default_folder() + "/CD3Modules/CD3Waterbalance/Module")
        catchment_model.register_python_path(
            self.get_default_folder() + "/CD3Modules/CD3Waterbalance/WaterDemandModel")
        catchment_model.register_python_plugin(
            self.get_default_folder() + "/CD3Modules/CD3Waterbalance/Module/cd3waterbalancemodules.py")


        # select the model based on the soil parameter types and carry this foward

        if key[0] == SoilParameters.impervious_threshold:

            catchment_w_routing = catchment_model.add_node("Catchment_w_Routing")
            model = catchment_w_routing


        elif key[0] == SoilParameters_Irrigation.horton_inital_infiltration: 
        
            catchment_w_irrigation = catchment_model.add_node('Catchment_w_Irrigation')
            model = catchment_w_irrigation

            # add the correct ET data to the model
            df = pd.read_csv("/workspaces/DynaMind-ToolBox/tests/resources/climate_data.csv")
            df['Date'] = pd.to_datetime(df['Date'],format='%d/%m/%Y')
            df.set_index('Date',inplace=True)
            self._climate_data["evapotranspiration"] = df.loc['2000']['ET'].to_list()

        
        for p in parameters.items():
            model.setDoubleParameter(p[0], p[1])

        rain = catchment_model.add_node("SourceVector")
        rain.setDoubleVectorParameter("source", self._climate_data["rainfall intensity"])
        evapo = catchment_model.add_node("SourceVector")
        evapo.setDoubleVectorParameter("source", self._climate_data["evapotranspiration"])

        
        catchment_model.add_connection(rain, "out", model, "Rain")
        catchment_model.add_connection(evapo, "out", model, "Evapotranspiration")

        # if its the irrigation module, we also need an irrigation stream to the catchment
        if key[0] == SoilParameters_Irrigation.horton_inital_infiltration:
            irrigation = catchment_model.add_node("SourceVector")

            # at the moment we hardcode the irrigation and set it to be zero
            irrigation.setDoubleVectorParameter("source", [0 for i in self._climate_data["rainfall intensity"]])
            catchment_model.add_connection(irrigation, "out", model, "irrigation")

        flow_probe = {}
        for key, r in reporting.items():
            rep = catchment_model.add_node("FlowProbe")
            catchment_model.add_connection(model, r["port"], rep, "in")
            flow_probe[key] = rep
        
        catchment_model.init_nodes()
        catchment_model.start(self.start_date)
        
        for key, probe in flow_probe.items():
            
            scaling = 1. / reporting[key]["factor"]
            self._standard_values[key] = [v * scaling for v in probe.get_state_value_as_double_vector('Flow')]
            

        self._standard_values[UnitFlows.rainfall] = [v for v in self._climate_data["rainfall intensity"]]
        self._standard_values[UnitFlows.evapotranspiration] = [v for v in self._climate_data["evapotranspiration"]]

        pervious_evapotranspiration_irrigated = []
        impervious_evapotranspiration = []
        roof_evapotranspiration = []
        pervious_evapotranspiration = []

        for idx, v in enumerate(self._standard_values[UnitFlows.groundwater_infiltration]):
            pervious_evapotranspiration.append(self._standard_values[UnitFlows.rainfall][idx]
                                               - self._standard_values[UnitFlows.pervious_runoff][idx]
                                               - self._standard_values[UnitFlows.actual_infiltration][idx]
                                               + self._standard_values[UnitFlows.effective_evapotranspiration][idx])
            pervious_evapotranspiration_irrigated.append(
                self._standard_values[UnitFlows.rainfall][idx] -
                self._standard_values[UnitFlows.pervious_runoff][idx] -
                self._standard_values[UnitFlows.actual_infiltration][idx] +
                self._standard_values[UnitFlows.effective_evapotranspiration][idx] +
                self._standard_values[UnitFlows.outdoor_demand][idx]
            )

            impervious_evapotranspiration.append(
                self._standard_values[UnitFlows.rainfall][idx] - self._standard_values[UnitFlows.impervious_runoff][
                    idx])
            roof_evapotranspiration.append(
                self._standard_values[UnitFlows.rainfall][idx] - self._standard_values[UnitFlows.roof_runoff][
                    idx])
        self._standard_values[UnitFlows.pervious_evapotranspiration] = pervious_evapotranspiration
        self._standard_values[UnitFlows.pervious_evapotranspiration_irrigated] = pervious_evapotranspiration_irrigated
        self._standard_values[UnitFlows.impervious_evapotranspiration] = impervious_evapotranspiration
        self._standard_values[UnitFlows.roof_evapotranspiration] = roof_evapotranspiration

        for key, values in self._standard_values.items():
            logging.info(
                f"{key} {format(sum(values), '.2f')}")

            # logging.warning(
            #     f"{key} {[format(v, '.2f') for v in values]}")

        #del catchment_model

    @property
    def unit_values(self) -> {}:
        return self._standard_values

    def get_default_folder(self):
        return self._library_path
