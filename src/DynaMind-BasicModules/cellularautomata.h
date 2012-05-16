/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

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
#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include "dmcompilersettings.h"
#include "dmmodule.h"
#include "muParser.h"
#include <dm.h>

using namespace mu;

using namespace DM;
class DM_HELPER_DLL_EXPORT CellularAutomata : public  Module {
    DM_DECLARE_NODE( CellularAutomata)
    struct Dimension {
        int widht;
        int height;
        int elements;
    };
    struct Parameter {
        long Width;
        long Height;
        double CellSize;
        int Steps;
        RasterData * OutputMap;
        std::string Desicion;
        std::map<std::string, std::string> neighs;
        std::map<std::string, std::string> rules;

        std::string CellularMap;

        std::vector<std::string> Neighbourhoods;
        std::vector<std::string> Values;

        std::vector<std::string> ListOfLandscapes;

        //unordered_map<std::string, std::string> rules;

    };

public:
    CellularAutomata();
    void  initRuntime();
    void init();
    void run();
    virtual bool  createInputDialog();
    void addLandscape(std::string s);
    std::vector<std::string> getLandscapes(){return this->param.ListOfLandscapes;}

private:
    void updateInport();
    bool runinit;
    std::map<std::string, RasterData * > landscapes;


    std::map<std::string, double**> NeighboorhoodMaps;
    std::map<std::string, int**> NeighboorhoodStamps;
    std::map<std::string, double** > NeighboorhoodPointerMap;
    std::map<std::string, Dimension> NeighboohoodDimensions;
    std::map<std::string, std::string> NeighboorhoodMapName;

    std::vector<std::string> NeighboorhoodList;
    std::vector<double*> RulesResults;
    std::vector<Parser*> Rules;
    Parser * Desicion;

    std::string NameOfOutput;
    std::string NameOfOutput_old;

    std::vector<double *> pRessults;
    std::vector<Parser> vParser;

    std::vector<std::string> vExistingData;

    int nbnumber;
    double * repeater;

    Parameter param;
    parameter_type paramRaw;

};


#endif // CELLULARAUTOMATA_H
