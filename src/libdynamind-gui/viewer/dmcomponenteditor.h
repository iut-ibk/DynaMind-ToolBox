#ifndef DM_DMCOMPONENTEDITOR_H
#define DM_DMCOMPONENTEDITOR_H

#include <QDialog>

namespace DM {

class Component;

namespace Ui {
class ComponentEditor;
}

class ComponentEditor : public QDialog {
    Q_OBJECT
    
public:
    explicit ComponentEditor(Component *c, QWidget *parent = 0);
    ~ComponentEditor();
    
private:
    Ui::ComponentEditor *ui;
    Component *c;
};


} // namespace DM
#endif // DM_DMCOMPONENTEDITOR_H
