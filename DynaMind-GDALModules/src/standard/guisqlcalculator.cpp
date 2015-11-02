#include "guisqlcalculator.h"
#include "ui_guisqlcalculator.h"
#include "customtextedit.h"
#include <QCompleter>


GUISQLCalculator::GUISQLCalculator(DM::Module *m, QWidget *parent) :
	m(m),
	QDialog(parent),
	ui(new Ui::GUISQLCalculator)
{
	ui->setupUi(this);

	ui->lineEditAttributeName->setText(QString::fromStdString(m->getParameterAsString("attribute")));
	ui->lineEditType->setText(QString::fromStdString(m->getParameterAsString("attribute_type")));
    ct = new CustomTextEdit(m);
    ct->setText(QString::fromStdString(m->getParameterAsString("query")));
    ui->groupBox_2->layout()->addWidget(ct);

}

void GUISQLCalculator::accept()
{
	m->setParameterValue("attribute", ui->lineEditAttributeName->text().toStdString());
    m->setParameterValue("attribute_type", ui->lineEditType->text().toStdString());
    m->setParameterValue("query", ct->toPlainText().toStdString());

	QDialog::accept();
}


GUISQLCalculator::~GUISQLCalculator()
{
	delete ui;
}
