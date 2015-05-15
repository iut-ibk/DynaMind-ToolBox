=============================================
Create Start and End Node from a Pipe Network
=============================================

:index:`GDALExtractNodes <GDALModules; GDALExtractNodes | Creates Start and End Node>` Creates start and end nodes from
a pipe network. This modules is used to prepare drainage network data for the SWMM export.

Parameter
---------

+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+
|        Name                    |          Type          |       Description                                                                              |
+================================+========================+================================================================================================+
|tolerance                       | DOUBLE                 | Tolerance used to identify if nodes are identical default 0.01 (m)                             |
+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+
|is_downstream_upstream          | BOOL                   | True if the downstream node is the first one defining the edge. False if the other way around. |
|                                |                        | Default is true                                                                                |
+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+


Data-stream
-----------

+--------------------+---------------------------+------------------+-------+------------------------------------------+
|        View        |          Attribute        |       Type       |Access |    Description                           |
+====================+===========================+==================+=======+==========================================+
| network            |                           | EDGE             | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | start_id                  | INT              | write |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | end_id                    | INT              | write |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | level_lo                  | DOUBLE           | read  | pipe level downstream                    |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | level_up                  | DOUBLE           | read  | pipe level upstream                      |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    |                           |                  |       |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
| node               |                           | WRITE            | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | node_id                   | INT              | write |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | height                    | DOUBLE           | write |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+

Detail Description
------------------

The module iterates over all edges in the network. For each of the edges it extracts the coordinates of the start and end node as well as their
height (level_lo / level_up). It checks if for these coordinates a if node exists within the defined tolerance. If no existing node is found a new node is created. The node height is defined by the lowest level of the connected edges.


Sample Code for Python Simulation
---------------------------------
.. code-block:: python

    nodes = sim.add_module('GDALExtractNodes',
                           {'tolerance': 0.01,
                            'is_downstream_upstream': False},
                           pipe_network)

..