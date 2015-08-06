#include "dmosgearth.h"


#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthQt/ViewerWidget>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthUtil/LogarithmicDepthBuffer>
#include <QWidget>


using namespace osgEarth;
using namespace osgEarth::Drivers;


void addBuildings(Map* map)
{
	// create a feature source to load the building footprint shapefile.
	OGRFeatureOptions feature_opt;
	feature_opt.name() = "buildings";
	feature_opt.connection() = "PG:dbname=melbourne host=localhost port=5432 user=postgres password=password";
	feature_opt.layer() = "building_melb";
	//feature_opt.url() = BUILDINGS_URL;
	//feature_opt.buildSpatialIndex() = true;

	// a style for the building data:
	Style buildingStyle;
	buildingStyle.setName( "buildings" );

	// Extrude the shapes into 3D buildings.
	ExtrusionSymbol* extrusion = buildingStyle.getOrCreate<ExtrusionSymbol>();
	extrusion->heightExpression() = NumericExpression( "15" );
	extrusion->flatten() = true;
	//extrusion->wallStyleName() = "building-wall";
	//extrusion->roofStyleName() = "building-roof";

	PolygonSymbol* poly = buildingStyle.getOrCreate<PolygonSymbol>();
	poly->fill()->color() = Color::Olive;

	// Clamp the buildings to the terrain.
	AltitudeSymbol* alt = buildingStyle.getOrCreate<AltitudeSymbol>();
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->binding() = alt->BINDING_VERTEX;

	// a style for the wall textures:
	//Style wallStyle;
	//wallStyle.setName( "building-wall" );
	//SkinSymbol* wallSkin = wallStyle.getOrCreate<SkinSymbol>();
	//wallSkin->library() = "us_resources";
	//wallSkin->addTag( "building" );
	//wallSkin->randomSeed() = 1;

	// a style for the rooftop textures:
	//Style roofStyle;
	//roofStyle.setName( "building-roof" );
	//SkinSymbol* roofSkin = roofStyle.getOrCreate<SkinSymbol>();
	//roofSkin->library() = "us_resources";
	//roofSkin->addTag( "rooftop" );
	//roofSkin->randomSeed() = 1;
	//roofSkin->isTiled() = true;

	// assemble a stylesheet and add our styles to it:
	StyleSheet* styleSheet = new StyleSheet();
	styleSheet->addStyle( buildingStyle );
	//styleSheet->addStyle( wallStyle );
	//styleSheet->addStyle( roofStyle );

	// load a resource library that contains the building textures.
	//ResourceLibrary* reslib = new ResourceLibrary( "us_resources", RESOURCE_LIB_URL );
	//styleSheet->addResourceLibrary( reslib );

	// set up a paging layout for incremental loading. The tile size factor and
	// the visibility range combine to determine the tile size, such that
	// tile radius = max range / tile size factor.
	FeatureDisplayLayout layout;
	layout.tileSizeFactor() = 52.0;
	layout.addLevel( FeatureLevel(0.0f, 20000.0f, "buildings") );

	// create a model layer that will render the buildings according to our style sheet.
	FeatureGeomModelOptions fgm_opt;
	fgm_opt.featureOptions() = feature_opt;
	fgm_opt.styles() = styleSheet;
	fgm_opt.layout() = layout;

	map->addModelLayer( new ModelLayer( "buildings", fgm_opt ) );
}


DMOSGEarth::DMOSGEarth()
{

	osgViewer::Viewer viewer;
	viewer.setRunFrameScheme( viewer.ON_DEMAND );
	viewer.setCameraManipulator( new osgEarth::Util::EarthManipulator() );

	// Create a Map and set it to Geocentric to display a globe
	osgEarth::Map* map = new osgEarth::Map();

	// Add an imagery layer (blue marble from a TMS source)
	{
		TMSOptions tms;
		tms.url() = "http://readymap.org/readymap/tiles/1.0.0/7/";
		ImageLayer* layer = new ImageLayer( "NASA", tms );
		map->addImageLayer( layer );
	}
	{
		addBuildings(map);
		/*OGRFeatureOptions building_layer;
		building_layer.connection() = "PG:dbname=melbourne host=localhost port=5432 user=postgres password=password";
		building_layer.layer() = "building_melb";
		//tms.url() = "http://readymap.org/readymap/tiles/1.0.0/7/";
		//ImageLayer* layer = new ImageLayer( "NASA", tms );
		ModelLayer* blayer = new ModelLayer( "buildings", building_layer );
		map->addModelLayer(blayer);*/
	}
	MapNode* mapNode = new MapNode( map );

	osgEarth::QtGui::ViewerWidget * viewerWidget  = new osgEarth::QtGui::ViewerWidget(mapNode);
	osgViewer::ViewerBase* v = viewerWidget->getViewer();
	v->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
	//v->set
	//new osgEarth::QtGui::ViewerWidget()
	//QWidget *viewerWidget = new QWidget();
	//viewerWidget->getViewer()->setSceneData( map );
	viewerWidget->setMinimumWidth(500);
	viewerWidget->setMinimumHeight(500);
	viewerWidget->show();
}
