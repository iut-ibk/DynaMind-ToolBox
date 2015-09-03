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

	this->start_date = "2000-Jan-01 00:00:00";
	this->addParameter("start_date", DM::STRING, &this->start_date);

	this->end_date = "2001-Jan-01 00:00:00";
	this->addParameter("end_date", DM::STRING, &this->end_date);

	this->timestep = "86400";
	this->addParameter("timestep", DM::STRING, &this->timestep);

	start_date_from_global = false;
	this->addParameter("date_from_global", DM::BOOL, &this->start_date_from_global);

	global_viewe_name = "city";
	this->addParameter("global_viewe_name", DM::STRING, &this->global_viewe_name);

	start_date_name = "start_date";
	this->addParameter("start_date_name", DM::STRING, &this->start_date_name);

	end_date_name = "end_date";
	this->addParameter("end_data_name", DM::STRING, &this->end_date_name);

	from_rain_station = false;
	this->addParameter("from_rain_station", DM::BOOL, &this->from_rain_station);

	to_rain_station = false;
	this->addParameter("to_rain_station", DM::BOOL, &this->to_rain_station);

}

void WaterDemandModel::run()
{
	station_ids = std::set<int>();
	rainfalls = std::map<int, std::vector<double> >();
	evaotranspirations = std::map<int, std::vector<double> >();

	stormwater_runoff = std::map<int, std::vector<double> >();
	non_potable_demand = std::map<int, std::vector<double> >();
	potable_demand = std::map<int, std::vector<double> >();
	outdoor_demand = std::map<int, std::vector<double> >();

	if (!initmodel())
		return;


	// init rain
	if (this->from_rain_station)
		initRain();

	// Calculate outdoor demand for standard unit (100m2 roof, 100m2 garden, 1 person)
	double imp_fraction = 0.2;
	calculateRunoffAndDemand(500, imp_fraction, 0.8, 1);


	if (this->to_rain_station) {
		this->station.resetReading();
		OGRFeature * st;
		while (st = station.getNextFeature()) {
			DM::DMFeature::SetDoubleList( st, "potable_demand_daily", this->mutiplyVector(this->potable_demand[st->GetFID()], 1));
			DM::DMFeature::SetDoubleList( st, "non_potable_demand_daily", this->mutiplyVector(this->non_potable_demand[st->GetFID()], 1));
			DM::DMFeature::SetDoubleList( st, "outdoor_demand_daily", this->mutiplyVector(this->outdoor_demand[st->GetFID()], 1./400.)); //Unit Area
			DM::DMFeature::SetDoubleList( st, "run_off_roof_daily", this->mutiplyVector(this->stormwater_runoff[st->GetFID()], 1./100.)); //Unit area
		}
		return;
	}
		int counter = 0;
	OGRFeature * p;
	this->parcels.resetReading();
	while(p = this->parcels.getNextFeature()) {
		counter++;
		double persons = p->GetFieldAsDouble("persons");
		double garden_area = p->GetFieldAsDouble("garden_area");
		double roof_area = p->GetFieldAsDouble("roof_area");

		int station_id = -1;
		if (this->from_rain_station)
			station_id= p->GetFieldAsInteger("station_id");

		DM::DMFeature::SetDoubleList( p, "potable_demand_daily", this->mutiplyVector(this->potable_demand[station_id], persons));
		DM::DMFeature::SetDoubleList( p, "non_potable_demand_daily", this->mutiplyVector(this->non_potable_demand[station_id], persons));
		DM::DMFeature::SetDoubleList( p, "outdoor_demand_daily", this->mutiplyVector(this->outdoor_demand[station_id], garden_area/400.)); //Unit Area
		DM::DMFeature::SetDoubleList( p, "run_off_roof_daily", this->mutiplyVector(this->stormwater_runoff[station_id], roof_area/100.)); //Unit area

		if (counter % 1000 == 0){
			this->parcels.syncAlteredFeatures();
			this->parcels.setNextByIndex(counter);
		}

	}
}

void WaterDemandModel::init()
{
	std::vector<DM::ViewContainer*> stream;



	if (this->from_rain_station)
		parcels.addAttribute("station_id", "station", DM::READ);

	station = DM::ViewContainer("station", DM::COMPONENT, DM::WRITE);

	timeseries = DM::ViewContainer("timeseries", DM::COMPONENT, DM::READ);
	timeseries.addAttribute("values", DM::Attribute::DOUBLEVECTOR, DM::READ);
	timeseries.addAttribute("station_id", "station", DM::READ);
	timeseries.addAttribute("type", DM::Attribute::STRING, DM::READ);

	if (start_date_from_global) {
		global_object = DM::ViewContainer(this->global_viewe_name, DM::COMPONENT, DM::READ);
		global_object.addAttribute(this->start_date_name, DM::Attribute::STRING, DM::READ);
		global_object.addAttribute(this->end_date_name, DM::Attribute::STRING, DM::READ);
		stream.push_back(&global_object);
	}

	if (this->from_rain_station){
		stream.push_back(&station);
		stream.push_back(&timeseries);
	}

	if (!this->to_rain_station) {
		parcels = DM::ViewContainer("parcel", DM::COMPONENT, DM::READ);
		parcels.addAttribute("area", DM::Attribute::DOUBLE, DM::READ);
		parcels.addAttribute("persons", DM::Attribute::DOUBLE, DM::READ);
		parcels.addAttribute("roof_area", DM::Attribute::DOUBLE, DM::READ);
		parcels.addAttribute("garden_area", DM::Attribute::DOUBLE, DM::READ);
		parcels.addAttribute("non_potable_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
		parcels.addAttribute("potable_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
		parcels.addAttribute("outdoor_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
		parcels.addAttribute("run_off_roof_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
		stream.push_back(&parcels);
	} else {
		station.addAttribute("non_potable_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
		station.addAttribute("potable_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
		station.addAttribute("outdoor_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
		station.addAttribute("run_off_roof_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	}

	this->registerViewContainers(stream);
}

bool WaterDemandModel::initmodel()
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

	cd3_start_date = this->start_date;
	cd3_end_date =this->end_date;

	if (start_date_from_global) {
		global_object.resetReading();
		OGRFeature * f;
		while (f = global_object.getNextFeature()) {
			cd3_start_date = f->GetFieldAsString(start_date_name.c_str());
			cd3_end_date = f->GetFieldAsString(end_date_name.c_str());
			break; // we assume only one global feature
		}
	}
	DM::Logger(DM::Standard) << cd3_start_date;
	DM::Logger(DM::Standard) << cd3_end_date;

	try{
		// Register default simulation
		this->simreg->addNativePlugin(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/libcd3core");

		// Register default modules
		nodereg->addNativePlugin(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/libcd3core");

		QString dance_nodes = QString::fromStdString(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/libdance4water-nodes");
		nodereg->addNativePlugin(dance_nodes.toStdString());

		nodereg->addToPythonPath(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/CD3Waterbalance/Module");
		nodereg->addToPythonPath(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/CD3Waterbalance/WaterDemandModel");
		try{
			nodereg->addPythonPlugin(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/CD3Waterbalance/Module/cd3waterbalancemodules.py");
		}  catch(...) {
			Logger(Error) << "Please point path to CD3 water balance modules";
			this->setStatus(DM::MOD_EXECUTION_ERROR);
			return false;

		}

		p = new SimulationParameters();
		p->dt = lexical_cast<int>(this->timestep);
		p->start = time_from_string(cd3_start_date);
		p->stop = time_from_string(cd3_end_date);
	}
	catch(...)
	{
		DM::Logger(DM::Error) << "Cannot start CD3 simulation";
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return false;
	}

	DM::Logger(DM::Debug) << "CD3 simulation finished";
	return true;
}

bool WaterDemandModel::createRainInput(MapBasedModel * m) {
	DM::Logger(DM::Debug) << "Start Raintank";
	if (!this->from_rain_station) {
		this->station_ids.insert(-1);

		DM::Logger(DM::Standard) << "Init with rain file";
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
		DM::Logger(DM::Debug) << this->evapofile;
		evapo->setParameter("rain_file", this->evapofile);
		evapo->setParameter("datestring", datetime);
		m->addNode("e_1", evapo);

		return true;
	}
	DM::Logger(DM::Standard) << "Init using stations";
	Node *  rain = nodereg->createNode("SourceVector");
	if (!rain)
		return false;
	//n_d->setParameter("source",sum_demand);
	m->addNode("r_1", rain);

	Node * evapo = nodereg->createNode("SourceVector");
	if (!evapo)
		return false;
	//n_d->setParameter("source",sum_demand);
	m->addNode("e_1", evapo);

	return true;
}

bool WaterDemandModel::calculateRunoffAndDemand(double lot_area,
												double roof_imp_fra,
												double perv_area_fra,
												double persons)
{

	MapBasedModel m;

	createRainInput(&m);

	Node * rain = m.getNode("r_1");
	Node * evapo = m.getNode("e_1");

	//catchment
	Node * catchment_w_routing = nodereg->createNode("Catchment_w_Routing");
	if (!catchment_w_routing || !rain || !evapo) {
		DM::Logger(DM::Error) << "Init Model failed";
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

	//non-potable_demand
	Node * nonpot_before = nodereg->createNode("FlowProbe");
	m.addNode("non_pot", nonpot_before);
	m.addConnection(new NodeConnection(consumer,"out_np",nonpot_before,"in" ));

	//potable_demand
	Node * pot_before = nodereg->createNode("FlowProbe");
	m.addNode("pot", pot_before);
	m.addConnection(new NodeConnection(consumer,"out_p",pot_before,"in" ));

	ISimulation *s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
	s->setModel(&m);

	for (std::set<int>::const_iterator it = this->station_ids.begin();
		 it != this->station_ids.end();
		 ++it) {
		int station_id = *it;

		if (station_id != -1) {
			DM::Logger(DM::Standard) << "Use rain station " << station_id;
			if (this->rainfalls.find(station_id) == this->rainfalls.end()) {
				DM::Logger(DM::Error) << "Rainfall vector for " << station_id << " not found";
				return false;
			}
			if (this->evaotranspirations.find(station_id) == this->evaotranspirations.end()) {
				DM::Logger(DM::Error) << "Evapotranspirations vector for " << station_id << " not found";
				return false;
			}
			std::vector<double> rain = this->rainfalls[station_id];
			std::vector<double> evaotranspiration = this->evaotranspirations[station_id];
			m.getNode("r_1")->setParameter("source",rain);
			m.getNode("e_1")->setParameter("source",evaotranspiration);
		}

		s->setSimulationParameters(*p);
		ptime starttime = s->getSimulationParameters().start;

		m.initNodes(s->getSimulationParameters());

		s->start(starttime);

		this->stormwater_runoff[station_id] = *(flow_probe_runoff->getState<std::vector<double> >("Flow"));
		this->non_potable_demand[station_id] = *(nonpot_before->getState<std::vector<double> >("Flow"));
		this->potable_demand[station_id] = *(pot_before->getState<std::vector<double> >("Flow"));
		this->outdoor_demand[station_id] = *(flow_probe_outdoor->getState<std::vector<double> >("Flow"));


	}
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

void WaterDemandModel::initRain()
{
	//time series naming
	this->timeseries.resetReading();
	this->timeseries.setAttributeFilter("type = 'rainfall intensity' or type = 'evapotranspiration'");
	OGRFeature * r;

	while (r = this->timeseries.getNextFeature()) {
		std::vector<double> vec;
		DM::DMFeature::GetDoubleList(r, "values", vec);
		std::string type = r->GetFieldAsString("type");
		int station_id = r->GetFieldAsInteger("station_id");
		if (type == "rainfall intensity")
			rainfalls[station_id] = vec;
		if (type == "evapotranspiration")
			evaotranspirations[station_id] = vec;
		station_ids.insert(station_id);
	}
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

std::vector<double> WaterDemandModel::mutiplyVector(std::vector<double> &vec, double multiplyer)
{
	std::vector<double> res_vec(vec.size());

	for (int i = 0; i < vec.size(); i++)
		res_vec[i] = vec[i] * multiplyer;

	return res_vec;

}
