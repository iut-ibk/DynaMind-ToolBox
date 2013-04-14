#ifndef GUIJOINDATASTREAMS_H
#define GUIJOINDATASTREAMS_H

#include <QDialog>
#include <joindatastreams.h>

namespace Ui {
class GUIJoinDatastreams;
}

class GUIJoinDatastreams : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIJoinDatastreams(DM::Module *m, QWidget *parent = 0);
    ~GUIJoinDatastreams();
    
private:
    Ui::GUIJoinDatastreams *ui;
    JoinDatastreams * m;

public slots:
    void addSystem();

};

#endif // GUIJOINDATASTREAMS_H
