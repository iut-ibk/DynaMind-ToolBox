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

#include <epanetmodelcreator.h>

EPANETModelCreator::EPANETModelCreator(bool vertex)
{
    #define X(a) #a,
    ComponentStrings = {TYPES};
    #undef X
    #undef TYPES

    UnitStrings = {"CFS", "GPM", "MGD", "IMGD", "AFD", "LPS", "LPM", "MLD", "CMH", "CMD"};
    HeadlossStrings = {"H-W","D-W","C-M"};
    UnbalancedStrings = {"STOP","CONTINUE"};
    HydraulicsStrings = {"USE", "SAVE"};

    this->vertex=vertex;
    for(uint index=0; index<ComponentStrings.size(); index++)
        model[static_cast<ComponentTypes>(index)] = boost::make_shared<EpanetElements>();

    initModel();
}

bool EPANETModelCreator::addJunction(DM::Node *junction)
{
    components.push_back(junction);
    int cindex = components.size()-1;
    QString id = QString::number(cindex);

    QString result = "";

    result += id + "\t";
    result += QString::number(junction->getZ()) + "\t";
    result += QString::number(junction->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble()) + "\t";

    (*model[EPANETModelCreator::JUNCTIONS])[id] = result;

    if(vertex)
        return addCoordinate(junction,QString::number(cindex));

    return true;
}

bool EPANETModelCreator::addReservoir(DM::Node *reservoir)
{
    components.push_back(reservoir);
    int cindex = components.size()-1;
    QString id = QString::number(cindex);

    QString result = "";

    result += QString::number(cindex) + "\t";
    result += QString::number(reservoir->getZ()) + "\t";

    (*model[EPANETModelCreator::RESERVOIRS])[id]=result;

    if(vertex)
        return addCoordinate(reservoir,QString::number(cindex));

    return true;
}

bool EPANETModelCreator::addTank(DM::Node *tank)
{
    components.push_back(tank);
    int cindex = components.size()-1;
    QString id = QString::number(cindex);

    QString result = "";

    result += QString::number(cindex) + "\t";
    result += QString::number(tank->getZ()) + "\t";
    result += QString::number(5) + "\t";
    result += QString::number(0) + "\t";
    result += QString::number(5) + "\t";
    result += QString::number(10) + "\t";
    result += QString::number(0) + "\t";

    (*model[EPANETModelCreator::TANKS])[id]=result;

    if(vertex)
        return addCoordinate(tank,QString::number(cindex));

    return true;
}

bool EPANETModelCreator::addPipe(DM::Edge *pipe)
{
    QString id;
    uint startnode, endnode, cindex;
    components.push_back(pipe);
    cindex = components.size()-1;
    id = QString::number(cindex);
    startnode = components.indexOf(pipe->getStartNode());
    endnode = components.indexOf(pipe->getEndNode());

    QString result = "";
    result += id + "\t";
    result += QString::number(startnode) + "\t";
    result += QString::number(endnode) + "\t";
    result += QString::number(pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Length))->getDouble()) + "\t";
    result += QString::number(pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter))->getDouble()) + "\t";
    result += "0.04\t";
    result += "0\t";
    result += "OPEN\t";

    (*model[EPANETModelCreator::PIPES])[id]=result;

    if(vertex)
        return addVertex(pipe,QString::number(cindex));

    return true;
}

bool EPANETModelCreator::addCoordinate(DM::Node *node, QString id)
{
    QString result = "";
    result += id + "\t";
    result += QString::number(node->getX()) + "\t";
    result += QString::number(node->getY()) + "\t";
    (*model[EPANETModelCreator::COORDINATES])[id]=result;
    return true;
}

bool EPANETModelCreator::addVertex(DM::Edge *edge, QString id)
{
    DM::Node *startnode, *endnode;
    startnode = edge->getStartNode();
    endnode = edge->getEndNode();

    QString result = "";
    result += id + "\t";
    result += QString::number(startnode->getX()) + "\t";
    result += QString::number(startnode->getY()) + "\n";
    result += id + "\t";
    result += QString::number(endnode->getX()) + "\t";
    result += QString::number(endnode->getY()) + "\t";
    (*model[EPANETModelCreator::VERTICES])[id]=result;

    return true;
}

bool EPANETModelCreator::save(string filepath)
{
    if(QFile::exists(QString::fromStdString(filepath)))
        if(!QFile::remove(QString::fromStdString(filepath)))
            return false;

    QFile file(QString::fromStdString(filepath));
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    for(uint index=0; index<ComponentStrings.size(); index++)
    {
        out << "\n[" << QString::fromStdString(ComponentStrings[index]) + "]\n";
        boost::shared_ptr<EpanetElements> elements = model[static_cast<ComponentTypes>(index)];
        EpanetElements::iterator itr;

        for(itr=elements.get()->begin(); itr!=elements.get()->end(); ++itr)
            out << itr->second << "\n";
    }

    file.close();
    return true;
}

bool EPANETModelCreator::setOptionUnits(EPANETModelCreator::UNITS unit)
{
    QString result = "Units\t" + UnitStrings[unit];
    (*model[EPANETModelCreator::OPTIONS])["Units"]=result;
    return true;
}

bool EPANETModelCreator::setOptionHeadloss(EPANETModelCreator::HEADLOSS headloss)
{
    QString result = "Headloss\t" + HeadlossStrings[headloss];
    (*model[EPANETModelCreator::OPTIONS])["Headloss"]=result;
    return true;
}

bool EPANETModelCreator::setOptionHydraulics(HYDRAULICS hydraulics, std::string filename)
{
    QString result = "Hydraulics\t" + HydraulicsStrings[hydraulics] + "\t" + QString::fromStdString(filename);
    (*model[EPANETModelCreator::OPTIONS])["Hydraulics"]=result;
    return true;
}

bool EPANETModelCreator::setOptionViscosity(double value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Viscosity",QString::number(value));
}

bool EPANETModelCreator::setOptionDiffusivity(double value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Diffusivity",QString::number(value));
}

bool EPANETModelCreator::setOptionSpecificGrafity(double value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Specific Grafity",QString::number(value));
}

bool EPANETModelCreator::setOptionTrails(int value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Trails",QString::number(value));
}

bool EPANETModelCreator::setOptionAccuracy(double value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Accuracy",QString::number(value));
}

bool EPANETModelCreator::setOptionUnbalanced(UNBALANCED type, int value)
{
    QString number = "";
    if(value > -1)
        number = QString::number(value);

    QString result = UnbalancedStrings[type] + "\t" + number;
    return addEntry(EPANETModelCreator::OPTIONS,"Unbalanced",result);
}

bool EPANETModelCreator::setOptionPattern(double value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Pattern",QString::number(value));
}

bool EPANETModelCreator::setOptionDemandMultiplier(double value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Multiplier",QString::number(value));
}

bool EPANETModelCreator::setOptionEmitterExponent(double value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Emitter Exponent",QString::number(value));
}

bool EPANETModelCreator::setOptionTolerance(double value)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Tolerance",QString::number(value));
}

bool EPANETModelCreator::setOptionMap(std::string filename)
{
    return addEntry(EPANETModelCreator::OPTIONS,"Map",QString::fromStdString(filename));
}

bool EPANETModelCreator::addEntry(EPANETModelCreator::ComponentTypes type, QString id, QString values)
{
    QString result = id + "\t" + values;
    (*model[type])[id]=result;
    return true;
}

void EPANETModelCreator::initModel()
{
    //OPTIONS
    setOptionUnits(EPANETModelCreator::LPS);
    setOptionHeadloss(EPANETModelCreator::DW);
    setOptionSpecificGrafity(1);
    setOptionViscosity(1);
    //setOptionTrails(40);
    setOptionAccuracy(0.001);
    setOptionUnbalanced(CONTINUE,10);
    setOptionPattern(1);
    //setOptionDemandMultiplier(1.0);
    setOptionEmitterExponent(0.5);
    setOptionDiffusivity(1);
    setOptionTolerance(0.01);

    //TIMES
    addEntry(TIMES,"Duration","2:00");
    addEntry(TIMES,"Hydraulic Timestep","0:05");
    addEntry(TIMES,"Quality Timestep","0:05");
    addEntry(TIMES,"Pattern Timestep","0:00");
    addEntry(TIMES,"Pattern Start","0:00");
    addEntry(TIMES,"Report Timestep","0:05");
    addEntry(TIMES,"Report Start","1:00");
    addEntry(TIMES,"Start ClockTime","12 am");
    addEntry(TIMES,"Statistic","NONE");

    //REPORT
    addEntry(REPORT,"Status","No");
    addEntry(REPORT,"Summary","No");
    addEntry(REPORT,"Page","0");
    addEntry(REPORT,"Messages","No");
}
