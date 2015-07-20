=============
Copy Features
=============

Copies elements from one view to another.
Per default only the geometry is copied. To copy the attributes set ``copy_attributes`` to true. To add a reference to the original set the ``add_link`` parameter to true.

Parameter
---------

+-------------------+------------------------+-----------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                     |
+===================+========================+=======================================================================+
|from_view          | STRING                 | source of the elements that are copied                                |
+-------------------+------------------------+-----------------------------------------------------------------------+
|to_view            | STRING                 | destination to where elements are copied                              |
+-------------------+------------------------+-----------------------------------------------------------------------+
|add_link           | BOOL                   | if true (default false) creates a link to the original feature        |
+-------------------+------------------------+-----------------------------------------------------------------------+
|copy_attributes    | BOOL                   | if true (default false) copy attributes from original feature         |
+-------------------+------------------------+-----------------------------------------------------------------------+


Data-stream
-----------

+---------------------+--------------------------+-----------------------------+-------+------------------------------------------+
|        View         |          Attribute       |       Type                  |Access |    Description                           |
+=====================+==========================+=============================+=======+==========================================+
|   from_view         |                          | COMPONENT, NODE, EDGE, FACE | read  |                                          |
+---------------------+--------------------------+-----------------------------+-------+------------------------------------------+
|                     | all                      | any                         | read  | if copy_attributes is set true           |
+---------------------+--------------------------+-----------------------------+-------+------------------------------------------+
|                     |                          |                             |       |                                          |
+---------------------+--------------------------+-----------------------------+-------+------------------------------------------+
|   to_view           |                          | COMPONENT, NODE, EDGE, FACE | write |                                          |
+---------------------+--------------------------+------------+----------------+-------+------------------------------------------+
|                     | from_view_id             | LINK       | from_view      | write | if add_link is true a link is created    |
+---------------------+--------------------------+------------+----------------+-------+------------------------------------------+
|                     | all                      | any                         | write | if copy_attributes is set true           |
+---------------------+--------------------------+-----------------------------+-------+------------------------------------------+

Module Name
-----------

:index:`GDALCopyElementToView <GDALModules; GDALCopyElementToView | Copy features>`

Sample Code for Python Simulation
---------------------------------
.. code-block:: python

    #Create conduits form node including reference
    junctions = sim.add_module('GDALCopyElementToView',
                               {'from_view': 'node',
                                'to_view': 'conduit',
                                'add_link': True,
                                'copy_attributes': False},
                                hotstart)
..