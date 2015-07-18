[List of Basic Modules](List_of_Basic_Modules.md)
#SpatialLinking
The Module enables spatial linking of components. It links the base component to the link component. The Link component can have many Base components. 

At the moment the module allows following links:
## Base: Face - Link: Face
 To connect the components the module calculates the center point of the base. Then it > links Bases with a center point within the Link face.
## Base: Node - Link: Face
 The module links Base nodes that are within the Link face