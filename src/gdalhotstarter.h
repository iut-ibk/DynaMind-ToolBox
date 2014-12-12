#ifndef GDALHOTSTARTER_H
#define GDALHOTSTARTER_H

#include <dmmodule.h>
#include <dm.h>

class OGRLayer;

class GDALHotStarter: public DM::Module
{
	DM_DECLARE_NODE(GDALHotStarter)
private:
		std::string hotStartDatabase;

	DM::ViewContainer *viewContainerFactory(OGRLayer * lyr);
public:
	GDALHotStarter();
	void init();
	void run();


};

#endif // GDALHOTSTARTER_H
