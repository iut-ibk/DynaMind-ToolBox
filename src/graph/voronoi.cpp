#include <voronoi.h>
#include <tbvectordata.h>
#include <cgalgeometry.h>
#include <boost/polygon/polygon.hpp>
#include <boost/polygon/voronoi.hpp>
#include <boost/assert.hpp>

using namespace boost::polygon::operators;

typedef boost::polygon::point_data<int> int_point_t;
typedef boost::polygon::voronoi_diagram<double> double_voronoi_t;
typedef boost::polygon::point_data<double> double_point_t;
typedef boost::polygon::polygon_data<int> int_poly_t;
typedef boost::polygon::segment_data<double> double_segment_t;

bool Voronoi::createVoronoi(std::vector<DM::Component*> &nodes, DM::Component *face, DM::System *sys, DM::View view)
{
    std::vector<int_point_t> points;
    std::vector<int_point_t> shape;
    vector<int_poly_t> ps;
    int_poly_t shape_poly;

    for (std::vector<DM::Component*>::iterator i = nodes.begin(); i != nodes.end(); ++i)
    {
        DM::Node* currentnode = dynamic_cast<DM::Node*>((*i));
        points.push_back(int_point_t(currentnode->getX(),currentnode->getY()));
    }

    DM::Face* f = dynamic_cast<DM::Face*>(face);
    std::vector<DM::Node*> face_nodes = f->getNodePointers();
    for(uint index=0; index < face_nodes.size(); index++) {
        int_point_t p(face_nodes[index]->getX(), face_nodes[index]->getY());
        points.push_back(p);
        shape.push_back(p);
    }

    boost::polygon::set_points(shape_poly, shape.begin(), shape.end());
    vector<int_poly_t> _shape_poly;
    assign(_shape_poly, shape_poly);

    double_voronoi_t vd;
    construct_voronoi(points.begin(), points.end(), &vd);

    DM::Node centroid_points = DM::CGALGeometry::CaclulateCentroid2D(f);

    double x, y, h, w;
    TBVectorData::getBoundingBox(face_nodes, x, y, h, w, true);
    double max_bound = max(w,h);

    for (double_voronoi_t::const_cell_iterator c = vd.cells().begin(); c != vd.cells().end(); ++c)
    {
        const double_voronoi_t::edge_type *edge = c->incident_edge();
        vector<double_point_t> cell_poly_vs;
        int_poly_t cell_poly;

        do {
            assert(edge->vertex0() == edge->prev()->vertex1());
            /* add finite vertices to polygon */
            if (edge->is_finite()) {
                double v0_x = edge->vertex0()->x();
                double v0_y = edge->vertex0()->y();
                cell_poly_vs.push_back(double_point_t(v0_x, v0_y));
            }
            /* calculate (finite) far point (inspired by: scipy/spatial/_plotutils.py) */
            else {
                const double_voronoi_t::vertex_type *v = edge->vertex0() ? edge->vertex0() : edge->vertex1();
                double_point_t fv = double_point_t(v->x(), v->y());

                double_point_t a = points[c->source_index()];
                double_point_t b = points[edge->twin()->cell()->source_index()];
                double_point_t t = double_point_t(b.x() - a.x(), b.y() - a.y());
                double t_len = sqrt(t.x() * t.x() + t.y() * t.y());
                t = double_point_t(t.x() / t_len, t.y() / t_len);
                double_point_t n = double_point_t(-t.y(), t.x());
                double_point_t midpoint = boost::polygon::center(double_segment_t(a,b));
                double_point_t dir = double_point_t(midpoint.x() - centroid_points.getX(), midpoint.y() - centroid_points.getY());
                double f = dir.x() * n.x() + dir.y() * n.y() >= 0 ? 1. : -1.;
                dir = double_point_t(f * n.x(), f * n.y());
                double_point_t fp = double_point_t(dir.x() * max_bound, dir.y() * max_bound);
                fp = double_point_t(fp.x() + fv.x(), fp.y() + fv.y());

                if (edge->next()->is_infinite()) { /* first infinite edge */
                    assert(v == edge->vertex0());
                    cell_poly_vs.push_back(fv);
                }

                cell_poly_vs.push_back(fp);
            }
            edge = edge->next();
        } while (edge != c->incident_edge());

        boost::polygon::set_points(cell_poly, cell_poly_vs.begin(), cell_poly_vs.end());

        /* intersect (now finite) cell with shape */
        vector<int_poly_t> _clipped_cell_poly;
        vector<int_poly_t> _cell_poly;
        assign(_cell_poly, cell_poly);
        assign(_clipped_cell_poly, _shape_poly & _cell_poly); /* finally */

        assert(_clipped_cell_poly.size() == 1);
        int_poly_t clipped_cell_poly = _clipped_cell_poly[0];

        ps.push_back(clipped_cell_poly);
    }

    if(view.getType()==DM::EDGE) {
        DM::Logger(DM::Warning) << "EDGE type currently not implemented";
        return false;
    }

    if(view.getType()==DM::FACE) {
        for (size_t i=0; i<ps.size(); i++) {
            std::vector<DM::Node*> nodesvec;
            for (int_poly_t::iterator_type v = ps[i].begin(); v != ps[i].end(); ++v)
                nodesvec.push_back(new DM::Node(v->x(), v->y(),0));

            for(size_t index=0; index < nodesvec.size(); index++)
                sys->addNode(nodesvec[index]);

            DM::Face *newface = sys->addFace(nodesvec, view);
            newface->addAttribute("area", area(ps[i]));
        }
        return true;
    }

    return false;

}
