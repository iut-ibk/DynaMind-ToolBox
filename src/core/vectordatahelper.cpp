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
#include "vectordatahelper.h"
#include "boost/foreach.hpp"
#include <QMutexLocker>
std::vector<std::string> VectorDataHelper::findElementsWithIdentifier(const std::string &  Identifier, const std::vector<std::string> & names) {
    std::vector<std::string> elements;

    if (Identifier.compare("") == 0) {
        return names;
    }

    BOOST_FOREACH(std::string name, names) {
        if (name.find(Identifier, 0) == 0)
            elements.push_back(name);
    }
    return elements;
}

double VectorDataHelper::calculateArea(const Face &f, const std::vector<Point> &pl) {
    //Check if first is last
    if (f.getIDs().size() < 3)
        return 0;
    Point pend = pl[f.getIDs()[f.getIDs().size()-1]];
    Point pstart = pl[0];
    bool startISEnd = true;
    if (!pend.compare2d(pstart))
            startISEnd = false;
    double A = 0;
    for (int i = 0; i< f.getIDs().size()-1;i++) {
        Point p_i = pl[f.getIDs()[i]];
        Point p_i1 = pl[f.getIDs()[i+1]];

        A+=p_i.getX()*p_i1.getY() - p_i1.getX()*p_i.getY();

    }
    if (!startISEnd)
        A+= pend.getX()*pstart.getY() - pstart.getX()*pend.getY();

    return A/2.;
}

Point VectorDataHelper::caclulateCentroid(const Face &f, const std::vector<Point> &pl) {
    //Check if first is last
    if (f.getIDs().size() < 3)
        return Point(0,0,0);
    Point pend = pl[f.getIDs()[f.getIDs().size()-1]];
    Point pstart = pl[0];
    bool startISEnd = true;
    if (!pend.compare2d(pstart))
            startISEnd = false;
    double A6 = VectorDataHelper::calculateArea(f, pl)*6.;
    double x = 0;
    double y = 0;
    for (int i = 0; i< f.getIDs().size()-1;i++) {
        Point p_i = pl[f.getIDs()[i]];
        Point p_i1 = pl[f.getIDs()[i+1]];

        x+= (p_i.getX() + p_i1.getX())*(p_i.getX() * p_i1.getY() - p_i1.getX() * p_i.getY());
        y+= (p_i.getY() + p_i1.getY())*(p_i.getX() * p_i1.getY() - p_i1.getX() * p_i.getY());


    }
    if (!startISEnd) {
        x+= (pend.getX() + pstart.getX())*(pend.getX() * pstart.getY() - pstart.getX() * pend.getY());
        y+= (pend.getY() + pstart.getY())*(pend.getX() * pstart.getY() - pstart.getX() * pend.getY());

       }
    return Point(x/A6,y/A6,0);
}

std::vector <std::string> VectorDataHelper::findConnectedEges(const VectorData &vec, const Point &p, double offset, int which, std::string Identifier) {
    std::vector<std::string> RessVector;
    std::vector<std::string> names(VectorDataHelper::findElementsWithIdentifier(Identifier, vec.getEdgeNames()));
    BOOST_FOREACH(std::string name, names) {
        std::vector<Edge> edges = vec.getEdges(name);
        std::vector<Point> points = vec.getPoints(name);


        BOOST_FOREACH(Edge e, edges) {
            Point p1_tmp = points[e.getID1()];
            Point p2_tmp = points[e.getID2()];
            switch (which) {
            case (BOTH):
                if (p.compare2d(p1_tmp, offset) || p.compare2d(p2_tmp, offset))
                    RessVector.push_back(name);
                break;
            case (UPPER):
                if ( p.compare2d(p2_tmp, offset))
                    RessVector.push_back(name);
                break;
            case (LOWER):
                if (p.compare2d(p1_tmp, offset))
                    RessVector.push_back(name);
                break;
            }


        }
    }
    return RessVector;
}

std::string VectorDataHelper::findEdgeID(const VectorData & vec, const Point & p1, const Point & p2, std::string Identifier, double offset) {


    std::vector<std::string> names(VectorDataHelper::findElementsWithIdentifier(Identifier, vec.getEdgeNames()));
    BOOST_FOREACH(std::string name, names) {
        std::vector<Edge> edges = vec.getEdges(name);
        std::vector<Point> points = vec.getPoints(name);


        BOOST_FOREACH(Edge e, edges) {
            Point p1_tmp = points[e.getID1()];
            Point p2_tmp = points[e.getID2()];

            if (p1.compare2d(p1_tmp, offset) && p2.compare2d(p2_tmp, offset))
                return name;

        }

    }
    return "";
}

std::string VectorDataHelper::findPointID(const VectorData &vec, const Point &p, std::string Identifier, double offset) {
    std::vector<std::string> names(VectorDataHelper::findElementsWithIdentifier(Identifier, vec.getPointsNames()));

    BOOST_FOREACH(std::string name, names) {
        std::vector<Point> points = vec.getPoints(name);


        BOOST_FOREACH(Point p_tmp, points) {
            if ( p.compare2d(p_tmp, offset) )
                return name;

        }

    }
    return "";

}

bool VectorDataHelper::checkIfPointExists(const std::vector<Point> &vec, const Point &point, double offset) {

    foreach (Point p, vec) {
        if (p.compare2d(point, offset))
            return true;
    }

    return false;
}

Attribute VectorDataHelper::findAttributeFromPoints(const VectorData &vec, const Point &point, std::string identifier, double offset) {
    std::vector<std::string> Names = VectorDataHelper::findElementsWithIdentifier(identifier, vec.getPointsNames());

    Attribute attr;
    foreach(std::string name, Names) {
        std::vector<Point> points = vec.getPoints(name);
        foreach(Point p, points) {
            if (p.compare2d(point, offset))
                attr = vec.getAttributes(name);
        }

    }
    return attr;


}


