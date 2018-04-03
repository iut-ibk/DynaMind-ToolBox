================
Value From Raster
================

appends the sum of the values under a polygon as attribute to the polygon. The module support simple python eval statements. Use value as the identifer of the cell value.

Parameter
---------

+-----------------------+------------------------+------------------------------------------------------------------------+
|        Name           |          Type          |       Description                                                      |
+=======================+========================+========================================================================+
|view_name              | STRING                 | lead view name                                                         |
+-----------------------+------------------------+------------------------------------------------------------------------+
|attribute_name         | STRING                 | name of the attribute added to the nodes                               |
+-----------------------+------------------------+------------------------------------------------------------------------+
|raster_file            | STRING                 | raster data file                                                       |
+-----------------------+------------------------+------------------------------------------------------------------------+
|filter_statement       | STRING                 | set filter statement e.g. 1 if value == 5 else 0 (value is name of val)|
+-----------------------+------------------------+------------------------------------------------------------------------+



Data-stream
-----------

+--------------------+---------------------------+------------------+-------+------------------------------------------+
|        View        |          Attribute        |       Type       |Access |    Description                           |
+====================+===========================+==================+=======+==========================================+
| view_name          |                           | NODE             | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | attribute_name            | DOUBLE           | write |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+

Module Name
-----------

:index:`DM_ValueFromRasterPoly <GDALModules; DM_ValueFromRasterPoly | Value from Raster under a Polygon>`


Sample Code for Python Simulation
---------------------------------
.. code-block:: python

    value_form_raster = sim.add_module('DM_ValueFromRasterPoly',
                              {'view_name': 'inlet',
                               'attribute_name': 'height',
                               'raster_file': 'vicmap_elev_dtm20.tif'},
                               nodes)
..

