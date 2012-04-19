#ifndef CHECKSHAPEMODULE_H
#define CHECKSHAPEMODULE_H

#include "dmcompilersettings.h"
#include "dmmodule.h"
#include "dm.h"
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
