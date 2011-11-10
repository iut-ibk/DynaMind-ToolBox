#ifndef INOUTMODULE_H
#define INOUTMODULE_H

#include "compilersettings.h"
#include "module.h"
using namespace vibens;
class VIBE_HELPER_DLL_EXPORT InOut : public  Module {

VIBe_DECLARE_NODE( InOut )

    public:
        VectorData * vec_in;
        VectorData * vec_out;

        InOut();
        void run();
        virtual ~InOut();
};


#endif // INOUTMODULE_H
