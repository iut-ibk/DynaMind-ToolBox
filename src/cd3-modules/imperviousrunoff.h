#ifndef IMPERVIOUSRUNOFF_H
#define IMPERVIOUSRUNOFF_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(ImperviousRunoff)

public:
    ImperviousRunoff();
    ~ImperviousRunoff();

    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);
private:
    Flow rain_in;
    Flow out_sw;
    double area;
};


#endif // IMPERVIOUSRUNOFF_H
