#include "storage_simulation.h"
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

DM_DECLARE_CUSTOM_NODE_NAME(StorageSimulation, Simulate Storage Behaviour ,Performance Assessment)

StorageSimulation::StorageSimulation()
{
	this->GDALModule = true;

	this->storage_volume_tank;
	this->addParameter("storage_volume", DM::STRING_LIST, &this->storage_volume_tank);

	this->demand_view_name = "demand";
	this->addParameter("demand_view_name", DM::STRING, &this->demand_view_name);

	this->storage_view_name = "storage";
	this->addParameter("storage_view_name", DM::STRING, &this->storage_view_name);

	this->inflow_attribute_name = "inflow";
	this->addParameter("inflow", DM::STRING, &this->inflow_attribute_name);

	this->demand_attribute_name = "demand_attribute_name";
	this->addParameter("demand", DM::STRING, &this->demand_attribute_name);

	this->start_date = "2000-Jan-01 00:00:00";
	this->addParameter("start_date", DM::STRING, &this->start_date);

	this->end_date = "2001-Jan-01 00:00:00";
	this->addParameter("end_date", DM::STRING, &this->end_date);

	this->timestep = "86400";
	this->addParameter("timestep", DM::STRING, &this->timestep);

	start_date_from_global = false;
	this->addParameter("date_from_global", DM::BOOL, &this->start_date_from_global);

	global_viewe_name = "global_viewe_name";
	this->addParameter("global_viewe_name", DM::STRING, &this->global_viewe_name);

	start_date_name = "start_date";
	this->addParameter("start_date_name", DM::STRING, &this->start_date_name);

	end_date_name = "end_date";
	this->addParameter("end_data_name", DM::STRING, &this->end_date_name);
}

void StorageSimulation::init()
{
	std::vector<DM::ViewContainer*> stream;

	demands = DM::ViewContainer(this->demand_view_name, DM::COMPONENT, DM::READ);
	demands.addAttribute(this->demand_attribute_name, DM::Attribute::DOUBLEVECTOR, DM::READ);
	demands.addAttribute(this->inflow_attribute_name, DM::Attribute::DOUBLEVECTOR, DM::READ);


	storages = DM::ViewContainer(this->storage_view_name, DM::COMPONENT, DM::WRITE);
	storages.addAttribute("volume", DM::Attribute::DOUBLE, DM::WRITE);
	storages.addAttribute("storage_behaviour_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	storages.addAttribute("provided_volume_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);

	storages.addAttribute("annual_water_savings", DM::Attribute::DOUBLE, DM::WRITE);
	storages.addAttribute(this->demand_view_name + "_id", DM::Attribute::INT, DM::WRITE);
	storages.addAttribute("dry_days", DM::Attribute::INT, DM::WRITE);
	storages.addAttribute("spills", DM::Attribute::INT, DM::WRITE);
	storages.addAttribute("spilled_volume", DM::Attribute::DOUBLE, DM::WRITE);

	if (start_date_from_global) {
		global_object = DM::ViewContainer(this->global_viewe_name, DM::COMPONENT, DM::READ);
		global_object.addAttribute(this->start_date_name, DM::Attribute::STRING, DM::READ);
		global_object.addAttribute(this->end_date_name, DM::Attribute::STRING, DM::READ);
		stream.push_back(&global_object);
	}

	stream.push_back(&demands);
	stream.push_back(&storages);
	this->registerViewContainers(stream);
}


std::vector<double> StorageSimulation::addVectors(std::vector<double> & vec1, std::vector<double> & vec2){
	std::vector<double> res_vec(vec1.size());

	for (int i = 0; i < vec1.size(); i++)
		res_vec[i] = vec1[i] + vec2[i];

	return res_vec;
}

std::vector<double> StorageSimulation::substractVectors(std::vector<double> & vec1, std::vector<double> & vec2){
	std::vector<double> res_vec(vec1.size());

	for (int i = 0; i < vec1.size(); i++)
		res_vec[i] = vec1[i] - vec2[i];

	return res_vec;
}

void StorageSimulation::run()
{
	DM::Logger(DM::Debug) << "Init CD3";
	if (!initmodel())
		return;
	DM::Logger(DM::Debug) << "Init CD3 done";

	OGRFeature * p;
	this->demands.resetReading();
	int counter = 0;
	while(p = this->demands.getNextFeature()) {
		counter++;
		//Create Raintanks
		//Input Vectors
		std::vector<double> demand_daily;
		std::vector<double> inflow_daily;

		DM::DMFeature::GetDoubleList(p, this->demand_attribute_name , demand_daily);
		DM::DMFeature::GetDoubleList(p, this->inflow_attribute_name, inflow_daily);

        if (demand_daily.size() == 0 || inflow_daily.size() == 0){
            DM::Logger(DM::Warning) << "No demand or inflow found for " << this->demand_view_name << " " << this->demand_attribute_name << " " << (int) p->GetFID();
            continue;
        }

		for (int i = 0; i < this->storage_volume_tank.size(); i++){
			OGRFeature * storage = storages.createFeature();
			std::stringstream link_id;
			link_id << this->demand_view_name;
			link_id << "_id";
			storage->SetField(link_id.str().c_str(), (int)p->GetFID());
			this->createTankOption(storage, QString::fromStdString(storage_volume_tank[i]).toDouble(), inflow_daily, demand_daily);
		}

		if (counter % 1000 == 0){
			this->demands.syncAlteredFeatures();
			this->storages.syncAlteredFeatures();
			this->demands.setNextByIndex(counter);
		}
	}
}


bool StorageSimulation::initmodel()
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

	DM::Logger(DM::Standard) << cd3_start_date;
	DM::Logger(DM::Standard) << cd3_end_date;

	if (start_date_from_global) {
		global_object.resetReading();
		OGRFeature * f;
		while (f = global_object.getNextFeature()) {
			cd3_start_date = f->GetFieldAsString(start_date_name.c_str());
			cd3_end_date = f->GetFieldAsString(end_date_name.c_str());
			break; // we assume only one global feature
		}
	}

	try{
		// Register default simulation
#if defined(_WIN32)
		this->simreg->addNativePlugin(this->getSimulation()->getSimulationConfig().getDefaultLibraryPath() + "/cd3core");
#else
		this->simreg->addNativePlugin(this->getSimulation()->getSimulationConfig().getDefaultLibraryPath() + "/libcd3core");
#endif

		// Register default modules
#if defined(_WIN32)
		nodereg->addNativePlugin(this->getSimulation()->getSimulationConfig().getDefaultLibraryPath() + "/cd3core");
#else
		nodereg->addNativePlugin(this->getSimulation()->getSimulationConfig().getDefaultLibraryPath() + "/libcd3core");
#endif

#if defined(_WIN32)
		QString dance_nodes = QString::fromStdString(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/dance4water-nodes");
#else
		QString dance_nodes = QString::fromStdString(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/CD3Modules/libdance4water-nodes");
#endif
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
		p->start = time_from_string(this->cd3_start_date);
		p->stop = time_from_string(this->cd3_end_date);
	}
	catch(...)
	{
		DM::Logger(DM::Error) << "Cannot start CD3 simulation";
	}
	DM::Logger(DM::Debug) << "CD3 simulation finished";

	m = new MapBasedModel();

	n_d = nodereg->createNode("SourceVector");
	//n_d->setParameter("source",sum_demand);
	m->addNode("demand", n_d);

	n_r = nodereg->createNode("SourceVector");
	//n_r->setParameter("source",runoff);
	m->addNode("runoff", n_r);

	//RWHT
	storage = nodereg->createNode("RWHT");
	//rwht->setParameter("storage_volume", storage_volume);
	m->addNode("rain_tank", storage);

    //Flow meter
    flow_p = nodereg->createNode("FlowProbe");
    m->addNode("flow_probe", flow_p);

	//Stormwater
	m->addConnection(new NodeConnection(n_r,"out",storage,"in_sw" ));
	m->addConnection(new NodeConnection(n_d,"out",storage,"in_np" ));
	m->addConnection(new NodeConnection(storage,"out_sw",flow_p,"in" ));

	s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();

	s->setModel(m);

	return true;
}


std::vector<double> StorageSimulation::create_montlhy_values(std::vector<double> daily, int seconds)
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



double StorageSimulation::createTankOption(OGRFeature * storage_f, double storage_volume, std::vector<double> &inflow,  std::vector<double> &demand)
{

	m->getNode("demand")->setParameter("source",demand);
	m->getNode("runoff")->setParameter("source",inflow);
	m->getNode("rain_tank")->setParameter("storage_volume", storage_volume);

	s->setSimulationParameters(*p);
	ptime starttime = s->getSimulationParameters().start;

	m->initNodes(s->getSimulationParameters());

	s->start(starttime);

	std::vector<double> storage_behaviour =  *(storage->getState<std::vector<double> >("storage_behaviour"));
	std::vector<double> provided_volume =  *(storage->getState<std::vector<double> >("provided_volume"));

	int dry_days =  *(storage->getState<int>("dry"));
	int spills =  *(storage->getState<int>("spills"));
    double spilled_volume = *(flow_p->getState<double>("TotalFlow"));

	//Underlying assumption is that water is used for toilet flushing first
	std::vector<double> provided_non_pot_volume(provided_volume.size());

	for (int i = 0; i < provided_volume.size(); i++) {
		double subisised = demand[i] - provided_volume[i];
		if (subisised < 0)
			provided_non_pot_volume[i] = demand[i];
		else
			provided_non_pot_volume[i] = provided_volume[i];
	}

	double savings = 0;
	double non_pot_savings = 0;
	for (int i = 0; i < provided_volume.size(); i++){
		savings+=provided_volume[i];
		non_pot_savings+=provided_non_pot_volume[i];
	}

	if (!storage_f) // for unit testing
		return savings;

	DM::DMFeature::SetDoubleList(storage_f, "storage_behaviour_daily", storage_behaviour);
	DM::DMFeature::SetDoubleList(storage_f, "provided_volume_daily", provided_volume);

	storage_f->SetField("volume", storage_volume);
	storage_f->SetField("annual_water_savings", savings);
	storage_f->SetField("dry_days", dry_days);
	storage_f->SetField("spills", spills);
	storage_f->SetField("spilled_volume", spilled_volume);
	return savings;

}
