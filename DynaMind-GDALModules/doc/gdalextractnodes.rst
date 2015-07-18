=========================
Create Start and End Node
=========================

 Creates start and end nodes from
a  network. This modules is used to prepare drainage network data for the SWMM export.

Parameter
---------

+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+
|        Name                    |          Type          |       Description                                                                              |
+================================+========================+================================================================================================+
|view_name                       | STRING                 | Name of network view                                                                           |
+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+
|node_name                       | STRING                 | Name of node view                                                                              |
+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+
|tolerance                       | DOUBLE                 | Tolerance used to identify if nodes are identical default 0.01 (m)                             |
+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+
|is_downstream_upstream          | BOOL                   | True if the downstream node is the first one defining the edge. False if the other way around. |
|                                |                        | Default is true                                                                                |
+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+
|add_height                      | BOOL                   | add height from edge                                                                           |
+--------------------------------+------------------------+------------------------------------------------------------------------------------------------+


Data-stream
-----------

+--------------------+---------------------------+------------------+-------+---------------------------------------------+
|        View        |          Attribute        |       Type       |Access |    Description                              |
+====================+===========================+==================+=======+=============================================+
| view_name          |                           | EDGE             | read  |                                             |
+--------------------+---------------------------+------------------+-------+---------------------------------------------+
|                    | start_id                  | INT              | write |                                             |
+--------------------+---------------------------+------------------+-------+---------------------------------------------+
|                    | end_id                    | INT              | write |                                             |
+--------------------+---------------------------+------------------+-------+---------------------------------------------+
|                    | level_lo                  | DOUBLE           | read  | if add_height is true, pipe level downstream|
+--------------------+---------------------------+------------------+-------+---------------------------------------------+
|                    | level_up                  | DOUBLE           | read  | if add_height is true,   pipe level upstream|
+--------------------+---------------------------+------------------+-------+---------------------------------------------+
|                    |                           |                  |       |                                             |
+--------------------+---------------------------+------------------+-------+---------------------------------------------+
| node_name          |                           | WRITE            | read  |                                             |
+--------------------+---------------------------+------------------+-------+---------------------------------------------+
|                    | node_id                   | INT              | write |                                             |
+--------------------+---------------------------+------------------+-------+---------------------------------------------+
|                    | height                    | DOUBLE           | write | if add_height is true                       |
+--------------------+---------------------------+------------------+-------+---------------------------------------------+

Module Name
-----------

:index:`GDALExtractNodes <GDALModules; GDALExtractNodes | Creates Start and End Node>`

Detail Description
------------------

The module iterates over all edges in the network. For each of the edges it extracts the coordinates of the start and end node as well as their
height (level_lo / level_up). It checks if for these coordinates a if node exists within the defined tolerance. If no existing node is found a new node is created. The node height is defined by the lowest level of the connected edges.


Sample Code for Python Simulation
---------------------------------
.. code-block:: python

    nodes = sim.add_module('GDALExtractNodes',
                           {'tolerance': 0.01,
                           'view_name': 'drainage',
                           'node_name': 'junction',
                            'is_downstream_upstream': False,
                            'add_height': True},
                           pipe_network)

..