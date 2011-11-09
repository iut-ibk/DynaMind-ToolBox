#include <DMcomponent.h>
#include <DMnode.h>

using namespace DM;

Node::Node(std::string name, std::string id, double x, double y, double z) : Component(name,id)
{
    this->x=x;
    this->y=y;
    this->z=z;
}

Node::Node(std::string name, std::string id) : Component(name, id)
{
    this->x=0;
    this->y=0;
    this->z=0;
}

Node::Node(const Node& n) : Component(n)
{
    x=n.x;
    y=n.y;
    z=n.z;
}

double Node::getX()
{
    return x;
}

double Node::getY()
{
    return y;
}

double Node::getZ()
{
    return z;
}

void Node::setX(double x)
{
    this->x=x;
}

void Node::setY(double y)
{
    this->y=y;
}

void Node::setZ(double z)
{
    this->z=z;
}

Component* Node::clone()
{
    return new Node(*this);
}
