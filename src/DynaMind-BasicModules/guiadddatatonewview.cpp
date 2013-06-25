#include "guiadddatatonewview.h"
#include "ui_guiadddatatonewview.h"
#include <QInputDialog>
#include <adddatatonewview.h>
#include <dm.h>
#include <algorithm>

typedef std::map<std::string, DM::View> view_map;

GUIAddDatatoNewView::GUIAddDatatoNewView(DM::Module *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIAddDatatoNewView)
{
    this->m = (AddDataToNewView*) m;
    ui->setupUi(this);
    DM::System * sys = this->m->getSystemIn();
    //std::vector<std::string> sys_in;
    //if (sys != 0)
    //    sys_in = sys->getNamesOfViews();
	view_map views = m->getViewsInStream()[0];

    ui->comboBox_views->clear();
    ui->lineEdit->setText(QString::fromStdString(m->getParameterAsString("NameOfNewView")));
	for(view_map::iterator it = views.begin(); it != views.end(); ++it)
		ui->comboBox_views->addItem(QString::fromStdString(it->first));

    ui->checkBox_onlySelected->setChecked(m->getParameter<bool>("onlySelected"));

    std::string nameofexview = this->m->getParameterAsString("NameOfExistingView");
    if (!nameofexview.empty()) {
        int index = ui->comboBox_views->findText(QString::fromStdString(nameofexview));
        ui->comboBox_views->setCurrentIndex(index);
    }

    if (ui->comboBox_views->count() == 0) {
        ui->comboBox_views->addItem("Connect Inport");
    }

    foreach (std::string s, this->m->getParameter<std::vector<std::string> >("newAttributes")) {
        ui->listWidget->addItem(QString::fromStdString(s));
    }

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addAttribute()));
}

GUIAddDatatoNewView::~GUIAddDatatoNewView()
{
    delete ui;
}
void GUIAddDatatoNewView::addAttribute() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Name"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
        this->m->addAttribute(text.toStdString());

    ui->listWidget->addItem(text);
}

void GUIAddDatatoNewView::accept() {
    if (ui->lineEdit->text().isEmpty()) {
        QDialog::accept();
        return;
    }
    std::string nameofExistingView = ui->comboBox_views->currentText().toStdString();
    this->m->setParameterValue("NameOfNewView", ui->lineEdit->text().toStdString());
    this->m->setParameterValue("NameOfExistingView", nameofExistingView);
    this->m->setParameterNative("onlySelected", ui->checkBox_onlySelected->isChecked());

    DM::System * sys = this->m->getSystemIn();
    /*std::vector<std::string> sys_in;
    if (sys == 0) {
        QDialog::accept();
        return;
    }
    if (sys != 0)
        sys_in = sys->getNamesOfViews();*/

	std::map<std::string, DM::View> views = m->getViewsInStream()[0];

    //if (std::find(sys_in.begin(), sys_in.end(), nameofExistingView) == sys_in.end()
	if(views.find(nameofExistingView) == views.end()
            || (nameofExistingView.compare("Connect Inport") == 0 ) ) {
        QDialog::accept();
        return;
    }

    this->m->setParameterValue("NameOfNewView", ui->lineEdit->text().toStdString());


    this->m->addView();
    QDialog::accept();
}
