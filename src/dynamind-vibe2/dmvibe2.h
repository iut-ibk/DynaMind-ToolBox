#ifndef DMDynaMind_H
#define DMDynaMind_H
#include "dmcompilersettings.h"
#include <dmmodule.h>
#include "dmcomponent.h"
#include "dmsystem.h"
#include "dm.h"

/**
*
* @ingroup VIBe2-DynaMind
* @brief Integrates the DynaMind VIBe simulation in the DynaMind Framework
* @author Christian Urich
*
*/
class DM_HELPER_DLL_EXPORT DMVIBe2 : public DM::Module
{
    DM_DECLARE_NODE(DMDynaMind)

    private:
        long height;
        long width;
        double cellsize;

        double maxCPopDensity;
        double maxDCPopDensity;
        double maxOBPopDensity;
        double popChangeLow;
        double popCUFRand;
        double popDCUFRand;
        double popAGRIRand;
        int InitialCityCenter;
        int Steps;
        int PopSteps;



        DM::View landuse;
        DM::View population;
        DM::View topology;

        DM::View mainSewer;
        DM::View wwtp;

        DM::View conduit;
        DM::View junction;

        DM::View globals;

    public:
        DMVIBe2();
    void run();
};

#endif // DMDynaMind_H
