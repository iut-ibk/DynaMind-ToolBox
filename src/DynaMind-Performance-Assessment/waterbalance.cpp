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
#include <math.h>

DM_DECLARE_NODE_NAME(WaterBalance,Performance)

WaterBalance::WaterBalance()
{   
    initvolume=0;

    simreg=0;
    nodereg=0;
    sink=0;
    s=0;
    p=0;

    std::vector<DM::View> views;
    DM::View view;

    //Define Parameter for potable water network    
    view = DM::View("TANK", DM::NODE, DM::READ);
    view.getAttribute("Id");
    view.getAttribute("InCapacity");
    view.getAttribute("MaxVolume");
    view.getAttribute("Successor");
    view.getAttribute("UDTank");
    view.getAttribute("InitVolume");
    view.addAttribute("Volumecurve");
    views.push_back(view);
    viewdef["Tank"]=view;

    view = DM::View("PIPE", DM::EDGE, DM::READ);
    view.getAttribute("Id");
    views.push_back(view);
    viewdef["Pipe"]=view;

    //Define Parameter for nonpotable water network
    view = DM::View("3RDTANK", DM::NODE, DM::READ);
    view.getAttribute("Id");
    view.getAttribute("InCapacity");
    view.getAttribute("MaxVolume");
    view.getAttribute("Successor");
    view.getAttribute("UDTank");
    view.getAttribute("InitVolume");
    view.addAttribute("Volumecurve");
    views.push_back(view);
    viewdef["3rdTank"]=view;

    view = DM::View("3RDPIPE", DM::EDGE, DM::READ);
    view.getAttribute("Id");
    views.push_back(view);
    viewdef["3rdPipe"]=view;

    //Catchment
    view = DM::View("CATCHMENT", DM::FACE, DM::READ);
    view.getAttribute("3rdTank");
    view.getAttribute("Blockmodel");
    view.getAttribute("Population");
    view.getAttribute("Tank");
    view.getAttribute("UDTank");
    views.push_back(view);
    viewdef["Block"]=view;

    //Define Parameter for ud network
    view = DM::View("STORAGE", DM::NODE, DM::READ);
    view.getAttribute("Id");
    view.getAttribute("InCapacity");
    view.getAttribute("MaxVolume");
    view.getAttribute("Successor");
    view.getAttribute("UDTank");
    view.getAttribute("InitVolume");
    view.addAttribute("Volumecurve");
    views.push_back(view);
    viewdef["UDTank"]=view;

    view = DM::View("CONDUIT", DM::EDGE, DM::READ);
    view.getAttribute("Id");
    views.push_back(view);
    viewdef["Conduit"]=view;

    this->addData("City", views);
}

void WaterBalance::run()
{
    clear();
    initmodel();

    try{
        nodereg->addNativePlugin("Modules/libdance4water-nodes.so");
        nodereg->addNativePlugin("libnodes.so");
        simreg->addNativePlugin("libnodes.so");

        p = new SimulationParameters();
        p->dt = lexical_cast<int>("60");
        p->start = time_from_string("2001-Jan-01 00:00:00");
        p->stop = time_from_string("2001-Feb-02 00:00:00");

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

        s->setSimulationParameters(*p);

        ptime starttime = s->getSimulationParameters().start;
        m.initNodes(s->getSimulationParameters());
        s->start(starttime);

        //Check result;
        double balance = -initvolume;

        balance += extractVolumeResult(&m,"Tank");
        balance += extractVolumeResult(&m,"3rdTank");
        balance += extractVolumeResult(&m,"UDTank");
        balance -= extractTotalBlockVolume(&m);

        if(abs(balance) > 0.1)
            DM::Logger(DM::Warning) << "Water Balance error > 0.1 m^3: " << balance << " m^3. Maybe simulation steps too high ?";

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

    std::vector<std::string> names;
    names.push_back("Tank");
    names.push_back("3rdTank");
    names.push_back("UDTank");

    for(int index=0; index < names.size(); index++)
    {
        conversions[names[index]] = new stringmap();

        if(!createTanks(m,viewdef[names[index]], conversions[names[index]]))
        {
            DM::Logger(DM::Error) << names[index] << " definition not valid";
            return false;
        }
    }

    if(!createBlocks(m))
    {
        DM::Logger(DM::Error) << "Block definition not valid";
        return false;
    }

    if(!connectBlocks(m))
    {
        DM::Logger(DM::Error) << "Block connection definition not valid";
        return false;
    }

    for(int index=0; index < names.size(); index++)
    {
        if(!connectTanks(m,names[index]))
            return false;
    }

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
        cd3tank->setParameter("initvolume",currenttank->getAttribute("InitVolume")->getDouble());
        initvolume += maxvolume[0]*currenttank->getAttribute("InitVolume")->getDouble()/100;

        m->addNode(currenttank->getUUID(),cd3tank);

        (*ids)[QString::number(currenttank->getAttribute("Id")->getDouble()).toStdString()]=currenttank->getUUID();
        checkconnection(m, currenttank->getUUID());
    }

    return true;
}

bool WaterBalance::createBlocks(MapBasedModel *m)
{
    std::vector<std::string> blockids = city->getUUIDsOfComponentsInView(viewdef["Block"]);

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
        const_flow[0] = 0.001;
        cd3block->setParameter("const_flow_sewer",const_flow);

        if(currentblock->getAttribute("Tank")->getString() == "1")
            const_flow[0] *= -1;

        cd3block->setParameter("const_flow_nonpotable",const_flow);
        cd3block->setParameter("const_flow_potable",const_flow);


        m->addNode(currentblock->getUUID(),cd3block);
    }

    return true;
}

bool WaterBalance::connectBlocks(MapBasedModel *m)
{
    std::vector<std::string> blockids = city->getUUIDsOfComponentsInView(viewdef["Block"]);

    for(int index=0; index<blockids.size(); index++)
    {
        DM::Face *currentblock = city->getFace(blockids[index]);

        if(currentblock->getAttribute("Blockmodel")->getString().empty())
            continue;

        connectBlock(m,currentblock,"Tank");
        connectBlock(m,currentblock,"3rdTank");
        connectBlock(m,currentblock,"UDTank");
    }

    return true;
}

bool WaterBalance::connectBlock(MapBasedModel *m, DM::Face *currentblock, std::string type)
{
    if(conversions[type]->find(currentblock->getAttribute(type)->getString())==conversions[type]->end())
    {
        for(stringmap::iterator i(conversions[type]->begin()); i != conversions[type]->end(); i++)
            DM::Logger(DM::Debug) << "ValidTank: " << (*i).second;

        DM::Logger(DM::Warning) << "Cannot connect to a not existing tank: Type=" << type << " Name=" << currentblock->getAttribute(type)->getString();
        return false;
    }

    std::string pname;

    if(type=="Tank")
        pname="outp";

    if(type=="3rdTank")
        pname="outnp";

    if(type=="UDTank")
        pname="outs";

    std::string successor = (*conversions[type])[currentblock->getAttribute(type)->getString()];
    return connect(m,type,successor,m->getNode(currentblock->getUUID()),pname);
}

bool WaterBalance::connectTanks(MapBasedModel *m, std::string type)
{
    std::vector<std::string> tankids = city->getUUIDsOfComponentsInView(viewdef[type]);

    for(int index=0; index<tankids.size(); index++)
    {
        DM::Node *tank = city->getNode(tankids[index]);

        if(!tank->getAttribute("Successor")->getString().empty())
            if(!connect(m,type,(*conversions[type])[tank->getAttribute("Successor")->getString()],m->getNode(tank->getUUID()),"out"))
                return false;

        if(!tank->getAttribute("UDTank")->getString().empty())
            if(!connect(m,"UDTank",(*conversions["UDTank"])[tank->getAttribute("UDTank")->getString()],m->getNode(tank->getUUID()),"overflow"))
                return false;
    }

    return true;
}

bool WaterBalance::connect(MapBasedModel *m, string type, std::string successor, Node *n, string outportname)
{
    if(successor.empty() || successor=="NULL")
    {
        DM::Logger(DM::Warning) << "Connection to " << type << " namend " << successor << " not possible";
        return false;
    }

    mixerports[successor]=mixerports[successor]+1;
    mixers[successor]->setParameter("num_inputs",mixerports[successor]);

    mixers[successor]->init(p->start,p->stop,p->dt);

    if(mixers[successor]!=m->getNode(successor+"_Mixer"))
        return false;

    m->addConnection(new NodeConnection(n,outportname,mixers[successor],"in_" + QString::number(mixerports[successor]-1).toStdString()));
    mixers[successor]->deinit();
    return true;
}

void WaterBalance::checkconnection(MapBasedModel *m, string name)
{
    if(mixers.find(name)==mixers.end())
    {
        //create new mixer
        mixers[name] = nodereg->createNode("Mixer");
        mixerports[name] = 0;
        m->addNode(name + "_Mixer" , mixers[name]);
        mixers[name]->setParameter("num_inputs",0);
        m->addConnection(new NodeConnection(mixers[name],"out",m->getNode(name),"in"));
    }
}

double WaterBalance::extractVolumeResult(MapBasedModel *m, string type)
{
    std::vector<std::string> tankids = city->getUUIDsOfComponentsInView(viewdef[type]);
    double totalvolume = 0;

    //write results
    for(int index=0; index<tankids.size(); index++)
    {
        DM::Node *currenttank = city->getNode(tankids[index]);
        Node *cd3tank = m->getNode(tankids[index]);

        DM::Attribute a("Volumecurve");
        a.setDoubleVector(*(cd3tank->getState<std::vector<double> >("TankVolume")));
        totalvolume += a.getDoubleVector()[a.getDoubleVector().size()-1];
        currenttank->addAttribute(a);
    }

    return totalvolume;
}

double WaterBalance::extractTotalBlockVolume(MapBasedModel *m)
{
    double totalvolume;

    std::vector<std::string> blockids = city->getUUIDsOfComponentsInView(viewdef["Block"]);

    for(int index=0; index<blockids.size(); index++)
    {
        DM::Face *currentblock = city->getFace(blockids[index]);

        if(currentblock->getAttribute("Blockmodel")->getString().empty())
            continue;

        Node *cd3block = m->getNode(currentblock->getUUID());
        totalvolume += (*cd3block->getState<double>("TotalVolume"));
    }

    return totalvolume;
}

void WaterBalance::clear()
{
    initvolume=0;

    mixers.clear();
    mixerports.clear();

    for(conversionmap::iterator i(conversions.begin()); i != conversions.end(); i++)
        delete (*i).second;

    conversions.clear();

    if(simreg)
        delete simreg;

    if(p)
        delete p;

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
    p=0;
}

void WaterBalance::initmodel()
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
