/* File: tmkpy.i */
%module tmkpy
%include <std_string.i>
%{
#include <tmk/cpp/algo/tmkfv.h>
#include <tmkpy.h>
%}

//https://stackoverflow.com/a/14073400/620113
//http://swig.10945.n7.nabble.com/SWIG-Exception-Handler-td8737.html
%include <exception.i>
%exception {
	try {
		$action
	} catch (const std::exception& e) {
		SWIG_exception(SWIG_RuntimeError, e.what());
	} catch (...) {
		SWIG_exception(SWIG_RuntimeError, "unknown exception");
	}
}

%import "std_vector.i"
%template() std::vector<float>;
%template() std::vector<int>;
%template() std::vector<std::vector<std::vector<float>>>;
%template() std::vector<std::string>;
%include <tmk/cpp/algo/tmkfv.h>
%include <tmk/cpp/io/tmkiotypes.h>
%include <tmkpy.h>


