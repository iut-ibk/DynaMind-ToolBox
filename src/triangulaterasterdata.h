#ifndef TRIANGULATERASTERDATA_H
#define TRIANGULATERASTERDATA_H

#include <dm.h>


class TriangulateRasterData
{
public:
    static void Triangulation(std::vector<DM::Node> & nodes, DM::RasterData * rData);
};

#endif // TRIANGULATERASTERDATA_H
