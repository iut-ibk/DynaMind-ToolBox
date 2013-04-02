#ifndef REMOVECOMPONENT_H
#define REMOVECOMPONENT_H

#include "dmcompilersettings.h"
#include <dm.h>

class  DM_HELPER_DLL_EXPORT RemoveComponent : public DM::Module
{
    DM_DECLARE_NODE(RemoveComponent)
    private:
        DM::View view_remove;
        std::string remove_name;


public:
    RemoveComponent();
    void init();
    void run();
};

#endif // RETROFITCITYBLOCK_H
