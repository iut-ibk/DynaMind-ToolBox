#include "createinlets.h"
DM_DECLARE_NODE_NAME(CreateInlets, Sewer)

CreateInlets::CreateInlets()
{


    Blocks = DM::View("CITYBLOCK", DM::FACE, DM::READ);
    Inlets = DM::View("INLET", DM::NODE, DM::WRITE);

    Inlets.addAttribute("ID_CATCHMENT");

    std::vector<DM::View> data;
    data.push_back(Blocks);
    data.push_back(Inlets);
    this->addParameter("Size", DM::DOUBLE, &Size);

    offsetX = 0;
    offsetY = 0;
    with = 1000;
    heigth = 1000;

    this->addParameter("offsetX", DM::DOUBLE, &offsetX);
    this->addParameter("offsetY", DM::DOUBLE, &offsetY);
    this->addParameter("with", DM::DOUBLE, &with);
    this->addParameter("heigth", DM::DOUBLE, &heigth);


    this->addData("City", data);

}

void CreateInlets::run() {

    DM::System * city = this->getData("City");

    std::vector<std::string> blocks = city->getNamesOfComponentsInView(Blocks);
    foreach (std::string nBlock, blocks) {
        DM::Face * f = city->getFace(nBlock);

        double minx;
        double maxx;
        double miny;
        double maxy;

        std::vector<std::string> points = f->getNodes();
        for (int i = 0; i < points.size(); i++) {
            DM::Node * n = city->getNode(points[i]);
            if (i == 0) {
                minx = n->getX();
                maxx = n->getX();
                miny = n->getY();
                maxy = n->getY();
                continue;
            }
            if (minx > n->getX())
                minx = n->getX();
            if (miny > n->getY())
                miny = n->getY();
            if (maxx < n->getX())
                maxx = n->getX();
            if (maxy < n->getY())
                maxy = n->getY();

        }

        double l = maxx-minx;
        double h = maxy-miny;

        double startX = minx + Size/2;
        double startY = miny + Size/2;


        double buildyear =f->getAttribute("BuildYear")->getDouble();
        if (buildyear < 1900)
            continue;
        while (startY < maxy) {
            while (startX < maxx) {
                if (startX < offsetX+with && startX > offsetX && startY < offsetY+heigth && startY > offsetY)  {
                    DM::Node * n = city->addNode(DM::Node(startX, startY, 0), Inlets);
                    n->addAttribute("BuildYear", buildyear);
                    n->getAttribute("ID_CATCHMENT")->setString(f->getName());

                }
                startX+=Size;
            }
            startY+=Size;
            startX = minx + Size/2;
        }




    }


}
