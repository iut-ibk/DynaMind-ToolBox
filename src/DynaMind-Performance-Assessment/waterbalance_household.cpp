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
#include <flow.h>
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
        p->dt = lexical_cast<int>("360");
        p->start = time_from_string("2001-Jan-01 00:00:00");
        p->stop = time_from_string("2001-Jan-02 00:00:00");
        MapBasedModel m;


        std::vector<std::string> node_names = nodereg->getRegisteredNames();
        //1000 29501 ms total

        //        for (int i = 0; i < 100; i++) {
        Node *consumption = nodereg->createNode("Consumption");
        m.addNode(QString::number(1).toStdString(), consumption);
        Flow const_flow;
        const_flow[0] = -0.001;
        consumption->setParameter("const_flow_potable",const_flow);
        consumption->setParameter("const_flow_nonpotable",const_flow);
        consumption->setParameter("const_flow_sewer",const_flow);


        Node *storage = nodereg->createNode("Storage");
        m.addNode(QString::number(2).toStdString(), storage);

        m.addConnection(new NodeConnection(consumption,"outp",storage,"demand_in" ));

        s = simreg->createSimulation(simreg->getRegisteredNames().front());
        DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
        s->setModel(&m);
        s->setSimulationParameters(*p);
        ptime starttime = s->getSimulationParameters().start;

        m.initNodes(s->getSimulationParameters());
        s->start(starttime);
        Logger(Error) << "Total Consumption"<< *(consumption->getState<double>("TotalVolume"));
        Logger(Error) << "Total Consumption"<< *(storage->getState<double>("TotalVolume"));

    }
    catch(...)
    {
        DM::Logger(DM::Error) << "Cannot start CD3 simulation";
    }


    //clear();

    DM::Logger(DM::Debug) << "CD3 simulation finished";
}

