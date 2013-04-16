#include "guicellularautomata_selectlandscape.h"
#include "ui_guicellularautomata_selectlandscape.h"

GUICellularAutomata_SelectLandscape::GUICellularAutomata_SelectLandscape(QStringList landscapes, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUICellularAutomata_SelectLandscape)
{
    ui->setupUi(this);
    this->ui->comboBox_landscapes->addItems(landscapes);
}

GUICellularAutomata_SelectLandscape::~GUICellularAutomata_SelectLandscape()
{
    delete ui;
}

void GUICellularAutomata_SelectLandscape::accept()
{
    emit selected(ui->comboBox_landscapes->currentText());
    QDialog::accept();

}
