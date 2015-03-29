%module(directors="1", allprotected="1") pydynamind
%rename(addLinkAttribute) addAttribute(const std::string name, std::string linkName, ACCESS access);
%ignore "DM_HELPER_DLL_EXPORT";

%pythoncode %{
from osgeo import ogr
import tempfile
%}

//%feature("autodoc", "3");
%feature("director");
%{
    #include <dmsimulation.h>
    #include <dmcomponent.h>
    #include <dmsystem.h>
    #include <dmrasterdata.h>
    #include <dmattribute.h>
    #include <dmedge.h>
    #include <dmnode.h>
    #include <dmface.h>
    #include <dmmodule.h>
    #include <dmview.h>
    #include <dmnodefactory.h>
    #include <dmmoduleregistry.h>
    #include <dmlog.h>
    #include <dmlogger.h>
    #include <dmlogsink.h>
    #include <dmsimulation.h>
    #include <iostream>
    #include <dmgdalsystem.h>
    #include <dmviewcontainer.h>
	#include <ogr_api.h>

    using namespace std;
    using namespace DM;
%}

%include std_vector.i
%include std_string.i
%include std_map.i
%include std_list.i
%include cpointer.i

%include "../core/dmcomponent.h"
%include "../core/dmsystem.h"
%include "../core/dmattribute.h"
%include "../core/dmedge.h"
%include "../core/dmface.h"
%include "../core/dmrasterdata.h"
%include "../core/dmnode.h"
%include "../core/dmview.h"
%include "../core/dmlog.h"
%include "../core/dmlogger.h"
%include "../core/dmlogsink.h"
%include "../core/dmsimulation.h"

namespace std {
	%template(stringvector) vector<string>;
	%template(viewmap)  map<string, DM::View>;
	%template(doublevector) vector<double>;
	%template(systemvector) vector<DM::System* >;
	%template(systemmap) map<string, DM::System* >;
	%template(edgevector) vector<DM::Edge* >;
	%template(nodevector) vector<DM::Node* >;
	%template(facevector) vector<DM::Face* >;
	%template(viewvector) vector<DM::View >;
	%template(viewcontainervector) vector<DM::ViewContainer >;
	%template(viewcontainervector_p) vector<DM::ViewContainer*>;
	%template(viewpointermap) map<string, vector<DM::View* > >;
	%template(componentvector) vector<DM::Component* >;
	%template(attributevector) vector<DM::Attribute* >;
	%template(attributemap) map<string, DM::Attribute* >;
	%template(componentmap) map<string, DM::Component* >;
	%template(nodemap) map<string, DM::Node* >;
	%template(edgemap) map<string, DM::Edge* >;
	%template(facemap) map<string, DM::Face* >;
	%template(stringmap) map<string, string >;
	%template(modulelist) list<DM::Module* >;
	%template(filtervector) vector<DM::Filter>;
	%template(linkvector) list<DM::Link*>;
}

%pointer_class(std::string,p_string)
%pointer_class(int,p_int)
%pointer_class(long,p_long)
%pointer_class(double,p_double)

%feature("director:except") %{
	if ($error != NULL) {
		PyErr_Print();
	}
%}



enum DataTypes {
	INT,
	LONG,
	DOUBLE,
	STRING,
	FILENAME,
	STRING_LIST,
	STRING_MAP,
	BOOL,
};

enum PortType {
	INPORT,
	OUTPORT,
};

enum ModuleStatus
{
	MOD_UNTOUCHED,
	MOD_EXECUTION_OK,
	MOD_EXECUTING,
	MOD_EXECUTION_ERROR,
	MOD_CHECK_OK,
	MOD_CHECK_ERROR,
};

class DM::FilterArgument {
	public:
		FilterArgument(std::string argument);
};

class DM::Filter {
public:
	Filter();
	Filter(std::string viewName, DM::FilterArgument spatialFilter, DM::FilterArgument attributeFilter);
};

%feature("autodoc", "

    The DynaMind module class

") Module;
class DM::Module {

public:
	Module();
	virtual ~Module();
	virtual bool createInputDialog();

    virtual std::string getHelpUrl();

	void setFilter(std::vector<DM::Filter> filter);
	std::vector<DM::Filter> getFilter();

    %feature("autodoc", "setName(name)

    Sets module name

    :type name: str
    :param name: set module name

    ") setName;
	void setName(std::string name);

    %feature("autodoc", "getName()

    Gets module name

    :return: Module name
    :rtype: str

    ") getName;
	std::string getName();

    %feature("autodoc", "init()

    This method should be overwritten if the module modifies the data stream definition in response to
    parameter input or the incoming data stream.

    Outside the module, it calls the init method of the module. Usually the Simulation takes care of initialising modules.

    ") init;
	virtual void init();

    %feature("autodoc", "run()

    This method needs to be overwritten when developing a new module

    ") run;
	virtual void run() = 0;

	//std::map<std::string, std::map<std::string, DM::View> >  getViews() const;


    %feature("autodoc", "getParameterAsString(module_name)

    Gets module parameter as string

    :type module_name: str
    :param module_name: parameter name
    :return: parameter value as string
    :rtype: str

    ") getParameterAsString;
	virtual std::string getParameterAsString(std::string Name);

	std::map<std::string,DM::View> getViewsInStdStream() const;
	std::map<std::string,DM::View> getViewsOutStdStream() const;
	 std::map<std::string,DM::View> getAccessedStdViews() const;

	virtual const char* getClassName() const = 0;
	void addParameter(const std::string &name, const DataTypes type, void * ref, const std::string description = "");
	virtual void setParameterValue(std::string name, std::string value);

	std::string getUuid() const;
	std::vector<std::string> getInPortNames() const;
	std::vector<std::string> getOutPortNames() const;

	void setStatus(DM::ModuleStatus status);

protected:
	void addData(std::string name, std::vector<DM::View> view);

	void addGDALData(std::string name, std::vector<DM::ViewContainer> view);
	DM::System * getData(std::string dataname);
	DM::RasterData * getRasterData(std::string dataname, const DM::View & view);


	DM::GDALSystem * getGDALData(std::string dataname);

    %feature("autodoc", "setIsGDALModule(is_gdal_module)

    Set to True to activate the GDAL API

    :type is_gdal_module: Boolean
    :param is_gdal_module: True for GDAL API

    ") setIsGDALModule;
	void setIsGDALModule(bool b);
	bool isGdalModule();

    %feature("autodoc", "registerViewContainers(view_containers)

    Registers a list of view containers in the Module. This is used in the Constructor or in :func:`~pydynamind.Module.init`
    to register views to be used later in :func:`~pydynamind.Module.run`

    :type view_containers: [ :class:`~pydynamind.ViewContainer` ]
    :param view_containers: List of view containers

    ") registerViewContainers;
	void registerViewContainers(std::vector<DM::ViewContainer *> views);

};

%extend DM::Simulation {
	void registerModulesFromDirectory(const std::string dir) {
		$self->registerModulesFromDirectory(QDir(QString::fromStdString(dir)));
	}
}

%extend DM::Module {
    %feature("autodoc", "getGDALDBName()

    Gets GDAL database name of the current simulation used in this module

    :return: database name
    :rtype: string

    ") getGDALDBName;
	std::string getGDALDBName() {
		std::string outport="";
		if (outport == "")
			outport =  $self->getOutPortNames()[0];
		GDALSystem * sys =  (DM::GDALSystem*) $self->getOutPortData(outport);
		return sys->getDBID();
	}
	%feature("autodoc", "getParameterList()

			 Returns names of parameters in the module as list

			 :return: paramter list
			 :rtype: [str]

	") getParameterList;
	std::vector<std::string> getParameterList(){
			std::vector<DM::Module::Parameter*> parameters = $self->getParameters();
			std::vector<std::string> names;
			foreach(DM::Module::Parameter* p, parameters) {
					 names.push_back(p->name);
			}
			return names;
	}
	%feature("autodoc", "get_class_name()

	Returns real class name

	:return: class name
	:rtype: str

	") get_class_name;
	std::string get_class_name() {
		return $self->getClassName();
	}

	%pythoncode %{
	_data = {'d':'Module'}
	def getClassName(self):
		"""
		Returns class name. However if called in Python the it returns Module for C++ modules.
		To obtain the real class name please call :func:`~pydynamind.Module.get_class_name`

		:type description: str
		:param description: class name

		"""
		return self.__class__.__name__

	def getFileName(self):
		return self.__module__.split(".")[0]

	def __getattr__(self, name):
		if name in self._data:
			return self._data[name].value()

	def __setattr__(self, name, value):
		if name in self._data:
			return self._data[name].assign(value)

		return super(Module, self).__setattr__(name, value)

	def createParameter(self,name, DN_type, description = ""):
		"""

		Creates a new module parameter. This method is used to add new parameter to the module.
		This module should only be used in the Constructor of the module.
		The parameter can be access in the module using ``self.parameter_name = value``.
		To do this, :func:`~pydynamind.Module.createParameter` may be called before the parameter is accessed.

		:type name: str
		:param name: parameter name
		:type DN_type: :class:`~pydynamind.DataTypes`
		:param DN_type: DynaMind attribute. Following attributes are supported:

			- pydynamind.BOOL : Boolean
			- pydynamind.INT : Integer
			- pydynamind.DOUBLE : Float
			- pydynamind.STRING : String
			- pydynamind.FILENAME : File name

		:type description: str
		:param description: add parameter description

		"""
		if 'd' in self._data:
			if self._data['d'] == 'Module':
				self._data = {}

		if DN_type == STRING:
			self._data[name] = p_string()
		if DN_type == FILENAME:
			self._data[name] = p_string()
		if DN_type == DOUBLE:
			self._data[name] = p_double()
		if DN_type == LONG:
			 self._data[name] = p_long()
		if DN_type == INT:
			self._data[name] = p_int()
		if DN_type == BOOL:
			self._data[name] = p_int()

		self.addParameter(name,DN_type,self._data[name],description)

	%}
}

%inline %{

void log(std::string s, DM::LogLevel l) {
	DM::Logger(l) << s;
}


//Init Logger and DynaMind
void initDynaMind(DM::LogLevel loglevel){
//Init Logger
ostream *out = &cout;
DM::Log::init(new OStreamLogSink(*out), loglevel);
DM::Logger(DM::Debug) << "Start";
}

%}

class INodeFactory
{
	public:
		virtual ~INodeFactory(){}
		virtual DM::Module *createNode() const = 0;
		virtual std::string getNodeName() const = 0;
		virtual std::string getFileName() const = 0;
};

class ModuleRegistry
{
	public:
		ModuleRegistry();
		bool addNodeFactory(INodeFactory *factory);
		void addNativePlugin(const std::string &plugin_path);
		Module * createModule(const std::string & name) const;
		std::list<std::string> getRegisteredModules() const;
		bool contains(const std::string &name) const;
};

%feature("autodoc", "

Class to access the GIS data. The class utilises heavily the GDAL library.
Internally the view container links to a GDAL layer and provides a simple
wrapper to read, and create OGRFeatures. The Python/GDAL documentation can be
found `here <http://gdal.org/python/>`_

:type name: str
:param name: view name
:type type: pydynamind.TYPE
:param type: Defines component type. Following types are supported:

    - pydynamind.COMPONENT
    - pydynamind.NODE
    - pydynamind.EDGE
    - pydynamind.FACE

:type access_geometry:
:param access_geometry: Defines access to the geometry. Following types are supported:

    - pydynamind.READ
    - pydynamind.MODIFY
    - pydynamind.WRITE

") ViewContainer;

class DM::ViewContainer {
	public:
	    %feature("autodoc", "__init__(self, name, type, access_geometry) -> ViewContainer")  ViewContainer(string name, int type, ACCESS accesstypeGeometry);
	    ViewContainer(string name, int type, ACCESS accesstypeGeometry);
		ViewContainer();

		%feature("autodoc", "addAttribute(name, attribute_type, access)

        Add attribute access to the view container.

        :type name: str
        :param name: attribute name
        :type attribute_type: pydynamind.Attribute.TYPE
        :param attribute_type: Defines the attribute type. Following attributes are supported:

            - pydynamind.Attribute.INT
            - pydynamind.Attribute.DOUBLE
            - pydynamind.Attribute.STRING
            - pydynamind.Attribute.DOUBLEVECTOR

        :type access: pydynamind.ACCESS
        :param access: Defines access to the attribute. Following types are supported:

            - pydynamind.READ
            - pydynamind.MODIFY
            - pydynamind.WRITE

        ") addAttribute;
		void addAttribute(std::string name, Attribute::AttributeType type, ACCESS access);
		void setAttributeFilter(std::string filter);
		void setCurrentGDALSystem(DM::GDALSystem *sys);
		OGRFeatureDefnShadow * getFeatureDef();
		void registerFeature(OGRFeatureShadow *f, bool isNew);
		void syncAlteredFeatures();
		void createIndex(std::string attribute);
		std::string getDBID();
		virtual ~ViewContainer();
		std::string getName() const;
};

%extend DM::ViewContainer {
	%pythoncode %{
	#Container for OGRObjects, otherwise the garbage collector eats them

	__ds = {}
	__connection_counter = {}
	def create_feature(self):
		"""

        Creates and inserts a new feature into the view. Features created with this function are
        managed by DynaMind and do not need to be destroyed.

        :return: feature
        :rtype: OGRFeature

        """
		#from osgeo import ogr
		self.register_layer()
		f = ogr.Feature(self.getFeatureDef())
		self.registerFeature(f, True)
		#Hold Object until destroyed
		self.__features.append(f)
		return f

	def set_attribute_filter(self, attribute_filter):
		"""

        Sets attribute filter.
        Before setting an attribute filter please call :func:`~pydynamind.ViewContainer.reset`.
        Filters can be set using OGR SQL a documentation can be found `here <http://www.gdal.org/ogr_sql.html>`_
        To speed up the filtering please create an index first with  :func:`~pydynamind.ViewContainer.create_index`
        The Attribute filter may be used in combination with :func:`~pydynamind.ViewContainer.next`

        :type attribute_filter: str
        :param attribute_filter: Attribute filter using OGR SQL syntax

        """

		self.register_layer()
		self.__ogr_layer.SetAttributeFilter(attribute_filter)

	def create_index(self, attribute_name):
		"""

        Creates a fresh index for an attribute. This speeds up the search within this index
        dramatically.

        :type attribute_name: str
        :param attribute_name: attribute name on which the index should be created

        """

		self.createIndex(attribute_name)
		return

	def register_layer(self):
		try:
			self.__ogr_layer
		except:
			db_id = self.getDBID()
			self.__features = []
			if db_id not in self.__ds.keys():
				print "Register Datasource " + str(tempfile.gettempdir()+"/"+db_id+".db")
				self.__ds[db_id] = ogr.Open(tempfile.gettempdir()+"/"+db_id+".db")
				self.__connection_counter[db_id] = 0
			else:
				print "Reuse connection"
			table_name = str(self.getName())
			print "Register Layer " + str(table_name)
			self.__ogr_layer = self.__ds[db_id].GetLayerByName(table_name)
			self.__connection_counter[db_id]+=1

			if self.__ogr_layer == None:
				print "Layer registration failed"
				raise


	def __iter__(self):
		return self


	def get_ogr_layer(self):
		"""

        Returns the OGR layer. Only use if really really needed.

        :return: OGR Layer
        :rtype: OGRLayer

        """
		self.register_layer()
		return self.__ogr_layer

	def get_ogr_ds(self):
		"""

        Returns OGR data source. Only use if really really needed.

        :return: OGR data source
        :rtype: OGRDataSource

        """
		self.register_layer()
		return self.__ds[self.getDBID()]

	def get_linked_features(self, feature, link_id = ""):
		"""

        Sets a filter to easy access linked features. May be used in combination with :func:`~pydynamind.ViewContainer.next`
        For example: ``for feature in self.view_container.get_linked_features(feature_from_other_view):``
        :type feature: OGRFeature
        :param feature: OGRFeature linking to this view
        :return: this ViewContainer set to only return linked features
        :rtype: :class:`~pydynamind.ViewContainer`

        """
		self.reset_reading()
		link_name = feature.GetDefnRef().GetName()
		if link_id:
			self.set_attribute_filter(link_id+" = " + str(feature.GetFID()))
		else:
			self.set_attribute_filter(link_name+"_id = " + str(feature.GetFID()))
		return self


	def get_feature(self, fid):
		"""

        Returns feature with id.

        :type fid: int
        :param fid: feature id
        :return: feature
        :rtype: OGRFeature

        """
		self.register_layer()
		feature = self.__ogr_layer.GetFeature(fid)
		self.registerFeature(feature, False)
		self.__features.append(feature)
		return feature

	def next(self):
		"""

        Iterator to iterate over the features in the :class:`~pydynamind.ViewContainer` under consideration of the
        set filters. The method my not be directly called but enables easy access to iterate over features using the standard
        for loop. e.g. ``for feature in self.view_container:``

        Before iterating over the all features please call :func:`~pydynamind.ViewContainer.reset_reading`.


        :return: next feature
        :rtype: OGRFeature

        """
		self.register_layer()
		feature = self.__ogr_layer.GetNextFeature()
		if not feature:
			raise StopIteration
		else:
			self.registerFeature(feature, False)
			self.__features.append(feature)
			return feature

	def reset_reading(self):
		"""

        Resets iterator

        """
		self.register_layer()
		self.__ogr_layer.ResetReading()

	def sync(self):
		"""
        Synchronises the ViewContainer writing the data to the database and freeing the memory.
        May be used before the end of the run method.

        """
		self.syncAlteredFeatures()
		for f in self.__features:
			f.Destroy()
		del self.__features[:]


		#self.__ds[self.getDBID()].Destroy()

	def finalise(self):
		"""
		Synchronises the ViewContainer writing the data to the database and freeing the memory.
		May be used before the end of the run method.

		"""
		self.sync()
		print "Destroy Layer"
		print self.__connection_counter[self.getDBID()]
		if self.__connection_counter[self.getDBID()] == 1:
			print "Real Destroy Connection"
			del self.__ds[self.getDBID()]
			self.__ds = {}

		self.__connection_counter[self.getDBID()]-=1

	%}
}

%pythoncode %{
def my_del(self):
	#print "Force no delete of python garbage collector"
	self.__disown__()

Component.__del__ = my_del
Node.__del__ = my_del
Edge.__del__ = my_del
Face.__del__ = my_del

class NodeFactory(INodeFactory):
	def __init__(self, klass):
		INodeFactory.__init__(self)
		self.klass = klass

	def createNode(self):
		return self.klass().__disown__()

	def getNodeName(self):
		return self.klass.__name__

	def getFileName(self):
		return self.klass.__module__.split(".")[0]

def registerNodes(registry):
	for klass in Module.__subclasses__():
		registry.addNodeFactory(NodeFactory(klass).__disown__())

%}

%pythoncode %{

import pydynamind as DM
import uuid
class SIM_STATUS:
		RUNNING, OK = range(2)

class Sim:
		"""
		Pythonic wrapper of the DynaMind Simulation class.
		"""
		_sim = DM.Simulation
		_sim_status = SIM_STATUS.OK

		def __init__(self):
			"""
			Init Logger and register Modules from default location
			"""
			DM.initDynaMind(DM.Standard)
			self._sim = DM.Simulation()

		def register_modules(self, path):
			"""
			Registers Python or C++ DynaMind modules for the simulation.

			:type path: str
			:param path: path to module
			"""

			self._sim.registerModulesFromDirectory(path)

		def run(self):
			"""
			Runs simulation
			"""
			if self.is_busy():
				print "Can't run simulation. a simulation is currently running"
				return

			self._sim_status = SIM_STATUS.RUNNING
			self._sim.run()
			self._sim_status = SIM_STATUS.OK

		def is_busy(self):
			"""
			Check if currently a simulation is running

			:return: True if busy, False if not
			:rtype: Boolean
			"""

			if self._sim_status != SIM_STATUS.OK:
				return True
			return False

		def __add_module(self, class_name, module_name, parent=None):
			"""
			Add module to DynaMind

			:param class_name: Name of the module class
			:param module_name: Name of generated module
			:return: True if module was added to the simulation
			"""
			if self.is_busy():
				print "Can't add module. A simulation is currently running"
				return
			if parent:
				m = self._sim.addModule(class_name, parent)
			else:
				m = self._sim.addModule(class_name)
			if m == None:
				return False
			m.setName(module_name)
			return True

		def __dict_to_dm_string(self, dictionary):
			"""
			Convert python dictionary to DynaMind string
			:param dictionary:

			:return:
			"""
			dm_string = ""
			for k in dictionary.keys():
				dm_string += '*||*'
				dm_string += str(k)
				dm_string += '*|*'
				dm_string += str(dictionary[k])
			if dm_string is not "":
				dm_string += '*||*'
			return dm_string

		def __list_to_dm_string(self, list):
			"""
			Convert python list to DynaMind string

			:param dictionary:
			:return:
			"""
			dm_string = ""
			dm_string += "*|*"
			for e in list:
				dm_string += e
				dm_string += "*|*"
			return dm_string


		def add_module(self, class_name, parameter={}, connect_module=None, parent_group=None, filters={}, module_name=""):
			"""
			Add a new model or group to the simulation the python way.

			:type class_name: str
			:param class_name: class name of module. See documentation for available modules
			:type parameter: dict
			:param parameter: module parameter as dict. ``{'parameter name': parameter_value}``. Following parameter are supported

					- boolean: Boolean
					- int: Integer
					- float: Float and Doubles
					- str: String value
					- vector: String list ``['val1', 'val2', '...']``
					- dict: String map as dict ``{'key': 'value'}``

			:type connect_module: :class:`~pydynamind.Module`
			:param connect_module: upstream module that the new module is connected to
			:type parent_group: :class:`~pydynamind.Module`
			:param parent_group: parent group if the newly created module. If not set the module is added to the root group.
			:type filters: dict
			:param filters: set module filter ``{ 'view_name' : {'attribute': 'attribute filter' :  spatial: 'view name'} }``
			:type module_name: str
			:param module_name: alternative module name. This is useful to later find the module in the simulation
			:return: created module. If the creation fails, it returns None.
			:rtype: :class:`~pydynamind.Module`
			"""

			m_uuid = str(uuid.uuid4())
			if module_name:
				m_uuid = module_name
			success = self.__add_module(class_name, m_uuid, parent_group)
			if not success:
				print "Adding module " + str(class_name) + " failed"
				return None

			m = self.get_module_by_name(m_uuid)

			self._set_module_parameter(m, parameter)

			if connect_module:
				m.init()
				self.link_modules(connect_module, m)
			filters_list = []
			for f in filters.keys():
				filter = filters[f]
				attribute_filter = ""
				spatial_filter = ""
				if "attribute" in filter:
					attribute_filter = filter["attribute"]
				if "spatial" in filter:
					spatial_filter = filter["spatial"]
					filters_list.append(DM.Filter(f, DM.FilterArgument(str(attribute_filter)), DM.FilterArgument(str(spatial_filter))))
			m.setFilter(filters_list)
			m.init()

			return m

		def set_modules_parameter(self, model_parameter_dict={}):
			"""
			Sets parameters and filters for a whole bunch of modules stored in a dict. See :func:`~pydynamind.Sim.set_module_parameter` for how to define a
			module parameter set.

			:type filters: dict
			:param model_parameter_dict: dict of modules including module parameter ``{'module name': module description, ...}``
			"""
			for module_parameter in model_parameter_dict.iteritems():
				# Iterate over every module
				self.set_module_parameter(module_parameter)

		def set_module_parameter(self, module_description=()):
			"""
			:deprecated: 0.7

			Set model parameter, this method will be removed. Please use :func:`~pydynamind.Sim.set_modules_parameter`

			:type: tuple
			:param module_description: the module description is a tuple with (module name, parameter list)
			the parameter list is a dict that may contain following key word:

				- parameter: ``{'parameter name': parameter_value,...}``
				- filter: ``{ 'view_name' : {'attribute': 'attribute filter' :  spatial: 'view name'} }``

			"""

			module_name, parameters = module_description

			#get modules
			m = self.get_module_by_name(module_name)
			if "parameter" in parameters:
				self._set_module_parameter(m, parameters["parameter"])
			if "filter" in parameters:
				filter = parameters["filter"]
				attribute_filter = ""
				spatial_filter = ""
				if "attribute" in filter:
					attribute_filter = filter["attribute"]
				if "spatial" in filter:
					spatial_filter = filter["spatial"]
				filters = [DM.Filter("", DM.FilterArgument(str(attribute_filter)), DM.FilterArgument(str(spatial_filter)))]
				m.setFilter(filters)
				m.init()

		def _set_module_parameter(self, module, parameter={}):
			"""
			Set model parameter

			:param name: name of the module
			:param parameter: module parameter list

			:return:
			"""
			for k in parameter.keys():
				val = parameter[k]
				if type(val) is dict:
					val = self.__dict_to_dm_string(val)
				if type(val) is list:
					val = self.__list_to_dm_string(val)
				if type(val) is bool:
					if val == True:
						val = "1"
					else:
						val = "0"
				if type(val) is float:
					val = str(val)
				if type(val) is int:
					val = str(val)
				module.setParameterValue(str(k), str(val))


		def remove_module(self, module_name):
			"""
			Removes a module from the simulation

			:type module_name: str
			:param module_name: module name
			"""
			if self.is_busy():
				print "Can't remove module. A simulation is currently running"
				return
			modules = self._sim.getModules()
			modules_map = {}
			for m in modules:
				modules_map[m.getName()] = m
			try:
				module = modules_map[module_name]
			except KeyError:
				print "Module " + str(module_name) + " not found"
				return

			self._sim.removeModule(module)

		def set_epsg_code(self, epsg_code):
			"""
			Sets EPGS code of simulation

			:type module_name: int
			:param epsg_code: EPSG code as integer
			"""
			sim_config = DM.SimulationConfig()
			sim_config.setCoordinateSystem(epsg_code)

			self._sim.setSimulationConfig(sim_config)

		def load_simulation(self, filename):
			"""
			Loads DynaMind simulation

			:type filename: str
			:param filename: Name of the .dyn file to load
			"""
			if self.is_busy():
				print "Can't load simulation. A simulation is currently running"
				return

			self._sim_status = SIM_STATUS.RUNNING
			self._sim.loadSimulation(str(filename))
			self._sim_status = SIM_STATUS.OK

		def get_module_by_name(self, name):
			"""
			Returns module by name, be careful names are not unique! If the module can not be found an exception will be raised.

			:type name: str
			:param name: name of the module
			:return: module
			:rtype: :class:`~pydynamind.Module`
			"""
			modules = self._sim.getModules()
			modules_map = {}
			for m in modules:
				modules_map[m.getName()] = m
			try:
				m = modules_map[name]
			except KeyError:
				raise Exception("No Module with the name " + str(name))
			return m

		def get_module(self, module_id):
			"""
			Finds and returns a module in simulation. The module_id can be get from :func:`~pydynamind.Sim.map_of_loaded_modules`.

			:type module_id: str
			:param module_id: unique id of the module
			:return: module
			:rtype: :class:`~pydynamind.Module`
			"""
			modules = self._sim.getModules()
			modules_map = {}
			for m in modules:
				modules_map[m.getUuid()] = m
			m = modules_map[module_id]
			return m

		def add_link(self, m_source, outport_name, m_sink, inport_name):
			"""
			Links two modules using their port names. If modules have only one port also :func:`~pydynamind.Sim.link_modules`
			can be used.

			:type m_source: :class:`~pydynamind.Module`
			:param m_source: source module

			:type outport_name: str
			:param outport_name: name of the outport

			:type m_sink: :class:`~pydynamind.Module`
			:param m_sink: sink module

			:type inport_name: str
			:param inport_name: inport name
			"""
			if not self._sim.addLink(m_source, outport_name, m_sink, inport_name):
				raise Exception("Couldn't link module " + str(m_source.getName()) + " with " + str(m_sink.getName()))

		def link_modules(self, m_source, m_sink):
			"""
			Helper to make linking modules less tedious. Just works if the to connecting
			modules have just one in and outport (This is the case for most module). Otherwise throws exception

			:type m_source: dynamind.Module
			:param m_source: Module
			:type m_sink: dynamind.Module
			:param m_sink: Module
			"""

			inports = m_sink.getInPortNames()
			outports = m_source.getOutPortNames()

			if len(inports) == 0 or len(outports) == 0:
				 raise Exception("Module has no out ports. Couldn't link module " + str(m_source.getName()) + " with " + str(m_sink.getName()))

			self.add_link(m_source, outports[0], m_sink, inports[0])

		def get_links(self):
			"""
			Returns all links in simulation as vector

			:return: vector of links
			:rtype: [:class:`~pydynamind.Link`]
			"""
			return self._sim.getLinks()

		def clear(self):
			"""
			Removes all modules, groups and links from the simulation
			"""
			if self.is_busy():
				print "Can't reset simulation. a simulation is currently running"
				return
			self._sim_status = SIM_STATUS.RUNNING
			self._sim.clear()
			self._sim_status = SIM_STATUS.OK

		def reset(self):
			"""
			Resets the data stream of the simulation.
			"""
			if self.is_busy():
				print "Can't reset simulation. a simulation is currently running"
				return
			self._sim_status = SIM_STATUS.RUNNING
			self._sim.reset()
			self._sim_status = SIM_STATUS.OK

		def map_of_loaded_modules(self):
			"""
			Returns a map of the modules in the simulation.

			:return: ``{'module_id' : 'module name',...}``
			:rtype: dict
			"""
			modulemap = {}
			modules = self._sim.getModules()

			for m in modules:
				modulemap[m.getUuid()] = m.getName()
			return modulemap


		def serialise(self):
			"""
			Serialise simulation

			:return: Returns simulation as xml string
			:rtype: str
			"""
			return self._sim.serialise()

		def write_simulation_file(self, filename):
			"""
			Writes simulation to file

			:type filename: str
			:param filename: name of the file
			"""
			self._sim.writeSimulation(filename)

		def execute(self, dynamind_model, epsg_code, parameter_set):
			"""
			Execute a dynamind model. The dynamind model is added to the simulation and parameters are set before the simulation is executed

			:type dynamind_model: str
			:param dynamind_model: dynamind model as string. See :func:`~pydynamind.Sim.serialise`
			:type epsg_code: int
			:param epsg_code: EPSG code
			:type parameter_set: dict
			:param parameter_set: set of modules parameters for the definition of the dict please see :func:`~pydynamind.Sim.set_modules_parameter`
			"""

			self.clear()
			self.set_epsg_code(epsg_code)

			simulation_file = tempfile.gettempdir() + str(uuid.uuid4())+".dyn"

			#Create temp simulation file and load sim
			sim_file = open(simulation_file, "w")
			sim_file.write(dynamind_model)
			sim_file.close()

			self.load_simulation(simulation_file)
			#Can now be removed
			os.remove(simulation_file)

			self.set_modules_parameter(parameter_set)
			print self.serialise()
			self.run()
%}

