# Building Addon

Basic features:

 - simulates a Buildings water demand 
 - differs between black water and grey water as well as potable and non potable water
 
<br>

## Parameters 



| Input  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|	
| Outdoor Demand      | Time Series | [m³/dt] |
| 	Bathtub | Time Series  |   [m³/dt] |
| Shower      | Time Series | [m³/dt] |
| 	Toilet | Time Series  |   [m³/dt] |
| Kitchen Tap      | Time Series | [m³/dt] |
| Handbasin Tap      | Time Series | [m³/dt] |
| 	Washing Machine | Time Series  |   [m³/dt] |
| 	Dishwasher  | Time Series  |   [m³/dt] |
| 	Evapcooler  | Time Series  |   [m³/dt] |

# 

|Output  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
|    Potable Water  | Time Series |  [m³/dt]
|    Non Potable Water |    Time Series     |  [m³/dt]  |
|    Black Water   | Time Series |  [m³/dt]
|    Grey Water    |    Time Series     |  [m³/dt]  |



<br>

## Description 

This Block uses the from the [Demand Model Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Demand%20Model%20Addon.md) generated indoor demands and the outdoor demand given by the [Catchment Block](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Catchment%20Addon.md) and [Garden Watering Block](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Garden%20Watering%20Addon.md) to estimate the Water needed by the Building as well as the waste water produced. 

The individual stream are calculated by simple balances: 

|Output Parameter  | Calculation  | 
| :------------ |:---------------:| 
|    Potable Water    | Bathtub + Shower + Kitchen Tap + Handbasin Tap + Washing Machine + Dishwasher + Evapcooler | 
|    Non Potable Water   |    Toilet + Outdoor Demand     | 
|    Black Water   | Toilet + Kitchen Tap |  
|    Grey Water    |    Bathtub + Handbasin Tap + Shower + Tap + Washing Machine + Dishwasher + Evapcooler    | 


<br>

## Recommended Improvements

- enable water use selection for evap cooler as well as a factor that rules the input-output-ratio
- take into account additional water losses for drinking, sweating, food preperation,...