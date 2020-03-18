import pycd3 as cd3

class Lot:
    def __init__(self, name, cd3_instance: cd3.CityDrain3, lot_detail: {}, standard_values: {} ):

        self._id = name
        self._cd3 = cd3_instance
        self._standard_values = standard_values
        self._potable_demand = None
        self._non_potable_demand = None
        self._roof_runoff = None
        self._pervious_runoff = None
        self._impervious_runoff = None

        self._total_potable_demand = None
        self._grey_water = None
        self._black_water = None

        self._total_sewerage = None
        self._total_grey_water = None
        self._total_stormwater_runoff = None

        self._total_demand = None
        self._total_non_potable_demand = None
        self._total_outdoor_demand = None
        self._total_evapotranspiration = None
        self._total_infiltration = None

        # Assume I'll be able to get all parameters for database
        self._create_lot(lot_detail)

    @property
    def potable_demand(self) -> ():
        return self._total_potable_demand

    @property
    def outdoor_demand(self) -> ():
        return self._total_outdoor_demand

    @property
    def non_potable_demand(self) -> ():
        return self._total_non_potable_demand

    @property
    def storm_water_runoff(self) -> ():
        return self._total_stormwater_runoff

    @property
    def sewerage(self) -> ():
        return self._total_sewerage

    @property
    def grey_water(self) -> ():
        return self._total_grey_water

    @property
    def infiltration(self) -> ():
        return self._total_infiltration

    @property
    def evapotranspiration(self) -> ():
        return self._total_evapotranspiration

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

        self._roof_runoff = self._create_stream("roof_runoff", lot["roof_area"])
        self._impervious_runoff = self._create_stream("surface_runoff", lot["impervious_area"])
        self._outdoor_demand = self._create_stream("outdoor_demand", lot["irrigated_garden_area"])
        self._evapotranspiration = self._create_stream("effective_evapotranspiration", lot["irrigated_garden_area"])
        self._infiltration = self._create_stream("actual_infiltration", lot["irrigated_garden_area"])

        # This and reconnected
        if "storages" in lot:
            for s in lot["storages"]:
                self._add_storage(s)

        self._total_stormwater_runoff = self._sum_streams([getattr(self, s) for s in ["_roof_runoff", "_impervious_runoff"]])

        self._total_potable_demand = self._sum_streams([getattr(self, s) for s in lot["potable_demand"]])

        self._total_sewerage = self._sum_streams([getattr(self, s) for s in lot["sewerage"]])

        self._total_evapotranspiration = self._sum_streams([self._evapotranspiration])
        #
        self._total_infiltration = self._sum_streams([self._infiltration])

        if "grey_water" in lot:
            self._total_grey_water = self._sum_streams([getattr(self, s) for s in lot["grey_water"]])

        if "non_potable_demand" in lot:
            self._total_non_potable_demand = self._sum_streams([getattr(self, s) for s in lot["non_potable_demand"]])

        if "outdoor_demand" in lot:
            self._total_outdoor_demand = self._sum_streams(
                [getattr(self, s) for s in lot["outdoor_demand"]])

    def _create_const_flow(self, value: float) -> cd3.Flow:
        f = cd3.Flow()
        f[0] = value
        return f

    def _add_storage(self, storage):

        s = self._cd3.add_node("RWHT")
        s.setDoubleParameter("storage_volume", storage["volume"])

        demand_stream = getattr(self, storage["demand"])
        inflow_stream = getattr(self, storage["inflow"])

        self._cd3.add_connection(demand_stream[0], demand_stream[1], s, "in_np")
        self._cd3.add_connection(inflow_stream[0], inflow_stream[1], s, "in_sw")

        inflow_stream[0] = s
        inflow_stream[1] = "out_sw"
        demand_stream[0] = s
        demand_stream[1] = "out_np"

    def _create_demand_node(self, residents: float):
        # Produces non-potable (out_np) and potable demands (out_p)
        consumer = self._cd3.add_node("Consumption")
        l_d_to_m_s = 1. / (1000. * 60. * 60. * 24.)

        leak_other = 6.
        washing_machine = 22.
        taps = 21.
        toilet = 19.
        shower_bath = 34.

        consumer.setParameter("const_flow_potable", self._create_const_flow((leak_other + washing_machine + taps + shower_bath) * l_d_to_m_s * residents))
        consumer.setParameter("const_flow_nonpotable", self._create_const_flow(toilet * l_d_to_m_s * residents))

        consumer.setParameter("const_flow_greywater", self._create_const_flow((washing_machine + taps + shower_bath) * l_d_to_m_s * residents))
        consumer.setParameter("const_flow_sewer", self._create_const_flow((toilet + taps) * l_d_to_m_s * residents))

        self._potable_demand = list((consumer, "out_p"))
        self._non_potable_demand = list((consumer, "out_np"))

        self._grey_water = list((consumer, "out_g"))
        self._black_water = list((consumer, "out_s"))

    def _create_stream(self, stream, area):
        source_vector = self._cd3.add_node("SourceVector")
        source_vector.setDoubleVectorParameter("source", self._standard_values[stream])
        source_vector.setDoubleParameter("factor", area)
        return list((source_vector, "out"))

    def _sum_streams(self, streams: []) -> list:
        mixer = self._cd3.add_node("Mixer")
        mixer.setIntParameter("num_inputs", len(streams))

        for idx, s in enumerate(streams):
            s: list
            self._cd3.add_connection(s[0], s[1], mixer, f"in_{idx}")
        return list((mixer, "out"))