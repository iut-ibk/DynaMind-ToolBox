# To XML Creator modified Simulator

Basic features:

 - routine to run CityDrain3 from Python
 - automated readout of model results and accuracy evaluation
 - integrated result plotter
 
<br>

### Functions and Input


| Function | Input  |  Type  | Example | 
| :------------: | :------------: |:---------------:|:---------------:|  	
|  _Deleter_  |  f(Path of Directory to model xml-file)  |  String  | f('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\output') |
|  _RunCD3_  |  f(Path of Directory to cd3.exe, Path of Directory to model xml-file)  |  Strings  | f('C:\Program Files (x86)\CityDrain3\bin\cd3.exe', 'C:\Users\Acer\Documents\GitHub\CD3Waterbalance\output\model.xml') |
| _Fractioncalculator_ | f([Catchment Attributevector](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Model%20Set%20Up/XML%20-%20Creator.md))  |  List  | f([[1,1.9,800,0.4,0.2,0.4,0.6,0.21,1.5,0.4,0.5,400,500,700,0.04,0.05,0.06],[1,1.8,10000,0,1,0,0.6,0.21,1.5,0.4,0.5,380,510,710,0.04,0.05,0.06]]) |
|  _GetOutputData_  |  f(Path of Directory to model xml-file)  |  String  |  f('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\output')  | 
|  _GetInputData_  |  f(Path of Directory to model input)  |  String  |  f('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\input') |
|  _Balance_  |  f(list of all storage output file names, list of filereader/pattern implemeter output file names, list of other neccessary output file names)  |  Lists  |  f(['Greywatertanklevels',  'Rainwatertanklevels', 'Stormwaterreservoirlevels'], ['Evapo_Model', 'Rain_Model'], ['Actual_Infiltration', 'Potable_Water_Demand', 'Sewer', 'Stormwaterdrain']) |
| _Plotter_ | f(limits for x-axis, limits of y-axis, list of files to plot)  |  Lists  | f([0,50], [0,10], ['Rain_Model', 'Stormwaterdrain', 'Evapo_Model', 'effective_rain','Indoor_Demand','Outdoor_Demand'])| 

<br>



### Results

| Function | Result |  
| :------------: | :------------: |
|  _Deleter_  |  deletes all txt-files (output files) of prior simulations |  
|  _RunCD3_  |  runs the CD3 program with selected model file  |  
| _Fractioncalculator_ |  prepares losses, total area and the average fractions iAR, iASD and pA   |  
|  _GetOutputData_  |  produces a list with the data of all output files  |  
|  _GetInputData_  |  produces a list with the data of all input files  |  
|  _Balance_  |  calculates a mass balance from input data and output data and displays results  | 
| _Plotter_ | plots graph of data wanted  |  

## More Information


<br>




        
