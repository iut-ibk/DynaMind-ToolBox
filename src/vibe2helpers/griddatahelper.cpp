/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * DAnCE4Water (Dynamic Adaptation for eNabling City Evolution for Water) is a
 * strategic planning tool ti identify the transtion from an from a conventional
 * urban water system to an adaptable and sustainable system at a city scale
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
 * @section DESCRIPTION
 *
 * The class contains helper functions for griddata
 */


#include "griddatahelper.h"
#include "vectordatahelper.h"
#include "alglib/src/spline1d.h"
#include <sstream>
#include <iostream>
struct Colors{
    float R;
    float G;
    float B;
};

void GriddataHelper::createColorTable(VectorData * vec , std::string Identifier, std::string attr_name) {
    MinMax minmax = GriddataHelper::MinMaxValues(vec, Identifier, attr_name);
    int n = 3;
    ap::real_1d_array R_x;
    ap::real_1d_array R_y;
    ap::real_1d_array G_x;
    ap::real_1d_array G_y;
    ap::real_1d_array B_x;
    ap::real_1d_array B_y;

    spline1dinterpolant R;
    spline1dinterpolant G;
    spline1dinterpolant B;

    double offsetmin = 0;
    if (minmax.minVal < 0) {
        offsetmin = minmax.minVal *-1;
    }

    double minValue = 0;
    double maxValue = minmax.maxVal+offsetmin;

    R_x.setlength(n);
    R_y.setlength(n);
    G_x.setlength(n);
    G_y.setlength(n);
    B_x.setlength(n);
    B_y.setlength(n);
    //min
    R_x(2) = maxValue;
    R_y(2) = 1.0;
    G_x(2) = maxValue;
    G_y(2) = 0.0;
    B_x(2) = maxValue;
    B_y(2) = 0.0;

    //middle
    R_x(1) = (maxValue +minValue)/2.;
    R_y(1) = 0.0;
    G_x(1) = (maxValue+minValue)/2.;
    G_y(1) = 1.0;
    B_x(1) = (maxValue +minValue)/2.;
    B_y(1) = 0.0;

    //max
    R_x(0) = minValue;
    R_y(0) = 0.0;
    G_x(0) = minValue;
    G_y(0) = 0.0;
    B_x(0) = minValue;
    B_y(0) = 1.0;

    spline1dbuildlinear(R_x,R_y,n, R);
    spline1dbuildlinear(G_x,G_y,n, G);
    spline1dbuildlinear(B_x,B_y,n, B);

    std::vector<std::string> names = VectorDataHelper::findElementsWithIdentifier(Identifier, vec->getAttributeNames());
    foreach (std::string name, names){
        double val =  vec->getAttributes(name).getAttribute(attr_name);
        Colors co;
        co.R = (float)spline1dcalc(R, val+offsetmin);
        co.G = (float)spline1dcalc(G,val+offsetmin);
        co.B = (float)spline1dcalc(B,val+offsetmin);

        std::stringstream ss;
        ss << "COLOR_" << name;

        Point p (co.R,co.G,co.B );
        std::vector<Point> pl;
        pl.push_back(p);

        vec->setPoints(ss.str(), pl);



        //RGBcolers_tmp(j,i) = co;
        //QColor c(co.R*255, co.G*255, co.B*255);
        //QColor c(255, 0, 0);
        //map.setPixel(j,i,c.rgba());
        //double colorread = (this->scene.rData.getValue(j,i)+offsetmin)*255/(maxValue-minValue);

        //int cr = colorread/255;
        //int yr = colorread - 255*cr;
        //QColor c1(cr, yr, 0);
        //heighmap.setPixel(j,i,c1.rgba());
    }

}
double GriddataHelper::sum(VectorData *vec, std::string Identifier, std::string attr_name) {
    double sum = 0;
    std::vector<std::string> names = VectorDataHelper::findElementsWithIdentifier(Identifier, vec->getAttributeNames());
    for (int i = 0; i < names.size(); i++) {
        Attribute attr = vec->getAttributes(names[i]);
        double val = attr.getAttribute(attr_name);
        sum=sum+val;

}

    return sum;
}
MinMax GriddataHelper::MinMaxValues(VectorData *vec, std::string Identifier, std::string attr_name) {
    MinMax minmax;
    std::vector<std::string> names = VectorDataHelper::findElementsWithIdentifier(Identifier, vec->getAttributeNames());
    for (int i = 0; i < names.size(); i++) {
        Attribute attr = vec->getAttributes(names[i]);
        std::vector<std::string> attr_names = attr.getAttributeNames();

        double val = attr.getAttribute(attr_name);
        if (i == 0) {
            minmax.maxVal = val;
            minmax.minVal = val;
            continue;
        }

        if (val < minmax.minVal)
            minmax.minVal = val;
        if (val > minmax.maxVal)
            minmax.maxVal = val;
    }



    return minmax;
}
