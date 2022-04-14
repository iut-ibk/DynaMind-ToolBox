import pycd3 as cd3
import logging
from pydynamind import *
from . import Lot, TransferNode, UnitParameters, Streams, LotStream, DemandProfile


def annual_sum(vec: list) -> float:
    return sum(vec) / (float(len(vec)) / 365.)

class WaterCycleModel():
    def __init__(self, lots: {},
                 sub_catchments: {},
                 wb_lot_to_sub_catchments: {},
                 wb_sub_storages: {},
                 wb_demand_profile: {},
                 soils: {},
                 stations: {},
                 dates: (),
                 library_path=None):

        self._standard_values = {}
        self._flow_probes = {}
        self.start_date = dates[0]
        self.end_date = dates[1]
        self._library_path = library_path
        self._nodes = {}
        self._sub_catchments = sub_catchments
        self._wb_sub_storages = wb_sub_storages
        self._wb_lot_to_sub_catchments = wb_lot_to_sub_catchments
        self._wb_demand_profile = wb_demand_profile
        self._stations = stations

        print(self.start_date, self.end_date)

        for station_id in self._stations.keys():
            for soil_id, parameters in soils.items():
                for wb_demand_profile_id, wb_p in wb_demand_profile.items():
                    logging.info(
                        f"station: {station_id} soil_id: {soil_id} wb_demand_profile_id: {wb_demand_profile_id}")
                    self._standard_values[(soil_id, station_id, wb_demand_profile_id)] = UnitParameters(self.start_date,
                                                           self.end_date,
                                                           parameters,
                                                           self._stations[station_id],
                                                           wb_p[DemandProfile.crop_factor],
                                                           self._library_path).unit_values


        self._lots = lots
        self._lot_storage_reporting = {}
        self._storage_reporting = {}
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

        sub_networks = {}

        # lot -> s1 -> potable
        # lot ->
        # lot -> s2 -> potable
        # lot ->
        for sub_id, stream in self._sub_catchments.items():
            sub_networks["sub_" + str(sub_id)] = self._create_sub_catchment_network(sub_id, stream,  "sub_" + str(sub_id) + "_" + str(stream) + "_total")

        self._networks = sub_networks
        
        # Needs unique ID
        self._build_network()
        self._cd3.init_nodes()
        self._cd3.start(self.start_date)
        #self._reporting()
        #print('FLOW PROBES', self._flow_probes)
        # for key, storage in self._lot_storage_reporting.items():
        #     for id, s in storage.items():
        #         print(key, id, s,  sum(s.get_state_value_as_double_vector('provided_volume')))

    def get_internal_storage_volumes_lot(self, parcel_id):
        total_provided=0
        if parcel_id not in self._lot_storage_reporting:
            return 0
        for key, s in self._lot_storage_reporting[parcel_id].items():
            total_provided += annual_sum(s.get_state_value_as_double_vector('provided_volume'))
        return total_provided

    def get_internal_storages(self, parcel_id):
        if parcel_id not in self._lot_storage_reporting:
            return {}
        return self._lot_storage_reporting[parcel_id]

    def get_parcel_internal_stream_volumes(self, parcel_id: int, lot_stream_id: LotStream):
        lot: Lot
        if parcel_id not in self._nodes:
            return None
        lot = self._nodes[parcel_id]
        return annual_sum(lot.get_internal_stream_report(lot_stream_id).get_state_value_as_double_vector('Flow'))

    def get_internal_storage_volumes(self, storage_id):
        total_provided = 0
        for key, storage in self._lot_storage_reporting.items():
            for id, s in storage.items():
                if id == storage_id:
                    total_provided += annual_sum(s.get_state_value_as_double_vector('provided_volume'))
        return total_provided

    def get_storage_volumes(self, storage_id):
        return annual_sum(self._storage_reporting[storage_id].get_state_value_as_double_vector('provided_volume'))

    def get_storage(self, storage_id):
        return self._storage_reporting[storage_id]

    def get_standard_value(self, soil_id, station_id, wb_demand_profile_id):
        return self._standard_values[(soil_id, station_id, wb_demand_profile_id)]

    def get_standard_values(self) -> dict:
        return self._standard_values

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

    def get_sub_daily_flow(self, sub_id):
        if "sub_" + str(sub_id) not in self._flow_probes:
            log(f"Node not found sub_{str(sub_id)}", Warning)
            return None
        return self._flow_probes["sub_" + str(sub_id)].get_state_value_as_double_vector('Flow')

    def _create_sub_catchment_network(self, sub_id, stream, reporting_node):
        edges = []
        for lot_id in self._wb_lot_to_sub_catchments[sub_id]:
            edges.append((lot_id, "sub_" + str(sub_id)))
        edges.append(("sub_" + str(sub_id), reporting_node))

        return {
            "stream": stream,
            "edges": edges,
            "reporting_node": reporting_node,
        }

    def _build_network(self):
        # Create lots
        for lot_id, lot in self._lots.items():
            self._nodes[lot_id] = Lot(lot_id,
                                      self._cd3,
                                      lot,
                                      self._standard_values,
                                      self._wb_demand_profile[lot["wb_demand_profile_id"]],
                                      self._lot_storage_reporting)
        print('I get past lot creation')
        # Create all nodes in network
        for name, network in self._networks.items():
            self._create_nodes(network)

        print('Network: ', self._networks)
        #print('NODES',self._nodes)

        # Add all storages
        for name, s in self._wb_sub_storages.items():
            self._create_storage(s)
            
        self._cd3.init_nodes()
        for name, network in self._networks.items():
            self._create_network(name, network)

    # option to connect storage back to multiple uses
    def _create_storage(self, storage):
        demand_port = self._nodes[storage["inflow"]].add_storage(storage)
        self._storage_reporting[storage["id"]] = demand_port[0] # link to actual storage
        if "demand" in storage:
            self._nodes[storage["demand"]].link_storage([demand_port[0], demand_port[1]["in_0"], demand_port[1]["out_0"]])
        if "demand_1" in storage:
            self._nodes[storage["demand_1"]].link_storage([demand_port[0], demand_port[1]["in_1"], demand_port[1]["out_1"]])
        if "demand_2" in storage:
            self._nodes[storage["demand_2"]].link_storage([demand_port[0], demand_port[1]["in_2"], demand_port[1]["out_2"]])

    def _create_nodes(self, network):
        nodes = {}
        # determine the number of inputs for the mixer
        for e in network["edges"]:
            end_n = e[1]
            if end_n not in nodes:
                nodes[end_n] = 0
            nodes[end_n] += 1

        
        for n in nodes.keys():
            if n not in self._nodes:
                self._nodes[n] = TransferNode(self._cd3, nodes[n])

    def _create_network(self, name, network):

        stream = network["stream"]
        for e in network["edges"]:
            if e[0] not in self._nodes:
                log(f"Node not found {str(e[0])}", Warning)
                return
            n_start = self._nodes[e[0]]
            n_end = self._nodes[e[1]]
            outflow = ()
            if type(n_start) is TransferNode:
                outflow = getattr(n_start, "out_port")
            else:
                outflow = n_start.get_stream(stream)
            if not outflow:
                continue
            #Careful only call once because it increments the ports
            inflow = n_end.in_port
            print('im actually here')
            self._cd3.add_connection(outflow[0], outflow[1], inflow[0], inflow[1])

        self._flow_probes[name] = self._nodes[network["reporting_node"]].add_flow_probe()

    def get_default_folder(self):
        return self._library_path


