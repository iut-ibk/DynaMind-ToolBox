#include "guicalculatecentroid.h"
#include "calculatecentroid.h"
#include "ui_guicalculatecentroid.h"
#include "dm.h"

GUICalculateCentroid::GUICalculateCentroid(DM::Module *m, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUICalculateCentroid)
{
	ui->setupUi(this);
	this->m = (CalculateCentroid*) m;

	/*DM::System * sys = this->m->getSystemIn();
	std::vector<std::string> sys_in;
	if (sys != 0)
		sys_in = sys->getNamesOfViews();

	ui->comboBox_views->clear();
	foreach (std::string s, sys_in) {*/
	mforeach(DM::View v, m->getViewsInStdStream())
		ui->comboBox_views->addItem(QString::fromStdString(v.getName()));

	std::string nameofexview = this->m->getParameterAsString("NameOfExistingView");
	if (!nameofexview.empty()) {
		int index = ui->comboBox_views->findText(QString::fromStdString(nameofexview));
		ui->comboBox_views->setCurrentIndex(index);
	}

	if (ui->comboBox_views->count() == 0) {
		ui->comboBox_views->addItem("Connect Inport");
	}


}

GUICalculateCentroid::~GUICalculateCentroid()
{
	delete ui;
}

void GUICalculateCentroid::accept() {

	std::string nameofExistingView = ui->comboBox_views->currentText().toStdString();
	if (nameofExistingView.compare("Connect Inport") == 0) {
		QDialog::accept();
		return;
	}

	this->m->setNameOfView(nameofExistingView);
	QDialog::accept();
}
