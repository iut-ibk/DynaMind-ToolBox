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
#include <QMap>

//using namespace mu;

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
        long offsetX;
        long offsetY;
        int Steps;
        RasterData * OutputMap;
        std::string Desicion;
        std::map<std::string, std::string> neighs;
        std::map<std::string, std::string> rules;

        std::string CellularMap;

        std::vector<std::string> Neighbourhoods;
        std::vector<std::string> Values;

        std::vector<std::string> ListOfLandscapes;
        bool appendToStream;
        bool appendToStream_old;
        std::string DimensionOfExisting;
    };

public:
    CellularAutomata();
    void  initRuntime();
    void init();
    void run();
    std::string getHelpUrl();

    virtual bool  createInputDialog();
    void addLandscape(std::string s);
    void removeLandscape(std::string s);
    void removeNeighboorhood(std::string neigh);
    void removeRule(std::string rule);

    std::vector<std::string> getLandscapes(){return this->param.ListOfLandscapes;}
    std::vector<std::string> getLandscapesInStream();
    virtual ~CellularAutomata();

private:
    void updateInport();
    bool runinit;
    std::map<std::string, RasterData * > landscapes;


    QMap<std::string, double**> NeighboorhoodMaps;
    QMap<std::string, int**> NeighboorhoodStamps;
    QMap<std::string, double** > NeighboorhoodPointerMap;
    QMap<std::string, Dimension> NeighboohoodDimensions;
    QMap<std::string, std::string> NeighboorhoodMapName;

    std::vector<std::string> NeighboorhoodList;
    std::vector<double*> RulesResults;
    std::vector<mu::Parser*> Rules;
    mu::Parser * Desicion;

    std::string NameOfOutput;
    std::string NameOfOutput_old;

    std::vector<double *> pRessults;
    std::vector<mu::Parser> vParser;

    std::vector<std::string> vExistingData;

    int nbnumber;
    double * repeater;

    std::map<std::string, std::string> paramRaw;

    void deinit();
public:
    Parameter param;
};


#endif // CELLULARAUTOMATA_H
