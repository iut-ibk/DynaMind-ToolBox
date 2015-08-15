#ifndef RAINWATERHARVESTINGOPTIONS_H
#define RAINWATERHARVESTINGOPTIONS_H

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


class RainWaterHarvestingOptions: public DM::Module
{
	DM_DECLARE_NODE(RainWaterHarvestingOptions)

	private:
		SimulationRegistry *simreg;
		NodeRegistry *nodereg;

		DynaMindStreamLogSink *sink;
		SimulationParameters *p;
		MapBasedModel * m;

		Node * n_d;
		Node * n_r;
		Node * rwht;
		ISimulation * s;


		DM::ViewContainer parcels;
		DM::ViewContainer rwhts;

		std::vector<string> storage_volume_tank;

		std::vector<double> create_montlhy_values(std::vector<double> daily, int seconds);

		std::vector<double> addVectors(std::vector<double> &vec1, std::vector<double> &vec2);
		std::vector<double> substractVectors(std::vector<double> &vec1, std::vector<double> &vec2);

		std::string rwht_view_name;
		std::string start_date;
		std::string end_date;
		std::string timestep;

public:
		RainWaterHarvestingOptions();
		void run();
		void init();
		bool initmodel();
		void clear();

		double createTankOption(OGRFeature *rwht, double storage_volume, std::vector<double> & runoff, std::vector<double>  & out_doordemand, std::vector<double> &non_potable_demand);
		Node * addRainTank(double storage_volume, Node* in_flow, Node* nonpot_before);
		Node * addRainwaterTank(Node* flow_probe_runoff, Node* nonpot_before, double storage_volume);

		std::vector<double> mutiplyVector(std::vector<double> & vec, double multiplyer);
};

#endif // RAINWATERHARVESTINGOPTIONS_H
