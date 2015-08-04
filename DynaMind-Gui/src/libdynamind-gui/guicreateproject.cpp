#include "guicreateproject.h"
#include "ui_guicreateproject.h"


#include <guisimulation.h>
#include <dmlogger.h>

GUICreateProject::GUICreateProject(GUISimulation * sim, QWidget *parent) :
	sim(sim),
	QDialog(parent),
	ui(new Ui::GUICreateProject)
{
	ui->setupUi(this);
}

void GUICreateProject::accept()
{
	if (this->ui->radioButton_legecy->isChecked()) {
		QDialog::accept();
		return;
	}

	int EPSGCode = this->ui->lineEdit_epsg_code->text().toInt();
	if (EPSGCode == 0) {
		DM::Logger(DM::Warning) << "Spatial backend disabled";
	}
	DM::SimulationConfig simconfig = this->sim->getSimulationConfig();
	simconfig.setCoordinateSystem(EPSGCode);
	this->sim->setSimulationConfig(simconfig);

	QDialog::accept();
}

void GUICreateProject::reject()
{
	delete(this);
}

GUICreateProject::~GUICreateProject()
{
	delete ui;
}
