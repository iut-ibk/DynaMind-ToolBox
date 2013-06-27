/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair
 
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

#include <spatialpressureperformance.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>
#include <list>
#include <cmath>

#include <CGAL/Interpolation_traits_2.h>
#include <CGAL/natural_neighbor_coordinates_2.h>
#include <CGAL/interpolation_functions.h>

#include <QFile>
#include <QTextStream>

using namespace DM;

DM_DECLARE_NODE_NAME(SpatialPressurePerformance,Watersupply-Performance)

SpatialPressurePerformance::SpatialPressurePerformance()
{   
    std::vector<DM::View> views;

    views.push_back(wsd.getCompleteView(WS::JUNCTION,DM::READ));
    this->addData("Watersupply-1", views);
    views.clear();
    views.push_back(wsd.getCompleteView(WS::JUNCTION,DM::READ));
    this->addData("Watersupply-2", views);

    this->resultfilepath="";
    this->addParameter("Result file path", DM::FILENAME, &this->resultfilepath);
}

void SpatialPressurePerformance::run()
{
    typedef std::map<std::string, DM::Component*> cmap;
    cmap::iterator itr;
    std::vector<DM::Node*> j1, j2;
    QString result = "";
    double errors=0;
    double abserrors=0;
    double sumabsminerrors=0;
    double sumabsmaxerrors=0;
    double sumerrors=0;
    double maxposerror=0;
    double maxnegerror=10000000000;
    double minposerror=10000000000;
    double minnegerror=0;

    uint samples = 0;
    uint minabssamples=0;
    uint maxabssamples=0;
    uint outside = 0;

    this->sys1 = this->getData("Watersupply-1");
    this->sys2 = this->getData("Watersupply-2");

    //JUNCTIONS
    cmap junctions1 = sys1->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::READ));
    cmap junctions2 = sys2->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::READ));

    //Extract data
    for(itr=junctions1.begin(); itr != junctions1.end(); ++itr)
        j1.push_back(static_cast<DM::Node*>((*itr).second));

    for(itr=junctions2.begin(); itr != junctions2.end(); ++itr)
        j2.push_back(static_cast<DM::Node*>((*itr).second));

    Delaunay triangulation;
    std::map<Vertex,DM::Node*> mapping;
    std::map<Vertex, K::FT, K::Less_xy_2> function_values;

    if(!createDelaunay(j1,triangulation,mapping,function_values))
    {
        DM::Logger(DM::Error) << "Watersupply-1 is contains no junctions";
        return;
    }

    for(uint index=0; index<j2.size(); index++)
    {
        DM::Node* currentjunction = j2[index];

        if(currentjunction->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble()<0)
            continue;

        int li;
        typedef Delaunay::Locate_type Locate_type;
        Locate_type lt;
        Vertex v(currentjunction->getX(), currentjunction->getY());

        Face_handle c = triangulation.locate(v,lt,li);
        triangulation.triangle(c);

        switch(lt)
        {
            case Delaunay::OUTSIDE_AFFINE_HULL:
            {
                outside++;
                break;
            }

            case Delaunay::OUTSIDE_CONVEX_HULL:
            {
                outside++;
                break;
            }

            default :
            {
                double pressureProjection = interpolateTriangle(currentjunction,triangulation,function_values);
                double pressure = currentjunction->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();

                if(pressure > 0 && pressureProjection > 0)
                {
                    double pressurediff = pressureProjection - pressure;
                    double absdiff = std::pow(pressurediff,2);
                    abserrors += absdiff;

                    if(pressurediff < 0.0)
                    {
                        sumabsminerrors += absdiff;
                        minabssamples++;

                        if(maxnegerror > pressurediff)
                            maxnegerror=pressurediff;

                        if(minnegerror < pressurediff)
                            minnegerror=pressurediff;
                    }
                    else
                    {
                        sumabsmaxerrors += absdiff;
                        maxabssamples++;

                        if(maxposerror < pressurediff)
                            maxposerror = pressurediff;

                        if(minposerror > pressurediff)
                            minposerror = pressurediff;
                    }

                    errors += pressureProjection/pressure;
                    sumerrors += pressurediff;
                    samples++;

                    result  += QString::number(currentjunction->getX()) + " "
                            + QString::number(currentjunction->getY()) + " "
                            + QString::number(pressurediff) + " "
                            + QString::number(pressureProjection/pressure) + "\n";
                }
                break;
            }
        }
    }

    if(!minabssamples)
        minabssamples=1;

    if(!maxabssamples)
        maxabssamples=1;

    if(this->resultfilepath != "")
    {
        QFile file(QString::fromStdString(this->resultfilepath));
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << result;
        file.close();
    }

    DM::Logger(DM::Standard) << "Mean relative error: " << errors/samples << " Samples: " << int(samples);
    DM::Logger(DM::Standard) << "Mean absulute error: " << std::sqrt(abserrors/samples) << " Samples: " << int(samples);
    DM::Logger(DM::Standard) << "Mean error: " << (sumerrors/samples) << " Samples: " << int(samples);
    DM::Logger(DM::Standard) << "Mean negative error: " << -std::sqrt(sumabsminerrors/minabssamples) << " Samples: " << int(minabssamples);
    DM::Logger(DM::Standard) << "Mean positive error: " << std::sqrt(sumabsmaxerrors/maxabssamples) << " Samples: " << int(maxabssamples);
    DM::Logger(DM::Standard) << "Max negative error: " << maxnegerror;
    DM::Logger(DM::Standard) << "Max positive error: " << maxposerror;
    DM::Logger(DM::Standard) << "Min negative error: " << minnegerror;
    DM::Logger(DM::Standard) << "Min positive error: " << minposerror;
    DM::Logger(DM::Warning) << double(outside) << " points outside of convex hull";
}

bool SpatialPressurePerformance::getCellValueVector(std::vector<Node *> &nodes,std::vector<double> &values, double x, double y, double h, double w, double dh, double dw)
{
    //calculate number of elements in vector
    uint xelements = uint(w/dw+1);
    uint yelements = uint(h/dh+1);

    //init vector
    for(uint index=0; index<xelements*yelements; index++)
        values.push_back(0);

    for(uint index=0; index < nodes.size(); index++)
    {
        DM::Node* currentnode = nodes[index];
        double cx = currentnode->getX();
        double cy = currentnode->getY();
        uint eindex = uint((cy-y)/dh)*xelements + uint((cx-x)/dw);

        double currentpressure = currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
        double currentdemand = currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble();

        if(currentpressure > values[eindex] && currentdemand > 0)
            values[eindex]=currentpressure;
    }

    return true;
}

bool SpatialPressurePerformance::createDelaunay(std::vector<Node *> &nodes, SpatialPressurePerformance::Delaunay &dt, std::map<Vertex,DM::Node*> &mapping,std::map<Vertex, K::FT, K::Less_xy_2> &function_values)
{
    if(!nodes.size())
        return false;

    for(uint index=0; index<nodes.size(); index++)
    {
        double pressure = nodes[index]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
        double demand = nodes[index]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble();

        if(demand<=0)
            continue;

        Vertex v(nodes[index]->getX(),nodes[index]->getY());
        dt.insert(v);
        function_values.insert(std::make_pair(v,pressure));
        mapping[v]=nodes[index];
    }
    return true;
}

double SpatialPressurePerformance::interpolateTriangle(Node *point, SpatialPressurePerformance::Delaunay &dt, std::map<Vertex, K::FT, K::Less_xy_2> &function_values)
{
    typedef CGAL::Data_access< std::map<Vertex, K::FT, K::Less_xy_2 > > Value_access;
    double v[3];
    point->get(v);
    Vertex p(v[0],v[1]);
    std::vector< std::pair< Vertex, K::FT > > coords;
    K::FT norm = CGAL::natural_neighbor_coordinates_2(dt, p,std::back_inserter(coords)).second;
    K::FT res =  CGAL::linear_interpolation(coords.begin(), coords.end(),norm,Value_access(function_values));
    return res;
}

double SpatialPressurePerformance::interpolateTriangle(std::vector<Node *> triangle, Node *point)
{
    double det =    (triangle[1]->getY() - triangle[2]->getY()) * (triangle[0]->getX() - triangle[2]->getX()) +
                    (triangle[2]->getX() - triangle[1]->getX()) * (triangle[0]->getY() - triangle[2]->getY());

    double l1 =     ((triangle[1]->getY() - triangle[2]->getY()) * (point->getX() - triangle[2]->getX()) +
                    (triangle[2]->getX() - triangle[1]->getX()) * (point->getY() - triangle[2]->getY())) / det;

    double l2 =     ((triangle[2]->getY() - triangle[0]->getY()) * (point->getX() - triangle[2]->getX()) +
                    (triangle[0]->getX() - triangle[2]->getX()) * (point->getY() - triangle[2]->getY())) / det;

    double l3 = 1.0f - l1 - l2;

    return  l1 * triangle[0]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble() +
            l2 * triangle[1]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble() +
            l3 * triangle[2]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
}
