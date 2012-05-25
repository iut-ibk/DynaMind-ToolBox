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
#ifndef DMMODULERUNNABLE_H
#define DMMODULERUNNABLE_H

#include <QRunnable>

namespace DM {
class Module;
}

namespace DM {
/**
 * @ingroup DynaMind-Core
 * @brief Runnable object for the modules
 */
class ModuleRunnable : public QRunnable
{
private:
    DM::Module * m;

public:
    /** @brief Constructor, needs module */
    ModuleRunnable(DM::Module *m);

    /**
     * @brief Runs module
     *
     * Befor we call the run mehtod from the module the updateParameter method is then the init method. Since the
     * module can change data in the init method the update method is called again after!
     * Then we check if all data are set, if not the simulation status is set to SIM_ERROR_SYSTEM_NOT_SET.
     */
    virtual void run();
};
}

#endif // MODULERUNNABLE_H
