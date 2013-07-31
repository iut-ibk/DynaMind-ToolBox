## Overview Basic Modules
|Name|Short description|
|----|-----------------|
|[ImportwithGDAL](Importwithgdal.md)|loads shapefiles, geoTIFFs, or load data from a WFS server
|[AddDataToNewView](AddDataToNewView.md)|adds components to another view
|[AppendAttributes](AppendAttributes)|intersects faces with rasterdata and add sum or median as attribute to face
|[AppendViewFromSystem](AppendViewFromSystem)|copies several userdefined datastreams in a new one (try to use [JoinDatastreams](JoinDatastreams) instead)
|[AttributeCalculator](AttributeCalculator)|calculates attributes based on other attributes, one of the most used modules for vector data.
|[CalculateCentroid](CalculateCentroid)|calculates centroids and area of faces
|[CalculateBoundingBox](CalculateBoundingBox)|creates bounding boxes for components
|[CellularAutomata](CellularAutomata)|the attribute calculator for raster data
|[SpatialLinking](SpatialLinking)|spatially links nodes with faces and faces with faces
|[JoinDatastreams](JoinDatastreams)|joins user defined data streams to a leading data stream
|[ExportToShapeFile](ExportToShapeFile)|exports data to shape file
|[ImportRasterData](ImportRasterData)|imports a ascii raster data file
|[ExportRasterData](ExportRasterData)|exports raster data set
|[RemoveComponent](RemoveComponent)|removes selected components
|[SinglePoint](SinglePoint)|adds a single point
|[ClearView](ClearView)|removes objects from a view (deprecated use [RemoveComponent](RemoveComponent))
|[FaceToEdge](FaceToEdge)|converts faces to edges
|[ExportNodesToFile](ExportNodesToFile)|exports nodes and their attributes into a text file