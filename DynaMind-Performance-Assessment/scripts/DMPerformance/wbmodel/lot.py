import pycd3 as cd3
from enum import Enum
import numpy as np

from .unitparameters import SoilParameters, UnitFlows


class LotStream(Enum):
    potable_demand = 1
    non_potable_demand = 2
    outdoor_demand = 3
    black_water = 4
    grey_water = 5
    roof_runoff = 6
    impervious_runoff = 7
    pervious_runoff = 8
    evapotranspiration = 9
    infiltration = 10
    rainfall = 11


class Streams(Enum):
    potable_demand = 1
    non_potable_demand = 2
    outdoor_demand = 3
    sewerage = 4
    grey_water = 5
    stormwater_runoff = 6
    evapotranspiration = 7
    infiltration = 8
    rainfall = 9

    # "potable_demand": ["_potable_demand"],
    # "non_potable_demand": ["_non_potable_demand"],
    # "outdoor_demand": ["_outdoor_demand"],
    # "sewerage": ["_black_water"],
    # "grey_water": ["_grey_water"],
    # "storages": [{"inflow": "_roof_runoff", "demand": "_outdoor_demand", "volume": 5},
    #              {"inflow": "_grey_water", "demand": "_outdoor_demand", "volume": 0.5}]


class Lot:
    def __init__(self,
                 id,
                 cd3_instance: cd3.CityDrain3,
                 lot_detail: {},
                 standard_values: {},
                 lot_storage_reporting: {}):
        self._id = id
        self._cd3 = cd3_instance

        self._standard_values = standard_values

        self._internal_streams = {}
        self._external_streams = {}
        self._internal_report_streams = {}

        self._lot_storage_reporting = lot_storage_reporting

        self._reporting_internal_stream = {}

        for e in LotStream:
            self._internal_streams[e] = None

        for e in Streams:
            self._external_streams[e] = None

        # Reporting stream
        self._lot_storage_reporting[id] = {}

        # Assume I'll be able to get all parameters for database
        self._create_lot(lot_detail)

    def get_stream(self, stream):
        return self._external_streams[stream]

    def get_internal_stream_report(self, stream: LotStream):
        return self._internal_streams[stream][0]

    def _create_lot(self, lot: {}):
        """
        What happens in the lot stays in the lot
        Following nodes are produced

        # Include potential for recycled water

        # Gather data for lot scale interventions
        # Lots can be part of supply zones

        # Exclude routing for now

        :return:
        """
        self._create_demand_node(lot["persons"])
        pervious_area = lot["area"] - lot["roof_area"] - lot["impervious_area"]

        evapo = (np.array(self._standard_values[UnitFlows.impervious_evapotranspiration]) * (lot["impervious_area"]) + \
                 np.array(self._standard_values[UnitFlows.roof_evapotranspiration]) * (lot["roof_area"]) + \
                 np.array(self._standard_values[UnitFlows.pervious_evapotranspiration_irrigated]) * lot[
                     "irrigated_garden_area"] + \
                 np.array(self._standard_values[UnitFlows.pervious_evapotranspiration]) * (
                             pervious_area - lot["irrigated_garden_area"])).tolist()

        self._internal_streams[LotStream.rainfall] = self._create_stream(self._standard_values[UnitFlows.rainfall],
                                                                         lot["area"])
        self._internal_streams[LotStream.roof_runoff] = self._create_stream(
            self._standard_values[UnitFlows.roof_runoff], lot["roof_area"])
        self._internal_streams[LotStream.pervious_runoff] = self._create_stream(
            self._standard_values[UnitFlows.pervious_runoff], pervious_area)
        self._internal_streams[LotStream.impervious_runoff] = self._create_stream(
            self._standard_values[UnitFlows.impervious_runoff], lot["impervious_area"])
        self._internal_streams[LotStream.outdoor_demand] = self._create_stream(
            self._standard_values[UnitFlows.outdoor_demand], lot["irrigated_garden_area"])
        self._internal_streams[LotStream.evapotranspiration] = self._create_stream(evapo, 1)
        self._internal_streams[LotStream.infiltration] = self._create_stream(
            self._standard_values[UnitFlows.groundwater_infiltration], pervious_area)

        # This and reconnected
        if "storages" in lot:
            for idx, s in enumerate(lot["storages"]):
                self._add_storage(s)

        # Setup Streams
        for stream_id in self._external_streams:
            if stream_id in lot["streams"]:
                self._external_streams[stream_id] = self._sum_streams(
                    [self._internal_streams[s] for s in lot["streams"][stream_id]])

    def _create_const_flow(self, value: float) -> cd3.Flow:
        f = cd3.Flow()
        f[0] = value
        return f

    def _add_storage(self, storage):

        s = self._cd3.add_node("RWHT")
        s.setDoubleParameter("storage_volume", storage["volume"])

        self._lot_storage_reporting[self._id][storage["id"]] = s

        demand_stream = self._internal_streams[storage["demand"]]
        inflow_stream = self._internal_streams[storage["inflow"]]

        self._cd3.add_connection(demand_stream[0], demand_stream[1], s, "in_np")
        self._cd3.add_connection(inflow_stream[0], inflow_stream[1], s, "in_sw")
        f_inflow_stream = self._add_flow_probe(s, "out_sw")
        f_demand_stream = self._add_flow_probe(s, "out_np")
        inflow_stream[0] = f_inflow_stream[0]
        inflow_stream[1] = f_inflow_stream[1]
        demand_stream[0] = f_demand_stream[0]
        demand_stream[1] = f_demand_stream[1]

    def _create_demand_node(self, residents: float):
        # Produces non-potable (out_np) and potable demands (out_p)
        consumer = self._cd3.add_node("Consumption")
        l_d_to_m_s = 1. / (1000. * 60. * 60. * 24.)

        leak_other = 6.
        washing_machine = 22.
        taps = 21.
        toilet = 19.
        shower_bath = 34.

        consumer.setParameter("const_flow_potable", self._create_const_flow(
            (leak_other + washing_machine + taps + shower_bath) * l_d_to_m_s * residents))
        consumer.setParameter("const_flow_nonpotable", self._create_const_flow(toilet * l_d_to_m_s * residents))

        consumer.setParameter("const_flow_greywater",
                              self._create_const_flow((washing_machine + taps + shower_bath) * l_d_to_m_s * residents))
        consumer.setParameter("const_flow_sewer", self._create_const_flow((toilet) * l_d_to_m_s * residents))

        self._internal_streams[LotStream.potable_demand] = self._add_flow_probe(consumer, "out_p")
        self._internal_streams[LotStream.non_potable_demand] = self._add_flow_probe(consumer, "out_np")

        self._internal_streams[LotStream.grey_water] = self._add_flow_probe(consumer, "out_g")
        self._internal_streams[LotStream.black_water] = self._add_flow_probe(consumer, "out_s")

    def _add_flow_probe(self, out_port, port_name):
        flow_probe = self._cd3.add_node("FlowProbe")
        self._cd3.add_connection(out_port, port_name, flow_probe, "in")
        return list((flow_probe, "out"))

    def _create_stream(self, stream, area):
        source_vector = self._cd3.add_node("SourceVector")
        source_vector.setDoubleVectorParameter("source", stream)
        source_vector.setDoubleParameter("factor", area)
        flow_probe = self._add_flow_probe(source_vector, "out")
        return flow_probe

    def _sum_streams(self, streams: []) -> list:
        mixer = self._cd3.add_node("Mixer")
        mixer.setIntParameter("num_inputs", len(streams))
        for idx, s in enumerate(streams):
            s: list
            self._cd3.add_connection(s[0], s[1], mixer, f"in_{idx}")
        return list((mixer, "out"))
