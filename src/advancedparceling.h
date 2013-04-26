#ifndef ADVANCEDPARCELING_H
#define ADVANCEDPARCELING_H

#include <dm.h>

class DM_HELPER_DLL_EXPORT AdvancedParceling : public DM::Module
{
    DM_DECLARE_NODE(AdvancedParceling)
private:
    DM::View parcels;
    DM::View cityblocks;
    DM::View bbs;

    double aspectRatio;
    double length;
    double offset;

    bool remove_new;
    std::string InputViewName;
    std::string OutputViewName;

public:
    AdvancedParceling();
    void run();

    void init();

    void createSubdevision(DM::System * sys,  DM::Face * f, int gen);
    void finalSubdevision(DM::System * sys, DM::Face * f, int gen);
};

#endif // ADVANCEDPARCELING_H
