#ifndef STORAGESIMULAITON_H
#define STORAGESIMULAITON_H

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


class StorageSimulation: public DM::Module
{
	DM_DECLARE_NODE(StorageSimulation)

	private:
		// Dynamind varaibels
		std::string storage_view_name;
		std::string demand_view_name;
		std::string inflow_attribute_name;
		std::string demand_attribute_name;
		std::string outflow_attribute_name;
		std::string start_date;
		std::string end_date;
		std::string timestep;

		bool start_date_from_global;
		std::string global_viewe_name;
		std::string start_date_name;
		std::string end_date_name;

		DM::ViewContainer demands;
		DM::ViewContainer storages;
		DM::ViewContainer global_object;

		// local cd3 varaibles
		std::string varaibles;
		std::string cd3_end_date;
		std::string cd3_start_date;

		SimulationRegistry *simreg;
		NodeRegistry *nodereg;

		DynaMindStreamLogSink *sink;
		SimulationParameters *p;
		MapBasedModel * m;

		Node * n_d;
		Node * n_r;
		Node * storage;
        Node * flow_p;
		ISimulation * s;

		std::vector<string> storage_volume_tank;
        int storage_volume_tank_single;

		std::vector<double> create_montlhy_values(std::vector<double> daily, int seconds);

		std::vector<double> addVectors(std::vector<double> &vec1, std::vector<double> &vec2);
		std::vector<double> substractVectors(std::vector<double> &vec1, std::vector<double> &vec2);

public:
		StorageSimulation();
		void run();
		void init();
		bool initmodel();
		void clear();

		double createTankOption(OGRFeature * storage_f, double storage_volume, std::vector<double> &inflow,  std::vector<double> &demand);
		Node * addRainTank(double storage_volume, Node* in_flow, Node* nonpot_before);
		Node * addRainwaterTank(Node* flow_probe_runoff, Node* nonpot_before, double storage_volume);

		std::vector<double> mutiplyVector(std::vector<double> & vec, double multiplyer);
};

#endif // STORAGESIMULAITON_H
