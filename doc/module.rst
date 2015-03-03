==================
Module Development
==================

Modules are the heart and soul of every DynaMind simulation. A module is a process to manipulate data in the data stream "running" through the module.
DynaMind supports the development of reusable modules by making data used in modules transparent, and parameters easy to modify. Further, an easy to us interface allows  modules to be connected to complex
simulations. The following section gives an introduction in the basic concepts needed for the module development. For the code examples the
Python interface is used, however the Python API is similar to the C++ API. The API doc can be found `here <http://www.dynamind-toolbox.org/wp-content/uploads/api-doc/nightly/doc/doxygen/output/html/index.html>`_.

Modules consist of two main parts:

    1. The *Initialisation* makes used data and parameters known to the simulation.
    2. The *Data Manipulation Process* (DMP) or the *run method* which contains the actual algorithms to manipulate data.

To access the data DynaMind provides two different data representations:

    (i) The *DynaMind Component API*: A native DynaMind API providing standard GIS components.
    (ii) The *DynaMind GDAL API*: Builds on the widely used GDAL API `see <http://www.gdal.org>`_.

The documentation focuses on the use on the newer DynaMind GDAL API.

The following code block shows an implementation of the two parts.


.. code-block:: python

    class MyModule(Module):
            """
            Module Initialisation
            """
            def __init__(self):
                Module.__init__(self)

                #To use the GDAL API
                self.setIsGDALModule(True)
                #Parameter Definition
                ...

                #Data Stream Definition
                ...

            """
            Data Manipulation Process (DMP)
            """
            def run(self):
                #Data Stream Manipulation
                ....
..

Module Initialisation
=====================
Lets take a closer look at the module initialisation. The idea of the module initialisation is to make the parameters and data
used by the module transparent to the simulation and to the users. The initialisation distinguishes between (1) the parameter definition;
which describes parameter that can be set by the user and (2) the data stream definition; which defines the needed, generated and modified data
in terms of geometry types and attributes.

Parameter Definition
--------------------
Parameter defined in the initialisation can be modified by the user. A parameter can be added using ``self.createParameter("ParameterName", TYPE, "description")``.
To initalise the parameter with a default value use ``self.ParameterName = foo``.

Parameters, once generated, are managed by DynaMind. This means that every time before the DMP is called the module parameter values are updated. Further, parameters
are automatically saved and loaded from the DyanMind save file. Also DynaMind provides interfaces to change parameter from outside the
module.


.. code-block:: python

    #Create Parameter
    self.createParameter("MyParameter",LONG,"Sample Description")

    #Initalise Parameter (may be called after the parameter is created!)
    self.MyParameter = 5
..

Following parameter types are supported:

+---------------+-----------------------+
| Parameter Type|Description            |
+===============+=======================+
|BOOL           | boolean               |
+---------------+-----------------------+
|INT            | integer               |
+---------------+-----------------------+
|LONG           | long integer          |
+---------------+-----------------------+
|STRING         | string                |
+---------------+-----------------------+
|FILENAME       | location to file [1]_ |
+---------------+-----------------------+
|STRING_LIST    | list of strings  [2]_ |
+---------------+-----------------------+
|STRING_MAP     | map of strings   [3]_ |
+---------------+-----------------------+

Data Stream Definition Using Views (only recommended for advanced users)
------------------------------------------------------------------------
To be able to connect a module to the data stream the data read, modified and created by the module need to be defined.
A data stream in DynaMind is defined as a collection of *Views*. *Views* describe data in the stream in terms of their geometry type, attributes and, links to other views.
To let the simulation know which data are used, views need to be registered during module initialisation. Views are created using
``foo = View("name", GEOMETRY_TYPE, ACCESS)``. Attributes are attached with ``foo.addAttribute("name", TYPE, ACCESS)``. For both
geometry and attribute the following access modes are available:  READ, WRITE or MODIFY.
The following code block shows how views are defined using the Python interface.

.. code-block:: python

    #Data read by the module
    self.streets = View("street", EDGE, READ)
    self.streets.addAttribute("width", DOUBLE, READ)

    #New data created by this module
    self.drain = View("drain", EDGE, WRITE)
    self.drain.addAttribute("diameter", DOUBLE,  WRITE)

..

Available geometry types:

+---------------+-----------------------------------------------------+
| Geometry Type |Description                                          |
+===============+=====================================================+
|COMPONET       | data without geometry                               |
+---------------+-----------------------------------------------------+
|NODE           | node in x,y,z                                       |
+---------------+-----------------------------------------------------+
|EDGE           | connection between nodes                            |
+---------------+-----------------------------------------------------+
|FACE           | closed polygon, can contain wholes                  |
+---------------+-----------------------------------------------------+

Available attribute types:

+---------------+-----------------------------------------------------+
| Attribute Type|Description                                          |
+===============+=====================================================+
|DOUBLE         | double value                                        |
+---------------+-----------------------------------------------------+
|STRING         | string                                              |
+---------------+-----------------------------------------------------+
|DOUBLEVECTOR   | vector of doubles                                   |
+---------------+-----------------------------------------------------+
|STRINGVECTOR   | vector of strings                                   |
+---------------+-----------------------------------------------------+

To register the views in DynaMind, views compiled into a vector and with ``self.addData("name", views)`` registered at the module.
The view definition is used by the simulation to check if all data are provided needed from a module, but also to optimise the data stream.

.. code-block:: python

    #Compile views
    views = []
    views.append(street)
    views.append(drain)

    #Add views to stream
    self.addData("city", views)

..

Data Stream Definition Using ViewContainer
------------------------------------------
When using the DynaMind-GDAL API ViewContainers provide an easier way to define and access the data stream.
ViewContainer enhance the View definition described before, providing a more direct access to its data. This means
that the *System* object is no longer required. Defining a ViewContainer is similar to
defining a View. However, instead of adding the views to the data stream, ViewContainers are registered at
the module itself using ``self.registerViewContainers(views)``. The following code block gives an example of how
to define and register a ViewContainer. Note that, to access the data in the ViewContainer later, it is important to
add the ViewContainer as Attribute to the class using the ``self.`` statement.


.. code-block:: python

    #Data read by the module
    self.streets = ViewContainer("street", EDGE, READ)
    self.streets.addAttribute("width", DOUBLE, READ)

    #New data created by this module
    self.drain = ViewContainer("drain", EDGE, WRITE)
    self.drain.addAttribute("diameter", DOUBLE,  WRITE)

    #Compile views
    views = []
    views.append(street)
    views.append(drain)

    #Register ViewContainer to stream
    self.registerViewContainers(views)

..



Data Manipulation Process (DMP)
===============================

This is where the actual data processing is happening.
The DMP is described in the ``run`` method of the module. Every time the module is executed the run
method is called. Depending on the chosen data API data are either accessed using the *System* class
for the *DynaMind Component API* or via *ViewContainer* using the *DynaMind GDAL API*


Data Access Using the *DynaMind GDAL API*
-----------------------------------------
The *DynaMind GDAL API* builds on the widely applied GDAL library. In GDAL
geometric objects are described as ``Feature``. Features contain geometry and attributes describing the object
and provides methods to set and get geometry as well as attributes. See the GDAL documentation for a full description of the
`Feature API <http://www.gdal.org/classOGRFeature.html>`_.

**Access Features**

The ViewContainer manages the assess to the features stored in the underlying data stream.
The API of the ViewContainer class is based on the GDAL *Layer API* tailored to the DynaMind environment.
To iterate over all features you can use the ViewContainer directly  in a ``for`` loop (see the following code block). The returned
feature is a "real" GDAL Feature. (Please have a look at `GDAL Feature API <http://www.gdal.org/classOGRFeature.html>`_ ).
Before you start reading the components it is recommended to reset the iterator using ``ViewContainer.reset_reading()``.
Currently it is still required to clear the ViewContainer cache after it has been used with calling ``ViewContainer.sync()``

.. code-block:: python

    def run(self):
        #Rest read position
        self.streets.reset_reading()

        #Iterate over all features of the ViewContainer
        for street in self.streets:
            street_width = street.GetFieldAsDouble("width")
        #Clear container cache
        self.streets.sync()
..

**Create Features**

``ViewContainer.create_feature()`` registers a new feature in the ViewContainer. The created itself is empty and
does not contain either geometry or attributes. The features geometry and attributes can be created and set using the GDAL Python API.
For performance reasons the features are not directly written into the data stream. To finally write the features and clear
the ViewContainer cache please call ``ViewContainer.sync()``.


.. code-block:: python

    #Create 1000 new nodes
    for i in range(1000):
        #Create new feature
        street = self.streets.create_feature()

        #Create geometry
        pt = ogr.Geometry(ogr.wkbPoint)
        pt.SetPoint_2D(0, 1, 1)

        #Set geometry in feature
        street.SetGeometry(pt)
    #Write create features into stream
    self.streets.sync()

..

**Modify Features**

Similar to reading features, existing features can be modified while iterating over the features stored in the ViewContainer.
To write the altered features to the data stream please use ``ViewContainer.sync()``.


.. code-block:: python

    #Rest read position
    self.streets.reset_reading()
    #Iterate over all features and set street width to 3
    for street in self.streets:
        street.SetField("width", 3)
    #Write altered features to stream
    self.streets.sync()

..

Error Handling
--------------
If an error occurs during the DMP the module should set its execution status to MOD_EXECUTION_ERROR. This let the simulation
know that an error occurred in the module and it stops the simulation. ``self.setStatus(MOD_EXECUTION_ERROR)``

Advanced API
============
Advanced features include working with linked data sets as one of the key concepts of DynaMind.

Linked Data
-----------

To link two features commonly and attribute is to create a an attribute that points from feature A to feature B.
This allows one to many relations ships to be created. The reflection of many to many relations ships requires a separate link table.

.. code-block:: python

    #parcel <- building
    self.parcel = ViewContainer("parcel", FACE, READ)

    #Link parcels to building using the parcel id
    self.building = ViewContainer("building", FACE, READ)
    self.building.addAttribute("parcel_id", INT,  WRITE)


    #Register ViewContainer to stream
    self.registerViewContainers([self.parcel, self.building])

..

Features can be linked using the feature id ``GetFID()``.


.. code-block:: python

    for p in parcel:
        #Find building in parcel
        b.SetField("parcel_id", p.GetFID())

..

To return the get the linked feature use the ``get_feature``, which returns the feature with the corresponding FID

.. code-block:: python

    for b in building:
        p_id = b.GetFieldAsInteger("parcel_id")
        p = parcel.get_feature(p_id)

..


If you require to return the reveres, this means all features B that point that are linked to a specific feature A you may
use the following function

.. code-block:: python

    for p in parcel:
        for b in self.building.get_linked_features(p):
        # Do something with the buildings on the same parcel

..


To speed things up please create a search index before using the ``get_linked_features`` method.
This can be done with the ``create_index`` method. The method needs to be called first after the run. Otherwise it causes and
error.

.. code-block:: python

        def run(self):
            # Init index before doing anything else. Otherwise it causes an error.
            self.building.create_index("component_id")


..

.. [1] DynaMind automatically translates an absolute file location into the relative location to simplify the file exchange
.. [2] A STRING_LIST is string. The strings a separated using ``*|*`` e.g. ``*|*text1*|*test2*|*``
.. [3] A STRING_MAP is string. The strings a separated using ``*||*``for a new row and ``*|*`` for columns e.g. ``*||*key1*|*value1*||*key2*|*value2*||*``
