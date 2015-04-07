#include <urbandevelDivision.h>
#include <tbvectordata.h>
#include <cgalgeometry.h>
#include <dmgeometry.h>
#include <QPolygonF>
#include <QTransform>
#include <cgalgeometry_p.h>
#include <tbvectordata.h>
#include <cgalgeometry.h>

DM_DECLARE_NODE_NAME(urbandevelDivision, DynAlp)

urbandevelDivision::urbandevelDivision()
{
    inputname = "SUPERBLOCK";
    this->addParameter("Input View", DM::STRING, &this->inputname);
    outputname = "CITYBLOCK";
    this->addParameter("Output View", DM::STRING, &this->outputname);

    aspectratio = 2;
    this->addParameter("AspectRatio", DM::DOUBLE, &this->aspectratio);
    length = 100;
    this->addParameter("Length", DM::DOUBLE, &this->length);
    offset = 5;
    this->addParameter("offset", DM::DOUBLE, &this->offset);
    tol = 0.00001; //should not be to small
    this->addParameter("tolerance", DM::DOUBLE, &this->tol);
    debug = false;
    this->addParameter("debug", DM::BOOL, &this->debug);
    combined_edges = false;
    this->addParameter("combined_edges", DM::BOOL, &this->combined_edges);
    splitShortSide = false;
    this->addParameter("splitShortSide", DM::BOOL, &this->splitShortSide);
    develtype = "res";
    this->addParameter("develtype (ignore if empty)", DM::STRING, &this->develtype);

    bbs = DM::View("BBS", DM::FACE, DM::WRITE);
    std::vector<DM::View> data;
    data.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::MODIFY));

    this->addData("data", data);

}

urbandevelDivision::~urbandevelDivision()
{

}

void urbandevelDivision::init()
{
    if (inputname.empty() || outputname.empty())
        return;

    inputView = DM::View(inputname, DM::FACE, DM::READ);
    outputView = DM::View(outputname, DM::FACE, DM::WRITE);
    outputView_nodes = DM::View(outputname+"_NODES", DM::NODE, DM::WRITE);
    cityview = DM::View("CITY", DM::NODE, DM::READ);

    cityview.addAttribute("currentyear", DM::Attribute::DOUBLE, DM::READ);

    inputView.addAttribute("status", DM::Attribute::STRING, DM::READ);

    outputView.addAttribute("status", DM::Attribute::STRING, DM::WRITE);
    outputView.addAttribute("generation", DM::Attribute::DOUBLE, DM::WRITE);
    outputView.addAttribute("year", DM::Attribute::DOUBLE, DM::WRITE);

    outputView_nodes.addAttribute("street_side", DM::Attribute::DOUBLE, DM::WRITE);

    std::vector<DM::View> data;

    data.push_back(cityview);
    data.push_back(inputView);
    data.push_back(outputView);
    data.push_back(outputView_nodes);
    //if (debug)
    //    datastream.push_back(DM::View("faces_debug", DM::FACE, DM::WRITE));

    this->addData("data", data);
}

/** The method is based on the minial bounding box */
void urbandevelDivision::run(){

    //DM::Logger(DM::Warning) << "Redevelopment not finished yet - offset is still missing";
    if (this->aspectratio < 1) {
        DM::Logger(DM::Warning) <<  "Aspect Ration < 1 please, just values > 1 are used";
    }

    DM::System * sys = this->getData("data");
    DM::SpatialNodeHashMap sphs(sys,100,false);

    std::vector<DM::Component *> cities = sys->getAllComponentsInView(cityview);
    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component * currentcityview = cities[0];

    currentyear = static_cast<int>(currentcityview->getAttribute("currentyear")->getDouble());

    std::vector<DM::Component *> inputareas = sys->getAllComponentsInView(inputView);

    for (int i = 0; i < inputareas.size(); i++)
    {
        DM::System workingSys;
        DM::Face * f = static_cast<DM::Face *> (inputareas[i]);
        std::string inputtype = f->getAttribute("type")->getString();
        std::string inputstatus = f->getAttribute("status")->getString();

        if ( inputstatus != "develop" )
        {
            DM::Logger(DM::Debug) << "not parceling as status = " << inputstatus;
            continue;
        }

        if ( develtype != inputtype || develtype.empty() )
        {
            DM::Logger(DM::Debug) << "not parceling as type = " << inputtype;
            continue;
        }

        DM::Logger(DM::Debug) << "parceling";

        DM::Face * fnew = TBVectorData::CopyFaceGeometryToNewSystem(f, &workingSys);
        workingSys.addComponentToView(fnew, inputView);
        this->createSubdivision(&workingSys, fnew, 0, inputtype);
        createFinalFaces(&workingSys, sys, fnew, outputView, sphs);
        DM::Logger(DM::Debug) << "end parceling";

        f->addAttribute("status", "developed");
    }
}

string urbandevelDivision::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/tree/master/doc/output/html/urbandevelDivision.html";
}

void urbandevelDivision::createSubdivision(DM::System * sys,  DM::Face *f, int gen, std::string type)
{
    bool split_length = false;
    std::vector<DM::Node> box;
    std::vector<double> size;

    double alpha = DM::CGALGeometry::CalculateMinBoundingBox(TBVectorData::getNodeListFromFace(sys, f), box, size);


    DM::Face * bb;
    std::vector<DM::Node*> l_bb;
    foreach (DM::Node  n, box) {
        l_bb.push_back(sys->addNode(n));
    }

    bb = sys->addFace(l_bb);

    DM::Node center = DM::CGALGeometry::CalculateCentroid(sys, bb);

    double x_c = center.getX();
    double y_c = center.getY();

    double split_l = size[0];
    double split_width = size[1];

    if (2*this->length > split_l) {
        if ( (this->length / this->aspectratio) * 2 >   split_width) { //width
            sys->addComponentToView(f, this->outputView);
            return;
        }
    }

    if (this->splitShortSide) {
         split_l = size[1];
         split_width = size[0];
    }
    if (2*this->length > split_l)
        split_length = true;
    int elements = 2;
    if (split_length) {
        if(!this->splitShortSide)
            elements = size[1] / (this->length / this->aspectratio);
        else
            elements = size[0] / (this->length / this->aspectratio);
    }
    for (int i = 0; i < elements; i++) {
        double l = size[0];
        double w = size[1];
        QRectF r1;
        //if (!this->splitShortSide)
            r1 = QRectF(-l/2.+ i*l/(double)elements,  -w/2-10, l/(double)elements,w+10);
        if (this->splitShortSide && !split_length)
            r1 = QRectF(-l/2.-10,  -w/2. + i*w/(double)elements, l + 10,w/(double)elements);

        //Rotate Intersection Rect
        QTransform t;
        t.rotate(alpha);

        QPolygonF intersection_tmp = t.map(r1);

        QTransform t1;
        t1.translate(x_c, y_c);
        QPolygonF intersection = t1.map(intersection_tmp);

        std::vector<DM::Node* > intersection_p;
        for (int i = 0; i < intersection.size()-1; i++) {
            QPointF & p = intersection[i];
            intersection_p.push_back(sys->addNode(DM::Node(p.x(), p.y(), 0)));
        }

        DM::Face * bb = sys->addFace(intersection_p, bbs);

        bb->addAttribute("type", gen);
        std::vector<DM::Face *> intersected_faces = DM::CGALGeometry::IntersectFace(sys, f, bb);

        if (intersected_faces.size() == 0) {
            DM::Logger(DM::Debug) << "Advanced parceling createSubdivision intersction failed";
            continue;
        }

        foreach (DM::Face * f_new ,intersected_faces ) {
            this->createSubdivision(sys, f_new, gen+1, type);
        }
    }
}

std::vector<DM::Node *> urbandevelDivision::extractCGALFace(Arrangement_2::Ccb_halfedge_const_circulator hec, DM::SpatialNodeHashMap & sphs)
{
    std::vector<DM::Node *> vp;
    Arrangement_2::Ccb_halfedge_const_circulator curr = hec;
    do{
        double x = CGAL::to_double(curr->source()->point().x());
        double y = CGAL::to_double(curr->source()->point().y());
        DM::Node * n = sphs.addNode(x,y,0,tol, this->outputView_nodes);
        if (curr->twin()->face()->is_unbounded())
            n->addAttribute("street_side", 1);
        vp.push_back(n);

    }
    while(++curr != hec );

    return vp;
}

bool urbandevelDivision::checkIfHoleFilling(DM::Face * orig, DM::Face * face_new)
{
    // Create Small offest and check if node is in hole or node to identify if this is just a filling of a hole
    for (double i = -1; i < 2; i++) {
        for (double j = -1; j < 2; j++) {
            if (i == 0 || j == 0)
                continue;
            DM::Node * en = face_new->getNodePointers()[0];
            DM::Node n(en->getX() + tol*10.* i, en->getY() + tol*10. * i, en->getZ() + tol*10.* i);
            if (debug){
                DM::Logger(DM::Debug)<< "Number of holes" << orig->getHolePointers().size();
                DM::Logger(DM::Debug) << "Face new" << en->getX() + 1 * i << " " << en->getY() + 1;
                TBVectorData::PrintFace(face_new, DM::Debug);
            }
            DM::Logger(DM::Debug) << "------";
            if (DM::CGALGeometry::NodeWithinFace(face_new, n)) { //Valid Point
                if (DM::CGALGeometry::NodeWithinFace(orig, n)) { //if outside the origin face this means there is a hole
                    return false;
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

void urbandevelDivision::createFinalFaces(DM::System *workingsys, DM::System * sys, DM::Face * orig, DM::View v, DM::SpatialNodeHashMap &sphs)
{

    Arrangement_2::Face_const_iterator              fit;
    Segment_list_2									segments;
    Arrangement_2									arr;
    if (tol != 0)
        segments = DM::CGALGeometry_P::Snap_Rounding_2D(workingsys, v,tol);
    else
        segments = DM::CGALGeometry_P::EdgeToSegment2D(workingsys, v);
    insert (arr, segments.begin(), segments.end());

    int faceCounter_orig = 0;

    if (debug) {
        std::vector<DM::Component *> systems = workingsys->getAllComponentsInView(v);
        for (int i = 0; i < systems.size(); i++)
        {

            DM::Face * f = dynamic_cast<DM::Face*>(systems[i]);
            DM::Face * f1 = TBVectorData::CopyFaceGeometryToNewSystem(f, sys);
            sys->addComponentToView(f1, DM::View("faces_debug", DM::FACE, DM::WRITE));
            faceCounter_orig++;
        }
    }
    if (!combined_edges){
        std::vector<DM::Component *> systems = workingsys->getAllComponentsInView(v);
        for (int i = 0; i < systems.size(); i++)
        {
            DM::Face * f = dynamic_cast<DM::Face*>(systems[i]);
            DM::Face * f1 = TBVectorData::CopyFaceGeometryToNewSystem(f, sys);
            sys->addComponentToView(f1,v);
            f1->addAttribute("status", "empty");
        }
        return;
    }

    int faceCounter = 0;
    int removed_faces = 0;
    for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit) {
        if (fit->is_unbounded()) { //if unbound I don't want you
            continue;
        }

        std::vector<DM::Node *> vp = extractCGALFace(fit->outer_ccb(), sphs);
        faceCounter++;

        if (vp.size() < 2)
            continue;

        DM::Face * f = sys->addFace(vp, v);
        f->addAttribute("year", currentyear);
        f->addAttribute("status", "empty");
        f->addAttribute("type",develtype);

        //Extract Holes
        Arrangement_2::Hole_const_iterator hi;
        for (hi = fit->holes_begin(); hi != fit->holes_end(); ++hi) {
            std::vector<DM::Node *> hole = extractCGALFace((*hi), sphs);
            f->addHole(hole);
        }


        if (checkIfHoleFilling(orig, f)) {
            DM::Logger(DM::Debug) << "Remove face";
            sys->removeComponentFromView(f, v);
            sys->removeChild(f);
            //sys->removeFace(f->getUUID());
            removed_faces++;
        }

    }

    DM::Logger(DM::Debug) << "Faces in Arrangment" << arr.number_of_faces();
    DM::Logger(DM::Debug) << "Input Faces " << faceCounter_orig;
    DM::Logger(DM::Debug) << "Created Faces " << faceCounter;
    DM::Logger(DM::Debug) << "Removed Faces " << removed_faces;
}

//Helper
void print_ccb (Arrangement_2::Ccb_halfedge_const_circulator circ)
{
    Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
    std::cout << "(" << curr->source()->point() << ")\n";
    do {
        std::cout << "   [" << curr->curve() << "]   "
                  << "(" << curr->target()->point() << ")\n";
    } while (++curr != circ);
    std::cout << std::endl;
}

void print_face (Arrangement_2::Face_const_handle f)
{
    // Print the outer boundary.
    if (f->is_unbounded())
        std::cout << "Unbounded face. " << std::endl;
    else {
        std::cout << "Outer boundary: ";
        print_ccb (f->outer_ccb());
    }

    // Print the boundary of each of the holes.
    Arrangement_2::Hole_const_iterator hi;
    int                                 index = 1;
    for (hi = f->holes_begin(); hi != f->holes_end(); ++hi, ++index) {
        std::cout << "    Hole #" << index << ": ";
        print_ccb (*hi);
    }

    // Print the isolated vertices.
    Arrangement_2::Isolated_vertex_const_iterator iv;
    for (iv = f->isolated_vertices_begin(), index = 1;
         iv != f->isolated_vertices_end(); ++iv, ++index)
    {
        std::cout << "    Isolated vertex #" << index << ": "
                  << "(" << iv->point() << ")" << std::endl;
    }
}



double urbandevelDivision::getLength() const
{
    return length;
}

void urbandevelDivision::setLength(double value)
{
    length = value;
}

double urbandevelDivision::getOffset() const
{
    return offset;
}

void urbandevelDivision::setOffset(double value)
{
    offset = value;
}

double urbandevelDivision::getAspectRatio() const
{
    return aspectratio;
}

void urbandevelDivision::setAspectRatio(double value)
{
    aspectratio = value;
}

DM::View urbandevelDivision::getInputView() const
{
    return inputView;
}

void urbandevelDivision::setInputView(const DM::View &value)
{
    inputView = value;
}

DM::View urbandevelDivision::getResultView() const
{
    return outputView;
}

void urbandevelDivision::setResultView(const DM::View &value)
{
    outputView = value;
}

bool urbandevelDivision::getCombined_edges() const
{
    return combined_edges;
}

void urbandevelDivision::setCombined_edges(bool value)
{
    combined_edges = value;
}
