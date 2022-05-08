# Catchment Addon

Basic features:

 - simulates the interaction between Rain, Evapotranspiration and pervious, as well as impervious area
 - produces dynamic flow using the _Muskingum Method_
 - differentiation between collected run-off and rainwater adding to the storm water drainage system can be made
 - infiltration rates estimated via _Horton Equation_
 - Catchment can be divided in Subareas, thus simulating a row of connected Catchments
 - generates the Outdoor Water Demand of connected [Building Block](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Building%20Addon.md)
 - additional Inflows can be implemented
 - optional simplified model approach for large simulation time steps
 
<br>

## Parameters 
### Input


| Flow  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|	
| Rain      | Time Series | [mm/dt] |
| 	Evapotranspiration | Time Series  |   [mm/dt] |
| Inflow | Time Series         |    [m³/dt] |

# 

|Catchment Attributes  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
|    Catchment Area   | Constant |  [m^2]
| Fraction of Pervious Area (pA) |   Constant  |  [-]   |
|   Fraction of Impervious Area to Reservoir (iAR)| Constant |  [-]
|    Fraction of Impervious Area to Stormwater Drain (iASD)  |    Constant     |  [-]  |
| Number of Subareas |   Constant  |  [-]   |
| Wetting Loss |   Constant  |  [mm]   |
| Depression Loss |   Constant  |  [mm]   |
| Outdoor Demand Weighing Factor |   Constant  |  [-]   |
| Catchment with or without Routing |   String  |  'with' _or_ 'without'   |

# 

|Horton Method  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
|    Initial Infiltration Capacity (Horton)   | Constant |  [m/h]
|    Final Infiltration Capacity (Horton)   |    Constant     |  [m/h]  |
| Decay Constant (Horton) |   Constant  |  [1/min]   |

# 

|Muskingum Method  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
| Runoff Runtime (pA)  |   Constant  |  [s]   |
| Weighting Coefficient (pA) |   Constant  |  [-]   |
| Runoff Runtime  (iAR) |   Constant  |  [s]   |
| Weighting Coefficient (iAR)|   Constant  |  [-]   |
| Runoff Runtime  (iASD) |   Constant  |  [s]   |
| Weighting Coefficient (iASD)|   Constant  |  [-]   |

### Output 

|Flow  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
| Possible Infiltration  |   Time Series  |  [m³/dt]   |
| Actual Infiltration |   Time Series  |  [m³/dt]  |
| Runoff |   Time Series  |  [m³/dt]  |
| Collected Water|  Time Series  | [m³/dt]   |
| Outdoor Demand |  Time Series |  [m³/dt]  |
| Outdoor Demand Check |  Time Series  |  [m³/dt]  |

<br>

## Description 


The Block is solving the general water balance equation for each time step. 

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/water%20balance%20equation.png?raw=true)

Runoff _R_ is being produced by rain and other inflows e.g. from another catchment. It can either be discharged into the stormwater drainage system or partly be collected in a reservoir. The quantities of water being diverted to either system can be regulated with the input parameters `Fraction of Impervious Area to Reservoir` and `Fraction of Impervious Area to Stormwater Drain`. Runoff contributing to discharged stormwater can also be produced on the pervious area, if the rain intensity is higher than the infiltration rate.

Long and short term storage _S_ has been taken under consideration. Long term storage is represented by soil infiltration and initial losses. Infiltration has been implemented using the _Horton Method_. Initial losses from floor depressions and wetting are to be set as constants (`Wetting Loss`, `Depression Loss`). For flow routing, representing short term storage, the _Muskingum Method_ was used. 

The outdoor water demand of a building connected to a catchment is being considered originating mainly from garden watering. Therefore it's estimated via the relation between evapotranspiration _ET_ and rain _P_. If precipitation is higher than or equal to evapotranspiration there's no demand. For evaportranspiration being greater, the outdoor water demand is equal to their difference.

The discharged water volumes are dependant on the catchment area. For the sake of simplicity catchments are considered squares. Their area need to be specified (`Catchment Area`). 

Depending on the chosen catchment type (with or without routing) the Muskingum Method will be used or not.

<br>

> ### Muskingum Method

Runoff concentration is being simulated by the [Muskingum Method](http://ponce.sdsu.edu/eonvideo/enghydro091_raw.html). The approach used is simplified. A detailed description of the simplification can be found [here](http://www.uibk.ac.at/umwelttechnik/teaching/phd/diss_achleitner.pdf). Flow routing is peformed for all 3 types of runoff:

-	runoff of pervious areas
-	runoff of impervious area flowing to the stormwater drain
-	runoff of impervious area flowing to the reservoir

Due to the different retention properties of those areas three flow routing algorithms have been constructed, resulting in 6 necessary parameters instead of two.

Used formulas:

>	![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/discrete%20Muskingum%20equation%201.png?raw=true)
 
>	![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/discrete%20Muskingum%20equation%202.png?raw=true)
 
>	![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/discrete%20Muskingum%20coefficient%20equation%201.png?raw=true)
 
>	![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/discrete%20Muskingum%20coefficient%20equation%202.png?raw=true) 
 
>	![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/Muskingum%20parameter%20equation.png?raw=true) 
# 

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/time%20index.png?raw=true) ... time index [-]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/space%20index.png?raw=true) ... space index [-]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/discharge.png?raw=true) ... discharge [L³/T]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/stored%20volume.png?raw=true) ... stored volume [L³]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/runoff%20velocity.png?raw=true) ... runoff velocity [L/T]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/Muskingum%20coefficient%201.png?raw=true) ... muskingum coefficient [-]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/Muskingum%20coefficient%202.png?raw=true) ... muskingum coefficient [1/T]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/Muskingum%20parameter%201.png?raw=true) ... muskingum Parameter (representing delay time) [T]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/Muskingum%20parameter%202.png?raw=true) ... muskingum Parameter (representing held back water) [-]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/length.png?raw=true) ... length the water travels [L]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/Number%20of%20subareas.png?raw=true) ... number of subareas [-]

<br>

> ### Horton Method

For estimating the soils infiltration capacity the [Horton Method](http://www.trentu.ca/iws/documents/WBA22_app-f.pdf) was used. Thereby the actual infiltration rate is limited by the possible infiltration rate. If precipitation is greater than evapotranspiration it is decreasing continuously. Whereas it is increasing when _ET_ is greater. The process of drying is simulated by the reversed Horton method.

When there's a switch of drying and wetting state the parameter ![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/possible%20infiltration%20rate%20for%20certain%20time.png?raw=true) allows for a smooth transition. The parameter takes the value of the possible infiltration calculated in the previous time step (![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/switcht%20dry%20and%20wet.png?raw=true)) and stays constant until the next switch.

Used formulas:

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/Horton%20equation.png?raw=true)

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/f_t%20start.png?raw=true) 

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/range%20f_t.png?raw=true) 
# 

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/time%20index.png?raw=true)  ... time index [-]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/possible%20infiltration%20rate.png?raw=true) ... possible infiltrtation rate [L/T]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/initial%20infiltration%20rate.png?raw=true) ... initial infiltration rate [L/T]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/final%20infiltration%20rate.png?raw=true) ... final infiltration rate [L/T]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/possible%20infiltration%20rate%20for%20certain%20time.png?raw=true) ... possible infiltration rate for certain time [L/T]

![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/decay%20constant.png?raw=true) ... decay constant [1/T]



<br>

## Recommended Improvements

 - implement alternative routing method like linear storage.