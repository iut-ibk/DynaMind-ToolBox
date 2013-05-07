#ifndef CreateBuilding_H
#define CreateBuilding_H

#include <dm.h>

using namespace DM;
class CreateBuilding : public Module
{
    DM_DECLARE_NODE(CreateBuilding);

private:
    DM::View houses;
    DM::View parcels;
    DM::View building_model;
    DM::View footprint;
    DM::View cityView;

    int stories;
    int length;
    double ratio;
    int buildyear;
    bool DatafromView;

    void createWindows(DM::Face * f, double distance, double width, double height);
public:
    CreateBuilding();
    void run();

};

#endif // CreateBuilding_H
