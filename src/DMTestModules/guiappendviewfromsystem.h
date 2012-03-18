#ifndef GUIAPPENDVIEWFROMSYSTEM_H
#define GUIAPPENDVIEWFROMSYSTEM_H

#include <QDialog>

namespace Ui {
class GUIAppendViewFromSystem;
}

namespace DM {
    class Module;
}
class AppendViewFromSystem;
class GUIAppendViewFromSystem : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIAppendViewFromSystem(DM::Module * m, QWidget *parent = 0);
    ~GUIAppendViewFromSystem();
    
private:
    Ui::GUIAppendViewFromSystem *ui;
    AppendViewFromSystem * m;

public slots:
    void addSystem();

};

#endif // GUIAPPENDVIEWFROMSYSTEM_H
