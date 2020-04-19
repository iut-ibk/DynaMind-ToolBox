import logging
import pycd3 as cd3

from enum import Enum


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

class UnitParameters:
    def __init__(self,
                 start_date,
                 end_date,
                 soil_parameters: {},
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

        self._library_path = library_path
        self._standard_values = {}
        self.start_date = start_date
        self.end_date = end_date

        self._rain_data = self._load_rainfall()
        self._evapotranspiration = self._load_eta()


        lot_area = 500
        perv_area_fra = 0.2
        roof_imp_fra = 0.5


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
        parameters["Soil Storage Capacity in m"] = soil_parameters[SoilParameters.soil_store_capacity]
        parameters["Daily Recharge Rate"] = soil_parameters[SoilParameters.daily_recharge_rate]
        parameters["Transpire Capacity"] = soil_parameters[SoilParameters.transpiration_capacity]


        reporting = {}
        reporting[UnitFlows.roof_runoff] = {"port": "Collected_Water", "factor" : (lot_area * roof_imp_fra)}
        reporting[UnitFlows.impervious_runoff] = {"port": "impervious_runoff", "factor" : (lot_area * (1 - perv_area_fra ))}
        reporting[UnitFlows.outdoor_demand] = {"port": "Outdoor_Demand", "factor" : (lot_area * ( perv_area_fra ))} #{"port": "Outdoor_Demand", "factor" : (lot_area * ( perv_area_fra ))}
        reporting[UnitFlows.possible_infiltration] = {"port": "Possible_Infiltration", "factor" : (lot_area * ( perv_area_fra ))}
        reporting[UnitFlows.actual_infiltration] = {"port": "Actual_Infiltration", "factor" : (lot_area * ( perv_area_fra ))}
        reporting[UnitFlows.groundwater_infiltration] = {"port": "groundwater_infiltration", "factor" : (lot_area * ( perv_area_fra ))}
        reporting[UnitFlows.pervious_storage] = {"port": "previous_storage", "factor" : (lot_area * ( perv_area_fra ))}
        reporting[UnitFlows.effective_evapotranspiration] = {"port": "effective_evapotranspiration", "factor": (lot_area * (perv_area_fra))}
        reporting[UnitFlows.pervious_runoff] = {"port": "pervious_runoff", "factor": (lot_area * (perv_area_fra))}

        # print flow
        flow = {'Q': cd3.Flow.flow, 'N': cd3.Flow.concentration}
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
            self.get_default_folder() +  "/CD3Modules/CD3Waterbalance/Module/cd3waterbalancemodules.py")

        catchment_w_routing = catchment_model.add_node("Catchment_w_Routing")

        for p in parameters.items():
            catchment_w_routing.setDoubleParameter(p[0], p[1])

        rain = catchment_model.add_node("SourceVector")
        rain.setDoubleVectorParameter("source", self._rain_data)

        evapo = catchment_model.add_node("SourceVector")
        evapo.setDoubleVectorParameter("source", self._evapotranspiration)

        catchment_model.add_connection(rain, "out", catchment_w_routing, "Rain")
        catchment_model.add_connection(evapo, "out", catchment_w_routing, "Evapotranspiration")

        flow_probe = {}
        for key, r in reporting.items():
            rep = catchment_model.add_node("FlowProbe")
            catchment_model.add_connection(catchment_w_routing, r["port"], rep, "in")
            flow_probe[key] = rep

        catchment_model.init_nodes()
        catchment_model.start(self.start_date)
        for key, probe in flow_probe.items():
            scaling = 1. / reporting[key]["factor"]
            self._standard_values[key] = [v * scaling for v in probe.get_state_value_as_double_vector('Flow')]

        self._standard_values[UnitFlows.rainfall] = [v for v in self._rain_data]
        self._standard_values[UnitFlows.evapotranspiration] = [v for v in self._evapotranspiration]

        pervious_evapotranspiration_irrigated = []
        impervious_evapotranspiration = []
        pervious_evapotranspiration = []

        for idx, v in enumerate( self._standard_values[UnitFlows.groundwater_infiltration]):
            pervious_evapotranspiration.append(self._standard_values[UnitFlows.rainfall][idx]
                                               - self._standard_values[UnitFlows.pervious_runoff][idx]
                                               - self._standard_values[UnitFlows.groundwater_infiltration][idx] )
            pervious_evapotranspiration_irrigated.append(
                self._standard_values[UnitFlows.rainfall][idx] - self._standard_values[UnitFlows.pervious_runoff][idx] -
                self._standard_values[UnitFlows.groundwater_infiltration][idx] +
                self._standard_values[UnitFlows.outdoor_demand][idx]
            )

            impervious_evapotranspiration.append(self._standard_values[UnitFlows.rainfall][idx] - self._standard_values[UnitFlows.impervious_runoff][idx])

        self._standard_values[UnitFlows.pervious_evapotranspiration] = pervious_evapotranspiration
        self._standard_values[UnitFlows.pervious_evapotranspiration_irrigated] = pervious_evapotranspiration_irrigated
        self._standard_values[UnitFlows.impervious_evapotranspiration] = impervious_evapotranspiration

        for key, values in self._standard_values.items():
            logging.info(
                f"{key} {format(sum(values), '.2f')}")

            # logging.warning(
            #     f"{key} {[format(v, '.2f') for v in values]}")

    @property
    def unit_values(self) -> {}:
        return self._standard_values

    def get_default_folder(self):
        return self._library_path

    def _load_rainfall(self):
        with open(self.get_default_folder() + '/Data/Raindata/melb_rain_24.ixx') as f:
            rainfall = f.read()
        return[float(r.split("\t")[4])/1000. for r in rainfall.splitlines()]

    def _load_eta(self):
        with open(self.get_default_folder() + '/Data/Raindata/melb_eva_24.ixx') as f:
            rainfall = f.read()
        return[float(r.split("\t")[1])/1000. for r in rainfall.splitlines()]