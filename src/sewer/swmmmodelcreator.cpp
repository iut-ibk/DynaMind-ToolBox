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

#include <swmmmodelcreator.h>

SWMMModelCreator::SWMMModelCreator(bool vertex)
{
	#define X(a) ComponentStrings.push_back(#a);
	TYPES;
	#undef X
	#undef TYPES

	UnitStrings.push_back("CFS");
	UnitStrings.push_back("GPM");
	UnitStrings.push_back("MGD");
    UnitStrings.push_back("CMS");
    UnitStrings.push_back("LPS");
    UnitStrings.push_back("MLD");

    LengthUnitStrings.push_back("IN");
    LengthUnitStrings.push_back("MM");

    ForceMainEquationString.push_back("H-W");
    ForceMainEquationString.push_back("D-W");

    LinkOffsetsStrings.push_back("DEPTH");
    LinkOffsetsStrings.push_back("ELEVATION");

    FlowRoutingStrings.push_back("STEADY");
    FlowRoutingStrings.push_back("KINWAVE");
    FlowRoutingStrings.push_back("DYNWAVE");

    InfiltrationStrings.push_back("HORTON");
    InfiltrationStrings.push_back("GREEN_AMPT");
    InfiltrationStrings.push_back("CURVE_NUMPER");

    InertialDampingString.push_back("NONE");
    InertialDampingString.push_back("PARTIAL");
    InertialDampingString.push_back("FULL");

    NormalFlowLimitedString.push_back("SLOPE");
    NormalFlowLimitedString.push_back("FROUDE");
    NormalFlowLimitedString.push_back("BOTH");

    RainformString.push_back("INTENSITY");
    RainformString.push_back("VOLUME");
    RainformString.push_back("CUMULATIVE");

	cindex=0;
	this->vertex=vertex;
	for(uint index=0; index<ComponentStrings.size(); index++)
        model[static_cast<ComponentTypes>(index)] = boost::make_shared<SWMMElements>();

	initModel();
}

uint SWMMModelCreator::addRainGage(string name, RAINFORM form, string interval, double SCF, string sourcetype, string source, string sourcename,LENGTHUNITS unit)
{        
    QString result = "";
    result += QString::fromStdString(name) + "\t";
    result += RainformString[form] + "\t";
    result += QString::fromStdString(interval) + "\t";
    result += QString::number(SCF) + "\t";
    result += QString::fromStdString(sourcetype) + "\t";
    result += QString::fromStdString(source) + "\t";
    result += QString::fromStdString(sourcename) + "\t";
    result += LengthUnitStrings[unit] + "\t";

    if(raingages.find(result)!=raingages.end())
        return raingages[result];

    cindex++;
    (*model[SWMMModelCreator::RAINGAGES])[QString::fromStdString(name)] = result;

    return cindex;
}

uint SWMMModelCreator::addSubCatchment(string rgage, uint outid, double area, double imperv)
{
    cindex++;

    QString id = QString::number(cindex);

    QString result = "";

    result += id + "\t";
    result += QString::fromStdString(rgage) + "\t";
    result += QString::number(outid) + "\t";
    result += QString::number(area/10000.0,'f',4) + "\t";
    result += QString::number(imperv,'f',4) + "\t";
    result += QString::number(sqrt(area*10000.0),'f',4) + "\t";
    result += "0.1\t";
    result += "1";

    (*model[SWMMModelCreator::SUBCATCHMENTS])[id] = result;

    result = "";
    result += id + "\t";
    result += QString::number(0.015) + "\t";
    result += QString::number(0.2) + "\t";
    result += QString::number(1.8) + "\t";
    result += QString::number(5) + "\t";
    result += QString::number(0) + "\t";
    result += "OUTLET";

    (*model[SWMMModelCreator::SUBAREAS])[id] = result;

    result = "";
    result += id + "\t";
    result += QString::number(imperv) + "\t";
    result += QString::number(6.12) + "\t";
    result += QString::number(3) + "\t";
    result += QString::number(6) + "\t";
    result += QString::number(0);

    (*model[SWMMModelCreator::INFILTRATION])[id] = result;

    return cindex;
}

uint SWMMModelCreator::addJunction(double x, double y, double elevation, double dwf)
{
	cindex++;
	QString id = QString::number(cindex);

	QString result = "";

	result += id + "\t";
    result += QString::number(elevation,'f',2) + "\t";
    result += "0\t";
    result += "0\t";
    result += "0\t";
    result += "100000\t";
    (*model[SWMMModelCreator::JUNCTIONS])[id] = result;

    result = "";
    result += id + "\t";
    result += "FLOW\t";
    result += QString::number(dwf,'f',4);
    (*model[SWMMModelCreator::DWF])[id] = result;


	if(!addCoordinate(x,y,QString::number(cindex)))
		return false;

	return cindex;
}

uint SWMMModelCreator::addOutfall(double x, double y, double elevation)
{
    cindex++;
    QString id = QString::number(cindex);

    QString result = "";

    result += id + "\t";
    result += QString::number(elevation) + "\t";
    result += "FREE\t";
    result += "NO\t";

    (*model[SWMMModelCreator::OUTFALLS])[id] = result;

    if(!addCoordinate(x,y,QString::number(cindex)))
        return false;

    return cindex;
}

uint SWMMModelCreator::addConduit(uint startnode, uint endnode, double diameter,  double length, double N, double Z1, double Z2, double Q0)
{
	cindex++;
	QString id = QString::number(cindex);

	QString result = "";
	result += id + "\t";
	result += QString::number(startnode) + "\t";
	result += QString::number(endnode) + "\t";
	result += QString::number(length,'f',2) + "\t";
    result += QString::number(N,'f',2) + "\t";
    result += QString::number(Z1,'f',2) + "\t";
    result += QString::number(Z2,'f',2) + "\t";
    result += QString::number(Q0,'f',2) + "\t";

    (*model[SWMMModelCreator::CONDUITS])[id]=result;

    //Define crossection
    result = "";
    result += id + "\t";
    result += "CIRCULAR\t";
    result += QString::number(diameter,'f',2) + "\t";
    result += "0\t0\t0\t";

    (*model[SWMMModelCreator::XSECTIONS])[id]=result;

	return cindex;
}

bool SWMMModelCreator::addCoordinate(double x, double y, QString id)
{
	QString result = "";
	result += id + "\t";
	result += QString::number(x) + "\t";
	result += QString::number(y) + "\t";
    (*model[SWMMModelCreator::COORDINATES])[id]=result;
	return true;
}

bool SWMMModelCreator::addPolygonVertex(double x1, double y1, QString id)
{
    cindex++;
	QString result = "";
	result += id + "\t";
	result += QString::number(x1) + "\t";
    result += QString::number(y1);

    (*model[SWMMModelCreator::POLYGONS])[QString::number(cindex)]=result;

	return true;
}

bool SWMMModelCreator::save(string filepath)
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
        if(index==POLYGONS && !vertex)
			continue;

		if(index==COORDINATES && !vertex)
			continue;

		out << "\n[" << QString::fromStdString(ComponentStrings[index]) + "]\n";
        boost::shared_ptr<SWMMElements> elements = model[static_cast<ComponentTypes>(index)];
        SWMMElements::iterator itr;

		for(itr=elements.get()->begin(); itr!=elements.get()->end(); ++itr)
			out << itr->second << "\n";
	}

	file.close();
	return true;
}

bool SWMMModelCreator::setOptionFlowUnits(SWMMModelCreator::UNITS unit)
{
    return addEntry(SWMMModelCreator::OPTIONS,"FLOW_UNITS",UnitStrings[unit]);
}

bool SWMMModelCreator::setOptionInfiltration(SWMMModelCreator::INFILTRATIONTYPE infiltration)
{
    return addEntry(SWMMModelCreator::OPTIONS,"INFILTRATION",InfiltrationStrings[infiltration]);
}

bool SWMMModelCreator::setOptionFlowRouting(SWMMModelCreator::FLOW_ROUTING routing)
{
    return addEntry(SWMMModelCreator::OPTIONS,"FLOW_ROUTING", FlowRoutingStrings[routing]);
}

bool SWMMModelCreator::setOptionLinkOffsets(SWMMModelCreator::LINK_OFFSETS linkoffset)
{
    return addEntry(SWMMModelCreator::OPTIONS,"LINK_OFFSETS",LinkOffsetsStrings[linkoffset]);
}

bool SWMMModelCreator::setOptionForceMainEquation(SWMMModelCreator::FORCE_MAIN_EQUATION equation)
{
    return addEntry(SWMMModelCreator::OPTIONS,"FORCE_MAIN_EQUATION",ForceMainEquationString[equation]);
}

bool SWMMModelCreator::setOptionInertialDamping(SWMMModelCreator::INERTIAL_DAMPING damping)
{
    return addEntry(SWMMModelCreator::OPTIONS,"INERTIAL_DAMPING",InertialDampingString[damping]);
}

bool SWMMModelCreator::setOptionNormalFlowLimited(SWMMModelCreator::NORMAL_FLOW_LIMITED flowlimited)
{
    return addEntry(SWMMModelCreator::OPTIONS,"NORMAL_FLOW_LIMITIED",NormalFlowLimitedString[flowlimited]);
}

bool SWMMModelCreator::addEntry(SWMMModelCreator::ComponentTypes type, QString id, QString values)
{
	QString result = id + "\t" + values;
	(*model[type])[id]=result;
	return true;
}

void SWMMModelCreator::initModel()
{
	//OPTIONS
    setOptionFlowUnits(LPS);
    setOptionInfiltration(HORTON);
    setOptionFlowRouting(DYNWAVE);
    addEntry(OPTIONS,"START_DATE","1/1/2000");
    addEntry(OPTIONS,"END_DATE","1/1/2000");
    addEntry(OPTIONS,"START_TIME","00:00");
    addEntry(OPTIONS,"END_TIME","12:00");
    addEntry(OPTIONS,"REPORT_START_DATE","1/1/2000");
    addEntry(OPTIONS,"REPORT_START_TIME","00:00");
    addEntry(OPTIONS,"SWEEP_START","01/01");
    addEntry(OPTIONS,"SWEEP_END","12/31");
    addEntry(OPTIONS,"DRY_DAYS","0");
    addEntry(OPTIONS,"REPORT_STEP","00:05:00");
    addEntry(OPTIONS,"WET_STEP","00:01:00");
    addEntry(OPTIONS,"DRY_STEP","00:01:00");
    addEntry(OPTIONS,"ROUTING_STEP","00:00:07");
    addEntry(OPTIONS,"ALLOW_PONDING","NO");
    setOptionInertialDamping(PARTIAL);
    addEntry(OPTIONS,"VARIABLE_STEP","0.75");
    addEntry(OPTIONS,"LENGTHENING_STEP","10");
    addEntry(OPTIONS,"MIN_SURFAREA","0");
    setOptionNormalFlowLimited(BOTH);
    addEntry(OPTIONS,"SKIP_STEADY_STATE","NO");
    addEntry(OPTIONS,"IGNORE_RAINFALL","NO");
    setOptionForceMainEquation(HW);
    setOptionLinkOffsets(DEPTH);

	//REPORT
    addEntry(REPORT,"INPUT","NO");
    addEntry(REPORT,"CONTINUITY","YES");
    addEntry(REPORT,"FLOWSTATS","YES");
    addEntry(REPORT,"CONTROLS","NO");
    addEntry(REPORT,"SUBCATCHMENTS","NONE");
    addEntry(REPORT,"NODES","NONE");
    addEntry(REPORT,"LINKS","NONE");
}
