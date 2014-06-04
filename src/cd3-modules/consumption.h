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

    Flow const_flow_greywater;
    Flow const_flow_sewer;

    Flow const_flow_stormwater;

    Flow out_p;
    Flow out_np;

    Flow out_g;
    Flow out_s;

    Flow out_sw;
    double totalvolume;
};

#endif // CONSUMPTION_H
