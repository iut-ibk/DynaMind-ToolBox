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

    #include <simulation.h>

    using namespace std;
    using namespace DM;
    using namespace vibens;
%}

%include cpointer.i
%include std_vector.i
%include std_string.i
%include std_map.i
#include typemaps.i

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
    %template(nodemap) map<string, DM::Node* >;
    %template(edgemap) map<string, DM::Edge* >;
    %template(stringmap) map<string, string >;
}

class Module {

public:
    Module();
    virtual ~Module();
    virtual bool createInputDialog();
    virtual void run() = 0;

    virtual double getDoubleData(const std::string &name);
    virtual void setDoubleData(const std::string &name, const double r);

    void addData(std::string name, std::vector<DM::View> view);
    void * getData(std::string dataname);

    std::vector<std::string> getParameterListAsVector();
    virtual std::string getParameterAsString(std::string Name);

    virtual void updateParameter();

    void addParameter(std::string name, int type, void * ref, std::string description = "");
    virtual void setParameterValue(std::string name, std::string value);
    virtual void appendToUserDefinedParameter(std::string name, std::string  value);
    virtual void removeFromUserDefinedParameter(std::string name, std::string  value);


    void sendImageToResultViewer(std::string);
    void sendRasterDataToResultViewer(std::map<std::string , std::string > maps);
    void sendDoubleValueToPlot(double, double);

};

class INodeFactory
{
    public:
        virtual ~INodeFactory(){}
        virtual Module *createNode() const = 0;
        virtual std::string getNodeName() = 0;
        virtual std::string getFileName() = 0;
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
        return self.klass

def registerFunctions(registry):
    for klass in Module.__subclasses__():
        registry.registerFunction(NodeFactory(klass).__disown__())
%}
