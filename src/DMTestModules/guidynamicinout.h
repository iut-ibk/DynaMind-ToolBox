/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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

namespace DM {
class Module;
}
#ifndef GUIDYNAMICINOUT_H
#define GUIDYNAMICINOUT_H

#include <QDialog>
#include <compilersettings.h>

class DynamicInOut;

namespace Ui {
class GUIDynamicInOut;
}

class DM_HELPER_DLL_EXPORT GUIDynamicInOut : public QDialog
{
    Q_OBJECT
    
public:
    explicit GUIDynamicInOut(DM::Module * m, QWidget *parent = 0);
    ~GUIDynamicInOut();

    
private:
    Ui::GUIDynamicInOut *ui;
    DynamicInOut * module;

protected slots:
    void addAttribute();
    void getAttribute();

};

#endif // GUIDYNAMICINOUT_H
