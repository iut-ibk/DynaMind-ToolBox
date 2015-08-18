# User Guide

## Modules

This text is supposed to give a short introduction for working with City Drain 3 using especially for the DAnCE4Water designed Python Addons. A detailed description to the City Drain software can be found [here](http://www.uibk.ac.at/umwelttechnik/teaching/phd/diss_achleitner.pdf).

To be able to simulate urban water cycles in City Drain a few additional modules were programmed.
The folder _City Drain Building Blocks_  contains all addons programmed. Depending on how many modules are being used a the complexity of the built water system reaches from simple to very complex. The following table shows the programmed modules and their function. By clicking on a modules name a more detailed description will be shown.

|Module|Function|
|:----:|:-----:|
|[Building Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Building%20Addon.md)| splits up water demand in potable and nonpotable demand and the produced water in greywater and blackwater |
|[Catchment Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Catchment%20Addon.md)| generates runoff form impervious area, infiltration on pervious area, outdoor water demand |
|[Collector Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Collector%20Addon.md)| adds up a number of streams to one stream (pipe function) |
|[Demand Model Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Demand%20Model%20Addon.md)| generates indoor water demand for buildings |
|[Distributor Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Distributor%20Addon.md)| splits up one signal into many signals |
|[File Reader Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/File%20Reader%20Addon.md)| reads data out of a file (ixx, csv or txt) and passes it on |
|[Garden Watering Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Garden%20Watering%20Addon.md)| generates garden watering events |
|[Greywater Reservoir Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Greywater%20Reservoir%20Addon.md)| stores greywater for nonpotable demand |
|[Greywater Tank Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Greywater%20Tank%20Addon.md)| stores greywater for nonpotable demand |
|[Pattern Implementer Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Pattern%20Implementer%20Addon.md)| creates a evapotranspiration pattern from constant data |
| [Rainwater Tank Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Rainwater%20Tank%20Addon.md)| stores rainwater for nonpotable demand |
| [Sewer, Stromdrain, Potable Water Reservoir Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Sewer%2C%20Stormdrain%20and%20Potable%20Water%20Reservoir%20Addon.md.md)| end of the line modules that state where water ends up |
|[Stormwater Reservoir Addon](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/CityDrain%20Building%20Blocks/Stormwaterreservoir%20Addon.md)| stores stormwater for nonpotable demand |

<br> 

The Blocks are designed to be connected in a certain way. The inports and outports names should give sufficient information for connecting the blocks correctly. Due to City Drains Top Down modelling approach a few limitations must be considered. The signal can only be passed on. The modules are not meant to circle a signal between 2 blocks within one time step. 
Thus a Rainwater Tank connected to a building, will always get the signal to supply the building with water, even when it is empty. (The building doesn't know that's the Tank is empty.) In case the Tank is empty, the signal will be passed on to another Tank or the Potable Water Reservoir. All storage modules have the same outport name for this scenario. It's called _Additional Demand_. 

Another City Drain Block that is being used is the so called FileOut. It's a standard block and enable allows to export data from the model platform to a text document. This document contains the flow quantity and quality information for every simulation time step.



## Set up a Model

To be able to set up a large system a number of python scripts were created. The so called [XML-Creator](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Model%20Set%20Up/XML%20-%20Creator.md) will generate a xml-file that contains all necessary information for an City Drain Model. 
The general set-up of an City Drain model file is the following:

The top contains the information about the general setting like the implemented modules, the simulations time step, start date and end date of simulation. 
The next segment, the so called Node List, contains all the used modules including their attributes (like the area for a catchment).
The last section shows all connections the model has. A connection equals the way a signal travels. The outport of one block will be connected with an inport of another block.

The XML-Creator routine creates a file that has this exact shape. 


## Run a Model

Due to City Drains unstable GUI the program will be executed with python. The python script [To XML Creator modified Simulator](https://github.com/ChristianF88/CD3Waterbalance/blob/master/doc/Model%20Set%20Up/To_XML_Creator_modified_Simulator.md) runs City Drain with a specified xml-file.
Furthermore it's able to create a simple water balance for checking the models accuracy as well as plotting any given txt-file. The Balance and Plotting function can only be used if the necessary FileOuts were created and the functions  are set correctly.


## Interface Script

The [Interface File]() unites the XML-Creator and the Simulator. In this file all necessary adjustments can be made and model inputs can be set. 



































