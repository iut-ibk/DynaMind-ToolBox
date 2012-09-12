#ifndef GUIVIEWDATAFORMODULES_H
#define GUIVIEWDATAFORMODULES_H
#include <dmcompilersettings.h>
#include <QDialog>

namespace Ui {
class GUIViewDataForModules;
}

namespace DM {
    class Module;
}
class DM_HELPER_DLL_EXPORT GUIViewDataForModules : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIViewDataForModules( DM::Module * m, QWidget *parent = 0);
    ~GUIViewDataForModules();
    
private:
    Ui::GUIViewDataForModules *ui;
    DM::Module * m;
};

#endif // GUIVIEWDATAFORMODULES_H
