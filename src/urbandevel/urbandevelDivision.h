#ifndef urbandevelDivision_H
#define urbandevelDivision_H

#include <dmmodule.h>

#include <dmgeometry.h>
#include <cgalgeometry_p.h>

class DM_HELPER_DLL_EXPORT urbandevelDivision : public DM::Module
{
    DM_DECLARE_NODE(urbandevelDivision)

public:
    urbandevelDivision();
    ~urbandevelDivision();
    void run();
    void init();
    std::string getHelpUrl();

    void createSubdivision(DM::System * sys,  DM::Face * block, int gen, std::string type);
    void createFinalFaces(DM::System * workingsys, DM::System *sys, DM::Face *orig, DM::View v, DM::SpatialNodeHashMap & snhm);
    std::vector<DM::Node *> extractCGALFace(Arrangement_2::Ccb_halfedge_const_circulator hec, DM::SpatialNodeHashMap & snhm);
    bool checkIfHoleFilling(DM::Face *block, DM::Face *element);

private:
    DM::View cityview;
    DM::View blockview;
    DM::View elementview;
    DM::View elementview_nodes;
    DM::View bbs;

    DM::View sb;

    std::string blockview_name;
    std::string elementview_name;

    double aspectratio;
    double length;
    double offset;
    double tol;
    bool combined_edges;
    bool onSignal;
    bool sizefromSB;
    bool splitShortSide;
    bool copyheight;
    std::string develtype;

    int currentyear;
    int worklength;
    int height_avg;

    bool debug;
};

#endif // urbandevelDivision_H
