//THIS CODE SHOULD BE IN AN OWN HEADER BUT QTCREATER CANNOT RESOLVE THESE MACROS

#define A(a) a,
#define X(b,c,d,e) namespace b##_ATTR_DEF{ enum TYPES{d};};
TABLE
#undef X
#undef A

#define B(a,b,c,d) d,
#define X(b,c,d,e) namespace b##_LINK_DEF{ enum TYPES{e};};
TABLE
#undef X
#undef B

#define NAMECOMP COMPONENTS
#define NAMETYPE TYPE
#define NAMECOMPONENTSTRING COMPONENTSTRING

#define X(a,b,c,d) a,
    enum NAMECOMP{TABLE};
#undef X

class DM_HELPER_DLL_EXPORT ViewDefinitionHelper
{

private:
    std::map<NAMECOMP,std::vector<std::string> > ATTRIBUTESTRING;
    std::map<NAMECOMP,std::vector<std::string> > LINKSTRING;
    std::map<NAMECOMP,std::map<std::string, DM::View> > LINKSVIEW;

    std::vector<std::string> NAMECOMPONENTSTRING;
    std::vector<DM::Components> NAMETYPE;

public:
    ViewDefinitionHelper()
    {
        #define NAME COMP

        NAMECOMPONENTSTRING = std::vector<std::string>();

        #define X(a,b,c,d) NAMECOMPONENTSTRING.push_back(#a);
        TABLE
        #undef X

        NAMETYPE = std::vector<DM::Components>();

        #define X(a,b,c,d) NAMETYPE.push_back(b);
        TABLE
        #undef X

        std::vector<std::string> tmpattrvec;

        #define A(a) tmpattrvec.push_back(#a);
        #define X(b,c,d,e) d ATTRIBUTESTRING[b]=std::vector<std::string>(tmpattrvec); tmpattrvec.clear();
        TABLE
        #undef X
        #undef A
        #undef NAME

        #define B(a,b,c,d) tmpattrvec.push_back(#d);
        #define X(d,e,f,g) g LINKSTRING[d]=std::vector<std::string>(tmpattrvec); tmpattrvec.clear();
        TABLE
        #undef X
        #undef B
        #undef NAME

        #define B(a,b,c,d) LINKSVIEW[a][LINKSTRING[a][a##_LINK_DEF::d]]=b::ViewDefinitionHelper().getCompleteView(b::c,DM::READ);
        #define X(d,e,f,g) g
        TABLE
        #undef X
        #undef B
        #undef NAME
    }

    DM::View getView(NAMECOMP c, DM::ACCESS a)
    {
        return DM::View(NAMECOMPONENTSTRING[c], NAMETYPE[c], a);
    }

    std::string getAttributeString(NAMECOMP c, uint index)
    {
        return ATTRIBUTESTRING[c][index];
    }

    std::string getLinkString(NAMECOMP c, uint index)
    {
        return LINKSTRING[c][index];
    }

    DM::View getCompleteView(NAMECOMP c, DM::ACCESS a)
    {
        DM::View result = getView(c, a);

        for(uint index=0; index<ATTRIBUTESTRING[c].size(); index++)
        {
            if(a==DM::WRITE)
                result.addAttribute(ATTRIBUTESTRING[c][index]);

            if(a==DM::READ)
                result.getAttribute(ATTRIBUTESTRING[c][index]);

            if(a==DM::MODIFY)
                result.modifyAttribute(ATTRIBUTESTRING[c][index]);
        }

        for(uint index=0; index<LINKSTRING[c].size(); index++)
            result.addLinks(LINKSTRING[c][index],LINKSVIEW[c][LINKSTRING[c][index]]);

        return result;
    }

    std::vector<DM::View> getAll(DM::ACCESS a)
    {
        std::vector<DM::View> result;

        for(uint index = 0; index < sizeof(NAMECOMPONENTSTRING)/sizeof(NAMECOMPONENTSTRING[0]); index++)
            result.push_back(getCompleteView(static_cast<NAMECOMP>(index),a));

        return result;
    }

    void setAttribute(NAMECOMP c,uint index,DM::View &view,DM::ACCESS a)
    {
        if(a==DM::WRITE)
            view.addAttribute(getAttributeString(c,index));

        if(a==DM::READ)
            view.getAttribute(getAttributeString(c,index));

        if(a==DM::MODIFY)
            view.modifyAttribute(getAttributeString(c,index));
    }
};

#undef NAMECOMP
#undef COMP
#undef COMPONENTSTRING
#undef NAMETYPE
#undef TYPE
#undef NAMECOMPONENTSTRING
#undef TABLE

//END OF --- THIS CODE SHOULD BE IN AN OWN HEADER BUT QTCREATER CANNOT RESOLVE THESE MACROS
