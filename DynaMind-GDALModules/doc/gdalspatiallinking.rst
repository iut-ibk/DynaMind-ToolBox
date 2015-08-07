===============
Spatial Linking
===============

Spatially links data. That means for each of the leading view's components the algorithm searches for link view components
that are within its geometrical extend. To link the components the link_id attribute is set in at the link view component (one to many relationship).


Parameter
---------

+-------------------+------------------------+------------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                      |
+===================+========================+========================================================================+
|leadingViewName    | STRING                 | name of leading view; needs to be a FACE                               |
+-------------------+------------------------+------------------------------------------------------------------------+
|linkViewName       | STRING                 | name of link view; NODE, EDGE or FACE components within the leading    |
|                   |                        | view FACE are joined to the leading view.                              |
+-------------------+------------------------+------------------------------------------------------------------------+
|withCentroid       | BOOL                   | links either when centroid is within the lead face (true), false if    |
|                   |                        | link view component is within or intersects face                       |
+-------------------+------------------------+------------------------------------------------------------------------+


Data-stream
-----------

+--------------------+--------------------------+------------------+-------+------------------------------------------+
|        View        |          Attribute       |       Type       |Access |    Description                           |
+====================+==========================+==================+=======+==========================================+
| leadingViewName    |                          | FACE             | read  | lead view components                     |
+--------------------+--------------------------+------------------+-------+------------------------------------------+
|                    |                          |                  |       |                                          |
+--------------------+--------------------------+------------------+-------+------------------------------------------+
|   linkViewName     |                          |  FACE;EDGE;NODE  | read  | view linked to lead view                 |
+--------------------+--------------------------+------------------+-------+------------------------------------------+
|                    |   leadingViewName_id     |    INT           | write | id of linked lead view component         |
+--------------------+--------------------------+------------------+-------+------------------------------------------+



Detailed Description
--------------------
For each of the leading view's components the algorithm searches for link view components
that are within its geometrical extend. To link the components the link_id attribute is set in at the link view component (one to many relationship).

To different approaches can be used:

1. Centroids:

If *withCentroid* is set true and a link view's component is within the face of a leading views face when the centroid (for faces) or
the node itself is within or on the face's edge (TODO check if this is actually true)

2. Intersects:

If *withCentroid* is set to false, the link view's component is within or intersects the face of a leading view using GDAL's
intersect and within algorithms.

Module Name
-----------

:index:`GDALSpatialLinking <GDALModules; GDALSpatialLinking | Spatially link data sets>`

Sample Code for Python Simulation
---------------------------------

.. code-block:: python

   # Link cityblock with parcel
   cityblock_parcel = sim.add_module('GDALSpatialLinking',
                                     {'leadingViewName': 'cityblock',
                                      'linkViewName': 'parcel',
                                      'withCentroid': True}, parcels)
..


