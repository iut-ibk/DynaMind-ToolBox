/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair

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

#include "compilersettings.h"

#include <iostream>
#include <simulation.h>
#include <dmdatabase.h>
#include <vibe_log.h>
#include <datamanagement.h>
#include <QThreadPool>
#include <module.h>
#include <DM.h>
#include <sstream>

using namespace std;
using namespace vibens;

bool DynaMiteTest();
bool DMBaseTest();

int main(int argc, char *argv[], char *envp[]) {

    //Init Logger
    ostream *out = &cout;
    vibens::Log::init(new OStreamLogSink(*out), vibens::Debug);
    vibens::Logger(vibens::Debug) << "Start";

    if(!DynaMiteTest())
       Logger(Error) << "DynaMiteTest FAILED";
   else
        Logger() << "DynaMiteTest DONE";

  /*  if(!DMBaseTest())
        Logger(Error) << "DMBaseTest FAILED";
    else
        Logger() << "DMBaseTest DONE";

    if(!DMBaseTest())
        Logger(Error) << "DMBaseTest_2 FAILED";
    else
        Logger() << "DMBaseTest_2 DONE";*/

    return 1;
}

bool DynaMiteTest()
{
    QThreadPool::globalInstance()->setMaxThreadCount(1);
    DataManagement::init();
    DMDatabase * db = new DMDatabase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim = new Simulation;
    sim->registerNativeModules("dmtestmodule");
    vibens::Module * in = sim->addModule("TestModule");
    vibens::Module * outm =sim->addModule("InOut");

    sim->addLink(in->getOutPort("Points"), outm->getInPort("Inport"));
    sim->run();

    QThreadPool::globalInstance()->waitForDone();
    delete sim;
    vibens::Logger(vibens::Debug) << "End";

    return true;
}

bool DMBaseTest()
{
    //create new System (VectorData)
    DM::System* sys = new DM::System("firstsys","1");

    //add some Nodes with attributes
    int nrnodes = 100;
    nrnodes=nrnodes+10;

    for(int index=0; index<nrnodes; index++)
    {
        std::stringstream ss;
        ss << index;
        if(!sys->addNode(new DM::Node(std::string("node_" + ss.str()), std::string("node_id_" + ss.str()),index,index,index)))
        {
            delete sys;
            Logger(Error) << "Node" + ss.str();
            return false;
        }

        DM::Node* tmpNode = sys->getNode("node_" + ss.str());
        if(!tmpNode->addAttribute(new DM::Attribute(std::string("attr_" + ss.str()), std::string("attr_id_" + ss.str()))))
        {
            delete sys;
            Logger(Error) << "Attr" + ss.str();
            return false;
        }
    }

    //add some edges with attributes
    for(int index=1; index<nrnodes; index++)
    {
        std::stringstream start, end,ss;
        start << (index-1);
        end << index;
        ss << index;
        if(!sys->addEdge(new DM::Edge(std::string("edge_" + ss.str()), std::string("edge_id_" + ss.str()),std::string("node_" + start.str()), std::string("node_" + end.str()))))
        {
            delete sys;
            Logger(Error) << "Edge" + ss.str();
            return false;
        }

        DM::Edge* tmpEdge = sys->getEdge("edge_" + ss.str());
        if(!tmpEdge->addAttribute(new DM::Attribute(std::string("attr_" + ss.str()), std::string("attr_id_" + ss.str()))))
        {
            delete sys;
            Logger(Error) << "Attr" + ss.str();
            return false;
        }
    }

    //create successorstate
    DM::System* succ = sys->createSuccessor();

    //check pointers in successor state :: NODES
    for(int index=0; index<nrnodes; index++)
    {
        std::stringstream ss;
        ss << index;
        DM::Node* tmpNode = succ->getNode("node_" + ss.str());
        if(!tmpNode)
        {
            delete sys;
            delete succ;
            Logger(Error) << "Cannot find node in successor state: node_" + ss.str();
            return false;
        }

        if(tmpNode->getAllAttributes().size()!=1)
        {
            delete sys;
            delete succ;
            Logger(Error) << "Not enough attributes in node: node_" + ss.str();
            return false;
        }

        if(!tmpNode->getAttribute("attr_" + ss.str()))
        {
            delete sys;
            delete succ;
            Logger(Error) << "Cannot find attribute in: node_" + ss.str();
            return false;
        }
    }

    //check pointers in successor state :: EDGES
    for(int index=1; index<nrnodes; index++)
    {
        std::stringstream ss;
        ss << index;
        DM::Edge* tmpEdge = succ->getEdge("edge_" + ss.str());

        if(!tmpEdge)
        {
            delete sys;
            delete succ;
            Logger(Error) << "Cannot find edge in successor state: edge_" + ss.str();
            return false;
        }

        if(tmpEdge->getAllAttributes().size()!=1)
        {
            delete sys;
            delete succ;
            Logger(Error) << "Not enough attributes in edge: edge_" + ss.str();
            return false;
        }

        if(!tmpEdge->getAttribute("attr_" + ss.str()))
        {
            delete sys;
            delete succ;
            Logger(Error) << "Cannot find attribute in: edge_" + ss.str();
            return false;
        }
    }

    //delete edge
    if(!succ->removeEdge("edge_3"))
    {
        delete sys;
        delete succ;
        Logger(Error) << "Cannot remove edge in successor state";
        return false;
    }

    //check if edge is realy removed
    if(succ->getEdge("edge_3"))
    {
        delete sys;
        delete succ;
        Logger(Error) << "Edge not removed in successor state: edge_2";
        return false;
    }


    //check if edge_0 exists in first state
    if(!sys->getEdge("edge_3"))
    {
        delete sys;
        delete succ;
        Logger(Error) << "Cannot find edge in first state";
        return false;
    }

    //delete node
    if(!succ->removeNode("node_1"))
    {
        delete sys;
        delete succ;
        Logger(Error) << "Cannot remove node in successor state";
        return false;
    }

    //check if node is realy removed
    if(succ->getNode("node_1"))
    {
        delete sys;
        delete succ;
        Logger(Error) << "Node not removed in successor state";
        return false;
    }


    //check if node_0 exists in first state
    if(!sys->getNode("node_1"))
    {
        delete sys;
        delete succ;
        Logger(Error) << "Cannot find node in first state";
        return false;
    }

    //check if edge is realy removed
    if(succ->getEdge("edge_1"))
    {
        delete sys;
        delete succ;
        Logger(Error) << "Edge not removed in successor state: edge_1";
        return false;
    }


    //check if egde_1 exists in first state
    if(!sys->getEdge("edge_1"))
    {
        delete sys;
        delete succ;
        Logger(Error) << "Cannot find edge in first state";
        return false;
    }

    delete sys;
    delete succ;
    return true;
}
