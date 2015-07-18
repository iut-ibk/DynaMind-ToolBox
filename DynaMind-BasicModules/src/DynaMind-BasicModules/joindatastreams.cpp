#include "joindatastreams.h"
#include "guijoindatastreams.h"
#include <algorithm>
#include <appendviewfromsystem.h>

DM_DECLARE_NODE_NAME(JoinDatastreams, Modules)

JoinDatastreams::JoinDatastreams()
{
    sizeold = 0;
    DM::View dummy("dummy",DM::SUBSYSTEM, DM::MODIFY);
    views.push_back(dummy);
    this->addParameter("Inports", DM::STRING_LIST, & this->Inports);
    this->addData("Combined", views);
}


//TODO: Works fine until someone is changing something upstream -> no update downstream!
void JoinDatastreams::init()
{
    std::vector<DM::View> combined_datastream;
    combined_datastream.push_back(DM::View("dummy",DM::SUBSYSTEM, DM::MODIFY));
    this->existingViews.clear();
    //Define New System
	foreach (std::string s, Inports) 
		foreach(const DM::View& v, getViewsInStream(s))
			combined_datastream.push_back(v.clone(DM::WRITE));

    foreach (std::string s, Inports) 
	{
        DM::View dummy("dummy",DM::SUBSYSTEM, DM::READ) ;
        std::vector<DM::View> data;
        data.push_back(dummy);
        this->addData(s, data);
    }

    this->addData("Combined", combined_datastream);
}

void JoinDatastreams::run() 
{
    DM::System * sys_out = this->getData("Combined");
    //Copy all Components from Views
    foreach (std::string d, Inports) 
	{
        DM::System * sys = this->getData(d);
        if (sys == 0)
            continue;

		AppendViewFromSystem::AppendSystemElements(sys, sys_out);
    }
}

void JoinDatastreams::addSystem(std::string sys) {
    Inports.push_back(sys);
    this->init();
}

string JoinDatastreams::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/JoinDatastreams.md";
}

bool JoinDatastreams::createInputDialog() {
    QWidget * w = new GUIJoinDatastreams(this);
    w->show();
    return true;
}
