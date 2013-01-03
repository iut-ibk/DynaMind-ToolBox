#include "triangulaterasterdata.h"

void TriangulateRasterData::Triangulation(std::vector<DM::Node> & nodes, DM::RasterData * rData) {

    unsigned long Y = rData->getHeight();
    unsigned long X = rData->getWidth();

    double noData = rData->getNoValue();
    double lX = rData->getCellSizeX();
    double lY = rData->getCellSizeY();

    nodes.reserve(Y*X*6);

    for (unsigned long  y = 0; y < Y; y++) {
        for (unsigned long  x = 0; x < X; x++) {
            double val = rData->getCell(x,y);
            DM::Logger(DM::Debug) << val;
            nodes.push_back( DM::Node( (x-0.5) * lX ,  (y-0.5) * lY, val) );
            nodes.push_back( DM::Node( (x+0.5) * lX,   (y-0.5) * lY, val) );
            nodes.push_back( DM::Node( (x-0.5) * lX,   (y+0.5) * lY, val) );

            nodes.push_back( DM::Node( (x-0.5) * lX,   (y+0.5) * lY, val) );
            nodes.push_back( DM::Node( (x+0.5) * lX,   (y-0.5) * lY, val) );
            nodes.push_back( DM::Node( (x+0.5) * lX,   (y+0.5) * lY, val) );

        }
    }

}
