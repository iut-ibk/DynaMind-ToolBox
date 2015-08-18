====================
Calculate Attributes
====================

Calculates a new or overwrites an existing attribute.


Parameter
---------

+-----------------------+------------------------+--------------------------------------------------------------------------------------------+
|        Name           |          Type          |       Description                                                                          |
+=======================+========================+============================================================================================+
|attribute              | STRING                 | view_name.attribute_name                                                                   |
+-----------------------+------------------------+--------------------------------------------------------------------------------------------+
|attributeType          | STRING                 | INT, DOUBLE, STRING                                                                        |
+-----------------------+------------------------+--------------------------------------------------------------------------------------------+
|variables              | STRING_MAP             | variable name;  view_name.attribute_name or view_name.linked_view_name.attribute_name      |
|                       |                        | to resolve the links view_name.linked_view_name_id needs to exist                          |
+-----------------------+------------------------+--------------------------------------------------------------------------------------------+
|equation               | STRING                 | equation to solve e.g. variable_a + 5                                                      |
|                       |                        | attributes used in equation need to be defined as variables                                |
+-----------------------+------------------------+--------------------------------------------------------------------------------------------+


Data-stream
-----------

+--------------------+---------------------------+------------------+-------+------------------------------------------+
|        View        |          Attribute        |       Type       |Access |    Description                           |
+====================+===========================+==================+=======+==========================================+
| attribute view     |                           | COMPONENT        | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | attribute                 | any              | write |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    |                           |                  |       |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
| variable view      |                           |  COMPONENT       | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | attribute_name            |  any             | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | linked_view_id            |  INT             | read  | (if linked attribute)                    |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
| linked view        |                           |  COMPONENT       | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | attribute_name            |  any             | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+



Detailed Description
--------------------

The module integrates the `muparser library <http://muparser.beltoforion.de/mup_features.html>`_, a parser for mathematical equations and can access
variables of linked components.

Common use cases
________________

*Assign a constant*

.. code-block:: bash

    attribute:     building.construction_year
    attributeType: INT
    variables:
        (empty)
    equation:      1990

*Assign a constant from global object*



.. code-block:: bash


    attribute:      building.construction_year
    attributeType:  INT
    variables:
         y : city.year
    equation:       y


.. note::

    When non - linked variables are defined the attribute of the first found feature is returned. Therefore only use in combination where it is guaranteed that
    only one feature exists within a view. (e.g. city or world)

*Assign only to a subset*

.. code-block:: bash

    attribute:        building.construction_year
    attributeType:    INT
    variables:
         y : city.year
         c : building.construction_year # Variables might have the be initialed before they can be overwritten. This can be done by assigning a constant
    equation:         y == 0 ? y : c # If year is not defined (0) the year is set to city.year, otherwise building.construction_year is assigned

.. note::

    For this use case setting a module filter is much more efficient. Use the  *Assign constant example* and set the module attribute filter to building: construction_year is null

*Calculate Number of people in a city district*

.. code-block:: bash

    attribute:         district.population
    attributeType:     INT
    variables:
        pop : district.parcel.building.household.persons
    equation:          pop # Assigns the number of people living in a district. The data is derived from the linked feature.

.. note::

    Links are resolved through a recursion and the sum of all the connected attributes is returned.

*Increase increment in loop*

.. code-block:: bash

    attribute:          city.year
    attributeType:      INT
    variables:
         y : city.year # Variables might have the be initialed before they can be overwritten. This can be done by assigning a constant
    equation: y + 1


Further to the expression provided by  the  `muparser library <http://muparser.beltoforion.de/mup_features.html>`_ following
expressions are supported:

Expressions
___________

+--------------------+---------------------------------------------------------------------------------------------+
|        Name        |          Description                                                                        |
+====================+=============================================================================================+
|  rand(value)       | returns random number in range ]0-value]                                                    |
+--------------------+---------------------------------------------------------------------------------------------+
|  round(value)      | rounds value to nearest int value                                                           |
+--------------------+---------------------------------------------------------------------------------------------+
|  counter           | current counter when used in loops                                                          |
+--------------------+---------------------------------------------------------------------------------------------+
|  num2str(value)    | number to string                                                                            |
+--------------------+---------------------------------------------------------------------------------------------+
|  str2num(value)    | string to number                                                                            |
+--------------------+---------------------------------------------------------------------------------------------+
|  ? then : else     | if clause e.g. a > 5 ? variable_a : variable_b see                                          |
|                    | for the operators `see <http://muparser.beltoforion.de/mup_features.html>`_                 |
+--------------------+---------------------------------------------------------------------------------------------+

Module Name
-----------

:index:`GDALAttributeCalculator <GDALModules; GDALAttributeCalculator | Calculate Attributes>`