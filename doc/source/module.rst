=======
Modules
=======

Modules are the heart and soul of every DynaMind simulation. A module is a process to manipulate data in the data stream "running" through the module.
DynaMind has been developed to support the development reusable modules by making the needed and generated data transparent, and parameters easy to modify. Further, connecting modules to complex
simulations should be made simple.

To provide this functionality modules consist of two parts: (1) The initialisation part makes the used data and parameter known to the simulation.
(2) The Data Manipulation Process (DMP) that is executed during the simulation. Before the DMP is executed the simulation takes care that the module
parameter are update and the needed data are available in the data stream. The following code block shows an implementation of the 2 parts using the Python
interface.


.. code-block:: python

    class MyModule(Module):
            """
            Module Initialisation
            """
            def __init__(self):
                Module.__init__(self)
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


Module Initialisation
=====================
Lets take closer look at the module initialisation. The idea of the module initialisation to make the parameters and data
used by the module transparent to its the simulation and its users. The initialisation distinguishes between two sections. (1) the parameter definition;
which describes parameter that can be set by the user. (ii) the data stream definition; which defines the needed, generated and modified data
in terms of geometry types and attributes.

Parameter Definition
--------------------
Parameter defined in the initialisation section can be modified by the user. A parameter can be added using ``self.createParameter("ParameterName", TYPE, "description")``. To initalise the parameter with a default
value use ``self.ParameterName = foo``.

Parameter, once generated, are managed by DynaMind. This means that every time before the DMP is called the module parameters are updated. Further, parameter
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

Data Stream Definition
----------------------
To be able to connect a module to the data stream, the data read, modified and created by the module need to be defined.
The data stream in DynaMind is defined as a collection of *Views*. *Views* describe data in the stream in terms of their type
of geometry, attributes and, links to other views. To let the simulation know which data are used, views need to be registered during module initialisation.
Views are created using ``foo = View("name", GEOMETRY_TYPE, ACCESS)``. Attributes are attached with ``foo.addAttribute("name", TYPE, ACCESS)``. For both geometry and attribute the following access modes are available: ACCESS READ, WRITE or MODIFY.
The following code block shows how views are defined using the Python interface.

.. code-block:: python

    #Data read by the module
    self.streets = View("STREET", EDGE, READ)
    self.streets.addAttribute("width", DOUBLE, READ)

    #New data created by this module
    self.drain = View("DRAIN", EDGE, WRITE)
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
|FACE           | Closed Polygon, can contain wholes                  |
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

To register the views in DynaMind, the used views  are compiled into a vector and with ``self.addData("name", views)`` registered at the module.
The view definition is used by the simulation to check if all data are provided needed from a module, but also to optimise the data stream.

.. code-block:: python

    #Compile views
    views = []
    views.append(street)
    views.append(drain)

    #Add views to stream
    self.addData("city", views)

..


Data Manipulation Process (DMP)
===============================

This is where the actual fun is happening.




GDAL Module Development using C++
=================================

Since v0.6 DynaMind enables to develop Modules that use GDAL within the data stream.
The following documentation gives an introduction in how to develop module using the GDAL functionality.


.. [1] DynaMind automatically translates an absolute file location into the relative location to simplify the file exchange
.. [2] A STRING_LIST is string. The strings a separated using ``*|*`` e.g. ``*|*text1*|*test2*|*``
.. [3] A STRING_MAP is string. The strings a separated using ``*||*``for a new row and ``*|*`` for columns e.g. ``*||*key1*|*value1*||*key2*|*value2*||*``