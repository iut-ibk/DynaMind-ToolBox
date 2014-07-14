#ifndef PARCELING_H
#define PARCELING_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>


class DM_HELPER_DLL_EXPORT GDALParceling: public DM::Module
{
	DM_DECLARE_NODE(GDALParceling)
private:
	DM::ViewContainer cityblocks;
	DM::ViewContainer parcels;


public:
	GDALParceling();
	void run();

};

#endif // PARCELING_H
