#include "gdalcopyelementtoview.h"
#include <sstream>
DM_DECLARE_NODE_NAME(GDALCopyElementToView, GDALModules)

typedef std::map<std::string, DM::View> viewmap;

GDALCopyElementToView::GDALCopyElementToView()
{
	GDALModule = true;

	fromViewName = "";
	this->addParameter("from_view", DM::STRING, &fromViewName);

	toViewName = "";
	this->addParameter("to_view", DM::STRING, &toViewName);

	add_link = false;
	this->addParameter("add_link", DM::BOOL, &add_link);

	copy_attributes = false;
	this->addParameter("copy_attributes", DM::BOOL, &copy_attributes);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALCopyElementToView::init()
{
	if (fromViewName.empty() || toViewName.empty()) {
		DM::Logger(DM::Error) << "from or to view name not defined";
		return;
	}
	viewmap inViews = getViewsInStream()["city"];


	if (inViews.find(fromViewName) == inViews.end()) {
		DM::Logger(DM::Error) << fromViewName << " not found";
		return;
	}



	this->vc_from = DM::ViewContainer(fromViewName, inViews[fromViewName].getType(),DM::READ );
	this->vc_to = DM::ViewContainer(toViewName, inViews[fromViewName].getType(),DM::WRITE );

	if (add_link) {
		std::stringstream link_name_ss;
		link_name_ss << fromViewName << "_id";
		link_name = link_name_ss.str();
		this->vc_to.addAttribute(link_name, fromViewName, DM::WRITE);

	}

	if (copy_attributes) {
		DM::View v = inViews[fromViewName];

		foreach (std::string attribtue_name, v.getAllAttributes()) {
			this->vc_to.addAttribute(attribtue_name, v.getAttributeType(attribtue_name), DM::WRITE );
			this->vc_from.addAttribute(attribtue_name, v.getAttributeType(attribtue_name), DM::READ );
		}
	}


	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&vc_from);
	data_stream.push_back(&vc_to);

	this->registerViewContainers(data_stream);
}

void GDALCopyElementToView::run()
{
	this->vc_from.resetReading();

	OGRFeature * f;
	while (f = vc_from.getNextFeature()) {
		OGRFeature * f_new = vc_to.createFeature();
		if (add_link) {
			f_new->SetField(link_name.c_str(), (int) f->GetFID());
		}

		if (copy_attributes) {
			foreach(std::string attribute_name, vc_from.getAllAttributes()) {
				OGRField * field = f->GetRawFieldRef(f->GetFieldIndex(attribute_name.c_str()));
				f_new->SetField(attribute_name.c_str(), field);
			}
		}

		OGRGeometry * geo = f->GetGeometryRef();
		if (!geo)
			continue;

		f_new->SetGeometry(geo);


	}
}
