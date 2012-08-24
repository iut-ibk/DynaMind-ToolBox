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

#ifndef WaterBalance_H
#define WaterBalance_H

#include <dmmodule.h>
#include <dm.h>

class MapBasedModel;
class NodeRegistry;
class ISimulation;
class SimulationRegistry;
class DynaMindStreamLogSink;
class Node;
class SimulationParameters;

class WaterBalance : public DM::Module
{
     DM_DECLARE_NODE(WaterBalance)

private:
    DM::System *city;

    typedef std::map<std::string,DM::View> viewmap;
    typedef std::map<std::string,std::string> stringmap;
    typedef std::map<std::string, stringmap * > conversionmap;

    conversionmap conversions;
    viewmap viewdef;

    std::map<std::string, Node *> mixers;
    std::map<std::string, int> mixerports;

    SimulationRegistry *simreg;
    NodeRegistry *nodereg;
    ISimulation *s;
    DynaMindStreamLogSink *sink;
    SimulationParameters *p;

public:
    WaterBalance();
    void run();
    bool createModel(MapBasedModel *m);
    bool createTanks(MapBasedModel *m, DM::View v, std::map<std::string,std::string> *ids);
    bool createBlocks(MapBasedModel *m);
    bool connectBlocks(MapBasedModel *m);
    bool connectBlock(MapBasedModel *m, DM::Face *currentblock, std::string type);
    bool connectTanks(MapBasedModel *m, std::string type);
    bool connect(MapBasedModel *m,std::string type,std::string successor, Node *n, std::string outportname);
    void checkconnection(MapBasedModel *m, std::string name);
    double extractVolumeResult(MapBasedModel *m,std::string type);
    double extractTotalBlockVolume(MapBasedModel *m);
    void clear();
    void initmodel();
};

#endif // WaterBalance_H
