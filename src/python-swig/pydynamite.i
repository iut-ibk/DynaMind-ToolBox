%module(directors="1", allprotected="1") pydynamite

%feature("director");
%{
    #include <DMcomponent.h>
    #include <DMsystem.h>
    #include <DMattribute.h>
    #include <DMedge.h>
    #include <DMnode.h>
    #include <module.h>
    #include <DMview.h>
    #include <nodefactory.h>
    #include <moduleregistry.h>
    #include <vibe_log.h>
    #include <vibe_logger.h>
    #include <vibe_logsink.h>
    #include <simulation.h>
    #include <iostream>

    using namespace std;
    using namespace DM;
    using namespace vibens;

%}

%include std_vector.i
%include std_string.i
%include std_map.i
%include cpointer.i

%include "../core/DMcomponent.h"
%include "../core/DMsystem.h"
%include "../core/DMattribute.h"
%include "../core/DMedge.h"
%include "../core/DMnode.h"
%include "../core/DMview.h"

namespace std {
    %template(stringvector) vector<string>;
    %template(doublevector) vector<double>;
    %template(systemvector) vector<DM::System* >;
    %template(systemmap) map<string, DM::System* >;
    %template(edgevector) vector<DM::Edge* >;
    %template(nodevector) vector<DM::Node* >;
    %template(viewvector) vector<DM::View >;
    %template(nodemap) map<string, DM::Node* >;
    %template(edgemap) map<string, DM::Edge* >;
    %template(stringmap) map<string, string >;
}

%pointer_class(std::string,p_string)
%pointer_class(int,p_int)
%pointer_class(double,p_double)

%feature("director:except") {
    if ($error != NULL) {
                PyErr_Print();
    }
}

class VIBe2 {
public:
    enum  DATATYPES {
        INT,
        LONG,
        DOUBLE,
        STRING,
        FILENAME,
        STRING_MAP,
        BOOL,
        LASTPRIMITIVETYPE,
        //Port Types
        USER_DEFINED_DOUBLEDATA_IN,
        USER_DEFINED_DOUBLEDATA_TUPLE_IN,
        USER_DEFINED_DOUBLEDATA_TUPLE_OUT,

        USER_DEFINED_INPUT,


        DOUBLEDATA_OUT,
        DOUBLEDATA_IN,

        SYSTEM_OUT,
        SYSTEM_IN,
        SYSTEM
    };

    enum PORTTYPES {
        OUTSYSTEM,
        OUTTUPLESYSTEM,
        OUTDOUBLEDATA,
        OUTTUPLEDOUBLEDATA,
        OUTPORTS,

        INSYSTEM,
        INTUPLEVSYSTEM,
        INDOUBLEDATA,
        INTUPLEDOUBLEDATA,
        INPORTS
    };

    enum CORINE {
        ContUrbanFabric = 2,
        DisContUrbanFabric = 3,
        RoadRailNetwork = 4,
        AgriculturalAreas = 5,
        ForestsSemiNatural = 6,
        WaterBodies = 7
    };
};



class Module {

public:
    Module();
    virtual ~Module();
    virtual bool createInputDialog();
    virtual void run() = 0;

    virtual double getDoubleData(const std::string &name);
    virtual void setDoubleData(const std::string &name, const double r);

    void addData(std::string name, std::vector<DM::View> view);
    DM::System * getData(std::string dataname);

    std::vector<std::string> getParameterListAsVector();
    virtual std::string getParameterAsString(std::string Name);

    virtual void updateParameter();

    void addParameter(std::string name, int type, void * ref, std::string description);

    virtual void setParameterValue(std::string name, std::string value);
    virtual void appendToUserDefinedParameter(std::string name, std::string  value);
    virtual void removeFromUserDefinedParameter(std::string name, std::string  value);


    void sendImageToResultViewer(std::string);
    void sendRasterDataToResultViewer(std::map<std::string , std::string > maps);
    void sendDoubleValueToPlot(double, double);

    virtual const char *getClassName();
    virtual const char *getFileName();

    %pythoncode %{
    def getClassName(self):
            return self.__class__.__name__

    def getFileName(self):
            return self.__module__.split(".")[0]

    %}


};

enum LogLevel {
        Debug = 0,
        Standard = 1,
        Warning = 2,
        Error = 3
};

%inline %{

void log(std::string s, LogLevel l) {
    Logger(l) << s;
}

void initlog(){
//Init Logger
ostream *out = &cout;
vibens::Log::init(new OStreamLogSink(*out), vibens::Debug);
vibens::Logger(vibens::Debug) << "Start";
}

%}

class INodeFactory
{
    public:
        virtual ~INodeFactory(){}
        virtual Module *createNode() const = 0;
        virtual std::string getNodeName() = 0;
        virtual std::string getFileName() = 0;
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

%pythoncode %{
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
