/*---------------------------------------------------*-C-*----------
 *
 *  units.i
 *
 *  SWIG wrapper definition for the units conversion package
 *
 *  The wrappers included with this module allow access to the
 *  Units library for two different purposes.
 *
 *   1.  Creation of a dynamic loadable library (units.sl)
 *       for use with any Tcl interpreter.  In this mode,
 *       the functions units::reduce, units::convert, and 
 *       units::new are exposed to the interpreter.
 *
 *   2.  Inclusion in another SWIG'd application.  Typemaps
 *       and typedefs are defined so that the C/C++ application 
 *       functions can be defined in terms of quantity types,
 *       such as Length or Time.  The typemaps will automatically
 *       invoke the appropriate unit conversions to deliver
 *       appropriately scaled floating point numbers to the 
 *       C routines.  Invalid conversions will throw an error
 *       back to the script level.
 *
 *  Bob Techentin
 *  November 29, 2000
 *  Copyright (C) by Mayo Foundation.  All Rights Reserved.
 *
 *------------------------------------------------------------------
 */

%title "Units Conversion"

%module units

%{
#include "units.h"
%}

/*
 *------------------------------------------------------------------
 *  Add exception handling, using the exception functions
 *  defined in the units module and the SWIG exception
 *  facility.
 *------------------------------------------------------------------
 */


%include exception.i

%except {
  char *msg;
  units_clear_exception();
  $function
    msg = units_check_exception();
  if (msg != NULL) {
    SWIG_exception(SWIG_RuntimeError, msg) ;
  }
}

/*
 *------------------------------------------------------------------
 *  unit conversion functions to be wrapped
 *------------------------------------------------------------------
 */

void new ( char *name, char *baseUnits ) ;
double convert ( char *value, char *targetUnits ) ;
char *reduce ( char *unitString ) ;


%{
  void new ( char *name, char *baseUnits ) {
    units_new( name, baseUnits);
  }

  double convert ( char *value, char *targetUnits ) {
    return units_convert( value, targetUnits );
  }

  char *reduce ( char *unitString ) {
    return units_reduce( unitString);
  }
%}





/*------------------------------------------------------------------
 *
 *   Typedefs for unit conversion.
 *
 *   These new types can be used in wrapper interface file
 *   function prototypes.  The typmapping mechanism will
 *   convert from a string into this type.  They will appear
 *   in the wrapper file as typedefs.
 *
 *------------------------------------------------------------------*/

/*  SI Base Units */
%typedef float Length ;
%typedef float Mass ;
%typedef float Time ;
%typedef float ElectricCurrent ;
%typedef float Temperature ;
%typedef float Amount ;
%typedef float LuminousIntensity ;

/*  SI Derived Units with Special Names */
%typedef float PlaneAngle ;
%typedef float SolidAngle ;
%typedef float Frequency ;
%typedef float Force ;
%typedef float Pressure ;
%typedef float Energy ;
%typedef float Power ;
%typedef float ElectricCharge ;
%typedef float ElectricPotential ;
%typedef float Capacitance ;
%typedef float ElectricResistance ;
%typedef float ElectricConductance ;
%typedef float MagneticFlux ;
%typedef float MagneticFluxDensity ;
%typedef float Inductance ;
%typedef float LuminousFlux ;
%typedef float Illuminance ;

/*  for Electrical Engineering  */
%typedef ElectricCurrent      Current ; 
%typedef ElectricCharge       Charge ;
%typedef ElectricPotential    Voltage ;
%typedef ElectricResistance   Resistance ;
%typedef ElectricConductance  Conductance ;



%{
/*
 *---------------------------------------------------------------
 *
 *  convertTypedWrapperValue
 *
 *  Converts a typed wrapper value into an appropriately
 *  scaled floating point number.
 *
 *  This routine is the guts of the unitsConversion typemap,
 *  extracted here to (1) reduce duplication in the generated
 *  wrapper code, and (2) be callable from other typemaps.
 *
 *---------------------------------------------------------------
 */
#include<string.h>

  /*  these functions defined below  */
  char *unitsDefaultString( char *typeName ) ;
  char *unitsTargetString( char *typeName ) ;


  static char *convertTypedWrapperValue( char *sourceString, 
				       char *basetype,
				       float *target) {

    static char typeName[100]; /*  basetype without trailing space    */
    char *endPtr ;             /*  for default unit checking          */
    char sourceCopy[100] ;     /*  copy of source argument (if needed)*/
    char *defaultUnits ;       /*  appended if none given in $source  */
    char *targetUnits ;        /*  target of conversion               */
    char *msg ;                /*  error message                      */
    char *cp ;                 /*  pointer for finding trailing spaces*/

    /*  Initial conversion finds the end of the number part.  */
    /*  Value is saved in case there are no target units.     */
    *target = (float) strtod(sourceString, &endPtr) ;

    /*  Compute type name without trailing space  */
    /*  which comes from SWIG's $basetype macro.  */
    strcpy(typeName, basetype) ;
    cp = strchr(typeName,' ') ;
    if (cp != 0) 
      *cp = '\0';

    /*  Apply default units if necessary  */
    if ( *endPtr == '\0' ) {
      strcpy(sourceCopy, sourceString) ;
      defaultUnits = unitsDefaultString( typeName ) ; 
      if ( defaultUnits != NULL ) {
	strcat( sourceCopy, " " ) ;
	strcat( sourceCopy, defaultUnits ) ;
	sourceString = sourceCopy ;
      }
    }

    /*  Perform conversion  */
    targetUnits = unitsTargetString( typeName ) ; 
    if ( targetUnits != NULL ) {
      units_clear_exception();
      *target = convert( sourceString, targetUnits ) ;
      msg = units_check_exception();
      return msg;
    }
    /* no valid target units  */
    strcat(typeName, " has no valid Target units");
    return typeName;
  }


%}





/*
 *---------------------------------------------------------------
 *
 *  Generic Unit Conversion Typemap
 *
 *---------------------------------------------------------------
 */


%typemap(tcl8,in) unitsConversion {
  /* executing %typemap(tcl8,in) unitsConversion for $basetype */
  char *tempString ;
  int tempLength ;

  /*  Get source string from source object  */
  tempString = Tcl_GetStringFromObj($source, &tempLength) ;
  if ( tempString == NULL )
    return TCL_ERROR ;
  else {
    char *ret = convertTypedWrapperValue( tempString, "$basetype", &$target);
    if ( ret != NULL ) {
      Tcl_SetStringObj(tcl_result, ret, -1) ;
      return TCL_ERROR ;
    }
  }
}


/*---------------------------------------------------------------
 *
 *  Apply unitsConversion typemap to defined types
 *
 *---------------------------------------------------------------*/

/*  SI Base Units */
%apply unitsConversion { Length } ;
%apply unitsConversion { Mass } ;
%apply unitsConversion { Time } ;
%apply unitsConversion { ElectricCurrent } ;
%apply unitsConversion { Temperature } ;
%apply unitsConversion { Amount } ;
%apply unitsConversion { LuminousIntensity } ;

/*  SI Derived Units with Special Names */
%apply unitsConversion { PlaneAngle } ;
%apply unitsConversion { SolidAngle } ;
%apply unitsConversion { Frequency } ;
%apply unitsConversion { Force } ;
%apply unitsConversion { Pressure } ;
%apply unitsConversion { Energy } ;
%apply unitsConversion { Power } ;
%apply unitsConversion { ElectricCharge } ;
%apply unitsConversion { ElectricPotential } ;
%apply unitsConversion { Capacitance } ;
%apply unitsConversion { ElectricResistance } ;
%apply unitsConversion { ElectricConductance } ;
%apply unitsConversion { MagneticFlux } ;
%apply unitsConversion { MagneticFluxDensity } ;
%apply unitsConversion { Inductance } ;
%apply unitsConversion { LuminousFlux } ;
%apply unitsConversion { Illuminance } ;

/*  for Electrical Engineering  */
%apply unitsConversion { Current } ;
%apply unitsConversion { Charge } ;
%apply unitsConversion { Voltage } ;
%apply unitsConversion { Resistance } ;
%apply unitsConversion { Conductance } ;





/*  The following section is copied directly into the wrapper file  */

%{
/*---------------------------------------------------------------
 *
 *  unitsDefaultTable
 *
 *  This table associates unit types (e.g., "Length") with 
 *  defaults (e.g., "meters") which are employed when no
 *  units are supplied to a conversion.
 *  
 *  The table is coded as a static C array.  The values
 *  are passed to unitsSetDefault() and unitsSetTarget()
 *  in the %init code section.
 *  
 *  The scripting language initializaiton and unitsConversion 
 *  typemape are responsible for ensuring that the proper 
 *  conversions are executed.
 *
 *---------------------------------------------------------------*/
  typedef struct {
    char *name ;
    char *unit ;
  } UnitsDefaultTableEntry ;

  static UnitsDefaultTableEntry unitsDefaultTable[] = {

    /*  SI Base Units */
    { "Length",              "meters" },
    { "Mass",                "kilograms" },
    { "Time",                "seconds" },
    { "ElectricCurrent",     "amperes" },
    { "Temperature",         "kelvins" },
    { "Amount",              "moles" },
    { "LuminousIntensity",   "candela" },

    /*  SI Derived Units with Special Names */
    { "PlaneAngle",          "radian" },
    { "SolidAngle",          "steradian" },
    { "Frequency",           "hertz" },
    { "Force",               "newtons" },
    { "Pressure",            "pascals" },
    { "Energy",              "joules" },
    { "Power",               "watts" },
    { "ElectricCharge",      "coulombs" },
    { "ElectricPotential",   "volts" },
    { "Capacitance",         "farads" },
    { "ElectricResistance",  "ohms" },
    { "ElectricConductance", "siemens" },
    { "MagneticFlux",        "webers" },
    { "MagneticFluxDensity", "tesla" },
    { "Inductance",          "henrys" },
    { "LuminousFlux",        "lumens" },
    { "Illuminance",         "lux" },

    /*  for Electrical Engineering  */
    { "Current",             "amperes" },
    { "Charge",              "coulombs" },
    { "Voltage",             "volts" },
    { "Resistance",          "ohms" },
    { "Conductance",         "siemens" },
  };

  const int UNITSDEFAULTTABLESIZE = sizeof(unitsDefaultTable)/sizeof(UnitsDefaultTableEntry) ;


/*
 *---------------------------------------------------------------
 *
 *  Default and Target Units
 *  
 *  These script-language dependent routines set and retrieve the
 *  default and target units strings from the script environment
 *  for the convertTypeWrapperValue() routine.
 *
 *  unitsSetDefault( char *typeName, char *unit ) ;
 *  unitsSetTarget( char *typeName, char *unit ) ;
 *  unitsDefaultString( char *typeName ) ;
 *  unitsTargetString( char *typeName ) ;
 *
 *---------------------------------------------------------------
 */

#ifdef SWIGTCL8
  /*---------------------------------------------------------------
  *  We have to save a pointer to the interpreter that was active
  *  during SWIG initialization so that we can access hash tables.
  *  The SWIG mechanism for linking variables does not seem to
  *  apply to hash tables (Tcl arrays).
  *---------------------------------------------------------------*/
  static Tcl_Interp *unitsInterpreter = NULL ;


  void unitsSetDefault( char *typeName, char *value ) {
    Tcl_SetVar2(unitsInterpreter, SWIG_prefix "default", typeName, value, 0);
  }

  void unitsSetTarget( char *typeName, char *value ) {
    Tcl_SetVar2(unitsInterpreter, SWIG_prefix "target", typeName, value, 0);
  }

  char *unitsDefaultString( char *typeName ) {
    return Tcl_GetVar2(unitsInterpreter, SWIG_prefix "default", typeName, 0);
  }

  char *unitsTargetString( char *typeName ) {
    return Tcl_GetVar2(unitsInterpreter, SWIG_prefix "target", typeName, 0);
  }

#endif  /*  ifdef SWIGTCL8  */

%}


%init %{
/*
 *------------------------------------------------------------------
 *
 *  units.i module initialization
 *
 *------------------------------------------------------------------
 */

#ifdef SWIGTCL8
/*
 *------------------------------------------------------------------
 *  package definition
 *
 *  The units functions are provided as a package.  If SWIG
 *  is run with the -namespace switch, then they will be
 *  in the units namespace.  If units_wrap.c is compiled with
 *  -DUSE_TCL_STUBS flag, and linked against libtclstub8.x 
 *  instead of libtcl8.x, then the dynamic library can be
 *  loaded into any stub-ified version of Tcl (8.1 or higher).
 *------------------------------------------------------------------
 */
  Tcl_PkgProvide(interp,"units","1.1");
#ifdef USE_TCL_STUBS
  if (Tcl_InitStubs(interp,"8.1",0) == NULL) {
    return TCL_ERROR;
  }
#endif
#endif


  /*
   *---------------------------------------------------------------
   *
   *  default/target unit initialization
   *
   *---------------------------------------------------------------
   */


#ifdef SWIGTCL8
  /*
   *---------------------------------------------------------------
   *
   *  Save this Tcl interpreter so that the initialization
   *  and Default/Target unit routines work.
   *
   *---------------------------------------------------------------
   */
  unitsInterpreter = interp ;

#endif /*  ifdef SWIGTCL8  */

  /*
   *---------------------------------------------------------------
   *
   *  Initialize default and target units arrays (hash tables)
   *
   *---------------------------------------------------------------
   */
  {
    int i;
    for (i=0; i<UNITSDEFAULTTABLESIZE; i++ ) {
      unitsSetDefault(unitsDefaultTable[i].name, unitsDefaultTable[i].unit);
      unitsSetTarget(unitsDefaultTable[i].name, unitsDefaultTable[i].unit);
    }
  }


%}
