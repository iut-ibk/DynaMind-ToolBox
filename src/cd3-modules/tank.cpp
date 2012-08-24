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
    addState("TankVolume", &currentvolume);
}

Tank::~Tank() {
}

int Tank::f(ptime time, int dt) {
    (void) time;

    currentvolume.push_back(currentvolume[currentvolume.size()-1]);
    int volumeindex = currentvolume.size()-1;

    currentvolume[volumeindex] += in[0];
    out[0] = 0;
    overflow[0] = 0;

    if(currentvolume[volumeindex] > maxvolume[0]*0.9)
    {
        if((currentvolume[volumeindex] - maxoutflow[0] * dt) > 0)
        {
            currentvolume[volumeindex] -= maxoutflow[0] * dt;
            out[0] = maxoutflow[0] * dt;
        }
        else
        {
            out[0] = currentvolume[volumeindex];
            currentvolume[volumeindex] = 0;
        }
    }

    if(currentvolume[volumeindex] < maxvolume[0]*0.5)
    {
        if((currentvolume[volumeindex] + maxoutflow[0] * dt) <= maxvolume[0])
        {
            currentvolume[volumeindex] += maxoutflow[0] * dt;
            out[0] = -maxoutflow[0] * dt;
        }
        else
        {
            out[0] = -(maxvolume[0] - currentvolume[volumeindex]);
            currentvolume[volumeindex] = maxvolume[0];
        }
    }

    if(currentvolume[volumeindex] > maxvolume[0])
    {
        overflow[0] = currentvolume[volumeindex] - maxvolume[0];
        currentvolume[volumeindex] = maxvolume[0];
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

