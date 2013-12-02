#include <voronoi.h>
#include <tbvectordata.h>
#include <cgalgeometry.h>

//TODO implement integration of infinite egdes

using namespace boost::polygon;

bool Voronoi::createVoronoi(std::vector<DM::Component*> &nodes,std::vector<DM::Component*> &face,DM::System *sys, DM::View v)
{
	typedef std::vector<DM::Component*>::const_iterator Citerator;
    typedef voronoi_diagram<double>::const_edge_iterator const_edge_iterator;
    typedef voronoi_diagram<double>::const_cell_iterator const_cell_iterator;

    if(face.size()>1)
        return false;

    std::vector<Point> points;
    std::vector<DM::Node*> nodepointers;
    std::vector<Segment> segments;

    for(Citerator i = nodes.begin(); i != nodes.end(); ++i)
    {
		DM::Node* currentnode = dynamic_cast<DM::Node*>((*i));
        points.push_back(Point(currentnode->getX(),currentnode->getY()));
        nodepointers.push_back(currentnode);
    }

    for(Citerator i = face.begin(); i != face.end(); ++i)
    {
		DM::Face* currentface = dynamic_cast<DM::Face*>((*i));
        std::vector<DM::Node*> nodes = currentface->getNodePointers();

        for(uint index=0; index < nodes.size(); index++)
        {
            uint index2 = 0;
            if(index != nodes.size()-1)
                index2=index+1;

            segments.push_back(Segment(nodes[index]->getX(), nodes[index]->getY(),nodes[index2]->getX(), nodes[index2]->getY()));
        }
    }

    voronoi_diagram<double> vd;
    construct_voronoi(points.begin(), points.end(), segments.begin(), segments.end(), &vd);

    if(v.getType()==DM::EDGE)
    {
        for (const_edge_iterator it = vd.edges().begin();
            it != vd.edges().end(); ++it)
        {
            if (it->is_finite())
            {
                DM::Node start(it->vertex0()->x(), it->vertex0()->y(),0);
                DM::Node end(it->vertex1()->x(), it->vertex1()->y(),0);

                if(!TBVectorData::PointWithinAnyFace(face,&start) || !TBVectorData::PointWithinAnyFace(face,&end))
                {
                    continue;
                }

                sys->addComponentToView(sys->addEdge(new DM::Edge(
                    sys->addNode(new DM::Node(start)),
                    sys->addNode(new DM::Node(end)))),
                    v);
            }
        }

        return true;
    }

    if(v.getType()==DM::FACE)
    {
        for (const_cell_iterator cit = vd.cells().begin();
            cit != vd.cells().end(); ++cit)
        {
            const voronoi_diagram<double>::cell_type &cell = *cit;
            const voronoi_diagram<double>::edge_type *edge = cell.incident_edge();

            if(!cell.contains_point())
                continue;

            std::vector<DM::Node*> nodesvec;

            do {
                    if (edge->is_primary())
                    {
                        if(edge->vertex0()!=0)
                        {
                            nodesvec.push_back(new DM::Node(edge->vertex0()->x(), edge->vertex0()->y(),0));
                            if(!TBVectorData::PointWithinAnyFace(face,nodesvec[nodesvec.size()-1]))
                            {
                                for(int index=0; index < nodesvec.size(); index++)
                                  delete nodesvec[index];

                                nodesvec.clear();
                                break;
                            }
                        }
                    }

                    edge = edge->next();
                } while (edge != cell.incident_edge());

            if(nodesvec.size() > 0)
            {
                for(int index=0; index < nodesvec.size(); index++)
                    sys->addNode(nodesvec[index]);

                DM::Face *newface = sys->addFace(nodesvec,v);

                newface->addAttribute("area",DM::CGALGeometry::CalculateArea2D(newface));



                uint pindex = cell.source_index();

                if(pindex > nodepointers.size())
                    continue;

                DM::Node *sourcenode = nodepointers[pindex];

                if(sourcenode!=0 && newface!=0)
                {
                    sourcenode->getAttribute("VoronoiCell")->setString(newface->getUUID());
                }
            }
        }

        return true;
    }

    return false;
}
