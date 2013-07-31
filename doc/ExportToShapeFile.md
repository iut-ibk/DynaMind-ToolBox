[List of Bacis Modules](List_of_Basic_Modules.md)
#ExportToShapeFile

Export vector data and their attributes to a shape file.

##Parameter

|Name|Description|
|----|-----------|
|FileName| file name will be automatically extended to _ _TYPE_ _ _RUN_. Note: also define folder e.g. /tmp/test you will find a shapefile with the name test_node_1.shp in the /tmp/ folder
|Name| name of the view you want to export |
|CoordinateSystemEPSG|EPSG code as number. A list of codes can be found [here](http://spatialreference.org/ref/) please use the search field. Note: we are not transforming the coordinates at this stage. If you imported data make sure you use the same EPSG code
|offsetX|offset in X
|offsetY|offset in Y
