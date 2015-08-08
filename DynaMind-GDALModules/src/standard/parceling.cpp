#include "parceling.h"
#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>

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
	this->counter_added = 0;

	QThreadPool pool;
	while( (poFeature = cityblocks.getNextFeature()) != NULL ) {
		counter++;
		char* geo;
		poFeature->GetGeometryRef()->exportToWkt(&geo);

		ParcelSplitter * ps = new ParcelSplitter(this, this->width, this->length, geo);
		pool.start(ps);
	}
	pool.waitForDone();

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
	//delete writable_wr;
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




