#ifndef urbandevelSetAttribute_H
#define urbandevelSetAttribute_H

#include <dmmodule.h>

class DM_HELPER_DLL_EXPORT urbandevelSetAttribute : public DM::Module
{
    DM_DECLARE_NODE(urbandevelSetAttribute)

public:
    urbandevelSetAttribute();
    ~urbandevelSetAttribute();

	void run();
	void init();

    std::string getHelpUrl();

private:
    DM::View view;

    std::string view_name;
    std::string checkattribute;
    std::string setattribute;
    std::string checkvalue;
    std::string setvalue;
    bool cisnumber;
    bool sisnumber;
};

#endif // urbandevelSetAttribute_H
