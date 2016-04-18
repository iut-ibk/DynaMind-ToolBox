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
QString GUIHelpViewer::getBaseUrl()
{
	QSettings settings;
	QString doc_rul = settings.value("doc_url").toString();
	if (!doc_rul.isEmpty())
		return doc_rul.replace(" ", "%20");

	return "file:///" +  QCoreApplication::applicationDirPath().replace(" ", "%20") + "/doc";
}

GUIHelpViewer::GUIHelpViewer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GUIHelpViewer)
{
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,
		true);
	ui->setupUi(this);
	this->url_view_not_avaiable = QUrl(getBaseUrl() + "/index.html");
}
void GUIHelpViewer::showHelpForModule(DM::Module* m) {
	if (!m){
		ui->webView->load(getBaseUrl() + "/index.html");
		return;
	}
	if (!m->getHelpUrl().empty()) {
		this->currentUrl =getBaseUrl() + "/" +QString::fromStdString(m->getHelpUrl());
		ui->webView->load(this->currentUrl);
		return;
	}
	ui->webView->load(getBaseUrl() + "/index.html");
}

GUIHelpViewer::~GUIHelpViewer()
{
	delete ui;
}

void GUIHelpViewer::on_commandBackToOvwerView_clicked()
{
	currentUrl = getBaseUrl() + "/index.html";
	ui->webView->load(this->currentUrl);
}
