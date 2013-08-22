/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
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

#include "guiaboutdialog.h"
#include "ui_guiaboutdialog.h"
#include "guisimulation.h"
#include <QDir>

GUIAboutDialog::GUIAboutDialog(GUISimulation * sim, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIAboutDialog)
{

	ui->setupUi(this);
	std::string version = DM::CoreVersion;
	ui->label_2->setText(QString::fromStdString(version));
	/*
	std::vector<std::string> modules = sim->getLoadModuleFiles();
	foreach(std::string m, modules) {
	ui->listModules->addItem(QString::fromStdString(m));
	}*/

	ui->label_tmp->setText(QDir::tempPath());



}

GUIAboutDialog::~GUIAboutDialog()
{
	delete ui;
}
