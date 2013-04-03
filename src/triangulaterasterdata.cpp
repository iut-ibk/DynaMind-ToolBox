/**
 * @file
 * @author  Christian Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Christian Urich
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
/*
#include "triangulaterasterdata.h"

void TriangulateRasterData::Triangulation(std::vector<DM::Vector3> & nodes, DM::RasterData * rData) {

    unsigned long Y = rData->getHeight();
    unsigned long X = rData->getWidth();

    unsigned long OX = rData->getXOffset();
    unsigned long OY = rData->getYOffset();

    double noData = rData->getNoValue();
    double lX = rData->getCellSizeX();
    double lY = rData->getCellSizeY();

    //nodes.reserve(Y*X*6);

    for (unsigned long  y = 0; y < Y; y++) {
        for (unsigned long  x = 0; x < X; x++) {
            double val = rData->getCell(x,y);
            if (val == noData)
                continue;
            nodes.push_back( DM::Vector3( (x-0.5) * lX + OX ,  (y-0.5) * lY + OY, val) );
            nodes.push_back( DM::Vector3( (x+0.5) * lX + OX,   (y-0.5) * lY + OY, val) );
            nodes.push_back( DM::Vector3( (x-0.5) * lX + OX,   (y+0.5) * lY + OY, val) );

            nodes.push_back( DM::Vector3( (x-0.5) * lX + OX,   (y+0.5) * lY + OY, val) );
            nodes.push_back( DM::Vector3( (x+0.5) * lX + OX,   (y-0.5) * lY + OY, val) );
            nodes.push_back( DM::Vector3( (x+0.5) * lX + OX,   (y+0.5) * lY + OY, val) );

        }
    }

}
*/