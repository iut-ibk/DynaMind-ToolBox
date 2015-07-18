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
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,
		true);
	ui->setupUi(this);
	QSettings settings;
	this->url_view_not_avaiable = QUrl(settings.value("doc_url").toString() + "/index.html");
}
void GUIHelpViewer::showHelpForModule(DM::Module* m) {
	QSettings settings;
	this->currentUrl = settings.value("doc_url").toString();
	if (!m){
		ui->webView->load(settings.value("doc_url").toString() + "/index.html");
		return;
	}
	if (!m->getHelpUrl().empty()) {
		this->currentUrl = QUrl(settings.value("doc_url").toString() + "/" +QString::fromStdString(m->getHelpUrl()));
		ui->webView->load(this->currentUrl);
		return;
	}
	ui->webView->load(settings.value("doc_url").toString() + "/index.html");
}

GUIHelpViewer::~GUIHelpViewer()
{
	delete ui;
}

void GUIHelpViewer::on_commandBackToOvwerView_clicked()
{
	QSettings settings;
	this->currentUrl = settings.value("doc_url").toString() + "/index.html";
	ui->webView->load(this->currentUrl);
}
