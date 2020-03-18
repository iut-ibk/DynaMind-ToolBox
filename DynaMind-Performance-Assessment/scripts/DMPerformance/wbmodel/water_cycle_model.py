import pycd3 as cd3
import logging

from . import Lot, TransferNode, UnitParameters

class WaterCycleModel():
    def __init__(self, nodes):
        self._standard_values = {}
        self._flow_probes = {}
        self.number_of_nodes = nodes
        self.start_date = "2001-Jan-01 00:00:00"
        self.end_date = "2002-Jan-01 00:00:00"

        self._standard_values = UnitParameters(self.start_date,
                                               self.end_date).unit_values
        self.water_balance_model(nodes)

    def water_balance_model(self, nodes):
        flow = {'Q': cd3.Flow.flow, 'N': cd3.Flow.concentration}

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

        self._networks["outdoor_demand"] = self._create_catchment_network("outdoor_demand", "od1", "od1_total",
                                                                              "od1_total")

        self._networks["storm_water_runoff"] = self._create_catchment_network("storm_water_runoff", "sw1", "sw1_total",
                                                                              "sw1_total")

        self._networks["evapotranspiration"] = self._create_catchment_network("evapotranspiration", "e1", "e1_total",
                                                                              "e1_total")

        self._networks["infiltration"] = self._create_catchment_network("infiltration", "if1", "if1_total",
                                                                              "if1_total")
        self._storages = [
            {
                "id": "stormwater_recycling",
                "demand": "od1",
                "inflow": "sw1",
                "volume": 10.
            },
            {
                "id": "stormwater_recycling",
                "demand": "od1",
                "inflow": "gw1",
                "volume": 10.
            }
        ]
        # Needs unique ID
        self._nodes = {}
        self._build_network()
        self._cd3.init_nodes()
        self._cd3.start(self.start_date)
        self._reporting()

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
            "roof_area": 200,
            "impervious_area": 200,
            "irrigated_garden_area": 600,
            "potable_demand":[ "_potable_demand"],
            "non_potable_demand": ["_non_potable_demand"],
            "outdoor_demand": ["_outdoor_demand"],
            "sewerage": ["_black_water"],
            "grey_water": ["_grey_water"],
            "storages": [{"inflow": "_roof_runoff", "demand": "_outdoor_demand", "volume": 5},
                         {"inflow": "_grey_water", "demand": "_outdoor_demand", "volume": 0.5}]
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
            self._nodes[l["id"]] = Lot(str(l["id"]), self._cd3, l, self._standard_values)

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

