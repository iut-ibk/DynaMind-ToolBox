#include "parceling.h"
#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>

#include <CGAL/Boolean_set_operations_2.h>
#include <SFCGAL/algorithm/intersection.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/intersections.h>
#include <QThreadPool>

#include "parcelsplitter.h"


DM_DECLARE_CUSTOM_NODE_NAME(GDALParceling,Parcel Faces, Urban Form)

GDALParceling::GDALParceling()
{
	qRegisterMetaType<SFCGAL::Polygon>("SFCGAL::Polygon");
	GDALModule = true;

	this->width = 100;
	this->addParameter("width", DM::DOUBLE, &this->width);

	this->length = 100;
	this->addParameter("length", DM::DOUBLE, &this->length);

	this->blockName = "cityblock";
	this->addParameter("blockName", DM::STRING, &this->blockName);

	this->subdevisionName = "parcel";
	this->addParameter("subdevisionName", DM::STRING, &this->subdevisionName);

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);

	counter_added = 0;

}



void GDALParceling::run()
{
	DM::GDALSystem * city = this->getGDALData("city");

	cityblocks.setCurrentGDALSystem(city);
	parcels.setCurrentGDALSystem(city);

	cityblocks.resetReading();
	OGRFeature *poFeature;
	int counter = 0;
	QVector<SFCGAL::Polygon> splite_vector;
	QThreadPool pool;
	while( (poFeature = cityblocks.getNextFeature()) != NULL ) {
		std::cout << poFeature->GetFID() << std::endl;
		counter++;
		char* geo;


		poFeature->GetGeometryRef()->exportToWkt(&geo);
		std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(geo));

		switch ( g->geometryTypeId() ) {
		case SFCGAL::TYPE_POLYGON:
			break;
		default:
			continue;
		}
		SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();

		//Transfer to GDAL polygon
		Polygon_with_holes_2 p = poly.toPolygon_with_holes_2(true);
		ParcelSplitter * ps = new ParcelSplitter(this->width, this->length, p);
		//ps->run();
		connect(ps, SIGNAL(resultPolygon(QString)), this, SLOT(addToSystem(QString)));
		pool.start(ps);
	}
	pool.waitForDone();
//	foreach(SFCGAL::Polygon  poly, splite_vector)
//		this->addToSystem(poly);

	parcels.syncAlteredFeatures();
	DM::Logger(DM::Debug) << this->counter_added;
}

void GDALParceling::addToSystem(QString poly)
{
	//DM::Logger(DM::Error) << "Hello";
	 QMutexLocker ml(&mMutex);
	std::string wkt = poly.toStdString();

	char * writable_wr = new char[wkt.size() + 1]; //Note not sure if memeory hole?
	std::copy(wkt.begin(), wkt.end(), writable_wr);
	writable_wr[wkt.size()] = '\0';

	OGRGeometry * ogr_poly;

	OGRErr err = OGRGeometryFactory::createFromWkt(&writable_wr, 0, &ogr_poly);

	if (!ogr_poly->IsValid()) {
		DM::Logger(DM::Warning) << "Geometry is not valid!";
		return;
	}
	if (ogr_poly->IsEmpty()) {
		DM::Logger(DM::Warning) << "Geometry is empty ";
		DM::Logger(DM::Warning) << "OGR Error " << err;
		DM::Logger(DM::Warning) << poly.toStdString();
		return;
	}
	//Create Feature
	OGRFeature * parcel = parcels.createFeature();
	parcel->SetGeometry(ogr_poly);
	OGRGeometryFactory::destroyGeometry(ogr_poly);

	counter_added++;
}

void GDALParceling::init()
{
	if (this->blockName.empty() || this->subdevisionName.empty())
		return;

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);
}

string GDALParceling::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalparceling.html";
}




