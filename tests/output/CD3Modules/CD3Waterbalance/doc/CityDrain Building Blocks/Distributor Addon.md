# Distributor Addon

Basic features:

 - takes one signal in and multiplies it as many times as wanted
 
<br>

## Parameters 



| Input  | Type  |  Unit  |
| :------------: |:---------------:| :-----:|	
| Number of Outports (n)     | Constant | [-] |
| Flow      | Time Series | _any_ |


# 

|Output  | Type  |  Unit  |
| :------------: |:---------------:| :-----:|
|    Flow 1   |    Time Series     |  _same as iput_  |
|    Flow 2  |    Time Series     |  _same as iput_  |
|    ...  |    ...     |  ...  |
|    Flow n  |    Time Series     |  _same as iput_  |


<br>

## Description 

The Block creates the desired number of outports. The Inflow is then set to all the Outflows.
Its not being split up. 100 % of it goes to every outport.

Outflow_1 (dt) = Inflow (dt)

Outflow_2 (dt) = Inflow (dt)

...
 
Outflow_n (dt) = Inflow (dt)

<br>

##Recommended Improvements
