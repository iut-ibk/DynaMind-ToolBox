#include "appendviewfromsystem.h"
#include "guiappendviewfromsystem.h"

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
        DM::NodeMap nm = sys->getAllNodes();
        for (DM::NodeMap::const_iterator it = nm.begin(); it != nm.end(); ++it ){
            sys_out->addNode(new DM::Node(*(it->second)));
        }
        DM::EdgeMap em = sys->getAllEdges();
        for (DM::EdgeMap::const_iterator it = em.begin(); it != em.end(); ++it ){
            sys_out->addEdge(new DM::Edge(*(it->second)));
        }
        DM::FaceMap fm = sys->getAllFaces();
        for (DM::FaceMap::const_iterator it = fm.begin(); it != fm.end(); ++it ){
            sys_out->addFace(new DM::Face(*(it->second)));
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

//TODO: Works finw until someone is changing something upstream -> no update downstream!
void AppendViewFromSystem::init()
{
    //Define New System
    if (Inports.size() > 0 ) {
        bool changed = false;
        foreach (std::string s, Inports) {
            DM::System * sys = this->getData(s);
            if (sys != 0) {
                foreach (std::string v, sys->getViews()) {
                    if (find(existingViews.begin(), existingViews.end(), v) == existingViews.end()) {
                        DM::View old = sys->getViewDefinition(v);
                        DM::View new_v(v, old.getType(), DM::WRITE);
                        if (old.getIdOfDummyComponent().empty())
                            continue;
                        DM::AttributeMap cmp = sys->getComponent(old.getIdOfDummyComponent())->getAllAttributes();

                        for (DM::AttributeMap::const_iterator it = cmp.begin();
                             it != cmp.end();
                             ++it) {
                            new_v.addAttribute(it->first);
                        }
                        views.push_back(new_v);
                        existingViews.push_back(v);
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
