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

#include <setZfromRasterdata.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//CGAL
#include <CGAL/Interpolation_traits_2.h>
#include <CGAL/natural_neighbor_coordinates_2.h>
#include <CGAL/interpolation_functions.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>

DM_DECLARE_NODE_NAME(SetZfromRasterdata,Modules)

SetZfromRasterdata::SetZfromRasterdata()
{   
    std::vector<DM::View> views;
    DM::View view;

    view = DM::View("NODES", DM::NODE, DM::MODIFY);
    views.push_back(view);
    viewdef["NODES"]=view;

    //Nodes which should be connected to the graph
    view = DM::View("ELEVATION", DM::RASTERDATA, DM::READ);
    views.push_back(view);
    viewdef["ELEVATION"]=view;

    this->addData("Layout", views);

    this->offset = -1.5;
    this->addParameter("Offset [m]", DM::DOUBLE, &this->offset);

    this->interpolate = false;
    this->addParameter("Interpolate with delaunay triangulation:", DM::BOOL, &this->interpolate);
}

void SetZfromRasterdata::run()
{
    sys = this->getData("Layout");
    r = this->getRasterData("Layout", viewdef["ELEVATION"]);
    std::map<std::string,DM::Component*> nodes = sys->getAllComponentsInView(viewdef["NODES"]);

    if(!r || !nodes.size())
    {
        DM::Logger(DM::Error) << "Could not find Rasterdata and/or nodes";
        return;
    }

    if(this->interpolate)
        setZWithDelaunay(r,nodes,this->offset);
    else
        setZWithRaster(r,nodes,this->offset);

    return;
}

bool SetZfromRasterdata::setZWithRaster(DM::RasterData * r, std::map<std::string, DM::Component*> &nodes,double offset)
{
    typedef std::map<std::string, DM::Component*>::iterator Iter;

    for(Iter it = nodes.begin(); it != nodes.end(); it++)
    {
        DM::Node *currentnode = static_cast<DM::Node*>((*it).second);
        currentnode->setZ(r->getValue(currentnode->getX(),currentnode->getY())+this->offset);
    }
    return true;
}

bool SetZfromRasterdata::setZWithDelaunay(DM::RasterData * r, std::map<std::string, DM::Component*> &nodes,double offset)
{
    SetZfromRasterdata::Delaunay dt;
    std::map<Vertex, K::FT, K::Less_xy_2> function_values;
    typedef std::map<std::string, DM::Component*>::iterator Iter;

    if(!nodes.size())
        return false;

    //triangulate rester data
    double xsellsize = r->getCellSizeX();
    double ysellsize = r->getCellSizeY();

    double yinit = r->getYOffset()+ysellsize/2;
    double xinit = r->getXOffset()+xsellsize/2;
    int errorcount = 0;

    std::vector<Vertex> vvec;

    for(uint index = 0; index < r->getHeight()*r->getWidth(); index++)
    {
        uint xindex = index/r->getWidth();
        uint yindex = index%r->getWidth();
        Vertex v(xinit+xindex*xsellsize,yinit+yindex*ysellsize);
        vvec.push_back(v);
        function_values.insert(std::make_pair(v,r->getCell(xindex,yindex)));
        if(index%1000000==0)
            DM::Logger(DM::Standard) << int(index) << " of " << r->getHeight()*r->getWidth();
    }

    dt.insert(vvec.begin(),vvec.end());

    for(Iter it = nodes.begin(); it != nodes.end(); it++)
    {
        DM::Node *currentnode = static_cast<DM::Node*>((*it).second);

        int li;
        typedef Delaunay::Locate_type Locate_type;
        Locate_type lt;
        Vertex v(currentnode->getX(), currentnode->getY());

        dt.locate(v,lt,li);

        switch(lt)
        {
            case Delaunay::OUTSIDE_AFFINE_HULL:
            {
                errorcount++;
                break;
            }

            case Delaunay::OUTSIDE_CONVEX_HULL:
            {
                errorcount++;
                break;
            }

            default :
            {
                double elevation = interpolateTriangle(currentnode,dt,function_values);
                currentnode->setZ(elevation+offset);
            }
        }
    }

    if(errorcount > 0)
        DM::Logger(DM::Error) << "Could not find elevation for " << errorcount << " nodes";
    return true;
}

double SetZfromRasterdata::interpolateTriangle(DM::Node *point, SetZfromRasterdata::Delaunay &dt, std::map<Vertex, K::FT, K::Less_xy_2> &function_values)
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
