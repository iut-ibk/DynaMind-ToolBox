/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

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
#ifndef OUTFALLPLACEMENT_H
#define OUTFALLPLACEMENT_H
#include "dmmodule.h"
#include "dm.h"

class DM_HELPER_DLL_EXPORT OutfallPlacement : public  DM::Module {
    DM_DECLARE_NODE(OutfallPlacement)

    private:

        DM::View Junction;
    DM::View Conduit;
    DM::View Inlet;
    DM::View WWTP;
    DM::View Outfall;
    DM::View ConduitOutfall;
    DM::View Storage;

    DM::View ForbiddenAreas;

    DM::System * city;

    std::map<DM::Node *, std::vector<DM::Edge*> > StartNodeSortedEdges;
    std::map<DM::Node *, std::vector<DM::Edge*> > EndNodeSortedEdges;
    std::map<DM::Node *, std::vector<DM::Edge*> > ConnectedEdges;


    std::map<std::string, std::string> StringMaxStrahler;
    std::map<std::string, std::string> StringStrahlerDifferenz;

    std::map<std::string, std::string> StringMaxStrahlerStorage;
    std::map<std::string, std::string> StringStrahlerDifferenzStorage;

    std::map<int, int> MaxStrahler;
    std::map<int, int> StrahlerDifferenz;

    std::map<int, int> MaxStrahlerStorage;
    std::map<int, int> StrahlerDifferenzStorage;



public:
    OutfallPlacement();
    void run();
};

#endif // OUTFALLPLACEMENT_H
