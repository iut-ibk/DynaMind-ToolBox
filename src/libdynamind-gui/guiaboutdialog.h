#ifndef GUIABOUTDIALOG_H
#define GUIABOUTDIALOG_H
#include <dmcompilersettings.h>
#include <QDialog>

class GUISimulation;
namespace Ui {
class GUIAboutDialog;
}

class DM_HELPER_DLL_EXPORT GUIAboutDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIAboutDialog(GUISimulation *, QWidget *parent = 0);
    ~GUIAboutDialog();
    
private:
    Ui::GUIAboutDialog *ui;



};

#endif // GUIABOUTDIALOG_H
