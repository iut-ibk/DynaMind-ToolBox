%module(directors="1", allprotected="1") pydynamind
%rename(addLinkAttribute) addAttribute(const std::string name, std::string linkName, ACCESS access);
%ignore "DM_HELPER_DLL_EXPORT";

%pythoncode %{
from osgeo import ogr
%}

%feature("autodoc", "1");
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
	%template(doublevector) vector<double>;
	%template(systemvector) vector<DM::System* >;
	%template(systemmap) map<string, DM::System* >;
	%template(edgevector) vector<DM::Edge* >;
	%template(nodevector) vector<DM::Node* >;
	%template(facevector) vector<DM::Face* >;
	%template(viewvector) vector<DM::View >;
	%template(viewcontainervector) vector<DM::ViewContainer >;
	%template(viewcontainervector_p) vector<DM::ViewContainer*>;
	%template(viewmap) map<string, vector<DM::View* > >;
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
}

%pointer_class(std::string,p_string)
%pointer_class(int,p_int)
%pointer_class(long,p_long)
%pointer_class(double,p_double)

%feature("director:except") {
	if ($error != NULL) {
		PyErr_Print();
	}
}



enum  DataTypes {
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
	Filter(std::string viewName, DM::FilterArgument spatialFilter, DM::FilterArgument attributeFulter);
};

class DM::Module {

public:
	Module();
	virtual ~Module();
	virtual bool createInputDialog();
	virtual void run() = 0;
	virtual void init();
	virtual std::string getHelpUrl();

	void setFilter(std::vector<DM::Filter> filter);
	std::vector<DM::Filter> getFilter();

	void setName(std::string name);
	std::string getName();


	std::map<std::string, std::map<std::string, DM::View> >  getViews() const;

	virtual const char* getClassName() const = 0;

	virtual std::string getParameterAsString(std::string Name);

	void addParameter(const std::string &name, const DataTypes type, void * ref, const std::string description = "");
	virtual void setParameterValue(std::string name, std::string value);

	std::vector<std::string> getInPortNames() const;
	std::vector<std::string> getOutPortNames() const;

	void setStatus(DM::ModuleStatus status);

protected:
	void addData(std::string name, std::vector<DM::View> view);

	void addGDALData(std::string name, std::vector<DM::ViewContainer> view);
	DM::System * getData(std::string dataname);
	DM::RasterData * getRasterData(std::string dataname, const DM::View & view);


	DM::GDALSystem * getGDALData(std::string dataname);
	void setIsGDALModule(bool b);
	bool isGdalModule();
	void registerViewContainers(std::vector<DM::ViewContainer *> views);

};

%extend DM::Simulation {
	void registerModulesFromDirectory(const std::string dir) {
		$self->registerModulesFromDirectory(QDir(QString::fromStdString(dir)));
	}
}

%extend DM::Module {
	std::string getGdalDBName(std::string outport) {
		GDALSystem * sys =  (DM::GDALSystem*) $self->getOutPortData(outport);
		return sys->getDBID();
	}

	%pythoncode %{
	_data = {'d':'Module'}
	def getClassName(self):
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

	def createParameter(self,name, DN_type, description):
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

class DM::ViewContainer {
	public:
		ViewContainer();
		ViewContainer(string name, int type, ACCESS accesstypeGeometry);
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
	%pythoncode {
	#Container for OGRObejcts, otherwise the garbage collector eats them
	__features = None
	__ogr_layer = None
	__ds = None

	def create_feature(self):
		#from osgeo import ogr
		self.register_layer()
		f = ogr.Feature(self.getFeatureDef())
		self.registerFeature(f, True)
		#Hold Object until destroyed
		self.__features.append(f)
		return f

	def set_attribute_filter(self, attribute_filter):
		self.register_layer()
		self.__ogr_layer.SetAttributeFilter(attribute_filter)

	def create_index(self, attribute_name):
		self.createIndex(attribute_name)
		return
		db_id = self.getDBID()
		ds = ogr.Open("/tmp/"+db_id+".db", True)
		result = ds.ExecuteSQL("CREATE INDEX "+ attribute_name +"_index ON " + self.getName() + " (" + attribute_name + ");")
		ds.Destroy()

	def register_layer(self):
		if self.__ogr_layer:
			return
		db_id = self.getDBID()
		self.__features = []
		self.__ds = ogr.Open("/tmp/"+db_id+".db")
		table_name = str(self.getName())
		self.__ogr_layer = self.__ds.GetLayerByName(table_name)

	def __iter__(self):
		return self

	def get_linked_features(self, feature, link_id = ""):
		self.reset_reading()
		link_name = feature.GetDefnRef().GetName()
		if link_id:
			self.set_attribute_filter(link_id+" = " + str(feature.GetFID()))
		else:
			self.set_attribute_filter(link_name+"_id = " + str(feature.GetFID()))
		return self

	def get_feature(self, fid):
		self.register_layer()
		feature = self.__ogr_layer.GetFeature(fid)
		self.registerFeature(feature, False)
		self.__features.append(feature)
		return feature

	def next(self):
		self.register_layer()
		feature = self.__ogr_layer.GetNextFeature()
		if not feature:
			raise StopIteration
		else:
			self.registerFeature(feature, False)
			self.__features.append(feature)
			return feature

	def reset_reading(self):
		self.register_layer()
		self.__ogr_layer.ResetReading()

	def sync(self):
		self.syncAlteredFeatures()
		for f in self.__features:
			f.Destroy()
		del self.__features[:]
	}
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
		Wrapper of the DynaMind Simulation class. Although DyanMind provides an almost complete wrapper for the Simulation class
		this class provides functionality used in the dance_platform web application. The class is currently implemented as Singelton to make it
		easier to use in the flask environment.

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
			Register dynamind module
			"""
			print "Register " + path
			self._sim.registerModulesFromDirectory(path)
		def run(self):
			"""
			Start simulation
			"""
			if self.is_busy():
				print "Can't run simulation. a simulation is currently running"
				return

			self._sim_status = SIM_STATUS.RUNNING
			self._sim.run()
			self._sim_status = SIM_STATUS.OK

		def is_busy(self):
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


		def add_module(self, class_name, parameter={}, connect_module=None, parent_group=None, module_name=""):
			"""
			Add model the python way
			:param class_name:
			:param parameter:
			:param connect_module:
			:param parent_group:
			:return:
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
			m.init()

			return m

		def set_modules_parameter(self, model_parameter_dict={}):
			"""
			Sets parameter for a whole bunch of modules stored in a dict. See set_module_parameter for how to define a
			module parameter set
			:param model_parameter_dict: dict of modules including module parameter
			:return:
			"""
			for module_parameter in model_parameter_dict.iteritems():
				# Iterate over every module
				self.set_module_parameter(module_parameter)

		def set_module_parameter(self, module_description=()):
			"""
			Set model parameter
			:param module_description: the module description is a tuple with (module name, parameter list)
			the parameter list is a dict that may contain following key word:
			- parameter: {parameter name in module, parameter value}
			- filter: {filter name, filter definition}
			:return: nothing if everything was fine
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
			Remove module from Simulation
			:param module_name:
			:return:
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
			Set EPGS code of simulation
			:param epsg_code:
			:return:
			"""
			sim_config = DM.SimulationConfig()
			sim_config.setCoordinateSystem(epsg_code)

			self._sim.setSimulationConfig(sim_config)

		def load_simulation(self, filename):
			"""
			Load dynamind simulation
			:param filename: Name of the .dyn file to load
			"""
			if self.is_busy():
				print "Can't load simulation. A simulation is currently running"
				return
			print self._sim
			print filename

			self._sim_status = SIM_STATUS.RUNNING
			self._sim.loadSimulation(str(filename))
			self._sim_status = SIM_STATUS.OK

		def get_module_by_name(self, name):
			"""
			Return module by name, be careful names are not unique! IF a mo
			:param name: name of the module
			:return:
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
			Finds and returns module in simulation
			:param module_id: unique id of the module
			:return: module
			"""
			modules = self._sim.getModules()
			modules_map = {}
			for m in modules:
				modules_map[m.getUuid()] = m
			m = modules_map[module_id]
			return m

		def add_link(self, m_source, outport_name, m_sink, inport_name):
			"""
			Link two modules using their port names. If modules have only one port also link_modules
			can be used.
			:param m_source:
			:param outport_name:
			:param m_sink:
			:param inport_name:
			:return:
			"""
			if not self._sim.addLink(m_source, outport_name, m_sink, inport_name):
				raise Exception("Couldn't link module " + str(m_source.getName()) + " with " + str(m_sink.getName()))

		def link_modules(self, m_source, m_sink):
			"""
			Helper Class to make inking modules less tedious. Just works with if
			module has one source and one sink. Otherwise troughs exception
			:param m_source: Module
			:param m_sink: Module
			:return: nothing
			"""
			inports = m_sink.getInPortNames()
			outports = m_source.getOutPortNames()

			if len(inports) == 0 or len(outports) == 0:
				 raise Exception("Module has no out ports. Couldn't link module " + str(m_source.getName()) + " with " + str(m_sink.getName()))

			self.add_link(m_source, outports[0], m_sink, inports[0])

		def get_links(self):
			"""
			returns a list of links
			:return:
			"""
			return self._sim.getLinks()

		def clear(self):
			"""
			Clear simulation

			removes all modules from the simulation
			"""
			if self.is_busy():
				print "Can't reset simulation. a simulation is currently running"
				return
			self._sim_status = SIM_STATUS.RUNNING
			self._sim.clear()
			self._sim_status = SIM_STATUS.OK

		def reset(self):
			"""
			Reset simulation

			resets current simulation
			"""
			if self.is_busy():
				print "Can't reset simulation. a simulation is currently running"
				return
			self._sim_status = SIM_STATUS.RUNNING
			self._sim.reset()
			self._sim_status = SIM_STATUS.OK

		def map_of_loaded_modules(self):
			"""
			Return map of the modules in the simulation
			:return: [key|name]
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
			"""
			return self._sim.serialise()

		def write_simulation_file(self, filename):
			"""
			Write simulation to file
			:param filename: name of the file
			"""
			self._sim.writeSimulation(filename)

		def execute(self, dynamind_model, epsg_code, parameter_set):

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

