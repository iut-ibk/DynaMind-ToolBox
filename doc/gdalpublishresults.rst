===============
Export GIS data
===============

:index:`GDALPublishResults <GDALModules; GDALPublishResults | Export GIS data>` export GIS data from DynaMind to common GIS formats `GDAL/OGR <http://www.gdal.org/ogr_formats.html>`_.

Parameter
---------

+-------------------+------------------------+-----------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                     |
+===================+========================+=======================================================================+
|sink               | STRING                 | points to where the view should be exported                           |
+-------------------+------------------------+-----------------------------------------------------------------------+
|driver_name        | STRING                 | name of used driver                                                   |
+-------------------+------------------------+-----------------------------------------------------------------------+
|view_name          | STRING                 | name of view  to be exported                                          |
+-------------------+------------------------+-----------------------------------------------------------------------+
|layer_name         | STRING                 | layer_name in sink                                                    |
+-------------------+------------------------+-----------------------------------------------------------------------+
|steps              | INT                    | is used in loop data are exported every x step                        |
+-------------------+------------------------+-----------------------------------------------------------------------+
|targetEPSG         | INT                    | target EPSG code                                                      |
+-------------------+------------------------+-----------------------------------------------------------------------+
|sourceEPSG         | INT                    | EPSG code used in the simulation                                      |
+-------------------+------------------------+-----------------------------------------------------------------------+


Detailed Description
--------------------

The module builds on the GDAL/OGR library and supports export of data from DynaMind into
common GIS data formats. An overview of supported formats can be found `here <http://www.gdal.org/ogr_formats.html>`_.



Export to PostGIS
_________________

If you want to export data to a PostGIS server please set sink to following
`PG:dbname=melbourne host=127.0.0.1 port=5432 user=username password=password` and driver_name is `''` (empty) since
the name the driver name is already set with the PG command.


Sample Code for Python Simulation
---------------------------------
.. code-block:: python

    # Upload results to PostGIS database
    export = sim.add_module('GDALPublishResults',
                               {'sink': 'PG:dbname=melbourne host=localhost port=5432 user=user password=password',
                                'driver_name': '',
                                'targetEPSG' : '4283',
                                'sourceEPSG' : '32755',
                                'view_name': 'catchment',
                                'layer_name': 'result_catchment'},
                                link_to_module)

..
