#ifndef RWHT_H
#define RWHT_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(RWHT)

public:
    RWHT();
    ~RWHT();

    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);
private:
    double current_volume;
    double storage_volume;

    Flow in_sw;
    Flow in_np;

    Flow out_sw;
    Flow out_np;

    double total_in;
    double total_out;

    int spills;
    int dry;

};


#endif // RWHT_H
