#include "waterbalance_household.h"

//CD3 includes
#include <node.h>
#include <simulation.h>
#include <cd3assert.h>
#include <mapbasedmodel.h>
#include <log.h>
#include <logger.h>
#include <logsink.h>
#include <noderegistry.h>
#include <simulationregistry.h>
#include <dynamindlogsink.h>
#include <nodeconnection.h>


DM_DECLARE_NODE_NAME(WaterBalanceHouseHold,Performance)

WaterBalanceHouseHold::WaterBalanceHouseHold()
{
	this->rainfile = "";
	this->addParameter("rainfile", DM::STRING, &this->rainfile);

	this->cd3_dir = "";
	this->addParameter("cd3_dir", DM::STRING, &this->cd3_dir);

	parcel = DM::View("PARCEL", DM::COMPONENT, DM::READ);
	parcel.getAttribute("area");
	parcel.getAttribute("BUILDING");
	building = DM::View("BUILDING", DM::COMPONENT, DM::READ);
	building.getAttribute("area");
	//building.addAttribute("water_savings");
	rwht = DM::View("RWHT", DM::COMPONENT, DM::WRITE);
	rwht.addAttribute("storage_behaviour");
	rwht.addLinks("BUILDING", building);

	std::vector<DM::View> stream;
	stream.push_back(parcel);
	stream.push_back(building);
	stream.push_back(rwht);
	this->addData("city", stream);
}

void WaterBalanceHouseHold::run()
{
	DM::System * city = this->getData("city");
    DM::Logger(DM::Standard) << "Init CD3";
    initmodel();


	DM::Component * rwht_cmp = createRaintank();
	city->addComponent(rwht_cmp, this->rwht);
}


void WaterBalanceHouseHold::initmodel()
{
    sink = new DynaMindStreamLogSink();
    Log::init(sink, Error);

    simreg = new SimulationRegistry();
    nodereg = new NodeRegistry();
    s = 0;

    std::map<std::string, Flow::CalculationUnit> flowdef;
    flowdef["Q"]=Flow::flow;
    Flow::undefine();
    Flow::define(flowdef);

    QDir dir("./");

    Logger(Standard) << dir.absolutePath().toStdString();

    try{
		QString dance_nodes = QString::fromStdString(this->cd3_dir) + "/Modules/libdance4water-nodes";
		nodereg->addNativePlugin(dance_nodes.toStdString());
		QString standard_nodes = QString::fromStdString(this->cd3_dir) + "/libnodes";
		nodereg->addNativePlugin(standard_nodes.toStdString());
		simreg->addNativePlugin(standard_nodes.toStdString());

        p = new SimulationParameters();
        p->dt = lexical_cast<int>("86400");
        p->start = time_from_string("2000-Jan-01 00:00:00");
		p->stop = time_from_string("2001-Jan-01 00:00:00");
    }
    catch(...)
    {
        DM::Logger(DM::Error) << "Cannot start CD3 simulation";
    }

    //clear();

	DM::Logger(DM::Debug) << "CD3 simulation finished";
}

DM::Component * WaterBalanceHouseHold::createRaintank()
{
	MapBasedModel m;

	Node * rain = nodereg->createNode("IxxRainRead_v2");
	//std::string rainfile = "/Users/christianurich/Documents/DynaMind-ToolBox/Data/Raindata/melb_rain.ixx";
	rain->setParameter("rain_file", this->rainfile);
	std::string datetime("d.M.yyyy HH:mm:ss");
	rain->setParameter("datestring", datetime);
	m.addNode("r_1", rain);

	Node *runoff = nodereg->createNode("ImperviousRunoff");
	runoff->setParameter("area", 100.);
	std::string ro = "ro";
	m.addNode(ro, runoff);

	m.addConnection(new NodeConnection(rain,"out",runoff,"rain_in" ));
	//m.addConnection(new NodeConnection(runoff,"out_sw",storage_rain,"in" ));

	Node * consumer = this->createConsumer(4);
	m.addNode("c_1", consumer);


	Node * rwht = nodereg->createNode("RWHT");
	rwht->setParameter("storage_volume", 2.5);
	std::string rain_tank = "rain_tank";
	m.addNode(rain_tank, rwht);

	m.addConnection(new NodeConnection(consumer,"out_np",rwht,"in_np" ));
	m.addConnection(new NodeConnection(runoff,"out_sw",rwht,"in_sw" ));

	Node *storage_non_p = nodereg->createNode("Storage");
	m.addNode("1", storage_non_p);

	Node *storage_rain = nodereg->createNode("Storage");
	m.addNode("2", storage_rain);

	m.addConnection(new NodeConnection(rwht,"out_sw",storage_rain,"in" ));
	m.addConnection(new NodeConnection(rwht,"out_np",storage_non_p,"in" ));


	s = simreg->createSimulation(simreg->getRegisteredNames().front());
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
	s->setModel(&m);
	s->setSimulationParameters(*p);
	ptime starttime = s->getSimulationParameters().start;

	m.initNodes(s->getSimulationParameters());
	s->start(starttime);

	Logger(Debug) << "Total Consumption"<< *(storage_non_p->getState<double>("TotalVolume"));
	Logger(Debug) << "Total Runoff"<< *(storage_rain->getState<double>("TotalVolume"));

	Logger(Debug) << "Dry"<< *(rwht->getState<int>("dry"));
	Logger(Debug) << "Spills"<< *(rwht->getState<int>("spills"));
	storage_behaviour =  *(rwht->getState<std::vector<double> >("storage_behaviour"));


	DM::Component * rwht_cmp = new DM::Component();
	DM::Attribute attr = DM::Attribute("storage_behaviour");
	attr.setDoubleVector(storage_behaviour);
	rwht_cmp->addAttribute(attr);

	return rwht_cmp;
}

Flow WaterBalanceHouseHold::createConstFlow(double const_flow)
{
    Flow cf;
    cf[0] = const_flow;

    return cf;
}

Node *WaterBalanceHouseHold::createConsumer(int persons)
{
    Node *consumption = nodereg->createNode("Consumption");
    double l_d_to_m_s = 1./(1000.*60.*60.*24.) * (double) persons;
    double leak_other = 6. *l_d_to_m_s;
    double washing_machine = 22 * l_d_to_m_s;
    double taps = 21. * l_d_to_m_s;
    double toilet = 19. * l_d_to_m_s;
    double shower_bath = 34. * l_d_to_m_s;


    consumption->setParameter("const_flow_potable",createConstFlow( (leak_other + washing_machine + taps + shower_bath) * -1.));
    consumption->setParameter("const_flow_nonpotable",createConstFlow(toilet* -1. ));
    //consumption->setParameter("const_flow_nonpotable",createConstFlow(0 ));

    consumption->setParameter("const_flow_greywater",createConstFlow(0));
    consumption->setParameter("const_flow_sewer",createConstFlow(leak_other + washing_machine + taps + shower_bath + toilet));

    consumption->setParameter("const_flow_stormwater",createConstFlow(0));

    return consumption;
}

