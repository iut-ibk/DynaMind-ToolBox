#ifndef GDALCREATERECTANGLE_H
#define GDALCREATERECTANGLE_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALCreateRectangle:  public DM::Module
{
	DM_DECLARE_NODE(GDALCreateRectangle)
private:
	std::string viewName;
	DM::ViewContainer view;
	double width;
	double length;

public:
	GDALCreateRectangle();
	void init();
	void run();
};

#endif // GDALCREATERECTANGLE_H
