#ifndef CLEARVIEW_H
#define CLEARVIEW_H

#include <dmmodule.h>
#include <dm.h>
class DM_HELPER_DLL_EXPORT ClearView : public DM::Module
{
    DM_DECLARE_NODE(ClearView)
    private:
        //DM::System * sys_in;
    std::string NameOfExistingView;
    std::string NameOfExistingView_old;
public:
    ClearView();
    void run();
    void init();

    bool createInputDialog();
   // DM::System * getSystemIn();
    std::string getHelpUrl();

};

#endif // CLEARVIEW_H
