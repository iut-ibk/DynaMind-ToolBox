#include "appendviewfromsystem.h"
#include "guiappendviewfromsystem.h"
#include <algorithm>

DM_DECLARE_NODE_NAME(AppendViewFromSystem, Modules)

AppendViewFromSystem::AppendViewFromSystem()
{
	sizeold = 0;
	DM::View dummy("dummy",DM::SUBSYSTEM, DM::WRITE);
	views.push_back(dummy);
	this->addParameter("Inports", DM::STRING_LIST, & this->Inports);
	this->addData("Combined", views);

}


void AppendViewFromSystem::run() {
	DM::System * sys_out = this->getData("Combined");
	//Copy all Components from Views
	foreach (std::string d, Inports) {
		DM::System * sys = this->getData(d);
		if (sys == 0)
			continue;
		DM::ComponentMap cm = sys->getAllComponents();
		for (DM::ComponentMap::const_iterator it = cm.begin(); it != cm.end(); ++it ){
			sys_out->addComponent(new DM::Component(*(it->second)));
		}
		DM::NodeMap nm = sys->getAllNodes();
		for (DM::NodeMap::const_iterator it = nm.begin(); it != nm.end(); ++it ){
			sys_out->addNode(new DM::Node(*(it->second)));
		}
		DM::EdgeMap em = sys->getAllEdges();
		for (DM::EdgeMap::const_iterator it = em.begin(); it != em.end(); ++it )
		{
			DM::Edge* e = new DM::Edge(*(it->second));
			e->setStartpoint(sys_out->getNode(e->getStartpointName()));
			e->setEndpoint(sys_out->getNode(e->getEndpointName()));
			sys_out->addEdge(e);
		}
		DM::FaceMap fm = sys->getAllFaces();
		for (DM::FaceMap::const_iterator it = fm.begin(); it != fm.end(); ++it )
		{
			// relink nodes
			std::vector<DM::Node*> nodes;
			DM::Face* f = new DM::Face(*(it->second));
			foreach(DM::Node* n, f->getNodePointers())
				nodes.push_back(sys_out->getNode(n->getUUID()));

			f->setNodes(nodes);
			sys_out->addFace(f);
		}
		// get new systems faces
		fm = sys_out->getAllFaces();
		for (DM::FaceMap::const_iterator it = fm.begin(); it != fm.end(); ++it )
		{
			// relink holes

			std::vector<DM::Face*>holes;
			foreach(DM::Face* f, it->second->getHolePointers())
				holes.push_back(sys_out->getFace(f->getUUID()));

			it->second->clearHoles();
			foreach(DM::Face* f, holes)
				it->second->addHole(f);
		}

		DM::SystemMap sm = sys->getAllSubSystems();
		for (DM::SystemMap::const_iterator it = sm.begin(); it != sm.end(); ++it ){
			sys_out->addSubSystem(new DM::System(*(it->second)));
		}
		DM::RasterDataMap rm = sys->getAllRasterData();
		for (DM::RasterDataMap::const_iterator it = rm.begin(); it != rm.end(); ++it ){
			sys_out->addRasterData(new DM::RasterData(*(it->second)));
		}
	}

}

//TODO: Works fine until someone is changing something upstream -> no update downstream!
void AppendViewFromSystem::init()
{
	//Define New System
	if (Inports.size() > 0 )
	{
		bool changed = false;
		foreach (std::string s, Inports)
		{
			//DM::System * sys = this->getData(s);
			//if (sys != 0)
			{
				//foreach (std::string v, sys->getNamesOfViews())
				foreach(DM::View v, this->getViewsInStream(s))
				{
					if (std::find(existingViews.begin(), existingViews.end(), v.getName()) == existingViews.end())
					{
						//DM::View * old = sys->getViewDefinition(v);
						DM::View new_v(v.getName(), v.getType(), DM::WRITE);
						/*if (old->getIdOfDummyComponent().empty())
							continue;
						if (!sys->getComponent(old->getIdOfDummyComponent())) {
							DM::Logger(DM::Error) << "Standard dummy Component 0";
							sys = 0;
							return;
						}
						DM::AttributeMap cmp = sys->getComponent(old->getIdOfDummyComponent())->getAllAttributes();
						*/
						foreach(std::string attName, v.getAllAttributes())
						{
							if(v.getAttributeType(attName) == DM::Attribute::LINK)
								new_v.addLinks(attName, v.getNameOfLinkedView(attName));
							else
								new_v.addAttribute(attName);
						}
						/*
						for (DM::AttributeMap::const_iterator it = cmp.begin();
							 it != cmp.end();
							 ++it) {
							if (sys->getComponent(old->getIdOfDummyComponent())->getAttribute(it->first)->getType() == DM::Attribute::LINK)  {
								DM::LinkAttribute l_attr = sys->getComponent(old->getIdOfDummyComponent())->getAttribute(it->first)->getLink();
								new_v.addLinks(it->first, DM::View(l_attr.viewname, DM::READ, DM::COMPONENT));
							} else {
							new_v.addAttribute(it->first);
							}
						}*/
						views.push_back(new_v);
						existingViews.push_back(v.getName());
						changed = true;
					}
				}
			}
		}
		if (changed)
			this->addData("Combined", views);
	}

	if (sizeold == Inports.size() )
		return;
	foreach (std::string s, Inports) {
		DM::View dummy("dummy",DM::SUBSYSTEM, DM::READ) ;
		std::vector<DM::View> data;
		data.push_back(dummy);
		this->addData(s, data);
	}
	sizeold = Inports.size();


}
bool AppendViewFromSystem::createInputDialog() {
	QWidget * w = new GUIAppendViewFromSystem(this);
	w->show();
	return true;
}

void AppendViewFromSystem::addSystem(std::string sys) {
	Inports.push_back(sys);
	this->init();

}

string AppendViewFromSystem::getHelpUrl(){
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/AppendViewFromSystem.md";
}

