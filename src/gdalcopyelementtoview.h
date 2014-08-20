#ifndef GDALCOPYELEMENTTOVIEW_H
#define GDALCOPYELEMENTTOVIEW_H

#include <dmmodule.h>
#include <dm.h>
#include <ogrsf_frmts.h>

class GDALCopyElementToView : public DM::Module
{
	DM_DECLARE_NODE(GDALCopyElementToView)
public:
	GDALCopyElementToView();
	void init();
	void run();

private:
	DM::ViewContainer vc_from;
	DM::ViewContainer vc_to;
	std::string fromViewName;
	std::string toViewName;
};


#endif // GDALCOPYELEMENTTOVIEW_H
