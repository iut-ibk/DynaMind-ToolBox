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
#ifndef CSG_SCSG_S_OPERATIONS_H
#define CSG_SCSG_S_OPERATIONS_H
#include "dmcompilersettings.h"
#include "dataLayer.h"

#include <iostream>
#include <string>

using namespace std;

namespace csg_s {


class DM_HELPER_DLL_EXPORT csg_s_operations{
    
public:

    static int returnPositionX(int Position);

    static int returnPositionY(int Position);
    /// sucht nach einem Sammler und überträgt das Ergebniss in eine andere Landschaft
    static void findSewer(DM::dataLayer *Input, DM::dataLayer *Output);

    static void addRadiusValue(int x, int y, DM::dataLayer *Layer, int rmax);

    static void addRadiusValue(int x, int y, DM::dataLayer *Layer, int rmax, double value);

    static void addRadiusValueADD(int x, int y, DM::dataLayer *Layer, int rmax, double value);

    static void intSewerNetwork(DM::dataLayer *InitSewer, DM::dataLayer *ResultField);

    static void exportRaster(QString name, DM::dataLayer * Layer_, double multiplikator);
    static void writePath(string Text, QString name);
};

}

#endif
