====================
Working with Modules
====================

Set Successor Mode
------------------

The successor mode forces a split of the data stream. This means that a new data base is created for the down stream modules.
To switch set the successor model right click on the module to open the context menu and select ``force successor mode``


.. tip:: Successor modes are useful to debug a simulation since it allows to export the database at this point in the simulation (see :ref:`export_database` )

.. _export_database:

Export Simulation Database
--------------------------

To export the simulation database right click on the module to open the context menu and select ``export data at out port``.  The simulate database is exported as `SpatiaLite <http://www.qgis.org/>`_  database and can open with standard GIS software e.g. `QGIS <http://www.qgis.org/>`_.

.. note:: This option is only available for modules that use the GDAL database backend.






