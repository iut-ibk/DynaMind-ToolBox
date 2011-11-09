#include <DMcomponent.h>
#include <DMedge.h>

using namespace DM;

Edge::Edge(std::string name, std::string id, std::string startpoint, std::string endpoint) : Component(name,id)
{
    start=startpoint;
    end=endpoint;
}

Edge::Edge(const Edge& e) : Component(e)
{
    start=e.start;
    end=e.end;
}

std::string Edge::getStartpointName()
{
    return start;
}

std::string Edge::getEndpointName()
{
    return end;
}

Component* Edge::clone()
{
    return new Edge(*this);
}

