/* --------------------------------------------*-C-*------------

   units.c

   This module contains the C implmenetation of the units conversion
   package, originally implemented in Tcl.  This facility will
   convert a variety of scientific and engineering shorthand notations
   into floating point numbers.

   Bob Techentin
   November 15, 2000
   Copyright (C) Mayo Foundation.  All Rights Reserved.

   -----------------------------------------------------------------*/

#include "units.h"

/*
 *-----------------------------------------------------------------
 *
 *       Functions exported from this module
 *
 *  void units_new ( char *name, char *baseUnits ) 
 *  double units_convert ( char *value, char *targetUnits ) 
 *  char *units_reduce ( char *unitString ) 
 *  void units_clear_exception() 
 *  char *units_check_exception() 
 *
 *  These are prototype in units.h, so we do not need to
 *  repeat the prototypes here.
 *
 *-----------------------------------------------------------------
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>


/*  Define a few useful constants  */
static char *primitivestring="-primitive";
static char powerchar='^';
static char *unitNameCharacters="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char *separators=" \t*-";
static char *divider="/";
static char *validchars="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789. \t*^/+-";


/*  
 *-----------------------------------------------------------------
 *
 *  Structure Definitions
 *
 *  We really only have two types of structures here.
 *  Units and Prefixes.  We will define them with a "next"
 *  pointer to facilitate linking them into a list.  That's
 *  so we don't have to create a separate type of table or
 *  list to store them.
 *
 *  A 'unit' structure includes a name, scalar (floating
 *  point) factor, and a numerator and denominator.  The
 *  numerator and denominator are actually a linked list of
 *  other, primitive units.  A unit is primitive if flagged
 *  as such.  It would be nice to just check for NULL
 *  numerator and denominator lists, but some units like
 *  radians actually reduce to 1.0, but are not primitive.
 *
 *-----------------------------------------------------------------
 */

typedef struct unit {
  char *name;
  double factor;
  int primitive;
  struct unit *numerator;
  struct unit *denominator;
  struct unit *next;
} Unit ;

typedef struct prefix {
  char *name;
  double factor;
  struct prefix *next;
} Prefix ;

/*  head of lists  */
static Unit *uroot = NULL;
static Prefix *proot = NULL;


/*
 *-----------------------------------------------------------------
 *  
 *   Local Functions (not exported)
 *  
 *-----------------------------------------------------------------
 */

/*  String manipulation functions  */
static char *stringcopy _ANSI_ARGS_((CONST char *string));
static char *stringset _ANSI_ARGS_((char *target, CONST char *source));
static char *stringappend _ANSI_ARGS_((char *target, CONST char *source));

/*  Unit and Prefix creation/destruction  */
static Unit *createunit _ANSI_ARGS_(());
static void destroyunit _ANSI_ARGS_((Unit *u));
static Prefix *createprefix _ANSI_ARGS_((char *name, double factor));

/*  Unit untility functions  */
static Unit *reduceUnit  _ANSI_ARGS_((char *unitString));
static Unit *sortUnitList _ANSI_ARGS_(( Unit *u));
static void cancelUnitList _ANSI_ARGS_((Unit **numerator, Unit **denominator));
static int unitInitialize _ANSI_ARGS_(());

 /*  Error Generation Functions  */
static void throw_exception _ANSI_ARGS_((char *message));
static void throw_exception1 _ANSI_ARGS_((char *message, char *arg1));
static void throw_exception2 _ANSI_ARGS_((char *message, char *arg1, char *arg2));


/*
 *-----------------------------------------------------------------
 *
 *  String Utility Functions
 *
 *  These are just a little dangerous because they
 *  allocate memory which must be freed, but they
 *  are very convenient.  Especially if you're used
 *  to a nice String class.
 *
 *-----------------------------------------------------------------
 */

/*  Make a new copy of a string  */
static char *stringcopy(string)
     CONST char *string;     /*  string to copy  */
{
  char *copy;
  copy = (char *)malloc(strlen(string)+1);
  strcpy(copy,string);
  return(copy);
} 

/*  Set a string, allocating new memory  */
static char *stringset(target, source) 
     char *target;          /*  char *pointer to be reallocated  */
     CONST char *source;    /*  source string  */
{
  target = (char *) realloc( target, strlen(source)+1 ) ;
  strcpy(target,source);
  return(target);
} 

/*  Append a string, allocating more memory  */
static char *stringappend(target, source) 
     char *target;          /*  char *pointer to be reallocated  */
     CONST char *source;    /*  source string  */
{
  size_t newsize;
  newsize = (size_t) strlen(target) + strlen(source) + 1;
  target = (char *) realloc( target, newsize ) ;
  strcat(target,source);
  return(target);
} 



/*
 *-----------------------------------------------------------------
 *
 *  Unit/Prefix Utility Functions
 *
 *  It is handy to create/destroy them here,
 *  just for consistency.
 *
 *-----------------------------------------------------------------
 */
static Unit *createunit() {
  Unit *u;
  u = (Unit *) malloc(sizeof(Unit));
  u->name=stringset(NULL,"");
  u->factor=1.0;
  u->primitive=0;
  u->numerator=NULL;
  u->denominator=NULL;
  u->next=NULL;
  return u;
}

/*  
 *  Free name and structure, but recusively
 *  destroy any units referenced in
 *  numerator or denominator
 */
 static void destroyunit(u)
     Unit *u;           /*  unit struct to be destroyed  */
{
  Unit *u1, *u2 ;
  if ( u != NULL ) {
    free(u->name);
    u1=u->numerator;
    while ( u1!=NULL ) {
      u2=u1->next;
      destroyunit(u1);
      u1=u2;
    }
    u1=u->denominator;
    while ( u1!=NULL ) {
      u2=u1->next;
      destroyunit(u1);
      u1=u2;
    }
    free(u);
  }
}

/*
 *  Prefixes are only created, not destroyed
 */
static Prefix *createprefix(name, factor)
     char *name;        /*  name for prefix   */
     double factor;     /*  multiple factor (e.g., 1.0E3)  */
{
  Prefix *p;
  p = (Prefix *) malloc(sizeof(Prefix));
  p->name=stringset(NULL,name);
  p->factor=factor;
  p->next=NULL;
  return p;
}



/* 
 *-----------------------------------------------------------------
 *
 *   Error handling / Exception Handling 
 *
 *  The exported unit functions all set a failure code
 *  if there is a problem.  The caller must call
 *  units_clear_exception() before and units_check_exception()
 *  afterwards to get a text description of the problem.
 *
 *  The admittedly fragile code, here, is pattered after the C
 *  exception code described in the SWIG users manual.
 *
 *-----------------------------------------------------------------
 */

/*  return codes used internal to this module  */
#define SUCCESS 0
#define FAILURE 1

/*  static message buffer and error flag  */
#define ERRMSGLEN 256
static char error_message[ERRMSGLEN];
static int error_status = 0;

static void throw_exception(message)
     char *message;         /*  message string  */
{
  strncpy(error_message, message,ERRMSGLEN) ;
  error_status = 1;
}

static void throw_exception1(message, arg1)
     char *message;         /*  message string  */
     char *arg1;            /*  argument to format into message  */
{
  if ( strlen(message) + strlen(arg1) < ERRMSGLEN-1 ) {
      sprintf(error_message, message, arg1) ;
  } else {
    strncpy(error_message, message, ERRMSGLEN) ;
  }
  error_status = 1;
}

static void throw_exception2(message, arg1, arg2)
     char *message;         /*  message string  */
     char *arg1;            /*  argument to format into message  */
     char *arg2;            /*  argument to format into message  */
{
  if ( strlen(message) + strlen(arg1) + strlen(arg2) < ERRMSGLEN-1 ) {
      sprintf(error_message, message, arg1, arg2) ;
  } else {
    strncpy(error_message, message, ERRMSGLEN) ;
  }
  error_status = 1;
}

void units_clear_exception() {
  error_status = 0;
}
char *units_check_exception() {
  if (error_status) return error_message;
  else return NULL;
}



/*  new  */

void units_new ( name, baseUnits )
     char *name;           /*  name of new unit  */
     char *baseUnits;      /*  base units (must be reducible  */
{
  Unit *u ;
  Unit *newunit;

  /*  check for initialization  */
  if ( uroot == NULL ) {
    if ( unitInitialize() == FAILURE ) {
      return;
    }
  }

  /*  check for duplicates  */
  for (u=uroot; u!=NULL; u=u->next) {
    if ( strcmp(u->name,name) == 0 ) {
      throw_exception2("unit '%s' is already defined", name, NULL);
      return;
    }
  }

  /*  Check for valid characters  */
  if ( strspn(name,unitNameCharacters) != strlen(name) ) {
    throw_exception2("non-alphabetic characters in unit name '%s'",name,NULL);
    return;
  }

  /*  Compute reduced units  */
  newunit = reduceUnit( baseUnits ) ;
  if ( newunit == NULL ) {
    throw_exception2("'%s' cannot be reduced to primitive units",baseUnits,NULL);
    return;
  }

  /*  Add unit to list  */
  newunit->name = stringset(newunit->name, name);
  if ( uroot == NULL ) {
    uroot = newunit;
  } else {
    /* find end of list */
    for (u=uroot; u->next!=NULL; u=u->next) { 
      /* do nothing */
    }
    u->next = newunit;
  }
  return;
}


/*  convert  */

double units_convert ( value, targetUnits )
     char *value;        /*  value string to convert  */
     char *targetUnits;  /*  target units for conversion  */
{
  Unit *rValue;     /*  reduced value unit   */
  Unit *rTarget;    /*  reduced target unit  */
  double result;    /*  returned value       */

  /*  check for initialization  */
  if ( uroot == NULL ) {
    if ( unitInitialize() == FAILURE ) {
      /* exception code has already been set, so just return */
      return 0.0;
    }
  }

  /*  Reduce each of value and target to primitive units */
  rValue = reduceUnit( value ) ;
  rTarget = reduceUnit( targetUnits ) ;
  if ( (rValue==NULL) || (rTarget==NULL) ) {
    destroyunit( rValue ) ;
    destroyunit( rTarget ) ;
    /* exception code has already been set, so just return */
    return 0.0;
  }

  /*
   *  If the value has units, it must be compatible with
   *  the target.  (If it is unitless, then compatibility
   *  is not required.
   */
  if ( (rValue->numerator!=NULL) || (rValue->denominator!=NULL) ) {
    Unit *u1, *u2;
    for (u1=rValue->numerator, u2=rTarget->numerator;
	 (u1!=NULL)&&(u2!=NULL); u1=u1->next, u2=u2->next) {
      if ( strcmp(u1->name,u2->name) != 0 ) {
	break;
      }
    }
    if ( (u1!=NULL) || (u2!=NULL) ) {
      throw_exception2("'%s' and '%s' have incompatible units",
		       value,targetUnits);
      destroyunit(rValue);
      destroyunit(rTarget);
      return 0.0;
    }
    for (u1=rValue->denominator, u2=rTarget->denominator;
	 (u1!=NULL)&&(u2!=NULL); u1=u1->next, u2=u2->next) {
      if ( strcmp(u1->name,u2->name) != 0 ) {
	break;
      }
    }
    if ( (u1!=NULL) || (u2!=NULL) ) {
      throw_exception2("'%s' and '%s' have incompatible units",
		       value,targetUnits);
      destroyunit(rValue);
      destroyunit(rTarget);
      return 0.0;
    }
  }


  /*  Compute and return scaled value  */
  result = rValue->factor / rTarget->factor;
  destroyunit(rValue);
  destroyunit(rTarget);
  return result;
}


/*  reduce  */

char *units_reduce ( unitString )
     char *unitString;     /*  unit string to reduce  */
{
  static char *returnValue=NULL;

  Unit *u;    /* reduced unitString  */
  Unit *uptr; /* pointer for traversing u's numer & denomin  */

  /*  check for initialization  */
  if ( uroot == NULL ) {
    if ( unitInitialize() == FAILURE ) {
      return NULL;
    }
  }

  u = reduceUnit(unitString) ;
  if ( u==NULL ) {
    /* exception code has already been set, so just return */
    return NULL;
  }
 
  /*  Assemble reduced unit string from Unit structure  */
  returnValue = stringset(returnValue,"                         ");
  sprintf(returnValue, "%lg", u->factor);
  if ( strcspn(returnValue,".eE") == strlen(returnValue) ) {
    returnValue = stringappend(returnValue,".0");
  }
  for (uptr=u->numerator; uptr!=NULL; uptr=uptr->next) {
    returnValue = stringappend(returnValue," ");
    returnValue = stringappend(returnValue,uptr->name);
  }
  if ( u->denominator != NULL ) {
    returnValue = stringappend(returnValue," /");
    for (uptr=u->denominator; uptr!=NULL; uptr=uptr->next) {
      returnValue = stringappend(returnValue," ");
      returnValue = stringappend(returnValue,uptr->name);
    }
  }

  destroyunit(u);

  /*  return value (through parameter)  */
  return returnValue;
}



/*  local functions  */

static Unit *reduceUnit ( unitString ) 
     char *unitString;  /*  unit string to reduce  */
{
  Unit *u;                 /*  new reduced unit  */
  char *cp;                /*  for traversing strings  */
  char *tokenString;       /*  tokenized copy of unitString  */
  int numeratorlen;        /*  for splitting unitString  */
  char *subunit;           /*  one token from tokenString  */
  char *subunitcopy;       /*  copy of subunit, for plural chopping  */
  double factor;           /*  value of constant factor subunits  */
  int numerflag = 1;       /*  boolean switch between numer/denom  */
  int exponent;            /*  subunit exponent value  */
  int e;                   /*  loop counter for exponents  */
  int len, pos;            /*  for separating prefix and unit name  */
  Unit *up1, *up2;         /*  for searching the unit list  */
  Prefix *p;               /*  for walking prefix list  */
  Unit *usub;              /*  new subunits (added to u)  */


  /*  check for initialization  */
  if ( uroot == NULL ) {
    if ( unitInitialize() == FAILURE ) {
      return NULL;
    }
  }

  /* create the Unit to return  */
  u = createunit();

  /* check for primitive unit */
  if ( strcmp(unitString,primitivestring) == 0 ) {
    u->name = stringset(u->name,unitString);
    u->primitive = 1;
    return u;
  }

  /*  verify syntax of unit string   */

  /*  it may contain at most one "/"  */
  if ( (cp=strpbrk(unitString,divider)) != NULL ) {
    if ( strpbrk(cp+1,divider) != NULL ) {
      throw_exception2("invalid unit string '%s':  only one '%s' allowed",
		       unitString, divider);
      destroyunit(u);
      return NULL;
    }
  }
  
  /*  it may contain only letters, digits, the power character ('^'),
      decimal points, digits, +- signs, and separators
  */
  if ( strspn(unitString,validchars) < strlen(unitString) ) {
    throw_exception1("invalid characters in unit string '%s'",unitString);
    destroyunit(u);
    return NULL;
  }

  /*  Disassemble string into scale factor and numerator/denominator  */

  /*  get factor
      This is a little tricky because strtod() could skip white space,
      then not find a valid float, and return 0.0.  So we do a little
      manual whitespace-skipping to ensure that we are really getting
      a float value, or not.
  */
  while ( isspace(*unitString) ) unitString++;
  u->factor = strtod(unitString,&cp);
  if (  cp == unitString ) {
    /* no leading factor (we got a 0.0 value - fix it)  */
    u->factor = 1.0;
  } else {
    /* we've capture the factor - skip over characters */
    unitString = cp;
  }

  
  /*  Make a copy of unitString, and replace any "/" with " / "  */
  tokenString = stringcopy(unitString);
  numeratorlen = strcspn(tokenString,divider);
  if ( numeratorlen < strlen(tokenString) ) {
    tokenString[numeratorlen] = '\0';
    tokenString = stringappend(tokenString," ");
    tokenString = stringappend(tokenString,divider);
    tokenString = stringappend(tokenString," ");
    tokenString = stringappend(tokenString,unitString+numeratorlen+1);
  }


  /*  process each subunit in turn, starting in the numerator
   
      Note that we're going to use a boolean flag to switch
      between numerator and denominator if we encounter a "/".
      This same style is used for processing recursively
      reduced subunits
  */
  for (subunit=strtok(tokenString,separators); subunit!=NULL; 
                        subunit=strtok(NULL,separators) ) {

    /* check for null subunit  */
    if ( strlen(subunit) == 0 ) {
      continue;
    }

    /*  check for "/"  */
    if ( strcmp(subunit,divider) == 0 ) {
      numerflag = numerflag ? 0 : 1 ;
      continue;
    }

    /*  constant factor  */

    factor = strtod(subunit,&cp);
    if ( cp > subunit ) {
      if ( factor == 0.0 ) {
	throw_exception1("illegal zero factor in '%s'",unitString);
	destroyunit(u);
	free(tokenString);
	return NULL;
      }
      if ( numerflag ) {
	u->factor *= factor;
      } else {
	u->factor /= factor;
	continue;
      }
    }

    /*  Check for power character (e.g., "s^2")  */
    cp=strchr(subunit,powerchar);
    if ( cp != NULL ) {
      *cp = '\0';  /* terminate subunit name  */
      exponent = strtol(++cp,&cp,10);
      if ( (*cp != '\0') || (exponent <= 0) ) {
	throw_exception1("invalid integer exponent in '%s'",unitString);
	destroyunit(u);
	free(tokenString);
	return NULL;
      }
    } else {
      exponent = 1;
    }

    /*  Check subunit name syntax  */
    if ( strspn(subunit,unitNameCharacters) != strlen(subunit) ) {
      throw_exception1("invalid non-alphabetic unit name in '%s'",unitString);
      destroyunit(u);
      free(tokenString);
      return NULL;
    }


    /*  Search the unit list in order, because we 
	wouldn't want to accidentally match the "m" 
	at the end of "gram" and conclude that we 
	have "meter".   */
    len = strlen(subunit);
    p = NULL;

    for (up1=uroot; up1!=NULL; up1=up1->next) {

      /*  Try to match the string starting at the
	  at the end, just in case there is a prefix.
	  We only have a match if both the prefix and
	  unit name are exact matches.  */
      pos = len - strlen(up1->name);
      if ( strcmp(subunit+pos,up1->name) == 0 ) {
	/* zero length prefix is OK */
	if ( pos == 0 ) {
	  break;
	}
	/* otherwise, search prefix list */
	for (p=proot; p!=NULL; p=p->next) {
	  if ( (strlen(p->name) == pos) &&
	       (strncmp(subunit,p->name,pos) == 0) ) {
	    break;
	  }
	}
	if ( p != NULL ) {
	  break;
	}
      }
    }

    /*  If we didn't find a match, then try again
	if there is a trailing "s".
    */
    if ( up1==NULL && subunit[len-1]=='s' ) {
      subunitcopy=stringcopy(subunit);
      len--;
      subunitcopy[len] = '\0';
      for (up1=uroot; up1!=NULL; up1=up1->next) {
	pos = len - strlen(up1->name);
	if ( strcmp(subunitcopy+pos,up1->name) == 0 ) {
	  /* zero length prefix is OK */
	  if ( pos == 0 ) {
	    break;
	  }
	  /* otherwise, search prefix list */
	  for (p=proot; p!=NULL; p=p->next) {
	    if ( strncmp(subunitcopy,p->name,pos) == 0 ) {
	      break;
	    }
	  }
	  if ( p != NULL ) {
	    break;
	  }
	}
      }

      /*  If we still didn't find a match, then try again
	  if there is a trailing "e" (which was a
	  trailing "es".)
      */
      if ( up1==NULL && subunit[len-1]=='e' ) {
	len--;
	subunitcopy[len] = '\0';
	for (up1=uroot; up1!=NULL; up1=up1->next) {
	  pos = len - strlen(up1->name);
	  if ( strcmp(subunitcopy+pos,up1->name) == 0 ) {
	    /* zero length prefix is OK */
	    if ( pos == 0 ) {
	      break;
	    }
	    /* otherwise, search prefix list */
	    for (p=proot; p!=NULL; p=p->next) {
	      if ( strncmp(subunitcopy,p->name,pos) == 0 ) {
		break;
	      }
	    }
	    if ( p != NULL ) {
	      break;
	    }
	  }
	}
      } /* end if ends in "es" */
      free(subunitcopy);
    } /* end if ends in "s" */
  

    /*  If we fell through to here, without a match,
	then we really have a bad unit name
    */
    if ( up1==NULL ) {
      throw_exception2("invalid unit name '%s' in '%s'", subunit, unitString);
      destroyunit(u);
      free(tokenString);
      return NULL;
    }


    /*  Multiply the factor by prefix factor ('exponent' times)  */
    if ( p!=NULL ) {
      for (e=0; e<exponent; e++) {
	if ( numerflag ) {
	  u->factor *= p->factor;
	} else {
	  u->factor /= p->factor;
	}
      }
    }


    /*  Is this a primitive subunint?  */
    if ( up1->primitive ) {
      /*  just append the matching subunit to
	  the result. 
	  Actually, we can get away with inserting
	  at the head of the numerator/denominator
	  lists because we will sort them later.
      */
      for (e=0; e<exponent; e++) {
	usub=createunit();
	usub->name = stringset(usub->name,up1->name);
	if ( numerflag ) {
	  usub->next=u->numerator;
	  u->numerator=usub;
	} else {
	  usub->next=u->denominator;
	  u->denominator=usub;
	}
      }
    } else {
      /*  Recursively reduce.  */
      /*  except that this is already reduced  */
      /* up1 = reduceUnit( subunit ); */

      /*  Include multiple factor from reduced unit  */
      for (e=0; e<exponent; e++) {
	if ( numerflag ) {
	  u->factor *= up1->factor;
	} else {
	  u->factor /= up1->factor;
	}
      }
    
      /*  Add primitive subunits to numerator/denominator
	  
	  Note that we're use a nested boolean flag to switch
	  between numerator and denominator.  Subunits in
	  the numerator of the unitString are processed
	  normally, but subunits in the denominator of
	  unitString must be inverted.

	  Editors note:  This inverting numerator/denominator
	  flag business looks a little odd translating from
	  the reference (Tcl) implementation to here.  Please
	  excuse the very un-C-like verbosity.
      */
      for (e=0; e<exponent; e++) {
	for (up2=up1->numerator; up2!=NULL; up2=up2->next) {
	  usub=createunit();
	  usub->name=stringset(usub->name,up2->name);
	  if ( numerflag ) {
	    usub->factor *= up2->factor;
	    usub->next=u->numerator;
	    u->numerator=usub;
	  } else {
	    usub->factor /= up2->factor;
	    usub->next=u->denominator;
	    u->denominator=usub;
	  }
	}
	for (up2=up1->denominator; up2!=NULL; up2=up2->next) {
	  usub=createunit();
	  usub->name=stringset(usub->name,up2->name);
	  if ( numerflag ) {
	    usub->factor /= up2->factor;
	    usub->next=u->denominator;
	    u->denominator=usub;
	  } else {
	    usub->factor *= up2->factor;
	    usub->next=u->numerator;
	    u->numerator=usub;
	  }
	}
      }
 
    } /* end if non-primitive unit */

  } /* end for each subunit */

  /*  Sort both numerator and denominator  */
  u->numerator = sortUnitList(u->numerator);
  u->denominator = sortUnitList(u->denominator);

  /*  Cancel any duplicate units.  */
  cancelUnitList(&u->numerator,&u->denominator);



  /*  delete string and return */
  free(tokenString);
  return u;


}


static Unit *sortUnitList( u )
     Unit *u;   /*  linked list of units to sort  */
{
  /*
   *  The author can barely comprehend the necessity
   *  for writing list sorting code in this day
   *  and age.  *sigh*
   *
   *  Even though the lists are *very* short,
   *  copy the pointers into an array, then sort
   *  the pointers.
   */
  int len;
  Unit *u1;
  Unit **a;
  int i, j, inserted;

  /* count list length */
  len=0;
  for (u1=u; u1!=NULL; u1=u1->next) {
    len++;
  }
  /*  do we really have to do this?  */
  if ( len < 2 ) {
    return u;
  }
  /* copy list to array */
  a = (Unit**) malloc((size_t)len*sizeof(Unit*));
  len=0;
  for (u1=u; u1!=NULL; u1=u1->next) {
    *(a+len)=u1;
    len++;
  }
  /* 
   *  Sort the array of pointers using an insertion sort.
   *  This isn't really any more complicated than writing
   *  the comparison function and calling qsort().
   */
  for (i = 1; i < len; i++) {
    inserted = 0;
    j = i;
    while ((j >= 1) && (inserted == 0)) {
      if (strcmp(a[j]->name,a[j-1]->name)<0) {
	Unit *tmp;
	tmp=a[j];
	a[j]=a[j-1];
	a[j-1]=tmp;
      } else {
	inserted = 1;
      }
      j--;
    }
  }
  /* reassemble the linked list */
  for (i=0; i<len-1; i++) {
    a[i]->next=a[i+1];
  }
  a[len-1]->next=NULL;
  u1=a[0];
  free(a);
  return u1;
}

static void cancelUnitList(numerator, denominator)
     Unit **numerator;    /*  numerator list of units  */
     Unit **denominator;  /*  denominator list of units  */
{
  Unit *n1, *n2, *d1, *d2;
  /* 
   *  March through the list with dual pointers so we
   *  can keep the forward-only links intact.
   */
  n1=NULL;
  n2=*numerator;
  while (n2!=NULL) {
    
    /* search denominator for matching unit */
    d1=NULL;
    d2=*denominator;
    while (d2!=NULL) {
      if ( strcmp(n2->name,d2->name) == 0 ) {
	/*  delete unit from both lists  */
	if (n1==NULL) {
	  *numerator=n2->next;
	  destroyunit(n2);
	  n2=*numerator;
	} else {
	  n1->next=n2->next;
	  destroyunit(n2);
	  n2=n1->next;
	}
	if (d1==NULL) {
	  *denominator=d2->next;
	  destroyunit(d2);
	  d2=*denominator;
	} else {
	  d1->next=d2->next;
	  destroyunit(d2);
	  d2=d1->next;
	}
	break;
      } else {
	/* look to next denominator unit */
	d1=d2;
	d2=d2->next;
      }
    } /* end while searching denominator */

    /* 
     *  If we ran off end of denominator, move on to
     *  next numerator element.  If not, then we fell
     *  out of the loops after deleting a subunit, so
     *  we can't increment the numerator pointers.
     */
    if ( d2==NULL ) {
      /*  have all numerators been deleted?  */
      if ( n2 != NULL ) {
	n1=n2;
	n2=n2->next;
      }
    }
  }
}

/*------------------------------------------------------------
 *
 *  UNIT table
 *
 *  Make sure that you change the unitcount variable if 
 *  you change the unit initialization.
 *
 *  Users can add units to this table with the function 
 *  int newUnitConversion( const char* unitName, const char* baseUnits)
 *
 *  Note that unit strings are parsed from the end using a first-
 *  match comparison.  The table should list the long unit names
 *  first, then abbreviations, such that a short match won't
 *  override a long match.  For example, the "s" abbreviation for 
 *  "seconds" should come after the "siemens" unit.  It is OK
 *  to define units in terms of other units' abbreviations, even
 *  though they appear later in the table.
 *
 *  The SI units included here are described in NIST Special
 *  Publication 330, 1991 Edition, "The Interational System of Units
 *  (SI)" and NIST Special Publication 811, 1995 Edition, "Guide for
 *  the Use of the International System of Units (SI)".  Note that the
 *  units table defines gram instead of kilogram because the prefix
 *  rules define 1e-6kg = 1mg (and NOT 1ukg).  Also note that an
 *  abbreviation for "ohm" is not included because you can't easily
 *  type an omega.
 *
 *------------------------------------------------------------*/


/* SI Base Units */
/* SI Supplementary Units */
static char *baseUnits[] = {
  "meter",
  "gram",
  "second",
  "ampere",
  "kelvin",
  "mole",
  "candela",
  NULL};

/*  SI Derived Units with Special Names  */
static char *derivedUnits[] = {
  "radian",	"meter/meter",
  "steradian",	"meter^2/meter^2",
  "hertz",	"/second",
  "newton",	"meter-kilogram/second^2",
  "pascal",	"kilogram/meter-second^2",
  "joule",	"meter^2-kilogram/second^2",
  "watt",	"meter^2-kilogram/second^3",
  "coulomb",	"second-ampere",
  "volt",	"meter^2-kilogram/second^3-ampere",
  "farad",	"second^4-ampere^2/meter^2-kilogram",
  "ohm",	"meter^2-kilogram/second^3-ampere^2",
  "siemens",	"second^3-ampere^2/meter^2-kilogram",
  "weber",	"meter^2-kilogram/second^2-ampere",
  "tesla",	"kilogram/second^2-ampere",
  "henry",	"meter^2-kilogram/second^2-ampere^2",
  "lumen",	"candela-steradian",
  "lux",	"candela-steradian/meter^2",
  "becquerel",	"/second",
  "gray",	"meter^2/second^2",
  "sievert",	"meter^2/second^2",
  NULL,         NULL};

/*  Non-SI units  */
static char *nonSIUnits[] = {
  "angstrom",	"1.0E-10meter",
  "astronomicalUnit",	"1.495979E11meter",
  "atmosphere",	"1.01325E5pascal",
  "bar",	"1.0E5pascal",
  "calorie",	"4.1868joule",
  "curie",	"3.7E10becquerel",
  "day",	"8.64E4second",
  "degree",	"1.745329E-2radian",
  "erg",	"1.0E-7joule",
  "faraday",	"9.648531coulomb",
  "fermi",	"1.0E-15meter",
  "foot",	"3.048E-1meter",
  "gauss",	"1.0E-4tesla",
  "gilbert",	"7.957747E-1ampere",
  "grain",	"6.479891E-5kilogram",
  "hectare",	"1.0E4meter^2",
  "hour",	"3.6E3second",
  "inch",	"2.54E-2meter",
  "lightYear",	"9.46073E15meter",
  "liter",	"1.0E-3meter^3",
  "maxwell",	"1.0E-8weber",
  "mho",	"1.0siemens",
  "micron",	"1.0E-6meter",
  "mil",	"2.54E-5meter",
  "mile",	"1.609344E3meter",
  "minute",	"6.0E1second",
  "parsec",	"3.085E16meter",
  "pica",	"4.233333E-3meter",
  "pound",	"4.535924E-1kilogram",
  "revolution",	"6.283185radian",
  "revolutionPerMinute",	"1.047198E-1radian/second",
  "yard",	"9.144E-1meter",
  "year",	"3.1536E7second",
  NULL,         NULL};




/*  Abbreviations  */
static char *abbreviations[] = {
  "m",	"meter",
  "g",	"gram",
  "s",	"second",
  "A",	"ampere",
  "K",	"kelvin",
  "mol","mole",
  "cd",	"candela",
  "rad","radian",
  "sr",	"steradian",
  "Hz",	"hertz",
  "N",	"newton",
  "Pa",	"pascal",
  "J",	"joule",
  "W",	"watt",
  "C",	"coulomb",
  "V",	"volt",
  "F",	"farad",
  "S",	"siemens",
  "Wb",	"weber",
  "T",	"tesla",
  "H",	"henry",
  "lm",	"lumen",
  "lx",	"lux",
  "Bq",	"becquerel",
  "Gy",	"gray",
  "Sv",	"sievert",
  NULL, NULL};

static char *nonSIabbreviations[] = {
  "AU",	"astronomicalUnit",
  "ft",	"foot",
  "gr",	"grain",
  "ha",	"hectare",
  "h",	"hour",
  "in",	"inch",
  "L",	"liter",
  "Mx",	"maxwell",
  "mi",	"mile",
  "min","minute",
  "pc",	"parsec",
  "lb",	"pound",
  "r",	"revolution",
  "rpm","revolutionPerMinute",
  "yd",	"yard",
  NULL,   NULL};


/*------------------------------------------------------------
 *
 *  PREFIX table
 *
 *  Make sure that you change the prefixcount variable if 
 *  you change the prefix initialization.
 *
 *  Note that prefix strings are parsed from the beginning using a
 *  first-match comparison.  The table should list the long prefixes
 *  first, then abbreviations, such that a short match won't
 *  override a long match.  For example, the "m" abbreviation for 
 *  "milli" should come after both "milli" and "micro".
 *
 *  The SI prefixes included here are described in NIST Special
 *  Publication 330, 1991 Edition, "The Interational System of
 *  Units (SI)".  Note that we support the non-USA spelling of 
 *  deka- (deca-) and that we take the liberty of allowing
 *  the "micro-" prefix to be typed with a "u".
 *
 *------------------------------------------------------------*/

static char *prefixList[] = {
  "yotta",	"1e24",
  "zetta",	"1e21",
  "exa",	"1e18",
  "peta",	"1e15",
  "tera",	"1e12",
  "giga",	"1e9",	
  "mega",	"1e6",	
  "kilo",	"1e3",	
  "hecto",	"1e2",
  "deka",	"1e1",	
  "deca",	"1e1",	
  "deci",	"1e-1",
  "centi",	"1e-2",
  "milli",	"1e-3",
  "micro",	"1e-6",
  "nano",	"1e-9",
  "pico",	"1e-12",
  "femto",	"1e-15",
  "atto",	"1e-18",
  "zepto",	"1e-21",
  "yocto",	"1e-24",
  "Y",	"1e24",
  "Z",	"1e21",
  "E",	"1e18",
  "P",	"1e15",
  "T",	"1e12",
  "G",	"1e9",
  "M",	"1e6",
  "k",	"1e3",
  "h",	"1e2",
  "da",	"1e1",
  "d",	"1e-1",
  "c",	"1e-2",
  "m",	"1e-3",
  "u",	"1e-6",
  "n",	"1e-9",
  "p",	"1e-12",
  "f",	"1e-15",
  "a",	"1e-18",
  "z",	"1e-21",
  "y",	"1e-24",
  NULL, NULL};

static int unitInitialize() {
  Unit *u, *up;
  Prefix *p, *pp;
  char **name, **value;

  /*  Load the Prefix List  */
  for (name=prefixList,value=name+1; *name!=NULL; name+=2,value+=2) {
    p = createprefix(*name,atof(*value));
    if ( p == NULL ) {
      throw_exception2("units failure initializing prefix '%s' (%s)", 
		       *name, *value);
      return FAILURE;
    }
    if ( proot == NULL ) {
      proot = p;
      pp = p;
    } else {
      pp->next = p;
      pp = p;
    }
  }

  /*  Load Units List with Base (primitive) units  */
  for (name=baseUnits; *name!=NULL; name++) {
    u = createunit();
    if ( u == NULL ) {
      throw_exception1("units failure initializing primitive unit '%s'", 
		       *name);
      return FAILURE;
    }
    u->name = stringset(u->name,*name);
    u->primitive = 1;
    if ( uroot == NULL ) {
      uroot = u;        /* start new uroot */
      up = u;
    } else {
      up->next = u;     /* append to end of list */
      up = u;
    }
  }

  /*  Load Units List with Derived Units  */
  for (name=derivedUnits,value=name+1; *name!=NULL; name+=2,value+=2) {
    u = reduceUnit(*value);
    if ( u == NULL ) {
      throw_exception2("units failure initializing derived unit '%s' (%s)", 
		       *name, *value);
      return FAILURE;
    }
    u->name = stringset(u->name,*name);
    up->next = u;
    up = u;
  }
 
  /*  Load Units List with Non-SI Units  */
  for (name=nonSIUnits,value=name+1; *name!=NULL; name+=2,value+=2) {
    u = reduceUnit(*value);
    if ( u == NULL ) {
      throw_exception2("units failure initializing non-SI unit '%s' (%s)", 
		       *name, *value);
      return FAILURE;
    }
    u->name = stringset(u->name,*name);
    up->next = u;
    up = u;
  }
 
  /*  Load Units List with Abbreviations  */
  for (name=abbreviations,value=name+1; *name!=NULL; name+=2,value+=2) {
    u = reduceUnit(*value);
    if ( u == NULL ) {
      throw_exception2("units failure initializing abbreviation '%s' (%s)", 
		       *name, *value);
      return FAILURE;
    }
    u->name = stringset(u->name,*name);
    up->next = u;
    up = u;
  }

  /*  Load Units List with non-SI Abbreviations  */
  for (name=nonSIabbreviations,value=name+1; *name!=NULL; name+=2,value+=2) {
    u = reduceUnit(*value);
    if ( u == NULL ) {
      throw_exception2("units failure initializing non-SI abbreviation '%s' (%s)", 
		       *name, *value);
      return FAILURE;
    }
    u->name = stringset(u->name,*name);
    up->next = u;
    up = u;
  }

  return SUCCESS;
}




