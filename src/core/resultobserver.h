/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#ifndef RESULTOBSERVER_H
#define RESULTOBSERVER_H
#include <compilersettings.h>
#include <iostream>
#include <rasterdata.h>
#include <QVector>
class VIBE_HELPER_DLL_EXPORT ResultObserver
{
public:
    ResultObserver();
    virtual void addResultImage(std::string UUID, std::string filename) = 0;
    virtual void addRasterDataToViewer( QVector<RasterData>) = 0;
    virtual void addVectorDataToViewer(QVector<VectorData>) = 0;
    virtual void addDoubleDataToPlot(std::string UUID, double x, double y) = 0;
};

#endif // RESULTOBSERVER_H
