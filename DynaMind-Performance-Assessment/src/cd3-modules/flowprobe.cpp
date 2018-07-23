#include "flowprobe.h"

CD3_DECLARE_NODE_NAME(FlowProbe)


FlowProbe::FlowProbe() {

	element = 0;
	addParameter(ADD_PARAMETERS(element));
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));

	addState("TotalFlow", &totalflow);
	addState("Flow", &flow);
}

FlowProbe::~FlowProbe() {
}

int FlowProbe::f(ptime time, int dt) {
	(void) time;
	totalflow += in[element];
	flow.push_back(in[element]);
	for (size_t i = 0; i < out.size(); i++)
		out[i] = in[i];
	return dt;
}

bool FlowProbe::init(ptime start, ptime end, int dt) {
	totalflow=0;
	flow.clear();
	return true;
}

