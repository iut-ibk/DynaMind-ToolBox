===============
Export GIS Data
===============

Exports GIS data from DynaMind to common GIS formats `GDAL/OGR <http://www.gdal.org/ogr_formats.html>`_.

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
|targetEPSG         | INT                    | target EPSG code; if 0 simulation EPSG is used                        |
+-------------------+------------------------+-----------------------------------------------------------------------+
|overwrite          | BOOL                   | if true and existing datasource is overwritten, default is false      |
+-------------------+------------------------+-----------------------------------------------------------------------+
|append             | BOOL                   | if true and data are append to existing layer at data source          |
+-------------------+------------------------+-----------------------------------------------------------------------+


Module Name
-----------

:index:`GDALPublishResults <GDALModules; GDALPublishResults | Export GIS data>`


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
                                'layer_name': 'result_catchment',
                                'overwrite': False},
                                link_to_module)

..
