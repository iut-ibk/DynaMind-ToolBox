#include "guiwelcome.h"
#include "ui_guiwelcome.h"

GUIWelcome::GUIWelcome(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GUIWelcome)
{
	ui->setupUi(this);
}

GUIWelcome::~GUIWelcome()
{
	delete ui;
}
