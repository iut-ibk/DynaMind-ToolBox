#include "dmosgearth.h"


#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthQt/ViewerWidget>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <QWidget>


using namespace osgEarth;
using namespace osgEarth::Drivers;

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
