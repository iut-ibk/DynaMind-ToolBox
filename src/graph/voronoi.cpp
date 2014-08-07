#include <voronoi.h>
#include <tbvectordata.h>
#include <cgalgeometry.h>
#include <boost/polygon/polygon.hpp>
#include <boost/polygon/voronoi.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/assert.hpp>

using namespace boost::polygon::operators;

typedef boost::polygon::point_data<int> int_point_t;
typedef boost::polygon::polygon_data<int> int_poly_t;
typedef boost::polygon::voronoi_diagram<double> double_voronoi_t;
typedef boost::polygon::point_data<double> double_point_t;
typedef boost::polygon::segment_data<double> double_segment_t;

static vector<boost::tuple<bool, int_point_t, int_poly_t> > shape2cells(int_poly_t shape, vector<int_poly_t> cells, vector<int_point_t> points)
{
    vector<boost::tuple<bool, int_point_t, int_poly_t> > new_cells;
    vector<int_poly_t> _shape;
    assign(_shape, shape);

    for (size_t i=0; i<cells.size(); i++) {
        int_point_t sp = points[i];
        vector<int_poly_t> _clipped_cells;
        vector<int_poly_t> _cell;
        assign(_cell, cells[i]);
        assign(_clipped_cells, _shape & _cell);

        for (size_t j=0; j<_clipped_cells.size(); j++) {
            int_poly_t p = _clipped_cells[j];
            if (contains(p, sp, true))
                new_cells.push_back(boost::tuple<bool,int_point_t,int_poly_t>(true, sp, p));
            else {
                int_point_t np;
                center(np, p);

                /* TODO: fails on concave polygons; take center of inner circle? */
                if (!contains(p, np, false))
                    DM::Logger(DM::Warning) << "fatal: calculated point outside of polygon - bad things might happen";

                new_cells.push_back(boost::tuple<bool,int_point_t,int_poly_t>(false, np, p));
            }
        }
    }

    return new_cells;
}

static vector<int_poly_t> make_finite_vd(vector<int_point_t> points)
{
    double_point_t centroid_point;
    vector<int_poly_t> ps(points.size());
    double sum_x = 0, sum_y = 0;
    double min_x = INT_MAX, min_y = INT_MAX, max_x = INT_MIN, max_y = INT_MIN;
    double max_bound;

    double_voronoi_t vd;
    construct_voronoi(points.begin(), points.end(), &vd);

    /* find centroid of points min/max */
    for (size_t i=0; i<points.size(); i++) {
        double px = points[i].x();
        double py = points[i].y();
        sum_x += px;
        sum_y += py;
        min_x = px < min_x ? px : min_x;
        min_y = py < min_y ? py : min_y;
        max_x = px > max_x ? px : max_x;
        max_y = py > max_y ? py : max_y;
    }
    centroid_point = double_point_t(sum_x / points.size(), sum_y / points.size());

    /* max side of bounding box */
    max_bound = max(max_x - min_x, max_y - min_y);

    /* construct clipped regions (polygons) */
    for (double_voronoi_t::const_cell_iterator c = vd.cells().begin(); c != vd.cells().end(); ++c) {
        const double_voronoi_t::edge_type *edge = c->incident_edge();
        vector<double_point_t> cell_poly_vs;
        int_poly_t cell_poly;

        do {
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
                double_point_t t = double_point_t(x(b) - x(a), y(b) - y(a));
                double t_len = sqrt(x(t) * x(t) + y(t) * y(t));
                t = double_point_t(x(t) / t_len, y(t) / t_len);
                double_point_t n = double_point_t(-y(t), x(t));
                double_point_t midpoint = boost::polygon::center(double_segment_t(a,b));
                double_point_t dir = double_point_t(midpoint.x() - centroid_point.x(), midpoint.y() - centroid_point.y());
                double f = dir.x() * n.x() + dir.y() * n.y() >= 0 ? 1. : -1.;
                dir = double_point_t(f * n.x(), f * n.y());
                double_point_t fp = double_point_t(dir.x() * max_bound, dir.y() * max_bound);
                fp = double_point_t(fp.x() + fv.x(), fp.y() + fv.y());

                /* Always two there are. No more, no less. A Master and an apprentice. (Yoda) */
                if (edge->next()->is_infinite()) { /* first infinite edge */
                    assert(v == edge->vertex0());
                    cell_poly_vs.push_back(fv);
                }

                cell_poly_vs.push_back(fp);
            }
            edge = edge->next();
        } while (edge != c->incident_edge());

        boost::polygon::set_points(cell_poly, cell_poly_vs.begin(), cell_poly_vs.end());
        ps[c->source_index()] = cell_poly;
    }

    return ps;
}

bool Voronoi::createVoronoi(std::vector<DM::Component*> &nodes, std::vector<DM::Component*> &faces, DM::System *sys, DM::View view)
{
    std::vector<int_point_t> points;
    vector<boost::tuple<bool, int_point_t, int_poly_t> > all_cells;

    for (size_t i=0; i<nodes.size(); i++) {
        DM::Node *currentnode = dynamic_cast<DM::Node*>(nodes[i]);
        points.push_back(int_point_t(currentnode->getX(), currentnode->getY()));
    }

    vector<int_poly_t> cells = make_finite_vd(points);
    for (size_t i=0; i<faces.size(); i++) {
        DM::Face *face = dynamic_cast<DM::Face *>(faces[i]);
        std::vector<DM::Node *> face_nodes = face->getNodePointers();
        std::vector<int_point_t> shape;
        for (size_t j=0; j<face_nodes.size(); j++) {
            shape.push_back(int_point_t(face_nodes[j]->getX(), face_nodes[j]->getY()));
        }

        int_poly_t shape_poly;
        boost::polygon::set_points(shape_poly, shape.begin(), shape.end());
        vector<boost::tuple<bool, int_point_t, int_poly_t> > new_cells = shape2cells(shape_poly, cells, points);
        all_cells.insert(all_cells.end(), new_cells.begin(), new_cells.end());
    }

    size_t non_voronoi_points = 0;
    for (size_t i=0; i<all_cells.size(); i++) {
        bool is_voronoi_point = all_cells[i].get<0>();
        if (!is_voronoi_point)
            non_voronoi_points++;

        int_point_t p = all_cells[i].get<1>();
        int_poly_t poly = all_cells[i].get<2>();
        std::vector<DM::Node *> cell_nodes;
        for (int_poly_t::iterator_type v = poly.begin(); v != poly.end(); ++v) {
            DM::Node *node = new DM::Node(v->x(), v->y(), 0);
            cell_nodes.push_back(node);
            sys->addNode(node);
        }

        DM::Face *newface = sys->addFace(cell_nodes, view);
        newface->addAttribute("area", area(poly));

        DM::Node *n = new DM::Node(p.x(), p.y(), 0);
        n->getAttribute("VoronoiCell")->setString(newface->getUUID());
    }

    DM::Logger(DM::Warning) << "non voronoi points:" << non_voronoi_points;

    return true;
}

