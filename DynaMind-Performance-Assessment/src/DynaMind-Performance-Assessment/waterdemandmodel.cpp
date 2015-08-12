#include "waterdemandmodel.h"
//GDAL inlcudes
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
#include <nodeconnection.h>

//DM includes
#include <dynamindlogsink.h>
#include <dmsimulation.h>
#include <dmgdalhelper.h>

//STD
#include <numeric>


DM_DECLARE_CUSTOM_NODE_NAME(WaterDemandModel,Water Demand Model, Performance Assessment)

WaterDemandModel::WaterDemandModel()
{
	this->GDALModule = true;
	this->rainfile = "";
	this->addParameter("rainfile", DM::FILENAME, &this->rainfile);

	this->evapofile = "";
	this->addParameter("evapofile", DM::FILENAME, &this->evapofile);


	parcels = DM::ViewContainer("parcel", DM::COMPONENT, DM::READ);
	parcels.addAttribute("area", DM::Attribute::DOUBLE, DM::READ);
	parcels.addAttribute("persons", DM::Attribute::DOUBLE, DM::READ);
	parcels.addAttribute("roof_area", DM::Attribute::DOUBLE, DM::READ);
	parcels.addAttribute("garden_area", DM::Attribute::DOUBLE, DM::READ);
	parcels.addAttribute("non_potable_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	parcels.addAttribute("potable_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	parcels.addAttribute("outdoor_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	parcels.addAttribute("run_off_roof_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);

	std::vector<DM::ViewContainer*> stream;
	stream.push_back(&parcels);
	this->registerViewContainers(stream);

}

void WaterDemandModel::run()
{
	initmodel();
	OGRFeature * p;
	this->parcels.resetReading();

	//Calculate outdoor demand for standard unit (100m2 roof, 100m2 garden, 1 person)
	double imp_fraction = 0.2;
	calculateRunoffAndDemand(500, imp_fraction, 0.8, 1);


	while(p = this->parcels.getNextFeature()) {
		double persons = p->GetFieldAsDouble("persons");
		double garden_area = p->GetFieldAsDouble("garden_area");
		double roof_area = p->GetFieldAsDouble("roof_area");


//		this->stormwater_runoff = *(flow_probe_runoff->getState<std::vector<double> >("Flow"));
//		this->non_potable_demand = *(nonpot_before->getState<std::vector<double> >("Flow"));
//		this->potable_demand = *(pot_before->getState<std::vector<double> >("Flow"));
//		this->outdoor_demand = *(flow_probe_outdoor->getState<std::vector<double> >("Flow"));


//		parcels.addAttribute("non_potable_demand_daily", DM::Attribute::DOUBLE, DM::WRITE);
//		parcels.addAttribute("potable_demand_daily", DM::Attribute::DOUBLE, DM::WRITE);
//		parcels.addAttribute("outdoor_demand_daily", DM::Attribute::DOUBLE, DM::WRITE);
//		parcels.addAttribute("run_off_roof_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);

		DM::DMFeature::SetDoubleList( p, "potable_demand_daily", this->mutiplyVector(this->potable_demand, persons));
		DM::DMFeature::SetDoubleList( p, "non_potable_demand_daily", this->mutiplyVector(this->non_potable_demand, persons));
		DM::DMFeature::SetDoubleList( p, "outdoor_demand_daily",  this->mutiplyVector(this->outdoor_demand,garden_area/400.));
		//DM::DMFeature::SetDoubleList( p, "outdoor_demand_daily", this->outdoor_demand);
		DM::DMFeature::SetDoubleList( p, "run_off_roof_daily", this->mutiplyVector(this->stormwater_runoff,roof_area/100.));
	}
}

void WaterDemandModel::initmodel()
{
	sink = new DynaMindStreamLogSink();
	Log::init(sink, Error);

	simreg = new SimulationRegistry();
	nodereg = new NodeRegistry();
	//s = 0;

	std::map<std::string, Flow::CalculationUnit> flowdef;
	flowdef["Q"]=Flow::flow;
	Flow::undefine();
	Flow::define(flowdef);

	QDir dir("./");

	Logger(Standard) << dir.absolutePath().toStdString();


	try{
		QString dance_nodes = QString::fromStdString(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/libdance4water-nodes");
		nodereg->addNativePlugin(dance_nodes.toStdString());
		nodereg->addToPythonPath(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/CD3Waterbalance/Module");
		try{
			nodereg->addPythonPlugin(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/CD3Waterbalance/Module/cd3waterbalancemodules.py");
		}  catch(...) {
			Logger(Error) << "Please point path to CD3 water balance modules";
			this->setStatus(DM::MOD_EXECUTION_ERROR);
			return;

		}

		p = new SimulationParameters();
		p->dt = lexical_cast<int>("86400");
		p->start = time_from_string("2000-Jan-01 00:00:00");
		p->stop = time_from_string("2001-Jan-01 00:00:00");
	}
	catch(...)
	{
		DM::Logger(DM::Error) << "Cannot start CD3 simulation";
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}

	DM::Logger(DM::Debug) << "CD3 simulation finished";
}

bool WaterDemandModel::calculateRunoffAndDemand(double lot_area,
													 double roof_imp_fra,
													 double perv_area_fra,
													 double persons)
{

	MapBasedModel m;
	DM::Logger(DM::Debug) << "Start Raintank";
	Node * rain = nodereg->createNode("IxxRainRead_v2");
	if (!rain) {
		DM::Logger(DM::Error) << "Couldn't create " << " IxxRainRead_v2";
		return false;
	}

	rain->setParameter("rain_file", this->rainfile);
	std::string datetime("d.M.yyyy HH:mm:ss");
	rain->setParameter("datestring", datetime);
	m.addNode("r_1", rain);

	//evapo
	Node * evapo = nodereg->createNode("IxxRainRead_v2");
	if (!evapo) {
		DM::Logger(DM::Error) << "Couldn't create " << " IxxRainRead_v2";
		return false;
	}
	std::cout <<  this->evapofile << std::endl;
	evapo->setParameter("rain_file", this->evapofile);
	evapo->setParameter("datestring", datetime);
	m.addNode("e_1", evapo);

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
	catchment_w_routing->setParameter("Outdoor_Demand_Weighing_Factor_[-]", 1.0);
	m.addNode("cw_1", catchment_w_routing);

	m.addConnection(new NodeConnection(rain,"out",catchment_w_routing,"Rain" ));
	m.addConnection(new NodeConnection(evapo,"out",catchment_w_routing,"Evapotranspiration"));

	Node * flow_probe_runoff = nodereg->createNode("FlowProbe");
	m.addNode("flow_probe_runoff", flow_probe_runoff);
	m.addConnection(new NodeConnection(catchment_w_routing,"Collected_Water",flow_probe_runoff,"in" ));

	Node * flow_probe_outdoor = nodereg->createNode("FlowProbe");
	m.addNode("flow_probe_outdoor", flow_probe_outdoor);
	m.addConnection(new NodeConnection(catchment_w_routing,"Outdoor_Demand",flow_probe_outdoor,"in" ));

	Node * consumer = this->createConsumer(persons);
	m.addNode("c_1", consumer);

//	//non-potable_demand
	Node * nonpot_before = nodereg->createNode("FlowProbe");
	m.addNode("non_pot", nonpot_before);
	m.addConnection(new NodeConnection(consumer,"out_np",nonpot_before,"in" ));

//	//potable_demand
	Node * pot_before = nodereg->createNode("FlowProbe");
	m.addNode("pot", pot_before);
	m.addConnection(new NodeConnection(consumer,"out_p",pot_before,"in" ));

	ISimulation *s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
	s->setModel(&m);
	s->setSimulationParameters(*p);
	ptime starttime = s->getSimulationParameters().start;

	m.initNodes(s->getSimulationParameters());

	s->start(starttime);

	this->stormwater_runoff = *(flow_probe_runoff->getState<std::vector<double> >("Flow"));
	this->non_potable_demand = *(nonpot_before->getState<std::vector<double> >("Flow"));
	this->potable_demand = *(pot_before->getState<std::vector<double> >("Flow"));
	this->outdoor_demand = *(flow_probe_outdoor->getState<std::vector<double> >("Flow"));
	delete s;
	return true;
}

Flow WaterDemandModel::createConstFlow(double const_flow)
{
	Flow cf;
	cf[0] = const_flow;
	return cf;
}


Node *WaterDemandModel::createConsumer(int persons)
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


std::string WaterDemandModel::getEvapofile() const
{
	return evapofile;
}

void WaterDemandModel::setEvapofile(const std::string &value)
{
	evapofile = value;
}

std::string WaterDemandModel::getRainfile() const
{
	return rainfile;
}

void WaterDemandModel::setRainfile(const std::string &value)
{
	rainfile = value;
}
std::vector<double> WaterDemandModel::create_montlhy_values(std::vector<double> daily, int seconds)
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
std::vector<double> WaterDemandModel::getNon_potable_demand() const
{
	return non_potable_demand;
}

void WaterDemandModel::setNon_potable_demand(const std::vector<double> &value)
{
	non_potable_demand = value;
}

std::vector<double> WaterDemandModel::mutiplyVector(std::vector<double> &vec, double multiplyer)
{
	std::vector<double> res_vec(vec.size());

	for (int i = 0; i < vec.size(); i++)
		res_vec[i] = vec[i] * multiplyer;

	return res_vec;

}

std::vector<double> WaterDemandModel::getStormwater_runoff() const
{
	return stormwater_runoff;
}

void WaterDemandModel::setStormwater_runoff(const std::vector<double> &value)
{
	stormwater_runoff = value;
}
