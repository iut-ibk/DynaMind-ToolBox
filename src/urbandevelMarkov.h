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
private:
    int yearcycle;
    DM::View city;
};

#endif // urbandevelMarkov_H
