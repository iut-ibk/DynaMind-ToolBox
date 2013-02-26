#include "guiimportwithgdal.h"
#include "ui_guiimportwithgdal.h"
#include "importwithgdal.h"
#include "guipickwfsdataset.h"
#include <QFileDialog>

GUIImportWithGDAL::GUIImportWithGDAL(DM::Module *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIImportWithGDAL)
{
    this->m = (ImportwithGDAL*) m;
    ui->setupUi(this);

    this->ui->lineEdit_Filename->setText(QString::fromStdString(m->getParameterAsString("Filename")));
    this->ui->lineEdit_viewName->setText(QString::fromStdString(m->getParameterAsString("ViewName")));
    this->ui->checkBox_append_existing->setChecked(m->getParameter<bool>("AppendToExisting"));

    this->ui->lineEdit_wfs_server->setText(QString::fromStdString(m->getParameterAsString("WFSServer")));
    this->ui->lineEdit_wfs_username->setText(QString::fromStdString(m->getParameterAsString("WFSUsername")));
    this->ui->lineEdit_wfs_password->setText(QString::fromStdString(m->getParameterAsString("WFSPassword")));
    this->ui->lineEdit_wfs_dataset->setText(QString::fromStdString(m->getParameterAsString("WFSDataName")));
    this->ui->lineEdit_epsgCode->setText(QString::fromStdString(m->getParameterAsString("Transform to EPSG:")));
    this->ui->checkBox_flip->setChecked(m->getParameter<bool>("flip_wfs"));

}

GUIImportWithGDAL::~GUIImportWithGDAL()
{
    delete ui;
}

void GUIImportWithGDAL::accept()
{
    m->setParameterNative<std::string>("Filename", this->ui->lineEdit_Filename->text().toStdString());
    m->setParameterNative<std::string>("ViewName", this->ui->lineEdit_viewName->text().toStdString());
    m->setParameterNative<bool>("AppendToExisting", this->ui->checkBox_append_existing->isChecked());
    m->setParameterNative<std::string>("WFSServer", this->ui->lineEdit_wfs_server->text().toStdString());
    m->setParameterNative<std::string>("WFSUsername", this->ui->lineEdit_wfs_username->text().toStdString());
    m->setParameterNative<std::string>("WFSPassword", this->ui->lineEdit_wfs_password->text().toStdString());
    m->setParameterNative<std::string>("WFSDataName", this->ui->lineEdit_wfs_dataset->text().toStdString());
    m->setParameterNative<bool>("flip_wfs", this->ui->checkBox_flip->isChecked());
    m->setParameterNative<int>("Transform to EPSG:", this->ui->lineEdit_epsgCode->text().toInt());

    QDialog::accept();
}

void GUIImportWithGDAL::on_pushButton_wfs_pick_clicked()
{
    GUIPickWFSDataset * guipicker = new GUIPickWFSDataset(this);

    connect(guipicker, SIGNAL(WFSServerChanged(QString)), this->ui->lineEdit_wfs_server, SLOT(setText(QString)));
    connect(guipicker, SIGNAL(WFSUsernameChanged(QString)), this->ui->lineEdit_wfs_username, SLOT(setText(QString)));
    connect(guipicker, SIGNAL(WFSPasswordChanged(QString)), this->ui->lineEdit_wfs_password, SLOT(setText(QString)));
    connect(guipicker, SIGNAL(WFSDatasetChanged(QString)), this->ui->lineEdit_wfs_dataset, SLOT(setText(QString)));

    guipicker->show();
}

void GUIImportWithGDAL::on_pushButton_Filename_clicked()
{
    QString s = QFileDialog::getOpenFileName(this,
                                             tr("Open file"), "", tr("Files (*.*)")) ;
    if (!s.isEmpty())
        this->ui->lineEdit_Filename->setText(s);
}
