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

#ifndef TESTMODULE_H
#define TESTMODULE_H
#include "dmcompilersettings.h"
#include "dmcomponent.h"
#include "dmsystem.h"
#include "QVariant"

#include "dmmodule.h"
#include "dm.h"
using namespace DM;

/** 
 * @brief The Module creates an inital data set that a simulation can use for testing
 *
 * Data Set:
 * - Sewer:
 *      - Inlets|NODE: A, B
 *      - Conduits|EDGE
 *      - SomeRandomInformation|EDGE
 * - RasterData:
 *      - RasterData|RASTERDATA
 *
 */

class DM_HELPER_DLL_EXPORT TestModule : public  Module {

    DM_DECLARE_NODE(TestModule)
    private:
        double value;
    DM::View inlets;
    DM::View conduits;
    DM::View SomeRandomInformation;
    DM::View rdata;

    DM::System * outputData;
    DM::RasterData * outputRasterData;
public:
    TestModule();
    /** @brief create initial data set
         *
         * The model adds 2 nodes to inlets at (0,0,0) and (0,0,1) and an edge to
         * Conduits and SomeRandomInformation and a RasterData field(200,200,20) with the cell value 15
         */
    void run();
    virtual ~TestModule();
};

#endif // TESTMODULE_H
