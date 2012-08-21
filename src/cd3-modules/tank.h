#ifndef TANK_H
#define TANK_H

#include <node.h>
#include <flow.h>
#include <vector>


CD3_DECLARE_NODE(Tank)

public:
    Tank();
    ~Tank();

    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);
private:
    Flow out;
    Flow in;
    Flow overflow;
    Flow maxvolume;
    Flow maxoutflow;
    std::vector<double> currentvolume;

};

#endif // TANK_H
