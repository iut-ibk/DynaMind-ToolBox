#ifndef urbandevelMarkov_H
#define urbandevelMarkov_H

#include <dmmodule.h>

class urbandevelMarkov: public DM::Module
{
    DM_DECLARE_NODE(urbandevelMarkov)

public:
    urbandevelMarkov();
    ~urbandevelMarkov();

    void run();
    void init();

    std::string getHelpUrl();

private:
    DM::View cityview;
    int dummy;
};

#endif // urbandevelMarkov_H
