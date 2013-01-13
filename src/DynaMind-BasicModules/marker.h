/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich

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
#ifndef MARKER_H
#define MARKER_H

#include "dmmodule.h"
#include "muParser.h"
#include "dm.h"
using namespace DM;
class DM_HELPER_DLL_EXPORT Marker : public  Module {

    DM_DECLARE_NODE( Marker)

    struct Parameter {
        unsigned long Width;
        unsigned long Height;
        unsigned long OffsetX;
        unsigned long OffsetY;

        double CellSize;
        std::string MarkerMap;

        std::string resultName;

        std::string VectorData;


        int Points;
        int Edges;
        std::string KeepMap;

        std::vector<std::string> RData;

        std::vector<std::string> RVariables;
        std::string RExpression;

        std::vector<std::string> rVariables;
        std::string rExpression;

        std::string PlacementOption;

        std::string minExpression;
        std::string maxExpression;


        std::string Identifier;



    };
private:
    std::map<std::string, RasterData * > inputRasterData;
    DM::System * vectorData;
    RasterData * OutputMap;


    parameter_type paramRaw;
    Parameter param;
    double evaluateExpresion_R(int index, DM::Node & p);
    double evaluateExpresion_r(int index, DM::Node & p);
    double calculater(const DM::Node &sp, const DM::Node &cp);
    double initVariables;


    std::vector<const std::vector<double> * > RDoubleAttributes;
    std::vector<const RasterData * > RRasterData;
    std::vector<double * >  RVariables;
    std::vector<double * > RRasterVariables;
    mu::Parser *  RExpression;

    std::vector<const std::vector<double> * > rDoubleAttributes;
    std::vector<const RasterData * > rRasterData;
    std::vector<double * >  rVariables;
    std::vector<double * > rRasterVariables;
    mu::Parser *  rExpression;

    mu::Parser *  minExpression;
    mu::Parser *  maxExpression;
    bool maxSet;
    bool minSet;

    void initRExpression();
    void initrExpression();
    void initMaxMinExpression();

    double * r;
    double * R;
    double * CellValue;

    DM::System * sys_in;


public:

    Marker();
    void run();
    void init();
    virtual bool createInputDialog();
    DM::System * getSystemIn();

};


#endif // MARKER_H
