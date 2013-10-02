[List of Basic Modules](List_of_Basic_Modules.md)

#PARCELS

subdivides a cityblock into parcels

##Parameter
| Name |  Type | Description| 
|------|-------|------------|
| Height | double | m |
| Width | double | m |

##Datastream
| Identifier | Attribute | Type | Access | Description|
|------------|-----------|------|--------|------------|
| CITYBLOCK || FACE | read | |
| PARCEL || FACE | write | |


##Detailed Description

A cityblock is subdivided into parcles with a equal size. The provided parcel width and height are the minimum. Note parcels are not linked with the cityblock.
