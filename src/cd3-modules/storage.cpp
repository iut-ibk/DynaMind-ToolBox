#include "storage.h"

CD3_DECLARE_NODE_NAME(Storage)


Storage::Storage() {

    addInPort(ADD_PARAMETERS(demand_in));
    addState("TotalVolume", &totalvolume);
}

Storage::~Storage() {
}

int Storage::f(ptime time, int dt) {
    (void) time;
    totalvolume += demand_in[0];
    return dt;
}

bool Storage::init(ptime start, ptime end, int dt) {
    totalvolume=0;
    return true;
}
