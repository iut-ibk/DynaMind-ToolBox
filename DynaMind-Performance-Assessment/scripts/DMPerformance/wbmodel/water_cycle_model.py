import pycd3 as cd3
import logging

from . import Lot, TransferNode, UnitParameters, Streams

class WaterCycleModel():
    def __init__(self, lots: {}, library_path = None):
        self._standard_values = {}
        self._flow_probes = {}
        self.start_date = "2001-Jan-01 00:00:00"
        self.end_date = "2002-Jan-01 00:00:00"
        self._library_path = library_path
        self._nodes = {}

        self._standard_values = UnitParameters(self.start_date,
                                               self.end_date, self._library_path).unit_values
        self._lots = lots
        self.water_balance_model()

    def water_balance_model(self):
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


        self._networks = {}

        for s in Streams:
            self._networks[s] = self._create_catchment_network(s, str(s), str(s) + "_total",
                                                                          str(s) + "_total")

        self._storages = [
            # {
            #     "id": "stormwater_recycling",
            #     "demand": "od1",
            #     "inflow": "sw1",
            #     "volume": 10.
            # },
            # {
            #     "id": "stormwater_recycling",
            #     "demand": "od1",
            #     "inflow": "gw1",
            #     "volume": 10.
            # }
        ]
        # Needs unique ID
        self._build_network()
        self._cd3.init_nodes()
        self._cd3.start(self.start_date)
        self._reporting()

    def _reporting(self, timeseries = False):
        for key, network in self._networks.items():
            try:
                logging.info(
                    f"{key} {format(sum(self._flow_probes[key].get_state_value_as_double_vector('Flow')), '.2f')}")

                if timeseries:
                    logging.info(
                        f"{key} {[format(v, '.2f') for v in self._flow_probes[key].get_state_value_as_double_vector('Flow')]}")
            except:
                pass


    def _create_catchment_network(self, stream, catchment_id, outfall_id, reporting_node):
        edges = []
        for id, lot in self._lots.items():
            edges.append((id, catchment_id))
        edges.append((catchment_id, outfall_id))
        return {
            "stream": stream,
            "edges": edges,
            "reporting_node": reporting_node,
        }

    def _build_network(self):

        for lot_id, lot in self._lots.items():
            self._nodes[lot_id] = Lot(str(lot_id), self._cd3, lot, self._standard_values)

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
                outflow = n_start.get_stream(stream)
            if not outflow:
                return
            #Careful only call once because it increments the ports
            inflow = n_end.in_port
            self._cd3.add_connection(outflow[0], outflow[1], inflow[0], inflow[1])

        self._flow_probes[stream] = self._nodes[network["reporting_node"]].add_flow_probe()

    def get_default_folder(self):
        return self._library_path


