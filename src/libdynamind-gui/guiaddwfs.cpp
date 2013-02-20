#include "guiaddwfs.h"
#include "ui_guiaddwfs.h"
#include "simplecrypt.h"
#include "QSettings"

GUIAddWFS::GUIAddWFS(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIAddWFS)
{
    ui->setupUi(this);
}

GUIAddWFS::~GUIAddWFS()
{
    delete ui;
}

void GUIAddWFS::accept()
{
    QSettings settings;
    SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023)); //some random number

    QStringList wfs_server;

    wfs_server << this->ui->lineEdit_name->text();
    wfs_server << this->ui->lineEdit_server->text();
    wfs_server << this->ui->lineEdit_username->text();
    if (!this->ui->lineEdit_password->text().isEmpty()) wfs_server << crypto.encryptToString(this->ui->lineEdit_password->text());
    else  wfs_server << this->ui->lineEdit_password->text();


    settings.setValue("wfs_server", wfs_server.join("*|*"));

    QDialog::accept();


}
