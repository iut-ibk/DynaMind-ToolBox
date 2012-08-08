#ifndef RAINDANCE_H
#define RAINDANCE_H

#include <dmmodule.h>
#include <dm.h>

using namespace DM;
class DM_HELPER_DLL_EXPORT RainDance : public Module
{
    DM_DECLARE_NODE(RainDance)
    private:
        std::string ViewRainAdd;
    DM::System * sys_in;
    DM::View v_rain;
    long length;

    public:

    void run();
    void init();

    RainDance();
};

#endif // RAINDANCE_H
