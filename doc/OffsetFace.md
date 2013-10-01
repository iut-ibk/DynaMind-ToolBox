[List of Bacis Modules](List_of_Basic_Modules.md)
#OffsetFace

the module offsets faces defined in the input view. The resulting faces are added to the output view.

##Parameter
|        Name       |          Type          |       Description         | 
|-------------------|------------------------|---------------------------|
| nameInputView   | STRING| name of input face      |
| nameOutputView   | STRING| name of output face      |
| offset   | DOUBLE| offest + offset inside; - offest outside     |

##Datastream
|     Identifier    |     Attribute    |      Type             |Access |    Description    |
|-------------------|------------------|-----------------------|-------|-------------------|
| [nameInputView]|                  | FACE   | read  | input face |
| [nameOutputView]|                 | FACE   | write  | out face |



