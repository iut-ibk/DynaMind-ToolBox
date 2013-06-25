#include "guiappendattributes.h"
#include "ui_guiappendattributes.h"
#include <appendattributes.h>
#include <dm.h>
GUIAppendAttributes::GUIAppendAttributes(DM::Module *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIAppendAttributes)
{
    this->m = (AppendAttributes*) m;
    ui->setupUi(this);
    DM::System * sys = this->m->getSystemIn();

	mforeach(DM::View v, m->getViewsInStream()[0])
	{
		switch (v.getType())
		{
		case DM::RASTERDATA:
			ui->comboBox_raster->addItem(QString::fromStdString(v.getName()));
			break;
		case DM::FACE:
		case DM::NODE:
            ui->comboBox_faces->addItem(QString::fromStdString(v.getName()));
			break;
		default:
			break;
		}
	}

    /*std::vector<std::string> sys_in;
    if (sys != 0)
        sys_in = sys->getNamesOfViews();
    foreach (std::string s, sys_in) {
        if (sys->getViewDefinition(s)->getType() == DM::RASTERDATA)
            ui->comboBox_raster->addItem(QString::fromStdString(s));
    }*/

    std::string nameofrasterdata = this->m->getParameterAsString("NameOfRasterData");
    if (!nameofrasterdata.empty()) {
        int index = ui->comboBox_raster->findText(QString::fromStdString(nameofrasterdata));
        ui->comboBox_raster->setCurrentIndex(index);
    }
	/*
    foreach (std::string s, sys_in) {
        if (sys->getViewDefinition(s)->getType() == DM::FACE)
            ui->comboBox_faces->addItem(QString::fromStdString(s));

        if (sys->getViewDefinition(s)->getType() == DM::NODE)
            ui->comboBox_faces->addItem(QString::fromStdString(s));
    }*/

    std::string NameOfExistingView = this->m->getParameterAsString("NameOfExistingView");
    if (!NameOfExistingView.empty()) {
        int index = ui->comboBox_faces->findText(QString::fromStdString(NameOfExistingView));
        ui->comboBox_faces->setCurrentIndex(index);
    }


    std::string newAttribute = this->m->getParameterAsString("newAttribute");
    ui->lineEdit->setText(QString::fromStdString(newAttribute));

    double multiplier = this->m->getParameter<double>("Multiplier");
    ui->lineEdit_multi->setText(QString::number(multiplier));

    bool median = this->m->getParameter<bool>("Median");
    bool sum = true;
    if (median)
        sum = false;

    ui->radioButton_median->setChecked(median);
    ui->radioButton_sum->setChecked(sum);

}

GUIAppendAttributes::~GUIAppendAttributes()
{
    delete ui;
}

void GUIAppendAttributes::accept() {

    std::string NameOfRasterData = ui->comboBox_raster->currentText().toStdString();
    this->m->setParameterValue("NameOfRasterData", NameOfRasterData);

    std::string NameOfExistingView = ui->comboBox_faces->currentText().toStdString();
    this->m->setParameterValue("NameOfExistingView", NameOfExistingView);

    std::string newAttribute = ui->lineEdit->text().toStdString();
    this->m->setParameterValue("newAttribute", newAttribute);

    std::string  multiplier = ui->lineEdit_multi->text().toStdString();
    this->m->setParameterValue("Multiplier", multiplier);

    if (ui->radioButton_median->isChecked()){
        this->m->setParameterValue("Median", "1");
    } else  {
        this->m->setParameterValue("Median", "0");
    }
    QDialog::accept();
}
