#include "cgalregulartriangulation.h"
#include <tbvectordata.h>
#include <dmgeometry.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Polygon_2.h>

#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;

typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;
typedef CGAL::Polygon_2<K> Polygon_2;


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

void CGALRegularTriangulation::Triangulation(DM::System * sys, DM::Face * f, std::vector<DM::Node> & triangels, double meshsize, std::vector<int> & ids)
{

    //Make Place Plane
    std::vector<DM::Node*> nodeList = TBVectorData::getNodeListFromFace(sys, f);

    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    TBVectorData::CorrdinateSystem( *(nodeList[0]), *(nodeList[1]), *(nodeList[2]), E_to);

    double alphas[3][3];
    TBVectorData::RotationMatrix(E, E_to, alphas);

    double alphas_t[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            alphas_t[j][i] =  alphas[i][j];
        }
    }

    DM::System transformedSys;

    std::vector<DM::Node*> ns_t;
    double const_height;
    for (int i = 0; i < nodeList.size(); i++) {
        DM::Node n = *(nodeList[i]);
        DM::Node n_t =  TBVectorData::RotateVector(alphas, n);
        ns_t.push_back(transformedSys.addNode(n_t));
        const_height = n_t.getZ();
    }

    DM::Face * f_t = transformedSys.addFace(ns_t);


    CDT cdt;
    Polygon_2 polygon1;
    std::vector<std::string> nodes;
    nodes = f_t->getNodes();
    for (int  i = 0; i <nodes.size()-1; i++ ) {
        DM::Node * n = transformedSys.getNode(nodes[i]);
        polygon1.push_back(Point(n->getX(),n->getY()));
    }
    //Insert the polyons into a constrained triangulation

    insert_polygon(cdt,polygon1);
    std::list<Point> list_of_seeds;
    //Add Holes: Holes use the same transormation matrix
    std::vector<std::vector<std::string> > holes = f->getHoles();
    foreach (std::vector<std::string> hole, holes) {
        std::vector<DM::Node* > nodes_h;
        foreach (std::string nuuid, hole) {
            DM::Node * n = sys->getNode(nuuid);
            DM::Node n_t = TBVectorData::RotateVector(alphas, *n);
            nodes_h.push_back(transformedSys.addNode(n_t));
        }
        DM::Face * f_h = transformedSys.addFace(nodes_h);
        DM::Node center_h = TBVectorData::CaclulateCentroid(&transformedSys, f_h);
        list_of_seeds.push_back(Point(center_h.getX(), center_h.getY()));
        Polygon_2 hole_p;
        for (int  i = 0; i <nodes_h.size()-1; i++ ) {
            DM::Node * n = nodes_h[i];
            hole_p.push_back(Point(n->getX(),n->getY()));
        }
        insert_polygon(cdt,hole_p);
    }


    std::cout << "Number of vertices: " << cdt.number_of_vertices() << std::endl;

    std::cout << "Meshing the domain..." << std::endl;
    CGAL::refine_Delaunay_mesh_2(cdt, list_of_seeds.begin(), list_of_seeds.end(),
                                 Criteria(0.125, meshsize));

    std::cout << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
    std::cout << "Number of finite faces: " << cdt.number_of_faces() << std::endl;
    /*int mesh_faces_counter = 0;
    for(CDT::Finite_faces_iterator fit = cdt.finite_faces_begin();
        fit != cdt.finite_faces_end(); ++fit)
    {
        if(fit->is_in_domain()){
            ++mesh_faces_counter;
            for (int i = 0; i < 3; i++) {
                triangels.push_back( TBVectorData::RotateVector(alphas_t, DM::Node( fit->vertex(i)->point().x(),  fit->vertex(i)->point().y(), const_height)));
            }

        }

    }
    std::cout << "Number of faces in the mesh domain: " << mesh_faces_counter << std::endl;*/

    int mesh_faces_counter = 0;
    int count=0;
    DM::System nodesearch;
    DM::SpatialNodeHashMap spnh(&nodesearch, 0.1);
    std::map<DM::Node * , int> nodeids;
    std::map<int, DM::Node * > idsnode;
    int id = 0;
    for (CDT::Finite_faces_iterator fit=cdt.finite_faces_begin();
         fit!=cdt.finite_faces_end();++fit){
        if (fit->is_in_domain() ) {
            for (int i = 0; i < 3; i++){
                bool newlyAdded = false;
                if (!spnh.findNode(fit->vertex(i)->point().x(),  fit->vertex(i)->point().y(), 0.001)) {
                    newlyAdded = true;
                }
                DM::Node * n = spnh.addNode(fit->vertex(i)->point().x(),  fit->vertex(i)->point().y(), const_height, 0.001);
                if (newlyAdded)  {
                    idsnode[count] = n;
                    nodeids[n] = count++;

                }
                ids.push_back(nodeids[n]);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        DM::Node * n = idsnode[i];
        triangels.push_back(TBVectorData::RotateVector(alphas_t, DM::Node(n->getX(), n->getY(), n->getZ())));
    }
    std::cout << "Number of faces in the mesh domain: " << mesh_faces_counter << std::endl;

}


