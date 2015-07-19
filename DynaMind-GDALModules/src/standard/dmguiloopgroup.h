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

#ifndef GUIGROUPTEST_H
#define GUIGROUPTEST_H

#include <QDialog>
#include <dmcompilersettings.h>

namespace DM {
class Module;
}

class DMLoopGroup;
namespace Ui {
class DMGUILoopGroup;
}

class DM_HELPER_DLL_EXPORT DMGUILoopGroup: public QDialog
{
	Q_OBJECT

public:
	explicit DMGUILoopGroup(DMLoopGroup * m, QWidget *parent = 0);
	~DMGUILoopGroup();

private:
	Ui::DMGUILoopGroup *ui;
	DMLoopGroup * m;

	void insertStreamEntry(std::string name, bool write);


protected slots:
	void on_addReadStream_clicked();
	void on_addWriteStream_clicked();
	void on_removeStream_clicked();
	void accept();
};

#endif // GUIGROUPTEST_H
