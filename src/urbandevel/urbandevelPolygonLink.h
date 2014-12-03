#ifndef urbandevelPolygonLink_H
#define urbandevelPolygonLink_H

#include <dmmodule.h>

class DM_HELPER_DLL_EXPORT urbandevelPolygonLink : public DM::Module
{
    DM_DECLARE_NODE(urbandevelPolygonLink)

public:
    urbandevelPolygonLink();
    ~urbandevelPolygonLink();

	void run();
	void init();

    std::string getHelpUrl();

private:
    DM::View blockview;
    DM::View elementview;
    DM::View elementcentroidview;

    std::string blockview_name;
    std::string elementview_name;
};

#endif // urbandevelPolygonLink_H
