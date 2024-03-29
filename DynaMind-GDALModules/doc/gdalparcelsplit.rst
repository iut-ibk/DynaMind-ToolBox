=================
Subdivide Parcels
=================

Splits a face into n parts with a defined width.


Parameter
---------

+-------------------+------------------------+------------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                      |
+===================+========================+========================================================================+
|blockName          | STRING                 | vew name of the faces to split                                         |
+-------------------+------------------------+------------------------------------------------------------------------+
|subdevisionName    | STRING                 | view name of the split faces                                           |
+-------------------+------------------------+------------------------------------------------------------------------+
|width              | DOUBLE                 | target width                                                           |
+-------------------+------------------------+------------------------------------------------------------------------+
|split_first        | BOOL                   | split a given face along its short side first before start splitting   |
+-------------------+------------------------+------------------------------------------------------------------------+
|target_length      | DOUBLE                 | target length. If set split first is ignored                           |
+-------------------+------------------------+------------------------------------------------------------------------+


Data-stream
-----------

+---------------------+--------------------------+------------------+-------+------------------------------------------+
|        View         |          Attribute       |       Type       |Access |    Description                           |
+=====================+==========================+==================+=======+==========================================+
|   blockName         |                          | FACE             | read  | faces to split                           |
+---------------------+--------------------------+------------------+-------+------------------------------------------+
|                     |                          |                  |       |                                          |
+---------------------+--------------------------+------------------+-------+------------------------------------------+
|   subdevisionName   |                          | FACE             | write | resulting faces                          |
+---------------------+--------------------------+------------------+-------+------------------------------------------+


Module Name
-----------

:index:`GDALParcelSplit <GDALModules; GDALParcelSplit | Subdivide parcel>`


Detailed Description
--------------------

Splits a polygon along its longer side into n faces. n is calculated as integer value of the
longside of the polygon dived by the given width.
