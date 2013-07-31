[List of Bacis Modules](List_of_Basic_Modules.md)
#FaceToEdge
Converts Faces to Edges. Just convertes geometry,attributes are not copied.

##Parameter
|        Name       |          Type          |       Description         | 
|-------------------|------------------------|---------------------------|
| Face  | string| name of the edge to convert       |
| Edge  | string | name of the new edges        |


##Datastream
|     Identifier    |     Attribute    |      Type             |Access |    Description    |
|-------------------|------------------|-----------------------|-------|-------------------|
| [Face] |                  | FACE   | read  |  |
| [Edge] |                 | EDGE  |  | write |  
