/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Markus Sengthaler

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


#ifndef DMCACHE_H
#define DMCACHE_H
/*
#include <dmcompilersettings.h>
#include <dmstdutilities.h>
#include <qobject.h>
#include <queue>
#include <qatomic.h>
*/

#include <dmdbconnector.h>


namespace DM {

/**************************************************************//**
** CACHE SETTINGS
******************************************************************/
// records cache hits and misses, it may decrease performance
#define CACHE_PROFILING 


/**************************************************************//**
@class DM::Cache
@ingroup DynaMind-Core
@brief a dictionary, with limited size, sorted by access order.
Offers get, add, replace, remove for a key value pair.

COMMENTS
Implemented as highly performant double-ended list without iterator,
NULL pointer checks or similar safty structures. Searching is
accelerated by a std::map with pointers to the desired element.
Each method is optimized to maximum performance in several test
cases (e.g. unit tests). Modify with care.
******************************************************************/
template<class Tkey,class Tvalue>
class Cache
{
protected:
	// double ended node structure
    class Node
    {
    public:
        Tkey key;
        Tvalue* value;
        Node* next;
        Node* last;
        Node(const Tkey &k, Tvalue* v)
        {
            key=k;
            value=v;
            next = NULL;
            last = NULL;
        }
        ~Node()
        {
            if(value)
                delete value;
        }
    };
private:
	// map for fast searching
	std::map<Tkey,Node*> map;
protected:
    Node*   _root;
    Node*   _last;
    unsigned long    _size;
    unsigned long    _cnt;
	QMutex*	mutex;

	// sets up a new node; be aware that no linking is done
	inline Node* newNode(const Tkey &k, Tvalue* v)
	{
		Node* n = new Node(k,v);
		map[k] = n;
		return n;
	}
	// removes a node; does not accoutn for linking
	inline void removeNode(Node* n)
	{
		map.erase(n->key);
		delete pop(n);
	}
	// pushes the given node to the front
    void push_front(Node* n)
    {
        if(_last==NULL)
            _last = n;

        n->next = _root;
        n->last = NULL;
        if(_root != NULL)
            _root->last = n;
        _root = n;
        _cnt++;
    }
	// releases the first list node
    Node* pop(Node* n)
    {
        Node* last = n->last;
        Node* next = n->next;
        if(last)
            last->next = next;
        if(next)
            next->last = last;
        if(n==_last)
            _last = last;
        if(n==_root)
            _root = next;
        _cnt--;
        return n;
    }
	// search for the node with the given key
    Node* search(const Tkey &key)
    {
        typename std::map<Tkey,Node*>::iterator it = map.find(key);
		if(it==map.end())
			return NULL;
		return it->second;
    }

public:
#ifdef CACHE_PROFILING
	// counters for profiling
    unsigned long hits;
    unsigned long misses;
    void ResetProfilingCounters()
    {
		mutex->lockInline();
        misses = 0;
        hits = 0;
		mutex->unlockInline();
    }
#endif
	//!< initializes a new cache structure with the given maximum size; a size of 0 results in an infinite cache
    Cache(unsigned long size)
    {
        _size=size;
        _cnt=0;
        _root = NULL;
        _last = NULL;
		mutex = new QMutex(QMutex::Recursive);
#ifdef CACHE_PROFILING
            hits = 0;
            misses = 0;
#endif
    }
	//!< deletes all nodes, leaves the values untouched (non-deep delete)
    ~Cache()
    {
		Clear();
		delete mutex;
    }
	//!< deletes all nodes, leaves the values untouched (non-deep delete)
	void Clear()
	{
		mutex->lockInline();
        Node* cur;
        Node* next;
        next = _root;
        while(next!=NULL)
        {
            cur=next;
            next=cur->next;
            delete cur;
        }
		map.clear();
		mutex->unlockInline();
	}
	//!< returns the current element count
	unsigned long getSize(){return _size;};
	//!< returns the value associated with the given key 
    virtual Tvalue* get(const Tkey& key)
    {
		mutex->lockInline();
        Node *n = search(key);
        // push front
        if(n!=NULL)
        {
            pop(n);
            push_front(n);
#ifdef CACHE_PROFILING
            hits++;
#endif
			
			mutex->unlockInline();
            return n->value;
        }
#ifdef CACHE_PROFILING
        misses++;
#endif
		mutex->unlockInline();
        return NULL;
    }
	//!< adds a new key-value pair, does nothing if key exists. 
	// If the maximum size is reached, it will remove the last key
    virtual void add(const Tkey& key,Tvalue* value)
    {
		mutex->lockInline();
        if(search(key) != NULL)
		{
			mutex->unlockInline();
            return;
		}

        Node *n = newNode(key,value);
        push_front(n);

		if(_size)
			if(_cnt>_size)
				removeNode(_last);
		
		mutex->unlockInline();
    }
	//!< replaces the value associated with the given key, returns false if key was not existant
    virtual bool replace(const Tkey& key,Tvalue* value)
    {
		mutex->lockInline();
        Node *n = search(key);
        if(n==NULL)
		{
			mutex->unlockInline();
            return false;
		}

		removeNode(n);
        add(key, value);
		mutex->unlockInline();
        return true;
    }
	//!< removes the element from cache
    void remove(const Tkey& key)
    {
		mutex->lockInline();
        Node *n = search(key);
        if(n)	removeNode(n);
		mutex->unlockInline();
    }
};
}   // namespace DM


#endif // DMCACHE_H
