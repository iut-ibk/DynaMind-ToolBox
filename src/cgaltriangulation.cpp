/**
 * @file
 * @author  Christian Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
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

#include "cgaltriangulation.h"
#include <tbvectordata.h>
#include <dmgeometry.h>

void CGALTriangulation::mark_domains(CDT& ct,  CDT::Face_handle start, int index, std::list<CDT::Edge>& border)
{
    if(start->info().nesting_level != -1){
        return;
    }
    std::list<CDT::Face_handle> queue;
    queue.push_back(start);

    while(! queue.empty()){
        CDT::Face_handle fh = queue.front();
        queue.pop_front();
        if(fh->info().nesting_level == -1){
            fh->info().nesting_level = index;
            for(int i = 0; i < 3; i++){
                CDT::Edge e(fh,i);
                CDT::Face_handle n = fh->neighbor(i);
                if(n->info().nesting_level == -1){
                    if(ct.is_constrained(e)) border.push_back(e);
                    else queue.push_back(n);
                }
            }
        }
    }
}
//explore set of facets connected with non constrained edges,
//and attribute to each such set a nesting level.
//We start from facets incident to the infinite vertex, with a nesting
//level of 0. Then we recursively consider the non-explored facets incident
//to constrained edges bounding the former set and increase the nesting level by 1.
//Facets in the domain are those with an odd nesting level.
void CGALTriangulation::mark_domains(CDT& cdt)
{
    for(CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it){
        it->info().nesting_level = -1;
    }

    int index = 0;
    std::list<CDT::Edge> border;
    mark_domains(cdt, cdt.infinite_face(), index++, border);
    while(! border.empty()){
        CDT::Edge e = border.front();
        border.pop_front();
        CDT::Face_handle n = e.first->neighbor(e.second);
        if(n->info().nesting_level == -1){
            mark_domains(cdt, n, e.first->info().nesting_level+1, border);
        }
    }
}

void insert_polygon(CDT& cdt,const Polygon_2& polygon){
    if ( polygon.is_empty() ) return;
    CDT::Vertex_handle v_prev=cdt.insert(*boost::prior(polygon.vertices_end()));
    for (Polygon_2::Vertex_iterator vit=polygon.vertices_begin();
         vit!=polygon.vertices_end();++vit)
    {
        CDT::Vertex_handle vh=cdt.insert(*vit);
        cdt.insert_constraint(vh,v_prev);
        v_prev=vh;
    }
}

void CGALTriangulation::Triangulation(DM::System *sys, DM::Face *f, std::vector<DM::Node> & triangles)
{

    //Make Place Plane
    std::vector<DM::Node*> nodeList = TBVectorData::getNodeListFromFace(sys, f);

    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    TBVectorData::CorrdinateSystem( *(nodeList[0]), *(nodeList[1]), *(nodeList[nodeList.size()-2]), E_to);

    double alphas[3][3];
    TBVectorData::RotationMatrix(E, E_to, alphas);

    double alphas_t[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            alphas_t[j][i] =  alphas[i][j];
        }
    }

    DM::System transformedSys;
    DM::SpatialNodeHashMap transfromedSysSNH(&transformedSys,100,false);
    std::vector<DM::Node*> ns_t;
    for (unsigned int i = 0; i < nodeList.size(); i++) {
        DM::Node n = *(nodeList[i]);
        DM::Node n_t =  TBVectorData::RotateVector(alphas, n);
        transfromedSysSNH.addNode(n_t.getX(), n.getY(), n.getZ(), 0.0001);
        ns_t.push_back( transfromedSysSNH.addNode(n_t.getX(), n_t.getY(), n_t.getZ(), 0.0001));
    }

    DM::Face * f_t = transformedSys.addFace(ns_t);


    CDT cdt;
    Polygon_2 polygon1;
    std::vector<std::string> nodes;
    nodes = f_t->getNodes();
	for (unsigned int  i = 0; i <nodes.size(); i++ ) {
        DM::Node * n = transformedSys.getNode(nodes[i]);
        polygon1.push_back(Point(n->getX(),n->getY()));
    }
    //Insert the polyons into a constrained triangulation

    insert_polygon(cdt,polygon1);

    //Add Holes: Holes use the same transormation matrix
    std::vector<std::vector<std::string> > holes = f->getHoles();
    foreach (std::vector<std::string> hole, holes) {
        std::vector<DM::Node* > nodes_h;
        foreach (std::string nuuid, hole) {
            DM::Node * n = sys->getNode(nuuid);
            DM::Node n_t = TBVectorData::RotateVector(alphas, *n);
            nodes_h.push_back(transfromedSysSNH.addNode(n_t.getX(), n_t.getY(), n_t.getZ(), 0.0001));
        }
        Polygon_2 hole_p;
		for (unsigned int  i = 0; i <nodes_h.size(); i++ ) {
            DM::Node * n = nodes_h[i];
            hole_p.push_back(Point(n->getX(),n->getY()));
        }
        insert_polygon(cdt,hole_p);
    }

    //Mark facets that are inside the domain bounded by the polygon
    mark_domains(cdt);

    int count=0;
    for (CDT::Finite_faces_iterator fit=cdt.finite_faces_begin();
         fit!=cdt.finite_faces_end();++fit)
    {
        if ( fit->info().in_domain() ) ++count;
        if (fit->info().in_domain() ) {
            for (int i = 0; i < 3; i++) {
                DM::Node * n_t = transfromedSysSNH.findNode(fit->vertex(i)->point().x(),  fit->vertex(i)->point().y(), 0.0001);
                if (!n_t) {
                    DM::Logger(DM::Warning) << "Transformend Node doesn't exist trinagulation failed";
                    return  triangles.clear();
                }

                triangles.push_back( TBVectorData::RotateVector(alphas_t, DM::Node( fit->vertex(i)->point().x(),  fit->vertex(i)->point().y(), n_t->getZ())));
            }
        }
    }

}
