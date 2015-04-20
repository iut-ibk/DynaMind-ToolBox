#include "nonspatiallinking.h"
#include <sstream>
#include <ogr_feature.h>

typedef std::map<std::string, DM::View> viewmap;
DM_DECLARE_NODE_NAME(NonSpatialLinking, GDALModules)

NonSpatialLinking::NonSpatialLinking()

{	this->GDALModule = true;
	this->leadingViewName = "";
	this->addParameter("leading_view", DM::STRING, &leadingViewName);
	this->leadingattributeName = "";
	this->addParameter("leading_attribute", DM::STRING, &leadingattributeName);

	this->joinViewName = "";
	this->addParameter("join_view", DM::STRING, &joinViewName);
	this->joinAttributeName = "";
	this->addParameter("join_attribute", DM::STRING, &joinAttributeName);

	this->alternative_link_name = "";
	this->addParameter("alternative_link_name", DM::STRING, &alternative_link_name);

	this->fullJoin = false;
	this->addParameter("full_join", DM::BOOL, &fullJoin);
}

void NonSpatialLinking::init()
{
	leadingView = DM::ViewContainer(leadingViewName, DM::COMPONENT, DM::READ);
	leadingView.addAttribute(leadingattributeName, DM::Attribute::INT, DM::READ);

	joinView = DM::ViewContainer(joinViewName, DM::COMPONENT, DM::READ);
	joinView.addAttribute(joinAttributeName, DM::Attribute::INT, DM::READ);

	if(!fullJoin) {
		std::stringstream id_name;
		if (alternative_link_name.empty()) {
			id_name << leadingViewName << "_id";
			joinView.addAttribute(id_name.str(), DM::Attribute::INT, DM::WRITE);
		} else {
			joinView.addAttribute(alternative_link_name, DM::Attribute::INT, DM::WRITE);
		}
	} else {
		viewmap inViews = getViewsInStream()["city"];
		if (inViews.find(leadingViewName) == inViews.end()) {
			DM::Logger(DM::Warning) << leadingViewName << " not found in data stream";
		} else {
			DM::View v = inViews[leadingViewName];
			foreach(std::string attr_name, v.getAllAttributes()) {
				joinView.addAttribute(attr_name, v.getAttributeType(attr_name), DM::WRITE);
			}
			this->attribute_names = v.getAllAttributes();
		}
	}

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
	if (alternative_link_name.empty()) {
		id_name << leadingViewName << "_id";
	} else {
		id_name << this->alternative_link_name;
	}

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
			if (id == -1)
				continue;
			if (!fullJoin) {
				feat->SetField(id_name.str().c_str(), id);
				continue;
			}
			foreach(std::string attr_name, attribute_names) {
				int index_lead = feat_lead->GetFieldIndex(attr_name.c_str());
				int index = feat->GetFieldIndex(attr_name.c_str());
				//std::cout <<  attr_name << " " << index_lead << " " << index << std::endl;
				OGRField * field = feat_lead->GetRawFieldRef(index_lead);
				if (field)
					feat->SetField(index, field);
			}

		}
	}
}




