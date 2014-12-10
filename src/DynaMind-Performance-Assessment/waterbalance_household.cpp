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
#include <numeric>

DM_DECLARE_NODE_NAME(WaterBalanceHouseHold,Performance)


WaterBalanceHouseHold::WaterBalanceHouseHold()
{
	this->GDALModule = true;
	this->rainfile = "";
	this->addParameter("rainfile", DM::STRING, &this->rainfile);

	this->evapofile = "";
	this->addParameter("evapofile", DM::STRING, &this->evapofile);

	this->cd3_dir = "";
	this->addParameter("cd3_dir", DM::STRING, &this->cd3_dir);

	this->storage_volume_tank;
	this->addParameter("storage_volume", DM::STRING_LIST, &this->storage_volume_tank);

	parcels = DM::ViewContainer("parcel", DM::COMPONENT, DM::READ);
	parcels.addAttribute("area", DM::Attribute::DOUBLE, DM::READ);
	parcels.addAttribute("persons", DM::Attribute::DOUBLE, DM::READ);

	parcels.addAttribute("roof_area", DM::Attribute::DOUBLE, DM::WRITE);
	parcels.addAttribute("non_potable_demand", DM::Attribute::DOUBLE, DM::WRITE);
	parcels.addAttribute("potable_demand", DM::Attribute::DOUBLE, DM::WRITE);
	parcels.addAttribute("run_off_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	parcels.addAttribute("non_potable_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);

	parcels.addAttribute("building_id", DM::Attribute::INT, DM::READ);
	buildings = DM::ViewContainer("building", DM::COMPONENT, DM::READ);
	buildings.addAttribute("area", DM::Attribute::DOUBLE, DM::READ);
	buildings.addAttribute("persons", DM::Attribute::INT, DM::READ);
//	buildings.addAttribute("income", DM::Attribute::STRING, DM::READ);
//	buildings.addAttribute("education", DM::Attribute::STRING, DM::READ);

	rwhts = DM::ViewContainer("rwht", DM::COMPONENT, DM::WRITE);
	rwhts.addAttribute("volume", DM::Attribute::DOUBLE, DM::WRITE);
	rwhts.addAttribute("storage_behaviour_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	rwhts.addAttribute("provided_volume_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	rwhts.addAttribute("provided_volume_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	rwhts.addAttribute("total_savings", DM::Attribute::DOUBLE, DM::WRITE);
	rwhts.addAttribute("parcel_id", DM::Attribute::INT, DM::WRITE);
	rwhts.addAttribute("construction_costs", DM::Attribute::DOUBLE, DM::WRITE);
	rwhts.addAttribute("annual_cost_savings", DM::Attribute::DOUBLE, DM::WRITE);
	rwhts.addAttribute("persons", DM::Attribute::INT, DM::WRITE);
	rwhts.addAttribute("income", DM::Attribute::STRING, DM::WRITE);
	rwhts.addAttribute("education", DM::Attribute::STRING, DM::WRITE);
	rwhts.addAttribute("household_id", DM::Attribute::INT, DM::WRITE);


	std::vector<DM::ViewContainer*> stream;
	stream.push_back(&parcels);
	stream.push_back(&buildings);
	stream.push_back(&rwhts);
	this->registerViewContainers(stream);

	this->initCD3();
}

void WaterBalanceHouseHold::run()
{
	DM::Logger(DM::Debug) << "Init CD3";
	initmodel();
	DM::Logger(DM::Debug) << "Init CD3 done";
	buildings.createIndex("parcel_id");
	OGRFeature * p;
	this->parcels.resetReading();
	int id;
	while(p = this->parcels.getNextFeature()) {
		id++;
		double roofarea = 0;
		double persons = 0;
		double parcel_area = p->GetFieldAsDouble("area");
		std::string income;
		std::string education;


		buildings.resetReading();
		std::stringstream parcel_query;
		parcel_query << "parcel_id = " << p->GetFID();
		buildings.setAttributeFilter(parcel_query.str().c_str());

		OGRFeature * b;
		while (b = buildings.getNextFeature()) {
			roofarea+=b->GetFieldAsDouble("area");
			persons+= b->GetFieldAsInteger("persons");
			income= b->GetFieldAsString("income");
			education= b->GetFieldAsString("education");

		}
		//DM::Logger(DM::Standard) << "Connected Roof Area " << roofarea;

		double imp_fraction = 0;

		if (roofarea > 0) {
			imp_fraction = roofarea / parcel_area;
		} else {
			continue;
		}

		calculateRunoffAndDemand(parcel_area, imp_fraction, 1.0 - imp_fraction - 0.15, persons);

		Node * flow_probe_runoff = m->getNode("flow_probe_runoff");
		std::vector<double> ro = *(flow_probe_runoff->getState<std::vector<double> >("Flow"));
		DM::DMFeature::SetDoubleList( p, "run_off_monthly", this->create_montlhy_values(ro, 86400));

		Node * nonpot_before = m->getNode("non_pot");
		std::vector<double> ro1 = *(nonpot_before->getState<std::vector<double> >("Flow"));
		DM::DMFeature::SetDoubleList( p, "non_potable_monthly", this->create_montlhy_values(ro1, 86400));

		Node * pot_before = m->getNode("pot");
		std::vector<double> pot_1 = *(pot_before->getState<std::vector<double> >("Flow"));
		double sum = 0;
		for (int i = 0; i < pot_1.size(); i++)
			sum+=pot_1[i];
		p->SetField("potable_demand",sum);

		sum = 0;
		for (int i = 0; i < ro1.size(); i++)
			sum+=ro1[i];
		//std::cout << sum << std::endl;
		p->SetField("non_potable_demand", sum);

		p->SetField("roof_area", roofarea);

		//Create Raintanks
		for (int i = 0; i < this->storage_volume_tank.size(); i++){
			OGRFeature * rwht = rwhts.createFeature();
			double savings = this->createTankOption(rwht, QString::fromStdString(storage_volume_tank[i]).toDouble(),this->stormwater_runoff, this->non_potable_demand);
			rwht->SetField("parcel_id", (int)p->GetFID());
			if (storage_volume_tank[i] == "2")
				rwht->SetField("construction_costs", 2525.0);
			if (storage_volume_tank[i] == "5")
				rwht->SetField("construction_costs", 2985.0);
			if (storage_volume_tank[i] == "10")
				rwht->SetField("construction_costs", 3560.0);

			rwht->SetField("annual_cost_savings", savings * (2.34+1.8));
			rwht->SetField("persons", persons);
			rwht->SetField("income", income.c_str());
			rwht->SetField("education", education.c_str());
			rwht->SetField("household_id",id);

		}

//		delete s;
//		delete m;

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
		QString dance_nodes = QString::fromStdString(this->cd3_dir) + "/libdance4water-nodes";
		nodereg->addNativePlugin(dance_nodes.toStdString());
		try{
			nodereg->addPythonPlugin("/Users/christianurich/Documents/CD3Waterbalance/Module/cd3waterbalancemodules.py");
		}  catch(...) {
			Logger(Error) << "big fat error";

		}

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

bool WaterBalanceHouseHold::calculateRunoffAndDemand(double lot_area,
													 double roof_imp_fra,
													 double perv_area_fra,
													 double persons)
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

	//evapo
	Node * evapo = nodereg->createNode("IxxRainRead_v2");
	if (!evapo) {
		DM::Logger(DM::Error) << "Couldn't create " << " IxxRainRead_v2";
		return false;
	}
	evapo->setParameter("rain_file", this->evapofile);
	evapo->setParameter("datestring", datetime);
	m->addNode("e_1", evapo);

	//catchment
	Node * catchment_w_routing = nodereg->createNode("Catchment_w_Routing");
	if (!rain) {
		DM::Logger(DM::Error) << "Couldn't create " << " Catchment_w_Routing";
		return false;
	}

	catchment_w_routing->setParameter("Catchment_Area_[m^2]", lot_area);
	catchment_w_routing->setParameter("Fraktion_of_Pervious_Area_pA_[-]", perv_area_fra);
	catchment_w_routing->setParameter("Fraktion_of_Impervious_Area_to_Stormwater_Drain_iASD_[-]",1.0 - roof_imp_fra - perv_area_fra);
	catchment_w_routing->setParameter("Fraktion_of_Impervious_Area_to_Reservoir_iAR_[-]",roof_imp_fra);
	catchment_w_routing->setParameter("Outdoor_Demand_Weighing_Factor_[-]", 0.25);
	m->addNode("cw_1", catchment_w_routing);

	m->addConnection(new NodeConnection(rain,"out",catchment_w_routing,"Rain" ));
	m->addConnection(new NodeConnection(evapo,"out",catchment_w_routing,"Evapotranspiration"));

	Node * flow_probe_runoff = nodereg->createNode("FlowProbe");
	m->addNode("flow_probe_runoff", flow_probe_runoff);
	m->addConnection(new NodeConnection(catchment_w_routing,"Collected_Water",flow_probe_runoff,"in" ));

	Node * consumer = this->createConsumer(persons);
	m->addNode("c_1", consumer);

	Node *non_pot_mixer = nodereg->createNode("Mixer");
	m->addNode("non_pot_mixer", non_pot_mixer);

	//non-potable_demand
	Node * nonpot_before = nodereg->createNode("FlowProbe");
	m->addNode("non_pot", nonpot_before);
	m->addConnection(new NodeConnection(non_pot_mixer,"out",nonpot_before,"in" ));

	//potable_demand
	Node * pot_before = nodereg->createNode("FlowProbe");
	m->addNode("pot", pot_before);
	m->addConnection(new NodeConnection(consumer,"out_p",pot_before,"in" ));

	s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
	s->setModel(m);
	s->setSimulationParameters(*p);
	ptime starttime = s->getSimulationParameters().start;

	m->initNodes(s->getSimulationParameters());

	m->addConnection(new NodeConnection(consumer,"out_np",non_pot_mixer,"in_0"));
	m->addConnection(new NodeConnection(catchment_w_routing,"Outdoor_Demand",non_pot_mixer,"in_1"));

	s->start(starttime);

	this->stormwater_runoff = *(flow_probe_runoff->getState<std::vector<double> >("Flow"));
	this->non_potable_demand = *(nonpot_before->getState<std::vector<double> >("Flow"));

	return true;
}

Flow WaterBalanceHouseHold::createConstFlow(double const_flow)
{
	Flow cf;
	cf[0] = const_flow;
	return cf;
}

void WaterBalanceHouseHold::initCD3()
{
	this->m = new MapBasedModel();
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

	consumption->setParameter("const_flow_potable",createConstFlow( (leak_other + washing_machine + taps + shower_bath) ));
	consumption->setParameter("const_flow_nonpotable",createConstFlow(toilet ));

	consumption->setParameter("const_flow_greywater",createConstFlow(0));
	consumption->setParameter("const_flow_sewer",createConstFlow(leak_other + washing_machine + taps + shower_bath + toilet));

	consumption->setParameter("const_flow_stormwater",createConstFlow(0));

	return consumption;
}


std::string WaterBalanceHouseHold::getCd3_dir() const
{
	return cd3_dir;
}

void WaterBalanceHouseHold::setCd3_dir(const std::string &value)
{
	cd3_dir = value;
}


std::string WaterBalanceHouseHold::getEvapofile() const
{
	return evapofile;
}

void WaterBalanceHouseHold::setEvapofile(const std::string &value)
{
	evapofile = value;
}

std::string WaterBalanceHouseHold::getRainfile() const
{
	return rainfile;
}

void WaterBalanceHouseHold::setRainfile(const std::string &value)
{
	rainfile = value;
}
std::vector<double> WaterBalanceHouseHold::create_montlhy_values(std::vector<double> daily, int seconds)
{

	QDateTime start = QDateTime::fromString("00:00:00 01.01.2000", "hh:mm:ss dd.MM.yyyy");
	std::vector<double> monthly;
	double sum = 0;
	int month = start.date().month();
	for (int i = 0; i < daily.size(); i++) {
		QDateTime today = start.addSecs(seconds*i);
		//check if date switched
		if (month == today.date().month()) {
			sum+=daily[i];
			continue;
		}
		month = today.date().month();
		monthly.push_back(sum);
		sum = daily[i];
	}
	monthly.push_back(sum);
	return monthly;
}
std::vector<double> WaterBalanceHouseHold::getNon_potable_demand() const
{
	return non_potable_demand;
}

void WaterBalanceHouseHold::setNon_potable_demand(const std::vector<double> &value)
{
	non_potable_demand = value;
}

std::vector<double> WaterBalanceHouseHold::getStormwater_runoff() const
{
	return stormwater_runoff;
}

void WaterBalanceHouseHold::setStormwater_runoff(const std::vector<double> &value)
{
	stormwater_runoff = value;
}


double WaterBalanceHouseHold::createTankOption(OGRFeature * rwht_f, double storage_volume, std::vector<double> &runoff, std::vector<double> &demand)
{
	delete s;
	delete m;

	//Create new model
	this->initCD3();

	Node * n_d = nodereg->createNode("SourceVector");
	n_d->setParameter("source",demand);
	m->addNode("demand", n_d);

	Node * n_r = nodereg->createNode("SourceVector");
	n_r->setParameter("source",runoff);
	m->addNode("runoff", n_r);

	//RWHT
	Node * rwht = nodereg->createNode("RWHT");
	rwht->setParameter("storage_volume", storage_volume);
	m->addNode("rain_tank", rwht);

	//Stormwater
	m->addConnection(new NodeConnection(n_r,"out",rwht,"in_sw" ));
	m->addConnection(new NodeConnection(n_d,"out",rwht,"in_np" ));

	s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();

	s->setModel(m);
	s->setSimulationParameters(*p);
	ptime starttime = s->getSimulationParameters().start;

	m->initNodes(s->getSimulationParameters());


	s->start(starttime);

	std::vector<double> storage_behaviour =  *(rwht->getState<std::vector<double> >("storage_behaviour"));
	std::vector<double> provided_volume =  *(rwht->getState<std::vector<double> >("provided_volume"));

	double savings = 0;
	for (int i = 0; i < provided_volume.size(); i++){
		savings+=provided_volume[i];
	}

	if (!rwht_f) // for unit testing
		return savings;

	DM::DMFeature::SetDoubleList(rwht_f, "storage_behaviour_daily", storage_behaviour);
	DM::DMFeature::SetDoubleList(rwht_f, "provided_volume_daily", provided_volume);
	DM::DMFeature::SetDoubleList(rwht_f, "provided_volume_monthly", this->create_montlhy_values(provided_volume, 86400));

	rwht_f->SetField("volume", storage_volume);
	rwht_f->SetField("total_savings", savings);

	return savings;

}
