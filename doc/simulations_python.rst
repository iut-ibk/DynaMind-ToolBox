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

To add modules call the ``add_module(class_name, parameter={}, connect_module=None, parent_group=None, filters={}, module_name="")`` function of the initialised simulation.
This functions adds a new module to the simulation and returns the generated module. The returned module can be used to link the module later to another module.

Parameter:

 - class_name: The class name of the to add module. A list of available modules can be found in the documentation. In the example below the GDALImportData is loaded to import data into the simulation
 - parameter: A dictionary to set module parameter. The dictionary uses as key the parameter name and as value the parameter value e.g. ``{'parameter1': 10}``. The list of parameter for a module can be found in its documentation
 - connect_module: The newly created module is connected to the connect_module
 - parent_group: This is used to add the module to a group
 - filters: A dict of filters. ``{ view_name : {'attribute': 'attribute filter' :  spatial: 'view name'} }``
 - module_name: per default the module name is a randomly set; module_name can be used to define a specific name.

.. code-block:: python

    # Load data from PostGIS database with GDALImport Data. Per default the module has no import
    catchment = sim.add_module('GDALImportData',{'source': 'PG:dbname=elwood host=localhost port=5432 user=user password=password',
                                          'layer_name': 'elwood_catchment',
                                          'view_name': 'catchment',
                                          'epsg_from': 4283,
                                          'epsg_to': 32755,
                                          'append' : False})

    # load land use data with the attribute lu_desc = 'Road Void' and that are within the catchment.
    # the parameter append is set to true to allow to connect the module to the catchment module created before
    land_use = sim.add_module('GDALImportData', {'source': 'PG:dbname=melbourne host=localhost port=5432 user=user password=password',
                                          'layer_name': 'landuse_victoria',
                                          'view_name': 'landuse',
                                          'epsg_from': 4283,
                                          'epsg_to': 32755,
                                          'append' : True},
                             catchment,
                             filters={'landuse': {'attribute': "lu_desc = 'Road Void'",
                                                  'spatial': "catchment"}
                             })
    # Upload results to PostGIS database
    export = sim.add_module('GDALPublishResults',
                               {'sink': 'PG:dbname=melbourne host=localhost port=5432 user=user password=password',
                                'driver_name': '',
                                'targetEPSG' : '4283',
                                'sourceEPSG' : '32755',
                                'view_name': 'catchment',
                                'layer_name': 'result_catchment'},
                             land_use)

..



Execute Simulation
==================

After adding and linking the modules the simulation can be executed calling the ``run()`` method.

.. code-block:: python

    # Execute simulation
    sim.run()

..

==================
Advanced Use Cases
==================

Access GDAL Database
====================

The simulation object is a complete DynaMind simulation. This means that when a simulation has been executed the simulation
still holds the calculate data until the simulation is destroyed. This allows the data to be accessed within the Python
script. To do this data (or the name of the database) can be retrieved from the last module in the simulation using `getGDALDBName()`


.. code-block:: python

    # load land use data with the attribute lu_desc = 'Road Void' and that are within the catchment.
    # the parameter append is set to true to allow to connect the module to the catchment module created before
    sim.run()
    #return db name from last module
    last_module.getGDALDBName()
..
