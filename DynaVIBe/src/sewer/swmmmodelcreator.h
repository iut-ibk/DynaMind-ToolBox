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

#ifndef SWMMModelCreator_H
#define SWMMModelCreator_H

#include <dmsystem.h>
#include <map>
#include <vector>
#include <memory>
#include <QString>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <sewerviewdef.h>

class SWMMModelCreator
{
private:
	#define TYPES \
    X(RAINGAGES) \
	X(OPTIONS) \
	X(REPORT) \
	X(JUNCTIONS) \
    X(OUTFALLS) \
    X(CONDUITS) \
    X(XSECTIONS) \
    X(SUBCATCHMENTS) \
    X(SUBAREAS) \
    X(INFILTRATION) \
    X(DWF) \
    X(COORDINATES) \
    X(POLYGONS)

	#define X(a) a,
		enum ComponentTypes {TYPES};
	#undef X

	std::vector<std::string> ComponentStrings;
	std::vector<QString> UnitStrings;
    std::vector<QString> LengthUnitStrings;
    std::vector<QString> InfiltrationStrings;
    std::vector<QString> FlowRoutingStrings;
    std::vector<QString> LinkOffsetsStrings;
    std::vector<QString> ForceMainEquationString;
    std::vector<QString> InertialDampingString;
    std::vector<QString> NormalFlowLimitedString;
    std::vector<QString> RainformString;

    typedef std::map<QString,QString> SWMMElements;

	bool vertex;
    std::map<ComponentTypes,boost::shared_ptr<SWMMElements> > model;
	uint cindex;
    std::map<QString,uint> raingages;

public:
    enum UNITS {CFS, GPM, MGD, CMS, LPS, MLD};
    enum INFILTRATIONTYPE {HORTON, GREEN_AMPT, CURVE_NUMBER};
    enum FLOW_ROUTING {STEADY, KINWAVE, DYNWAVE};
    enum LINK_OFFSETS {DEPTH, ELEVATION};
    enum FORCE_MAIN_EQUATION {HW, DW};
    enum INERTIAL_DAMPING {NONE, PARTIAL, FULL};
    enum NORMAL_FLOW_LIMITED {SLOPE, FROUDE, BOTH};
    enum RAINFORM {INTENSITY, VOLUME, COMULATIVE};
    enum LENGTHUNITS {IN, MM};


    SWMMModelCreator(bool vertex=true);

    //Raingage
    uint addRainGage(string name, RAINFORM form, string interval, double SCF, string sourcetype, string source, string sourcename, LENGTHUNITS unit);

    //FACE COMPONENTS OF SWMM
    uint addSubCatchment(string rgage, uint outid, double area, double imperv);

    //NODE COMPONENTS OF SWMM
    uint addJunction(double x, double y, double elevation, double dwf);
    uint addOutfall(double x, double y, double elevation);

    //LINK COMPONENTS OF SWMM
    uint addConduit(uint startnode, uint endnode, double diameter, double length, double N, double Z1, double Z2, double Q0 = 0);

	//OPTIONS
    bool setOptionFlowUnits(UNITS unit);
    bool setOptionInfiltration(INFILTRATIONTYPE infiltration);
    bool setOptionFlowRouting(FLOW_ROUTING flowrouting);
    bool setOptionLinkOffsets(LINK_OFFSETS linkoffset);
    bool setOptionForceMainEquation(FORCE_MAIN_EQUATION equation);
    bool setOptionInertialDamping(INERTIAL_DAMPING damping);
    bool setOptionNormalFlowLimited(NORMAL_FLOW_LIMITED limited);

    bool save(std::string filepath);
    ~SWMMModelCreator(){}

	//Coordinates
	bool addCoordinate(double x, double y, QString id);
    bool addPolygonVertex(double x1, double y1, QString id);

    //Helper methodes


private:
	bool addEntry(ComponentTypes type, QString id, QString values);
	void initModel();
};

#endif // SWMMModelCreator_H
