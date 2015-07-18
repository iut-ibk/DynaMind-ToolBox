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


#include "guiaddwfs.h"
#include "ui_guiaddwfs.h"
#include "simplecrypt.h"
#include "QSettings"

GUIAddWFS::GUIAddWFS(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIAddWFS)
{
	ui->setupUi(this);
}

GUIAddWFS::~GUIAddWFS()
{
	delete ui;
}

void GUIAddWFS::accept()
{
	QSettings settings;
	SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023)); //some random number

	QStringList wfs_server;

	wfs_server << this->ui->lineEdit_name->text();
	wfs_server << this->ui->lineEdit_server->text();
	wfs_server << this->ui->lineEdit_username->text();
	if (!this->ui->lineEdit_password->text().isEmpty()) wfs_server << crypto.encryptToString(this->ui->lineEdit_password->text());
	else  wfs_server << this->ui->lineEdit_password->text();

	settings.setValue("wfs_server", wfs_server.join("*|*"));

	QDialog::accept();
}
