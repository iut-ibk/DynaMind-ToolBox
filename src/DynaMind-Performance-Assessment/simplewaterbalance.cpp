#include "simplewaterbalance.h"

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

DM_DECLARE_NODE_NAME(SimpleWaterBalance,Performance)

std::vector<double> SimpleWaterBalance::create_montlhy_values(std::vector<double> daily)
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


SimpleWaterBalance::SimpleWaterBalance()
{
	this->GDALModule = true;
	this->rainfile = "";
	this->addParameter("rainfile", DM::STRING, &this->rainfile);

	this->cd3_dir = "";
	this->addParameter("cd3_dir", DM::STRING, &this->cd3_dir);

	parcels = DM::ViewContainer("parcel", DM::COMPONENT, DM::READ);
	parcels.addAttribute("area", DM::Attribute::INT, DM::READ);

	parcels.addAttribute("non_potable_demand_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	parcels.addAttribute("potable_demand_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	parcels.addAttribute("run_off_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	parcels.addAttribute("non_pot_annual", DM::Attribute::DOUBLE, DM::WRITE);
	parcels.addAttribute("pot_annual", DM::Attribute::DOUBLE, DM::WRITE);

	buildings = DM::ViewContainer("building", DM::COMPONENT, DM::READ);
	buildings.addAttribute("area", DM::Attribute::DOUBLE, DM::READ);
	buildings.addAttribute("parcel_id", DM::Attribute::INT, DM::READ);


	households = DM::ViewContainer("household", DM::COMPONENT, DM::READ);
	households.addAttribute("persons", DM::Attribute::INT, DM::READ);
	households.addAttribute("building_id", DM::Attribute::INT, DM::READ);


	//	rwhts = DM::ViewContainer("rwht", DM::COMPONENT, DM::WRITE);
	//	rwhts.addAttribute("storage_behaviour_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	//	rwhts.addAttribute("provided_volume_daily", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	//	rwhts.addAttribute("provided_volume_monthly", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	//	rwhts.addAttribute("connected_roof_area", DM::Attribute::DOUBLE, DM::WRITE);

	//	rwhts.addAttribute("building_id", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> stream;

	stream.push_back(&parcels);
	stream.push_back(&buildings);
	//stream.push_back(&rwhts);
	stream.push_back(&households);
	this->registerViewContainers(stream);


	//Init Model
	this->m = new MapBasedModel();

}

void SimpleWaterBalance::run()
{
	DM::Logger(DM::Debug) << "Init CD3";
	initmodel();
	DM::Logger(DM::Debug) << "Init CD3 done";
	buildings.createIndex("parcel_id");
	households.createIndex("building_id");
	OGRFeature * p;
	OGRFeature * hh;
	this->parcels.resetReading();
	while(p = this->parcels.getNextFeature()) {
		double persons = 0;

		buildings.resetReading();
		std::stringstream parcel_query;
		parcel_query << "parcel_id = " << p->GetFID();
		buildings.setAttributeFilter(parcel_query.str().c_str());

		OGRFeature * b;
		while (b = buildings.getNextFeature()) {
			households.resetReading();
			std::stringstream household_query;

			household_query << "building_id = " << b->GetFID();
			households.setAttributeFilter(household_query.str().c_str());

			while (hh = households.getNextFeature()) {
				persons+= hh->GetFieldAsInteger("persons");
			}
		}

		createCatchment(p, persons);

		//		DM::Logger(DM::Standard) << "Connected Roof Area " << roofarea;

		//		//OGRFeature * rwht = rwhts.createFeature();
		//

		//		Node * run_off = m->getNode("ro");
		//		std::vector<double> ro = *(run_off->getState<std::vector<double> >("run_off"));

		//		DM::DMFeature::SetDoubleList( p, "run_off_monthly", this->create_montlhy_values(ro));
		//		Node * pot = m->getNode("s_pot");
		//		Node * non_pot = m->getNode("s_non_pot");

		//		double non_potable = *(non_pot->getState<double>("TotalVolume"));
		//		double potable = *(pot->getState<double>("TotalVolume"));

		//		p->SetField("non_potable_demand",-non_potable);
		//		p->SetField("potable_demand", -potable);
		//		p->SetField("total_roof_area", roofarea);

		//		Logger(Debug) << "Total Consumption non potable"<< non_potable;
		//		Logger(Debug) << "Total Consumption  potable"<< potable;
	}

	s->setSimulationParameters(*this->p);
	ptime starttime = s->getSimulationParameters().start;

	m->initNodes(s->getSimulationParameters());
	s->start(starttime);

	this->parcels.resetReading();
	while(p = this->parcels.getNextFeature()) {
		std::stringstream id_stream;
		id_stream << p->GetFID();
		Node * n = m->getNode("s_non_pot" + id_stream.str());
		if (n) {
			double volume =  *(n->getState<double>("TotalVolume"));
			p->SetField("non_pot_annual", volume);
		}
		n = m->getNode("s_pot" + id_stream.str());
		if (n) {
			double volume =  *(n->getState<double>("TotalVolume"));
			p->SetField("pot_annual", volume);
		}

	}
	//std::cout<< *(storage_non_p->getState<double>("TotalVolume")) << std::endl;
	//cout<< *(storage_pot->getState<double>("TotalVolume")) << std::endl;
}

bool SimpleWaterBalance::createCatchment(OGRFeature * parcel, double persons)
{
	DM::Logger(DM::Debug) << "Start Raintank";
	if (!nodereg) {
		DM::Logger(DM::Error) << "No nodereg";
		return false;
	}
	//	Node * rain = nodereg->createNode("IxxRainRead_v2");
	//	if (!rain) {
	//		DM::Logger(DM::Error) << "Couldn't create " << " IxxRainRead_v2";
	//		return false;
	//	}
	//	rain->setParameter("rain_file", this->rainfile);
	//	std::string datetime("d.M.yyyy HH:mm:ss");
	//	rain->setParameter("datestring", datetime);
	//	m->addNode("r_1", rain);

	//	Node *runoff = nodereg->createNode("ImperviousRunoff");
	//	runoff->setParameter("area", area);
	//	m->addNode("ro", runoff);

	//	m->addConnection(new NodeConnection(rain,"out",runoff,"rain_in" ));

	Node * consumer = this->createHousehold(persons);
	std::stringstream id_stream;
	id_stream << parcel->GetFID();

	m->addNode("c" + id_stream.str(), consumer);

	Node *storage_non_p = nodereg->createNode("Storage");
	m->addNode("s_non_pot" + id_stream.str(), storage_non_p);

	Node *storage_pot = nodereg->createNode("Storage");
	m->addNode("s_pot" + id_stream.str(), storage_pot);

	m->addConnection(new NodeConnection(consumer,"out_p" ,storage_pot,"in" ));
	m->addConnection(new NodeConnection(consumer,"out_np",storage_non_p,"in" ));

	s->setModel(m);

	return true;
}

Flow SimpleWaterBalance::createConstFlow(double const_flow)
{
	Flow cf;
	cf[0] = const_flow;
	return cf;
}

Node *SimpleWaterBalance::createHousehold(int persons)
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

	consumption->setParameter("const_flow_greywater",createConstFlow(0));
	consumption->setParameter("const_flow_sewer",createConstFlow(leak_other + washing_machine + taps + shower_bath + toilet));

	consumption->setParameter("const_flow_stormwater",createConstFlow(0));

	return consumption;
}

void SimpleWaterBalance::initmodel()
{
	sink = new DynaMindStreamLogSink();
	Log::init(sink, Error);

	simreg = new SimulationRegistry();
	nodereg = new NodeRegistry();
	s = simreg->createSimulation("DefaultSimulation");
	DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();

	std::map<std::string, Flow::CalculationUnit> flowdef;
	flowdef["Q"]=Flow::flow;
	Flow::undefine();
	Flow::define(flowdef);

	QDir dir("./");

	Logger(Standard) << dir.absolutePath().toStdString();

	try{
		QString dance_nodes = QString::fromStdString(this->cd3_dir) + "libdance4water-nodes";
		nodereg->addNativePlugin(dance_nodes.toStdString());
		try{
		nodereg->addPythonPlugin("/Users/christianurich/Documents/CD3Waterbalance/Module/cd3waterbalancemodules.py");
		//		QString standard_nodes = QString::fromStdString(this->cd3_dir) + "/libnodes";
		//		nodereg->addNativePlugin(standard_nodes.toStdString());
		//		simreg->addNativePlugin(standard_nodes.toStdString());
		}  catch(...) {
			std::cout << "big fat error" << std::endl;
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

