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

#ifndef ReduceTree_H
#define ReduceTree_H

#include <dm.h>
#include <string.h>

namespace DM
{
    namespace WS
    {
        #define JUNCTION_LINK

        #define JUNCTION_ATTR\
        A(Demand) \
        A(Elevation)

        #define PIPE_LINK
        #define PIPE_ATTR\
        A(Diameter) \
        A(Length)

        #define TABLE \
        X(JUNCTION,DM::NODE, JUNCTION_ATTR, JUNCTION_LINK) \
        X(PIPE,DM::EDGE,PIPE_ATTR, PIPE_LINK)

        //THIS CODE SHOULD BE IN AN OWN HEADER BUT QTCREATER CANNOT RESOLVE THESE MACROS
        #define A(a) a,
        #define X(b,c,d,e) namespace b##_ATTR_DEF{ enum {d};};
        TABLE
        #undef X
        #undef A

        #define A(a) a,
        #define X(b,c,d,e) namespace b##_LINK_DEF{ enum {e};};
        TABLE
        #undef X
        #undef A

        #define NAMECOMP COMP
        #define NAMETYPE TYPE
        #define NAMECOMPONENTSTRING COMPONENTSTRING

        #define X(a,b,c,d) a,
            enum NAMECOMP{TABLE};
        #undef X

        #define NAME COMP
        #define X(a,b,c,d) #a,
            string NAMECOMPONENTSTRING[] = {TABLE};
        #undef X

        #define X(a,b,c,d) b,
            DM::Components NAMETYPE[] = {TABLE};
        #undef X

        class DM_HELPER_DLL_EXPORT ViewDefinitionHelper
        {
        private:
            std::map<NAMECOMP,std::vector<std::string> > ATTRIBUTESTRING;

        public:
            ViewDefinitionHelper()
            {
                #define A(a) #a,
                #define X(b,c,d,e) std::string tmp_##b##_ATTR[] = {b##_ATTR}; ATTRIBUTESTRING[b]=std::vector<std::string>(tmp_##b##_ATTR, tmp_##b##_ATTR + sizeof(tmp_##b##_ATTR)/sizeof(tmp_##b##_ATTR[0]));
                TABLE
                #undef X
                #undef A
            }

            DM::View getView(NAMECOMP c, DM::ACCESS a)
            {
                return DM::View(NAMECOMPONENTSTRING[c], NAMETYPE[c], a);
            }

            DM::View getCompleteView(NAMECOMP c, DM::ACCESS a)
            {
                DM::View result(NAMECOMPONENTSTRING[c], NAMETYPE[c], a);

                for(uint index=0; index<ATTRIBUTESTRING[c].size(); index++)
                {
                    if(a==DM::WRITE)
                        result.addAttribute(ATTRIBUTESTRING[c][index]);

                    if(a==DM::READ)
                        result.getAttribute(ATTRIBUTESTRING[c][index]);

                    if(a==DM::MODIFY)
                        result.modifyAttribute(ATTRIBUTESTRING[c][index]);
                }

                return result;
            }

            std::vector<DM::View> getAll(DM::ACCESS a)
            {
                std::vector<DM::View> result;

                for(uint index = 0; index < sizeof(NAMECOMPONENTSTRING)/sizeof(NAMECOMPONENTSTRING[0]); index++)
                    result.push_back(getCompleteView(static_cast<NAMECOMP>(index),a));

                return result;
            }
        };

        #undef NAMECOMP
        #undef NAMETYPE
        #undef NAMECOMPONENTSTRING
        #undef TABLE
        #undef JUNCTION_LINK
        #undef JUNCTION_ATTR
        #undef PIPE_LINK
        #undef PIPE_ATTR
    }
}

#endif // ReduceTree_H
