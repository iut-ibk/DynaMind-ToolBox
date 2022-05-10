

# Stormwaterreservoir Addon

Basic features:

 - simulates simple storage unit for Stormwater
 - the treatments yield and storage size can be specified
 
<br>

## Parameters 



| Input  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|	
| Stormwater In      | Time Series | _any_ |
| 	Stormwater Out | Time Series  |   _any_ |
| Yield of Treatment      | Constant | [-] |
| 	Storage Volume | Constant  |   [m^3] |


# 

|Output  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
|    Current Volume  | Time Series |  _same as input_ |
|    Additional Demand |    Time Series     |  _same as input_  |
|    Wastewater   | Time Series |  _same as input_ |
|    Stormwater Overflow    |    Time Series     |  _same as input_  |



<br>

## Description 

This module collects Stormwater. The stored volume depends on the yield of treatment specified. 1 - yield of treatment gives the part of Stormwater that is equivalent to the Wastewater produced by the treatment process. If the reservoirs specified volume is exceeded inflowing water is given to the Stormwater Overflow, thereby being added to the Stormwaterdrain.


<br>



