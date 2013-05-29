#include "dynamindmainwindow.h"
#include <dmmainwindow.h>
#include <guisimulation.h>
DynaMindMainWindow::DynaMindMainWindow()
{
    mw = new DMMainWindow();
}

void DynaMindMainWindow::createMainWindow() {

    mw->show();


    return;
}

void DynaMindMainWindow::registerNativeDll(string fileName) {
    mw->getSimulation()->registerModule(fileName);
}
