/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich
 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "guigrouptest.h"
#include "ui_guigrouptest.h"
#include <grouptest.h>
#include <QInputDialog>

GUIGroupTest::GUIGroupTest(DM::Module * m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIGroupTest)
{
    ui->setupUi(this);

    this->m = (GroupTest *) m;

    ui->lineEdit->setText(QString::fromStdString(m->getParameterAsString("Runs")));

    foreach (std::string s, this->m->getParameter<std::vector<std::string > >("nameOfInViews") ) {
        this->ui->listWidget_in->addItem(QString::fromStdString(s));
    }


    foreach (std::string s, this->m->getParameter<std::vector<std::string > >("nameOfOutViews") ) {
        this->ui->listWidget_out->addItem(QString::fromStdString(s));
    }
    connect(ui->addInport, SIGNAL(pressed()), this, SLOT(addInPort()));
    connect(ui->addOutport, SIGNAL(pressed()), this, SLOT(addOutPort()));
    connect(ui->rmInport, SIGNAL(pressed()), this, SLOT(removeInPort()));

}

GUIGroupTest::~GUIGroupTest()
{
    delete ui;
}

void GUIGroupTest::addInPort() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Name"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        this->m->addInPort(text.toStdString());
        this->ui->listWidget_in->addItem(text);
    }

}

void GUIGroupTest::addOutPort() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Name"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        this->ui->listWidget_out->addItem(text);
        this->m->addOutPort(text.toStdString());
    }

}
void GUIGroupTest::removeInPort() {
    if (!ui->listWidget_in->currentItem())
        return;
    std::string toRemove = ui->listWidget_in->currentItem()->text().toStdString();
    this->m->removeInPort(toRemove);
    this->ui->listWidget_in->removeItemWidget(this->ui->listWidget_in->currentItem());

}

void GUIGroupTest::accept() {
    this->m->setParameterValue("Runs", ui->lineEdit->text().toStdString());
    QDialog::accept();
}
