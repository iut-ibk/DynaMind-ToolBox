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
    double length;
    double width;
    bool onSignal;
    bool l_on_parcel_b;
    bool yearFromCity;
    bool create3DGeometry;

    void createWindows(DM::Face * f, double distance, double width, double height);
public:
    urbandevelBuilding();
    void run();
    void init();
    std::string getHelpUrl();

};

#endif // CREATESINGLEFAMILYHOUSES_H
