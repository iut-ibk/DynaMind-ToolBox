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
#ifndef GUICELLULARAUTOMATA_NEIGHBOURHOOD_H
#define GUICELLULARAUTOMATA_NEIGHBOURHOOD_H

#include <QDialog>
#include <guicellularautomata.h>
namespace DM {
	class Module;
}
namespace Ui {
	class GUICellularAutomata_Neighbourhood;
}

class GUICellularAutomata_Neighbourhood : public QDialog
{
	Q_OBJECT

public:
	explicit GUICellularAutomata_Neighbourhood(GUICellularAutomata * GUICellular, QWidget *parent = 0);
	std::string getNeighbourhood(std::string name);
	~GUICellularAutomata_Neighbourhood();

private:
	Ui::GUICellularAutomata_Neighbourhood *ui;
	DM::Module * m;
	GUICellularAutomata * gui;
public slots:
	void accept();

signals:
	void valuesChanged();

};

#endif // GUICELLULARAUTOMATA_NEIGHBOURHOOD_H
