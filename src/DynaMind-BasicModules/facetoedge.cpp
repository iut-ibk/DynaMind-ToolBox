#include "facetoedge.h"
#include "tbvectordata.h"
#include <QMap>
DM_DECLARE_NODE_NAME(FaceToEdge, Modules)
FaceToEdge::FaceToEdge()
{
	this->edge_name = "";
	this->face_name = "";
	this->linkToFaces = false;
	this->sharedEdges = false;


	this->addParameter("Face", DM::STRING, &face_name);
	this->addParameter("Edge", DM::STRING, &edge_name);
	this->addParameter("LinkToFaces", DM::BOOL, &linkToFaces);
	this->addParameter("SharedEdges", DM::BOOL, &sharedEdges);
}

void FaceToEdge::init()
{
	if (edge_name.empty())
		return;
	if (face_name.empty())
		return;
	this->view_edge = DM::View(edge_name, DM::EDGE, DM::WRITE);
	if (linkToFaces)  this->view_edge.addLinks(face_name, face_name);
	this->view_face = DM::View(face_name, DM::FACE, DM::READ);

	std::vector<DM::View> datastream;

	datastream.push_back(view_edge);
	datastream.push_back(view_face);

	this->addData("sys", datastream);

}

void FaceToEdge::run()
{
	typedef std::pair<DM::Node*, DM::Node*>  nodepair;
	std::map<nodepair,  DM::Edge*> StartNodeEdgeList;
	std::set<DM::Node*> node_tot;
	DM::System * sys = this->getData("sys");

	int edgeCounter = 0;
	foreach(DM::Component* c, sys->getAllComponentsInView(view_face))
	{
		DM::Face* f = (DM::Face*)c;

		std::vector<DM::Node*> nodes = f->getNodePointers();
		nodes.push_back(nodes[0]);
		int number_of_nodes = nodes.size();

		for (int j = 1; j < number_of_nodes; j++){
			if (!sharedEdges) {
				DM::Edge * e = sys->addEdge(nodes[j-1], nodes[j], view_edge);
				if (linkToFaces) e->getAttribute(face_name)->addLink(e, face_name);
				continue;
			}

			DM::Edge * e = NULL;
			if (StartNodeEdgeList.find(nodepair(nodes[j-1], nodes[j])) != StartNodeEdgeList.end()) {
				e =  StartNodeEdgeList[nodepair(nodes[j-1], nodes[j])];
			}
			if (StartNodeEdgeList.find(nodepair(nodes[j], nodes[j-1])) != StartNodeEdgeList.end()) {
				e =  StartNodeEdgeList[nodepair(nodes[j], nodes[j-1])];
			}
			if (!e) {
				e = sys->addEdge(nodes[j-1], nodes[j], view_edge);
				StartNodeEdgeList[nodepair(nodes[j-1], nodes[j])] = e;
				node_tot.insert(nodes[j]);
				node_tot.insert(nodes[j-1]);

				edgeCounter++;
			}
			if (linkToFaces) {
				e->getAttribute(face_name)->addLink(f, face_name);
				e->addAttribute("shared_by", e->getAttribute("shared_by")->getDouble() +1);

			}
		}
	}
	DM::Logger(DM::Standard) << "Created Edges " << edgeCounter;
	DM::Logger(DM::Standard) << "Nodes used " << node_tot.size();
}

string FaceToEdge::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/FaceToEdge.md";
}
