==============
GDALHotStarter
==============

The GDALHoststarter is used to hot start a simulation with a previous database. This is particularly useful for
 bigger simulation where the data preparation and linking of data takes a significant amount of time, but once established,
 can be used as basis for many simulations.

Parameter
---------

+-------------------+------------------------+-------------------------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                                   |
+===================+========================+=====================================================================================+
|hot_start_database | STRING                 | location of the hot start database file (usually a file from a previous simulation) |                            |
+-------------------+------------------------+-------------------------------------------------------------------------------------+


Detailed Description
--------------------

The simulation database from a previous simulation can be used to "hot start" a new simulation. This GDALHostStarter may
be the first module in a simulation since it replaces the current simulation file with the *hot_start_database* defined in the
parameter settings. The file is copied and therefore the original file stays untouched.




