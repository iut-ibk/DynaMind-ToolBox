[List of Bacis Modules](List_of_Basic_Modules.md)
#ExportRasterData
Exports raster data into a text file

##Parameter
|        Name       |          Type          |       Description         | 
|-------------------|------------------------|---------------------------|
| Filename    | string | file name into which the data set is exporter. ".txt" is append automatically       |
| NameOfExistingView    | string | name of the exported raster data view. GUI provides a pull down menu|


##Datastream
|     Identifier    |     Attribute    |      Type             |Access |    Description    |
|-------------------|------------------|-----------------------|-------|-------------------|
| [NameOfExistingView] |                  | RASTERDATA | read  | |


#Detailed Description
Following data format is used:

~~~
ncols         398
nrows         441
xllcorner     335365.33966313
yllcorner     5804184.603761
cellsize      10
NODATA_value  -9999
-9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 
-9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999 -9999
~~~

