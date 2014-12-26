==================
GDALSpatialLinking
==================

This module spatially links GIS data. This means if a components of the leading view is joined to the link view by
creating a id.

For example to join parcels with buildings, parcels is the leading view and buildings is the link view

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
| **leadingViewName**|                          | FACE             | read  | lead view components                     |
+--------------------+--------------------------+------------------+-------+------------------------------------------+
|                    |                          |                  |       |                                          |
+--------------------+--------------------------+------------------+-------+------------------------------------------+
| **linkViewName**   |                          |  FACE;EDGE;NODE  | read  | view linked to lead view                 |
+--------------------+--------------------------+------------------+-------+------------------------------------------+
|                    | **leadingViewName** _id  |    INT           | write | id of linked lead view component         |
+--------------------+--------------------------+------------------+-------+------------------------------------------+


Detailed Description
--------------------
The algorithm builds on the gdal library. For each of the leading view's components. The algorithm searches for link view components
that are within the component. It implements two different algorithms.

1. Centroids:

If *withCentroid* is set true and a link view's component is within the face of a leading views face when the centroid (for faces) or
the node itself is within or on the face's edge (TODO check if this is actually true)

2. Intersects:

If *withCentroid* is set to false, the link view's component is within or intersects the face of a leading view using GDAL's
intersect and within algorithms.




