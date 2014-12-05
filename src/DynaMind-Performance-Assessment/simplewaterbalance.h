#ifndef SIMPLEWATERBALANCE_H
#define SIMPLEWATERBALANCE_H

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


class SimpleWaterBalance : public DM::Module
{
	DM_DECLARE_NODE(SimpleWaterBalance)
private:
	SimulationRegistry *simreg;
	NodeRegistry *nodereg;
	ISimulation *s;
	DynaMindStreamLogSink *sink;
	SimulationParameters *p;
	MapBasedModel * m;

	DM::ViewContainer households;
	DM::ViewContainer buildings;
	DM::ViewContainer parcels;
	//DM::ViewContainer rwhts;

	std::vector<double> storage_behaviour;
	std::string rainfile;
	std::string cd3_dir;
	std::vector<double> create_montlhy_values(std::vector<double> daily);

	Node *createHousehold(int persons);
public:

	SimpleWaterBalance();

	void run();
	void initmodel();
	bool createCatchment(OGRFeature * parcel, double persons);
	void clear();
	Flow createConstFlow(double const_flow);
};

#endif // SIMPLEWATERBALANCE_H
