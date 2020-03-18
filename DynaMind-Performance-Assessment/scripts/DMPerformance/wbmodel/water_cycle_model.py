import pycd3 as cd3
import logging

from . import Lot, TransferNode

class WaterCycleModel:
    def __init__(self, nodes):
        self._flow_probes = {}
        self.number_of_nodes = nodes
        self.start_date = "2001-Jan-01 00:00:00"
        self.end_date = "2002-Jan-01 00:00:00"

        self._rain_data = self._load_rainfall()


        self._evapotranspiration = self._load_eta()
        self.standard_catchment()

        # self.water_balance_model(nodes)

    def water_balance_model(self, nodes):
        flow = {'Q': cd3.Flow.flow, 'N': cd3.Flow.concentration}
        # print flow
        self._cd3 = cd3.CityDrain3(
            self.start_date,
            self.end_date,
            "86400",
            flow
        )
        # Register Modules
        self._cd3.register_native_plugin(
            self.get_default_folder() + "/libcd3core")
        self._cd3.register_native_plugin(
            self.get_default_folder() + "/CD3Modules/libdance4water-nodes")

        rain = self._cd3.add_node("SourceVector")
        rain.setDoubleVectorParameter("source", self._rain_data)

        self._lots = []
        for i in range(nodes):
            self._lots.append(self._create_lot(i))
        self._networks = {}
        self._networks["potable_demand"] = self._create_catchment_network("potable_demand", "p1", "p1_total",
                                                                          "p1_total")
        self._networks["non_potable_demand"] = self._create_catchment_network("non_potable_demand", "np1", "np1_total",
                                                                              "np1_total")
        self._networks["grey_water"] = self._create_catchment_network("grey_water", "gw1", "gw1_total", "gw1_total")
        self._networks["sewerage"] = self._create_catchment_network("sewerage", "sewerage1", "sewerage1_total",
                                                                    "sewerage1_total")
        self._networks["storm_water_runoff"] = self._create_catchment_network("storm_water_runoff", "sw1", "sw1_total",
                                                                              "sw1_total")
        self._storages = [
            {
                "id": "stormwater_recycling",
                "demand": "np1",
                "inflow": "sw1",
                "volume": 100.
            }
        ]
        # Needs unique ID
        self._nodes = {}
        self._build_network()
        self._cd3.init_nodes()
        self._cd3.start(self.start_date)
        self._reporting()

    def standard_catchment(self):


        lot_area = 500
        perv_area_fra = 0.2
        roof_imp_fra = 0.8
        horton_initial_cap = 0.09
        horton_final_cap = 0.001
        horton_decay_constant = 0.06
        perv_soil_storage_capacity = 30
        daily_recharge_rate = 0.25
        transpiration_capacity = 7

        parameters = {}
        parameters["Catchment_Area_[m^2]"] = lot_area
        parameters["Fraktion_of_Pervious_Area_pA_[-]"] = perv_area_fra
        parameters["Fraktion_of_Impervious_Area_to_Stormwater_Drain_iASD_[-]"] = 1.0 - roof_imp_fra - perv_area_fra
        parameters["Fraktion_of_Impervious_Area_to_Reservoir_iAR_[-]"] = roof_imp_fra
        parameters["Outdoor_Demand_Weighing_Factor_[-]"] = 1.0
        parameters["Initial_Infiltration_Capacity_[m/h]"] = horton_initial_cap
        parameters["Final_Infiltration_Capacity_[m/h]"] = horton_final_cap
        parameters["Decay_Constant_[1/min]"] = horton_decay_constant
        parameters["Soil Storage Capacity in mm"] = perv_soil_storage_capacity
        parameters["Daily Recharge Rate"] = daily_recharge_rate
        parameters["Transpire Capacity"] = transpiration_capacity


        reporting = {}
        reporting["roof_runoff"] = "Collected_Water"
        reporting["surface_runoff"] = "Runoff"
        reporting["outdoor_demand"] = "Outdoor_Demand"
        reporting["possible_infiltration"] = "Possible_Infiltration"
        reporting["actual_infiltraiton"] = "Actual_Infiltration"
        reporting["groundwater_infiltration"] = "groundwater_infiltration"
        reporting["effective_evapotranspiration"] = "effective_evapotranspiration"
        reporting["previous_storage"] = "previous_storage"

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
            print(p[0], p[1])
            catchment_w_routing.setDoubleParameter(p[0], p[1])

        rain = catchment_model.add_node("SourceVector")
        rain.setDoubleVectorParameter("source", self._rain_data)
        print(self._rain_data)
        evapo = catchment_model.add_node("SourceVector")
        evapo.setDoubleVectorParameter("source", self._evapotranspiration)

        catchment_model.add_connection(rain, "out", catchment_w_routing, "Rain")
        catchment_model.add_connection(evapo, "out", catchment_w_routing, "Evapotranspiration")

        flow_probe = {}
        for r in reporting.items():
            rep = catchment_model.add_node("FlowProbe")
            catchment_model.add_connection(catchment_w_routing, r[1], rep, "in")
            flow_probe[r[0]] = rep

        catchment_model.init_nodes()
        catchment_model.start(self.start_date)

        for key, probe in flow_probe.items():
            logging.info(
                f"{key} {format(sum(probe.get_state_value_as_double_vector('Flow')), '.2f')}")

            logging.info(
                f"{key} {[format(v, '.2f') for v in probe.get_state_value_as_double_vector('Flow')]}")

    def _reporting(self, timeseries = False):
        for key, network in self._networks.items():
            logging.info(
                f"{key} {format(sum(self._flow_probes[key].get_state_value_as_double_vector('Flow')), '.2f')}")

            if timeseries:
                logging.info(
                    f"{key} {[format(v, '.2f') for v in self._flow_probes[key].get_state_value_as_double_vector('Flow')]}")

    def _create_lot(self, id):
        return {
            "id": id,
            "persons": 5,
            "roof_area": 100,
            "impervious_area": 200,
            "irrigated_garden_area": 100,
            "potable_demand":[ "_potable_demand"],
            "non_potable_demand": ["_non_potable_demand"],
            "sewerage": ["_black_water"],
            "grey_water": ["_grey_water"],
            "storages": []

            # "storages": [{"inflow": "_roof_runoff", "demand": "_non_potable_demand", "volume": 1},
            #              {"inflow": "_grey_water", "demand": "_non_potable_demand", "volume": 0}]
        }

    def _create_catchment_network(self, stream, catchment_id, outfall_id, reporting_node):
        edges = []
        for l in self._lots:
            edges.append((l["id"], catchment_id))
        edges.append((catchment_id, outfall_id))
        return {
        "stream": stream,
        "edges": edges,
        "reporting_node": reporting_node,
        }

    def _build_network(self):

        for l in self._lots:
            self._nodes[l["id"]] = Lot(str(l["id"]), self._cd3, l, self._rain_data)

        for name, network in self._networks.items():
            self._create_nodes(network)

        for s in self._storages:
            self._create_storage(s)

        for name, network in self._networks.items():
            self._create_network(network)


    def _create_storage(self, storage):
        demand_port = self._nodes[storage["inflow"]].add_storage(storage)
        self._nodes[storage["demand"]].link_storage(demand_port)

    def _create_nodes(self, network):
        nodes = {}
        for e in network["edges"]:
            end_n = e[1]
            if end_n not in nodes:
                nodes[end_n] = 0
            nodes[end_n] += 1

        for n in nodes.keys():
            if n not in self._nodes:
                self._nodes[n] = TransferNode(self._cd3, nodes[n])

    def _create_network(self, network):
        stream = network["stream"]
        for e in network["edges"]:
            n_start = self._nodes[e[0]]
            n_end = self._nodes[e[1]]
            outflow = ()
            if type(n_start) is TransferNode:
                outflow = getattr(n_start, "out_port")
            else:
                outflow = getattr(n_start, stream)

            #Careful only call once because it increments the ports
            inflow = n_end.in_port
            self._cd3.add_connection(outflow[0], outflow[1], inflow[0], inflow[1])

        self._flow_probes[stream] = self._nodes[network["reporting_node"]].add_flow_probe()

    def get_default_folder(self):
        #self.getSimulationConfig().getDefaultLibraryPath()
        return "/Users/christianurich/Documents/dynamind/build/output/"

    def _load_rainfall(self):
        with open('/Users/christianurich/Documents/dynamind/DynaMind-ToolBox/Data/Raindata/melb_rain_24.ixx') as f:
            rainfall = f.read()
        return[float(r.split("\t")[4]) for r in rainfall.splitlines()]

    def _load_eta(self):
        with open('/Users/christianurich/Documents/dynamind/DynaMind-ToolBox/Data/Raindata/melb_eva_24.ixx') as f:
            rainfall = f.read()
        return[float(r.split("\t")[1]) for r in rainfall.splitlines()]