#ifndef FACETOEDGE_H
#define FACETOEDGE_H

#include <dm.h>

class DM_HELPER_DLL_EXPORT FaceToEdge: public DM::Module
{
    DM_DECLARE_NODE(FaceToEdge)
private:
        std::string face_name;
        std::string edge_name;
        DM::View view_face;
        DM::View view_edge;

public:
    FaceToEdge();
    void init();
    void run();
};

#endif // FACETOEDGE_H
