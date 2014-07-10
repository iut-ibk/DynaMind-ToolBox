#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>


enum DRIVERTYPE {
	ShapeFile,
	PostGIS
};


class DM_HELPER_DLL_EXPORT GDALImportData: public DM::Module
{
	DM_DECLARE_NODE(GDALImportData)
private:
	DM::ViewContainer components;

	std::string driverType;
	std::string source;
	std::string layername;
	std::string viewName;
	DM::ViewContainer *initShapefile();

	DM::ViewContainer * vc;

	OGRDataSource *poDS;


	OGRLayer *initLayer();

	int OGRtoDMGeometry(OGRFeatureDefn * def);
	DM::Attribute::AttributeType OGRToDMAttribute(OGRFieldDefn * fdef);



public:
	GDALImportData();
	void run();
	void init();
	virtual ~GDALImportData();
};

#endif // IMPORTDATA_H
