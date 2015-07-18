=================================
CityDrain 3 for Module Developers
=================================

CityDrain3 is used in several modules as backend to conceptually simulate the urban water cycle.
This manual gives a quick overview of how CityDrain can be integrated in a DynaMind module using its Python interface.


What is CityDrain
=================

CityDrain is a conceptual model to simulate water and pollution flows. Originally it was developed with
focus and integrated urban water management with particular focus on the environmental impacts
of combined sewer systems. However, in recent years models have been developed to extend its functionality with
the aim to simulate the total urban water cycle.

CityDrain builds on the tradition of hydrological modelling, which allows the complex processes for water quality to
be simulated. Water quality and quality processes are formulated for specific components of the urban water system.
For example, a catchment model simulating the runoff and, built up and wash off processes, a sewer/drain simulating the
kinematic wave of water running through the system as well as decay and built up of pollutions. In CityDrain these process are
described in *Nodes*.  Several of these nodes can be linked together in form of a directed graph to route water quantity and
quality changes throughout the urban water system.

Simulation Interface
====================

This section discusses step by step how to create a CityDrain3 Simulation withe the Python interface.

To create a new CityDrain simulation first import the Python interface ``pycd3`` (see below) and create
a new ``CityDrain3`` simulation. To initialise the Simulation set the start and end date
as well as the used time step in seconds. Further to be able to add nodes to the simulation register a native or python plugin.

.. code-block:: python

    # import CityDrain3
    from pycd3 import CityDrain3

    # Init Simulation
    cd3 = CityDrain3("2000-Jan-01 00:00:00",  "2001-Jan-01 00:00:00", "86400")

    # Register Modules
    cd3.register_native_plugin("plugins/libdance4water-nodes")
..


After setting up the simulation nodes can be added to the simulation.
The first parameter is the node name. The second parameter allows to
set parameters of the node. I case of the example we set const_flow to 1.
In the last step to set up the model the nodes can be connected using `add_connection``

.. code-block:: python

    # Add node
    const_flow = cd3.add_node("ConstantFlow", {"const_flow": 1.0})
    flow_probe = cd3.add_node("FlowProbe")

    # Connect nodes
    cd3.add_connection(const_flow, "out", flow_probe, "in")

..

To execute the simulation call ``start`` as parameter the start date is required.

.. code-block:: python

    # Execute Simulation
    cd3.start("2000-Jan-01 00:00:00")

..

Results are either written directly to an output file using the FileOut node or
through accessing the nodes system states. In this example the
FlowProbe stores the volumes on at each time step in the Flow state.
The results can be accessed after the simulation run using the ``get_state_...`` methods.

.. code-block:: python

    # Execute Simulation
    flow_probe.get_state_value_as_double_vector("Flow")

..

