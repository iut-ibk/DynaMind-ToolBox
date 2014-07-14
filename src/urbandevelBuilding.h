#ifndef urbandevelBuilding_H
#define urbandevelBuilding_H

#include <dm.h>
#include <dmcompilersettings.h>

using namespace DM;
class DM_HELPER_DLL_EXPORT urbandevelBuilding : public Module
{
    DM_DECLARE_NODE(urbandevelBuilding);

private:
    DM::View houses;
    DM::View parcels;
    DM::View building_model;
    DM::View footprint;
    DM::View cityView;

    int buildingyear;
    int stories;
    double width;
    double ratio;
    bool onSignal;
    bool rotate90;
    bool paramfromCity;
    bool createPopulation;
    bool create3DGeometry;

    void createWindows(DM::Face * f, double distance, double width, double height);
public:
    urbandevelBuilding();
    void run();
    void init();
    std::string getHelpUrl();

};

#endif // CREATESINGLEFAMILYHOUSES_H
