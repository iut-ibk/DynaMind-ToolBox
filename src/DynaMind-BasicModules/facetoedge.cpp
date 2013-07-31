#include "facetoedge.h"
#include "tbvectordata.h"

DM_DECLARE_NODE_NAME(FaceToEdge, Modules)
FaceToEdge::FaceToEdge()
{
    this->edge_name = "";
    this->face_name = "";

    this->addParameter("Face", DM::STRING, &face_name);
    this->addParameter("Edge", DM::STRING, &edge_name);
}

void FaceToEdge::init()
{
    if (edge_name.empty())
        return;
    if (face_name.empty())
        return;
    this->view_edge = DM::View(edge_name, DM::EDGE, DM::WRITE);
    this->view_face = DM::View(face_name, DM::FACE, DM::READ);

    std::vector<DM::View> datastream;

    datastream.push_back(view_edge);
    datastream.push_back(view_face);

    this->addData("sys", datastream);

}

void FaceToEdge::run()
{
    DM::System * sys = this->getData("sys");

    std::vector<std::string> face_uuids = sys->getUUIDs(view_face);

    int number_of_faces = face_uuids.size();

    for (int i = 0; i < number_of_faces; i++) {
        DM::Face * f = sys->getFace(face_uuids[i]);

        std::vector<DM::Node*> nodes = TBVectorData::getNodeListFromFace(sys, f);

        int number_of_nodes = nodes.size();

        for (int j = 1; j < number_of_nodes; j++)
            sys->addEdge(nodes[j-1], nodes[j], view_edge);
    }
}

string FaceToEdge::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/FaceToEdge.md";
}
