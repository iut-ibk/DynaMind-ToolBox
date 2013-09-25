#ifndef urbandevelCreateParcels_H
#define urbandevelCreateParcels_H

#include <dm.h>
#include <dmgeometry.h>
#include <cgalgeometry_p.h>

class DM_HELPER_DLL_EXPORT urbandevelCreateParcels : public DM::Module
{
    DM_DECLARE_NODE(urbandevelCreateParcels)
    private:
    DM::View resultView;
    DM::View face_nodes;
    DM::View inputView;
    DM::View bbs;

    double aspectRatio; //needs to be > 1
    double length;
    double offset;

    bool remove_new;
    std::string InputViewName;
    std::string OutputViewName;

    double tol;
    bool combined_edges;
    bool debug;

public:
    urbandevelCreateParcels();
    void run();
    void init();
    void createSubdevision(DM::System * sys,  DM::Face * f, int gen);

    /** @brief creates final parceling and identify edges, transfers results from working sys to sys */
    void createFinalFaces(DM::System * workingsys, DM::System *sys, DM::Face *orig, DM::View v, DM::SpatialNodeHashMap & sphs);

    /** @brief extract faces and returns vector of face nodes*/
    std::vector<DM::Node *> extractCGALFace(Arrangement_2::Ccb_halfedge_const_circulator hec, DM::SpatialNodeHashMap & sphs);

    /** @brief returns if face is filling of a assumed hole. I assume it is a hole when non of its edges is part of the boundary */
    bool checkIfHoleFilling(DM::Face *orig, DM::Face *face_new);


    double getLength() const;
    void setLength(double value);
    double getOffset() const;
    void setOffset(double value);
    double getAspectRatio() const;
    void setAspectRatio(double value);
    DM::View getInputView() const;
    void setInputView(const DM::View &value);
    DM::View getResultView() const;
    void setResultView(const DM::View &value);
    bool getCombined_edges() const;
    void setCombined_edges(bool value);
};

#endif
