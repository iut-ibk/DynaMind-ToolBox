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

	QDateTime start = QDateTime::fromString("00:00:00 01.09.2000", "hh:mm:ss dd.MM.yyyy");
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

void WaterBalanceHouseHold::analyse_raintank()
{

}

WaterBalanceHouseHold::WaterBalanceHouseHold()
{
	this->GDALModule = true;
	this->rainfile = "";
	this->addParameter("rainfile", DM::STRING, &this->rainfile);

	this->evapofile = "";
	this->addParameter("evapofile", DM::STRING, &this->evapofile);

	this->cd3_dir = "";
	this->addParameter("cd3_dir", DM::STRING, &this->cd3_dir);

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
	while(p = this->parcels.getNextFeature()) {
		double roofarea = 0;
		double persons = 0;
		double parcel_area = p->GetFieldAsDouble("area");

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
		double imp_fraction = 0;

		if (roofarea > 0) {
			imp_fraction = roofarea / parcel_area;
		}
		std::cout << roofarea << "/" << parcel_area << std::endl;
		createRaintank(rwht, parcel_area, imp_fraction, 1.0 - imp_fraction - 0.15, persons, 2.5);

		Node * flow_probe_runoff = m->getNode("flow_probe_runoff");
		std::vector<double> ro = *(flow_probe_runoff->getState<std::vector<double> >("Flow"));
		DM::DMFeature::SetDoubleList( p, "run_off_monthly", this->create_montlhy_values(ro, 86400));

		Node * nonpot_before = m->getNode("nonpot_before");
		std::vector<double> ro1 = *(nonpot_before->getState<std::vector<double> >("Flow"));
		DM::DMFeature::SetDoubleList( p, "non_potable_monthly", this->create_montlhy_values(ro1, 86400));

		Node * pot = m->getNode("s_pot");
		Node * non_pot = m->getNode("s_non_pot");

		double non_potable = *(non_pot->getState<double>("TotalVolume"));
		double potable = *(pot->getState<double>("TotalVolume"));

		p->SetField("non_potable_demand",non_potable);
		p->SetField("potable_demand", potable);
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
		QString dance_nodes = QString::fromStdString(this->cd3_dir) + "/libdance4water-nodes";
		nodereg->addNativePlugin(dance_nodes.toStdString());
		try{
		nodereg->addPythonPlugin("/Users/christianurich/Documents/CD3Waterbalance/Module/cd3waterbalancemodules.py");
		}  catch(...) {
			Logger(Error) << "big fat error";

		}

		p = new SimulationParameters();
		p->dt = lexical_cast<int>("86400");
		p->start = time_from_string("2000-Sep-01 00:00:00");
		p->stop = time_from_string("2001-Sep-01 00:00:00");
	}
	catch(...)
	{
		DM::Logger(DM::Error) << "Cannot start CD3 simulation";
	}

	DM::Logger(DM::Debug) << "CD3 simulation finished";
}

bool WaterBalanceHouseHold::createRaintank(OGRFeature * f,
										   double lot_area,
										   double roof_imp_fra,
										   double perv_area_fra,
										   double persons,
										   double storage_volume)
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
	catchment_w_routing->setParameter("Outdoor_Demand_Weighing_Factor_[-]", 0.5);
	m->addNode("cw_1", catchment_w_routing);

	m->addConnection(new NodeConnection(rain,"out",catchment_w_routing,"Rain" ));
	m->addConnection(new NodeConnection(evapo,"out",catchment_w_routing,"Evapotranspiration"));

	Node * flow_probe_runoff = nodereg->createNode("FlowProbe");
	m->addNode("flow_probe_runoff", flow_probe_runoff);
	m->addConnection(new NodeConnection(catchment_w_routing,"Collected_Water",flow_probe_runoff,"in" ));




	Node *runoff = nodereg->createNode("Storage");
	m->addNode("runoff", runoff);
	m->addConnection(new NodeConnection(catchment_w_routing,"Runoff",runoff,"in"));

	Node * consumer = this->createConsumer(persons);
	m->addNode("c_1", consumer);

	Node *non_pot_mixer = nodereg->createNode("Mixer");
	m->addNode("non_pot_mixer", non_pot_mixer);

	Node * nonpot_before = nodereg->createNode("FlowProbe");
	m->addNode("nonpot_before", nonpot_before);
	m->addConnection(new NodeConnection(non_pot_mixer,"out",nonpot_before,"in" ));


	Node * rwht = nodereg->createNode("RWHT");
	rwht->setParameter("storage_volume", storage_volume);
	m->addNode("rain_tank", rwht);
	m->addConnection(new NodeConnection(flow_probe_runoff,"out",rwht,"in_sw" ));
	m->addConnection(new NodeConnection(nonpot_before,"out",rwht,"in_np" ));

	Node *storage_rain = nodereg->createNode("Storage");
	m->addNode("2", storage_rain);

	Node *storage_non_p = nodereg->createNode("Storage");
	m->addNode("s_non_pot", storage_non_p);
	m->addConnection(new NodeConnection(rwht,"out_sw",storage_rain,"in" ));
	m->addConnection(new NodeConnection(rwht,"out_np",storage_non_p,"in" ));


	//Potable Demand
	Node *storage_pot = nodereg->createNode("Storage");
	m->addNode("s_pot", storage_pot);

	m->addConnection(new NodeConnection(consumer,"out_p",storage_pot,"in" ));

	s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
	s->setModel(m);
	s->setSimulationParameters(*p);
	ptime starttime = s->getSimulationParameters().start;

	m->initNodes(s->getSimulationParameters());

	m->addConnection(new NodeConnection(consumer,"out_np",non_pot_mixer,"in_0"));
	m->addConnection(new NodeConnection(catchment_w_routing,"Outdoor_Demand",non_pot_mixer,"in_1"));

	s->start(starttime);

	catchment_w_routing->setParameter("Catchment_Area_[m^2]", lot_area);
	catchment_w_routing->setParameter("Fraktion_of_Pervious_Area_pA_[-]", perv_area_fra);
	catchment_w_routing->setParameter("Fraktion_of_Impervious_Area_to_Stormwater_Drain_iASD_[-]",1.0 - roof_imp_fra - perv_area_fra);
	catchment_w_routing->setParameter("Fraktion_of_Impervious_Area_to_Reservoir_iAR_[-]",roof_imp_fra);
	std::cout << "Results" << std::endl;
	std::cout << lot_area << "\t" << perv_area_fra << "\t" << 1.0 - roof_imp_fra - perv_area_fra << "\t" << roof_imp_fra <<std::endl;
	std::cout<< "Potable Demand\t"<< *(storage_pot->getState<double>("TotalVolume")) << "\tm3" << std::endl;
	std::cout<< "Non-Potable Demand\t"<< *(storage_non_p->getState<double>("TotalVolume")) << "\tm3" << std::endl;
	std::cout<< "Total Runoff\t"<< *(runoff->getState<double>("TotalVolume")) << "\tm3" << std::endl;
	std::cout<< "Rain Runoff \t"<< *(storage_rain->getState<double>("TotalVolume")) << "\tm3" << std::endl;
	std::cout << "--------" << std::endl;

	Logger(Error) << "Total Runoff"<< *(storage_rain->getState<double>("TotalVolume"));

	Logger(Debug) << "Dry"<< *(rwht->getState<int>("dry"));
	Logger(Debug) << "Spills"<< *(rwht->getState<int>("spills"));

	if (!f) // for unit testing
		return true;
	storage_behaviour =  *(rwht->getState<std::vector<double> >("storage_behaviour"));
	DM::DMFeature::SetDoubleList(f, "storage_behaviour_daily", storage_behaviour);

	std::vector<double> provided_volume =  *(rwht->getState<std::vector<double> >("provided_volume"));
	DM::DMFeature::SetDoubleList(f, "provided_volume_daily", provided_volume);

	f->SetField("connected_roof_area", lot_area * roof_imp_fra);
	DM::DMFeature::SetDoubleList(f, "provided_volume_monthly", this->create_montlhy_values(provided_volume, 86400));

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
	//Init Model
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
	//consumption->setParameter("const_flow_nonpotable",createConstFlow(0 ));

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
