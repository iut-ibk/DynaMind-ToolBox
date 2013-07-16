#include "clearview.h"
#include <guiclearview.h>

DM_DECLARE_NODE_NAME(ClearView, Modules)
ClearView::ClearView()
{
    sys_in = 0;
    this->NameOfExistingView = "";
    this->NameOfExistingView_old = "";
    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addParameter("NameOfExistingView", DM::STRING, &this->NameOfExistingView);
    this->addData("Data", data);
}

void ClearView::run() {
    DM::Logger(DM::Warning) << "ClearView is depricated please use RemoveComponent";
    DM::System * data = this->getData("Data");
    DM::View v = DM::View (NameOfExistingView, sys_in->getViewDefinition(NameOfExistingView)->getType(), DM::MODIFY);
    DM::ComponentMap cmp = data->getAllComponentsInView(v);
    for (DM::ComponentMap::const_iterator it = cmp.begin(); it != cmp.end(); ++it) {
        data->removeComponentFromView(it->second, v);
    }

}

void ClearView::init() 
{
    //sys_in = this->getData("Data");
    //if (sys_in == 0)
    //    return;
    if (NameOfExistingView.empty())
        return;
    if (NameOfExistingView.compare(NameOfExistingView_old) == 0)
        return;

    std::vector<DM::View> data;
    data.push_back(  DM::View (NameOfExistingView, getViewInStream("Data", NameOfExistingView).getType(), DM::MODIFY) );
    this->addData("Data", data);
    NameOfExistingView_old = NameOfExistingView;

}

bool ClearView::createInputDialog() {
    QWidget * w = new GUIClearView(this);
    w->show();
    return true;
}

DM::System * ClearView::getSystemIn() {
    return this->sys_in;
}

string ClearView::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-ToolBox/wiki/Clearview";
}
