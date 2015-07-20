#include "gdallogattriubtes.h"
#include "ogr_feature.h"
#include <sstream>
#include <fstream>

DM_DECLARE_CUSTOM_NODE_NAME(GDALLogAttriubtes, Log Attributes , Data Import and Export)

GDALLogAttriubtes::GDALLogAttriubtes()
{
	this->GDALModule = true;

	this->leadingViewName = "";
	this->addParameter("leadingViewName", DM::STRING, &leadingViewName);

	this->attributeNames.clear();
	this->addParameter("attributeNames", DM::STRING_LIST, &attributeNames);

	this->printFeatureID = true;
	this->addParameter("printFeatureID", DM::BOOL, &printFeatureID);

	this->excelFriendly = false;
	this->addParameter("excelFriendly", DM::BOOL, &excelFriendly);

	this->file_name = "";
	this->addParameter("file_name", DM::FILENAME, &file_name);

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
	bool write_to_file = false;
	std::ofstream outfile;
	if (!this->file_name.empty()) {
		write_to_file = true;
		outfile.open(file_name, std::ios_base::app);
	}
	this->leadingView.resetReading();
	OGRFeature * f;
	while (f = this->leadingView.getNextFeature()) {
		if (printFeatureID) {
			DM::Logger(DM::Error) << this->leadingViewName << " " << f->GetFID();
		}
		std::stringstream ss;

		foreach (std::string attr_name, this->attributeNames) {
			int feature_type = leadingView.getAttributeType(attr_name);
			switch (feature_type) {
			case DM::Attribute::INT:
				if (excelFriendly)
					ss << "\t" << attr_name << "\t" << f->GetFieldAsInteger(attr_name.c_str());
				else
					DM::Logger(DM::Error) << "\t" << attr_name << "\t" << f->GetFieldAsInteger(attr_name.c_str());
				break;
			case DM::Attribute::DOUBLE:
				if (excelFriendly)
					ss << "\t" << attr_name << "\t" << f->GetFieldAsDouble(attr_name.c_str());
				else
					DM::Logger(DM::Error) << "\t" << attr_name << "\t" << f->GetFieldAsDouble(attr_name.c_str());
				break;
			case DM::Attribute::STRING:
				if (excelFriendly)
					ss << "\t" << attr_name << "\t" << f->GetFieldAsString(attr_name.c_str());
				else
					DM::Logger(DM::Error) << "\t" << attr_name << "\t" << f->GetFieldAsString(attr_name.c_str());
				break;
			}
		}
		if (excelFriendly)
			DM::Logger(DM::Error) << ss.str();
		if (write_to_file)
			outfile << ss.str() << "\n";
	}
}


