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
    #define X(a) ComponentStrings.push_back(#a);
    TYPES;
    #undef X
    #undef TYPES

    UnitStrings.push_back("CFS");
    UnitStrings.push_back("GPM");
    UnitStrings.push_back("MGD");
    UnitStrings.push_back("IMGD");
    UnitStrings.push_back("AFD");
    UnitStrings.push_back("LPS");
    UnitStrings.push_back("LPM");
    UnitStrings.push_back("MLD");
    UnitStrings.push_back("CMH");
    UnitStrings.push_back("CMD");

    HeadlossStrings.push_back("H-W");
    HeadlossStrings.push_back("D-W");
    HeadlossStrings.push_back("C-M");

    UnbalancedStrings.push_back("STOP");
    UnbalancedStrings.push_back("CONTINUE");

    HydraulicsStrings.push_back("USE");
    HydraulicsStrings.push_back("SAVE");

    PipeStatusString.push_back("OPEN");
    PipeStatusString.push_back("CLOSED");
    PipeStatusString.push_back("CV");

    cindex=0;
    this->vertex=vertex;
    for(uint index=0; index<ComponentStrings.size(); index++)
        model[static_cast<ComponentTypes>(index)] = boost::make_shared<EpanetElements>();

    initModel();
}

uint EPANETModelCreator::addJunction(double x, double y, double elevation, double basedemand, std::string demandpattern)
{
    cindex++;
    QString id = QString::number(cindex);

    QString result = "";

    result += id + "\t";
    result += QString::number(elevation) + "\t";
    result += QString::number(basedemand) + "\t";
    result += QString::fromStdString(demandpattern) + "\t";

    (*model[EPANETModelCreator::JUNCTIONS])[id] = result;

    if(!addCoordinate(x,y,QString::number(cindex)))
        return false;

    return cindex;
}

uint EPANETModelCreator::addReservoir(double x, double y, double head, std::string headpattern)
{
    cindex++;
    QString id = QString::number(cindex);

    QString result = "";
    result += QString::number(cindex) + "\t";
    result += QString::number(head) + "\t";
    result += QString::fromStdString(headpattern) + "\t";

    (*model[EPANETModelCreator::RESERVOIRS])[id]=result;

    if(!addCoordinate(x,y,QString::number(cindex)))
        return false;

    return cindex;
}

uint EPANETModelCreator::addTank(double x, double y, double bottomelevation, double initiallevel, double minlevel, double maxlevel, double nominaldiamter, double minvolume, std::string volumecurve)
{
    cindex++;
    QString id = QString::number(cindex);

    QString result = "";

    result += QString::number(cindex) + "\t";
    result += QString::number(bottomelevation) + "\t";
    result += QString::number(initiallevel) + "\t";
    result += QString::number(minlevel) + "\t";
    result += QString::number(maxlevel) + "\t";
    result += QString::number(nominaldiamter) + "\t";
    result += QString::number(minvolume) + "\t";
    result += QString::fromStdString(volumecurve) + "\t";

    (*model[EPANETModelCreator::TANKS])[id]=result;

    if(!addCoordinate(x,y,QString::number(cindex)))
        return false;

    return cindex;
}

uint EPANETModelCreator::addPipe(uint startnode, uint endnode, double length, double diameter, double roughness, double minorloss, EPANETModelCreator::PIPESTATUS status)
{
    cindex++;
    QString id = QString::number(cindex);

    QString result = "";
    result += id + "\t";
    result += QString::number(startnode) + "\t";
    result += QString::number(endnode) + "\t";
    result += QString::number(length) + "\t";
    result += QString::number(diameter) + "\t";
    result += QString::number(roughness) + "\t";
    result += QString::number(minorloss) + "\t";
    result += PipeStatusString[status] + "\t";

    (*model[EPANETModelCreator::PIPES])[id]=result;

    return cindex;
}

bool EPANETModelCreator::addCoordinate(double x, double y, QString id)
{
    QString result = "";
    result += id + "\t";
    result += QString::number(x) + "\t";
    result += QString::number(y) + "\t";
    (*model[EPANETModelCreator::COORDINATES])[id]=result;
    return true;
}

bool EPANETModelCreator::addVertex(double x1, double y1, double x2, double y2, QString id)
{
    QString result = "";
    result += id + "\t";
    result += QString::number(x1) + "\t";
    result += QString::number(y1) + "\n";
    result += id + "\t";
    result += QString::number(x2) + "\t";
    result += QString::number(y2) + "\t";
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
        if(index==VERTICES && !vertex)
            continue;

        if(index==COORDINATES && !vertex)
            continue;

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
