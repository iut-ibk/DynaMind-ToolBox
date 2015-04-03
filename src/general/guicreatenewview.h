/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair

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
#ifndef GUICREATENEWVIEW_H
#define GUICREATENEWVIEW_H
#include <dmcompilersettings.h>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QDialog>
#endif

#include <map>


namespace DM{
	class Module;
}
namespace Ui {
class GUICreateNewView;
}

class CreateNewView;

class DM_HELPER_DLL_EXPORT GUICreateNewView : public QDialog
{
	Q_OBJECT

public:
	explicit GUICreateNewView(DM::Module * m, QWidget *parent = 0);
	~GUICreateNewView();

private:
	Ui::GUICreateNewView *ui;
	CreateNewView * m;
	typedef std::map<std::string,std::string>::iterator MapIt;
	std::map<std::string,std::string> attributes;

public slots:
	void accept();
	void on_buttonAddAttribute_clicked();
	void on_buttonDeleteAttribute_clicked();
};

#endif // GUICREATENEWVIEW_H
