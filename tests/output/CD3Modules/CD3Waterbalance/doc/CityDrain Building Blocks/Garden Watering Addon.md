# Garden Watering Addon

Basic features:

 - collects the by the [Catchment Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Catchment%20Addon.md) generated outdoor demands and creates events where the water is being used
 - different watering behaviours can be selected
 - a maximal flow rate for the watering events can be specified
 - creates a more realistic watering behaviour by using a normal distribution 
 
<br>

## Parameters 

| Input  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|	
| Outdoor Demand In     | Time Series | [m³/dt] |
| 	Watering Method | String  |   'Normal Watering' _or_ 'Smart Watering' |
| Smart Watering Time     | Vector(String) | [hh,hh] |
| 	Maximal Watering Flow Rate | Constant  |   [L/min] |
| Average Watering Frequency      | Constant | [d] |
| Deviation of Frequency      | Constant | [d] |

# 

|Output  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
|    Outdoor Demand Out  | Time Series |  [m³/dt]
|    Check Storage  |    Time Series     |  [m³/dt]  |




<br>

## Description 

This City Drain module acts like a storage unit, only emptying (creating a watering event) every ... days. The Average Watering Frequency and the Deviation of Frequency inputs specify a normal distribution. This distribution generates a 'pseudo random' frequency when the module is being set-up in City Drain. Thus different Garden Watering Blocks will create different watering frequencies resulting in a normal distributed watering behaviour for suburbs or cities. The frequency is only choosen once, so that every module has a different frequency but the frequency of a certain module stays constant.

A watering events flow rate is limited by the flow rate of the tap used. The input value Maximal Watering Flow Rate takes this limiting factor into account. Furthermore 2 possible watering behaviours were considered. The 'Normal Watering' mode creates events for multiples of the sampled watering frequency, regardless whether the event will take place during the day when the sun is high or any other time. The 'Smart Watering' mode creates events for multiples of the sampled watering frequency too, but forced the start of watering to be inbetween the given Smart Watering Time (eg. [18,4]).

Attention: The Smart Watering Time must start before 12 am and end the next morning. 12 am equals 24 hours not! 0 hours.

<br>

## Recommended Improvements

- imporve the smart watering time input capabilities
