#include "guiappendviewfromsystem.h"
#include "ui_guiappendviewfromsystem.h"
#include "appendviewfromsystem.h"
#include <QInputDialog>

GUIAppendViewFromSystem::GUIAppendViewFromSystem(DM::Module *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIAppendViewFromSystem)
{
    ui->setupUi(this);
    this->m = (AppendViewFromSystem *) m;

    ui->listWidget->clear();
    std::vector<std::string> systemlist = m->getParameter<std::vector<std::string> >("Inports");
    foreach (std::string in, systemlist)
        ui->listWidget->addItem(QString::fromStdString(in));

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addSystem()));
}

GUIAppendViewFromSystem::~GUIAppendViewFromSystem()
{
    delete ui;
}

void GUIAppendViewFromSystem::addSystem()
{    bool ok;
     QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                          tr("Name"), QLineEdit::Normal,
                                          "", &ok);
      if (ok && !text.isEmpty()) {
          this->m->addSystem(text.toStdString());
          ui->listWidget->addItem(text);
      }


}
