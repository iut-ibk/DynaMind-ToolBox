#include "redistributer.h"

CD3_DECLARE_NODE_NAME(Redistributer)

Redistributer::Redistributer()
{
	addParameter(ADD_PARAMETERS(ports))
			.setUnit("-");
}

int Redistributer::f(ptime time, int dt) {

	double flow_sum = 0;

	for (size_t i = 0; i < ports; i++) {
		flow_sum += (*inputs[i])[0];
	}

	double avg_flow = flow_sum/ports;
	// Mix all exchange water in one pot and mix its concentrations

	//Volume overflow
	double overflow = 0;
	for (size_t j = 0; j < loadings.size(); j++) {
		loadings[j] = 0;
	}

	for (size_t i = 0; i < ports; i++) {
		if (  avg_flow > (*inputs[i])[0]) { // If volume is smaller then the average no water needs to be shifted
			continue;
		}
		double overflow_volume = 0;
		overflow_volume = (*inputs[i])[0] - avg_flow;
		overflow += (*inputs[i])[0] - avg_flow; //This is the volume we need to shift form here
		for (size_t j = 0; j < loadings.size(); j++) {
				loadings[j] += overflow_volume * (*inputs[i])[j+1];
		}
	}
	for (size_t i = 0; i < ports; i++) {
		(*outputs[i])[0] = avg_flow - (*inputs[i])[0];

		//Handle concentraions
		for (size_t j = 0; j < loadings.size(); j++) {
			if (  avg_flow > (*inputs[i])[0]) { // I need to receive some water
				(*outputs[i])[j+1] = loadings[j] / overflow;
			} else {
				(*outputs[i])[j+1] = (*inputs[i])[j+1];
			}
		}
	}
    return dt;
}

bool Redistributer::init(ptime start, ptime end, int dt)
{
	(void) start;
	(void) end;

	if (outputs.size() == ports)
		return true;

	for (size_t i = 0; i < ports; i++) {
		Flow *tmp = new Flow();
		std::ostringstream name;
		name << "out_" << i;
		addOutPort(name.str(), tmp);
		outputs.push_back(tmp);
		output_names.push_back(name.str());
	}

	for (size_t i = 0; i < ports; i++) {
		Flow *tmp = new Flow();
		std::ostringstream name;
		name << "in_" << i;
		addInPort(name.str(), tmp);
		inputs.push_back(tmp);
		input_names.push_back(name.str());
	}

	loadings.clear();

	for (size_t i = 1; i < (*inputs[0]).size(); i++)
		loadings.push_back(0);

	return true;
}
