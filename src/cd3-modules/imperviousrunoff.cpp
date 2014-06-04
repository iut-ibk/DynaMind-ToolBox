#include "imperviousrunoff.h"

CD3_DECLARE_NODE_NAME(ImperviousRunoff)


ImperviousRunoff::ImperviousRunoff() {

    addInPort(ADD_PARAMETERS(rain_in)); //in mm
    addOutPort(ADD_PARAMETERS(out_sw));
    addParameter(ADD_PARAMETERS(area)); // in m2
}

ImperviousRunoff::~ImperviousRunoff() {
}

int ImperviousRunoff::f(ptime time, int dt) {
    (void) time;
    out_sw[0] = area * rain_in[0] / 1000.;
    return dt;
}

bool ImperviousRunoff::init(ptime start, ptime end, int dt) {
    return true;
}
