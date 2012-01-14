#ifndef INOUTMODULE_H
#define INOUTMODULE_H

#include "compilersettings.h"
#include "module.h"
using namespace DM;
class VIBE_HELPER_DLL_EXPORT InOut : public  Module {

VIBe_DECLARE_NODE( InOut )

    public:
        //VectorData * vec_in;
        DM::System * sys_in;
        double a;


        InOut();
        void run();
        virtual ~InOut();
};


#endif // INOUTMODULE_H
