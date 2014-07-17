#ifndef GDALADDCOMPONENT_H
#define GDALADDCOMPONENT_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>

/**
 * @brief Creates 1000 nodes for memory testing

 */


class DM_HELPER_DLL_EXPORT GDALAddComponent: public DM::Module
{
	DM_DECLARE_NODE(CreateNodes)
private:
	DM::ViewContainer components;
	int elements;
	bool append;
public:
	GDALAddComponent();
	void run();
	void init();
};

#endif // GDALADDCOMPONENT_H
