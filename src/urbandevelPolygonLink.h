#ifndef urbandevelPolygonLink_H
#define urbandevelPolygonLink_H

#include <dmmodule.h>
#include <dm.h>

using namespace DM;
class DM_HELPER_DLL_EXPORT urbandevelPolygonLink : public Module
{
    DM_DECLARE_NODE(urbandevelPolygonLink)
private:
        std::string blocks_name;
        std::string elements_name;
        DM::System * city;
        DM::View blocks_view;
        DM::View elements_view;
        DM::View elements_centroids_view;

public:
    urbandevelPolygonLink();
	void run();
	void init();
};

#endif // urbandevelPolygonLink_H
