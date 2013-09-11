#include "prettyparcels.h"
#include "tbvectordata.h"
#include "cgalgeometry.h"
#include "dmgeometry.h"
#include "QPolygonF"
#include "QTransform"

DM_DECLARE_NODE_NAME(PrettyParcels, DynAlp)

PrettyParcels::PrettyParcels()
{
    this->in = DM::View("CITYBLOCK", DM::FACE, DM::READ);
    this->in.getAttribute("selected");
    this->out = DM::View("PARCEL", DM::FACE, DM::WRITE);
    this->out.addAttribute("selected");
    this->bbs = DM::View("BBS", DM::FACE, DM::WRITE);
    this->bbs.addAttribute("generation");

    this->out.addAttribute("generation");

    aspectRatio = 2;
    length = 100;
    offset = 1;
    remove_new = false;

    this->addParameter("AspectRatio", DM::DOUBLE, &aspectRatio);
    this->addParameter("Length", DM::DOUBLE, &length);
    this->addParameter("offset", DM::DOUBLE, & offset);
    this->addParameter("remove_new", DM::BOOL, & remove_new);

    InputViewName = "SUPERBLOCK";
    OutputViewName = "CITYBLOCK";

    this->addParameter("INPUTVIEW", DM::STRING, &InputViewName);
    this->addParameter("OUTPUTVIEW", DM::STRING, &OutputViewName);

    this->out.addLinks("SUPERBLOCK", in);
    std::vector<DM::View> datastream;
    datastream.push_back(bbs);
    datastream.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::MODIFY));

    this->addData("city", datastream);




}

void PrettyParcels::init()
{
    if (InputViewName.empty() || OutputViewName.empty())
        return;

    DM::System * sys = this->getData("city");

    if (!sys)
        return;

    DM::View * InputView = sys->getViewDefinition(InputViewName);

    if (!InputView)
        return;
    in = DM::View(InputView->getName(), InputView->getType(), DM::READ);
    this->in.getAttribute("selected");
    out = DM::View(OutputViewName, DM::FACE, DM::WRITE);
    this->out.addAttribute("selected");
    this->out.addAttribute("generation");

    std::vector<DM::View> datastream;


    datastream.push_back(in);
    datastream.push_back(out);
    datastream.push_back(bbs);


    this->addData("city", datastream);

}

/** The method is based on the minial bounding box */
void PrettyParcels::run(){

    DM::System * city = this->getData("city");

    std::vector<std::string> block_uuids = city->getUUIDs(this->in);

    //Here comes the action
    foreach (std::string uuid, block_uuids) {
        DM::Face *f  =city->getFace(uuid);
        if (f->getAttribute("selected")->getDouble() > 0.01) {
            this->createSubdevision(city, f, 0);

        }

    }


    if (!remove_new)
        return;
    foreach (std::string uuid, block_uuids) {
        DM::Face *f  =city->getFace(uuid);
        f->addAttribute("selected", 0);
    }
}

void PrettyParcels::createSubdevision(DM::System * sys, DM::Face *f, int gen)
{
    std::vector<DM::Node> box;
    std::vector<double> size;

    double alpha = DM::CGALGeometry::CalculateMinBoundingBox(TBVectorData::getNodeListFromFace(sys, f), box, size);

    DM::Face * bb;
    std::vector<DM::Node*> l_bb;
    foreach (DM::Node  n, box) {
        l_bb.push_back(sys->addNode(n));
    }
    l_bb.push_back(l_bb[0]);

    bb = sys->addFace(l_bb);

    DM::Node center = TBVectorData::CaclulateCentroid(sys, bb);

    double x_c = center.getX();
    double y_c = center.getY();


    if (this->length*2 > size[0]) {
        finalSubdevision(sys, f, gen+1);
        return;
    }
    //Create New Face
    QPolygonF f_origin = TBVectorData::FaceAsQPolgonF(sys, f);

    int elements = size[1]/(this->length);
    /*if (elements < 4) {
        elements = 3;
    } else {
        elements = 2;
    }*/
    elements = 2;
    for (int i = 0; i < 2; i++) {
        double l = size[0];
        double w = size[1];
        QRectF r1 (-l/2.+ i*l/(double)elements,  -w/2-10, l/(double)elements,w+10);

        QTransform t;
        t.rotate(alpha);

        QPolygonF intersection_tmp = t.map(r1);

        QTransform t1;
        t1.translate(x_c, y_c);
        QPolygonF intersection = t1.map(intersection_tmp);
        /*QPolygonF intersected = f_origin.intersected(intersection);

        if (intersected.size() == 0)
            intersected = intersection;*/

        std::vector<DM::Node* > intersection_p;
        for (int i = 0; i < intersection.size()-1; i++) {
            QPointF & p = intersection[i];
            intersection_p.push_back(sys->addNode(DM::Node(p.x(), p.y(), 0)));
        }
        intersection_p.push_back(intersection_p[0]);
        DM::Face * bb = sys->addFace(intersection_p, bbs);
        bb->addAttribute("generation", gen);
        std::vector<DM::Face *> intersected_faces = DM::CGALGeometry::IntersectFace(sys, f, bb);

        if (intersected_faces.size() == 0) {
            DM::Logger(DM::Warning) << "Advanced parceling createSubdevision interseciton failed";
            continue;
        }
        //std::vector<DM::Node*> newFace;

        foreach (DM::Face * f_new, intersected_faces) {
            f_new->addAttribute("generation", gen);
            this->createSubdevision(sys, f_new, gen+1);
        }

/*      newFace.push_back(sys->addNode(DM::Face(n.getX(), n.getY(), 0)));
        newFace.push_back(newFace[0]);

        DM::Face * f_new;
        f_new = sys->addFace(newFace);

        f_new->addAttribute("generation", gen);

        this->createSubdevision(sys, f_new, gen+1);
        */
    }
}

void PrettyParcels::finalSubdevision(DM::System *sys, DM::Face *f, int gen)
{
    //DM::Logger(DM::Debug) << "Start Final Subdevision";
    std::vector<DM::Node> box;
    std::vector<double> size;
    //QPolygonF f_origin = TBVectorData::FaceAsQPolgonF(sys, f);

    double alpha = DM::CGALGeometry::CalculateMinBoundingBox(TBVectorData::getNodeListFromFace(sys, f), box, size);

    DM::Face * bb;
    std::vector<DM::Node*> l_bb;
    foreach (DM::Node  n, box) {
        l_bb.push_back(sys->addNode(n));
    }
    l_bb.push_back(l_bb[0]);

    bb = sys->addFace(l_bb);

    DM::Node center = TBVectorData::CaclulateCentroid(sys, bb);

    //DM::Logger(DM::Debug) << alpha;
    //Finale spilts in the other direciton
    //Calculate Number of Splits
    int elements = size[1]/2/(this->length/this->aspectRatio)+1;
    //CreateSplitBox
    //DM::Logger(DM::Debug) << elements;
    //0---1---2
    //0---1---2---3
    for (int i = 0; i < elements; i++) {
        QRectF r1 (-size[0]/2.-10, -size[1]/2 + i*size[1]/((double)elements),  size[0]+10, size[1]/(double)elements);
        QTransform t;
        t.rotate(alpha);

        QPolygonF intersection_tmp = t.map(r1);

        QTransform t1;
        t1.translate(center.getX(), center.getY());
        QPolygonF intersection = t1.map(intersection_tmp);
        std::vector<DM::Node* > intersection_p;
        for (int i = 0; i < intersection.size()-1; i++) {
            QPointF & p = intersection[i];
            intersection_p.push_back(sys->addNode(DM::Node(p.x(), p.y(), 0)));
        }
        intersection_p.push_back(intersection_p[0]);

        std::vector<DM::Face *> intersected_face = DM::CGALGeometry::IntersectFace(sys, f, sys->addFace(intersection_p));

        if (intersected_face.size() == 0) {
            DM::Logger(DM::Warning) << "Final Intersection Failed";
            continue;
        }

        //std::vector<DM::Node*> newFace;


        foreach (DM::Face *newf, intersected_face) {
            //newFace.push_back(sys->addNode(DM::Node(n.getX(), n.getY(), 0)));
            //newFace.push_back(newFace[0]);

            std::vector<DM::Node *> newFace = newf->getNodePointers();

        if (offset > 0.0001) {
/*            DM::Logger(DM::Debug) << "----";
            DM::Logger(DM::Debug)<< newFace.size();
            foreach (DM::Node * n, newFace) {
                DM::Logger(DM::Debug) << n->getX() <<" " << n->getY();
            }
            DM::Logger(DM::Debug) << "Start offset"; */
            std::vector<DM::Node> new_parcel = DM::CGALGeometry::OffsetPolygon(newFace, offset);
            if (new_parcel.size() < 3) {
                DM::Logger(DM::Warning) << "Advaned offset interseciton failed";
                return;
            }
            std::vector<DM::Node*> newFace_Offset;

            foreach (DM::Node p, new_parcel) {
                newFace_Offset.push_back(sys->addNode(p));
            }
            newFace_Offset.push_back(newFace_Offset[0]);
            newFace = newFace_Offset;
            //DM::Logger(DM::Debug) << newFace.size();
            if (newFace.size() < 3) {
                DM::Logger(DM::Warning) << "Advaned parceling interseciton failed";
                continue;
            }
        }
        DM::Face * f_new = sys->addFace(newFace, this->out);
        f_new->addAttribute("generation", gen);
        f_new->addAttribute("selected", 1);

        //f_new->getAttribute("SUPERBLOCK")->setLink(in.getName(), f->getUUID());

    }
    //DM::Logger(DM::Debug) << "Done Final Subdevision";
}




}
