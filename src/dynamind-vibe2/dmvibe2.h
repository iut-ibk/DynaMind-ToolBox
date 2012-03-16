#ifndef DMVIBE2_H
#define DMVIBE2_H
#include "dmcompilersettings.h"
#include <dmmodule.h>
#include "dmcomponent.h"
#include "dmsystem.h"
#include "dm.h"

/**
*
* @ingroup Dynamind-Vibe2
* @brief Integrates the VIBe2 VIBe simulation in the DynaMind Framework
* @author Christian Urich
*
*/
class DM_HELPER_DLL_EXPORT DMVibe2 : public DM::Module
{
    DM_DECLARE_NODE(DMVibe2)

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
        DMVibe2();
    void run();
};

#endif // DMVIBE2_H
