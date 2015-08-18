# File Reader Addon

Basic features:

- imports time series with certain time step
- adjusts time step of file to simulation time step automatically
- gives out needed times with corresponding value
 
<br>

## Parameters 

| Input  | Type  |  Unit  |
| :------------ |:---------------:| :-----:|	
| File      | Time Series (File) | [DD.MM.YYYY HH:MM:SS value(see below)] |
| 	Filecontent | Letter  |  "H" for Height [ _any_ ] or "F" for Flow [ _any_ ] |

# 

|Output   | Type  |  Unit  |
| :------------ |:---------------:| :-----:|
|  Inputdata with adjusted time step  |   Time Series  |  _same as input_  |

<br>

## Description 

This file reader is able to adjust the files time step automatically to any needed for the simulation. Just select a file and set the models simulation time step as desired. The files time step length ought to be constant throughout the entire time span of simulation. By reference to the file, the set time step can either be larger, smaller or the same. Different time scales result in the necessity to calculate the corresponding values for each new point in time. The algorithms needed for the calculation of time related values (flow) differ from those needed for over time summed up values (height).

<br>

### Flow

Linear interpolation was used to calculate the new values.

# 

Used formulas:

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_lin_interpolation_intervall.png?raw=true) 

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_lin_interpolation.png?raw=true) 

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_i.png?raw=true) ... time index [-] 

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_j.png?raw=true) ... time index [-] 

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_tf.png?raw=true) ... time from file [DD.MM.YYYY HH:MM:SS]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_ts.png?raw=true) ... time from simulation [DD.MM.YYYY HH:MM:SS]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_v_old.png?raw=true) ... value from file [l/h]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_v_new.png?raw=true) ... value for simulation [l/h] 

<br>

### Height

For the simulation time step being larger than the files, the heights between 2 time steps are being summed up. The other way around the height will be divided by a factor consisting of the time steps relation to one another.

# 

Used formulas for 
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_ts_gr_tf.png?raw=true):

# 

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_sum_ts_j-1_intevall_right.png?raw=true)

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_sum_ts_j_intevall_right.png?raw=true)

>![alt text](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Formulas/file_reader_sum_pj_decimals.png?raw=true)

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_sum_v_new.png?raw=true)

>![alt text](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Formulas/file_reader_i.png?raw=true) ... time index [-]

>![alt text](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Formulas/file_reader_j.png?raw=true) ... time index [-]

>![alt text](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Formulas/file_reader_sum_a.png?raw=true) ... sum index [-]
 
>![alt text](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Formulas/file_reader_sum_n.png?raw=true) ... time index dependent on time step ratio [-]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_tf.png?raw=true) ... time from file [DD.MM.YYYY HH:MM:SS]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_ts.png?raw=true) ... time from file [DD.MM.YYYY HH:MM:SS]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_v_old.png?raw=true) ... value from file [mm]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_v_new.png?raw=true) ... value for simulation [mm]

>![alt text](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Formulas/file_reader_sum_decimals.png?raw=true) ... get decimal places of x [-]

# 

Used formulas for 
![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_ts_kl_tf.png?raw=true):

# 

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_fraction_intervall_right.png?raw=true)

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_fraction_v_new.png?raw=true)

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_i.png?raw=true) ... time index [-]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_j.png?raw=true) ... time index [-]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_sum_n.png?raw=true) ... time index dependent on time step ratio [-]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_tf.png?raw=true) ... time from file [DD.MM.YYYY HH:MM:SS]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_ts.png?raw=true) ... time from file [DD.MM.YYYY HH:MM:SS]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_v_old.png?raw=true) ... value from file [mm]

>![alt text](https://raw.githubusercontent.com/ChristianF88/CD3Waterbalance/master/doc/Formulas/file_reader_v_new.png?raw=true) ... value for simulation [mm]

<br>

## Recommended Improvements

-	improve the flow calculations for large timesteps
