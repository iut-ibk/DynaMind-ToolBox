#include "guifilter.h"
#include "ui_guifilter.h"
#include <dmmodule.h>

GUIFilter::GUIFilter(DM::Module * m, QWidget *parent) :
	QDialog(parent),
	m(m),
	ui(new Ui::GUIFilter)
{
	ui->setupUi(this);
	std::vector<DM::Filter> filters = m->getFilter();
	if(filters.size() == 0)
		return;
	DM::Filter f = filters[0];
	ui->lineViewEdit->setText(QString::fromStdString(f.getViewName()));
	ui->line_attributeFilter->setText(QString::fromStdString(f.getAttributeFilter().getArgument()));
	ui->lineEdit_spatialFilter->setText(QString::fromStdString(f.getSpatialFilter().getArgument()));
}

GUIFilter::~GUIFilter()
{
	delete ui;
}

void GUIFilter::accept()
{
	std::vector<DM::Filter> filters;
	DM::Filter f(ui->lineViewEdit->text().toStdString(),
				DM::FilterArgument(ui->line_attributeFilter->text().toStdString()),
				DM::FilterArgument(ui->lineEdit_spatialFilter->text().toStdString()));
	filters.push_back(f);
	m->setFilter(filters);

	QDialog::accept();
}
