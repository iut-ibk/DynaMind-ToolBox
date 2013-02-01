/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
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

#define CREATEALLCOMPONENTS_H
#ifndef CREATEALLCOMPONENTS_H
#define CREATEALLCOMPONENTS_H

#include <dmmodule.h>
#include <dm.h>

/** @ingroup TestModules
 * @brief Creates all component types for testing purposes

 */

class DM_HELPER_DLL_EXPORT CreateAllComponenets: public DM::Module
{
    DM_DECLARE_NODE(CreateAllComponenets)
public:
    CreateAllComponenets();
    void run();
};

class DM_HELPER_DLL_EXPORT CheckAllComponenets: public DM::Module
{
    DM_DECLARE_NODE(CheckAllComponenets)
public:
    CheckAllComponenets();
    void run();
};

class DM_HELPER_DLL_EXPORT SuccessorCheck: public DM::Module
{
    DM_DECLARE_NODE(SuccessorCheck)
public:
    SuccessorCheck();
    void run();
};
#endif // CREATEALLCOMPONENTS_H
