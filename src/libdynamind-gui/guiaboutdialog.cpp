#include "guiaboutdialog.h"
#include "ui_guiaboutdialog.h"
#include "guisimulation.h"
GUIAboutDialog::GUIAboutDialog(GUISimulation * sim, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIAboutDialog)
{

    ui->setupUi(this);
    std::string version = DM::CoreVersion;
    ui->label_2->setText(QString::fromStdString(version));

    std::vector<std::string> modules = sim->getLoadModuleFiles();
    foreach(std::string m, modules) {
        ui->listModules->addItem(QString::fromStdString(m));
    }



}

GUIAboutDialog::~GUIAboutDialog()
{
    delete ui;
}
