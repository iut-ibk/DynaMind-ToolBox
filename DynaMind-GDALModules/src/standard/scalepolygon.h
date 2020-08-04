#ifndef SCALEPOLYGON_H
#define SCALEPOLYGON_H

#include <dmmodule.h>
#include <dm.h>

#include <dmviewcontainer.h>

class DM_HELPER_DLL_EXPORT ScalePolygon: public DM::Module
{
	DM_DECLARE_NODE(ScalePolygon)
private:
	DM::ViewContainer cityblocks;
	DM::ViewContainer parcels;
	int counter_added;


	std::string blockName;
	std::string subdevisionName;


	double scale;

public:
	ScalePolygon();
	void run();
	void init();
	string getHelpUrl();

};

#endif // SCALEPOLYGON_H
