#ifndef CHECKSHAPEMODULE_H
#define CHECKSHAPEMODULE_H

#include "compilersettings.h"
#include "module.h"
#include "DM.h"
using namespace DM;
class DM_HELPER_DLL_EXPORT CheckShape : public  Module {

DM_DECLARE_NODE( CheckShape )

private:
    DM::View shape;
    public:
        DM::System * sys_in;

        CheckShape();
        void run();
        virtual ~CheckShape();
};


#endif // CheckShape_H
