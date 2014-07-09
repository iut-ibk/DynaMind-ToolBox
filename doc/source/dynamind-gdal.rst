DynaMind-GDAL
=============

To extend the functionality of DynaMind we integrated GDAL as additional option of the DynaMind data structure.
Unfortunately the use different data streams is currently not supported, meaning that modules developed with the DynaMind 
data structure can not be combined with modules that build on GDAL.

Why the shift do GDAL:

- __Standard__: GDAL is an established standard in GIS and many 3rd party libraries to extend its functionality are available
- __API__: GDAL provides a widely used API for using GIS data. This makes avoids that people familiar with GDAL don't have
to get familiar with yet another form of data representation
- __Functionality__: GDAL provides long missing features such as spatial and attribute filters, but also GDAL provides useful 
operations such as boolean operation for geometries etc.
- Database: GDAL allows DynaMind to store its data directly into a database supported by GDAL (e.g. PostGIS or Spatialite). This allows
enables DynaMind to work on big data
