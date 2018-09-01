#!/usr/bin/env python
#  -*- coding: utf-8 -*-

import sys

sys.path.insert(0, "/Users/christianurich/Documents/Dynamind-ToolBox/build/release/output/")

import pycd3 as cd3
from pydynamind import *
# import matplotlib.pyplot as plt
#
# plt.style.use('ggplot')


#

# %%

class Polder(Module):

    display_name = "Polder"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.cd3 = None
        self.flow_probes = dict()

    def init(self):
        # self.view_polder = ViewContainer("polder", DM.COMPONENT, DM.READ)
        # self.view_polder.addAttribute("area", DM.Attribute.DOUBLE, DM.READ)

        self.timeseries = ViewContainer("timeseries", DM.COMPONENT, DM.READ)
        self.timeseries.addAttribute("data", DM.Attribute.DOUBLEVECTOR, DM.READ)

        # view_register = [self.view_polder, self.timeseries]
        view_register = [self.timeseries]

        self.registerViewContainers(view_register)

    def init_citydrain(self):
        flow = {'Q': cd3.Flow.flow, 'N': cd3.Flow.concentration}
        print flow
        self.cd3 = cd3.CityDrain3(
            "2005-Jan-01 00:00:00",
            "2006-Jan-01 00:00:00",
            "86400",
            flow
        )

        # Register Modules
        self.cd3.register_native_plugin(
            self.getSimulationConfig().getDefaultLibraryPath() + "/libcd3core")
        self.cd3.register_native_plugin(
            self.getSimulationConfig().getDefaultLibraryPath() + "/CD3Modules/libdance4water-nodes")

    def setup_catchment(self, rain_data):
        # rain = self.cd3.add_node("IxxRainRead_v2")
        # rain.setStringParameter("rain_file", "/tmp/rainfall_clean.ixx")

        rain = self.cd3.add_node("SourceVector")
        rain.setDoubleVectorParameter("source", rain_data)

        catchment = self.cd3.add_node("ImperviousRunoff")
        catchment.setDoubleParameter("area", 1000.)
        catchment.setDoubleVectorParameter("loadings", [2.4])

        flow_probe = self.cd3.add_node("FlowProbe")

        self.cd3.add_connection(rain, "out", catchment, "rain_in")
        self.cd3.add_connection(catchment, "out_sw", flow_probe, "in")

        self.flow_probes["catchment"] = flow_probe

        return [flow_probe, "out"]

    def setup_polder(self, mixer):
        pump_volumes = [2000., 100. / 86400., 100. / 86400.]
        volumes = [1000., 100., 100.]
        polder = self.cd3.add_node("Polder")
        polder.setDoubleVectorParameter("Qp", pump_volumes)
        polder.setDoubleVectorParameter("Vmin", volumes)
        self.cd3.init_nodes()

        self.cd3.add_connection(mixer[0], "out", polder, "in")

        for i in range(len(pump_volumes)-1):
            self.add_loop(i, polder, mixer)

        return polder

    def add_loop(self, id, polder, mixer):
        # print id, mixer
        treatment = self.cd3.add_node("SimpleTreatment")
        treatment.setDoubleParameter("removal_fraction", 0.25)

        n_start = self.cd3.add_node("CycleNodeStart")

        n_end = self.cd3.add_node("CycleNodeEnd")
        n_end.setNodeParameter("start", n_start)

        flow_probe_pump_1 = self.cd3.add_node("FlowProbe")
        self.cd3.add_connection(polder, "out_" + str(id+1), treatment, "in")
        self.cd3.add_connection(treatment, "out", flow_probe_pump_1, "in")
        self.cd3.add_connection(flow_probe_pump_1, "out", n_end, "in")

        self.cd3.add_connection(n_start, "out", mixer[0], "in_" + str(id+1))

        self.flow_probes["treatment_" + str(id+1)] = flow_probe_pump_1

        return n_start, n_end

    def setup_mixer(self, ports):
        mixer = self.cd3.add_node("Mixer")

        # Number of inputs depends on number of connected pumps
        mixer.setIntParameter("num_inputs", ports)

        return [mixer, "in"]

    def connect_catchment(self, catchment, mixer):
        self.cd3.add_connection(catchment[0], catchment[1], mixer[0], "in_0")

    def run(self):



        self.timeseries.reset_reading()
        # self.timeseries.set_attribute_filter("type = 'rainfall intensity' or type = 'evapotranspiration'")
        self.timeseries.set_attribute_filter("type = 'rainfall intensity'")
        for t in self.timeseries:
            rain = dm_get_double_list(t, "data")
            print rain




        self.init_citydrain()
        c = self.setup_catchment(rain)
        m = self.setup_mixer(3)
        p = self.setup_polder(m)

        self.cd3.init_nodes()
        self.connect_catchment(c, m)

        self.cd3.start("2005-Jan-01 00:00:00")

        # print p.get_state_value_as_double("StorageVolume")
        print p.get_state_value_as_double_vector("storage_level")
        for probe in self.flow_probes.keys():
            print probe, self.flow_probes[probe].get_state_value_as_double_vector("Flow")

        self.timeseries.finalise()

        # flow_probe = self.cd3.add_node("FlowProbe")
        # flow_probe_pump = self.cd3.add_node("FlowProbe")
        #
        # flow_probe_pump_1 = self.cd3.add_node("FlowProbe")
        # flow_probe_pump_1.setIntParameter("element", 1)
        #
        # flow_probe_pump_c = self.cd3.add_node("FlowProbe")
        # # flow_probe_pump_c.setIntParameter("element", 1)


if __name__ == "__main__":
    polder = Polder()
    polder.run()

#
# c.add_connection(rain, "out", catchment, "rain_in")
# c.add_connection(catchment, "out_sw", flow_probe_pump_c, "in")
#
# c.add_connection(flow_probe_pump_c, "out", mixer, "in_0")
# c.add_connection(n_start, "out", mixer, "in_1")
#
# c.add_connection(flow_probe_pump, "out", treatment, "in")
#
# c.add_connection(mixer, "out", flow_probe, "in")
# c.add_connection(flow_probe, "out", polder, "in")
#
# c.add_connection(treatment, "out", n_end, "in")
#
# c.add_connection(polder, "out_0", flow_probe_pump, "in")
# c.add_connection(polder, "out_1", flow_probe_pump_1, "in")
#
# # Execute Simulation
#
# c.start("2005-Jan-01 00:00:00")
#
# print flow_probe.get_state_value_as_double_vector("Flow")
# print polder.get_state_value_as_double("StorageVolume")
#
# print "flow_0", flow_probe_pump.get_state_value_as_double_vector("Flow")
# print "flow_1", flow_probe_pump_1.get_state_value_as_double_vector("Flow")
# print "flow_c", flow_probe_pump_c.get_state_value_as_double_vector("Flow")
#
# print "total_load", polder.get_state_value_as_double_vector("total_pollution")
#
# # %%
# x = []
# for idx in enumerate(polder.get_state_value_as_double_vector("total_pollution")):
#     x.append(idx[0])
# x1 = []
# for idx in enumerate(flow_probe_pump_1.get_state_value_as_double_vector("Flow")):
#     x1.append(idx[0])
#
# # plt.plot(x, flow_probe_pump_1.get_state_value_as_double_vector("Flow"))
# plt.subplot(311)
# plt.plot(x, polder.get_state_value_as_double_vector("storage_level"))
# plt.subplot(312)
# plt.plot(x, polder.get_state_value_as_double_vector("total_pollution"))
# plt.subplot(313)
# plt.plot(x1, flow_probe_pump_1.get_state_value_as_double_vector("Flow"))
# plt.show()
#
