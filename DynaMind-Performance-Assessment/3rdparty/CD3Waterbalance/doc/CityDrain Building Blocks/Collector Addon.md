# Collector Addon

Basic features:

 - takes desired number of signals in and adds them up to one single signal
 
<br>

## Parameters 


|Input  | Type  |  Unit  |
| :------------: |:---------------:| :-----:|
| Number of Inports (n)     | Constant | [-] |
|    Flow 1   |    Time Series     |  _any_  |
|    Flow 2  |    Time Series     |  _any_ |
|    ...  |    ...     |  ...  |
|    Flow n  |    Time Series     |  _any_  |

# 

| Output  | Type  |  Unit  |
| :------------: |:---------------:| :-----:|	
| Flow      | Time Series | _same as input_ |



<br>

## Description 

The Block creates the desired number of Inports. All the Inflows are being added up and the sum is being given to the outport.
The given Outflow for each time step is being calculated as follows:

Outflow (dt) = Inflow_1 (dt) + Inflow_2 (dt) + ... + Inflow_n (dt)


<br>

##Recommended Improvements

 - optional: reprogram block with faster algorithm

