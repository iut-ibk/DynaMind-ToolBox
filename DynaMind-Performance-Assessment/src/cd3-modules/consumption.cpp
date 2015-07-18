#include "consumption.h"

CD3_DECLARE_NODE_NAME(Consumption)

Consumption::Consumption() {
    addParameter(ADD_PARAMETERS(const_flow_potable))
                .setUnit("m^3/s, g/m^3");
    addParameter(ADD_PARAMETERS(const_flow_nonpotable))
                .setUnit("m^3/s, g/m^3");
    addParameter(ADD_PARAMETERS(const_flow_greywater))
                .setUnit("m^3/s, g/m^3");
    addParameter(ADD_PARAMETERS(const_flow_sewer))
                .setUnit("m^3/s, g/m^3");
    addParameter(ADD_PARAMETERS(const_flow_stormwater))
                .setUnit("m^3/s, g/m^3");

    addOutPort(ADD_PARAMETERS(out_p));
    addOutPort(ADD_PARAMETERS(out_np));

    addOutPort(ADD_PARAMETERS(out_g));
    addOutPort(ADD_PARAMETERS(out_s));

    addOutPort(ADD_PARAMETERS(out_sw));


    addState("TotalVolume", &totalvolume);
}

Consumption::~Consumption() {
}

int Consumption::f(ptime time, int dt) {
    (void) time;
    //totalvolume += outp[0] + outnp[0] + outs[0];
    return dt;
}

bool Consumption::init(ptime start, ptime end, int dt) {
    (void) start;
    (void) end;
    out_p = const_flow_potable;
    out_np = const_flow_nonpotable;

    out_g = const_flow_greywater;
    out_s = const_flow_sewer;

    out_sw = const_flow_stormwater;

    out_p[0] = const_flow_potable[0] * dt;
    out_np[0]  = const_flow_nonpotable[0] * dt;

    out_g[0]  = const_flow_greywater[0] * dt;
    out_s[0]  = const_flow_sewer[0] * dt;

    out_sw[0]  = const_flow_stormwater[0] * dt;

    totalvolume=0;
    return true;
}

