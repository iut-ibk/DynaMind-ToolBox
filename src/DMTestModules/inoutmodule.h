#ifndef INOUTMODULE_H
#define INOUTMODULE_H

#include "dmcompilersettings.h"
#include "dmmodule.h"
#include "dm.h"

using namespace DM;

/** @ingroup TestModules
 * @brief The Module addes the Attributes C to Inlets and D,F to Conduits
 *
 * Data Set:
 * - Inport:
 *      - Read
 *          - Inlets|NODE: A, B
 *          - Conduits|EDGE
 *      - Add
 *          - Inlets|NODE: C
 *          - Conduits|EDGE: D, F
 *
 */
class DM_HELPER_DLL_EXPORT InOut : public  Module {

    DM_DECLARE_NODE( InOut )

    private:

        DM::View inlets;
    DM::View conduits;

public:
    DM::System * sys_in;
    double a;


    InOut();

    /** @brief Adds to nodes to the Inlets (0,0,2) and (0,0,3) */
    void run();
    virtual ~InOut();

};


#endif // INOUTMODULE_H
