#ifndef INOUTMODULE_H
#define INOUTMODULE_H

#include "dmcompilersettings.h"
#include "dmmodule.h"
#include "dm.h"

using namespace DM;
class DM_HELPER_DLL_EXPORT InOut : public  Module {

    DM_DECLARE_NODE( InOut )

    private:

        DM::View inlets;
    DM::View conduits;

public:
    //VectorData * vec_in;
    DM::System * sys_in;
    double a;


    InOut();
    void run();
    virtual ~InOut();

};


#endif // INOUTMODULE_H
