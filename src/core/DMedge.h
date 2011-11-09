#ifndef EDGE_H
#define EDGE_H

namespace DM {

    class Component;

    class Edge : public Component
    {
    private:
        std::string start;
        std::string end;

    public:
        Edge(std::string name, std::string id, std::string startpoint, std::string endpoint);
        Edge(const Edge& e);
        std::string getStartpointName();
        std::string getEndpointName();
        Component* clone();
    };
}
#endif // Edge_H
