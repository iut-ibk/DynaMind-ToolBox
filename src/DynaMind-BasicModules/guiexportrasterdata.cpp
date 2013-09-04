#include "guiexportrasterdata.h"
#include "ui_guiexportrasterdata.h"
#include <QInputDialog>
#include <exportrasterdata.h>
#include <dm.h>
#include <algorithm>

GUIExportRasterData::GUIExportRasterData(DM::Module *m, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIExportRasterData)
{
	this->m = (ExportRasterData*) m;
	ui->setupUi(this);

	DM::System * sys = this->m->getSystemIn();
	//std::vector<std::string> sys_in;
	//if (sys != 0)
	//    sys_in = sys->getNamesOfViews();

	ui->comboBox->clear();
	ui->lineEdit->setText(QString::fromStdString(m->getParameterAsString("FileName")));
	//foreach (std::string s, sys_in) {
	mforeach(DM::View v, m->getViewsInStdStream())
	{
		if (v.getType() == DM::RASTERDATA)
			ui->comboBox->addItem(QString::fromStdString(v.getName()));
	}

	std::string nameofexview = this->m->getParameterAsString("NameOfExistingView");
	if (!nameofexview.empty()) {
		int index = ui->comboBox->findText(QString::fromStdString(nameofexview));
		ui->comboBox->setCurrentIndex(index);
	}

	if (ui->comboBox->count() == 0) {
		ui->comboBox->addItem("Connect Inport");
	}


}

GUIExportRasterData::~GUIExportRasterData()
{
	delete ui;
}

void GUIExportRasterData::accept() {
	if (ui->lineEdit->text().isEmpty()) {
		QDialog::accept();
		return;
	}
	/*DM::System * sys = this->m->getSystemIn();
	std::vector<std::string> sys_in;
	if (sys == 0) {
		QDialog::accept();
		return;
	}*/
	std::string nameofExistingView = ui->comboBox->currentText().toStdString();

	std::map<std::string, DM::View> views = m->getViewsInStdStream();
	//if (sys != 0)
	//    sys_in = sys->getNamesOfViews();

	//if (std::find(sys_in.begin(), sys_in.end(), nameofExistingView) == sys_in.end()
	if(views.find(nameofExistingView) == views.end()
			|| (nameofExistingView.compare("Connect Inport") == 0 ) ) {
		QDialog::accept();
		return;
	}
	this->m->setParameterValue("NameOfExistingView", nameofExistingView);

	this->m->setParameterValue("FileName", ui->lineEdit->text().toStdString());

	QDialog::accept();

}
