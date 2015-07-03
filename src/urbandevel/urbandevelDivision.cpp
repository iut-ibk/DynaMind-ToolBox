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
    blockview_name = "SUPERBLOCK";
    elementview_name = "CITYBLOCK";

    aspectratio = 2;
    length = 100;
    offset = 4;
    tol = 0.001; //should not be too small
    onSignal = true;
    sizefromSB = false;
    combined_edges = false;
    splitShortSide = false;
    develtype = "res";
    copyheight = false;

    this->addParameter("Input", DM::STRING, &this->blockview_name);
    this->addParameter("Output", DM::STRING, &this->elementview_name);
    this->addParameter("AspectRatio", DM::DOUBLE, &this->aspectratio);
    this->addParameter("Length", DM::DOUBLE, &this->length);
    this->addParameter("offset", DM::DOUBLE, &this->offset);
    this->addParameter("tolerance", DM::DOUBLE, &this->tol);
    this->addParameter("Develop on signal", DM::BOOL, &this->onSignal);
    this->addParameter("adjust size from SB height", DM::BOOL, &this->sizefromSB);
    this->addParameter("combined_edges", DM::BOOL, &this->combined_edges);
    this->addParameter("splitShortSide", DM::BOOL, &this->splitShortSide);
    this->addParameter("develtype (ignore if empty)", DM::STRING, &this->develtype);
    this->addParameter("copyheight", DM::BOOL, &this->copyheight);
}

urbandevelDivision::~urbandevelDivision()
{

}

void urbandevelDivision::init()
{
    if (blockview_name.empty() || elementview_name.empty())
        return;

    blockview = DM::View(blockview_name, DM::FACE, DM::READ);
    elementview = DM::View(elementview_name, DM::FACE, DM::WRITE);
    elementview_nodes = DM::View(elementview_name+"_NODES", DM::NODE, DM::WRITE);
    bbs = DM::View("BBS", DM::FACE, DM::WRITE);

    elementview.addAttribute("status", DM::Attribute::STRING, DM::WRITE);
    elementview.addAttribute("generation", DM::Attribute::DOUBLE, DM::WRITE);
    elementview.addAttribute("year", DM::Attribute::DOUBLE, DM::WRITE);
    elementview.addAttribute("area", DM::Attribute::DOUBLE, DM::WRITE);
    elementview.addAttribute("type", DM::Attribute::STRING, DM::WRITE);
    elementview.addAttribute(blockview_name, elementview_name, DM::WRITE);

    elementview_nodes.addAttribute("street_side", DM::Attribute::DOUBLE, DM::WRITE);

    if(onSignal)
    {
        cityview = DM::View("CITY", DM::NODE, DM::READ);
        cityview.addAttribute("currentyear", DM::Attribute::DOUBLE, DM::READ);
        cityview.addAttribute("startyear", DM::Attribute::DOUBLE, DM::READ);

        blockview.addAttribute("status", DM::Attribute::STRING, DM::READ);
        blockview.addAttribute("type", DM::Attribute::STRING, DM::READ);
        blockview.addAttribute("noheight", DM::Attribute::DOUBLE, DM::WRITE);
        elementview.addAttribute("year", DM::Attribute::STRING, DM::WRITE);
    }

    if (sizefromSB) {
        elementview.addAttribute(blockview_name, elementview_name, DM::MODIFY);
        elementview.addAttribute("height_avg", DM::Attribute::DOUBLE, DM::WRITE);
        sb = DM::View("SUPERBLOCK", DM::FACE, DM::READ);
        sb.addAttribute("height_avg", DM::Attribute::DOUBLE, DM::READ);
    }

    if (copyheight) {
        elementview.addAttribute("height_avg", DM::Attribute::DOUBLE, DM::WRITE);
        elementview.addAttribute("height_min", DM::Attribute::DOUBLE, DM::WRITE);
        elementview.addAttribute("height_max", DM::Attribute::DOUBLE, DM::WRITE);
        blockview.addAttribute("height_avg", DM::Attribute::DOUBLE, DM::READ);
        blockview.addAttribute("height_min", DM::Attribute::DOUBLE, DM::READ);
        blockview.addAttribute("height_max", DM::Attribute::DOUBLE, DM::READ);
    }

    std::vector<DM::View> data;

    if (onSignal)
        data.push_back(cityview);
    data.push_back(blockview);
    data.push_back(elementview);
    data.push_back(elementview_nodes);
    if (sizefromSB)
        data.push_back(sb);

    this->addData("data", data);
}

/** The method is based on the minial bounding box */
void urbandevelDivision::run()
{
    debug = false;

    DM::System * sys = this->getData("data");
    DM::SpatialNodeHashMap snhm(sys,100,false);

    int startyear = 0;

    if (onSignal)
    {

        std::vector<DM::Component *> cities = sys->getAllComponentsInView(cityview);

        if (cities.size() != 1)
        {
            DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
            return;
        }

        DM::Component * currentcityview = cities[0];

        currentyear = static_cast<int>(currentcityview->getAttribute("currentyear")->getDouble());
        startyear = static_cast<int>(currentcityview->getAttribute("startyear")->getDouble());
    }

    if ( currentyear <= startyear+1 )
    {
        std::vector<DM::Component *> elements = sys->getAllComponentsInView(elementview);
        for (int i = 0; i < elements.size(); i++)
        {
            std::string eyear = elements[i]->getAttribute("year")->getString();
            if ( eyear.empty() )
            {
                QString year = QString::number(startyear) + QString("-1-1");
                std::string stdyear = year.toUtf8().constData();
                elements[i]->changeAttribute("year", stdyear);
            }
        }
    }

    std::vector<DM::Component *> blocks = sys->getAllComponentsInView(blockview);

    for (int i = 0; i < blocks.size(); i++)
    {
        DM::System workingSys;
        DM::Face * block = static_cast<DM::Face *> (blocks[i]);

        height_avg = 0;
        worklength = length;

        // if development should happen on signal and the status is not "develop" skip the block

        std::string inputtype = "notonsignal";
        std::string inputstatus = "notonsignal";

        if ( onSignal )
        {
            inputtype = block->getAttribute("type")->getString();
            inputstatus = block->getAttribute("status")->getString();

            if ( inputstatus != "process" )
            {
                DM::Logger(DM::Debug) << "not parceling as status = " << inputstatus;
                continue;
            }


        }

        DM::Logger(DM::Warning) << "block past signal check, status = " << inputstatus;
        // if no develtype is set and does not match skip the block

        if ( !develtype.empty() && develtype.compare(inputtype) != 0 )
        {
            DM::Logger(DM::Debug) << "not parceling as type = " << inputtype;
            continue;
        }

        DM::Logger(DM::Warning) << "block past type check";

        // if sizefromSB is true modify the length variable according to the height value

        if (sizefromSB) {

            std::vector<DM::Component*> sblink = block->getAttribute("SUPERBLOCK")->getLinkedComponents();

            block->addAttribute("noheight", 0);

            if (sblink.size() != 1)
            {
                DM::Logger(DM::Warning) << "Superblocks mismatch (exactly one component needed). There are " << sblink.size();
                height_avg = 1;
                block->addAttribute("noheight",2);
            }
            else {
                height_avg = static_cast<int>(sblink[0]->getAttribute("height_avg")->getDouble());
                block->addAttribute("noheight",1);
            }

            if (height_avg >=3) {
                worklength = worklength*1.5;
            }
            else if (height_avg >=6) {
                worklength = worklength*3;
            }

            DM::Logger(DM::Warning) << "Adjusting length " << height_avg << " from " << length << " to " << worklength;
        }

        DM::Logger(DM::Warning) << "begin parceling";

        DM::Face * newblock = TBVectorData::CopyFaceGeometryToNewSystem(block, &workingSys);
        workingSys.addComponentToView(newblock, blockview);
        this->createSubdivision(&workingSys, newblock, 0, inputtype);
        createFinalFaces(&workingSys, sys, newblock, elementview, snhm);

        DM::Logger(DM::Debug) << "end parceling";

        std::vector<DM::Component *> elements = sys->getAllComponentsInView(elementview);

        for (int j = 0; j < elements.size(); j++)
        {
            //DM::Logger(DM::Warning) << "status = " << status;

            DM::Face * element = static_cast<DM::Face *> (elements[j]);
            std::string status = element->getAttribute("status")->getString();


            if (status == "new")
            {
                double area = DM::CGALGeometry::CalculateArea2D(element);
                //element->addAttribute("centroid_x", a.getX());
                //element->addAttribute("centroid_y", a.getY());
                element->addAttribute("area", area);
                element->addAttribute("year", currentyear);
                element->addAttribute("status", "empty");
                element->addAttribute("type",develtype);
                if (copyheight) {
                    int height_avg = static_cast<int>(block->getAttribute("height_avg")->getDouble());
                    int height_min = static_cast<int>(block->getAttribute("height_min")->getDouble());
                    int height_max = static_cast<int>(block->getAttribute("height_max")->getDouble());
                    element->changeAttribute("height_avg", height_avg);
                    element->changeAttribute("height_min", height_min);
                    element->changeAttribute("height_max", height_max);
                }
                if (onSignal)
                {
                    QString year = QString::number(currentyear) + QString("-1-1");
                    std::string stdyear = year.toUtf8().constData();
                    element->changeAttribute("year", stdyear);
                }

                block->getAttribute(elementview.getName())->addLink(element, elementview.getName()); //Link SB->CB
                element->getAttribute(blockview.getName())->addLink(block, blockview.getName()); //Link CB->SB
            }

        }
        block->addAttribute("status", "developed");
    }
}

string urbandevelDivision::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/tree/master/doc/output/html/urbandevelDivision.html";
}

void urbandevelDivision::createSubdivision(DM::System * sys,  DM::Face *block, int gen, std::string type)
{
    bool split_length = false;
    std::vector<DM::Node> box;
    std::vector<double> size;

    double alpha = DM::CGALGeometry::CalculateMinBoundingBox(TBVectorData::getNodeListFromFace(sys, block), box, size);


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

    if (2*worklength > split_l) {
        if ( (worklength / aspectratio) * 2 >   split_width) { //width
            sys->addComponentToView(block, this->elementview);
            return;
        }
    }

    if (this->splitShortSide) {
         split_l = size[1];
         split_width = size[0];
    }
    if (2*worklength > split_l)
        split_length = true;
    int elements = 2;
    if (split_length) {
        if(!this->splitShortSide)
            elements = size[1] / (worklength / aspectratio);
        else
            elements = size[0] / (worklength / this->aspectratio);
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

        std::vector<DM::Face *> intersected_faces = DM::CGALGeometry::IntersectFace(sys, block, bb);

        if (intersected_faces.size() == 0) {
            DM::Logger(DM::Debug) << "Advanced parceling createSubdivision intersction failed";
            continue;
        }

        foreach (DM::Face * f_new ,intersected_faces ) {
            this->createSubdivision(sys, f_new, gen+1, type);
        }
    }
}

std::vector<DM::Node *> urbandevelDivision::extractCGALFace(Arrangement_2::Ccb_halfedge_const_circulator hec, DM::SpatialNodeHashMap & snhm)
{
    std::vector<DM::Node *> vp;
    Arrangement_2::Ccb_halfedge_const_circulator curr = hec;
    do{
        double x = CGAL::to_double(curr->source()->point().x());
        double y = CGAL::to_double(curr->source()->point().y());
        DM::Node * n = snhm.addNode(x,y,0,tol, this->elementview_nodes);
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

    std::vector<DM::Component *> systems = workingsys->getAllComponentsInView(v);

    if (!combined_edges){
        std::vector<DM::Component *> systems = workingsys->getAllComponentsInView(v);
        for (int i = 0; i < systems.size(); i++)
        {
            DM::Face * f = dynamic_cast<DM::Face*>(systems[i]);
            DM::Face * f1 = TBVectorData::CopyFaceGeometryToNewSystem(f, sys);
            sys->addComponentToView(f1,v);
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
        f->addAttribute("status", "new");

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

/*

double urbandevelDivision::getLength() const
{
    return worklength;
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
    return blockview;
}

void urbandevelDivision::setInputView(const DM::View &value)
{
    blockview = value;
}

DM::View urbandevelDivision::getResultView() const
{
    return elementview;
}

void urbandevelDivision::setResultView(const DM::View &value)
{
    elementview = value;
}

bool urbandevelDivision::getCombined_edges() const
{
    return combined_edges;
}

void urbandevelDivision::setCombined_edges(bool value)
{
    combined_edges = value;
}

*/
