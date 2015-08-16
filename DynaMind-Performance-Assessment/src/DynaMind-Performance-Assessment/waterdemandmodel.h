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
		// Dynamind varaibels
		std::string start_date;
		std::string end_date;
		std::string timestep;

		bool start_date_from_global;
		std::string global_viewe_name;
		std::string start_date_name;
		std::string end_date_name;

		std::string rainfile;
		std::string evapofile;

		DM::ViewContainer parcels;
		DM::ViewContainer global_object;

		// local cd3 varaibles
		std::string varaibles;
		std::string cd3_end_date;
		std::string cd3_start_date;

		SimulationRegistry *simreg;
		NodeRegistry *nodereg;

		DynaMindStreamLogSink *sink;
		SimulationParameters *p;



		std::vector<double> stormwater_runoff;
		std::vector<double> non_potable_demand;
		std::vector<double> potable_demand;
		std::vector<double> outdoor_demand;

		std::vector<double> create_montlhy_values(std::vector<double> daily, int seconds);



	public:
		WaterDemandModel();
		void run();
		void init();
		bool initmodel();
		bool calculateRunoffAndDemand(double area,
							double roof_imp_fra,
							double perv_area_fra,
							double persons);
		void clear();
		Node *createConsumer(int persons);
		Flow createConstFlow(double const_flow);
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
