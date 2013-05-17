#ifndef PRETTYPARCELS_H
#define PRETTYPARCELS_H

#include <dm.h>

class DM_HELPER_DLL_EXPORT PrettyParcels : public DM::Module
{
    DM_DECLARE_NODE(PrettyParcels)
private:
    DM::View out;
    DM::View in;
    DM::View bbs;

    int year;
    double length;
    double aspectRatio;
    double offset;

    bool remove_new;
    std::string InputViewName;
    std::string OutputViewName;

public:
    PrettyParcels();

    void run();
    void init();

    void createSubdevision(DM::System * sys,  DM::Face * f, int gen);
    void finalSubdevision(DM::System * sys, DM::Face * f, int gen);
};

#endif // PRETTYPARCELS_H
