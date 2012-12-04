//X MACROS HEADER FOR VIEW DEFINITIONS

#define A(a) a,
#define X(b,c,d,e) class b##_ATTR_DEF{ public: enum {d};};
TABLE
#undef X
#undef A

#define A(a) a,
#define X(b,c,d,e) class b##_LINK_DEF{ public: enum {e};};
TABLE
#undef X
#undef A

#define NAMECOMP COMP
#define NAMETYPE TYPE
#define NAMECOMPONENTSTRING COMPONENTSTRING

#define X(a,b,c,d) a,
    enum NAMECOMP{TABLE};
#undef X

#define NAME COMP
#define X(a,b,c,d) #a,
    string NAMECOMPONENTSTRING[] = {TABLE};
#undef X

#define X(a,b,c,d) b,
    DM::Components NAMETYPE[] = {TABLE};
#undef X

class DM_HELPER_DLL_EXPORT ViewDefinitionHelper
{
private:
    std::map<NAMECOMP,std::vector<std::string> > ATTRIBUTESTRING;

public:
    ViewDefinitionHelper()
    {
        #define A(a) #a,
        #define X(b,c,d,e) std::string tmp_##b##_ATTR[] = {b##_ATTR}; ATTRIBUTESTRING[b]=std::vector<std::string>(tmp_##b##_ATTR, tmp_##b##_ATTR + sizeof(tmp_##b##_ATTR)/sizeof(tmp_##b##_ATTR[0]));
        TABLE
        #undef X
        #undef A
    }

    DM::View getView(NAMECOMP c, DM::ACCESS a)
    {
        return DM::View(NAMECOMPONENTSTRING[c], NAMETYPE[c], a);
    }

    DM::View getCompleteView(NAMECOMP c, DM::ACCESS a)
    {
        DM::View result(NAMECOMPONENTSTRING[c], NAMETYPE[c], a);

        for(uint index=0; index<ATTRIBUTESTRING[c].size(); index++)
        {
            if(a==DM::WRITE)
                result.addAttribute(ATTRIBUTESTRING[c][index]);

            if(a==DM::READ)
                result.getAttribute(ATTRIBUTESTRING[c][index]);

            if(a==DM::MODIFY)
                result.modifyAttribute(ATTRIBUTESTRING[c][index]);
        }

        return result;
    }

    std::vector<DM::View> getAll(DM::ACCESS a)
    {
        std::vector<DM::View> result;

        for(uint index = 0; index < sizeof(NAMECOMPONENTSTRING)/sizeof(NAMECOMPONENTSTRING[0]); index++)
            result.push_back(getCompleteView(static_cast<NAMECOMP>(index),a));

        return result;
    }
};

#undef NAMECOMP
#undef NAMETYPE
#undef NAMECOMPONENTSTRING
#undef TABLE
