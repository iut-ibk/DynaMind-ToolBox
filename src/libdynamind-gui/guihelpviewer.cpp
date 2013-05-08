/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 
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

#include "guihelpviewer.h"
#include "ui_guihelpviewer.h"
#include <QDir>
#include <QFile>

#include <sstream>
#include <dmlogger.h>
GUIHelpViewer::GUIHelpViewer(GUISimulation * sim, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUIHelpViewer)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,
   true);
    ui->setupUi(this);
    this->sim = sim;
    stringstream filename;
    filename << "file://"<< QApplication::applicationDirPath().toStdString() << "/" <<  "doc/modules/" << "nohelpavaiable" << ".html";
    this->url_view_not_avaiable = QUrl(QString::fromStdString(filename.str()));
}
void GUIHelpViewer::showHelpForModule(std::string classname) 
{
    /*
	this->currentUrl = this->url_view_not_avaiable;
    DM::Module * m = this->sim->getModuleWithUUID(uuid);
    this->currentUrl = QString::fromStdString("https://docs.google.com/document/pub?id=1gTg8ebDhoZCq-p6xJP5icqu0xTHY6KU1WEHn8k_lyWM");
    
    if (!m){
        ui->webView->load(this->currentUrl);
        return;
    }
    if (!m->getHelpUrl().empty()) {
        this->currentUrl = QUrl(QString::fromStdString(m->getHelpUrl()));
        ui->webView->load(this->currentUrl);
        return;
    }

    ui->webView->load(this->currentUrl);*/
}

GUIHelpViewer::~GUIHelpViewer()
{
    delete ui;
}

void GUIHelpViewer::on_commandBackToOvwerView_clicked()
{
    this->currentUrl = QString::fromStdString("https://docs.google.com/document/pub?id=1gTg8ebDhoZCq-p6xJP5icqu0xTHY6KU1WEHn8k_lyWM");
    ui->webView->load(this->currentUrl);
}
