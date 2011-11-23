%module(directors="1", allprotected="1") pydynamite
%feature("director");
%{
#include <DMcomponent.h>
#include <DMsystem.h>
#include <DMattribute.h>
#include <DMedge.h>
#include <DMnode.h>

using namespace std;
using namespace DM;
%}

%include std_vector.i
%include std_string.i
%include std_map.i
#include typemaps.i

namespace std {
    %template(stringvector) vector<string>;
    %template(doublevector) vector<double>;
    %template(systemvector) vector<System* >;
}

%include <DMcomponent.h>
%include <DMsystem.h>
%include <DMattribute.h>
%include <DMedge.h>
%include <DMnode.h>


