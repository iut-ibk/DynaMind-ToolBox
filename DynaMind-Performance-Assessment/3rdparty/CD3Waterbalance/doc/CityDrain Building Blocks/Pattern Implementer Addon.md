# Pattern Implementer Addon

Basic features:

 - uses Normal Distribution to create a more realistic Evapotranspiration Pattern from daily values 

 
<br>

## Parameters 


| Input  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|	
| Inport      | Time Series | _any_|
| 	Sun Zenith | Constant  |   [0-23.99 h] |
| Sundown | Constant   |    [0-23.99 h] |

# 

|Output  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
|  Outport  | Time Series |  _same as input_  |



<br>

## Description 

For producing a more realistic evapotranspiration pattern out of the daily constant values from weather data the so called Pattern Implementer uses a normal distribution. 
Using the time of the suns zenith as expectation value and the time of the sundown as (expectation value + 3 * standard deviation) a discrete normal distribution is being calculated.
With the average of all distribution values and the initial distribution factors for each time step of the simulation are being created. Those factors are being multiplied with the constant values
given by the File Reader (the evapotranspiration file). The result is evapotranspiration curve shaped like the normal distribution.



<br>

