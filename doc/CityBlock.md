[List of Bacis Modules](List_of_Basic_Modules.md)

#CITYBLOCK

Subdivides a rectangular face into smaller rectangular faces e.g. SUPERBLOCK -> CITYBLOCK.

## Parameters

| Name |  Type | Description| 
|------|-------|------------|
| Width | double | target width |
| Height | double | target height |
| CreateStreets | bool | if the edges between the blocks are appended to the data stream as separate view named EdgeName |
| Offset | double | 0 for no offset|
| SuperBlockName | string | |
| BlockName | string | |
| EdgeName | string | |
| CenterCityblockName | string | |
| InterSectionsName |string | |

## Datastream

| Identifier | Attribute | Type | Access | Description|
|------------|-----------|------|--------|------------|
| [SuperBlockName] | | FACE | read | |
||||||
| **[BlockName]** | | FACE | write | |
| | area | double | write | area of the block |
| | width | double | write | real height used for the block |
| | height | double | write | real height used for the block |
| | **_[EdgeName]_** | LINK | write | |
| | **[CenterCityblockName]** | LINK | write | |
||||||
| **_[EdgeName]_** | |  EDGE | write | |
| | width | double | write | width (2*Width)|
| | **[BlockName]** | LINK | write | |
||||||
| **[CenterCityblockName]** | |NODE | write | |
| | centroid_x | double | write | |
| | centroid_y | double | write | |
| | **[BlockName]** | LINK | write | |
||||||
| **[InterSectionsName]** | | NODE | write | |

##Detailed Description

The module calculates the bounding box of the input face (SuperBlockName). The bounding box defines the number of elements in x (bounding box length in x / width) and number of elements in y (bounding box length in y / height). Which defines the block grid used for the generation of the blocks. If an offset is defined the  blocks are offset to the inside (reduces width and height by 2 times the offset). The module creates for every block a set of new nodes. The module exports the grid nodes (InterSectionsName) (without offset). If the option CreateStreets is selected the module exports the edges (EdgeName) defining the block grid used for the generation. The Edges are linked with their adjacent blocks and vice versa.