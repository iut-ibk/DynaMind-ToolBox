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

void AppendViewFromSystem::AppendSystemElements(DM::System* source, DM::System* target)
{
	foreach(DM::Component* c, source->getAllComponents())
		target->addComponent(new DM::Component(*c));

	std::map<DM::Node*, DM::Node*> nodeConversionMap;
	foreach(DM::Node* n, source->getAllNodes())
		nodeConversionMap[n] = target->addNode(*n);

	foreach(DM::Edge* e, source->getAllEdges())
	{
		DM::Edge* newe = new DM::Edge(*e);
		newe->setStartpoint(nodeConversionMap[e->getStartNode()]);
		newe->setEndpoint(nodeConversionMap[e->getEndNode()]);
		target->addEdge(newe);
	}

	std::map<DM::Face*, DM::Face*> faceConversionMap;
	foreach(DM::Face* f, source->getAllFaces())
	{
		DM::Face* newf = new DM::Face(*f);
		std::vector<DM::Node*> newnodes;
		foreach(DM::Node* n, f->getNodePointers())
			newnodes.push_back(nodeConversionMap[n]);
		newf->setNodes(newnodes);
		faceConversionMap[f] = target->addFace(newf);
	}
	// relink holes
	foreach(DM::Face* f, target->getAllFaces())
	{
		std::vector<DM::Face*> holes = f->getHolePointers();
		f->clearHoles();
		foreach(DM::Face* h, holes)
			f->addHole(faceConversionMap[h]);
	}

	// TODO sub systems copy constructor currently just copies the pointers
	//foreach(DM::System* s, source->getAllSubSystems())
	//	target->addSubSystem(new DM::System(*s));

	foreach(DM::RasterData* r, source->getAllRasterData())
		// TODO it is currently not recommended to copy a whole rasterdata set, 
		// as in general rasterdata sets are maintained by the module
		// this may and will change in future
		// target->addRasterData( new DM::RasterData(*r));
		target->addRasterData(r);
}

void AppendViewFromSystem::run() {
	DM::System * sys_out = this->getData("Combined");
	//Copy all Components from Views
	foreach (std::string d, Inports) 
	{
		DM::System * sys = this->getData(d);
		if (sys == 0)
			continue;

		AppendSystemElements(sys, sys_out);
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

