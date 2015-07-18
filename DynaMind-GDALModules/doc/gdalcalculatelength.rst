========================
Calculate Feature Length
========================

Calculates the length of and edge or linestring.

Parameter
---------

+-----------------------+------------------------+------------------------------------------------------------------------+
|        Name           |          Type          |       Description                                                      |
+=======================+========================+========================================================================+
|leading_view           | STRING                 | may be from type EDGE or LINESTRING                                    |
+-----------------------+------------------------+------------------------------------------------------------------------+



Data-stream
-----------

+--------------------+---------------------------+------------------+-------+------------------------------------------+
|        View        |          Attribute        |       Type       |Access |    Description                           |
+====================+===========================+==================+=======+==========================================+
| leading_view       |                           | EDGE             | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | length                    | DOUBLE           | write |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+

Module Name
-----------

:index:`GDALCalculateLength <GDALModules; GDALCalculateLength | Calculate length>`

Sample Code for Python Simulation
---------------------------------
.. code-block:: python

    #calculate length of edges in network
    calculate_length = sim.add_module('GDALCalculateLength',
                               {'leading_view': 'network'},
                               previous_module)
..

