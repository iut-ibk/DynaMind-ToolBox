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
GUIHelpViewer::GUIHelpViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUIHelpViewer)
{
    ui->setupUi(this);
    stringstream filename;
    filename << "file://"<< QDir::currentPath().toStdString() << "/" <<  "doc/modules/" << "nohelpavaiable" << ".html";
    this->url_view_not_avaiable = QUrl(QString::fromStdString(filename.str()));
}
void GUIHelpViewer::showHelpForModule(std::string classname) {
    this->currentUrl = this->url_view_not_avaiable;
    stringstream filename;
    filename << QDir::currentPath().toStdString() << "/" <<  "doc/modules/" << classname << ".html";
    DM::Logger(DM::Debug) << "Helpfile at " << filename.str();

    QString qFilename = QString::fromStdString(filename.str());
    qFilename.replace("\\", "/");
    qFilename.replace(" ", "%20");

    QFile f(qFilename);
    if (f.exists()) {
        stringstream url;
        url << "file://" << filename.str();
        this->currentUrl = QUrl(QString::fromStdString(url.str()));
    }
    ui->webView->load(this->currentUrl);
}

GUIHelpViewer::~GUIHelpViewer()
{
    delete ui;
}
