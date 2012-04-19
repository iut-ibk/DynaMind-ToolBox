#ifndef MYMODULE_H
#define MYMODULE_H

#include "dmmodule.h"

using namespace DM;

class DM_HELPER_DLL_EXPORT MyModule : public Module
{
    DM_DECLARE_NODE(MyModule)
    private:
        //List of parameters used in the module
        int MyParameter1;
        string MyParameter2;

    public:
        MyModule();  //Constructor where data and objects for the module are defined
        void run(); //Function executed from the Simulation
};


#endif // MYMODULE_H
