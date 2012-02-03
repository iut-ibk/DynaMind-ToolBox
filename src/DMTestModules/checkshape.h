#ifndef CHECKSHAPEMODULE_H
#define CHECKSHAPEMODULE_H

#include "compilersettings.h"
#include "module.h"
using namespace DM;
class DM_HELPER_DLL_EXPORT CheckShape : public  Module {

DM_DECLARE_NODE( CheckShape )

    public:
        DM::System * sys_in;

        CheckShape();
        void run();
        virtual ~CheckShape();
};


#endif // CheckShape_H
