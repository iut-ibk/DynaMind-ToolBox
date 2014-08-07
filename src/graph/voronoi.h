#include <dmcomponent.h>
#include <dmnode.h>
#include <dmedge.h>
#include <dmsystem.h>
#include <dmface.h>

class Voronoi
{
public:
    static bool createVoronoi(std::vector<DM::Component*> &nodes, std::vector<DM::Component*> &faces, DM::System *sys, DM::View v);
};
