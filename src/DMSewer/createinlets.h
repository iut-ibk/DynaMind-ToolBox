#ifndef CREATEINLETS_H
#define CREATEINLETS_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT CreateInlets : public DM::Module
{
    DM_DECLARE_NODE(CreateInlets)
private:
        DM::View Blocks;
        DM::View Inlets;
        double Size;

        double offsetX;
        double offsetY;
        double with;
        double heigth;
    public:

    CreateInlets();
    void run();
};

#endif // CREATEINLETS_H
