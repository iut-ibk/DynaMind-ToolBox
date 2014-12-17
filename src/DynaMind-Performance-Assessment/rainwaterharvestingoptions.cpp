#include "rainwaterharvestingoptions.h"
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

DM_DECLARE_NODE_NAME(RainWaterHarvestingOptions,Performance)


RainWaterHarvestingOptions::RainWaterHarvestingOptions()
{
	this->GDALModule = true;

	this->cd3_dir = "";
	this->addParameter("cd3_dir", DM::STRING, &this->cd3_dir);

	this->storage_volume_tank;
	this->addParameter("storage_volume", DM::STRING_LIST, &this->storage_volume_tank);

	parcels = DM::ViewContainer("parcel", DM::COMPONENT, DM::READ);
	parcels.addAttribute("non_potable_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::READ);
	parcels.addAttribute("potable_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::READ);
	parcels.addAttribute("outdoor_demand_daily", DM::Attribute::DOUBLEVECTOR, DM::READ);
	parcels.addAttribute("run_off_roof_daily", DM::Attribute::DOUBLEVECTOR, DM::READ);

	rwhts = DM::ViewContainer("rwht", DM::COMPONENT, DM::WRITE);
	rwhts.addAttribute("volume", DM::Attribute::DOUBLE, DM::WRITE);
	rwhts.addAttribute("storage_behaviour_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	rwhts.addAttribute("provided_volume_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	//rwhts.addAttribute("provided_volume_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);

	rwhts.addAttribute("non_potable_savings", DM::Attribute::DOUBLE, DM::WRITE);
	rwhts.addAttribute("outdoor_water_savings", DM::Attribute::DOUBLE, DM::WRITE);
	rwhts.addAttribute("annual_water_savings", DM::Attribute::DOUBLE, DM::WRITE);

	rwhts.addAttribute("parcel_id", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> stream;
	stream.push_back(&parcels);
	stream.push_back(&rwhts);
	this->registerViewContainers(stream);

}

std::vector<double> RainWaterHarvestingOptions::addVectors(std::vector<double> & vec1, std::vector<double> & vec2){
	std::vector<double> res_vec(vec1.size());

	for (int i = 0; i < vec1.size(); i++)
		res_vec[i] = vec1[i] + vec2[i];

	return res_vec;
}

std::vector<double> RainWaterHarvestingOptions::substractVectors(std::vector<double> & vec1, std::vector<double> & vec2){
	std::vector<double> res_vec(vec1.size());

	for (int i = 0; i < vec1.size(); i++)
		res_vec[i] = vec1[i] - vec2[i];

	return res_vec;
}

void RainWaterHarvestingOptions::run()
{
	DM::Logger(DM::Debug) << "Init CD3";
	if (!initmodel())
		return;
	DM::Logger(DM::Debug) << "Init CD3 done";

	OGRFeature * p;
	this->parcels.resetReading();
	while(p = this->parcels.getNextFeature()) {
		//Create Raintanks
		//Input Vectors
		std::vector<double> non_potable_demand_daily;
		std::vector<double> run_off_roof_daily;
		std::vector<double> outdoor_demand_daily;

		DM::DMFeature::GetDoubleList(p, "non_potable_demand_daily", non_potable_demand_daily);
		DM::DMFeature::GetDoubleList(p, "run_off_roof_daily", run_off_roof_daily);
		DM::DMFeature::GetDoubleList(p, "outdoor_demand_daily", outdoor_demand_daily);


		for (int i = 0; i < this->storage_volume_tank.size(); i++){
			OGRFeature * rwht = rwhts.createFeature();
			rwht->SetField("parcel_id", (int)p->GetFID());
			this->createTankOption(rwht, QString::fromStdString(storage_volume_tank[i]).toDouble(), run_off_roof_daily, outdoor_demand_daily,  non_potable_demand_daily);
		}
	}
}

bool RainWaterHarvestingOptions::initmodel()
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
		QString dance_nodes = QString::fromStdString(this->cd3_dir) + "/libdance4water-nodes";
		nodereg->addNativePlugin(dance_nodes.toStdString());
		try{
			nodereg->addPythonPlugin("/Users/christianurich/Documents/CD3Waterbalance/Module/cd3waterbalancemodules.py");
		}  catch(...) {
			Logger(Error) << "big fat error because citydrain modules can' t be loaded";
			this->setStatus(DM::MOD_EXECUTION_ERROR);
			return false;
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

	m = new MapBasedModel();

	n_d = nodereg->createNode("SourceVector");
	//n_d->setParameter("source",sum_demand);
	m->addNode("demand", n_d);

	n_r = nodereg->createNode("SourceVector");
	//n_r->setParameter("source",runoff);
	m->addNode("runoff", n_r);

	//RWHT
	rwht = nodereg->createNode("RWHT");
	//rwht->setParameter("storage_volume", storage_volume);
	m->addNode("rain_tank", rwht);

	//Stormwater
	m->addConnection(new NodeConnection(n_r,"out",rwht,"in_sw" ));
	m->addConnection(new NodeConnection(n_d,"out",rwht,"in_np" ));

	s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();

	s->setModel(m);

	return true;
}


std::string RainWaterHarvestingOptions::getCd3_dir() const
{
	return cd3_dir;
}

void RainWaterHarvestingOptions::setCd3_dir(const std::string &value)
{
	cd3_dir = value;
}

std::vector<double> RainWaterHarvestingOptions::create_montlhy_values(std::vector<double> daily, int seconds)
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



double RainWaterHarvestingOptions::createTankOption(OGRFeature * rwht_f, double storage_volume, std::vector<double> &runoff, std::vector<double> &out_doordemand, std::vector<double> &non_potable_demand)
{

	std::vector<double> sum_demand = this->addVectors(non_potable_demand, out_doordemand);

	m->getNode("demand")->setParameter("source",sum_demand);
	m->getNode("runoff")->setParameter("source",runoff);
	m->getNode("rain_tank")->setParameter("storage_volume", storage_volume);

	s->setSimulationParameters(*p);
	ptime starttime = s->getSimulationParameters().start;

	m->initNodes(s->getSimulationParameters());

	s->start(starttime);

	std::vector<double> storage_behaviour =  *(rwht->getState<std::vector<double> >("storage_behaviour"));

	std::vector<double> provided_volume =  *(rwht->getState<std::vector<double> >("provided_volume"));

	//Underlying assumption is that water is used for toilet flushing first
	std::vector<double> provided_non_pot_volume(provided_volume.size());

	for (int i = 0; i < provided_volume.size(); i++) {
		double subisised = non_potable_demand[i] - provided_volume[i];
		if (subisised < 0)
			provided_non_pot_volume[i] = non_potable_demand[i];
		else
			provided_non_pot_volume[i] = provided_volume[i];
	}

	//std::vector<double> provided_outdoor_demand  = this->substractVectors(provided_non_pot_voume, out_doordemand);

	double savings = 0;
	double non_pot_savings = 0;
	for (int i = 0; i < provided_volume.size(); i++){
		savings+=provided_volume[i];
		non_pot_savings+=provided_non_pot_volume[i];
	}

	if (!rwht_f) // for unit testing
		return savings;

	DM::DMFeature::SetDoubleList(rwht_f, "storage_behaviour_daily", storage_behaviour);
	DM::DMFeature::SetDoubleList(rwht_f, "provided_volume_daily", provided_volume);

	//DM::DMFeature::SetDoubleList(rwht_f, "provided_volume_monthly", this->create_montlhy_values(provided_volume, 86400));

	rwht_f->SetField("volume", storage_volume);
	rwht_f->SetField("annual_water_savings", savings);
	rwht_f->SetField("outdoor_water_savings", savings - non_pot_savings);
	rwht_f->SetField("non_potable_savings", non_pot_savings);

	return savings;

}
