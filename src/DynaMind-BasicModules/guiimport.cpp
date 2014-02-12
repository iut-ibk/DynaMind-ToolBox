#include "GUIImport.h"
#include "ui_GUIImport.h"
#include "import.h"
#include "guipickwfsdataset.h"
#include <QFileDialog>
#include <QtGui/QTreeWidget>
#include <QInputDialog>

GUIImport::GUIImport(DM::Module *m, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIImport)
{
	this->m = (Import*) m;
	ui->setupUi(this);

	this->ui->lineEdit_Filename->setText(QString::fromStdString(m->getParameterAsString("Filename")));
	this->ui->lineEdit_viewName->setText(QString::fromStdString(m->getParameterAsString("ViewName")));
	this->ui->checkBox_append_existing->setChecked(this->m->append);

	this->ui->lineEdit_wfs_server->setText(QString::fromStdString(m->getParameterAsString("WFSServer")));
	this->ui->lineEdit_wfs_username->setText(QString::fromStdString(m->getParameterAsString("WFSUsername")));
	this->ui->lineEdit_wfs_password->setText(QString::fromStdString(m->getParameterAsString("WFSPassword")));
	this->ui->lineEdit_wfs_dataset->setText(QString::fromStdString(m->getParameterAsString("WFSDataName")));
	this->ui->lineEdit_epsgCode->setText(QString::fromStdString(m->getParameterAsString("Transform to EPSG:")));
	this->ui->checkBox_flip->setChecked(this->m->flip_wfs);
	this->ui->checkBox_linkWithExistingView->setChecked(this->m->linkWithExistingView);

	this->ui->lineEdit_offx->setText(QString::number(this->m->offsetX));
	this->ui->lineEdit_offy->setText(QString::number(this->m->offsetY));

	treeCheckMapper = new QSignalMapper(this);

	foreach(const Import::ImportView& view, this->m->viewConfig)
	{
		QTreeWidgetItem* viewItem = new QTreeWidgetItem();

		viewItem->setText(1, QString::fromStdString(view.oldName));
		viewItem->setText(2, "->");
		viewItem->setText(3, QString::fromStdString(view.newName));

		foreach(const Import::ImportAttribute& attribute, view.attributes)
		{
			QTreeWidgetItem* attrItem = new QTreeWidgetItem();

			attrItem->setText(1, QString::fromStdString(attribute.oldName));
			attrItem->setText(2, "->");
			attrItem->setText(3, QString::fromStdString(attribute.newName));

			viewItem->addChild(attrItem);

			QCheckBox* check = new QCheckBox();
			check->setChecked(true);
			connect(check, SIGNAL(clicked()), treeCheckMapper, SLOT(map()));
			treeCheckMapper->setMapping(check, (QObject*)attrItem);
			this->ui->viewTree->setItemWidget(attrItem, 0, check);
		}

		this->ui->viewTree->addTopLevelItem(viewItem);

		QCheckBox* check = new QCheckBox();
		check->setChecked(true);
		connect(check, SIGNAL(clicked()), treeCheckMapper, SLOT(map()));
		treeCheckMapper->setMapping(check, (QObject*)viewItem);
		this->ui->viewTree->setItemWidget(viewItem, 0, check);
	}

	connect(treeCheckMapper, SIGNAL(mapped(QObject*)),
		this, SLOT(updateTree(QObject*)));

	this->ui->viewTree->setColumnWidth(0, 70);
	this->ui->viewTree->resizeColumnToContents(1);
	this->ui->viewTree->resizeColumnToContents(2);
	this->ui->viewTree->resizeColumnToContents(3);
}

void GUIImport::updateTree(QObject* obj)
{
	QTreeWidgetItem* sender = (QTreeWidgetItem*)obj;

	QCheckBox* check = (QCheckBox*)ui->viewTree->itemWidget(sender, 0);
	if (check->checkState() == Qt::PartiallyChecked)
		check->setCheckState(Qt::Checked);
	
	if (!sender->parent())
	{
		// just disable or enable childs
		for (int i = 0; i < sender->childCount(); i++)
		{
			QTreeWidgetItem* attrItem = sender->child(i);
			QCheckBox* attr_check = (QCheckBox*)ui->viewTree->itemWidget(attrItem, 0);
			attr_check->setCheckState(check->checkState());
		}
	}
	else
	{
		// clicking on child, check state of siblings
		int numChecked = 0;
		int numChilds = sender->parent()->childCount();
		if (numChilds > 0)
		{
			for (int i = 0; i < numChilds; i++)
			{
				QCheckBox* attr_check = (QCheckBox*)ui->viewTree->itemWidget(sender->parent()->child(i), 0);
				if (attr_check->checkState() == Qt::Checked)
					numChecked++;
			}

			QCheckBox* view_check = (QCheckBox*)ui->viewTree->itemWidget(sender->parent(), 0);

			if (numChecked == 0)
				view_check->setCheckState(Qt::Unchecked);
			else if (numChecked == numChilds)
				view_check->setCheckState(Qt::Checked);
			else
				view_check->setCheckState(Qt::PartiallyChecked);
		}
	}
}

void GUIImport::on_viewTree_itemDoubleClicked(QTreeWidgetItem * item, int column)
{
	bool ok;
	QString newName = QInputDialog::getText(this, "Renaming", "rename to", QLineEdit::Normal, item->text(3), &ok);
	if (ok)
		item->setText(3, newName);
}

GUIImport::~GUIImport()
{
	delete ui;
}

void GUIImport::accept()
{
	m->FileName = this->ui->lineEdit_Filename->text().toStdString();
	m->ViewName = this->ui->lineEdit_viewName->text().toStdString();
	m->append = this->ui->checkBox_append_existing->isChecked();
	m->WFSServer = this->ui->lineEdit_wfs_server->text().toStdString();
	m->WFSUsername = this->ui->lineEdit_wfs_username->text().toStdString();
	m->WFSPassword = this->ui->lineEdit_wfs_password->text().toStdString();
	m->WFSDataName = this->ui->lineEdit_wfs_dataset->text().toStdString();
	m->flip_wfs = this->ui->checkBox_flip->isChecked();
	m->linkWithExistingView = this->ui->checkBox_linkWithExistingView->isChecked();
	m->epsgcode = this->ui->lineEdit_epsgCode->text().toInt();

	m->offsetX= this->ui->lineEdit_offx->text().toDouble();
	m->offsetY= this->ui->lineEdit_offy->text().toDouble();

	m->init();
	QDialog::accept();
}

void GUIImport::on_pushButton_wfs_pick_clicked()
{
	GUIPickWFSDataset * guipicker = new GUIPickWFSDataset(this);

	connect(guipicker, SIGNAL(WFSServerChanged(QString)), this->ui->lineEdit_wfs_server, SLOT(setText(QString)));
	connect(guipicker, SIGNAL(WFSUsernameChanged(QString)), this->ui->lineEdit_wfs_username, SLOT(setText(QString)));
	connect(guipicker, SIGNAL(WFSPasswordChanged(QString)), this->ui->lineEdit_wfs_password, SLOT(setText(QString)));
	connect(guipicker, SIGNAL(WFSDatasetChanged(QString)), this->ui->lineEdit_wfs_dataset, SLOT(setText(QString)));

	guipicker->show();
}

void GUIImport::on_pushButton_Filename_clicked()
{
	QString s = QFileDialog::getOpenFileName(this,
											 tr("Open file"), "", tr("Files (*.*)")) ;
	if (!s.isEmpty())
		this->ui->lineEdit_Filename->setText(s);
}
