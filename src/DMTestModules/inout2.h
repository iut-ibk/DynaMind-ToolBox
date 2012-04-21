#ifndef INOUT2MODULE_H
#define INOUT2MODULE_H

#include "dmcompilersettings.h"
#include "dmmodule.h"
#include <dm.h>
using namespace DM;

/** @ingroup TestModules
 * @brief The Module addes the Attributes F to Inlets and F to Conduits
 *
 * Data Set:
 * - Inport:
 *      - Read
 *          - Inlets|NODE: A, B, C, D
 *          - Conduits|EDGE
 *      - Add
 *          - Inlets|NODE: F
 *          - Conduits|EDGE: F
 *
 */
class DM_HELPER_DLL_EXPORT InOut2 : public  Module {

DM_DECLARE_NODE( InOut2 )

    private:
         DM::View inlets;
         DM::View conduits;
         DM::View streets;

    public:
        DM::System * sys_in;
        double a;


        InOut2();
          /** @brief Adds to nodes to the Inlets (0,0,2) and (0,0,3) */
        void run();
        virtual ~InOut2();
};


#endif // INOUT2MODULE_H
