#ifndef CREATEBUILDING_H
#define CREATEBUILDING_H

#include <dmmodule.h>

class CreateBuilding: public DM::Module
{
    DM_DECLARE_NODE(CreateBuilding)
public:
    CreateBuilding();
    ~CreateBuilding();

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

#endif // CREATEBUILDING_H
