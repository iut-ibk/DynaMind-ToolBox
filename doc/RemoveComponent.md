[List of Bacis Modules](List_of_Basic_Modules.md)
#RemoveComponent
The modules removes selected components from the view and deregisters the component from linked objects. 

##Parameter
|        Name       |          Type          |       Description         | 
|-------------------|------------------------|---------------------------|
| View     | string | name of the view   |


##Datastream
|     Identifier    |     Attribute    |      Type             |Access |    Description    |
|-------------------|------------------|-----------------------|-------|-------------------|
| [View] |                  | COMPONENT  | modify  |  |
|                   | selected  | double | read | if > 0 the component is removed from the view  |
