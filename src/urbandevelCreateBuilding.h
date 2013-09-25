#ifndef urbandevelCreateBuilding_H
#define urbandevelCreateBuilding_H

#include <dmmodule.h>

class urbandevelCreateBuilding: public DM::Module
{
    DM_DECLARE_NODE(urbandevelCreateBuilding)
public:
    urbandevelCreateBuilding();
    ~urbandevelCreateBuilding();

    void run();
    void init();
private:
    int length;
    double ratio;
    int stories;
    int year;

    DM::View buildings;
    DM::View parcels;
};

#endif // urbandevelCreateBuilding_H
