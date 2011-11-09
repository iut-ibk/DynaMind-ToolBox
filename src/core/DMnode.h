#ifndef NODE_H
#define NODE_H

namespace DM {
    class Component;

    class Node : public Component
    {
    private:
        double x;
        double y;
        double z;

    public:
        Node(std::string name, std::string id, double x, double y, double z);
        Node(std::string name, std::string id);
        Node(const Node& n);
        double getX();
        double getY();
        double getZ();
        void setX(double x);
        void setY(double y);
        void setZ(double z);
        Component* clone();
    };
}
#endif // NODE_H
