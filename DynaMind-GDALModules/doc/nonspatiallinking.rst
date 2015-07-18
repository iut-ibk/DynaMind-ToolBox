====================
Link with Identifier
====================

Links data using a identifier.
Data from the leading_view with the same leading_attribute as in the join_view join_attribute are linked together.
If the parameter full_join is activated all attributes are copied to form the leading to the join view.

Parameter
---------

+-----------------------+------------------------+------------------------------------------------------------------------+
|        Name           |          Type          |       Description                                                      |
+=======================+========================+========================================================================+
|leading_view           | STRING                 | name of leading view                                                   |
+-----------------------+------------------------+------------------------------------------------------------------------+
|leading_attribute      | STRING                 | name of  identifier used to join                                       |
+-----------------------+------------------------+------------------------------------------------------------------------+
|join_view              | STRING                 | name of link view; NODE, EDGE or FACE components within the leading    |
+-----------------------+------------------------+------------------------------------------------------------------------+
|join_attribute         | STRING                 | links either when centroid is within the lead face (true), false if    |
|                       |                        | link view component is within or intersects face                       |
+-----------------------+------------------------+------------------------------------------------------------------------+
|alternative_link_name  | STRING                 | default is set to  leading_view_id                                     |
+-----------------------+------------------------+------------------------------------------------------------------------+
|full_join              | BOOL                   | default is false, if true all attributes are copied from the           |
|                       |                        | leading_view to the join_view                                          |
+-----------------------+------------------------+------------------------------------------------------------------------+


Data-stream
-----------

+--------------------+---------------------------+------------------+-------+------------------------------------------+
|        View        |          Attribute        |       Type       |Access |    Description                           |
+====================+===========================+==================+=======+==========================================+
| leading_view       |                           | COMPONENT        | read  | lead view components                     |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | leading_attribute         | any              | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    |                           |                  |       |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
| join_view          |                           |  COMPONENT       | read  | view linked to lead view                 |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | join_attribute            |  any             | read  |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | leadingViewName_id        |  INT             | write |                                          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+
|                    | attributes from lead_view |  any             | write | if full_join is selected all attributes  |
|                    |                           |                  |       | form the leading_view are added          |
+--------------------+---------------------------+------------------+-------+------------------------------------------+


Module Name
-----------

:index:`NonSpatialLinking <GDALModules; GDALSpatialLinking | Spatially link data sets>`

