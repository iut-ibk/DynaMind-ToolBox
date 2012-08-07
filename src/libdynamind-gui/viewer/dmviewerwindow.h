#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QWidget>

namespace Ui {
class ViewerWindow;
}
namespace DM {

class System;

class ViewerWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit ViewerWindow(System *system, QWidget *parent = 0);
    ~ViewerWindow();
    
private slots:
    void on_actionAdd_Layer_triggered();
    void offsetChanged();
    
    void on_x_layerOffset_valueChanged(double arg1);    
    void on_y_layerOffset_valueChanged(double arg1);
    void on_z_layerOffset_valueChanged(double arg1);
    
    void on_actionReset_View_triggered();
    
private:
    Ui::ViewerWindow *ui;
    DM::System *system;
};

}
#endif // VIEWERWINDOW_H
