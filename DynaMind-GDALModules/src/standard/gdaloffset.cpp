#include "gdaloffset.h"



#include "offsetworker.h"

#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>

#define CGAL_HAS_THREADS
DM_DECLARE_CUSTOM_NODE_NAME(GDALOffset, Offset Faces, Geometry Processing)


GDALOffset::GDALOffset()
{
	GDALModule = true;

	this->blockName = "cityblock";
	this->addParameter("blockName", DM::STRING, &this->blockName);

	this->subdevisionName = "parcel";
	this->addParameter("subdevisionName", DM::STRING, &this->subdevisionName);

	this->offset = 0;
	this->addParameter("offset", DM::DOUBLE, &this->offset);

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);

	counter_added = 0;
}

void GDALOffset::addToSystem(QString poly)
{
	QMutexLocker ml(&mMutex);
	std::string wkt = poly.toStdString();

	char * writable_wr = new char[wkt.size() + 1]; //Note not sure if memory hole?
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


void GDALOffset::run()
{
	cityblocks.resetReading();
	OGRFeature *poFeature;
	int counter = 0;

	QThreadPool pool;
	while( (poFeature = cityblocks.getNextFeature()) != NULL ) {
		counter++;
		char* geo;
		poFeature->GetGeometryRef()->exportToWkt(&geo); //geo destroyed by worker
		OffsetWorker * ow = new OffsetWorker(this, geo, this->offset);
		pool.start(ow);

	}
	pool.waitForDone();
	parcels.syncAlteredFeatures();
}

void GDALOffset::init()
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

std::string GDALOffset::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdaloffset.html";
}

