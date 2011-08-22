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
#ifndef VECTORDATAHELPER_H
#define VECTORDATAHELPER_H
#include "compilersettings.h"
#include "vectordata.h"
#include "QMutex"

enum VDH{
    UPPER,
    LOWER,
    BOTH

};
class VIBE_HELPER_DLL_EXPORT VectorDataHelper
{
public:
    //VectorDataHelper(){};
    static std::vector<std::string> findElementsWithIdentifier(const std::string &  Identifier, const std::vector<std::string> & names) ;
    static std::string findEdgeID(const VectorData & vec, const Point & p1, const Point & p2, std::string identifier = "", double offset = 0);
    static std::string findPointID(const VectorData & vec, const Point & p, std::string identifier = "", double offset = 0);
    static std::vector<std::string> findConnectedEges(const VectorData & vec, const Point & p,double offset=0., int which = BOTH,std::string identifier = "");
    static bool checkIfPointExists(const std::vector<Point> & vec, const Point & p, double offset = 0);
    static Attribute findAttributeFromPoints(const VectorData & vec, const Point & p, std::string identifier, double offset = 0);
    static double calculateArea(const Face & f, const std::vector<Point> & pl);
    static Point caclulateCentroid(const Face & f, const std::vector<Point> & pl);
    static double calculateLength(const Edge & e, const std::vector<Point> & pl);

    static QMutex mutex;
};

#endif // VECTORDATAHELPER_H
