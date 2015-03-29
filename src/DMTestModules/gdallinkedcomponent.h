#ifndef GDALLINKEDCOMPONENT_H
#define GDALLINKEDCOMPONENT_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>

/**
 * @brief Create Linked features
 */
class DM_HELPER_DLL_EXPORT GDALLinkedComponent: public DM::Module
{
	DM_DECLARE_NODE(GDALLinkedComponent)
private:
	DM::ViewContainer components;
	DM::ViewContainer linked_components;
	int elements;
	bool append;
	bool addAttributes;

public:
	GDALLinkedComponent();
	void run();
	void init();
};
#endif // GDALLINKEDCOMPONENT_H
