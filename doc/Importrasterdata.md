Imports a ASCII raster data file


##Parameter
|        Name       |          Type          |       Description         | 
|-------------------|------------------------|---------------------------|
| Filename    | filename |        |
| DataName    | string | Name of the raster data set in the data stream e.g. Elevation|
| Multiplier    | double | Each value of the raster data set is multiplied with the multiplier while importing|
| Flip    | bool |flips the raster data set upside down|
| appendToStream    | bool |appends the imported raster data set to an existing data stream|


##Datastream
|     Identifier    |     Attribute    |      Type             |Access |    Description    |
|-------------------|------------------|-----------------------|-------|-------------------|
| [DataName] |                  | RASTERDATA | write  | |

##File Structure

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
##Example
[[images/import_raster_data.png]]
