%module vsal_csharp

// Additional code
%{
#include "vsal/VideoStream.h"
#include "vsal/VideoStreamFactory.h"
%}

// SWIG interfaces
%include "windows.i"
%include "std_string.i"
%include "exception.i"
%include "arrays_csharp.i"

// Handle exceptions
%exception { 
    try {
        $action
    } catch (std::exception &e) {
        std::string s("SensorUtilities error: "), s2(e.what());
        s = s + s2;
        SWIG_exception(SWIG_RuntimeError, s.c_str());
    } catch (...) {
        SWIG_exception(SWIG_RuntimeError, "unknown exception");
    }
}

// VideoStream interface
CSHARP_ARRAYS(unsigned char, byte)
%apply unsigned char INOUT[] {unsigned char *}

// VideoStreamFactory interface
%ignore vsal::VideoStreamFactory::create(int argc, char** argv);

// Includes
%include "vsal/vsal_export.h"
%include "vsal/VideoStream.h"
%include "vsal/VideoStreamFactory.h"