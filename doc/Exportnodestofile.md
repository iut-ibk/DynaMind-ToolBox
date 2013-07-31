The module exports nodes and their attributes into a text file

##Parameter
|        Name       |          Type          |       Description         | 
|-------------------|------------------------|---------------------------|
| FileName     | string |  name of the text file       |
| View     | string |  name of the view |


##Datastream
|     Identifier    |     Attribute    |      Type             |Access |    Description    |
|-------------------|------------------|-----------------------|-------|-------------------|
|[ViewName] |                  | NODE   | read  | |
|                   | all attributes  | DOUBLE or STRING | read | module exports all attributes if attribute type is either double or string |
