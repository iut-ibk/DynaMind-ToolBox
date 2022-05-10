# -*- coding: utf-8 -*-
"""
Created on Mon Dec 01 15:11:56 2014

@author: Acer
"""
from Singleton import Singleton

@Singleton
class Global_counters(object):
    def __init__(self):

        self.number_of_buildings = 0
        self.number_of_connections = 0
        self.number_of_greywatertanks = 0     
        self.number_of_raintanks = 0
        self.number_of_collectors = 0
        self.number_of_distributors = 0
        self.number_of_stormwaterpipes = 0
        self.number_of_sewers = 0
        self.number_of_potablwaterreservoirs = 0
        self.number_of_catchments = 0
        self.number_of_demandmodels = 0
        self.number_of_stormwaterreservoirs = 0
        self.number_of_filereaders = 0
        self.number_of_patternimplementers = 0
        self.number_of_fileouts = 0
        self.number_of_gardenwateringmodules = 0
        self.number_of_greywaterreservoirs = 0
        self.number_of_clusters = 0
        self.numbers_of_buildinglevels_created = 0
        self.number_of_collectors_ports_list = []
        self.number_of_distributors_ports_list = []
        self.numbers_names_of_fileouts_list = []
        

    


