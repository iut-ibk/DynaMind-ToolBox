#include "dmnodefactory.h"
#include "dmmoduleregistry.h"

#include "urbandevelDivision.h"
#include "urbandevelBuilding.h"
#include "urbandevelControl.h"
#include "urbandevelCycle.h"
#include "urbandevelRankEuclid.h"
#include "urbandevelRankArea.h"
#include "urbandevelRankYear.h"
#include "urbandevelTrigger.h"
#include "urbandevelPolygonLink.h"
#include "urbandevelSetHeight.h"
#include "urbandevelSetType.h"
#include "urbandevelWaterAttributes.h"

using namespace std;

extern "C" void DM_HELPER_DLL_EXPORT  registerModules(DM::ModuleRegistry *registry) {
    registry->addNodeFactory(new DM::NodeFactory<urbandevelDivision>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelBuilding>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelControl>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelCycle>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelRankEuclid>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelRankArea>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelRankYear>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelPolygonLink>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelTrigger>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelSetType>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelSetHeight>());
    registry->addNodeFactory(new DM::NodeFactory<urbandevelWaterAttributes>());
}
