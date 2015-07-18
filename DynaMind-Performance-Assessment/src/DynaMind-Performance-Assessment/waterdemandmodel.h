#ifndef WATERDEMANDMODEL_H
#define WATERDEMANDMODEL_H

#include <dmmodule.h>
#include <dm.h>
#include <flow.h>
#include <vector>

class MapBasedModel;
class NodeRegistry;
class ISimulation;
class SimulationRegistry;
class DynaMindStreamLogSink;
class Node;
class SimulationParameters;


class WaterDemandModel: public DM::Module
{
	DM_DECLARE_NODE(WaterDemandModel)

	private:
		SimulationRegistry *simreg;
		NodeRegistry *nodereg;

		DynaMindStreamLogSink *sink;
		SimulationParameters *p;

		DM::ViewContainer parcels;

		std::string rainfile;
		std::string evapofile;
		std::string cd3_dir;

		std::vector<double> create_montlhy_values(std::vector<double> daily, int seconds);

		std::vector<double> stormwater_runoff;
		std::vector<double> non_potable_demand;
		std::vector<double> potable_demand;
		std::vector<double> outdoor_demand;

	public:
		WaterDemandModel();
		void run();
		void initmodel();
		bool calculateRunoffAndDemand(double area,
							double roof_imp_fra,
							double perv_area_fra,
							double persons);
		void clear();
		Node *createConsumer(int persons);
		Flow createConstFlow(double const_flow);
		std::string getCd3_dir() const;
		void setCd3_dir(const std::string &value);
		std::string getEvapofile() const;
		void setEvapofile(const std::string &value);
		std::string getRainfile() const;
		void setRainfile(const std::string &value);
		Node * addRainTank(double storage_volume, Node* in_flow, Node* nonpot_before);
		Node * addRainwaterTank(Node* flow_probe_runoff, Node* nonpot_before, double storage_volume);
		std::vector<double> getStormwater_runoff() const;
		void setStormwater_runoff(const std::vector<double> &value);
		std::vector<double> getNon_potable_demand() const;
		void setNon_potable_demand(const std::vector<double> &value);

		std::vector<double> mutiplyVector(std::vector<double> & vec, double multiplyer);
};

#endif // WATERDEMANDMODEL_H
