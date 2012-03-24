#include "guiclearview.h"
#include "ui_guiclearview.h"
#include <clearview.h>

GUIClearView::GUIClearView(DM::Module *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIClearView)
{
    this->m = (ClearView*) m;
    ui->setupUi(this);

    DM::System * sys = this->m->getSystemIn();
    std::vector<std::string> sys_in;
    if (sys != 0)
        sys_in = sys->getViews();

    ui->comboBox->clear();
    foreach (std::string s, sys_in) {
        ui->comboBox->addItem(QString::fromStdString(s));
    }

    std::string nameofexview = this->m->getParameterAsString("NameOfExistingView");
    if (!nameofexview.empty()) {
        int index = ui->comboBox->findText(QString::fromStdString(nameofexview));
        ui->comboBox->setCurrentIndex(index);
    }


    if (ui->comboBox->count() == 0) {
        ui->comboBox->addItem("Connect Inport");
    }


}

GUIClearView::~GUIClearView()
{
    delete ui;
}

void GUIClearView::accept() {

    DM::System * sys = this->m->getSystemIn();
    std::vector<std::string> sys_in;
    if (sys == 0) {
        QDialog::accept();
        return;
    }
    std::string nameofExistingView = ui->comboBox->currentText().toStdString();

    if (sys != 0)
        sys_in = sys->getViews();

    if (std::find(sys_in.begin(), sys_in.end(), nameofExistingView) == sys_in.end()
            || (nameofExistingView.compare("Connect Inport") == 0 ) ) {
        QDialog::accept();
        return;
    }
    this->m->setParameterValue("NameOfExistingView", nameofExistingView);


    QDialog::accept();

}
