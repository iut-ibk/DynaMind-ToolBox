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
		MapBasedModel * m;

		DM::ViewContainer buildings;
		DM::ViewContainer parcels;
		DM::ViewContainer rwhts;

		std::vector<double> storage_behaviour;
		std::string rainfile;
		std::string cd3_dir;
		std::vector<double> create_montlhy_values(std::vector<double> daily);
		void analyse_raintank();

    public:
        WaterBalanceHouseHold();
        void run();
        void initmodel();
		bool createRaintank(OGRFeature *f, double area, double persons);
        void clear();
        Node *createConsumer(int persons);
        Flow createConstFlow(double const_flow);
};

#endif // WATERBALANCE_HOUSEHOLD_H
