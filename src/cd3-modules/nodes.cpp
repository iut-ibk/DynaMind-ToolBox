#include "tank.h"
#include "defaultblock.h"
#include "consumption.h"
#include "storage.h"
#include "imperviousrunoff.h"
#include "rwht.h"

#include <noderegistry.h>
#include <nodefactory.h>
#include <simulationregistry.h>
#include <simulationfactory.h>

#include <typefactory.h>
#include <cd3globals.h>

static const char *SOURCE = "Watersupply";

extern "C" {
	void CD3_PUBLIC registerNodes(NodeRegistry *registry) {
                registry->addNodeFactory(new NodeFactory<Tank>(SOURCE));
                registry->addNodeFactory(new NodeFactory<Default>(SOURCE));
                registry->addNodeFactory(new NodeFactory<Consumption>(SOURCE));
                registry->addNodeFactory(new NodeFactory<Storage>(SOURCE));
                registry->addNodeFactory(new NodeFactory<ImperviousRunoff>(SOURCE));
                registry->addNodeFactory(new NodeFactory<RWHT>(SOURCE));
	}

    void CD3_PUBLIC registerSimulations(SimulationRegistry *registry) {
    }
}
