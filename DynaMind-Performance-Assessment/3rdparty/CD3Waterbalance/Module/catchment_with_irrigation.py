import pandas as pd
import math
from enum import Enum
import pycd3

class Catchment_w_Irrigation(pycd3.Node):

    def __init__(self):
        pycd3.Node.__init__(self)

        # in flows
        self.rain = pycd3.Flow()
        self.evapo = pycd3.Flow()
        self.irrigation = pycd3.Flow()

        # out flows
        self.roof_runoff = pycd3.Flow()
        self.impervious_runoff = pycd3.Flow()
        self.pervious_runoff = pycd3.Flow()
        self.saturation_runoff = pycd3.Flow()
        self.actual_infiltration = pycd3.Flow()
        self.groundwater_infiltration = pycd3.Flow()
        self.pervious_evapotranspiration = pycd3.Flow()
        self.impervious_evapotranspiration = pycd3.Flow()
        self.irrigation_required = pycd3.Flow()
        self.poss_infiltration = pycd3.Flow()
        self.pervious_level = pycd3.Flow()
        self.Perv_Rainstorage = pycd3.Flow()
        

        # in ports
        self.addInPort("Rain", self.rain)
        self.addInPort("Evapotranspiration", self.evapo)
        self.addInPort("irrigation", self.irrigation)

        # out ports
        self.addOutPort("roof_runoff", self.roof_runoff)
        self.addOutPort("impervious_runoff", self.impervious_runoff)
        self.addOutPort("pervious_runoff", self.pervious_runoff) #infiltration excess and saturation runoff combined into one outport
        #self.addOutPort("saturation_runoff", self.saturation_runoff)
        self.addOutPort('possible_infiltration',self.poss_infiltration)
        self.addOutPort("actual_infiltration", self.actual_infiltration)
        self.addOutPort("groundwater_infiltration", self.groundwater_infiltration)
        self.addOutPort("pervious_evapotranspiration", self.pervious_evapotranspiration)
        self.addOutPort('impervious_evapotranspiration',self.impervious_evapotranspiration)

        self.addOutPort("outdoor_demand", self.irrigation_required) # not sure about this one just yet
        self.addOutPort('pervious_storage', self.pervious_level)
        self.addOutPort('Perv_Rainstorage', self.Perv_Rainstorage)


        # parameters to be set by dynamind
        self.area_property = pycd3.Double(1000)
        self.addParameter("Catchment_Area_[m^2]", self.area_property)
        self.perv_area = pycd3.Double(0.4)
        self.addParameter("Fraktion_of_Pervious_Area_pA_[-]", self.perv_area)
        self.imp_area_stormwater = pycd3.Double(0.4)
        self.addParameter("Fraktion_of_Impervious_Area_to_Stormwater_Drain_iASD_[-]", self.imp_area_stormwater)
        self.imp_area_raintank = pycd3.Double(0.2)
        self.addParameter("Fraktion_of_Impervious_Area_to_Reservoir_iAR_[-]", self.imp_area_raintank)
        self.Horton_initial_cap = pycd3.Double(0.09)
        self.addParameter("Initial_Infiltration_Capacity_[m/h]", self.Horton_initial_cap)
        self.Horton_final_cap = pycd3.Double(0.001)
        self.addParameter("Final_Infiltration_Capacity_[m/h]", self.Horton_final_cap)
        self.Horton_decay_constant = pycd3.Double(0.06)
        self.addParameter("Decay_Constant_[1/min]", self.Horton_decay_constant)

        self.perv_soil_storage_capacity = pycd3.Double(0.030)
        self.addParameter("Soil Storage Capacity [m]", self.perv_soil_storage_capacity)
        self.daily_recharge_rate = pycd3.Double(0.25) # in %
        self.addParameter("Daily Recharge Rate", self.daily_recharge_rate)
        self.transpiration_capacity = pycd3.Double(0.007)
        self.addParameter("Transpire Capacity", self.transpiration_capacity)
        self.initial_loss = pycd3.Double(0.004)
        self.addParameter("Wetting_Loss_[m]", self.initial_loss)

        # new parameters to be set by dynamind
        self.initial_perv_storage_level = pycd3.Double(0.3)
        self.addParameter("Initial_Pervious_Storage_Level_[m]", self.initial_perv_storage_level)
        self.wilting_point = pycd3.Double(0.09)
        self.addParameter("Wilting_Point_[%]", self.wilting_point)
        self.field_capacity = pycd3.Double(0.22)
        self.addParameter("Field_Capacity_[%]", self.field_capacity)
        self.saturation = pycd3.Double(0.4)
        self.addParameter("Saturation_[%]", self.saturation)

        # this allows automatic watering of the system to a particular level specified in the code
        self.automatic_irrigation = False



    def init(self, start, stop, dt):

        # other parameters to store information within the class
        self.dt =dt
        self.num_of_continuous_dry_days = 0
        self.num_of_continuous_wet_days = 0
        self.temp_cap_dry = self.Horton_initial_cap/3600.*self.dt
        self.temp_cap_wet = self.Horton_initial_cap/3600.*self.dt
        self.possible_infiltr_raw = self.Horton_final_cap/3600.*self.dt
        self.rain_storage_imp = 0
        self.rain_storage_perv = 0
        self.surface_evaporation_imp = 0
        self.surface_evaporation_perv = 0
        self.condition = 'dry'
        self.current_perv_storage_level = 1 * self.initial_perv_storage_level
        self.itteration = 1

        #the previous day storage values for checking the mass balance
        self.previous_perv_storage_level = 0
        self.previous_rain_storage_imp = 0
        self.previous_rain_storage_perv = 0
        
        # if the timestep is daily and irrigation is appiled, then the infiltration is overestimated
        # as irrigation only occurs for no longer than one hour
        # if self.dt == 86400:
        #     self.daily_time_step = True
        # else:
        #     self.daily_time_step = False

        self.daily_time_step = True



        return True

    #     self.total_rain = 0.
    #     self.total_actual_infiltr = 0.
    #     self.total_collected_w = 0.
    #     self.total_outdoor_use = 0.
    #     self.total_runoff = 0.
    #     self.total_effective_rain = 0.
    #     self.total_groundwater = 0.
    #     self.total_evapotranspiration = 0.    
    ###################################################################################################################################

    ############ Not Sure IF Current is needed or if def needs to be called f (C++ thing) ###############
    def f(self, current, dt):

        #record the previous days, storage levels
        self.previous_perv_storage_level = self.current_perv_storage_level
        self.previous_rain_storage_imp = self.rain_storage_imp
        self.previous_rain_storage_perv = self.rain_storage_perv

        # on rainy days, there is no surface evaporation and this needs to be reset to 0
        self.surface_evaporation_perv = 0
        self.surface_evaporation_imp = 0

        # determine the irrigation requiements for the timestep only if it isnt going to rain
        # in this way, we look at the final soil store from the previous day and will add additional water to bring
        # the store to the target level (in this case it is field capacty but can be changed)
        if (self.automatic_irrigation == True) and (self.rain[0] == 0):
            self.irrigation_required[0] = self.irrigation_requiements()
        else:
            self.irrigation_required[0] = self.irrigation[0] * self.area_property * self.perv_area

        # if it rained yesterday but not today, reset num of dry days
        if (self.condition == 'wet') and (self.rain[0] + self.irrigation[0] == 0):
            self.num_of_continuous_dry_days = 1
            self.condition = 'dry'

        # if it did not rain yesterday but did today, reset num of wet days    
        elif (self.condition == 'dry') and (self.rain[0] + self.irrigation[0] > 0):
            self.num_of_continuous_wet_days = 1
            self.condition = 'wet'
        
        # if it did not rain yesterday and did not today, increase number of non raindays
        elif (self.condition == 'dry') and (self.rain[0] + self.irrigation[0] == 0):
            self.num_of_continuous_dry_days += 1
            self.condition = 'dry'

        #if it did rain yesterday and did rain today, increase number of rain days
        elif (self.condition == 'wet') and (self.rain[0] + self.irrigation[0] > 0): 
            self.num_of_continuous_wet_days += 1
            self.condition = 'wet'

        # calculate the runoffs and evapotranspirations
        self.roof_runoff[0] = self.roof_runoff_loss()
        self.impervious_runoff[0] = self.impervious_runoff_loss()
        self.poss_infiltration[0] = self.possible_infiltration(self.condition)
        infiltration, self.pervious_runoff[0] = self.pervious_infiltration_runoff()

        # update soil storage due to infiltration gain and saturation excess loss
        sat_runoff, infiltration = self.saturartion_runoff(infiltration)
        self.pervious_runoff[0] += sat_runoff
        self.actual_infiltration[0] = infiltration

        # update soil storage due to evapotranspiration loss 
        self.pervious_evapotranspiration[0] = self.evapotranspiration_loss() + self.surface_evaporation_perv
        self.impervious_evapotranspiration[0] = self.surface_evaporation_imp

        # update soil storage and groundwater loss
        self.groundwater_infiltration[0] = self.ground_water_loss()

        self.pervious_level[0] = self.current_perv_storage_level
        self.Perv_Rainstorage[0] = self.rain_storage_perv
        
        # write a mass balance test to ensure all the streams are adding up
        self.mass_balance_test()


        return dt

#################### Functions  Used In Run ###############################

    # return the voume of water that runs off the roof
    def roof_runoff_loss(self):
        return self.rain[0] * self.area_property * self.imp_area_raintank

    # return the volume of water that runs off impervious areas
    def impervious_runoff_loss(self):

        # update the rain_storage (this is only relavent when rain occured in the previous timestep
        # and therefore some rain is already stored in the system)
        if self.imp_area_stormwater > 0:    
            self.rain_storage_imp += self.rain[0]
            runoff = max([self.rain_storage_imp - self.initial_loss,0]) * self.area_property * self.imp_area_stormwater
            #runoff = max([self.rain_storage_imp - self.initial_loss - self.depression_loss,0]) * self.area_property * self.imp_area_stormwater

            # reset the rain store to be full
            if runoff > 0:
                self.rain_storage_imp = self.initial_loss
            return runoff
        else:
            return 0

    # calculates the infiltration rate for the given timestep using hortens model
    def possible_infiltration(self,conditions):

        if conditions == 'wet':
            self.temp_cap_wet = self.Horton_initial_cap / 3600. * self.dt - (self.Horton_initial_cap / 3600 * self.dt - self.temp_cap_dry) * math.exp( -1 * self.Horton_decay_constant * self.dt / 60. * self.num_of_continuous_dry_days)
            self.possible_infiltr_raw = self.Horton_final_cap / 3600. * self.dt + (self.temp_cap_wet - self.Horton_final_cap / 3600. * self.dt) * math.exp(-1 * self.Horton_decay_constant * self.dt / 60. * self.num_of_continuous_wet_days)
            #self.num_of_continuous_wet_days += 1.0
            self.surface_evaporation = 0

        elif conditions == 'dry':
            self.temp_cap_dry = self.Horton_final_cap/3600.*self.dt + (self.temp_cap_wet - self.Horton_final_cap/3600.*self.dt) * math.exp(-1*self.Horton_decay_constant * self.dt / 60. * self.num_of_continuous_wet_days)
            self.possible_infiltr_raw = self.Horton_initial_cap/3600.*self.dt - (self.Horton_initial_cap/3600.*self.dt - self.temp_cap_dry) * math.exp(-1*self.Horton_decay_constant * self.dt / 60. * self.num_of_continuous_dry_days)
            #self.num_of_continuous_dry_days += 1.0

            #if it is dry then the amount of water in the imp and perv rain storege will decrease by evaporation, this needs to be stored and added onto daily total evoptranspiration to preseve mass.
            actual_evapo = self.actual_evapotranspiration()
            store_before_imp = self.rain_storage_imp
            store_before_perv = self.rain_storage_perv
            
            # self.rain_storage_imp = max([self.rain_storage_imp - actual_evapo,0])
            # self.rain_storage_perv = max([self.rain_storage_perv - actual_evapo,0])

            # when the soil is at the wiling point, the actual evapo is will be calculated as zero and this causes
            # no water to be evaporated from the surface which is incorrect. We have made a hard assumtion that on
            # dry days, the rain storage will decrease by half to reflect the evaporation 
            self.rain_storage_imp -= (0.5 * self.rain_storage_imp)
            self.rain_storage_perv -= (0.5 * self.rain_storage_perv)
           
            store_after_imp = self.rain_storage_imp
            store_after_perv = self.rain_storage_perv
            if self.imp_area_stormwater > 0:
                self.surface_evaporation_imp = (store_before_imp - store_after_imp) * self.area_property * (self.imp_area_stormwater)
            if self.perv_area > 0:
                self.surface_evaporation_perv = (store_before_perv - store_after_perv) * self.area_property * (self.perv_area)

        return self.possible_infiltr_raw

    # determines the volume of water infiltrating into and running off the pervious region of a lot
    def pervious_infiltration_runoff(self):
        if self.perv_area > 0:
            try:
                fraction = self.rain[0] / (self.rain[0] + self.irrigation[0])
            except ZeroDivisionError:
                fraction = 1
            
            self.rain_storage_perv += (self.rain[0] + self.irrigation[0])
            
            avaliable_water = max([(self.rain_storage_perv - self.initial_loss),0])
            
            # as possible infiltration is given on a daily timestep, this completely overestimates the infiltration of irrigation which 
            # we know for certain lasts no more than a hour. In this case, the irrigation portion of the model will consider only 1 hours worth of infiltration 
            if self.daily_time_step:
                
                infiltration = min([self.possible_infiltr_raw, avaliable_water * fraction]) * self.area_property * self.perv_area + min([self.possible_infiltr_raw/24, avaliable_water * (1- fraction)]) * self.area_property * self.perv_area

            else:
                
                infiltration = min([self.possible_infiltr_raw, avaliable_water]) * self.area_property * self.perv_area

            runoff = (avaliable_water * self.area_property * self.perv_area ) - infiltration

            # reset the rain store to be full
            if infiltration > 0:
                self.rain_storage_perv = self.initial_loss
            return infiltration, runoff
        else:
            return 0, 0


    # calculate the actucal evapotranspiration based on the potential evapotranspiration and the soil moisture
    def actual_evapotranspiration(self):
 
        soil_moisture = (self.current_perv_storage_level * 1000)/(self.perv_soil_storage_capacity * 1000)
        # added the transpiration_constant to the soil moisture to allow for the calibration of the evapotranspiration
        stress_index = (soil_moisture - self.wilting_point)/(self.field_capacity-self.wilting_point)

        if stress_index < 0.00001:
            return 0 
        elif stress_index < 1:
            return self.transpiration_capacity * stress_index * self.evapo[0]
        else:
            return self.evapo[0]


    def saturartion_runoff(self, infiltration: float):
        
        self.current_perv_storage_level += (infiltration / (self.area_property * self.perv_area))

        saturation_runoff = self.current_perv_storage_level - (self.saturation * self.perv_soil_storage_capacity)
        saturation_runoff = max([saturation_runoff, 0])

        infiltration -= saturation_runoff

        self.current_perv_storage_level -= saturation_runoff
        return saturation_runoff * self.area_property * self.perv_area, infiltration

    def evapotranspiration_loss(self):
        actual_evapo = self.actual_evapotranspiration()
        
        self.current_perv_storage_level -= actual_evapo

        # sometimes, it is possible for the caluclated Aet to draw the soil water store below the wilting point
        # which is not possible. To avoid this, find the difference and add it back to the soil store and take it off the ET.
        if self.current_perv_storage_level < (self.perv_soil_storage_capacity * self.wilting_point):
            not_et = (self.perv_soil_storage_capacity * self.wilting_point) - self.current_perv_storage_level
            self.current_perv_storage_level += not_et
            actual_evapo -= not_et
        return actual_evapo * self.area_property * self.perv_area 


    def ground_water_loss(self):

        # ground water is only extracted if the soil moisture is above field capacity, ie. gravitational drainage to the point of field capacity.
        recharge_volume = self.daily_recharge_rate * (self.current_perv_storage_level - (self.field_capacity * self.perv_soil_storage_capacity))
        recharge_volume = max([recharge_volume,0])

        self.current_perv_storage_level -= recharge_volume
        return recharge_volume * self.area_property * self.perv_area
        
    # determines the amount of water that is needed to keep the soil moisture at the field capacity
    def irrigation_requiements(self):

        # if the soil moisture is below field capacity, then the amount of water needed to keep the soil moisture at field capacity is the amount of water needed to keep the soil moisture at saturation
        if self.current_perv_storage_level < (self.field_capacity * self.perv_soil_storage_capacity):

            irrigation_depth = (self.field_capacity * self.perv_soil_storage_capacity) - self.current_perv_storage_level
            self.irrigation[0] = irrigation_depth

            return irrigation_depth * self.area_property * self.perv_area        
        else:
            return 0
    
    def mass_balance_test(self):

        soil_storage = (self.current_perv_storage_level * self.area_property * self.perv_area) - (self.previous_perv_storage_level * self.area_property * self.perv_area)
        rain_storage = (self.rain_storage_imp * self.area_property * self.imp_area_stormwater) + (self.rain_storage_perv * self.area_property * self.perv_area) - (self.previous_rain_storage_imp * self.area_property * self.imp_area_stormwater) - (self.previous_rain_storage_perv * self.area_property * self.perv_area)

        rainfall = (self.rain[0] * self.area_property) + (self.irrigation[0] * self.area_property * self.perv_area)
        evaporation = (self.pervious_evapotranspiration[0]) + (self.impervious_evapotranspiration[0])
        runoff = (self.pervious_runoff[0]) + (self.impervious_runoff[0]) + (self.roof_runoff[0])
        groundwater = self.groundwater_infiltration[0]

        balance = rainfall - soil_storage - rain_storage - evaporation - runoff - groundwater
        print(f'{self.itteration} Balance:', round(balance,4))
        #print('rainfall:',rainfall,'soil_storage:',soil_storage,'rain_storage:',rain_storage,'evaporation:',evaporation,'runoff:',runoff,'groundwater:',groundwater, 'level:',(self.rain_storage_perv * self.area_property * self.perv_area))
        self.itteration += 1



    def getClassName(self):
        #print "getClassName"
        return "Catchment_w_Irrigation"


    

