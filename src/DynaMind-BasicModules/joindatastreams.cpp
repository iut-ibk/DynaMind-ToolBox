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
    if (Inports.size() > 0 ) 
	{
        foreach (std::string s, Inports) 
		{
			mforeach(DM::View v, getViewsInStream()[s])
			{
				DM::View new_v(v.getName(), v.getType(), DM::WRITE);
				
				foreach(std::string attName, v.getAllAttributes())
				{
					if(v.getAttributeType(attName) == DM::Attribute::LINK)
						new_v.addLinks(attName, v.getNameOfLinkedView(attName));
					else
						new_v.addAttribute(attName);
				}

				combined_datastream.push_back(new_v);
			}

			/*
            DM::System * sys = this->getData(s);
            if (sys != 0) {
                foreach (std::string v, sys->getNamesOfViews()) {
                    if (std::find(existingViews.begin(), existingViews.end(), v) == existingViews.end()) {
                        DM::View * old = sys->getViewDefinition(v);
                        DM::View new_v(v, old->getType(), DM::WRITE);
                        if (old->getIdOfDummyComponent().empty())
                            continue;
                        if (!sys->getComponent(old->getIdOfDummyComponent())) {
                            DM::Logger(DM::Error) << "Standard dummy Component 0";
                            sys = 0;
                            return;
                        }
                        DM::AttributeMap cmp = sys->getComponent(old->getIdOfDummyComponent())->getAllAttributes();

                        for (DM::AttributeMap::const_iterator it = cmp.begin();
                             it != cmp.end();
                             ++it) {
                            if (sys->getComponent(old->getIdOfDummyComponent())->getAttribute(it->first)->getType() == DM::Attribute::LINK)  {
                                DM::LinkAttribute l_attr = sys->getComponent(old->getIdOfDummyComponent())->getAttribute(it->first)->getLink();
                                new_v.addLinks(it->first, DM::View(l_attr.viewname, DM::READ, DM::COMPONENT));
                            } else {
                                new_v.addAttribute(it->first);
                            }
                        }
                        combined_datastream.push_back(new_v);
                        //existingViews.push_back(v);
                    }
                }
            }*/
        }

    }

    foreach (std::string s, Inports) {
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
