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

		DM::View building;
		DM::View parcel;
		DM::View rwht;

		std::vector<double> storage_behaviour;
		std::string rainfile;
		std::string cd3_dir;
		std::vector<double> create_montly_values(std::vector<double> dayly);
		void analyse_raintank();

    public:
        WaterBalanceHouseHold();
        void run();
        void initmodel();
		DM::Component *  createRaintank(double area);
        void clear();
        Node *createConsumer(int persons);
        Flow createConstFlow(double const_flow);
};

#endif // WATERBALANCE_HOUSEHOLD_H
