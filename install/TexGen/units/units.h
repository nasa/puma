/*
 * units.h
 *
 *    This module contains the C implmenetation of the units
 *    conversion package, originally implemented in Tcl.  This
 *    facility will convert a variety of scientific and engineering
 *    shorthand notations into floating point numbers.
 *
 *  Bob Techentin
 *  November 15, 2000
 *  Copyright (C) Mayo Foundation.  All Rights Reserved.  
 * 
 */

#ifndef units_h
#define units_h


/*
 * Macros used to declare a function to be exported by a DLL.
 * Used by Windows, maps to no-op declarations on non-Windows systems.
 * The default build on windows is for a DLL, which causes the DLLIMPORT
 * and DLLEXPORT macros to be nonempty. To build a static library, the
 * macro STATIC_BUILD should be defined.
 */

#ifdef STATIC_BUILD
# define DLLIMPORT
# define DLLEXPORT
#else
# if defined(__WIN32__) && (defined(_MSC_VER) || (defined(__GNUC__) && defined(__declspec)))
#   define DLLIMPORT __declspec(dllimport)
#   define DLLEXPORT __declspec(dllexport)
# else
#  define DLLIMPORT
#  define DLLEXPORT
# endif
#endif

/*
 * These macros are used to control whether functions are being declared for
 * import or export.  If a function is being declared while it is being built
 * to be included in a shared library, then it should have the DLLEXPORT
 * storage class.  If is being declared for use by a module that is going to
 * link against the shared library, then it should have the DLLIMPORT storage
 * class.  If the symbol is beind declared for a static build or for use from a
 * stub library, then the storage class should be empty.
 *
 * The convention is that a macro called BUILD_xxxx, where xxxx is the
 * name of a library we are building, is set on the compile line for sources
 * that are to be placed in the library.  When this macro is set, the
 * storage class will be set to DLLEXPORT.  At the end of the header file, the
 * storage class will be reset to DLLIMPORt.
 */

#undef TCL_STORAGE_CLASS
#ifdef BUILD_tcl
# define TCL_STORAGE_CLASS DLLEXPORT
#else
# ifdef USE_TCL_STUBS
#  define TCL_STORAGE_CLASS
# else
#  define TCL_STORAGE_CLASS DLLIMPORT
# endif
#endif

/*
 * Definitions that allow this header file to be used either with or
 * without ANSI C features like function prototypes, and C/C++ compilers.
 * (pilfered from Tcl.h)
 */

#undef _ANSI_ARGS_
#undef CONST
#ifndef INLINE
#   define INLINE
#endif

#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus) || defined(USE_PROTOTYPE)
#   define _USING_PROTOTYPES_ 1
#   define _ANSI_ARGS_(x)	x
#   define CONST const
#else
#   define _ANSI_ARGS_(x)	()
#   define CONST
#endif

#ifdef EXTERN
#undef EXTERN
#endif /* EXTERN */

#ifdef __cplusplus
#   define EXTERN extern "C" TCL_STORAGE_CLASS
#else
#   define EXTERN extern TCL_STORAGE_CLASS
#endif


/*
 *-------------------------------------------------------------------
 *
 * units_new --
 *
 *  Create a new unit conversion
 *
 * Results:
 *      Creates a new unit conversion with the specified name. The new
 *      unit name must be only alphabetic (upper or lower case)
 *      letters.  The baseUnits string can consist of any valid units
 *      conversion string, including constant factors, numerator and
 *      denominator parts, units with prefixes, and exponents. Base
 *      units could also be the string "-primitive" to represent a new
 *      kind of quantity which cannot be derived from other units, but
 *      you probably don't want to do that.
 *
 * Side Effects:
 *      Future reductions and conversions could use this unit.
 *
 *-------------------------------------------------------------------
 */
EXTERN void units_new _ANSI_ARGS_(( char *name, char *baseUnits )) ;

/*
 *-------------------------------------------------------------------
 *
 * units_convert --
 *
 *      Convert a quantity string to a floating point number
 *
 * Results:
 *      Converts the value string into a floating point number, scaled
 *      to the specified targetUnits. The value string may contain a
 *      number and units. If units are specified, then they must be
 *      compatible with the targetUnits. If units are not specified
 *      for the value, then it will be scaled to the target units. For
 *      example, scaling "1.0" to "millimeter" would result in
 *      "1000.0".
 *
 * Side Effects:
 *      None.
 *
 *-------------------------------------------------------------------
 */
EXTERN double units_convert _ANSI_ARGS_(( char *value, char *targetUnits ));

/*
 *-------------------------------------------------------------------
 *
 * units_reduce --
 *
 *      Reduce a unit string to a scale factor and primitive units
 *
 * Results:
 *      Returns a unit string consisting of a scale factor followed by
 *      a space separated list of sorted and reduced primitive
 *      units. The reduced unit string may include a forward-slash
 *      (separated from the surrounding primitive subunits by spaces)
 *      indicating that the remaining subunits are in the
 *      denominator. Generates an error if the unitString is invalid.
 *
 * Side Effects:
 *      None.
 *
 *-------------------------------------------------------------------
 */
EXTERN char *units_reduce _ANSI_ARGS_(( char *unitString )) ;

/*
 *-------------------------------------------------------------------
 *
 * units_clear_exception --
 * units_check_exception --
 *
 *      Unit function error checking
 *
 * Results:

 *      These two error checking functions must be used to detect
 *      errors in the unit reduction and conversion functions.
 *      Implementation is designed primarily to work with SWIG's
 *      exception mechanism.  (http://www.swig.org)

 *      Each call to a units conversion function should be preceeded
 *      by a call to unit_clear_exception.  After calling the units
 *      function, unit_check_exception will return NULL if there was
 *      no error, or a pointer to a static character buffer containing
 *      an error message if there was an error.
 *
 * Side Effects:
 *      None.
 *
 *-------------------------------------------------------------------
 */
EXTERN void units_clear_exception _ANSI_ARGS_(());
EXTERN char *units_check_exception _ANSI_ARGS_(());

#endif
