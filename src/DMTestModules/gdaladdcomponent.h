#ifndef GDALADDCOMPONENT_H
#define GDALADDCOMPONENT_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>

/** @ingroup TestModules
 * @brief Creates 1000 nodes for memory testing

 */


class DM_HELPER_DLL_EXPORT GDALAddComponent: public DM::Module
{
	DM_DECLARE_NODE(CreateNodes)
private:
	DM::ViewContainer households;
public:
	GDALAddComponent();
	void run();
};

#endif // GDALADDCOMPONENT_H
