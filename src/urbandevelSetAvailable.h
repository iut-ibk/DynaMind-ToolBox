#ifndef urbandevelSetAvailable_H
#define urbandevelSetAvailable_H

#include <dmmodule.h>
#include <dm.h>

using namespace DM;
class DM_HELPER_DLL_EXPORT urbandevelSetAvailable : public Module
{
    DM_DECLARE_NODE(urbandevelSetAvailable)
private:
        std::string blocks_name;
        std::string elements_name;
        DM::System * city;
        DM::View blocks_view;
        DM::View elements_view;
        DM::View elements_centroids_view;

public:
    urbandevelSetAvailable();
	void run();
	void init();
};

#endif // urbandevelSetAvailable_H
