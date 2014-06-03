#ifndef STORAGE_H
#define STORAGE_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(Storage)

public:
    Storage();
    ~Storage();

    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);
private:
    Flow demand_in;
    double totalvolume;
};

#endif // STORAGE_H
