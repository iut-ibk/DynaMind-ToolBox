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
#ifndef PYMODULE_H
#define PYMODULE_H
#include "compilersettings.h"
#include <Python.h>
#include <boost/python/wrapper.hpp>
#include "module.h"
#include <string.h>
#include <map>
#include <vector>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
using namespace std;



namespace vibens {
typedef std::map<std::string, RasterData*> RASTERDATA_MAP;
typedef std::map<std::string, VectorData*> VECTORDATA_MAP;

void wrap_module();

struct VIBE_HELPER_DLL_EXPORT  ModuleWrapper : Module, boost::python::wrapper<Module> {
    ModuleWrapper() ;
    void run();

    bool createInputDialog();
    virtual ~ModuleWrapper();
    const char * getClassName();
    const char * getFileName();
    void setClassName(string class_name);
    void setFileName(string file_name);
    void setSelf(boost::python::object self);
    char * class_name;
    char * file_name;
    boost::python::object self;
    void addParameter_all(boost::python::object self,std::string Name, int Type, std::string description);
    void addParameter(boost::python::object self,std::string Name, int Type);

    void updateParameter();
    void setParameter();
    string returnParameterValue(string name);

    void init(const boost::unordered_map<std::string, std::string> &parameters);
    void default_init(const boost::unordered_map<std::string, std::string> &parameters);
    bool isFromOutSide(std::string name);
    map<string, double> double_params;
    map<string, long> long_params;
    map<string, bool> bool_params;
    map<string, std::string> string_params;
    map<string, map<string, std::string> >string_maps;

    vector<string> rasterdata_in;
    vector<string> rasterdata_out;
    vector<string> vectordata_in;
    vector<string> vectordata_out;
    vector<string> doubledata_in;
    vector<string> doubledata_out;
    map<string, RASTERDATA_MAP> user_defined_rasterdata_in;
    map<string, VECTORDATA_MAP> user_defined_vectordata_in;


};

class VIBE_HELPER_DLL_EXPORT  RasterDataMap {

private:
    RASTERDATA_MAP rdatamap;

public:
    std::string Name;
    RASTERDATA_MAP getRasterData() {return this->rdatamap;}
    void setRasterData(RASTERDATA_MAP rdatamap) {this->rdatamap = rdatamap;}
    std::vector<std::string> getNames() {
        std::vector<std::string> v;
        for (std::map<std::string, RasterData*>::iterator it = this->rdatamap.begin(); it != this->rdatamap.end(); ++it) {
            v.push_back(it->first);
        }
        return v;
    }
    RasterData * get(std::string name) {
        return this->rdatamap[name];
    }
    RasterData * getItem(int id) {
        int counter = 0;
        for (std::map<std::string, RasterData*>::iterator it = this->rdatamap.begin(); it != this->rdatamap.end(); ++it) {
            if (counter == id)
                return (it->second);
            counter++;
        }
        RasterData * r = new RasterData();
        return r;
    }

    int size() {return this->rdatamap.size();}


};

class VIBE_HELPER_DLL_EXPORT  VectorDataMap {

private:
    VECTORDATA_MAP vdatamap;

public:
    std::string Name;
    VECTORDATA_MAP getVectorData() {return this->vdatamap;}
    void setVectorData(VECTORDATA_MAP vdatamap) {this->vdatamap = vdatamap;}
    std::vector<std::string> getNames() {
        std::vector<std::string> v;
        for (std::map<std::string, VectorData*>::iterator it = this->vdatamap.begin(); it != this->vdatamap.end(); ++it) {
            v.push_back(it->first);
        }
        return v;
    }
    VectorData * get(std::string name) {
        return this->vdatamap[name];
    }
    VectorData * getItem(int id) {
        int counter = 0;
        for (std::map<std::string, VectorData*>::iterator it = this->vdatamap.begin(); it != this->vdatamap.end(); ++it) {
            if (counter == id)
                return (it->second);
            counter++;
        }
        VectorData * r = new VectorData();
        return r;
    }

    int size() {return this->vdatamap.size();}


};

class VIBE_HELPER_DLL_EXPORT  RasterDataIn {
private:
    RasterData * rdata;
public:
    RasterData * getItem() {return rdata;}
    void setRasterData(RasterData * rdata) {this->rdata = rdata;}
};
class VIBE_HELPER_DLL_EXPORT  VectorDataIn {
private:
    VectorData * vdata;
public:
    VectorData * getItem() {return vdata;}
    void setVectorData(VectorData * vdata) {this->vdata = vdata;}
};
}



#endif // PYMODULE_H
