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

#ifndef EpanetModelCreator_H
#define EpanetModelCreato_H

#include <dmsystem.h>
#include <map>
#include <vector>
#include <memory>
#include <QString>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <watersupplyviewdef.h>

class EPANETModelCreator
{
private:
    #define TYPES \
    X(OPTIONS) \
    X(TIMES) \
    X(REPORT) \
    X(JUNCTIONS) \
    X(TANKS) \
    X(RESERVOIRS) \
    X(PIPES) \
    X(COORDINATES) \
    X(VERTICES)

    #define X(a) a,
        enum ComponentTypes {TYPES};
    #undef X

    std::vector<std::string> ComponentStrings;
    std::vector<QString> UnitStrings;
    std::vector<QString> HeadlossStrings;
    std::vector<QString> UnbalancedStrings;
    std::vector<QString> HydraulicsStrings;

    typedef std::map<QString,QString> EpanetElements;

    bool vertex;
    std::map<ComponentTypes,boost::shared_ptr<EpanetElements> > model;
    DM::WS::ViewDefinitionHelper wsd;
    QVector<DM::Component*> components;

public:
    enum UNITS {CFS, GPM, MGD, IMGD, AFD, LPS, LPM, MLD, CMH, CMD};
    enum HEADLOSS {HW,DW,CM};
    enum UNBALANCED {STOP, CONTINUE};
    enum HYDRAULICS {USE, SAVE};

    EPANETModelCreator(bool vertex=true);

    //NODE COMPONENTS OF EPANET
    bool addJunction(DM::Node *junction);
    bool addReservoir(DM::Node *reservoir);
    bool addTank(DM::Node *tank);

    //LINK COMPONENTS OF EPANET
    bool addPipe(DM::Edge *pipe);

    //OPTIONS
    bool setOptionUnits(UNITS unit);
    bool setOptionHeadloss(HEADLOSS headloss);
    bool setOptionHydraulics(HYDRAULICS hydraulics, std::string filename);
    bool setOptionViscosity(double value);
    bool setOptionDiffusivity(double value);
    bool setOptionSpecificGrafity(double value);
    bool setOptionTrails(int value);
    bool setOptionAccuracy(double value);
    bool setOptionUnbalanced(UNBALANCED, int value =-1);
    bool setOptionPattern(double value);
    bool setOptionDemandMultiplier(double value);
    bool setOptionEmitterExponent(double value);
    bool setOptionTolerance(double value);
    bool setOptionMap(std::string filename);


    bool save(std::string filepath);
    ~EPANETModelCreator(){}

private:
    bool addCoordinate(DM::Node *node, QString id);
    bool addVertex(DM::Edge *edge, QString id);
    bool addEntry(ComponentTypes type, QString id, QString values);
    void initModel();
};

#endif // EpanetModelCreato_H
