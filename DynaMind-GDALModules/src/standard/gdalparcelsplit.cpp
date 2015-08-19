#include "gdalparcelsplit.h"
#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>
#include <QThreadPool>
#include "parcelsplitworker.h"

DM_DECLARE_CUSTOM_NODE_NAME(GDALParcelSplit,Subdivide Parcels, Urban Form)


GDALParcelSplit::GDALParcelSplit()
{
	GDALModule = true;

	this->width = 15;
	this->addParameter("width", DM::DOUBLE, &this->width);

	this->blockName = "cityblock";
	this->addParameter("blockName", DM::STRING, &this->blockName);

	this->subdevisionName = "parcel";
	this->addParameter("subdevisionName", DM::STRING, &this->subdevisionName);

	this->generated = 0;
	this->addParameter("generated", DM::INT, &this->generated);

	this->splitFirst = true;
	this->addParameter("split_first", DM::BOOL, &this->splitFirst);

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);

	counter_added = 0;

}

void GDALParcelSplit::init()
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

string GDALParcelSplit::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalparcelsplit.html";
}


void GDALParcelSplit::run()
{
	cityblocks.resetReading();
	OGRFeature *poFeature;
	this->counter_added = 0;

	QThreadPool pool;
	while( (poFeature = cityblocks.getNextFeature()) != NULL ) {
		char* geo;

		poFeature->GetGeometryRef()->exportToWkt(&geo); //Geo destroyed by worker
		ParcelSplitWorker * ps = new ParcelSplitWorker(this, this->width, this->splitFirst, geo);
		pool.start(ps);
	}
	pool.waitForDone();
	this->generated = counter_added;
}


void GDALParcelSplit::addToSystem(QString poly)
{
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



