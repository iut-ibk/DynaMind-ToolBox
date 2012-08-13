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


#include <simulation.h>
#include <cd3assert.h>
#include <mapbasedmodel.h>
#include <log.h>
#include <logger.h>
#include <logsink.h>
#include <noderegistry.h>
#include <simulationregistry.h>

#include <iostream>
#include <fstream>
#include <QFile>
#include <QTime>

DM_DECLARE_NODE_NAME(WaterBalance,Performance)

WaterBalance::WaterBalance()
{   
    this->cd3logfile = "";
    this->addParameter("CD3 log file", DM::FILENAME, &this->cd3logfile);

    std::vector<DM::View> views;

    //Define Parameter for Sewer network
    //this->sewernetwork = DM::View("CONDUIT", DM::EDGE, DM::READ);
    //views.push_back(this->sewernetwork);

    //Define Parameter for potable water network    
    this->tank = DM::View("TANK", DM::NODE, DM::READ);
    this->tank.addAttribute("Volume");
    views.push_back(this->tank);
    this->tank3rd = DM::View("3RDTANK", DM::NODE, DM::READ);
    this->tank3rd.addAttribute("Volume");
    views.push_back(this->tank3rd);
    views.push_back(DM::View("PIPE",DM::EDGE,DM::READ));
    views.push_back(DM::View("3RDPIPE",DM::EDGE,DM::READ));
    views.push_back(DM::View("DEMAND_NODE",DM::NODE,DM::READ));

    //Define Parameter for non potable water network
    //this->nonpotablenetork = DM::View("3RDPIPE", DM::EDGE, DM::READ);
    //views.push_back(this->nonpotablenetork);

    //Rater data
    this->rdata = DM::View("CATCHMENT", DM::FACE, DM::READ);
    this->rdata.getAttribute("CD3-TYPE");
    this->rdata.getAttribute("RAINFILE");
    this->rdata.getAttribute("POPULATION");
    views.push_back(this->rdata);




    this->addData("City", views);
}

void WaterBalance::run()
{
    if(this->cd3logfile.empty())
    {
        DM::Logger(DM::Error) << "CD3 log file path not set";
        return;
    }

    ofstream logfile;
    logfile.open(this->cd3logfile.c_str());

    ostream *out = &logfile;
    Log::init(new OStreamLogSink(*out), Debug);

    SimulationRegistry *simreg = new SimulationRegistry();
    NodeRegistry *nodereg = new NodeRegistry();
    ISimulation *s = 0;

    try{
        nodereg->addNativePlugin("libdance4water-nodes.so");
        nodereg->addNativePlugin("libnodes.so");
        simreg->addNativePlugin("libnodes.so");

        MapBasedModel m;

        if(!simreg->getRegisteredNames().size())
        {
            DM::Logger(DM::Error) << "CD3 has no valid simulation";
            throw std::exception();
        }

        ISimulation *s = simreg->createSimulation(simreg->getRegisteredNames().front());
        DM::Logger(DM::Debug) << "CD3 Simulation: " << simreg->getRegisteredNames().front();
        s->setModel(&m);

        SimulationParameters p;

        p.dt = lexical_cast<int>("300");
        p.start = time_from_string("2001-Jan-01 00:00:00");
        p.stop = time_from_string("2001-Jan-03 00:00:00");

        s->setSimulationParameters(p);


        ptime starttime = s->getSimulationParameters().start;
        s->start(starttime);
    }
    catch(...)
    {
        DM::Logger(DM::Error) << "Cannot start CD3 simulation";
    }

    if(s)
        delete s;

    delete nodereg;
    delete simreg;

    logfile.close();

    DM::Logger(DM::Debug) << "CD3 simulation finished";
}
