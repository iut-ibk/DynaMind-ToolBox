#ifndef GUIAPPENDATTRIBUTES_H
#define GUIAPPENDATTRIBUTES_H

#include <QDialog>
#include <dmcompilersettings.h>
namespace Ui {
class GUIAppendAttributes;
}

namespace DM{
class Module;
}

class AppendAttributes;
class DM_HELPER_DLL_EXPORT GUIAppendAttributes : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIAppendAttributes(DM::Module * m, QWidget *parent = 0);
    ~GUIAppendAttributes();
    
private:
    Ui::GUIAppendAttributes *ui;
    AppendAttributes * m;

public slots:
    void accept();
};

#endif // GUIAPPENDATTRIBUTES_H
