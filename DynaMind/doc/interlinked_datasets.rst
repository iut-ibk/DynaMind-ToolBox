====================
Interlinked Database
====================


DynaMind builds on an interlinked data model providing the basis for complex simulations.
The data management builds on   `SpatiaLite <http://www.gaia-gis.it/gaia-sins/>`_ accessed through the `GDAL library <http://www.gdal.org>`_ providing a fast and memory efficient backend for spatial and non-spatial data. To access the database, the data read, modified, and written by the module need to be pre defined. This allows to optimise the access to the database but more importantly to validate and guarantee that all data needed by a module are available before it is executed.

Data Model
----------

DynaMind uses a `relational data <http://en.wikipedia.org/wiki/Relational_model>`_ data model organised in Views.
A *View* provides a template for the data stored in the database table. A *View* is defined by its unique name and type, and its attributes.

For example:

 - building (POLYGON)
     - stories (INT)
     - roof_area (DOUBLE)


Following *View* types are supported:

 - *COMPONENT* for non-spatial data
 - *NODE* or *POINT* for point data
 - *EDGE* or *LINESTRING* for lines
 - *FACE* or *POLYGON* for closed polygons

As attributes following data types are supported

 - *INT*
 - *DOUBLE*
 - *STRING*
 - *STRINGVECTOR*
 - *DOUBLEVECTOR*
 - *LINK* (Links to components in a view)


Relationships between views are described with the commonly used using  `foreign keys <http://en.wikipedia.org/wiki/Foreign_key>`_.
For example to describe the relation ship between parcels and buildings. The building view contains an attribute parcel_id that refers to the unique id that links to the parcel on which it is built. Commonly these links are indicated using *viewname_id* in this case parcel_id.

Naming Convention
-----------------

For view and attribute names lower case and underscores for separation should be used

e.g.

- building
   - residential_units
   - roof_area
   - persons
   - parcel_id #Link to parcel










