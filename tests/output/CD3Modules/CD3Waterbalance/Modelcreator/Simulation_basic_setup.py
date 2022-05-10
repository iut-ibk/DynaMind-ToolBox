# -*- coding: utf-8 -*-
"""
Created on Mon Dec 01 12:01:01 2014

@author: Acer
"""

#EXAMPLE: [startdate, stopdate] = ['2000-Jan-01 00:00:00', '2001-Jan-01 00:00:00']
class Simulation_basic_setup:
    def __init__(self, startdate, stopdate, timestep, module):
         self.startdate = startdate
         self.stopdate = stopdate
         self.timestep = timestep
         self.module= module
         '''
         the line <pluginpath path="nodes"/> causes problems in the latest citydrain version?!
         '''
    
         self.string1 = """<?xml version="1.0" encoding="UTF-8"?>
<citydrain version="1.0">
    <pluginpath path="nodes"/> 
    <pythonmodule module="""
         self.string2="""/>
    <pythonmodule module="""
         self.string3="""/>
    <simulation class="DefaultSimulation">
        <time start=""" 
         self.string4 = """ stop="""
    
         self.string5=""" dt="""
         self.string6=""">
            <flowdefinition>
                <flow name="Q"/>
                <concentration name="BOD5"/>
                <concentration name="COD"/>
                <concentration name="Cu"/>
                <concentration name="Ntot"/>
            </flowdefinition>
        </time>
    </simulation>
    <model>
        <nodelist> \n """ 
         self.Simulationsetupstring = self.string1 +'"'+ str(self.module) +'"'+ self.string2 +'"'+ str(self.module) +'"'+ self.string3 +'"'+ str(self.startdate) +'"'+ self.string4 +'"'+ str(self.stopdate) +'"'+ self.string5 +'"'+ str(self.timestep) +'"'+ self.string6
