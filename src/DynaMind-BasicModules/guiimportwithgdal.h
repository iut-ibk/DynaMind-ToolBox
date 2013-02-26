#ifndef GUIIMPORTWITHGDAL_H
#define GUIIMPORTWITHGDAL_H

#include <dmcompilersettings.h>
#include <QDialog>

namespace DM {
class Module;
}
namespace Ui {
class GUIImportWithGDAL;
}

class ImportwithGDAL;

class DM_HELPER_DLL_EXPORT GUIImportWithGDAL : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIImportWithGDAL(DM::Module * m, QWidget *parent = 0);
    ~GUIImportWithGDAL();
    
private:
    Ui::GUIImportWithGDAL *ui;
    ImportwithGDAL * m;


public slots:
    void accept();
    void on_pushButton_wfs_pick_clicked();
    void on_pushButton_Filename_clicked();
};

#endif // GUIIMPORTWITHGDAL_H
