#include "gdalcreatecentroids.h"
#include <sstream>
#include <ogrsf_frmts.h>

DM_DECLARE_NODE_NAME(GDALCreateCentroids, GDALModules)

GDALCreateCentroids::GDALCreateCentroids()
{
	GDALModule = true;

	leadingViewName = "";
	this->addParameter("leading_view", DM::STRING, &this->leadingViewName);

	centroidViewName = "";
	this->addParameter("centroid_view", DM::STRING, &this->centroidViewName);

}
void GDALCreateCentroids::init()
{
	if(leadingViewName.empty() || centroidViewName.empty())
		return;

	leadingView = DM::ViewContainer(leadingViewName, DM::FACE, DM::READ);
	centroidView = DM::ViewContainer(centroidViewName, DM::NODE, DM::WRITE);

	std::stringstream ss_link_name;
	ss_link_name << leadingViewName << "_id";

	link_name = ss_link_name.str();
	centroidView.addAttribute(link_name, leadingViewName, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&leadingView);
	data_stream.push_back(&centroidView);

	this->registerViewContainers(data_stream);

}


void GDALCreateCentroids::run()
{
	leadingView.resetReading();
	OGRFeature * f;
	DM::Logger(DM::Debug) << link_name;
	while (f = leadingView.getNextFeature()) {
		OGRGeometry * geo = f->GetGeometryRef();
		if (!geo) {
			DM::Logger(DM::Warning) << f->GetFID() << "Geometry is not valid";
		}
		OGRFeature * f_c = centroidView.createFeature();
		OGRPoint p;
		geo->Centroid(&p);
		f_c->SetGeometry(&p);
		int id = (int) f->GetFID();
		f_c->SetField(link_name.c_str(), id);
	}
}

