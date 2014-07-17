#ifndef GDALADDCOMPONENTVIEWCONTAINER_H
#define GDALADDCOMPONENTVIEWCONTAINER_H
#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>



class DM_HELPER_DLL_EXPORT GDALAddComponentViewContainer: public DM::Module
{
	DM_DECLARE_NODE(GDALAddComponentViewContainer)
private:
	DM::ViewContainer components;
	int elements;
	bool append;
public:
	GDALAddComponentViewContainer();
	void run();
	void init();
};
#endif // GDALADDCOMPONENTVIEWCONTAINER_H
