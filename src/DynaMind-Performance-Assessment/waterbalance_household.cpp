#include "waterbalance_household.h"
#include "ogrsf_frmts.h"
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
#include <dmgdalhelper.h>

DM_DECLARE_NODE_NAME(WaterBalanceHouseHold,Performance)

std::vector<double> WaterBalanceHouseHold::create_montlhy_values(std::vector<double> daily)
{

	QDate start = QDate::fromString("01.01.2000", "dd.MM.yyyy");
	std::vector<double> monthly;
	double sum = 0;
	int month = start.month();
	for (int i = 0; i < daily.size(); i++) {
		QDate today = start.addDays(i);
		//check if date switched
		if (month == today.month()) {
			sum+=daily[i];
			continue;
		}
		month = today.month();
		monthly.push_back(sum);
		sum = daily[i];
	}
	monthly.push_back(sum);
	return monthly;
}

void WaterBalanceHouseHold::analyse_raintank()
{

}

WaterBalanceHouseHold::WaterBalanceHouseHold()
{
	this->GDALModule = true;
	this->rainfile = "";
	this->addParameter("rainfile", DM::STRING, &this->rainfile);

	this->cd3_dir = "";
	this->addParameter("cd3_dir", DM::STRING, &this->cd3_dir);

	parcels = DM::ViewContainer("parcel", DM::COMPONENT, DM::READ);
	parcels.addAttribute("persons", DM::Attribute::DOUBLE, DM::READ);

	parcels.addAttribute("roof_area", DM::Attribute::DOUBLE, DM::WRITE);
	parcels.addAttribute("non_potable_demand", DM::Attribute::DOUBLE, DM::WRITE);
	parcels.addAttribute("potable_demand", DM::Attribute::DOUBLE, DM::WRITE);
	parcels.addAttribute("run_off_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);

	parcels.addAttribute("building_id", DM::Attribute::INT, DM::READ);
	buildings = DM::ViewContainer("building", DM::COMPONENT, DM::READ);
	buildings.addAttribute("area", DM::Attribute::DOUBLE, DM::READ);
	buildings.addAttribute("persons", DM::Attribute::INT, DM::READ);

	rwhts = DM::ViewContainer("rwht", DM::COMPONENT, DM::WRITE);
	rwhts.addAttribute("storage_behaviour_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	rwhts.addAttribute("provided_volume_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	rwhts.addAttribute("provided_volume_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	rwhts.addAttribute("connected_roof_area", DM::Attribute::DOUBLE, DM::WRITE);

	rwhts.addAttribute("building_id", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> stream;
	stream.push_back(&parcels);
	stream.push_back(&buildings);
	stream.push_back(&rwhts);
	this->registerViewContainers(stream);

	//Init Model
	this->m = new MapBasedModel();
}

void WaterBalanceHouseHold::run()
{
	DM::Logger(DM::Debug) << "Init CD3";
	initmodel();
	DM::Logger(DM::Debug) << "Init CD3 done";
	buildings.createIndex("parcel_id");
	OGRFeature * p;
	this->parcels.resetReading();
	while(p = this->parcels.getNextFeature()) {
		double roofarea = 0;
		double persons = 0;

		buildings.resetReading();
		std::stringstream parcel_query;
		parcel_query << "parcel_id = " << p->GetFID();
		buildings.setAttributeFilter(parcel_query.str().c_str());

		OGRFeature * b;
		while (b = buildings.getNextFeature()) {
			roofarea+=b->GetFieldAsDouble("area");
			persons+= b->GetFieldAsInteger("persons");
		}
		DM::Logger(DM::Standard) << "Connected Roof Area " << roofarea;

		OGRFeature * rwht = rwhts.createFeature();
		createRaintank(rwht, roofarea, persons);

		Node * run_off = m->getNode("ro");
		std::vector<double> ro = *(run_off->getState<std::vector<double> >("run_off"));

		DM::DMFeature::SetDoubleList( p, "run_off_monthly", this->create_montlhy_values(ro));
		Node * pot = m->getNode("s_pot");
		Node * non_pot = m->getNode("s_non_pot");

		double non_potable = *(non_pot->getState<double>("TotalVolume"));
		double potable = *(pot->getState<double>("TotalVolume"));

		p->SetField("non_potable_demand",-non_potable);
		p->SetField("potable_demand", -potable);
		p->SetField("total_roof_area", roofarea);

		Logger(Debug) << "Total Consumption non potable"<< non_potable;
		Logger(Debug) << "Total Consumption  potable"<< potable;

	}

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

	DM::Logger(DM::Debug) << "CD3 simulation finished";
}

bool WaterBalanceHouseHold::createRaintank(OGRFeature * f, double area, double persons)
{
	DM::Logger(DM::Debug) << "Start Raintank";
	Node * rain = nodereg->createNode("IxxRainRead_v2");
	if (!rain) {
		DM::Logger(DM::Error) << "Couldn't create " << " IxxRainRead_v2";
		return false;
	}
	rain->setParameter("rain_file", this->rainfile);
	std::string datetime("d.M.yyyy HH:mm:ss");
	rain->setParameter("datestring", datetime);
	m->addNode("r_1", rain);

	Node *runoff = nodereg->createNode("ImperviousRunoff");
	runoff->setParameter("area", area);
	m->addNode("ro", runoff);

	m->addConnection(new NodeConnection(rain,"out",runoff,"rain_in" ));

	Node * consumer = this->createConsumer(persons);
	m->addNode("c_1", consumer);

	Node * rwht = nodereg->createNode("RWHT");
	rwht->setParameter("storage_volume", 2.5);
	m->addNode("rain_tank", rwht);

	m->addConnection(new NodeConnection(runoff,"out_sw",rwht,"in_sw" ));

	Node *storage_non_p = nodereg->createNode("Storage");
	m->addNode("s_non_pot", storage_non_p);

	Node *storage_pot = nodereg->createNode("Storage");
	m->addNode("s_pot", storage_pot);

	Node *storage_rain = nodereg->createNode("Storage");
	m->addNode("2", storage_rain);

	m->addConnection(new NodeConnection(consumer,"out_p",storage_pot,"in" ));
	m->addConnection(new NodeConnection(consumer,"out_np",rwht,"in_np" ));

	m->addConnection(new NodeConnection(rwht,"out_sw",storage_rain,"in" ));
	m->addConnection(new NodeConnection(rwht,"out_np",storage_non_p,"in" ));


	s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
	s->setModel(m);
	s->setSimulationParameters(*p);
	ptime starttime = s->getSimulationParameters().start;

	m->initNodes(s->getSimulationParameters());
	s->start(starttime);

	std::cout<< *(storage_non_p->getState<double>("TotalVolume")) << std::endl;
	std::cout<< *(storage_pot->getState<double>("TotalVolume")) << std::endl;

	Logger(Error) << "Total Runoff"<< *(storage_rain->getState<double>("TotalVolume"));

	Logger(Debug) << "Dry"<< *(rwht->getState<int>("dry"));
	Logger(Debug) << "Spills"<< *(rwht->getState<int>("spills"));
	storage_behaviour =  *(rwht->getState<std::vector<double> >("storage_behaviour"));

	DM::DMFeature::SetDoubleList(f, "storage_behaviour_daily", storage_behaviour);


	std::vector<double> provided_volume =  *(rwht->getState<std::vector<double> >("provided_volume"));

	DM::DMFeature::SetDoubleList(f, "provided_volume_daily", provided_volume);

	f->SetField("connected_roof_area", area);

	DM::DMFeature::SetDoubleList(f, "provided_volume_monthly", this->create_montlhy_values(provided_volume));

	return true;
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
	double washing_machine = 22. * l_d_to_m_s;
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

