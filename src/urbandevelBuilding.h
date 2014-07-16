#ifndef urbandevelBuilding_H
#define urbandevelBuilding_H

#include <dm.h>
#include <dmcompilersettings.h>

class DM_HELPER_DLL_EXPORT urbandevelBuilding : public DM::Module
{
    DM_DECLARE_NODE(urbandevelBuilding);

private:
    DM::View city;
    DM::View parcelview;
    DM::View buildingview;

    double offset;
    int buildingyear;
    int stories;
    bool onSignal;
    bool paramfromCity;
    bool genPopulation;
    int spacepp;

public:
    urbandevelBuilding();
    void run();
    void init();
    std::string getHelpUrl();
};

#endif // urbandevelBuilding_H
