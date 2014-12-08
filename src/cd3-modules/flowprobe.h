#ifndef FLOWPROBE_H
#define FLOWPROBE_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(FlowProbe)

public:
	FlowProbe();
	~FlowProbe();

	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
private:
	Flow in, out;
	double totalflow;
	std::vector<double> flow;
};
#endif // FLOWPROBE_H
