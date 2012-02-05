#ifndef INOUTMODULE_H
#define INOUTMODULE_H

#include "compilersettings.h"
#include "module.h"
#include "DM.h"

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
