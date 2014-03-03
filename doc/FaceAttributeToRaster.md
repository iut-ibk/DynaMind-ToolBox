[List of Basic Modules](List_of_Basic_Modules.md)

#FaceAttributeToRaster

Transfers attribute to raster data cell that is within the face.

## Parameters

| Name |  Type | Description| 
|------|-------|------------|
| FaceName | string | name of input face |
| attributeName | string | name of attribute |
| rasterDataName | string | name of raster data field |


## Datastream

| Identifier | Attribute | Type | Access | Description |
|------------|-----------|------|--------|------------|
| **[FaceName]** | | FACE | read | |
| | **[attributeName]**| double | read | attribute that is written to raster cell |
||||||
| **[rasterDataName]** | | RASTERDATA | modify | raster data are cleared before the simulation is executed |
