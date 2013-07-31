The module loads data from following sources into DynaMind

- Shapefile
- GeoTIFF
- WFS

##Parameter
|        Name       |          Type          |       Description         | 
|-------------------|------------------------|---------------------------|
| Filename     | file name | only for Shapefile and GeoTIFF short description         |
|| || ||
| Server     | string | only for WFS / server address        |
| Username     | string | only for WFS / user name       |
| Password     | string | only for WFS / password is saved encrypted  |
| Dataset     | string | only for WFS / data set to load  |
|| || ||
| View Name     | string | view name of the dataset in DynaMind  |
| EPSG Code     | int | Coordinate system in which the data set should be transformed while loading a [list of codes](http://spatialreference.org/ref/epsg/)  |
| append to existing data stream     | bool | append loaded data to an existing stream  |
| link with existing view     | bool | links loaded nodes with existing nodes |

##Datastream
|     Identifier    |     Attribute    |      Type             |Access |    Description    |
|-------------------|------------------|-----------------------|-------|-------------------|
| [View Name] |                  | FACE or EDGE or NODE or RASTERDATA   | write  | the data type is automatically assigned depending on the data source |
|                   | first attribute in data set  | double or string | write | data type depends on type attribute type of the source |
 | second attribute in data set  | double or string | write |  |
 | n attribute in data set  | double or string | write |  |