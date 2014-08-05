#include "nonspatiallinking.h"
#include <sstream>
#include <ogr_feature.h>


DM_DECLARE_NODE_NAME(NonSpatialLinking, GDALModules)

NonSpatialLinking::NonSpatialLinking()

{	this->GDALModule = true;
	this->leadingViewName = "";
	this->addParameter("leadingViewName", DM::STRING, &leadingViewName);
	this->leadingattributeName = "";
	this->addParameter("leadingattributeName", DM::STRING, &leadingattributeName);

	this->joinViewName = "";
	this->addParameter("joinViewName", DM::STRING, &joinViewName);
	this->joinAttributeName = "";
	this->addParameter("joinAttributeName", DM::STRING, &joinAttributeName);
}

void NonSpatialLinking::init()
{
	leadingView = DM::ViewContainer(leadingViewName, DM::COMPONENT, DM::READ);
	leadingView.addAttribute(leadingattributeName, DM::Attribute::INT, DM::READ);

	joinView = DM::ViewContainer(joinViewName, DM::COMPONENT, DM::READ);
	joinView.addAttribute(joinAttributeName, DM::Attribute::INT, DM::READ);

	std::stringstream id_name;
	id_name << leadingViewName << "_id";
	joinView.addAttribute(id_name.str(), DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer * > data_stream;
	data_stream.push_back(&leadingView);
	data_stream.push_back(&joinView);

	this->registerViewContainers(data_stream);

}

void NonSpatialLinking::run()
{
	joinView.resetReading();
	OGRFeature * feat = 0;

	std::stringstream id_name;
	id_name << leadingViewName << "_id";
	leadingView.createIndex(leadingattributeName);
	while (feat = joinView.getNextFeature()) {
		std::stringstream filter;
		filter << leadingattributeName << " = '" << feat->GetFieldAsString(joinAttributeName.c_str()) << "'";
		leadingView.setAttributeFilter(filter.str());
		leadingView.resetReading();
		OGRFeature * feat_lead = 0;
		double id = -1;
		while (feat_lead = leadingView.getNextFeature()) {
			id = feat_lead->GetFID();
		}
		if (id == -1)
			continue;
		feat->SetField(id_name.str().c_str(), id);

	}
}




