#ifndef SINGLEPOINT_H
#define SINGLEPOINT_H

#include <dm.h>
#include <dmmodule.h>

class DM_HELPER_DLL_EXPORT SinglePoint: public DM::Module
{
    DM_DECLARE_NODE(SinglePoint)
private:
        double x;
        double y;
        double z;
        std::string viewname;
        bool appendToExisting;
public:
    SinglePoint();
    void run();
    void init();
};

#endif // SINGLEPOINT_H
