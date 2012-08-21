#include "defaultblock.h"
#include <flow.h>

CD3_DECLARE_NODE_NAME(Default)

Default::Default() {
    addParameter(ADD_PARAMETERS(const_flow_potable))
                .setUnit("m^3/s, g/m^3");
    addParameter(ADD_PARAMETERS(const_flow_nonpotable))
                .setUnit("m^3/s, g/m^3");
    addParameter(ADD_PARAMETERS(const_flow_sewer))
                .setUnit("m^3/s, g/m^3");
    addOutPort(ADD_PARAMETERS(outp));
    addOutPort(ADD_PARAMETERS(outnp));
    addOutPort(ADD_PARAMETERS(outs));

    addState("GeneratedVolume", &generatedvolume);
}

Default::~Default() {
}

int Default::f(ptime time, int dt) {
    (void) time;
    generatedvolume[0] += outp[0] + outnp[0] + outs[0];
    return dt;
}

bool Default::init(ptime start, ptime end, int dt) {
    (void) start;
    (void) end;
    outp = const_flow_potable;
    outnp = const_flow_nonpotable;
    outs = const_flow_sewer;

    outp[0] = outp[0] * dt;
    outnp[0] = outnp[0] * dt;
    outs[0] = outs[0] * dt;

    return true;
}

