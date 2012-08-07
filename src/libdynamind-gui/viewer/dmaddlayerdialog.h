#ifndef ADDLAYERDIALOG_H
#define ADDLAYERDIALOG_H

#include <QDialog>

namespace Ui {
class AddLayerDialog;
}

class QTreeWidgetItem;

namespace DM {
    class System;
    class Layer;
    class View;
    class Viewer;



class AddLayerDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit AddLayerDialog(DM::System *system, QWidget *parent = 0);
    ~AddLayerDialog();
    DM::Layer *getLayer(DM::Viewer *v);
    
private slots:
    void on_color_start_button_clicked();
    void on_color_stop_button_clicked();
    void on_viewList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_attributeList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    
private:
    Ui::AddLayerDialog *ui;
    QColor start, stop;
    DM::System *system;
    
    DM::View *view;
    std::string attribute;
};

}

#endif // ADDLAYERDIALOG_H
