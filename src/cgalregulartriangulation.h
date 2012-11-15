#ifndef CGALREGULARTRIANGULATION_H
#define CGALREGULARTRIANGULATION_H

#include <dm.h>

class CGALRegularTriangulation
{
public:
static void Triangulation(DM::System * sys, DM::Face * f, std::vector<DM::Node> & triangels, double meshsize);
};

#endif // CGALREGULARTRIANGULATION_H
