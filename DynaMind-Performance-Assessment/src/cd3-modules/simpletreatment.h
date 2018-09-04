#ifndef SIMPLETREATMENT_H
#define SIMPLETREATMENT_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(SimpleTreatment)

public:
    SimpleTreatment();

    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);
	std::vector<double> treated;

private:
    double removal_fraction;
    Flow in, out;
};

#endif // SIMPLETREATMENT_H
