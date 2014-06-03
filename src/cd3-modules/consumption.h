#ifndef CONSUMPTION_H
#define CONSUMPTION_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(Consumption)

public:
    Consumption();
    ~Consumption();

    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);
private:
    Flow const_flow_potable;
    Flow const_flow_nonpotable;
    Flow const_flow_sewer;
    Flow outp;
    Flow outnp;
    Flow outs;
    double totalvolume;
};

#endif // CONSUMPTION_H
