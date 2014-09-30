#include "gdallogattriubtes.h"
#include "ogr_feature.h"

DM_DECLARE_NODE_NAME(GDALLogAttriubtes, GDALModules)

GDALLogAttriubtes::GDALLogAttriubtes()
{
	this->GDALModule = true;

	this->leadingViewName = "";
	this->addParameter("leadingViewName", DM::STRING, &leadingViewName);


	this->attributeNames.clear();
	this->addParameter("eraseViewName", DM::STRING_LIST, &attributeNames);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALLogAttriubtes::init()
{
	if (leadingViewName.empty()){
		return;
	}

	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];

	if ( inViews.find(this->leadingViewName) == inViews.end()) {
		DM::Logger(DM::Warning) << "View not found " << this->leadingViewName;
		return;
	}

	this->leadingView = DM::ViewContainer(this->leadingViewName, DM::COMPONENT, DM::MODIFY);

	DM::View view = inViews[this->leadingViewName];
	foreach (std::string attr_name, this->attributeNames) {
		if (!view.hasAttribute(attr_name)) {
			DM::Logger(DM::Warning) << "Attribute name not found " << attr_name;
			return;
		}

		this->leadingView.addAttribute(attr_name, view.getAttributeType(attr_name), DM::READ);
	}

	std::vector<DM::ViewContainer * > data_stream;
	data_stream.push_back(&leadingView);
	this->registerViewContainers(data_stream);
}
void GDALLogAttriubtes::run()
{
	this->leadingView.resetReading();
	OGRFeature * f;
	while (f = this->leadingView.getNextFeature()) {
		DM::Logger(DM::Error) << this->leadingViewName << " " << f->GetFID();
		foreach (std::string attr_name, this->attributeNames) {
			int feature_type = leadingView.getAttributeType(attr_name);
			switch (feature_type) {
			case DM::Attribute::INT:
				DM::Logger(DM::Error) << "\t" << attr_name << "\t" << f->GetFieldAsInteger(attr_name.c_str());
				break;
			case DM::Attribute::DOUBLE:
				DM::Logger(DM::Error) << "\t" << attr_name << "\t" << f->GetFieldAsDouble(attr_name.c_str());
				break;
			case DM::Attribute::STRING:
				DM::Logger(DM::Error) << "\t" << attr_name << "\t" << f->GetFieldAsString(attr_name.c_str());
				break;
			}
		}
	}
}


