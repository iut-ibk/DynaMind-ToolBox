#ifndef PRETTYPARCELS_H
#define PRETTYPARCELS_H

#include <dm.h>

class DM_HELPER_DLL_EXPORT PrettyParcels : public DM::Module
{
    DM_DECLARE_NODE(PrettyParcels)
private:
    DM::View parcels;
    DM::View cityblocks;
//  DM::View bbs;

    double length;
    double aspectRatio;
    double offset;

    bool FLAG_remove_new;
    std::string InputViewName;
    std::string OutputViewName;

public:
    PrettyParcels();
    void run();

    void init();

    void createSubdivision(DM::System * sys,  DM::Face * f, int gen);
    void finalSubdivision(DM::System * sys, DM::Face * f, int gen);
};

#endif // PRETTYPARCELS_H
