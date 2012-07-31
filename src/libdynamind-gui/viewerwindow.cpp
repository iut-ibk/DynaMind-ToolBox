#include "viewerwindow.h"
#include "ui_viewerwindow.h"

namespace DM {

ViewerWindow::ViewerWindow(System *system, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewerWindow)
{
    ui->setupUi(this);
    ui->viewer->setSystem(system);
}

ViewerWindow::~ViewerWindow()
{
    delete ui;
}

}
