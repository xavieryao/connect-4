%module strategy

%{
#define SWIG_FILE_WITH_INIT
#include "UCTStrategy.h"
    %}

%include "carrays.i"
%array_class(int, intArray)
%array_class(intArray, int2dArray)
    %include "UCTStrategy.h"
