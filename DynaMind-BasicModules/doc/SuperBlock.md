[List of Basic Modules](List_of_Basic_Modules.md)

#SUPERBLOCK

creates a rectangular face.

##Parameter
| Name |  Type | Description| 
|------|-------|------------|
| NameOfSuperblock | string| name of the created face|
| Height | double | m |
| Width | double | m |
| offsetx | double | m |
| offsety | double | m |
| appendToExisting| bool | append face to existing view |

##Datastream
| Identifier | Attribute | Type | Access | Description|
|------------|-----------|------|--------|------------|
| **[NameOfSuperblock]** || FACE | write | |
| | height | double | write | |
| | width | double | write | |