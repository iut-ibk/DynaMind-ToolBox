#include "dmviewerwindow.h"
#include "ui_dmviewerwindow.h"
#include "dmaddlayerdialog.h"

namespace DM {

ViewerWindow::ViewerWindow(System *system, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewerWindow), system(system)
{
    ui->setupUi(this);
    ui->viewer->setSystem(system);
    this->addAction(ui->actionAdd_Layer);
    this->addAction(ui->actionReset_View);
    offsetChanged();
}

ViewerWindow::~ViewerWindow() {
    delete ui;
}

void ViewerWindow::on_actionAdd_Layer_triggered() {
    AddLayerDialog dialog(system, this);
    if (dialog.exec()) {
        Layer *l = dialog.getLayer(ui->viewer);
        if (!l) return;
        ui->viewer->addLayer(l);
    }
}

void ViewerWindow::offsetChanged() {
    ui->viewer->setLayerOffset(ui->x_layerOffset->value(),
                                      ui->y_layerOffset->value(),
                                      ui->z_layerOffset->value());
}

void ViewerWindow::on_x_layerOffset_valueChanged(double ) {
    offsetChanged();
}

void ViewerWindow::on_y_layerOffset_valueChanged(double ) {
    offsetChanged();
}

void ViewerWindow::on_z_layerOffset_valueChanged(double ) {
    offsetChanged();
}

void ViewerWindow::on_actionReset_View_triggered() {
    ui->viewer->showEntireScene();
}

}
