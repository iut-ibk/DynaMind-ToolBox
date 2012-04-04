#ifndef REMOVESTRAHLER_H
#define REMOVESTRAHLER_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT RemoveStrahler : public DM::Module
{
    DM_DECLARE_NODE(RemoveStrahler)
    private:
        DM::View conduits;
        DM::View junctions;
public:
    void run();
    RemoveStrahler();
};

#endif // REMOVESTRAHLER_H
