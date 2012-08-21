/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair
 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "waterbalance.h"

#include <log.h>
#include <logger.h>
#include <logsink.h>

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

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>

#include <iostream>
#include <fstream>
#include <QFile>
#include <QTime>
#include <map>

DM_DECLARE_NODE_NAME(WaterBalance,Performance)

WaterBalance::WaterBalance()
{   
    simreg=0;
    nodereg=0;
    sink=0;
    s=0;

    std::vector<DM::View> views;

    //Define Parameter for potable water network    
    this->tank = DM::View("TANK", DM::NODE, DM::READ);
    this->tank.getAttribute("Id");
    this->tank.getAttribute("InCapacity");
    this->tank.getAttribute("MaxVolume");
    this->tank.getAttribute("Successor");
    this->tank.getAttribute("UDTank");
    this->tank.addAttribute("Volumecurve");
    views.push_back(this->tank);

    //Define Parameter for nonpotable water network
    this->tank3rd = DM::View("3RDTANK", DM::NODE, DM::READ);
    this->tank3rd.getAttribute("Id");
    this->tank3rd.getAttribute("InCapacity");
    this->tank3rd.getAttribute("MaxVolume");
    this->tank3rd.getAttribute("Successor");
    this->tank3rd.getAttribute("UDTank");
    this->tank3rd.addAttribute("Volumecurve");
    views.push_back(this->tank3rd);

    //Catchment
    this->rdata = DM::View("CATCHMENT", DM::FACE, DM::READ);
    this->rdata.getAttribute("3rdTank");
    this->rdata.getAttribute("Blockmodel");
    this->rdata.getAttribute("Population");
    this->rdata.getAttribute("Tank");
    this->rdata.getAttribute("UDTank");
    views.push_back(this->rdata);

    //Define Parameter for potable water network
    this->storage = DM::View("STORAGE", DM::NODE, DM::READ);
    this->storage.getAttribute("Id");
    this->storage.getAttribute("Maxvolume");
    this->storage.addAttribute("Volumecurve");
    views.push_back(this->storage);

    this->addData("City", views);
}

void WaterBalance::run()
{
    initCD3();

    try{
        nodereg->addNativePlugin("Modules/libdance4water-nodes.so");
        nodereg->addNativePlugin("libnodes.so");
        simreg->addNativePlugin("libnodes.so");

        MapBasedModel m;

        if(!createModel(&m))
        {
            DM::Logger(DM::Error) << "CD3 has no valid simulation";
            throw std::exception();
        }

        if(!simreg->getRegisteredNames().size())
        {
            DM::Logger(DM::Error) << "CD3 has no valid model";
            throw std::exception();
        }

        s = simreg->createSimulation(simreg->getRegisteredNames().front());
        DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
        s->setModel(&m);

        SimulationParameters p;

        p.dt = lexical_cast<int>("300");
        p.start = time_from_string("2001-Jan-01 00:00:00");
        p.stop = time_from_string("2001-Jan-03 00:00:00");

        s->setSimulationParameters(p);

        ptime starttime = s->getSimulationParameters().start;
        m.initNodes(s->getSimulationParameters());
        s->start(starttime);


    }
    catch(...)
    {
        DM::Logger(DM::Error) << "Cannot start CD3 simulation";
    }


    clear();

    DM::Logger(DM::Debug) << "CD3 simulation finished";
}

bool WaterBalance::createModel(MapBasedModel *m)
{
    this->city = this->getData("City");

    if(!createTanks(m,tank,&tankconversion))
        return false;

    if(!createTanks(m,tank3rd,&tank3rdconversion))
        return false;

    if(!createTanks(m,storage,&storageconversion))
        return false;

    return true;
}

bool WaterBalance::createTanks(MapBasedModel *m, DM::View v, std::map<std::string,std::string> *ids)
{
    std::vector<std::string> tankids = city->getUUIDsOfComponentsInView(v);

    //create tanks
    for(int index=0; index<tankids.size(); index++)
    {
        DM::Node *currenttank = city->getNode(tankids[index]);
        Node *cd3tank = nodereg->createNode("Tank");

        Flow maxvolume;
        Flow maxoutflow;
        maxvolume[0] = currenttank->getAttribute("MaxVolume")->getDouble();
        maxoutflow[0] = currenttank->getAttribute("InCapacity")->getDouble();
        cd3tank->setParameter("maxvolume",maxvolume);
        cd3tank->setParameter("maxoutflow",maxoutflow);

        m->addNode(currenttank->getUUID(),cd3tank);

        (*ids)[currenttank->getAttribute("Id")->getString()]=currenttank->getUUID();
    }

    //create connections
    for(int index=0; index<tankids.size(); index++)
    {
        DM::Node *currenttank = city->getNode(tankids[index]);

        if(currenttank->getAttribute("Successor")->getString().empty())
            continue;

        Node *source = m->getNode(tankids[index]);
        Node *sink = m->getNode(ids->at(currenttank->getAttribute("Successor")->getString()));m->getNode(ids->at(currenttank->getAttribute("Successor")->getString()));

        m->addConnection(new NodeConnection(source,"out",sink,"in"));
    }

    return true;
}

bool WaterBalance::createBlocks(MapBasedModel *m)
{
    std::vector<std::string> blockids = city->getUUIDsOfComponentsInView(rdata);

    for(int index=0; index<blockids.size(); index++)
    {
        DM::Face *currentblock = city->getFace(blockids[index]);

        if(currentblock->getAttribute("Blockmodel")->getString().empty())
            continue;

        if(!nodereg->contains(currentblock->getAttribute("Blockmodel")->getString()))
        {
            DM::Logger(DM::Error) << "CD3 block named \"" << currentblock->getAttribute("Blockmodel")->getString() << "\n does not exist";
            return false;
        }

        Node *cd3block = nodereg->createNode(currentblock->getAttribute("Blockmodel")->getString());

        //TMP CODE FOR GENERATING SOME OUTPUT IN BLOCKS
        Flow const_flow;
        const_flow[0] = 0.3;
        cd3block->setParameter("const_flow_nonpotable",const_flow);
        cd3block->setParameter("const_flow_potable",const_flow);
        cd3block->setParameter("const_flow_sewer",const_flow);

        m->addNode(currentblock->getUUID(),cd3block);
    }

    std::map<std::string, Node*> tank3rdmix;
    std::map<std::string, Node*> tankmix;
    std::map<std::string, Node*> storagemix;


    int tankports=0;
    int tank3rdports=0;
    int storageports=0;

    for(int index=0; index<blockids.size(); index++)
    {
        DM::Face *currentblock = city->getFace(blockids[index]);

        if(currentblock->getAttribute("Blockmodel")->getString().empty())
            continue;

        if(!nodereg->contains(currentblock->getAttribute("Blockmodel")->getString()))
        {
            DM::Logger(DM::Error) << "CD3 block named \"" << currentblock->getAttribute("Blockmodel")->getString() << "\n does not exist";
            return false;
        }

        std::string tanksucc;

        //tank connections
        tanksucc = currentblock->getAttribute("Tank")->getString();

        if(tankmix.find(tanksucc)==tankmix.end())
        {
            tankmix[tanksucc]=nodereg->createNode("Mixer");
            m->addNode("tankmix_" + tanksucc, tankmix[tanksucc]);
            m->addConnection(new NodeConnection(tankmix[tanksucc],"out",m->getNode(tankconversion[tanksucc]),"in"));
        }

        Node *current = tankmix[tanksucc];
        current->setParameter("num_inputs",++tankports);
        m->addConnection(new NodeConnection(m->getNode(currentblock->getUUID()),"outp",tankmix[tanksucc],"in_" + tankports));

        //tank3rd connections
        tanksucc = currentblock->getAttribute("3rdTank")->getString();

        if(tank3rdmix.find(tanksucc)==tank3rdmix.end())
        {
            tank3rdmix[tanksucc]=nodereg->createNode("Mixer");
            m->addNode("tank3rdmix_" + tanksucc, tank3rdmix[tanksucc]);
            m->addConnection(new NodeConnection(tank3rdmix[tanksucc],"out",m->getNode(tank3rdconversion[tanksucc]),"in"));
        }

        current = tank3rdmix[tanksucc];
        current->setParameter("num_inputs",++tank3rdports);
        m->addConnection(new NodeConnection(m->getNode(currentblock->getUUID()),"outnp",tank3rdmix[tanksucc],"in_" + tank3rdports));

        //tank connections
        tanksucc = currentblock->getAttribute("UDTank")->getString();

        if(storagemix.find(tanksucc)==tankmix.end())
        {
            storagemix[tanksucc]=nodereg->createNode("Mixer");
            m->addNode("udmix_" + tanksucc, storagemix[tanksucc]);
            m->addConnection(new NodeConnection(storagemix[tanksucc],"out",m->getNode(storageconversion[tanksucc]),"in"));
        }

        current = tankmix[tanksucc];
        current->setParameter("num_inputs",++tankports);
        m->addConnection(new NodeConnection(m->getNode(currentblock->getUUID()),"outs",storagemix[tanksucc],"in_" + storageports));
    }

    return true;
}

void WaterBalance::clear()
{
    tankconversion.clear();
    tank3rdconversion.clear();
    storageconversion.clear();

    if(simreg)
        delete simreg;

    if(nodereg)
        delete nodereg;

    if(s)
        delete s;

    if(sink)
        delete sink;

    simreg=0;
    nodereg=0;
    sink=0;
    s=0;
}

void WaterBalance::initCD3()
{
    sink = new DynaMindStreamLogSink(DM::Logger().sink);
    Log::init(sink, Debug);

    simreg = new SimulationRegistry();
    nodereg = new NodeRegistry();
    s = 0;

    std::map<std::string, Flow::CalculationUnit> flowdef;
    flowdef["Q"]=Flow::flow;
    Flow::undefine();
    Flow::define(flowdef);
}
