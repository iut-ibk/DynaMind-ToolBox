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
		std::string evapofile;
		std::string cd3_dir;
		std::vector<double> create_montlhy_values(std::vector<double> daily, int seconds);
		void analyse_raintank();

    public:
        WaterBalanceHouseHold();
        void run();
        void initmodel();
		bool createRaintank(OGRFeature * f,
							double area,
							double roof_imp_fra,
							double perv_area_fra,
							double persons,
							double storage_volume);
        void clear();
        Node *createConsumer(int persons);
        Flow createConstFlow(double const_flow);
		void initCD3();
		std::string getCd3_dir() const;
		void setCd3_dir(const std::string &value);
		std::string getEvapofile() const;
		void setEvapofile(const std::string &value);
		std::string getRainfile() const;
		void setRainfile(const std::string &value);
};

#endif // WATERBALANCE_HOUSEHOLD_H
