#include "flowprobe.h"

CD3_DECLARE_NODE_NAME(FlowProbe)


FlowProbe::FlowProbe() {

	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));
	addState("TotalFlow", &totalflow);
	addState("Flow", &flow);
}

FlowProbe::~FlowProbe() {
}

int FlowProbe::f(ptime time, int dt) {
	(void) time;
	totalflow += in[0];
	flow.push_back(in[0]);
	out[0] = in[0];
	return dt;
}

bool FlowProbe::init(ptime start, ptime end, int dt) {
	totalflow=0;
	return true;
}

