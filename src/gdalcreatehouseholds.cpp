#include "gdalcreatehouseholds.h"
#include "ogrsf_frmts.h"
#include <sstream>

DM_DECLARE_NODE_NAME(GDALCreateHouseholds, GDALModules)

GDALCreateHouseholds::GDALCreateHouseholds()
{

	srand (time(NULL));

	GDALModule = true;

	district = DM::ViewContainer("DISTRICT", DM::COMPONENT, DM::READ);
	district.addAttribute("hh", DM::Attribute::INT, DM::READ);

	building = DM::ViewContainer("BUILDING", DM::FACE, DM::READ);
	building.addAttribute("area", DM::Attribute::DOUBLE, DM::READ);
	building.addAttribute("DISTRICT_id", DM::Attribute::INT, DM::READ);

	household = DM::ViewContainer("HOUSEHOLD", DM::NODE, DM::WRITE);
	household.addAttribute("BUILDING_id", DM::Attribute::INT, DM::WRITE);
	household.addAttribute("persons", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> datastream;
	datastream.push_back(&district);
	datastream.push_back(&building);
	datastream.push_back(&household);

	this->registerViewContainers(datastream);
}

void GDALCreateHouseholds::run()
{
	district.resetReading();

	OGRFeature * d = 0;
	building.createIndex("DISTRICT_id");

	while (d = district.getNextFeature()) {
		int d_id = d->GetFID();
		int households = d->GetFieldAsInteger("hh");
		building.resetReading();
		std::stringstream filter;
		filter << "DISTRICT_id = " << d_id;
		building.setAttributeFilter(filter.str());

		OGRFeature * b = 0;
		std::vector<int> building_ids;
		std::vector<OGRPoint> building_centroids;
		while (b = building.getNextFeature()) {
			int b_id = b->GetFID();
			OGRGeometry * geo = b->GetGeometryRef();
			if (!geo)
				continue;
			OGRPoint pt;
			geo->Centroid(&pt);
			building_centroids.push_back(pt);
			building_ids.push_back(b_id);
		}
		//Assign Households
		if(building_ids.size() == 0)
			continue;
		int counter = 0;
		int max_size = building_ids.size();
		while(households > 0) {
			if (counter == max_size)
				counter = 0;
			OGRFeature * h = household.createFeature();
			h->SetField("BUILDING_id", building_ids[counter]);
			h->SetField("persons",rand() % 4 + 1 );
			h->SetGeometry(&building_centroids[counter]);
			counter++;
			households--;
		}
	}

}
