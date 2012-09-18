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
#include "preferences.h"
#include "QSettings"
#include "QFileDialog"
#include "QListWidget"
#include <iostream>

Preferences::Preferences(QWidget *parent)
{
    setupUi(this);
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( writePreference() ), Qt::DirectConnection );
    connect(pushButton, SIGNAL(clicked()),this, SLOT( openFileDialog() ), Qt::DirectConnection );
    connect(pushButton_2, SIGNAL(clicked()),this, SLOT( add() ), Qt::DirectConnection );
    connect(pushButton_3, SIGNAL(clicked()),this, SLOT( remove() ), Qt::DirectConnection );

    connect(pushButton_VIBePath, SIGNAL(clicked()),this, SLOT( openFileDialog() ), Qt::DirectConnection );
    connect(pushButton_VIBePathAdd, SIGNAL(clicked()),this, SLOT( add() ), Qt::DirectConnection );
    connect(pushButton_VIBePathRemove, SIGNAL(clicked()),this, SLOT( remove() ), Qt::DirectConnection );

    connect(pushButton_NativeModule, SIGNAL(clicked()),this, SLOT( openFileDialog() ), Qt::DirectConnection );
    connect(pushButton_NativeModuleAdd, SIGNAL(clicked()),this, SLOT( add() ), Qt::DirectConnection );
    connect(pushButton_NativeModuleRemove, SIGNAL(clicked()),this, SLOT( remove() ), Qt::DirectConnection );

    connect(pushButton_urbansim, SIGNAL(clicked()),this, SLOT( openFileDialog() ), Qt::DirectConnection );
    connect(pushButton_swmm, SIGNAL(clicked()),this, SLOT( openFileDialog() ), Qt::DirectConnection );
    connect(pushButton_Editra, SIGNAL(clicked()),this, SLOT( openFileDialog() ), Qt::DirectConnection );



    this->setParent(parent, Qt::Dialog);
    QSettings settings;
    QString text = settings.value("pythonModules").toString();
    QStringList list = text.replace("\\","/").split(",");
    foreach(QString s, list)
        this->listWidget->addItem(new QListWidgetItem(s));

    text = settings.value("VIBeModules").toString();
    list = text.replace("\\","/").split(",");
    foreach(QString s, list)
        this->listWidget_VIBePath->addItem(new QListWidgetItem(s));

    text = settings.value("nativeModules").toString();
    list = text.replace("\\","/").split(",");
    foreach(QString s, list)
        this->listWidget_NativeModule->addItem(new QListWidgetItem(s));

    text = settings.value("UrbanSim").toString();
    this->lineEdit_urbansim->setText(text);

    text = settings.value("SWMM").toString();
    this->lineEdit_swmm->setText(text);

    text = settings.value("Editra").toString();
    this->lineEdit_Editra->setText(text);

}


void Preferences::writePreference() {

    QSettings settings;
    //settings.beginGroup("Preferences");
    QStringList pythonModules;
    for (int i = 0; i < this->listWidget->count(); i++)
        pythonModules << this->listWidget->item(i)->text();
    QStringList VIBeModules;
    for (int i = 0; i < this->listWidget_VIBePath->count(); i++)
        VIBeModules << this->listWidget_VIBePath->item(i)->text();
    QStringList NativeModules;
    for (int i = 0; i < this->listWidget_NativeModule->count(); i++)
        NativeModules << this->listWidget_NativeModule->item(i)->text();
    settings.setValue("pythonModules", pythonModules.join(","));
    settings.setValue("nativeModules", NativeModules.join(","));
    settings.setValue("VIBeModules", VIBeModules.join(","));
    settings.setValue("UrbanSim", this->lineEdit_urbansim->text());
    settings.setValue("SWMM", this->lineEdit_swmm->text());
    settings.setValue("Editra", this->lineEdit_Editra->text());

}
void Preferences::openFileDialog() {
    QString sender = QObject::sender()->objectName();
    if (sender.compare("pushButton_NativeModule") == 0) {
         QString s = QFileDialog::getOpenFileName(this, tr("File to"), "");
         this->lineEdit_NativeModule->setText(s);
        return;
    }
    QString s = QFileDialog::getExistingDirectory(this, tr("Path to"), "");
    if(!s.isEmpty()) {
        if (sender.compare("pushButton_urbansim") == 0)
            this->lineEdit_urbansim->setText(s);
        else if (sender.compare("pushButton_swmm") == 0)
            this->lineEdit_swmm->setText(s);
        else if (sender.compare("pushButton_Editra") == 0)
            this->lineEdit_Editra->setText(s);
        else if (sender.compare("pushButton_VIBePath") == 0)
            this->lineEdit_VIBePath->setText(s);
        else
            this->lineEdit->setText(s);
    }


}
void Preferences::add() {
    QString sender = QObject::sender()->objectName();

    if (sender.compare("pushButton_VIBePathAdd") == 0) {
        if (lineEdit_VIBePath->text().size() > 0)
            this->listWidget_VIBePath->addItem(new QListWidgetItem(this->lineEdit_VIBePath->text()));
        this->lineEdit_VIBePath->clear();

        return;
    }
    if (sender.compare("pushButton_NativeModuleAdd") == 0) {
        if (lineEdit_NativeModule->text().size() > 0)
            this->listWidget_NativeModule->addItem(new QListWidgetItem(this->lineEdit_NativeModule->text()));
        this->lineEdit_NativeModule->clear();

        return;
    }
    if (lineEdit->text().size() > 0)
        this->listWidget->addItem(new QListWidgetItem(this->lineEdit->text()));
    this->lineEdit->clear();



}

void Preferences::remove() {
    QString sender = QObject::sender()->objectName();

    if (sender.compare("pushButton_VIBePathRemove") == 0) {
        if (this->listWidget_VIBePath->selectedItems().size() < 1)
            return;
        QListWidgetItem * item =this->listWidget_VIBePath->selectedItems()[0];
        delete item;
        return;
    }
    if (sender.compare("pushButton_NativeModuleRemove") == 0) {
        if (this->listWidget_NativeModule->selectedItems().size() < 1)
            return;
        QListWidgetItem * item =this->listWidget_NativeModule->selectedItems()[0];
        delete item;
        return;
    }
    if (this->listWidget->selectedItems().size() < 1)
        return;
    QListWidgetItem * item =this->listWidget->selectedItems()[0];
    delete item;

}
