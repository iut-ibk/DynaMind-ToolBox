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

#include <swmmdynamindconverter.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

using namespace DM;

SWMMDynamindConverter::SWMMDynamindConverter()
{
    openedswmmfile=false;
}

SWMMModelCreator *SWMMDynamindConverter::getCreator()
{
	return &creator;
}

bool SWMMDynamindConverter::createSWMMModel(System *sys, string inpfilepath, string rainfile)
{
    DM::S::ViewDefinitionHelper sd;
	typedef std::map<std::string, DM::Component*> cmap;
	cmap::iterator itr;

	//SET OPTIONS
    if(!creator.setOptionFlowUnits(SWMMModelCreator::LPS))return false;

    //SET RAINGAGES

	//JUNCTIONS
    cmap junctions = sys->getAllComponentsInView(sd.getView(DM::S::JUNCTION, DM::READ));

	for(itr = junctions.begin(); itr != junctions.end(); ++itr)
		if(!addJunction(static_cast<DM::Node*>((*itr).second)))return false;

    //OUTFALLS
    cmap outfalls = sys->getAllComponentsInView(sd.getView(DM::S::OUTFALL, DM::READ));

    for(itr = outfalls.begin(); itr != outfalls.end(); ++itr)
        if(!addOutfall(static_cast<DM::Node*>((*itr).second)))return false;

    //CONDUITS
    cmap conduits = sys->getAllComponentsInView(sd.getView(DM::S::CONDUIT, DM::READ));

    for(itr = conduits.begin(); itr != conduits.end(); ++itr)
        if(!addConduit(static_cast<DM::Edge*>((*itr).second)))return false;

    //Catchments
    cmap inlets = sys->getAllComponentsInView(sd.getView(DM::S::INLET, DM::READ));

    for(itr = inlets.begin(); itr != inlets.end(); ++itr)
    {
        DM::Node *currentinlet = static_cast<DM::Node*>((*itr).second);
        string cid = currentinlet->getAttribute(sd.getAttributeString(DM::S::INLET,DM::S::INLET_ATTR_DEF::CATCHMENT))->getString();
        DM::Face *currentcatchment = static_cast<DM::Face*>(sys->getComponent(cid));

        if(currentcatchment==0)
            continue;

        if(!addCatchment(currentcatchment,currentinlet,rainfile))return false;
    }


	if(!creator.save(inpfilepath))return false;

	return true;
}

bool SWMMDynamindConverter::mapSWMMAttributes(System *sys)
{
    //TODO implement
    DM::Logger(DM::Warning) << "Reading SWMM results is currently not implemented";

    return false;
}

bool SWMMDynamindConverter::addJunction(DM::Node *junction)
{
    double x, y , elevation;
	x = junction->getX();
	y = junction->getY();
	elevation = junction->getZ();
    double DWF = junction->getAttribute(sd.getAttributeString(DM::S::JUNCTION,DM::S::JUNCTION_ATTR_DEF::DWF))->getDouble();

    uint index = creator.addJunction(x,y,elevation,DWF);

	if(!index)
		return false;

	components[junction]=index;

    return true;
}

bool SWMMDynamindConverter::addOutfall(Node *outfall)
{
    double x, y , elevation;
    x = outfall->getX();
    y = outfall->getY();
    elevation = outfall->getZ();

    uint index = creator.addOutfall(x,y,elevation);

    if(!index)
        return false;

    components[outfall]=index;

    return true;
}

bool SWMMDynamindConverter::addCatchment(Face *catchment, Node *inlet, string rainfile)
{
    if(!components.contains(inlet))
        return false;

    uint outlet = components[inlet];
    double area = catchment->getAttribute(sd.getAttributeString(DM::S::CATCHMENT,DM::S::CATCHMENT_ATTR_DEF::Area))->getDouble();
    double imperv = catchment->getAttribute(sd.getAttributeString(DM::S::CATCHMENT,DM::S::CATCHMENT_ATTR_DEF::Impervious))->getDouble();
    double raingagenr = catchment->getAttribute(sd.getAttributeString(DM::S::CATCHMENT,DM::S::CATCHMENT_ATTR_DEF::Raingage))->getDouble();
    string raingage = "RG" + QString::number(raingagenr).toStdString();

    uint index = creator.addSubCatchment(raingage,outlet,area,imperv);

    if(!index)
        return false;

    components[catchment]=index;

    //add vertex of polygon
    std::vector<DM::Node*> nodes = catchment->getNodePointers();
    for(uint nindex=0; nindex<nodes.size(); nindex++)
        creator.addPolygonVertex(nodes[nindex]->getX(),nodes[nindex]->getY(),QString::number(index));

    index = creator.addRainGage(raingage,SWMMModelCreator::VOLUME,"0:05",1.0, "FILE", rainfile,raingage,SWMMModelCreator::MM);

    return true;
}

bool SWMMDynamindConverter::addConduit(DM::Edge *conduit)
{
    DM::Node *startnode = conduit->getStartNode();
    DM::Node *endnode = conduit->getEndNode();

    if(!components.contains(startnode))
        return false;

    if(!components.contains(endnode))
        return false;

    double length = conduit->getAttribute(sd.getAttributeString(DM::S::CONDUIT,DM::S::CONDUIT_ATTR_DEF::Length))->getDouble();
    double diameter = conduit->getAttribute(sd.getAttributeString(DM::S::CONDUIT,DM::S::CONDUIT_ATTR_DEF::Diameter))->getDouble();
    double N = 0.01;
    double Z1 = 0;
    double Z2 = 0;

    uint index = creator.addConduit(components[startnode],components[endnode],diameter,length,N,Z1,Z2);

    if(!index)
        return false;

    components[conduit]=index;

	return true;
}
