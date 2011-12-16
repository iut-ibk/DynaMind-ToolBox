
#include "complexgeometry.h"

#include <DM.h>
#include <vibe_logger.h>

bool ComplexGeometry()
{
    DM::System * sys = new DM::System("Environment");

    //Create Parcel View
    DM::View parcelview("Parcels");

    //The Parcel is described as a subsystem
    parcelview.addComponent(DM::SUBSYSTEM);
    parcelview.addAttributes("Area");

    sys->addView(parcelview);


    //Create Nodes
    DM::Node * N1 = sys->addNode(0,0,0);
    DM::Node * N2 = sys->addNode(1,0,0);
    DM::Node * N3 = sys->addNode(1,1,0);
    DM::Node * N4 = sys->addNode(0,1,0);

    //Create Edges
    DM::Edge * E1 = sys->addEdge(N4, N1);
    DM::Edge * E2 = sys->addEdge(N1, N2);
    DM::Edge * E3 = sys->addEdge(N2, N3);
    DM::Edge * E4 = sys->addEdge(N3, N4);

    //Create Parcel
    DM::System * parcel = new DM::System("Parcel");

    parcel->addEdge(E1);
    parcel->addEdge(E2);
    parcel->addEdge(E3);
    parcel->addEdge(E4);
    DM::Attribute attr("A");
    attr.setDouble(10);

    parcel->addAttribute(attr);

    //Add parcel to the system
    sys->addSubSystem(parcel, "Parcels");

    //Get Component and Test if it is working
    std::vector<std::string> NamesOfViews = sys->getViews();
    for (std::vector<std::string>::const_iterator it = NamesOfViews.begin(); it != NamesOfViews.end(); ++it) {
        std::string s = *it;
        vibens::Logger(vibens::Debug) << s;
    }

    std::map<std::string, DM::Component*> components_view = sys->getAllComponentsInView("Parcels");
    for (std::map<std::string, DM::Component*>::const_iterator it = components_view.begin(); it != components_view.end(); ++it) {
        vibens::Logger(vibens::Debug) << it->first;
         DM::Component * c  = it->second;
         std::map<std::string, DM::Attribute*> attributes = c->getAllAttributes();


         for (std::map<std::string, DM::Attribute*> ::const_iterator itattribute = attributes.begin(); itattribute != attributes.end(); ++itattribute ) {
             DM::Attribute * a = itattribute->second;
             vibens::Logger(vibens::Debug) << a->getName();
             vibens::Logger(vibens::Debug) << a->getDouble();
         }
    }

    return true;

}
