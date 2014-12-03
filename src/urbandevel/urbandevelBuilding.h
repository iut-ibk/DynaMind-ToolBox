#ifndef urbandevelBuilding_H
#define urbandevelBuilding_H

#include <dmmodule.h>

class DM_HELPER_DLL_EXPORT urbandevelBuilding : public DM::Module
{
    DM_DECLARE_NODE(urbandevelBuilding);

public:
    urbandevelBuilding();
    ~urbandevelBuilding();
    void run();
    void init();
    std::string getHelpUrl();

private:
    DM::View cityview;
    DM::View parcelview;
    DM::View buildingview;

    double offset;
    int buildingyear;
    int stories;
    bool onSignal;
    bool paramfromCity;
    bool genPopulation;
    int spacepp;
    std::string buildingtype;
};

#endif // urbandevelBuilding_H
