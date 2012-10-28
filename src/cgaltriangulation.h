#ifndef CGAL_TRIANGULATION_H
#define CGAL_TRIANGULATION_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>
#include <iostream>

#include <dm.h>

struct FaceInfo2
{
  FaceInfo2(){}
  int nesting_level;

  bool in_domain(){
    return nesting_level%2 == 1;
  }
};


typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2,K>    Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<K,Fbb>        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
typedef CDT::Point                                                Point;
typedef CGAL::Polygon_2<K>                                        Polygon_2;

class CGALTriangulation
{
public:
    static void mark_domains(CDT& ct,  CDT::Face_handle start, int index, std::list<CDT::Edge>& border );
    static void mark_domains(CDT& cdt);
    static void Triangulation(DM::System * sys, DM::Face * f, std::vector<DM::Node> & triangels);
};

#endif // CGAL_TRIANGULATION_H
