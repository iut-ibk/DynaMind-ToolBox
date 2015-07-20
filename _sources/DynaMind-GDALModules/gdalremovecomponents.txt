================
Remove Component
================

:index:`GDALRemoveComponets <GDALModules; GDALRemoveComponets | Remove components>` deletes all components within the defined view. To delete specific components please set a module filter.

Parameter
---------

+-------------------+------------------------+------------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                      |
+===================+========================+========================================================================+
|view_name          | STRING                 | view name of the faces to split                                        |
+-------------------+------------------------+------------------------------------------------------------------------+


Data-stream
-----------

+---------------------+--------------------------+------------------+-------+------------------------------------------+
|        View         |          Attribute       |       Type       |Access |    Description                           |
+=====================+==========================+==================+=======+==========================================+
| blockName           |                          | any              | write | removes component                        |
+---------------------+--------------------------+------------------+-------+------------------------------------------+


Sample Code for Python Simulation
---------------------------------
.. code-block:: python

    # Clean network with same start and end node
    clean_network = sim.add_module('GDALRemoveComponets',
                                  {'view_name': 'network'},
                                  nodes,
                                  filters={'network': {'attribute': "start_id = 0 or end_id = 0"}})

..
