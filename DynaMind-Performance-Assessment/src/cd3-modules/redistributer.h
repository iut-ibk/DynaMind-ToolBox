#ifndef REDISTRIBUTER_H
#define REDISTRIBUTER_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(Redistributer)

public:
	Redistributer();

	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);

	int ports;

private:
	double current_volume;
	double storage_volume;

	std::vector<Flow *> inputs;
	std::vector<std::string> input_names;

	std::vector<Flow *> outputs;
	std::vector<std::string> output_names;

	std::vector<double> loadings;


};

#endif // REDISTRIBUTER_H
