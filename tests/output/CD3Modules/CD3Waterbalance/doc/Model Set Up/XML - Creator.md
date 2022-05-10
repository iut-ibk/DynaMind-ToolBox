
# XML - Creator

Basic features:

 - automated setup of an CD3 - Model
 - 5 levels of implementation

      - Building Level
      -	Cluster Level
      - ReservoirlevelOne
      - ReservoirlevelTwo
      - Supply Level
	  
 - basic and selective flowmeter implemetation
 # 
 - ...
 
<br>

## Input


| Infrastructure Information  | Part of XML created  | Class created | 
| :------------ |:---------------:|:---------------:|  	
| Supplyvector      | Connectionlist | - |
| Catchment Attributevector | Nodelist |  Catchment  |
| Greywater Tank Attributevector | Nodelist | Greywater Tank   |
| Greywater Reservoir Attributevector | Nodelist | Greywater Reservoir   |
| Stormwater Reservoir Attributevector | Nodelist | Stormwater Reservoir   |   
| Rainwater Tank Attributevector | Nodelist | Rainwater Tank   |
| Garden Watering Attributevector | Nodelist | Garden Watering Module   |
| Building Demand Attributevector | Nodelist | Demand Model  |  


<br>

| Simulation Information  | Part of XML created  | Class created | 
| :------------ |:---------------:| 	:---------------:|
| Simulationsetup Vector  | Header, Pythonmodule, Simulation | -  |
| Needtohaveinputs Vector  | Nodelist |  File Reader, Pattern Implementer |

<br>

| Model Outputs  | Part of XML created  | Class created | 
| :------------ |:---------------:| 	:---------------:|
| Fileout  Name List      | Nodelist, Connectionlist | Fileout  |

<br>

## Output

An XML - File containing the model information for CD3.

## Detailed Input Description


<br>

> ### Supplyvector


The Supplyvector contains the information for all 5 model setup levels.
In its core the Clustervector accesses the Buildinglevel and creates Building Complexes.
The Clustervector contains the following information:

	[[Greywatertank existent (for each Building (0 = No, 1 = Yes)], [contributing to Greywaterreservoir (0 = No, 1 = Yes), using treated Greywater from Reservoir (0 = No, 1 = Yes), using treated Stormwater (0 = No, 1 = Yes)], Number of Clusters ]
    
    For example: 
    
    	Clustervector = [[1,0,1,0,1,0], [0,0,0], 3]
        
        The Cluster contains 6 Buildings (that equals the length of the Clustervector[0]). The first, third and fifth Building do have a Greywatertank connected to them.
        The Cluster is not contributing to a Greywaterreservoir. It's not using Greywater from a Reservoir and it's not 
        connected to a Stormwaterreservoir. There're 3 Clusters of the configuration.
    

# 

Looking at the next higher level, the ReservoirlevelOne - vector needs to have the following setup:

	[[Clustervector_0, Clustervector_1, ..., Clustervector_a, Reservoir_0 existent (0 = No, 1 = Greywaterreservoir, 2 = Stormwaterreservoir)], ..., 
    [Clustervector_b, Clustervector_b+1, ..., Clustervector_c, Reservoir_n existent (0 = No, 1 = Greywaterreservoir, 2 = Stormwaterreservoir)]] 
    
    =
    
    [ReservoirlevelOnearea _0, ..., ReservoirlevelOnearea _n]
    
    
    For example: 
    
    	ReservoirlevelOnevector = 
        [[[[1,0,1,1,1,1],[1,0,0],1], [[1,0,0,0,1,0,1],[1,1,0],1], [[0,1],[0,0,0],5], 1], 
         [[[1,0,1,1],[0,0,0],1], [[1,0,0],[0,0,0],7], [[0,1,0,1,1,0,1,0,1],[0,0,0],1], 0],
         [[[1,0,1,1,0],[0,0,1],10], 2]]
         
        The vector decribes 3 areas. The first one consists out of 3 Clustervectors. The 7 clusters 
        (5 clusters of the third configuration) are connected to a Greywaterreservoir. The first cluster contributes to the 
        Greywaterreservoirs inflow. The second cluster contributes to the inflow and uses the reservoirs outflow. 
        The last 5 clusters are not connected to the reservoir.
        There's no Greywaterreservoir available for the second Area. Hence none of the clusters is contributing or using
        Greywater from a reservoir. 
        While the third Area is consists out of 10 Clusters, which are connected to a Stormwaterreservoir.
        
# 

The next scale of implementation is the ReservoirlevelTwo. Its inputvector has the following structure:

	[[ReservoirlevelOnevector_0, ReservoirlevelOnevector_1, ..., ReservoirlevelOnevector_a, Reservoir_m existent (0 = No, 1 = Greywaterreservoir, 2 = Stormwaterreservoir)], ..., 
    [ReservoirlevelOnevector_b, ReservoirlevelOnevector_b+1, ..., ReservoirlevelOnevector_c, Reservoir_z existent (0 = No, 1 = Greywaterreservoir, 2 = Stormwaterreservoir)]]
    
    =
    
    [ReservoirlevelTwoarea _0, ..., ReservoirlevelTwoarea _n]
    
    
    For example:
    	
        ReservoirlevelTwovector = 
        [[[[[1,0,1,0,0,0],[0,0,1],1],[[1,1,0,1],[1,1,1],1],[[0,1],[1,1,1],1],1], 
          [[[1,0,0],[0,0,1],1],[[0,0,0,0,0],[0,0,1],1],[[0,1],[0,0,1],1],0],2]]
          
        This example vector depicts one area only. The digit one at the vectors last position states that a Stormwaterreservoir
        is in place. It's connected to two ReservoirlevelOne subareas. The Clustervectors contain the "connection 
        information" (see above).
        
        Attention!!
        
        

#  

Finally the Supplyvector can be explained:

	[[ReservoirlevelTwovector_0, ReservoirlevelTwovector_1, ...ReservoirlevelTwovector_a], ...,
     [ReservoirlevelTwovector_0, ReservoirlevelTwovector_1, ...ReservoirlevelTwovector_b]]
     
     =
     
     [Supplyarea_0, ..., Supplyarea_n]
     

	Thus the Supplyvector consits out of a number of different supply districts. One district my be combined out of 
    a number of Stormwatervectors.

        
<br>


> ### Catchment Attributevector

To set the Catchments features this vectors ist used.

	Catchment Attributevector = [Catchmentvector_0, Catchmentvector_1, ..., Catchmentvector_n]
    
    Each Catchmentvector contains the Catchments properties.
    
    Catchmentvector = [Number of Subareas, Decay Constant, Catchment Area, Fraktion of Impervious Area to Reservoir iAR,
    				   Fraktion of Impervious Area to Stormwater Drain iASD, Fraktion of Pervious Area pA,
                       Initial Infiltration Capacity, Final Infiltration Capacity, Depression Loss, Wetting Loss,
                       Outdoor Demand Weighing Factor, Runoff Runtime iAR, Runoff Runtime iASD, Runoff Runtime pA,
                       Weighting Coefficient iAR, Weighting Coefficient iASD, Weighting Coefficient pA, Selected Model]
                       
	Units:
    	
        Number of Subareas = [-]
		Decay Constant = [1/min]
        Catchment Area = [m^2]
        Fraktion of Impervious Area to Reservoir iAR = [-]
        Fraktion of Impervious Area to Stormwater Drain iASD = [-]
        Fraktion of Pervious Area pA = [-]
        Initial Infiltration Capacity = [m/h]
        Final Infiltration Capacity = [m/h]
        Depression Loss = [mm]
        Wetting Loss = [mm]
        Outdoor Demand Weighing Factor = [-]
        Runoff Runtime iAR = [s]
        Runoff Runtime iASD = [s]
        Runoff Runtime pA = [s]
        Weighting Coefficient iAR = [-]
        Weighting Coefficient iASD = [-]
        Weighting Coefficient pA = [-]
		Selected Model = [string]
        
		
For more detailed information regarding the input, output, etc. please check the
[Catchment Description File](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Catchment%20Addon.md)


<br>

> ### Greywater Tank Attributevector

This vector is responsible for setting the Greywatertanks properties.

	Greywatertank Attributevector = [Greywatertankvector_0, Greywatertankvector_1, ..., Greywatertankvector_n]
    
    Greywatertankvector =[Yield of Treatment, Storage Volume]
    
    Units:
    	
        Yield of Treatment = [-]
        Storage Volume = [m^3]

		
For more detailed information regarding the input, output, etc. please check the
[Greywatertank Description File](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Greywater%20Tank%20Addon.md)

<br>


> ### Greywater Reservoir Attributevector

This vector is responsible for setting the Greywaterreservoirs properties.

	Greywaterreservoir Attributevector = [Greywaterreservoirvector_0, Greywaterreservoirvector_1, ..., Greywaterreservoirvector_n]
    
    Greywaterreservoirvector =[Yield of Treatment, Storage Volume]
    
    Units:
    	
        Yield of Treatment = [-]
        Storage Volume = [m^3]

		
For more detailed information regarding the input, output, etc. please check the
[Greywaterreservoir Description File](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Greywater%20Reservoir%20Addon.md)

<br>

> ### Stormwater Reservoir Attributevector

This vector is responsible for setting the Stormwaterreservoirs properties.

	Stormwaterreservoir Attributevector = [Stormwaterreservoirvector_0, Stormwaterreservoirvector_1, ..., Stormwaterreservoirvector_n]
    
    Stormwaterreservoirvector =[Yield of Treatment, Storage Volume]
    
    Units:
    	
        Yield of Treatment = [-]
        Storage Volume = [m^3]

		
For more detailed information regarding the input, output, etc. please check the
[Stormwaterreservoirs Description File](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Stormwater%20Reservoir%20Addon.md)


<br>

> ### Rainwater Tank Attributevector

This vector contains the properties of all raintanks.

	Rainwatertank Attributevector = [Rainwatertankvector_0, Rainwatertankvector_1, ..., Rainwatertankvector_n]
    
    Rainwatertankvector =[Storage Volume]
	
    Units:
    
    	Storage Volume = [m^3]
    
	
For more detailed information regarding the input, output, etc. please check the
[Rainwatertank Description File](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Rainwater%20Tank%20Addon.md)


<br>


> ### Garden Watering Attributevector

This vector contains the properties of all Garden Watering Modules.

	Garden Watering Attributevector = [Gardenwateringvector_0, Gardenwateringvector_1, ..., Gardenwateringvector_n]
    
    Gardenwateringvector =[Average Watering Frequency, Deviation of Frequency, Maximal Watering Flow Rate, Smart Watering Start Time End Time, Watering Method]
	
    Units:
    
    	Average Watering Frequency = [d]
		Deviation of Frequency = [d]
		Maximal Watering Flow Rate = [L/min] 
		Smart Watering Start Time End Time = [hh,hh] 
		Watering Method = [String]
		
	For example:
		
		Garden Watering Attributevector = [[7,2,22,[18,6],"Smart Watering"],[14,5,18,[20,4],"Normal Watering"]]
    
	
For more detailed information regarding the input, output, etc. please check the
[Garden Watering Module Description File](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Garden%20Watering%20Addon.md)


<br>


> ### Building Demand Attributevector

A Buildings Water Demand depends on the number of units it consits out of. Furthermore their size (number of people occupying the unit) and use (residential, commercial). The vector needs to be set up as follows.

	Building Demand Attributevector = [Buildingdemandvector_0, Buildingdemandvector_1, ..., Buildingdemandvector_n]
    
    Buildingdemandvector = [[number of people in residential unit 1, number of people in residential unit 2, ..., number of people in residential unit n],
    						[number of people in commercial unit 1, number of people in commercial unit 2, ..., number of people in commercial unit n], Select Model]
	
    
    length(Buildingdemandvector[0]) = number of residential units in building
    
    length(Buildingdemandvector[1]) = number of commerical units in building
    
    Buildingdemandvector[0][i] = number of people in residential unit i
    
    Buildingdemandvector[1][i] = number of people in commercial unit i
	
	Select Model = [String]
    
    For example:
    
    	Building Demand Attributevector = [[[4,5,2],[2], 'Stochastic Model'], [[6],[0],'Stochastic Model']]
        
        The vector decribes 2 buildings. The first building consists out of 4 units. 3 residential and 1 commercial units.
        The first residential unit has 4 people living in it, the second one has 5 occupants and 2 people live in the 3rd 
        residential unit. 2 people are working in the commercial unit of the first building.
        The second building only has one unit, it's a residential unit with 6 occupants.
    

For more detailed information regarding the input, output, etc. please check the
[Demand Model Description File](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Demand%20Model%20Addon.md)


<br>

> ### Simulationsetup Vector

City Drain 3 need some basic information regarding the Simulation Setup. 

	Simulationsetup Vector = [Start Simulation, End Simulation, Time Step of Simulation, Path of Addons to implement]
    
    Units:
    
	Start Simulation = [YYYY-MMM-DD hh:mm:ss]
	End Simulation = [YYYY-MMM-DD hh:mm:ss]
	Time Step of Simulation = [s]
	Path of Addons to implement = [path of file]
	
    For example:
    
    	Simulationsetup Vector = ["2000-Jan-01 00:00:00", "2001-Jan-01 00:00:00", "360", "C:/Users/Acer/Documents/GitHub/CD3Waterbalance/Module/cd3waterbalancemodules.py"]


<br>

> ### Needtohaveinputs Vector

This vector takes care of the input information for both [File Readers](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/File%20Reader%20Addon.md) and the [Pattern Implementer](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Pattern%20Implementer%20Addon.md). 

	Needtohaveinputs Vector = [Path of Rain File, Path of Evapotranspiration File, average Time of when the Sun is at its Zenith, average Time of Sundown]
    
    Units:
    
	Path of Rain File = [path of file]
	Path of Evapotranspiration File = [path of file]
	average Time of when the Sun is at its Zenith = [h]
	average Time of Sundown = [h]
	
    For example:
    
    	Needtohaveinputs Vector = ["C:/Users/Acer/Documents/GitHub/CD3Waterbalance/simulationwithpatterns/inputfiles/rain.ixx", "C:/Users/Acer/Documents/GitHub/CD3Waterbalance/simulationwithpatterns/inputfiles/evapo.ixx", "13", "20.5"]

		
<br>

> ### Fileout Name List

A City Drain Fileout is a built in block that creates a txt - file during the simulation. This file contains all quantity and quality information for every single time step simulated.
To be able to check any flow in detail, Fileouts can be inserted at any position desired. 
After running the XML - Creator the Model will be set up with the desired Fileouts.

	Fileout Connection Name List = [Fileoutvector_0, Fileoutvector_1, ..., Fileoutvector_n]
    
    Fileoutvector = ["Inport" or "Outport", Name of Model Block, Name of Port, Name of txt File]
    
    For example:
    
    	Fileoutvector = [["Inport", "Raintank_0", "Collected_Water", "RaintankInflow.txt"],["Outport", "Raintank_0", "Current_Volume", "StorageCurve.txt"]]
 

<br>


## The 5 Levels of Implementation in Detail

> ### Building Level

# 

This level represents the smallest scale of implementation. It consist out of 5 to 6 blocks, a Demandmodel, a Garden Watering model, a Building, a Catchment, a Raintank and optional a Greywatertank, as illustrated in the shematic below (Figure 1). The Demandmodel creates the indoor demand of the Building block its connected to. The Catchment produces the Buildings outdoor demand, which is proportional to the water loss of the pervious area due to evapotranspiration. Depending on the water stream potable or nonpotable water will be used to meet the demand. Similarly the produced wastewater is differenciated between blackwater and greywater. The Raintanks inflowing water is being produced by the Catchment and the outflowing water equals the Buildings demand of nonpotable water. If there's not enough water in the Raintank to meet the Buildings demand the signal is passed on to the Greywatertank if present, alternativly it's passed on to a higher scale.

If a Building without Raintank is supposed to simulated simply set the tanks volume to zero.

# 
Figrue 1 : Building Complex Setup
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Level%20shematics/Buildinglevel.png)


<br>

> ### Cluster Level

# 

A number of Building Complexes (Figure 1) represent a Cluster. Next to the Building Complexes a street will be added. It's represented by a Catchment. Furthermore four Collector blocks are added, enabling the user to check runoff, greywater produced, raintank overflow and the additional demand of nonpotable water on a clusterlevel. This allows direct comparisons of clusters and show potential for optimization. 

# 
Figure 2: Cluster Setup
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Level%20shematics/Clusterlevel.png)

<br>

> ### ReservoirlevelOne

# 

Any number of Clusters desired make up one ReservoirlevelOne. A Stormwaterreservoir or Greywaterreservoir can be added. If implemented the level will add one Collector for adding inflow and one Collector for adding demand of nonpotable water (outflow) to the choosen reservoir. The Collectors summarize all inflow and outflow signals from the lower level. Figure 3 illustrates the system created by the XML Creator.

# 
Figure 3: ReservoirlevelOne Complex
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Level%20shematics/ReservoirlevelOne.png)

<br>

> ### ReservoirlevelTwo

# 

Alike the before described scale of implementation (Figure 3) the ReservoirlevelTwos reservoirs are a optional feature (Figure 4). It consists out of ReservoirlevelOne vectors. If the choosen to include, 2 Collectors will be added additionally. Analogous to the before described level the first Collector will reach down into the Clusterlevel and summarize all runoff and Raintank overflow as input into the Stormwaterreservoir or collect all Greywater (produced by buildings and overflowing from Greywatertanks) as inflow for a Greywaterreservoir. Whilst the other Collector collects all additional demand from either the Clusterlevels or Reservoirs from the Res.levelOne.

# 

Figure 4: ReservoirlevelTwo Complex
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Level%20shematics/ReservoirlevelTwo.png)

<br>

Attention:

If two Stormwaterreservoirs or two Greywaterreservoirs are being used in the same flow path. The Inflow to the second Reservoir is being used from the Clusters not connected yet! 

For Example:
		
        Vec = [[[[1],[0,0,1],1],0],[[[0],[0,0,1],1],2],2]
        
        The first ReservoirlevelOne vector is connected to a Stormwaterreservoir as the Clustervector states. 
        Still there's no Reservoir in the first Res.levelOne.The second Res.levelOnevector also states that
        its only cluster is connected to a Stormwaterreservoir and a Reservoir is in place. 
        That means the ReservoirlevelTwos Stormwaterreservoir will get its inflow from the the first Res.levelOne vector 
        (because it is not connected yet). It's inflow will be the Clusters Runoff and Raintankoverflow.
        It's outflow is the additional demand from that cluster as well as the additional demand from the Stormwaterreservoir
        from the second Res.levelTwo.

# 

> ### Supply Level

# 

Containing numerous ReservoirlevelTwo Complexes (Figure 4) this level is the topmost scale. The Supply Level is able to set up more than one Supply Complex. To each Complex 4 Collectors, a Potable Water Reservoir, a Stormwaterdrain and a Sewer will be added. Figure 5 shows each Collectors function.

# 
Figure 5: Supply Complex
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Level%20shematics/Supplylevel_1.png)


<br>

The rain and evapotranspiration files are being distributed to all Catchments, no distinction between the different Supply Complexes is being made. Before being passed on to the Catchments the evapotranspiration data is modified by the Pattern Implementer to depict a more realistic evapotranspiration curve. 
For checking the models accuracy still unconnected model block outports will be summarized b Collectors. Figure 6 outlines the number of Collectors added and their role in detail.


# 
Figure 6: Rain and Evapotranspiration Input Setup and Stream Collection
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Level%20shematics/Supplylevel_2.png)
 
<br>

Finally the necessary Fileouts are added to the model (Figure 7).

# 

Figure 7: Adding Fileouts
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Level%20shematics/Supplylevel_3.png)



