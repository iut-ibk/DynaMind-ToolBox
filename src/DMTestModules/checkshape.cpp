#include "checkshape.h"
#include <DMcomponent.h>
#include <DMsystem.h>
#include <DMattribute.h>
#include <DMnode.h>


VIBe_DECLARE_NODE_NAME( CheckShape,Modules )
CheckShape::CheckShape() {
    std::vector<DM::View> views;
    DM::View shape = DM::View("Shape");
    shape.getAttributes("Shapelist");

    views.push_back(shape);

    this->addData("KUMMSTDUNITREIN", views);
}



void CheckShape::run()
{
    sys_in = this->getData("KUMMSTDUNITREIN");

    DM::Attribute *attr = sys_in->getAttribute("Shapelist");

    if(!attr)
    {
        Logger(Error) << "No attribute found with name \"Shapelist\"";
        std::map<std::string, DM::Attribute*> attrs = sys_in->getAllAttributes();
        for (std::map<std::string, DM::Attribute*>::const_iterator it = attrs.begin(); it != attrs.end(); ++it)
        {
            DM::Attribute * s = it->second;
            Logger(Error) << "Possible Attribute name: " << s->getName();
        }

        return;
    }
    else
    {
       Logger(Standard) << "Shapelist attribute gefunden";
    }

    vector<string> shapes = attr->getStringVector();

    for(int index=0; index<shapes.size(); index++)
        if(!sys_in->getSubSystem(shapes[index]))
        {
            Logger(Error) << "Cannot extract subsystem: " << shapes[index];
            return;
        }

    Logger(Standard) << "Check well done :-)";
}

CheckShape::~CheckShape() {
}
