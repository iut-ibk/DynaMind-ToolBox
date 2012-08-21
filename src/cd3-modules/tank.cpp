#include "tank.h"
#include <flow.h>

CD3_DECLARE_NODE_NAME(Tank)

Tank::Tank() {
    addOutPort(ADD_PARAMETERS(out));
    addOutPort(ADD_PARAMETERS(overflow));
    addInPort(ADD_PARAMETERS(in));

    addParameter(ADD_PARAMETERS(maxoutflow)).setUnit("m^3/s");
    addParameter(ADD_PARAMETERS(maxvolume)).setUnit("m^3");

    addState("TankVolume", &currentvolume);
}

Tank::~Tank() {
}

int Tank::f(ptime time, int dt) {
    (void) time;

    currentvolume.insert(currentvolume.begin(),currentvolume[0]);
    currentvolume[0] += in[0];


    if(currentvolume[0] > maxvolume[0]*0.9)
    {
        if((currentvolume[0] - maxoutflow[0] * dt) > 0)
        {
            currentvolume[0] -= maxoutflow[0] * dt;
            out[0] = maxoutflow[0] * dt;
        }
        else
        {
            out[0] = currentvolume[0];
            currentvolume[0] = 0;
        }
    }
    else
    {
        if((currentvolume[0] + maxoutflow[0] * dt) <= maxvolume[0])
        {
            currentvolume[0] += maxoutflow[0] * dt;
            out[0] = -maxoutflow[0] * dt;
        }
        else
        {
            out[0] = -(maxvolume[0] - currentvolume[0]);
            currentvolume[0] = maxvolume[0];
        }
    }


    if(currentvolume[0] > maxvolume[0])
    {
        overflow[0] = currentvolume[0] - maxvolume[0];
        currentvolume[0] = maxvolume[0];
    }

    return dt;
}

bool Tank::init(ptime start, ptime end, int dt) {
    (void) start;
    (void) end;
    currentvolume.clear();
    currentvolume.push_back(0);
    out[0] = 0;
    overflow[0] = 0;
    return true;
}

