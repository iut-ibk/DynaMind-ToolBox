/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Markus Sengthaler

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

#ifndef GUICONTAINERGROUP_H
#define GUICONTAINERGROUP_H

#include <QDialog>
#include <QLabel>
#include <dmcompilersettings.h>
#include <map>
#include <dmmodule.h>

/*
namespace DM {
class Module;
struct Module::Parameter;
}*/

class ContainerGroup;
namespace Ui {
class GUIContainerGroup;
}

class DM_HELPER_DLL_EXPORT GUIContainerGroup: public QDialog
{
	Q_OBJECT

public:
	explicit GUIContainerGroup(ContainerGroup * m, QWidget *parent = 0);
	~GUIContainerGroup();

private:
	Ui::GUIContainerGroup*		ui;
	ContainerGroup*				m;
	std::vector<DM::Module*>	childs;

	// internal linking
	std::map<std::string, DM::Module*> childModules;

	// hidden config tab
	QWidget*	configParamWidget;
	QString		configTabName;

	void insertStreamEntry(std::string name, bool out);

	void addParameterEdit(std::string name, std::string id);

	DM::Module::Parameter* getParam(const std::string& originalParamString);

	struct ParamEdit
	{
		DM::Module::Parameter* p;

		QWidget* editWidget;
		QLabel*	label;

		ParamEdit()
		{
			label		= NULL;
			editWidget	= NULL;
		}

		~ParamEdit()
		{
			if(label)		delete label;
			if(editWidget)	delete editWidget;
		}
	};

	std::list<ParamEdit*>	parameters;

protected slots:
	void on_addInStream_clicked();
	void on_addOutStream_clicked();
	void on_removeStream_clicked();
	
	void on_addParameter_clicked();
	void on_rmParameter_clicked();

	void on_tabWidget_currentChanged();

	void on_selectedParameters_itemSelectionChanged();

	void on_editParamName_textEdited(const QString& newText);

	void on_enableConfig_stateChanged(int state);

	void accept();

	void openFileDialog();
signals:
	void selectFiles(QString);
};

#endif // GUICONTAINERGROUP_H
