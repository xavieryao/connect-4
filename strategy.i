%module strategy

%{
#define SWIG_FILE_WITH_INIT
extern int _M;
extern int _N;
#include "UCTStrategy.h"
#include "Point.h"
    %}

%include "carrays.i"
%array_class(int, intArray)
%array_class(intArray, int2dArray)
    %include "UCTStrategy.h"
    %include "Point.h"
