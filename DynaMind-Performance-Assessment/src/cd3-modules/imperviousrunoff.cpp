#include "imperviousrunoff.h"

CD3_DECLARE_NODE_NAME(ImperviousRunoff)


ImperviousRunoff::ImperviousRunoff() {

    addInPort(ADD_PARAMETERS(rain_in)); //in mm
    addOutPort(ADD_PARAMETERS(out_sw));
	addParameter(ADD_PARAMETERS(area))
			.setUnit("m^2"); // in m2
	addParameter(ADD_PARAMETERS(loadings))
		.setUnit("g/m^3");

	addState(ADD_PARAMETERS(run_off));
}

ImperviousRunoff::~ImperviousRunoff() {
}

int ImperviousRunoff::f(ptime time, int dt) {
    (void) time;
    out_sw[0] = area * rain_in[0] / 1000.;

	for (int i = 0; i < loadings.size(); i++){
		out_sw[i+1] = loadings[i] * out_sw[0] / 1000.;
	}
	run_off.push_back(out_sw[0]);


    return dt;
}

bool ImperviousRunoff::init(ptime start, ptime end, int dt) {
    return true;
}
