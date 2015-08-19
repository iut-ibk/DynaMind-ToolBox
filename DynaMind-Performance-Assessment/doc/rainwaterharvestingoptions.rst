==========================
RainWaterHarvestingOptions
==========================

Assesses the potential of water savings for different sized rain water harvesting tanks. For the assessment the module uses
CityDrain3 to simulate the storage behaviour of the tank using a daily time step. As in-flow into the tank the *run_off_roof_daily*
is required. As demand the sum *potable_demand_daily* and *outdoor_demand_daily* is applied. For the calculation of
*non_potable_savings* and *outdoor_water_savings* the in-door use is prioritized.

Parameter
---------

+-------------------+------------------------+-------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                 |
+===================+========================+===================================================================+
|storage_volume     | STRING_LIST            | Storage volumes as string list e.g. [2,5,10]                      |
+-------------------+------------------------+-------------------------------------------------------------------+
|start_date         | STRING                 | start date of simulation as string 2000-Jan-01 00:00:00           |
+-------------------+------------------------+-------------------------------------------------------------------+
|end_date           | STRING                 | end date of simulation as string 2001-Jan-01 00:00:00             |
+-------------------+------------------------+-------------------------------------------------------------------+
|time set           | STRING                 | time step in seconds                                              |
+-------------------+------------------------+-------------------------------------------------------------------+

Data-stream
-----------

+-------------------+--------------------------+------------------+-------+--------------------------------------+
|        View       |          Attribute       |       Type       |Access |    Description                       |
+===================+==========================+==================+=======+======================================+
|**parcel**         |                          | COMPONENT        | read  | parcel data                          |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | non_potable_demand_daily |    DOUBLEVECTOR  | read  | non potable water demand (m3/day)    |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | potable_demand_daily     |    DOUBLEVECTOR  | read  | potable water demand (m3/day)        |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | outdoor_demand_daily     |    DOUBLEVECTOR  | read  | out door water demand (m3/day)       |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | run_off_roof_daily       |    DOUBLEVECTOR  | read  | roof runoff (m3/day)                 |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   |                          |                  |       |                                      |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|**rwht**           |                          | COMPONENT        | write | rain water harvesting tank           |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | parcel_id                |    INT           | write | link to parcel                       |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | volume                   |    DOUBLE        | write | tank volume (m3)                     |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | storage_behaviour_daily  |    DOUBLEVECTOR  | write | water in tank (m3/day)               |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | provided_volume_daily    |    DOUBLEVECTOR  | write | provided water (m3/day)              |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | storage_behaviour_daily  |    DOUBLEVECTOR  | write | water in tank (m3/day)               |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | non_potable_savings      |    DOUBLE        | write | annual non potable water savings (m3)|
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | outdoor_water_savings    |    DOUBLE        | write | annual outdoor water savings (m3)    |
+-------------------+--------------------------+------------------+-------+--------------------------------------+
|                   | annual_water_savings     |    DOUBLE        | write | annual total water savings  (m3)     |
+-------------------+--------------------------+------------------+-------+--------------------------------------+