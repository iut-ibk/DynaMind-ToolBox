#include "guipickwfsdataset.h"
#include "ui_guipickwfsdataset.h"
#include <QSettings>
#include "simplecrypt.h"

#include "ogrsf_frmts.h"
#include "gdal_priv.h"

GUIPickWFSDataset::GUIPickWFSDataset(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIPickWFSDataset)
{
	ui->setupUi(this);

	//Get WFS Server

	QSettings settings;
	QString text = settings.value("wfs_server").toString();
	QStringList list = text.replace("\\","/").split("*||*");

	foreach(QString s, list) {
		QStringList server_description = text.replace("\\","/").split("*|*");
		if (server_description.size() != 4)
			continue;
		ui->comboBox_server->addItem(server_description[0]);
	}

}

GUIPickWFSDataset::~GUIPickWFSDataset()
{
	delete ui;
}

void GUIPickWFSDataset::on_pushButton_connect_clicked()
{

	this->ui->treeWidget_datasets->clear();

	this->ui->treeWidget_datasets->setColumnCount(1);
	QTreeWidgetItem* headerItem = new QTreeWidgetItem();
	headerItem->setText(0,QString("Datasets"));
	this->ui->treeWidget_datasets->setHeaderItem(headerItem);


	QSettings settings;
	QString text = settings.value("wfs_server").toString();
	QStringList list = text.replace("\\","/").split("*||*");
	QStringList servername;


	foreach(QString s, list) {
		QStringList server_description = text.replace("\\","/").split("*|*");
		if (server_description.size() != 4)
			continue;
		if (server_description[0] == this->ui->comboBox_server->currentText()) {
			servername << "WFS:http://";
			servername <<  server_description[2]; //Username
			servername <<  ":";
			SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023));
			servername <<  crypto.decryptToString(server_description[3]); //Password
			servername <<  "@";
			servername << server_description[1];
		}
	}

	QString server = servername.join("");
	if (server.isEmpty())
		return;
	OGRRegisterAll();
	GDALAllRegister();

	OGRDataSource       *poDS;
	OGRLayer  *poLayer;


	poDS = OGRSFDriverRegistrar::Open(  server.toStdString().c_str(), FALSE );

	int LayerCount = poDS->GetLayerCount();

	for (int i = 0; i < LayerCount; i++) {
		poLayer = poDS->GetLayer(i);
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, QString::fromStdString(poLayer->GetName()));
		this->ui->treeWidget_datasets->addTopLevelItem(item);

	}

}

void GUIPickWFSDataset::accept()
{
	QSettings settings;
	QString text = settings.value("wfs_server").toString();
	QStringList list = text.replace("\\","/").split("*||*");

	foreach(QString s, list) {
		QStringList server_description = text.replace("\\","/").split("*|*");
		if (server_description.size() != 4)
			continue;
		if (server_description[0] == this->ui->comboBox_server->currentText()) {
			emit WFSServerChanged(server_description[1]);
			emit WFSUsernameChanged(server_description[2]);
			emit WFSPasswordChanged(server_description[3]);
			emit WFSDatasetChanged(this->ui->treeWidget_datasets->currentItem()->text(0));
		}
	}

	QDialog::accept();
}
