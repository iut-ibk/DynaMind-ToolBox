#ifndef GDALPUBLISHRESULTS_H
#define GDALPUBLISHRESULTS_H

#include <dmmodule.h>
#include <dm.h>
#include <ogrsf_frmts.h>


class GDALPublishResults: public DM::Module
{
		DM_DECLARE_NODE(GDALPublishResults)
public:

	GDALPublishResults();
	void run();
	void init();
private:
	DM::ViewContainer components;

	std::string sink;
	std::string driverName;
	std::string viewName;
	std::string layerName;

	void writeFeatures(OGRLayer *lyr, std::vector<OGRFeature *> &feats);
};



#endif // GDALPUBLISHRESULTS_H
