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
    //TODO: Action
}

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
    sizeold == Inports.size();


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
