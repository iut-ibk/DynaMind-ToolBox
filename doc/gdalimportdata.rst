===============
Import GIS Data
===============

:index:`GDALImportData <GDALModules; GDALImportData | Import data>` imports GIS data into DynaMind from common GIS sources supported by the `GDAL/OGR <http://www.gdal.org/ogr_formats.html>`_.

Parameter
---------

+-------------------+------------------------+-----------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                     |
+===================+========================+=======================================================================+
|source             | STRING                 | data source (detailed description see below)                          |
+-------------------+------------------------+-----------------------------------------------------------------------+
|layer_name         | STRING                 | name of the imported layer                                            |
+-------------------+------------------------+-----------------------------------------------------------------------+
|view_name          | STRING                 | name of view                                                          |
+-------------------+------------------------+-----------------------------------------------------------------------+
|append             | BOOL                   | true if append data to existing stream and creates additional inport  |
+-------------------+------------------------+-----------------------------------------------------------------------+
|epsg_to            | INT                    | epsg code used internally in the simulation                           |
+-------------------+------------------------+-----------------------------------------------------------------------+
|epsg_from          | INT                    | epsg code of the source                                               |
+-------------------+------------------------+-----------------------------------------------------------------------+


Detailed Description
--------------------

The import GDAL module us usually the fist module in a DynaMind Simulation. It builds on the GDAL library and
therefore allows data to imported form multiple common GIS sources. An overview of supported formats can be
found `here <http://www.gdal.org/ogr_formats.html>`_. Please ensure that the `epsg_to` and `epsg_from` parameters are set currently to
ensure that the simulation is executed correctly. `epsg_to` should be the same throughout the simulation.


Import from Shapefile
_____________________

This is the most common way to import data into your simulation. please point the source parameter to your file.
The layer_name is the same as the name of the source file (without the directories and the `.shp` file ending). Please
also set the epsg_to and epsg_from parameter. These are used to transform the data in the right coordinate system.
If you are not sure about EPSG code of your source file have a look at `Prj2EPSG <http://prj2epsg.org/search>`_. This website
allows you to identify your coordinate system based on the `.prj` file that comes with the shape file.



Import from PostGIS
___________________

If you want to obtain data from a PostGIS server please set the source to following
`PG:dbname=melbourne host=127.0.0.1 port=5432 user=username password=password` and layer_name to the layer
name to be imported.


Import from WFS
_______________

If you want to obtain data from a WFS server please set the source to following `WFS:hostname` and layer_name to the layer
name to be imported.


Sample Code for Python Simulation
---------------------------------
.. code-block:: python

    # Load data from PostGIS database with GDALImport Data. Per default the module has no inport
    catchment = sim.add_module('GDALImportData',{'source': 'PG:dbname=elwood host=localhost port=5432 user=user password=password',
                                          'layer_name': 'elwood_catchment',
                                          'view_name': 'catchment',
                                          'epsg_from': 4283,
                                          'epsg_to': 32755,
                                          'append' : False})

    # load land use data with the attribute lu_desc = 'Road Void' and that are within the catchment.
    # the parameter append is set to true to allow to connect the module to the catchment module created before
    land_use = sim.add_module('GDALImportData', {'source': 'PG:dbname=melbourne host=localhost port=5432 user=user password=password',
                                          'layer_name': 'landuse_victoria',
                                          'view_name': 'landuse',
                                          'epsg_from': 4283,
                                          'epsg_to': 32755,
                                          'append' : True},
                             catchment,
                             filters={'landuse': {'attribute': "lu_desc = 'Road Void'",
                                                  'spatial': "catchment"}
                             })

..