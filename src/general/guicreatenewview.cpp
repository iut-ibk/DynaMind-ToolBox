#include "guicreatenewview.h"
#include "ui_guicreatenewview.h"
#include <QInputDialog>
#include <createnewview.h>
#include <dm.h>
#include <algorithm>

typedef std::map<std::string, DM::View> view_map;

GUICreateNewView::GUICreateNewView(DM::Module *m, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUICreateNewView)
{
	this->m = (CreateNewView*) m;
	ui->setupUi(this);

	ui->attributeList->clear();
	ui->viewName->setText(QString::fromStdString(this->m->NameOfView));
	ui->viewType->setCurrentIndex(this->m->viewType);
	ui->checkBox_modifyStream->setChecked(this->m->modifyExistingView);
	ui->checkBox_startNewStream->setChecked(this->m->startNewStream);
	this->attributes=this->m->newAttributes;

	MapIt it = attributes.begin();

	for(it;it!=attributes.end();it++)
		ui->attributeList->addItem(QString::fromStdString((*it).first));
}

GUICreateNewView::~GUICreateNewView()
{
	delete ui;
}

void GUICreateNewView::accept()
{
	if (ui->viewName->text().isEmpty())
	{
		QDialog::accept();
		return;
	}

	this->m->NameOfView=ui->viewName->text().toStdString();
	this->m->viewType=ui->viewType->currentIndex();
	this->m->modifyExistingView=ui->checkBox_modifyStream->isChecked();
	this->m->startNewStream=ui->checkBox_startNewStream->isChecked();
	this->m->newAttributes=this->attributes;

	m->init();

	QDialog::accept();
}

void GUICreateNewView::on_buttonAddAttribute_clicked()
{
	QString name = ui->newAttributename->text();

	if(name=="")
		return;

	if(this->attributes.find(name.toStdString())!=attributes.end())
		return;

	attributes[name.toStdString()] = QString::number(ui->newAttributeType->currentIndex()+1).toStdString();

	ui->attributeList->addItem(name);
}

void GUICreateNewView::on_buttonDeleteAttribute_clicked()
{
	foreach (QListWidgetItem* i,ui->attributeList->selectedItems())
	{
		this->attributes.erase(this->attributes.find(i->text().toStdString()));
		delete i;
	}
}
