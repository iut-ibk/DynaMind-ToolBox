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

#include "dmcompilersettings.h"

#ifndef APPENDATTRIBUTES_H
#define APPENDATTRIBUTES_H
#include <dmmodule.h>

/** @ingroup Modules
  * @author Christian Urich
  */
class DM_HELPER_DLL_EXPORT AppendAttributes : public  DM::Module {
    DM_DECLARE_NODE(AppendAttributes)

    private:
        DM::System * sys_in;
    std::string NameOfExistingView;
    std::string newAttribute;
    std::string newAttribute_old;
    std::string NameOfRasterData;
    std::vector<DM::View> data;

    DM::View readView;

    std::map<std::string, DM::RasterData*> attribueMaps;


    bool median;
    double multiplier;

public:
    AppendAttributes();
    void run();
    void init();
    bool createInputDialog();
    DM::System * getSystemIn(){return this->sys_in;}
    virtual std::string getHelpUrl();
};

#endif // APPENDATTRIBUTES_H
