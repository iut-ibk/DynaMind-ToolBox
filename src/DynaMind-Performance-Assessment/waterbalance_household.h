#ifndef WATERBALANCE_HOUSEHOLD_H
#define WATERBALANCE_HOUSEHOLD_H


#include <dmmodule.h>
#include <dm.h>
#include <flow.h>

class MapBasedModel;
class NodeRegistry;
class ISimulation;
class SimulationRegistry;
class DynaMindStreamLogSink;
class Node;
class SimulationParameters;


class WaterBalanceHouseHold: public DM::Module
{
    DM_DECLARE_NODE(WaterBalanceHouseHold)

    private:
        SimulationRegistry *simreg;
        NodeRegistry *nodereg;
        ISimulation *s;
        DynaMindStreamLogSink *sink;
        SimulationParameters *p;
    public:
        WaterBalanceHouseHold();
        void run();
        void initmodel();
        void clear();
        Node *createConsumer(int persons);
        Flow createConstFlow(double const_flow);
};

#endif // WATERBALANCE_HOUSEHOLD_H
