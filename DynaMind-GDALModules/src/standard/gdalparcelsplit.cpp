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

	this->target_length = 0;
	this->addParameter("target_length", DM::DOUBLE, &this->target_length);

	this->paramter_from_linked_view = "";
	this->addParameter("paramter_from_linked_view", DM::STRING, &this->paramter_from_linked_view);

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

	if (!this->paramter_from_linked_view.empty()) {
		this->linked_view = DM::ViewContainer(this->paramter_from_linked_view, DM::COMPONENT, DM::READ);
		this->linked_view.addAttribute("target_length", DM::Attribute::DOUBLE, DM::READ);
		this->linked_view.addAttribute("width", DM::Attribute::DOUBLE, DM::READ);

		std::stringstream ss_link_view_id;
		ss_link_view_id << this->paramter_from_linked_view << "_id";
		this->link_view_id = ss_link_view_id.str();

		this->cityblocks.addAttribute(this->link_view_id, DM::Attribute::LINK, DM::READ);
		this->parcels.addAttribute(this->link_view_id, DM::Attribute::LINK, DM::WRITE);

		views.push_back(&linked_view);
	}

	this->registerViewContainers(views);
}

string GDALParcelSplit::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalparcelsplit.html";
}


void GDALParcelSplit::run()
{

	std::map<int, std::map<std::string, double> > templates;
	OGRFeature *poFeature;
	if (!this->paramter_from_linked_view.empty()) {
		this->linked_view.resetReading();
		while ( (poFeature = linked_view.getNextFeature()) ) {
			std::map<std::string, double> params;
			params["target_length"] = poFeature->GetFieldAsDouble("target_length");
			params["width"] = poFeature->GetFieldAsDouble("width");
			templates[poFeature->GetFID()] = params;
		}
	}

	this->counter_added = 0;

	cityblocks.resetReading();
	QThreadPool pool;

	while( (poFeature = cityblocks.getNextFeature())  ) {
		char* geo;
		poFeature->GetGeometryRef()->exportToWkt(&geo); //Geo destroyed by worker

		double w = this->width;
		double tl= this->target_length;
		int link_id = 0;

		if (!this->paramter_from_linked_view.empty()) {
		    link_id = poFeature->GetFieldAsInteger(this->link_view_id.c_str());
		    // DM::Logger(DM::Standard) << (int) poFeature->GetFID() << "/" << link_id;

			if (link_id == 0) // If field returns zero the no template has been set
			    continue;

			//Check if link ID exists
            std::map<int, std::map<std::string, double> >::const_iterator it = templates.find(link_id);
            if (it == templates.end()) {
                DM::Logger(DM::Warning) << "Template " << link_id << "not found" << "for " << (int) poFeature->GetFID();
                continue;
            }

            w = templates[link_id]["width"];
			tl = templates[link_id]["target_length"];
		}

        // DM::Logger(DM::Standard) << (int) poFeature->GetFID() << "/split";

		auto *ps = new ParcelSplitWorker(
					poFeature->GetFID(),
					this,
					w,
					tl,
					this->splitFirst,
					geo,
					2,
                    link_id);

		pool.start(ps);
	}
	pool.waitForDone();
	this->generated = counter_added;
}


void GDALParcelSplit::addToSystem(QString poly, int link_id)
{
	QMutexLocker ml(&mMutex);

	std::string wkt = poly.toStdString();

	char * writable_wr = new char[wkt.size() + 1]; //Note not sure if memeory hole?
	std::copy(wkt.begin(), wkt.end(), writable_wr);
	writable_wr[wkt.size()] = '\0';

	OGRGeometry * ogr_poly;

	OGRErr err = OGRGeometryFactory::createFromWkt(&writable_wr, 0, &ogr_poly);
	//delete writable_wr;
	if (err != OGRERR_NONE) {
		DM::Logger(DM::Warning) << "Geometry is not valid!";
		return;
	}


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
    if (!this->paramter_from_linked_view.empty()) {
        parcel->SetField(this->link_view_id.c_str(), link_id);
    }
	OGRGeometryFactory::destroyGeometry(ogr_poly);

	counter_added++;
}



