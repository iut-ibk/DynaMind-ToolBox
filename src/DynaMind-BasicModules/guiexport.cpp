#include <guiexport.h>
#include <ui_guiexport.h>
#include "export.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QFileDialog>
#include <QtGui/QTreeWidget>
#include <QInputDialog>
#include <QComboBox>
#endif

#include <dmcomponent.h>
#include <dmlogger.h>
#include "ogrsf_frmts.h"

#define COL_CHECKBOX	0
#define COL_ORGNAME		1
#define COL_ARROW		2
#define COL_NEWNAME		3
#define COL_TYPE		4
#define COL_EPSG		5

// defined in guiimport.cpp
const char* GetTypeString(DM::Components t);
const char* GetTypeString(DM::Attribute::AttributeType t);

GUIExport::GUIExport(DM::Module *m, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIExport)
{
	this->m = (Export*) m;
	ui->setupUi(this);
	treeCheckMapper = NULL;

	this->ui->lineEdit_Filename->setText(QString::fromStdString(this->m->path));

	this->ui->epsgCode->setValue(this->m->epsgCode);

	updateTree();

	OGRRegisterAll();

	int nDrivers = OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();
	for (int i = 0; i < nDrivers; i++)
	{
		std::string driverName = OGRSFDriverRegistrar::GetRegistrar()->GetDriver(i)->GetName();
		this->ui->typeComboBox->addItem(QString::fromStdString(driverName));
		if (driverName == this->m->type)
			ui->typeComboBox->setCurrentIndex(i);
	}
}

void GUIExport::updateTree()
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

		viewItem->setText(COL_EPSG, QString::fromStdString(m->viewEPSGConfig[viewIter->first]));

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
	this->ui->viewTree->resizeColumnToContents(COL_EPSG);
}

void GUIExport::updateTreeChecks(QObject* obj)
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

void GUIExport::on_viewTree_itemDoubleClicked(QTreeWidgetItem * item, int column)
{
	if (column == COL_NEWNAME)
	{
		bool ok;
		QString newName = QInputDialog::getText(this, "Renaming", "rename to", QLineEdit::Normal, item->text(3), &ok);
		if (ok && !newName.isEmpty())
			item->setText(COL_NEWNAME, newName);
	}
	else if (column == COL_EPSG)
	{
		QList<QTreeWidgetItem*> selection = ui->viewTree->selectedItems();

		if (selection.size() == 0)
			DM::Logger(DM::Error) << "please select a view";
		else if (selection.size() > 1)
			DM::Logger(DM::Error) << "multiselection forbidden";
		else if (selection[0]->parent())
			DM::Logger(DM::Error) << "for overriding epsg code, please select a layer";
		else
		{
			bool ok;
			int epsgCode = QInputDialog::getInt(this, "Override source transformation",
				"specify source EPSG code (0 = default)",
				selection[0]->text(COL_EPSG).toInt(), 0, INT_MAX, 1, &ok);

			if (ok)
				selection[0]->setText(COL_EPSG, QString::number(epsgCode));
		}
	}
}

GUIExport::~GUIExport()
{
	delete ui;
}

void GUIExport::accept()
{
	m->path = this->ui->lineEdit_Filename->text().toStdString();
	m->epsgCode = this->ui->epsgCode->value();
	m->type = this->ui->typeComboBox->currentText().toStdString();

	updateViewConfig();
	m->initViews();
	QDialog::accept();
}

void GUIExport::updateViewConfig()
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

			// note: if checked or partially checked -> take as if checked
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
	// epsg overwritings
	for (int i = 0; i < ui->viewTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* it = ui->viewTree->topLevelItem(i);
		m->viewEPSGConfig[it->text(COL_ORGNAME).toStdString()] = it->text(COL_EPSG).toStdString();
	}
}

void GUIExport::on_pushButton_Filename_clicked()
{
	QString s = QFileDialog::getExistingDirectory(this, tr("Export to folder")) ;
	if (!s.isEmpty())
		this->ui->lineEdit_Filename->setText(s);

}

/*void GUIExport::on_wfsUpdateButton_clicked()
{	
	m->WFSServer = ui->lineEdit_wfs_server->text().toStdString();
	m->WFSUsername = ui->lineEdit_wfs_username->text().toStdString();

	m->WFSPassword = m->crypto.encryptToString(ui->lineEdit_wfs_password->text()).toStdString();

	this->m->init();

	updateTree();
}*/

void GUIExport::on_selectAllButton_clicked()
{
	for (int i = 0; i < ui->viewTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* it = ui->viewTree->topLevelItem(i);
		((QCheckBox*)ui->viewTree->itemWidget(it, COL_CHECKBOX))->setChecked(true);
		updateTreeChecks((QObject*)it);
	}
}

void GUIExport::on_selectNothingButton_clicked()
{
	for (int i = 0; i < ui->viewTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* it = ui->viewTree->topLevelItem(i);
		((QCheckBox*)ui->viewTree->itemWidget(it, COL_CHECKBOX))->setChecked(false);
		updateTreeChecks((QObject*)it);
	}
}
