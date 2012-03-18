#ifndef DMDynaMind_H
#define DMDynaMind_H
#include "dmcompilersettings.h"
#include <dmmodule.h>
#include "dmcomponent.h"
#include "dmsystem.h"
#include "dm.h"

/**
*
* @ingroup Dynamind-DynaMind
* @brief Integrates the DynaMind VIBe simulation in the DynaMind Framework
* @author Christian Urich
*
*/
class DM_HELPER_DLL_EXPORT DMDynaMind : public DM::Module
{
    DM_DECLARE_NODE(DMDynaMind)

    private:
        long height;
        long width;
        double cellsize;

        DM::View landuse;
        DM::View population;
        DM::View topology;

        DM::View mainSewer;
        DM::View wwtp;

        DM::View conduit;

    public:
        DMDynaMind();
    void run();
};

#endif // DMDynaMind_H
