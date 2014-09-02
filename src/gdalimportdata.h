#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>
#include <ogrsf_frmts.h>



class DM_HELPER_DLL_EXPORT GDALImportData: public DM::Module
{
	DM_DECLARE_NODE(GDALImportData)
private:
	DM::ViewContainer components;

	std::string source;
	std::string layername;
	std::string viewName;
	bool append;
	DM::ViewContainer *initShapefile();

	DM::ViewContainer *filterView;

	DM::ViewContainer *vc;

	OGRDataSource *poDS;

	OGRLayer *initLayer();

	int OGRtoDMGeometry(OGRFeatureDefn * def);
	DM::Attribute::AttributeType OGRToDMAttribute(OGRFieldDefn * fdef);

	bool isFlat;

	int epsg_to;
	int epsg_from; //Needed to set spatial filter


public:
	GDALImportData();
	void run();
	void init();
	virtual ~GDALImportData();
	OGRCoordinateTransformation *getTrafo(int sourceEPSG, int targetEPSG);
	int DMToOGRGeometry(int dm_geometry);
	std::map<std::string, std::string> translator;
};

#endif // IMPORTDATA_H
