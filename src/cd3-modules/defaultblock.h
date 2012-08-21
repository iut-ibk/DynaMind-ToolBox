#ifndef DEFAULTBLOCK_H
#define DEFAULTBLOCK_H

#include <node.h>
#include <flow.h>


CD3_DECLARE_NODE(Default)

public:
    Default();
    ~Default();

    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);
private:
    Flow const_flow_potable;
    Flow const_flow_nonpotable;
    Flow const_flow_sewer;
    Flow outp;
    Flow outnp;
    Flow outs;
    Flow generatedvolume;
};

#endif // DEFAULTBLOCK_H
