#include "guiwelcome.h"
#include "ui_guiwelcome.h"
#include <QFileDialog>
#include "guicreateproject.h"

GUIWelcome::GUIWelcome(GUISimulation *sim, QWidget *parent) :
	sim(sim),
	QWidget(parent),
	ui(new Ui::GUIWelcome)
{
	ui->setupUi(this);

	setWindowFlags(Qt::CustomizeWindowHint);
}

GUIWelcome::~GUIWelcome()
{
	delete ui;
}

void GUIWelcome::on_pushButton_new_project_clicked()
{
	GUICreateProject * create = new GUICreateProject(this->sim);
	create->show();
	delete(this);
}

void GUIWelcome::on_pushButton_load_project_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open DynaMind File"),
		"", tr("DynaMind Files (*.dyn)"));

	if (!fileName.isEmpty()){
		this->sim->currentDocument = fileName;
		this->sim->loadSimulation(fileName.toStdString());

		delete(this);
	}
}
