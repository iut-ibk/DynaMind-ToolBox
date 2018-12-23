#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import sys

# sys.path.insert(0, "/Users/christianurich/Documents/Dynamind-ToolBox/build/release/output/")

import pycd3 as cd3
from pydynamind import *

class Polder(Module):

    display_name = "Polder"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("start_date", DM.STRING)
        self.start_date = "2005-Jan-01 00:00:00"

        self.createParameter("end_date", DM.STRING)
        self.end_date = "2006-Jan-01 00:00:00"

        self.cd3 = None
        self.flow_probes = dict()

    def init(self):
        self.timeseries = ViewContainer("timeseries", DM.COMPONENT, DM.READ)
        self.timeseries.addAttribute("data", DM.Attribute.DOUBLEVECTOR, DM.READ)

        self.polder = ViewContainer("polder", DM.COMPONENT, DM.READ)
        self.polder.addAttribute("area", DM.Attribute.DOUBLE, DM.READ)
        self.polder.addAttribute("surface_water_area", DM.Attribute.DOUBLE, DM.READ)
        self.polder.addAttribute("impervious_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.polder.addAttribute("storage_level", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.polder.addAttribute("total_pollution", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.polder.addAttribute("overflow", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.polder.addAttribute("overflow_concentration", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.polder.addAttribute("run_off", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.polder.addAttribute("run_off", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.polder.addAttribute("run_off_concentration", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.polder.addAttribute("provided_water", DM.Attribute.DOUBLEVECTOR, DM.WRITE)

        self.reticulation = ViewContainer("reticulation", DM.COMPONENT, DM.READ)
        self.reticulation.addAttribute("pumping_rate", DM.Attribute.DOUBLE, DM.READ)
        self.reticulation.addAttribute("removal_capacity", DM.Attribute.DOUBLE, DM.READ)
        self.reticulation.addAttribute("removed_pollution", DM.Attribute.DOUBLE, DM.WRITE)


        self.pump = ViewContainer("polder_pump", DM.COMPONENT, DM.READ)
        self.pump.addAttribute("type", DM.Attribute.STRING, DM.READ)
        self.pump.addAttribute("pumping_rate", DM.Attribute.DOUBLE, DM.READ)
        self.pump.addAttribute("trigger_volume", DM.Attribute.DOUBLE, DM.READ)

        # view_register = [self.view_polder, self.timeseries]
        view_register = [
            self.timeseries,
            self.polder,
            self.reticulation,
            self.pump]

        self.registerViewContainers(view_register)
        self.treatments = []

    def init_citydrain(self):
        flow = {'Q': cd3.Flow.flow, 'N': cd3.Flow.concentration}
        # print flow
        self.cd3 = cd3.CityDrain3(
            self.start_date,
            self.end_date,
            "86400",
            flow
        )

        # Register Modules
        self.cd3.register_native_plugin(
            self.getSimulationConfig().getDefaultLibraryPath() + "/libcd3core")
        self.cd3.register_native_plugin(
            self.getSimulationConfig().getDefaultLibraryPath() + "/CD3Modules/libdance4water-nodes")

    def setup_catchment(self, polder):
        """
        Creates initial CD3 model. Including rainfall import -> impervious runoff -> flow probe (Q and TN)

        """
        # rain = self.cd3.add_node("IxxRainRead_v2")
        # rain.setStringParameter("rain_file", "/tmp/rainfall_clean.ixx")

        self.timeseries.reset_reading()
        self.timeseries.set_attribute_filter("type = 'rainfall intensity'")
        for t in self.timeseries:
            rain_data = dm_get_double_list(t, "data")

        rain = self.cd3.add_node("SourceVector")
        rain.setDoubleVectorParameter("source", rain_data)

        catchment = self.cd3.add_node("ImperviousRunoff")

        catchment.setDoubleParameter("area", polder.GetFieldAsDouble("area") * polder.GetFieldAsDouble("impervious_fraction"))
        catchment.setDoubleVectorParameter("loadings", [2.4])

        # Measure Catchment Runoff
        flow_probe = self.cd3.add_node("FlowProbe")

        flow_probe_n = self.cd3.add_node("FlowProbe")
        flow_probe_n.setIntParameter("element", 1)

        self.cd3.add_connection(rain, "out", catchment, "rain_in")
        self.cd3.add_connection(catchment, "out_sw", flow_probe, "in")

        self.cd3.add_connection(flow_probe, "out", flow_probe_n, "in")

        self.flow_probes["catchment"] = flow_probe
        self.flow_probes["catchment_n"] = flow_probe_n

        return [flow_probe_n, "out"]

    def setup_polder(self, mixer, pump_volumes, surface_water_area):
        """
        Add polder nodes and pumps
        Polder  -> Pumps -> Probe (Overflow)
                -> Pumps -> Probe -> Mixer -> (Polder)
        """
        polder = self.cd3.add_node("Polder")

        pv = []
        for p in pump_volumes:
            pv.append(p[0])

        vv = []
        for p in pump_volumes:
            vv.append(p[1])

        # print pv
        # print vv

        evapo = self.cd3.add_node("MonthlyEvo")

        polder.setDoubleVectorParameter("Qp", pv)
        polder.setDoubleVectorParameter("Vmin", vv)
        polder.setDoubleParameter("surface_area", surface_water_area)
        self.cd3.init_nodes()

        self.cd3.add_connection(evapo, "out", polder, "evapo")
        self.cd3.add_connection(mixer[0], "out", polder, "in")

        mixer_port_id = 0
        for i in range(len(pump_volumes)):
            if not pump_volumes[i][2]:
                # Add pump to overflow
                flow_probe_pump = self.cd3.add_node("FlowProbe")
                self.cd3.add_connection(polder, "out_" + str(i), flow_probe_pump, "in")
                self.flow_probes["flow_" + str(i)] = flow_probe_pump

                flow_probe_n = self.cd3.add_node("FlowProbe")
                flow_probe_n.setIntParameter("element", 1)
                self.cd3.add_connection(flow_probe_pump, "out", flow_probe_n, "in")
                self.flow_probes["flow_n_" + str(i)] = flow_probe_n
                continue

            # Add Circulation to wetland
            mixer_port_id += 1
            self.add_loop(i, polder, mixer, mixer_port_id)

        return polder

    def add_loop(self, id, polder, mixer, mixer_port_id):
        # print id, mixer
        treatment = self.cd3.add_node("SimpleTreatment")
        treatment.setDoubleParameter("removal_fraction", 0.25)
        self.treatments.append(treatment)

        n_start = self.cd3.add_node("CycleNodeStart")

        n_end = self.cd3.add_node("CycleNodeEnd")
        n_end.setNodeParameter("start", n_start)

        flow_probe_pump_1 = self.cd3.add_node("FlowProbe")
        self.cd3.add_connection(polder, "out_" + str(id), treatment, "in")
        self.cd3.add_connection(treatment, "out", flow_probe_pump_1, "in")
        self.cd3.add_connection(flow_probe_pump_1, "out", n_end, "in")

        self.cd3.add_connection(n_start, "out", mixer[0], "in_" + str(mixer_port_id))

        self.flow_probes["flow_" + str(id)] = flow_probe_pump_1

        return n_start, n_end

    def setup_mixer(self, pump_volumes):
        mixer = self.cd3.add_node("Mixer")
        ports = 1
        for p in pump_volumes:
            if p[2]:
                ports += 1
        print "number of ports ", ports

        # Number of inputs depends on number of connected pumps
        mixer.setIntParameter("num_inputs", ports)

        return [mixer, "in"]

    def connect_catchment(self, catchment, mixer):
        self.cd3.add_connection(catchment[0], catchment[1], mixer[0], "in_0")

    def run(self):
        # Calculate pump volumes
        pump_volumes = []
        for p in self.pump:
            pump_volumes.append([p.GetFieldAsDouble("pumping_rate"),  p.GetFieldAsDouble("trigger_volume"), False, p])

        # Init pumps
        reticulations = []
        for r in self.reticulation:
            pump_volumes.append([r.GetFieldAsDouble("pumping_rate"), 100, True, r])
            reticulations.append(r)


        for polder in self.polder:
            self.init_citydrain()

            # Setup inital dynamind model
            c = self.setup_catchment(polder)

            # Set up mixer needed to connect all pumps
            m = self.setup_mixer(pump_volumes)

            # Add pumps to polder model
            p = self.setup_polder(m, pump_volumes, polder.GetFieldAsDouble("surface_water_area"))



            self.cd3.init_nodes()
            self.connect_catchment(c, m)

            self.cd3.start(self.start_date)

            dm_set_double_list(polder, "storage_level",  p.get_state_value_as_double_vector("storage_level"))
            dm_set_double_list(polder, "total_pollution", p.get_state_value_as_double_vector("total_pollution"))
            dm_set_double_list(polder, "run_off", self.flow_probes["catchment"].get_state_value_as_double_vector("Flow"))

            #Calculate Overflow

            overflow = []
            water_supply = []
            overflow_concentration = []

            for idx, p in enumerate(pump_volumes):
                data_array_c = None
                if p[2]:
                    continue
                type = p[3].GetFieldAsString("type")

                if type == "overflow":
                    data_array = overflow
                    data_array_c = overflow_concentration

                else:
                    data_array = water_supply
                o = self.flow_probes["flow_" + str(idx)].get_state_value_as_double_vector("Flow")
                c = []
                if data_array_c is not None:
                    c = self.flow_probes["flow_n_" + str(idx)].get_state_value_as_double_vector("Flow")
                    if len(data_array_c) == 0:
                        for i in c:
                            data_array_c.append(i)
                    else:
                        for i, v in enumerate(c):
                            data_array_c[i] = data_array_c[i] + v
                if len(data_array) == 0:
                    for i in o:
                        data_array.append(i)
                else:
                    for i, v in enumerate(o):
                        data_array[i] = data_array[i] + v



            dm_set_double_list(polder, "overflow", overflow)
            dm_set_double_list(polder, "overflow_concentration", overflow_concentration)
            dm_set_double_list(polder, "provided_water", water_supply)
            dm_set_double_list(polder, "run_off_concentration", self.flow_probes["catchment_n"]
                               .get_state_value_as_double_vector("Flow"))

            # print sum(overflow), sum(water_supply)

            for idx, r in enumerate(reticulations):
                r.SetField("removed_pollution", self.treatments[idx].get_state_value_as_double_vector("treated")[0])

        self.reticulation.finalise()
        self.timeseries.finalise()
        self.polder.finalise()
        self.pump.finalise()
