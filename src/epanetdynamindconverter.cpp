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

#include <epanetdynamindconverter.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//Watersupply
#include <dmepanet.h>

using namespace DM;

EpanetDynamindConverter::EpanetDynamindConverter()
{
    openedepanetfile=false;
}

EPANETModelCreator *EpanetDynamindConverter::getCreator()
{
    return &creator;
}

bool EpanetDynamindConverter::createEpanetModel(System *sys, string inpfilepath)
{
    DM::WS::ViewDefinitionHelper wsd;
    typedef std::map<std::string, DM::Component*> cmap;
    cmap::iterator itr;

    //SET OPTIONS
    if(!creator.setOptionUnits(EPANETModelCreator::LPS))return false;
    if(!creator.setOptionHeadloss(EPANETModelCreator::DW))return false;

    //JUNCTIONS
    cmap junctions = sys->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::READ));

    for(itr = junctions.begin(); itr != junctions.end(); ++itr)
        if(!addJunction(static_cast<DM::Node*>((*itr).second)))return false;

    //RESERVOIRS
    cmap reservoir = sys->getAllComponentsInView(wsd.getView(DM::WS::RESERVOIR, DM::READ));

    for(itr = reservoir.begin(); itr != reservoir.end(); ++itr)
        if(!addReservoir(static_cast<DM::Node*>((*itr).second)))return false;

    //TANKS
    cmap tank = sys->getAllComponentsInView(wsd.getView(DM::WS::TANK, DM::READ));

    for(itr = tank.begin(); itr != tank.end(); ++itr)
        if(!addTank(static_cast<DM::Node*>((*itr).second)))return false;

    //PIPES
    cmap pipes = sys->getAllComponentsInView(wsd.getView(DM::WS::PIPE, DM::READ));

    for(itr = pipes.begin(); itr != pipes.end(); ++itr)
        if(!addPipe(static_cast<DM::Edge*>((*itr).second)))return false;


    if(!creator.save(inpfilepath))return false;

    return true;
}

bool EpanetDynamindConverter::mapEpanetAttributes(System *sys)
{
    //TODO map more attributes (currently only the diameter of a pipe is extracted)
    if(!mapPipeAttributes(sys))
        return false;

    return true;
}

bool EpanetDynamindConverter::mapPipeAttributes(System *sys)
{
    //TODO map more attributes (currently only the diameter of a pipe is extracted)
    if(!openedepanetfile)
        return false;

    DM::WS::ViewDefinitionHelper wsd;
    typedef std::map<std::string, DM::Component*> cmap;
    cmap::iterator itr;

    cmap pipes = sys->getAllComponentsInView(wsd.getView(DM::WS::PIPE, DM::READ));

    for(itr = pipes.begin(); itr != pipes.end(); ++itr)
    {
        DM::Edge *currentedge = static_cast<DM::Edge*>((*itr).second);
        float currentdiameter;
        char name[256];
        strcpy(name, QString::number(components[currentedge]).toStdString().c_str());
        int index;
        if(!checkENRet(EPANET::ENgetlinkindex(name, &index)))
            return false;

        if(!checkENRet(EPANET::ENgetlinkvalue(index,EN_DIAMETER,&currentdiameter)))
            return false;

        currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter),currentdiameter);
    }

    return true;
}

bool EpanetDynamindConverter::checkENRet(int ret)
{
    if(!openedepanetfile)
    {
        DM::Logger(DM::Error) << "Please open EPANET model with EpanetDynamindConverter";
        return false;
    }

    if(ret>0)
    {
        const uint SIZE = 100;
        char errorstring[SIZE];
        EPANET::ENgeterror(ret,errorstring,SIZE);

        if(ret > 7)
        {
            DM::Logger(DM::Error) << "EPANET error code: " << ret << " (" << errorstring << ")";
            return false;
        }
        else
        {
            DM::Logger(DM::Warning) << "EPANET warning code: " << ret << " (" << errorstring << ")";
            return true;
        }
    }

    return true;
}

bool EpanetDynamindConverter::addJunction(DM::Node *junction)
{
    double x, y , elevation, demand;
    x = junction->getX();
    y = junction->getY();
    elevation = junction->getZ();
    demand = junction->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble();

    uint index = creator.addJunction(x,y,elevation,demand);

    if(!index)
        return false;

    components[junction]=index;

    return true;
}

bool EpanetDynamindConverter::addReservoir(DM::Node *reservoir)
{
    double x = reservoir->getX();
    double y = reservoir->getY();
    double head = reservoir->getZ();
    std::string headpattern = "";

    uint index = creator.addReservoir(x,y,head,headpattern);

    if(!index)
        return false;

    components[reservoir]=index;

    return true;
}

bool EpanetDynamindConverter::addTank(DM::Node *tank)
{
    double x = tank->getX();
    double y = tank->getY();
    double bottomelevation = tank->getZ();
    double initiallevel = 5;
    double minlevel = 0;
    double maxlevel = 5;
    double nominaldiameter = 10;
    double minvolume = 0;
    std::string volumecurve = "";

    uint index = creator.addTank(x,y,bottomelevation,initiallevel,minlevel,maxlevel,nominaldiameter,minvolume,volumecurve);

    if(!index)
        return false;

    components[tank]=index;

    return true;
}

bool EpanetDynamindConverter::addPipe(DM::Edge *pipe)
{
    int startnode = components[pipe->getStartNode()];
    int endnode = components[pipe->getEndNode()];

    double length = pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Length))->getDouble();
    double diameter = pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter))->getDouble();
    double roughness = 0.04;
    double minorloss = 0;

    uint index = creator.addPipe(startnode, endnode,length,diameter,roughness,minorloss,EPANETModelCreator::OPEN);

    if(!index)
        return false;

    if(!creator.addVertex(pipe->getStartNode()->getX(),
                             pipe->getStartNode()->getY(),
                             pipe->getEndNode()->getX(),
                             pipe->getEndNode()->getY(),QString::number(index)))
        return false;

    components[pipe]=index;

    return true;
}

bool EpanetDynamindConverter::openEpanetModel(string inpfilepath, string rptfilepath)
{
    char inpfile[256];
    strcpy(inpfile,inpfilepath.c_str());
    char rptfile[256];
    strcpy(rptfile,rptfilepath.c_str());

    openedepanetfile=true;
    if(!checkENRet(EPANET::ENopen(inpfile,rptfile,"")))
    {
        openedepanetfile=false;
        return false;
    }

    return true;
}

bool EpanetDynamindConverter::closeEpanetModel()
{
    if(checkENRet(EPANET::ENclose()))
    {
        openedepanetfile=false;
        return true;
    }

    return false;
}
