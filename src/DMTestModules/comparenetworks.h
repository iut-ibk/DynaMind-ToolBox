#ifndef COMPARENETWORKSMODULE_H
#define COMPARENETWORKSMODULE_H

#include "compilersettings.h"
#include "module.h"
using namespace DM;
class VIBE_HELPER_DLL_EXPORT CompareNetworks : public  Module {

VIBe_DECLARE_NODE( CompareNetworks )

    public:
        DM::System *n1, *n2;
        std::string n1i, n2i;

        CompareNetworks();
        void run();
        virtual ~CompareNetworks();
};


#endif // CheckShape_H
