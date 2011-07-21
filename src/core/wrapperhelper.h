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

#ifndef WRAPPERHELPER_H
#define WRAPPERHELPER_H

#include "compilersettings.h"
#include <map>
#include <boost/python.hpp>
#include <algorithm>

template<class T>
struct VIBE_HELPER_DLL_EXPORT std_item
{
    typedef typename  T::value_type V;
    static V& get(T & x, int i)
    {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) return x[i];
        //IndexError();
    }
    static void set(T & x, int i, V const& v)
    {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) x[i]=v;
        //else IndexError();
    }
    static void del(T & x, int i)
    {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) x.erase(x.begin()+i);
        //else IndexError();
    }
    static void add(T & x, V const& v)
    {
        x.push_back(v);
    }
};

template<class Key, class Val>
struct map_item
{
    typedef  std::map<Key,Val> Map;

    static  Val & get(Map & self, const Key idx) {
        if( self.find(idx) != self.end() )
            return self[idx];
        PyErr_SetString(PyExc_KeyError,"Map key not found");
        boost::python::throw_error_already_set();
    }

    static void set(Map& self, const Key idx, const Val val) { self[idx]=val; }

    static void del(Map& self, const Key n) { self.erase(n); }

    static bool in(Map const& self, const Key n) { return self.find(n) != self.end(); }

    static boost::python::list keys(Map const& self)
    {
        boost::python::list t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append(it->first);
        return t;
    }
    static boost::python::list values(Map const& self)
    {
        boost::python::list t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append(it->second);
        return t;
    }
    static boost::python::list items(Map const& self)
    {
        boost::python::list t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.append( make_tuple(it->first, it->second) );
        return t;
    }
};


/*template<class T>
class IndexErrorPython


void IndexError() { PyErr_SetString(PyExc_IndexError, "Index out of range"); }*/
#endif // WRAPPERHELPER_H
