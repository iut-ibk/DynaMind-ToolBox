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

}

void WaterBalanceHouseHold::run()
{
    DM::Logger(DM::Standard) << "Init CD3";
    initmodel();
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
        nodereg->addNativePlugin("Modules/libdance4water-nodes");
        nodereg->addNativePlugin("libnodes");
        simreg->addNativePlugin("libnodes");

        p = new SimulationParameters();
        p->dt = lexical_cast<int>("86400");
        p->start = time_from_string("2000-Jan-01 00:00:00");
        p->stop = time_from_string("2010-Jan-01 00:00:00");

        MapBasedModel m;

        std::vector<std::string> node_names = nodereg->getRegisteredNames();

        Node * rain = nodereg->createNode("IxxRainRead_v2");
        std::string rainfile = "/Users/curich/Documents/DynaMind-ToolBox/Data/Raindata/melb_rain.ixx";
        rain->setParameter("rain_file", rainfile);
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

        Logger(Error) << "Total Consumption"<< *(storage_non_p->getState<double>("TotalVolume"));
        Logger(Error) << "Total Runoff"<< *(storage_rain->getState<double>("TotalVolume"));

        Logger(Error) << "Dry"<< *(rwht->getState<int>("dry"));
        Logger(Error) << "Spills"<< *(rwht->getState<int>("spills"));


    }
    catch(...)
    {
        DM::Logger(DM::Error) << "Cannot start CD3 simulation";
    }

    //clear();

    DM::Logger(DM::Debug) << "CD3 simulation finished";
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

