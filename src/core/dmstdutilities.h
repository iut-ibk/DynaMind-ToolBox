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


#ifndef UTILITIES_H
#define UTILITIES_H

#include <map>
#include <vector>


template <typename T1, typename T2>
inline bool map_contains(const std::map<T1,T2> *m, const T1 &key, T2 &to)
{
	typename std::map<T1,T2>::const_iterator i = m->find(key);
	if(i==m->end())	return false;

	to = i->second;
	return true;
}

template <typename T1, typename T2>
inline bool map_contains(const std::map<T1,T2> *m, const T1 &key)
{
	if(m->find(key)==m->end())	return false;
	return true;
}

template <typename T1, typename T2>
inline bool remove_element(std::map<T1,T2> *m, const T1 &key)
{
	typename std::map<T1,T2>::iterator i = m->find(key);
	if(i==m->end())	return false;
	m->erase(i);
	return true;
}

template <typename T1, typename T2>
inline bool delete_element(std::map<T1,T2*> *m, const T1 &key)
{
	typename std::map<T1,T2*>::iterator i = m->find(key);
	if(i==m->end())	return false;
	delete i->second;
	m->erase(i);
	return true;
}



template <typename T>
inline bool vector_contains(std::vector<T> *v, const T &key)
{
	return (find(v->begin(), v->end(), key) != v->end());
}
template <typename T>
inline bool vector_contains(std::vector<T> *v, const T &key, T &to)
{
	typename std::vector<T>::iterator i = v->find(key);
	if(i == v->end())
		return false;

	to = *i;
	return true;
}

template <typename T>
void deep_delete(std::vector<T*>* v)
{
	while(v->size())
	{
		delete *v->begin();
		v->erase(v->begin());
	}
	v->clear();
}

template <typename T1, typename T2>
void deep_delete(std::map<T1,T2*>* m)
{
	typename std::map<T1,T2*>::iterator it = m->begin();
	for(;it != m->end(); ++it)
		delete it->second;
	m->clear();
}

// mforeach

struct ForeachBaseBase {};

template <typename T1, typename T2>
class ForeachBase: public ForeachBaseBase
{
public:
	inline ForeachBase(const std::map<T1,T2>& t): c(t), brk(0), i(c.begin()), e(c.end()){}
	const std::map<T1,T2> c;
	mutable int brk;
	mutable typename std::map<T1,T2>::const_iterator i, e;
	inline bool condition() const { return (!brk++ && i != e);}
};

template <typename T1, typename T2> inline std::map<T1,T2> *pMForeachPointer(const std::map<T1,T2> &) { return 0; }

template <typename T1, typename T2> inline ForeachBase<T1,T2> pMForeachBaseNew(const std::map<T1,T2>& t)
{ 
	return ForeachBase<T1,T2>(t); 
}

template <typename T1, typename T2>
inline const ForeachBase<T1,T2> *pMForeachBase(const ForeachBaseBase *base, const std::map<T1,T2> *)
{ 
	return static_cast<const ForeachBase<T1,T2> *>(base); 
}


#if defined(Q_CC_MIPS)
/*
   Proper for-scoping in MIPSpro CC
*/
#  define MAP_FOREACH(variable,container)                                                             \
    if(0){}else                                                                                     \
    for (const ForeachBaseBase &_container_ = pMForeachBaseNew(container);                \
         pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->condition();       \
         ++pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->i)               \
        for (variable = pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->i->second; \
             pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->brk;           \
             --pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->brk)

#elif defined(Q_CC_DIAB)
// VxWorks DIAB generates unresolvable symbols, if container is a function call
#  define MAP_FOREACH(variable,container)                                                             \
    if(0){}else                                                                                     \
    for (const ForeachBaseBase &_container_ = pMForeachBaseNew(container);                \
         pMForeachBase(&_container_, (__typeof__(container) *) 0)->condition();       \
         ++pMForeachBase(&_container_, (__typeof__(container) *) 0)->i)               \
        for (variable = pMForeachBase(&_container_, (__typeof__(container) *) 0)->i->second; \
             pMForeachBase(&_container_, (__typeof__(container) *) 0)->brk;           \
             --pMForeachBase(&_container_, (__typeof__(container) *) 0)->brk)

#else
#  define MAP_FOREACH(variable, container) \
    for (const ForeachBaseBase &_container_ = pMForeachBaseNew(container); \
         pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->condition();       \
         ++pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->i)               \
        for (variable = pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->i->second; \
             pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->brk;           \
             --pMForeachBase(&_container_, true ? 0 : pMForeachPointer(container))->brk)
#endif // MSVC6 || MIPSpro

#define mforeach MAP_FOREACH




#endif //UTILITIES_H
