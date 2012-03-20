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
#ifndef ADDDATATONEWVIEW_H
#define ADDDATATONEWVIEW_H

#include <dmmodule.h>
#include <dm.h>
/**
 * @ingroup Modules
 * @brief The AddDataToNewView class
 */
class DM_HELPER_DLL_EXPORT AddDataToNewView : public DM::Module{

    DM_DECLARE_NODE(AddDataToNewView)
    private:
        DM::System * sys_in;
    std::string NameOfNewView;
    std::string NameOfExistingView;
    std::string NameOfNewView_old;
    std::vector<std::string> newAttributes;

    std::vector<DM::View> data;

    DM::View readView;
    DM::View writeView;
public:
    AddDataToNewView();
    void init();
    void run();
    bool createInputDialog();
    DM::System * getSystemIn();
    void addView();
    void addAttribute(std::string s);
    std::vector<std::string> getAttriutes(){return this->newAttributes;}
};

#endif // ADDDATATONEWVIEW_H
