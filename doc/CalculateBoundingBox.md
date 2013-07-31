Calculates bounding box or minimal bounding box of nodes, edges, faces or rasterdata contained by an existing view. (NameOfExistingView). For edges and nodes the bounding box over all elements is calculated (please select overAll option)

##Parameters:
- NameOfExistingView: Name of existing View
- NameOfNewView: (optional) Define name of new view. If blank a new view is NameOfExistingView_BOUNDIG_BOX is created
- MinBounding: Calculates the minimum bounding box (vector data only) (see options)
- overAll: Calculates the bounding box for all elements (see options)

## Datastream

The model creates a new Face (overAll = true) or Faces including following attributes

- NameOfExistingView_BOUNDIG_BOX | NameOfNewView
  - l length of the bounding box (in direction of the box)
  - b width
  - x_min
  - y_min
  - y_min
  - y_max

##Options

- MinBounding the minimal bounding box is calculated. 
- overAll creates the bounding box over all nodes. Of false the bounding box for every single face is created (not available for nodes or edges).


Bounding Boxes standard

[[images/calculate_bounding_box_standard_single.png]]

Bounding Boxes minimal

[[images/calculate_bounding_box_miminal.png]]

Bounding Box option overalll standard

[[images/calculate_bounding_box_tot_standard.png]]

Bounding Box option right minimal

[[images/calcualte_bounding_box_tot_min.png]]

Bounding Box Rasterdata

[[images/calcualte_bounding_raster.png]]