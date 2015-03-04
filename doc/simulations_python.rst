============================
Create Simulations in Python
============================

DynaMind provides a Python interface to set up complex a workflow.
The following document gives a step by step overview of the most important interfaces.


Initialising a New Simulation
=============================

To create a new simulation import pydynamind in your Python script and create a new simulation
instance of the ``Sim`` class (see following listing). This creates an DynaMind simulation.

Before modules can be added to the simulation modules may be registered using with calling
``register_modules("Module Path")``. DynaMind registers modules from all libraries found in the specified
folder (Native or Python).

The last step to initialise a simulations requires to define the spatial reference used for the simulation calling
``set_epsg_code(EPSG Code)``. A full list of EPSG codes can be found `here <http://spatialreference.org>`_. Setting
the EPSG code is crucial otherwise the GIS database can not be correctly initialise which can
make simulations painfully slow.

After registering modules and defining a spatial reference the DynaMind simulation is ready.

.. code-block:: python

    # Import DynaMind Python interface
    import pydynamind as DM

    # Create a new simulation
    sim = DM.Sim()

    # Register modules from folder
    sim.register_modules("Modules")

    # Set EPSG code for the simulation. This is crucial, otherwise the GIS backend of the
    # simulation database is disabled which makes simulations painfully slow
    # EPSG 32755 : WGS 84/UTM zone 55S (works well for Melbourne)
    sim.set_epsg_code(32755)

..

Adding Modules
==============

To add modules call the ``add_module function`` of the initialised simulation.



