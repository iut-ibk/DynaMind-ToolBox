#ifndef SYSTEM_H
#define SYSTEM_H

#include <map>
#include <vector>

namespace DM {

    class Component;
    class Node;
    class Edge;

    class System : public Component
    {
    private:
        std::map<std::string, Node* > nodes;
        std::map<std::string, Edge* > edges;
        std::map<std::string, System*> subsystems;

    public:
        System(std::string name, std::string id);
        System(const System& s);
        ~System();

        bool addNode(Node* node);
        bool addEdge(Edge* edge);
        bool addPredecessor(System* system);
        Node* getNode(std::string name);
        Edge* getEdge(std::string name);
        bool removeEdge(std::string name);
        bool removeNode(std::string name);
        bool addPredecessorState(System* system);
        std::vector<System*> getPredecessorStates();
        bool addSubSystem(System *newsystem);
        bool removeSubSystem(std::string name);
        System* getSubSystem(std::string name);
        std::map<std::string, System*> getAllSubSystems();
        System* createSuccessor();
        Component* clone();
    };
}
#endif // SYSTEM_H
