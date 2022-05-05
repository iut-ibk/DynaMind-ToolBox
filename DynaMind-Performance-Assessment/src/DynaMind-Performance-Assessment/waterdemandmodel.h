// #ifndef WATERDEMANDMODEL_H
// #define WATERDEMANDMODEL_H

// #include <dmmodule.h>
// #include <dm.h>
// #include <flow.h>
// #include <vector>

// class MapBasedModel;
// class NodeRegistry;
// class ISimulation;
// class SimulationRegistry;
// class DynaMindStreamLogSink;
// class Node;
// class SimulationParameters;


// class WaterDemandModel: public DM::Module
// {
// 	DM_DECLARE_NODE(WaterDemandModel)

// 	private:
// 		// Dynamind varaibels
// 		std::string start_date;
// 		std::string end_date;
// 		std::string timestep;

// 		bool start_date_from_global;
// 		std::string global_viewe_name;
// 		std::string start_date_name;
// 		std::string end_date_name;

// 		std::string rainfile;
// 		std::string evapofile;

// 		DM::ViewContainer parcels;
// 		DM::ViewContainer global_object;
// 		DM::ViewContainer station;
// 		DM::ViewContainer timeseries;
// 		DM::ViewContainer catchment_parameter;

// 		// local cd3 varaibles
// 		std::string varaibles;
// 		std::string cd3_end_date;
// 		std::string cd3_start_date;

// 		SimulationRegistry *simreg;
// 		NodeRegistry *nodereg;

// 		DynaMindStreamLogSink *sink;
// 		SimulationParameters *p;

// 		bool from_rain_station;
// 		bool to_rain_station;
// 		bool initRain();

// 		std::set<int> station_ids;
// 		std::map<int, std::vector<double> > rainfalls;
// 		std::map<int, std::vector<double> > evaotranspirations;
// 		std::map<int, std::map<std::string, double> > soil_parameters;

// 		std::map<int, std::vector<double> > stormwater_runoff;
// 		std::map<int, std::vector<double> > non_potable_demand;
// 		std::map<int, std::vector<double> > potable_demand;
// 		std::map<int, std::vector<double> > outdoor_demand;
// 		std::map<int, std::vector<double> > grey_water;
// 		std::map<int, std::vector<double> > black_water;

// 		std::map<int, std::vector<double> >infiltration;
// 		std::map<int, std::vector<double> > actual_infiltration;

// 		std::map<int, std::vector<double> > effective_evapotranspiration;
// 		std::map<int, std::vector<double> > previous_storage;
// 		std::map<int, std::vector<double> > groundwater_infiltration;

// 		std::vector<double> create_montlhy_values(std::vector<double> daily, int seconds);

// 		double d_leak_other;
// 		double d_washing_machine;
// 		double d_taps;
// 		double d_toilet;
// 		double d_shower_bath;

// 		double horton_initial_cap;
// 		double horton_final_cap;
// 		double horton_decay_constant;

// 		double perv_soil_storage_capacity;
// 		double daily_recharge_rate;
// 		double transpiration_capacity;

// 		bool catchment_parameter_from_station;

// 		bool createRainInput(MapBasedModel *m);

// 		void loadSoilData();
// public:
// 		WaterDemandModel();
// 		void run();
// 		void init();
// 		bool initmodel();
// 		bool calculateRunoffAndDemand(double area,
// 							double roof_imp_fra,
// 							double perv_area_fra,
// 							double persons);
// 		void clear();
// 		Node *createConsumer(int persons);
// 		Flow createConstFlow(double const_flow);
// 		std::string getEvapofile() const;
// 		void setEvapofile(const std::string &value);
// 		std::string getRainfile() const;
// 		void setRainfile(const std::string &value);
// 		Node * addRainTank(double storage_volume, Node* in_flow, Node* nonpot_before);
// 		Node * addRainwaterTank(Node* flow_probe_runoff, Node* nonpot_before, double storage_volume);

// 		std::vector<double> mutiplyVector(std::vector<double> & vec, double multiplyer);
// };

// #endif // WATERDEMANDMODEL_H
