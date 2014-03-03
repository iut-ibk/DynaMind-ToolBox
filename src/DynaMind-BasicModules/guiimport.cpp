#include <guiimport.h>
#include <ui_guiimport.h>
#include "import.h"
#include "guipickwfsdataset.h"
#include <QFileDialog>
#include <QtGui/QTreeWidget>
#include <QInputDialog>

#define COL_CHECKBOX	0
#define COL_ORGNAME		1
#define COL_ARROW		2
#define COL_NEWNAME		3
#define COL_TYPE		4



const char* GetTypeString(DM::Components t)
{
	static const char* typeNames[] = { "Component", "Node", "Edge", "Face", "System", "RasterData" };
	return typeNames[t];
}

const char* GetTypeString(DM::Attribute::AttributeType t)
{
	static const char* typeNames[] = { "no type", "double", "string", "time series", "link", "double vector", "string vector" };
	return typeNames[t];
}

GUIImport::GUIImport(DM::Module *m, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIImport)
{
	this->m = (Import*) m;
	ui->setupUi(this);
	treeCheckMapper = NULL;

	this->ui->lineEdit_Filename->setText(QString::fromStdString(this->m->FileName));
	this->ui->checkBox_append_existing->setChecked(this->m->append);

	this->ui->lineEdit_wfs_server->setText(QString::fromStdString(this->m->WFSServer));
	this->ui->lineEdit_wfs_username->setText(QString::fromStdString(this->m->WFSUsername));
	this->ui->lineEdit_wfs_password->setText(QString::fromStdString(this->m->WFSPassword));
	this->ui->lineEdit_wfs_dataset->setText(QString::fromStdString(this->m->WFSDataName));
	this->ui->lineEdit_epsgCode->setText(QString::number(this->m->epsgcode));
	this->ui->checkBox_flip->setChecked(this->m->flip_wfs);
	this->ui->checkBox_linkWithExistingView->setChecked(this->m->linkWithExistingView);

	this->ui->lineEdit_offx->setText(QString::number(this->m->offsetX));
	this->ui->lineEdit_offy->setText(QString::number(this->m->offsetY));

	updateTree();
}

void GUIImport::updateTree()
{
	if (treeCheckMapper)
	{
		delete treeCheckMapper;
		treeCheckMapper = NULL;
	}

	while (this->ui->viewTree->takeTopLevelItem(0))
		;

	treeCheckMapper = new QSignalMapper(this);

	for (StringMap::iterator viewIter = this->m->viewConfig.begin();
		viewIter != this->m->viewConfig.end(); ++viewIter)
	{
		if (strchr(viewIter->first.c_str(), '.') != NULL)
			continue;	// skip if it is not a view or empty

		Qt::CheckState state = viewIter->second.empty() ? Qt::Unchecked : Qt::Checked;

		// add view item
		QTreeWidgetItem* viewItem = new QTreeWidgetItem();

		viewItem->setText(COL_ORGNAME, QString::fromStdString(viewIter->first));
		viewItem->setText(COL_ARROW, "->");
		viewItem->setText(COL_NEWNAME, QString::fromStdString(state == Qt::Checked ? viewIter->second : viewIter->first));
		viewItem->setText(COL_TYPE, GetTypeString((DM::Components)this->m->viewConfigTypes[viewIter->first]));

		// add attributes
		for (StringMap::iterator attrIter = this->m->viewConfig.begin();
			attrIter != this->m->viewConfig.end(); ++attrIter)
		{
			if (strchr(attrIter->first.c_str(), '.') == NULL)
				continue; // skip if it is a view or emtpy

			bool isAttributeActive = !attrIter->second.empty();

			if (!isAttributeActive && state == Qt::Checked)
				state = Qt::PartiallyChecked;

			QStringList parsedString = QString::fromStdString(attrIter->first).split('.', QString::SkipEmptyParts);

			if (parsedString.size() != 2)
			{
				DM::Logger(DM::Error) << "error parsing parameter: " << attrIter->first;
				continue;
			}

			if (parsedString.first() == QString::fromStdString(viewIter->first))
			{
				// this attribute is for this view
				QTreeWidgetItem* attrItem = new QTreeWidgetItem();

				attrItem->setText(COL_ORGNAME, parsedString.last());
				attrItem->setText(COL_ARROW, "->");
				attrItem->setText(COL_NEWNAME, isAttributeActive ?
					QString::fromStdString(attrIter->second) :
					parsedString.last());

				attrItem->setText(COL_TYPE,
					GetTypeString((DM::Attribute::AttributeType)this->m->viewConfigTypes[attrIter->first]));

				viewItem->addChild(attrItem);

				QCheckBox* check = new QCheckBox();
				check->setChecked(isAttributeActive);
				connect(check, SIGNAL(clicked()), treeCheckMapper, SLOT(map()));
				treeCheckMapper->setMapping(check, (QObject*)attrItem);
				this->ui->viewTree->setItemWidget(attrItem, COL_CHECKBOX, check);
			}
		}

		this->ui->viewTree->addTopLevelItem(viewItem);

		QCheckBox* check = new QCheckBox();
		check->setCheckState(state);
		connect(check, SIGNAL(clicked()), treeCheckMapper, SLOT(map()));
		treeCheckMapper->setMapping(check, (QObject*)viewItem);
		this->ui->viewTree->setItemWidget(viewItem, COL_CHECKBOX, check);
	}

	connect(treeCheckMapper, SIGNAL(mapped(QObject*)),
		this, SLOT(updateTreeChecks(QObject*)));

	this->ui->viewTree->setColumnWidth(COL_CHECKBOX, 70);
	this->ui->viewTree->resizeColumnToContents(COL_ORGNAME);
	this->ui->viewTree->resizeColumnToContents(COL_ARROW);
	this->ui->viewTree->resizeColumnToContents(COL_NEWNAME);
	this->ui->viewTree->resizeColumnToContents(COL_TYPE);
}

void GUIImport::updateTreeChecks(QObject* obj)
{
	QTreeWidgetItem* sender = (QTreeWidgetItem*)obj;

	QCheckBox* check = (QCheckBox*)ui->viewTree->itemWidget(sender, COL_CHECKBOX);
	
	if (!sender->parent())
	{
		Qt::CheckState state = check->checkState();
		if (check->checkState() == Qt::PartiallyChecked)
			state = Qt::Unchecked;
		
		// just disable or enable childs
		for (int i = 0; i < sender->childCount(); i++)
		{
			QTreeWidgetItem* attrItem = sender->child(i);
			QCheckBox* attr_check = (QCheckBox*)ui->viewTree->itemWidget(attrItem, COL_CHECKBOX);
			attr_check->setCheckState(state);
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
				QCheckBox* attr_check = (QCheckBox*)ui->viewTree->itemWidget(sender->parent()->child(i), COL_CHECKBOX);
				if (attr_check->checkState() == Qt::Checked)
					numChecked++;
			}

			QCheckBox* view_check = (QCheckBox*)ui->viewTree->itemWidget(sender->parent(), COL_CHECKBOX);

			if (numChecked == numChilds)
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
	if (ok && !newName.isEmpty())
		item->setText(COL_NEWNAME, newName);
}

GUIImport::~GUIImport()
{
	delete ui;
}

void GUIImport::accept()
{
	m->FileName = this->ui->lineEdit_Filename->text().toStdString();
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

	updateViewConfig();
	m->initViews();
	QDialog::accept();
}

void GUIImport::updateViewConfig()
{
	for (StringMap::iterator it = m->viewConfig.begin(); it != m->viewConfig.end(); ++it)
	{
		if (strchr(it->first.c_str(), '.') == NULL)
		{
			// view
			QList<QTreeWidgetItem*> views = ui->viewTree->findItems(
				QString::fromStdString(it->first),
				Qt::MatchExactly, COL_ORGNAME);

			if (views.size() != 1)
			{
				DM::Logger(DM::Error) << "view tree corrupt";
				return;
			}

			QCheckBox* checkBox = (QCheckBox*)ui->viewTree->itemWidget(views.first(), COL_CHECKBOX);

			// note: if checked or partially checked -> take it
			it->second = (checkBox->checkState() != Qt::Unchecked) ?
				views.first()->text(COL_NEWNAME).toStdString() : "";
		}
		else
		{
			// attribute
			QStringList parsedString = QString::fromStdString(it->first).split('.', QString::SkipEmptyParts);

			if (parsedString.size() != 2)
			{
				DM::Logger(DM::Error) << "view tree corrupt";
				return;
			}

			QList<QTreeWidgetItem*> attributes = ui->viewTree->findItems(
				parsedString.last(), Qt::MatchExactly | Qt::MatchRecursive, COL_ORGNAME);

			QTreeWidgetItem* attribute = NULL;
			foreach(QTreeWidgetItem* a, attributes)
			{
				if (a->parent() && a->parent()->text(COL_ORGNAME) == parsedString.first())
				{
					attribute = a;
					break;
				}
			}

			if (!attribute)
			{
				DM::Logger(DM::Error) << "view tree corrupt";
				return;
			}

			QCheckBox* checkBox = (QCheckBox*)ui->viewTree->itemWidget(attribute, COL_CHECKBOX);

			// note: if checked or partially checked -> take it
			it->second = (checkBox->checkState() != Qt::Unchecked) ?
				attribute->text(COL_NEWNAME).toStdString() : "";
		}
	}
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

void GUIImport::on_lineEdit_Filename_textChanged()
{
	m->FileName = this->ui->lineEdit_Filename->text().toStdString();
	this->m->init();

	updateTree();
}
