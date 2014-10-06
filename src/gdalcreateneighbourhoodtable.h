#ifndef GDALCREATENEIGHBOURHOODTABLE_H
#define GDALCREATENEIGHBOURHOODTABLE_H

#include <dmmodule.h>

class GDALCreateNeighbourhoodTable : public DM::Module
{
	DM_DECLARE_NODE(GDALCreateNeighbourhoodTable)
public:
	GDALCreateNeighbourhoodTable();
	void init();
	void run();



private:
	std::string viewName;
	std::string neighName;

	std::string name_id1;
	std::string name_id2;

	DM::ViewContainer leadingView;
	DM::ViewContainer ngView;
};

#endif // GDALCREATENEIGHBOURHOODTABLE_H
