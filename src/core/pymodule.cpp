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
#include "pymodule.h"

#include <QLibrary>
#include <iostream>
#include <boost/python.hpp>

#include <QDir>
#include <QThread>
#include <boost/foreach.hpp>
#include "module.h"
#include "rasterdata.h"
#include "vectordata.h"

#include <vector>

#include <pythonenv.h>
#include "wrapperhelper.h"
#include <vibe_log.h>
#include <vibe_logger.h>
#include <vibe_logsink.h>

using namespace boost::python;
using namespace boost;
using namespace std;

namespace vibens {




ModuleWrapper::ModuleWrapper(){

}
ModuleWrapper::~ModuleWrapper() {
    this->Destructor();
}

void ModuleWrapper::run() {
    ScopedGILRelease scoped;
    this->updateParameter();
    try {

        this->get_override("run")();
    } catch(error_already_set const &) {
        // handle the exception in some way
        PyErr_Print();
        Logger(Error) << __FILE__ << ":" << __LINE__;
        boost::python::object sys(
                    boost::python::handle<>(PyImport_ImportModule("sys"))
                    );
        boost::python::object err = sys.attr("stderr");
        std::string err_text = boost::python::extract<std::string>(err.attr("getvalue")());


        Logger(Error) << err_text;
        QThread::currentThread()->exit();
    }

}
bool ModuleWrapper::createInputDialog() {
    ScopedGILRelease scoped;
    try {

        return this->get_override("createInputDialog")();
    } catch(error_already_set const &) {
        // handle the exception in some way
        PyErr_Print();
        //Logger(Error) << error;
        Logger(Error) << __FILE__ << ":" << __LINE__ ;
        boost::python::object sys(
                    boost::python::handle<>(PyImport_ImportModule("sys"))
                    );
        boost::python::object err = sys.attr("stderr");
        std::string err_text = boost::python::extract<std::string>(err.attr("getvalue")());

        Logger(Error) << err_text;
        //QThread::currentThread()->exit();

    }
    return false;
}
void ModuleWrapper::setSelf(boost::python::object self) {
    this->self = self;
}


void ModuleWrapper::init(const unordered_map<std::string, std::string> &parameters) {
    ScopedGILRelease scoped;
    this->paramRaw = parameters;
    try {
        override init = this->get_override("init");
        if (init) {

            dict d;
            for (parameter_type::const_iterator it = parameters.begin();
                 it != parameters.end();
                 it++) {
                d[it->first] = it->second;
            }
            init(d);
        } else {
        }
    } catch(error_already_set const &) {
        // handle the exception in some way
        PyErr_Print();
        boost::python::object sys(
                    boost::python::handle<>(PyImport_ImportModule("sys"))
                    );
        boost::python::object err = sys.attr("stderr");
        std::string err_text = boost::python::extract<std::string>(err.attr("getvalue")());

        Logger(Error) << err_text ;
        QThread::currentThread()->exit();
    }
    //Module::init(parameters);
}
const char *ModuleWrapper::getClassName()  {
    return class_name;
}
const char *ModuleWrapper::getFileName()  {
    return file_name;
}
void ModuleWrapper::setClassName(string class_name) {
    this->class_name = strdup(class_name.c_str());
}
void ModuleWrapper::setFileName(string file_name) {
    this->file_name= strdup(file_name.c_str());
}
 void ModuleWrapper::addParameter(boost::python::object self,std::string Name, int Type) {
     this->addParameter_all(self, Name, Type, "");
 }

void ModuleWrapper::addParameter_all(python::object self,std::string Name, int Type, std::string description) {
    ScopedGILRelease scoped;
    void * ref;
    python::dict param = python::extract<python::dict>(self.attr("__dict__"));
    if (Type == VIBe2::DOUBLE) {

        python::extract<double> dx(param[Name]);
        double val = dx;
        double_params[Name] = val;
        ref = new double(val);
    }
    if (Type == VIBe2::LONG) {
        python::extract<int> ix(param[Name]);
        long val = ix;
        long_params[Name] = val;
        ref = new long(val);
    }
    if (Type == VIBe2::BOOL) {
        python::extract<bool> ix(param[Name]);
        bool val = ix;
        bool_params[Name] = val;
        ref = new bool(val);
    }
    if (Type == VIBe2::STRING || Type == VIBe2::FILENAME) {
        python::extract<string> ix(param[Name]);
        std::string val = ix;
        string_params[Name] = val;
        ref = new std::string(val);
    }
    if (Type == VIBe2::STRING_MAP) {
        python::extract<std::map<std::string, std::string> > ix(param[Name]);
        std::map<std::string, std::string> val = ix;
        string_maps[Name] = val;
        ref = new std::map<std::string,std::string>(val);
    }
    if (Type == VIBe2::USER_DEFINED_RASTERDATA_IN) {
        python::extract<RasterDataMap> map(param[Name]);
        RasterDataMap rmap= map;
        std::map<std::string,RasterData*> val = rmap.getRasterData();
        user_defined_rasterdata_in[Name] = val;
        ref = new std::map<std::string,RasterData*> (val);

    }
    if (Type == VIBe2::USER_DEFINED_VECTORDATA_IN) {
        python::extract<VectorDataMap> map(param[Name]);
        VectorDataMap vmap= map;
        std::map<std::string,VectorData*> val = vmap.getVectorData();
        user_defined_vectordata_in[Name] = val;
        ref = new std::map<std::string,VectorData*> (val);

    }

    if (Type == VIBe2::RASTERDATA_OUT) {
        rasterdata_out.push_back(Name);
        //this->addPort(Name, VIBe2::OUTRASTER);
    }
    if (Type == VIBe2::RASTERDATA_IN) {
        rasterdata_in.push_back(Name);
        //this->addPort(Name, VIBe2::INRASTER);
    }
    if (Type == VIBe2::VECTORDATA_OUT) {
        vectordata_out.push_back(Name);
    }
    if (Type == VIBe2::VECTORDATA_IN) {
        vectordata_in.push_back(Name);
    }
    if (Type == VIBe2::DOUBLEDATA_OUT) {
        doubledata_out.push_back(Name);
    }
    if (Type == VIBe2::DOUBLEDATA_IN) {
        doubledata_in.push_back(Name);
    }

    Module::addParameter(Name, Type, ref, description);
}
void ModuleWrapper::setParameter() {
    ScopedGILRelease scoped;
    python::dict param = python::extract<python::dict>(self.attr("__dict__"));
    BOOST_FOREACH(string s, rasterdata_out) {
        RasterData r;
        this->setRasterData(s,r);

    }
    BOOST_FOREACH(string s, vectordata_out) {
        VectorData v;
        this->setVectorData(s, v);

    }
    BOOST_FOREACH(string s,doubledata_out) {
        python::extract<double> dx(param[s]);
        this->setDoubleData(s, dx);
    }
    this->internalCounter++;
}

string ModuleWrapper::returnParameterValue(string name) {
    string val;
    if (paramRaw.find(name) != paramRaw.end())
        val = paramRaw.at(name);
    QString ss = "DoubleIn_" + QString::fromStdString(name);
    if (paramRaw.find(ss.toStdString()) != paramRaw.end()) {
        Logger(Debug) << "Double Data From OutSide " << ss.toStdString();
        std::string name = paramRaw.at(ss.toStdString());
        val = QString::number(this->getDoubleData(name)).toStdString();
    }


    return val;
}
bool ModuleWrapper::isFromOutSide(std::string name) {

    for (std::map<std::string, double>::iterator it = this->InputDoubleData.begin(); it != InputDoubleData.end(); ++it ){
        std::string n = it->first;
        stringstream ss;

        ss << "DoubleIn_" << name;
        Logger(Debug) << ss.str();
        if (n.compare(ss.str()) == 0) {
            return true;
        }
    }
    return false;

}

void ModuleWrapper::updateParameter() {

    ScopedGILRelease scoped;
    typedef pair<std::string, double> dbtp;
    typedef pair<std::string, long> inttp;
    typedef pair<std::string, std::string> snttp;
    typedef pair<std::string, map<std::string, std::string> > msnttp;
    try {
        python::dict self_dict = python::extract<python::dict>(self.attr("__dict__"));

        BOOST_FOREACH(dbtp i, double_params) {
            //Check if Parameter is from Outside
            std::string name = i.first;

            if (!isFromOutSide(name)) {
                self_dict[i.first] = this->getParameter<double>(name);
            } else {
                stringstream ss;
                ss << "DoubleIn_" << name;
                self_dict[i.first] = (double) this->getDoubleData(ss.str());
                Logger(Debug) << "From Outside " << ss.str();
            }

        }
        BOOST_FOREACH(inttp i, long_params) {
            std::string name = i.first;
            if (!isFromOutSide(name)) {
                self_dict[i.first] = this->getParameter<long>(i.first);
            } else {
                stringstream ss;
                ss << "DoubleIn_" << name;
                self_dict[i.first] = (long) this->getDoubleData(ss.str());
                Logger(Debug) << "From Outside " << ss.str();

            }
        }
        BOOST_FOREACH(inttp i, bool_params) {
            self_dict[i.first] = this->getParameter<bool>(i.first);
        }
        BOOST_FOREACH(snttp i, string_params) {
            self_dict[i.first] = this->getParameter<std::string>(i.first);
        }
        BOOST_FOREACH(msnttp i, string_maps) {
            self_dict[i.first] = this->getParameter<std::map<std::string, std::string> >(i.first);
        }
        BOOST_FOREACH(string name, rasterdata_in) {
            try {
                this->getRasterData(name);
            } catch (...) {
                Logger(Debug) << "No RasterData for Port ";
            }
            int error = self_dict.has_key(name);
            if (self_dict.has_key(name)) {
                RasterDataIn  rin;
                rin.setRasterData(& this->getRasterData(name));

                self_dict[name] = rin;
            } else {
                Logger(Debug)  << "No Parameter for ";
            }

        }
        BOOST_FOREACH(string s,   doubledata_in) {

            self_dict[s] =  this->getDoubleData(s);
        }
        BOOST_FOREACH(string name, vectordata_in) {
            try {
                this->getVectorData(name);
            } catch (...) {
                Logger(Debug) << "No Vector for Port " << name ;
            }
            int error = self_dict.has_key(name);
            if (self_dict.has_key(name)) {
                VectorDataIn  rin;
                rin.setVectorData(& this->getVectorData(name));

                self_dict[name] = rin;
            } else {
                Logger(Debug)  << "No Parameter for " << name ;
            }

        }
        BOOST_FOREACH(string name, rasterdata_out) {
            try {
                this->getRasterData_Write(name);
            } catch (...) {
                Logger(Debug)  << "No RasterData for Port " << name ;
            }
            int error = self_dict.has_key(name);
            if (self_dict.has_key(name)) {
                RasterDataIn  rin;
                rin.setRasterData(& this->getRasterData_Write(name));

                self_dict[name] = rin;
            } else {
                Logger(Debug)  << "No Parameter for " << name ;
            }

        }
        BOOST_FOREACH(string name, vectordata_out) {
            try {
                this->getVectorData_Write(name);
            } catch (...) {
                Logger(Debug)  << "No VectorData for Port " << name;
            }
            int error = self_dict.has_key(name);
            if (self_dict.has_key(name)) {
                VectorDataIn  rin;
                rin.setVectorData(& this->getVectorData_Write(name));

                self_dict[name] = rin;
            } else {
                Logger(Debug)  << "No Parameter for " << name;
            }

        }
        for(map<string, RASTERDATA_MAP>::const_iterator it=user_defined_rasterdata_in.begin(); it!=user_defined_rasterdata_in.end(); ++it) {
            std::string name = it->first;
            map<string, RasterData*> val= this->getParameter< map<string, RasterData*>  >(name);

            RasterDataMap rdataMap;


            for(map<string, RasterData*>::const_iterator it1=val.begin(); it1!=val.end(); ++it1) {

                std::string s1 = it1->first;
                QString param_name =  QString::fromStdString(it1->first);
                val[s1] = &this->getRasterData(param_name.toStdString());
            }
            rdataMap.Name = name;
            rdataMap.setRasterData(val);
            self_dict[it->first] = rdataMap;

        }
        for(map<string, VECTORDATA_MAP>::const_iterator it=user_defined_vectordata_in.begin(); it!=user_defined_vectordata_in.end(); ++it) {
            std::string name = it->first;
            map<string, VectorData*> val= this->getParameter< map<string, VectorData*>  >(name);

            VectorDataMap vdataMap;


            for(map<string, VectorData*>::const_iterator it1=val.begin(); it1!=val.end(); ++it1) {

                std::string s1 = it1->first;
                QString param_name =  QString::fromStdString(it1->first);
                val[s1] = &this->getVectorData(param_name.toStdString());
            }
            vdataMap.Name = name;
            vdataMap.setVectorData(val);
            self_dict[it->first] = vdataMap;

        }

    } catch(error_already_set const &) {
        // handle the exception in some way
        PyErr_Print();
        boost::python::object sys(
                    boost::python::handle<>(PyImport_ImportModule("sys"))
                    );
        boost::python::object err = sys.attr("stderr");
        std::string err_text = boost::python::extract<std::string>(err.attr("getvalue")());

        Logger(Error) << err_text;
        QThread::currentThread()->exit();
    }
}

void wrap_module() {
    class_<RasterDataMap>("RasterDataMap")
            .def("__len__", &RasterDataMap::size)
            .def("getItem", &RasterDataMap::getItem, return_internal_reference<>())
            .def("get", &RasterDataMap::get, return_internal_reference<>())
            .def("getNames", &RasterDataMap::getNames)
            ;
    class_<VectorDataMap>("VectorDataMap")
            .def("__len__", &VectorDataMap::size)
            .def("getItem", &VectorDataMap::getItem, return_internal_reference<>())
            .def("get", &VectorDataMap::get, return_internal_reference<>())
            .def("getNames", &VectorDataMap::getNames)
            ;
    class_<RasterDataIn>("RasterDataIn")
            .def("getItem", &RasterDataIn::getItem, return_internal_reference<>())
            ;
    class_<VectorDataIn>("VectorDataIn")
            .def("getItem", &VectorDataIn::getItem, return_internal_reference<>())
            ;


    enum_<VIBe2::DATATYPES>("VIBe2")
            .value("INT", VIBe2::INT)
            .value("LONG", VIBe2::LONG)
            .value("DOUBLE", VIBe2::DOUBLE)
            .value("STRING", VIBe2::STRING)
            .value("BOOL", VIBe2::BOOL)
            .value("FILENAME", VIBe2::FILENAME)
            .value("RASTERDATA_OUT", VIBe2::RASTERDATA_OUT)
            .value("RASTERDATA_IN", VIBe2::RASTERDATA_IN)
            .value("VECTORDATA_OUT", VIBe2::VECTORDATA_OUT)
            .value("VECTORDATA_IN", VIBe2::VECTORDATA_IN)
            .value("DOUBLEDATA_OUT", VIBe2::DOUBLEDATA_OUT)
            .value("DOUBLEDATA_IN", VIBe2::DOUBLEDATA_IN)
            .value("STRING_MAP", VIBe2::STRING_MAP)
            .value("USER_DEFINED_RASTERDATA_IN",  VIBe2::USER_DEFINED_RASTERDATA_IN)
            .value("USER_DEFINED_VECTORDATA_IN",  VIBe2::USER_DEFINED_VECTORDATA_IN)
            ;

    class_<std::vector<Face> >("FaceList")
            .def("__len__", &std::vector<Face>::size)
            .def("clear", &std::vector<Face>::clear)
            .def("append", &std_item<std::vector<Face> >::add,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__getitem__", &std_item<std::vector<Face> >::get,
                 return_value_policy<copy_non_const_reference>())
            .def("__setitem__", &std_item<std::vector<Face> >::set,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__delitem__", &std_item<std::vector<Face> >::del)
            ;
    class_<std::vector<long> >("LongList")
            .def("__len__", &std::vector<long>::size)
            .def("clear", &std::vector<long>::clear)
            .def("append", &std_item<std::vector<long> >::add,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__getitem__", &std_item<std::vector<long> >::get,
                 return_value_policy<copy_non_const_reference>())
            .def("__setitem__", &std_item<std::vector<long> >::set,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__delitem__", &std_item<std::vector<long> >::del)
            ;

    class_<RasterData>("RasterData", init<unsigned long, unsigned long, double>())
            .def("setValue", &RasterData::setValue)
            .def("getValue", &RasterData::getValue)
            .def("getWidth", &RasterData::getWidth)
            .def("getHeight", &RasterData::getHeight)
            .def("getCellSize", &RasterData::getCellSize)
            .def("setSize", &RasterData::setSize)
            ;
    class_<std::vector<std::string> >("NameList")
            .def("__len__", &std::vector<std::string>::size)
            .def("clear", &std::vector<std::string>::clear)
            .def("append", &std_item<std::vector<std::string> >::add,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__getitem__", &std_item<std::vector<std::string> >::get,
                 return_value_policy<copy_non_const_reference>())
            .def("__setitem__", &std_item<std::vector<std::string> >::set,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__delitem__", &std_item<std::vector<std::string> >::del)
            ;
    class_<std::vector<Point> >("PointList")
            .def("__len__", &std::vector<Point>::size)
            .def("clear", &std::vector<Point>::clear)
            .def("append", &std_item<std::vector<Point> >::add,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__getitem__", &std_item<std::vector<Point> >::get,
                 return_value_policy<copy_non_const_reference>())
            .def("__setitem__", &std_item<std::vector<Point> >::set,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__delitem__", &std_item<std::vector<Point> >::del)
            ;
    class_<std::vector<Edge> >("EdgeList")
            .def("__len__", &std::vector<Edge>::size)
            .def("clear", &std::vector<Edge>::clear)
            .def("append", &std_item<std::vector<Edge> >::add,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__getitem__", &std_item<std::vector<Edge> >::get,
                 return_value_policy<copy_non_const_reference>())
            .def("__setitem__", &std_item<std::vector<Edge> >::set,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__delitem__", &std_item<std::vector<Edge> >::del)
            ;
    class_<std::vector<double> >("DoubleAttributesList")
            .def("__len__", &std::vector<double>::size)
            .def("clear", &std::vector<double>::clear)
            .def("append", &std_item<std::vector<double> >::add,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__getitem__", &std_item<std::vector<double> >::get,
                 return_value_policy<copy_non_const_reference>())
            .def("__setitem__", &std_item<std::vector<double> >::set,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__delitem__", &std_item<std::vector<double> >::del)
            ;

    class_<std::vector<Link> >("LinkList")
            .def("__len__", &std::vector<Link>::size)
            .def("clear", &std::vector<Link>::clear)
            .def("append", &std_item<std::vector<Link> >::add,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__getitem__", &std_item<std::vector<Link> >::get,
                 return_value_policy<copy_non_const_reference>())
            .def("__setitem__", &std_item<std::vector<Link> >::set,
                 with_custodian_and_ward<1,2>()) // to let container keep value
            .def("__delitem__", &std_item<std::vector<Link> >::del)
            ;
    class_<std::map<std::string, std::string> >("StringMap")
            .def("__len__", &std::map<std::string, std::string>::size)
            .def("__setitem__", &map_item<std::string, std::string>::set)
            .def("__iter__"    , boost::python::iterator<std::map<std::string,std::string>, return_internal_reference<> >())
            .def("__getitem__", &map_item<std::string, std::string>::get,
                 return_value_policy<copy_non_const_reference>())
            .def("keys", &map_item<std::string, std::string>::keys)
        ;
    class_<VectorData>("VectorData", init<>())
            .def("getPoints", &VectorData::getPoints, return_value_policy<copy_const_reference>())
            .def("getEdges", &VectorData::getEdges, return_value_policy<copy_const_reference>())
            .def("getDoubleAttributes", &VectorData::getDoubleAttributes, return_value_policy<copy_const_reference>())
            .def("getAttributes", &VectorData::getAttributes, return_value_policy<copy_const_reference>())
            .def("getLinks", &VectorData::getLinks, return_value_policy<copy_const_reference>())
            .def("getFaces", &VectorData::getFaces, return_value_policy<copy_const_reference>())
            .def("getPointsNames", &VectorData::getPointsNames)
            .def("getEdgeNames", &VectorData::getEdgeNames, return_value_policy<copy_const_reference>())
            .def("getFaceNames", &VectorData::getFaceNames, return_value_policy<copy_const_reference>())
            .def("getLinkNames", &VectorData::getLinkNames, return_value_policy<copy_const_reference>())
            .def("getAttributeNames", &VectorData::getAttributeNames, return_value_policy<copy_const_reference>())            
            .def("setPoints", &VectorData::setPoints)
            .def("setEdges", &VectorData::setEdges)
            .def("setLinks", &VectorData::setLinks)
            .def("setAttributes", &VectorData::setAttributes)
            .def("setFaces", &VectorData::setFaces)
            .def("setDoubleAttributes", &VectorData::setDoubleAttributes)
            .def("addVectorData", &VectorData::addVectorData)
            .def("clean", &VectorData::clear)
            ;
    class_<Point>("Point", init<double, double, double>())
            .def("getX", &Point::getX)
            .def("getY", &Point::getY)
            .def("getZ", &Point::getZ)
            ;
    class_<Edge>("Edge", init<long, long>())
            .def("getID1", &Edge::getID1)
            .def("getID2", &Edge::getID2)
            ;
    class_<Face>("Face", init<std::vector<long> >())
            .def("getIDs", &Face::getIDs)
            ;
    class_<Link>("Link", init<std::string, std::string>())
            .def("getVectorDataName", &Link::getVectorDataName)
            .def("getID", &Link::getID)
            ;
    class_<Attribute>("Attribute", init<>())
            .def("getAttribute", &Attribute::getAttribute)
            .def("setAttribute", &Attribute::setAttribute_double)
            .def("setAttribute", &Attribute::setAttribute_string)
            .def("getAttributeNames", &Attribute::getAttributeNames)
            .def("getStringAttribute", &Attribute::getStringAttribute)
            ;
    class_<ModuleWrapper, ModuleWrapper *, boost::noncopyable >("Module")
            .def("run", pure_virtual(&Module::run))
            .def("addParameter", &ModuleWrapper::addParameter)
            .def("addParameter", &ModuleWrapper::addParameter_all)
            .def("addDescription", &Module::addDescription)
            .def("getParameterAsString", &Module::getParameterAsString)
            .def("setParameterValue" ,&Module::setParameterValue)
            .def("appendToUserDefinedParameter", &Module::appendToUserDefinedParameter)
            .def("init",  &ModuleWrapper::init)
            .def("getRasterData", &Module::getRasterData, return_internal_reference<>())
            .def("getVectorData", &Module::getVectorData, return_internal_reference<>())
            .def("setRasterData", &ModuleWrapper::setRasterData)
            .def("setVectorData", &Module::setVectorData)
            .def("getClassName", &Module::getClassName)
            .def("getID", &Module::getID)
            .def("updateParameter", &ModuleWrapper::updateParameter)
            .def("setParameter", &ModuleWrapper::setParameter)
            .def("addPort", &Module::addPort)
            .def("getInternalCounter", &Module::getInternalCounter)
            .def("createRasterData", &Module::createRasterData, return_internal_reference<>())
            .def("sendImageToResultViewer", &Module::sendImageToResultViewer)
            .def("addUrlToHelpFile", &Module::addUrlToHelpFile)
            ;


}



}
