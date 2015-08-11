# Greywater Tank Addon

Basic features:

 - simulates simple storage unit for Greywater
 - the treatments yield and storage size can be specified
 
<br>

## Parameters 



| Input  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|	
| Greywater In      | Time Series | _any_ |
| 	Greywater Out | Time Series  |   _any_ |
| Yield of Treatment      | Constant | [-] |
| 	Storage Volume | Constant  |   [m^3] |


# 

|Output  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
|    Current Volume  | Time Series |  _same as input_ |
|    Additional Demand |    Time Series     |  _same as input_  |
|    Wastewater   | Time Series |  _same as input_ |
|    Greywater Overflow    |    Time Series     |  _same as input_  |



<br>

## Description 

This module collects Greywater. The stored volume depends on the yield of treatment specified. (1 - yield of treatment) gives the part of Greywater that is equivalent to the Wastewater produced by the treatment process. 
If the tanks/reservoirs specified volume is exceeded inflowing water is given to the Greywater Overflow, thereby being added to the Sewer.


<br>



