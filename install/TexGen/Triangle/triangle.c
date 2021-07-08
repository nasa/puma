/*****************************************************************************/
/*                                                                           */
/*      888888888        ,o,                          / 888                  */
/*         888    88o88o  "    o8888o  88o8888o o88888o 888  o88888o         */
/*         888    888    888       88b 888  888 888 888 888 d888  88b        */
/*         888    888    888  o88^o888 888  888 "88888" 888 8888oo888        */
/*         888    888    888 C888  888 888  888  /      888 q888             */
/*         888    888    888  "88o^888 888  888 Cb      888  "88oooo"        */
/*                                              "8oo8D                       */
/*                                                                           */
/*  A Two-Dimensional Quality Mesh Generator and Delaunay Triangulator.      */
/*  (triangle.c)                                                             */
/*                                                                           */
/*  Version 1.6                                                              */
/*  July 28, 2005                                                            */
/*                                                                           */
/*  Copyright 1993, 1995, 1997, 1998, 2002, 2005                             */
/*  Jonathan Richard Shewchuk                                                */
/*  2360 Woolsey #H                                                          */
/*  Berkeley, California  94705-1927                                         */
/*  jrs@cs.berkeley.edu                                                      */
/*                                                                           */
/*  This program may be freely redistributed under the condition that the    */
/*    copyright notices (including this entire header and the copyright      */
/*    notice printed when the `-h' switch is selected) are not removed, and  */
/*    no compensation is received.  Private, research, and institutional     */
/*    use is free.  You may distribute modified versions of this code UNDER  */
/*    THE CONDITION THAT THIS CODE AND ANY MODIFICATIONS MADE TO IT IN THE   */
/*    SAME FILE REMAIN UNDER COPYRIGHT OF THE ORIGINAL AUTHOR, BOTH SOURCE   */
/*    AND OBJECT CODE ARE MADE FREELY AVAILABLE WITHOUT CHARGE, AND CLEAR    */
/*    NOTICE IS GIVEN OF THE MODIFICATIONS.  Distribution of this code as    */
/*    part of a commercial system is permissible ONLY BY DIRECT ARRANGEMENT  */
/*    WITH THE AUTHOR.  (If you are not directly supplying this code to a    */
/*    customer, and you are instead telling them how they can obtain it for  */
/*    free, then you are not required to make any arrangement with me.)      */
/*                                                                           */
/*  Hypertext instructions for Triangle are available on the Web at          */
/*                                                                           */
/*      http://www.cs.cmu.edu/~quake/triangle.html                           */
/*                                                                           */
/*  Disclaimer:  Neither I nor Carnegie Mellon warrant this code in any way  */
/*    whatsoever.  This code is provided "as-is".  Use at your own risk.     */
/*                                                                           */
/*  Some of the references listed below are marked with an asterisk.  [*]    */
/*    These references are available for downloading from the Web page       */
/*                                                                           */
/*      http://www.cs.cmu.edu/~quake/triangle.research.html                  */
/*                                                                           */
/*  Three papers discussing aspects of Triangle are available.  A short      */
/*    overview appears in "Triangle:  Engineering a 2D Quality Mesh          */
/*    Generator and Delaunay Triangulator," in Applied Computational         */
/*    Geometry:  Towards Geometric Engineering, Ming C. Lin and Dinesh       */
/*    Manocha, editors, Lecture Notes in Computer Science volume 1148,       */
/*    pages 203-222, Springer-Verlag, Berlin, May 1996 (from the First ACM   */
/*    Workshop on Applied Computational Geometry).  [*]                      */
/*                                                                           */
/*    The algorithms are discussed in the greatest detail in "Delaunay       */
/*    Refinement Algorithms for Triangular Mesh Generation," Computational   */
/*    Geometry:  Theory and Applications 22(1-3):21-74, May 2002.  [*]       */
/*                                                                           */
/*    More detail about the data structures may be found in my dissertation: */
/*    "Delaunay Refinement Mesh Generation," Ph.D. thesis, Technical Report  */
/*    CMU-CS-97-137, School of Computer Science, Carnegie Mellon University, */
/*    Pittsburgh, Pennsylvania, 18 May 1997.  [*]                            */
/*                                                                           */
/*  Triangle was created as part of the Quake Project in the School of       */
/*    Computer Science at Carnegie Mellon University.  For further           */
/*    information, see Hesheng Bao, Jacobo Bielak, Omar Ghattas, Loukas F.   */
/*    Kallivokas, David R. O'Hallaron, Jonathan R. Shewchuk, and Jifeng Xu,  */
/*    "Large-scale Simulation of Elastic Wave Propagation in Heterogeneous   */
/*    Media on Parallel Computers," Computer Methods in Applied Mechanics    */
/*    and Engineering 152(1-2):85-102, 22 January 1998.                      */
/*                                                                           */
/*  Triangle's Delaunay refinement algorithm for quality mesh generation is  */
/*    a hybrid of one due to Jim Ruppert, "A Delaunay Refinement Algorithm   */
/*    for Quality 2-Dimensional Mesh Generation," Journal of Algorithms      */
/*    18(3):548-585, May 1995 [*], and one due to L. Paul Chew, "Guaranteed- */
/*    Quality Mesh Generation for Curved Surfaces," Proceedings of the Ninth */
/*    Annual Symposium on Computational Geometry (San Diego, California),    */
/*    pages 274-280, Association for Computing Machinery, May 1993,          */
/*    http://portal.acm.org/citation.cfm?id=161150 .                         */
/*                                                                           */
/*  The Delaunay refinement algorithm has been modified so that it meshes    */
/*    domains with small input angles well, as described in Gary L. Miller,  */
/*    Steven E. Pav, and Noel J. Walkington, "When and Why Ruppert's         */
/*    Algorithm Works," Twelfth International Meshing Roundtable, pages      */
/*    91-102, Sandia National Laboratories, September 2003.  [*]             */
/*                                                                           */
/*  My implementation of the divide-and-conquer and incremental Delaunay     */
/*    triangulation algorithms follows closely the presentation of Guibas    */
/*    and Stolfi, even though I use a triangle-based data structure instead  */
/*    of their quad-edge data structure.  (In fact, I originally implemented */
/*    Triangle using the quad-edge data structure, but the switch to a       */
/*    triangle-based data structure sped Triangle by a factor of two.)  The  */
/*    mesh manipulation primitives and the two aforementioned Delaunay       */
/*    triangulation algorithms are described by Leonidas J. Guibas and Jorge */
/*    Stolfi, "Primitives for the Manipulation of General Subdivisions and   */
/*    the Computation of Voronoi Diagrams," ACM Transactions on Graphics     */
/*    4(2):74-123, April 1985, http://portal.acm.org/citation.cfm?id=282923 .*/
/*                                                                           */
/*  Their O(n log n) divide-and-conquer algorithm is adapted from Der-Tsai   */
/*    Lee and Bruce J. Schachter, "Two Algorithms for Constructing the       */
/*    Delaunay Triangulation," International Journal of Computer and         */
/*    Information Science 9(3):219-242, 1980.  Triangle's improvement of the */
/*    divide-and-conquer algorithm by alternating between vertical and       */
/*    horizontal cuts was introduced by Rex A. Dwyer, "A Faster Divide-and-  */
/*    Conquer Algorithm for Constructing Delaunay Triangulations,"           */
/*    Algorithmica 2(2):137-151, 1987.                                       */
/*                                                                           */
/*  The incremental insertion algorithm was first proposed by C. L. Lawson,  */
/*    "Software for C1 Surface Interpolation," in Mathematical Software III, */
/*    John R. Rice, editor, Academic Press, New York, pp. 161-194, 1977.     */
/*    For point location, I use the algorithm of Ernst P. Mucke, Isaac       */
/*    Saias, and Binhai Zhu, "Fast Randomized Point Location Without         */
/*    Preprocessing in Two- and Three-Dimensional Delaunay Triangulations,"  */
/*    Proceedings of the Twelfth Annual Symposium on Computational Geometry, */
/*    ACM, May 1996.  [*]  If I were to randomize the order of vertex        */
/*    insertion (I currently don't bother), their result combined with the   */
/*    result of Kenneth L. Clarkson and Peter W. Shor, "Applications of      */
/*    Random Sampling in Computational Geometry II," Discrete &              */
/*    Computational Geometry 4(1):387-421, 1989, would yield an expected     */
/*    O(n^{4/3}) bound on running time.                                      */
/*                                                                           */
/*  The O(n log n) sweepline Delaunay triangulation algorithm is taken from  */
/*    Steven Fortune, "A Sweepline Algorithm for Voronoi Diagrams",          */
/*    Algorithmica 2(2):153-174, 1987.  A random sample of edges on the      */
/*    boundary of the triangulation are maintained in a splay tree for the   */
/*    purpose of point location.  Splay trees are described by Daniel        */
/*    Dominic Sleator and Robert Endre Tarjan, "Self-Adjusting Binary Search */
/*    Trees," Journal of the ACM 32(3):652-686, July 1985,                   */
/*    http://portal.acm.org/citation.cfm?id=3835 .                           */
/*                                                                           */
/*  The algorithms for exact computation of the signs of determinants are    */
/*    described in Jonathan Richard Shewchuk, "Adaptive Precision Floating-  */
/*    Point Arithmetic and Fast Robust Geometric Predicates," Discrete &     */
/*    Computational Geometry 18(3):305-363, October 1997.  (Also available   */
/*    as Technical Report CMU-CS-96-140, School of Computer Science,         */
/*    Carnegie Mellon University, Pittsburgh, Pennsylvania, May 1996.)  [*]  */
/*    An abbreviated version appears as Jonathan Richard Shewchuk, "Robust   */
/*    Adaptive Floating-Point Geometric Predicates," Proceedings of the      */
/*    Twelfth Annual Symposium on Computational Geometry, ACM, May 1996. [*] */
/*    Many of the ideas for my exact arithmetic routines originate with      */
/*    Douglas M. Priest, "Algorithms for Arbitrary Precision Floating Point  */
/*    Arithmetic," Tenth Symposium on Computer Arithmetic, pp. 132-143, IEEE */
/*    Computer Society Press, 1991.  [*]  Many of the ideas for the correct  */
/*    evaluation of the signs of determinants are taken from Steven Fortune  */
/*    and Christopher J. Van Wyk, "Efficient Exact Arithmetic for Computa-   */
/*    tional Geometry," Proceedings of the Ninth Annual Symposium on         */
/*    Computational Geometry, ACM, pp. 163-172, May 1993, and from Steven    */
/*    Fortune, "Numerical Stability of Algorithms for 2D Delaunay Triangu-   */
/*    lations," International Journal of Computational Geometry & Applica-   */
/*    tions 5(1-2):193-213, March-June 1995.                                 */
/*                                                                           */
/*  The method of inserting new vertices off-center (not precisely at the    */
/*    circumcenter of every poor-quality triangle) is from Alper Ungor,      */
/*    "Off-centers:  A New Type of Steiner Points for Computing Size-Optimal */
/*    Quality-Guaranteed Delaunay Triangulations," Proceedings of LATIN      */
/*    2004 (Buenos Aires, Argentina), April 2004.                            */
/*                                                                           */
/*  For definitions of and results involving Delaunay triangulations,        */
/*    constrained and conforming versions thereof, and other aspects of      */
/*    triangular mesh generation, see the excellent survey by Marshall Bern  */
/*    and David Eppstein, "Mesh Generation and Optimal Triangulation," in    */
/*    Computing and Euclidean Geometry, Ding-Zhu Du and Frank Hwang,         */
/*    editors, World Scientific, Singapore, pp. 23-90, 1992.  [*]            */
/*                                                                           */
/*  The time for incrementally adding PSLG (planar straight line graph)      */
/*    segments to create a constrained Delaunay triangulation is probably    */
/*    O(t^2) per segment in the worst case and O(t) per segment in the       */
/*    common case, where t is the number of triangles that intersect the     */
/*    segment before it is inserted.  This doesn't count point location,     */
/*    which can be much more expensive.  I could improve this to O(d log d)  */
/*    time, but d is usually quite small, so it's not worth the bother.      */
/*    (This note does not apply when the -s switch is used, invoking a       */
/*    different method is used to insert segments.)                          */
/*                                                                           */
/*  The time for deleting a vertex from a Delaunay triangulation is O(d^2)   */
/*    in the worst case and O(d) in the common case, where d is the degree   */
/*    of the vertex being deleted.  I could improve this to O(d log d) time, */
/*    but d is usually quite small, so it's not worth the bother.            */
/*                                                                           */
/*  Ruppert's Delaunay refinement algorithm typically generates triangles    */
/*    at a linear rate (constant time per triangle) after the initial        */
/*    triangulation is formed.  There may be pathological cases where        */
/*    quadratic time is required, but these never arise in practice.         */
/*                                                                           */
/*  The geometric predicates (circumcenter calculations, segment             */
/*    intersection formulae, etc.) appear in my "Lecture Notes on Geometric  */
/*    Robustness" at http://www.cs.berkeley.edu/~jrs/mesh .                  */
/*                                                                           */
/*  If you make any improvements to this code, please please please let me   */
/*    know, so that I may obtain the improvements.  Even if you don't change */
/*    the code, I'd still love to hear what it's being used for.             */
/*                                                                           */
/*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "triangle.h"

#include "predicates.h"

#ifndef NO_ACUTE
#include "acute.h"
#endif

/* Random number seed is not constant, but I've made it global anyway.       */

unsigned long randomseed;                     /* Current random number seed. */

/* Fast lookup arrays to speed some of the mesh manipulation primitives.     */

int plus1mod3[3] = {1, 2, 0};
int minus1mod3[3] = {2, 0, 1};

/********* Triangle utility functions                                *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  interpolate()  Interpolate the attributes of 'newvertex' in the triangle */
/*                 given by 'org', 'dest' and 'apex'.                        */
/*                                                                           */
/*****************************************************************************/

void interpolate(vertex newvertex, vertex org, vertex dest, vertex apex, int nextras)
{
  REAL xdo, ydo, xao, yao;
  REAL denominator;
  REAL dx, dy;
  REAL xi, eta;
  int i;

  xdo = dest[0] - org[0];
  ydo = dest[1] - org[1];
  xao = apex[0] - org[0];
  yao = apex[1] - org[1];

  denominator = 0.5 / (xdo * yao - xao * ydo);

  dx = newvertex[0] - org[0];
  dy = newvertex[1] - org[1];

  // To interpolate vertex attributes for the new vertex, define a
  // coordinate system with a xi-axis directed from the triangle's
  // origin to its destination, and an eta-axis, directed from its
  // origin to its apex.
  xi = (yao * dx - xao * dy) * (2.0 * denominator);
  eta = (xdo * dy - ydo * dx) * (2.0 * denominator);

  for (i = 2; i < 2 + nextras; i++) {
    // Interpolate the vertex attributes.
    newvertex[i] = org[i] + xi * (dest[i] - org[i]) + eta * (apex[i] - org[i]);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  behavior_update()  Update behavior values.                               */
/*                                                                           */
/*****************************************************************************/

void behavior_update(behavior *b)
{
  b->usesegments = b->poly || b->refine || b->quality || b->convex;
  b->goodangle = cos(b->minangle * PI / 180.0);
#ifndef NO_ACUTE
  b->maxgoodangle = cos(b->maxangle * PI / 180.0);
#endif
  if (b->goodangle == 1.0) {
    b->offconstant = 0.0;
  } else {
    b->offconstant = 0.475 * sqrt((1.0 + b->goodangle) / (1.0 - b->goodangle));
  }
  b->goodangle *= b->goodangle;
  /* Be careful not to allocate space for element area constraints that */
  /*   will never be assigned any value (other than the default -1.0).  */
  if (!b->refine && !b->poly) {
    b->vararea = 0;
  }
  /* Be careful not to add an extra attribute to each element unless the */
  /*   input supports it (PSLG in, but not refining a preexisting mesh). */
  if (b->refine || !b->poly) {
    b->regionattrib = 0;
  }
  /* Regular/weighted triangulations are incompatible with PSLGs */
  /*   and meshing.                                              */
  if (b->weighted && (b->poly || b->quality)) {
    b->weighted = 0;
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Triangle utility functions end here                       *********/

/********* Memory allocation and program exit wrappers begin here    *********/
/**                                                                         **/
/**                                                                         **/

void triexit(int status)
{
  exit(status);
}

VOID *trimalloc(int size)
{
  VOID *memptr;

  memptr = (VOID *) malloc((unsigned int) size);
  if (memptr == (VOID *) NULL) {
    printf("Error:  Out of memory.\n");
    triexit(1);
  }
  return(memptr);
}

void trifree(VOID *memptr)
{
  free(memptr);
}

/**                                                                         **/
/**                                                                         **/
/********* Memory allocation and program exit wrappers end here      *********/

/********* User interaction routines begin here                      *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  internalerror()   Ask the user to send me the defective product.  Exit.  */
/*                                                                           */
/*****************************************************************************/

void internalerror()
{
  printf("  Please report this bug to jrs@cs.berkeley.edu\n");
  printf("  Include the message above, your input data set, and the exact\n");
  printf("    command line you used to run Triangle.\n");
  triexit(1);
}

/*****************************************************************************/
/*                                                                           */
/*  parsecommandline()   Read the command line, identify switches, and set   */
/*                       up options and file names.                          */
/*                                                                           */
/*****************************************************************************/

void parsecommandline(char *options, behavior *b)
{
  int i, k;
  char workstring[FILENAMESIZE];

  b->poly = b->refine = b->quality = 0;
  b->vararea = b->fixedarea = b->usertest = 0;
  b->regionattrib = b->convex = b->weighted = b->jettison = 0;
  b->firstnumber = 1;
  b->neighbors = 0;
  b->nobound = 0;
  b->noholes = b->noexact = 0;
  b->incremental = b->sweepline = 0;
  b->dwyer = 1;
  b->splitseg = 0;
  b->nobisect = 0;
  b->conformdel = 0;
  b->steiner = -1;
  b->order = 1;
  b->minangle = 0.0;
#ifndef NO_ACUTE
  b->maxangle = 0.0;
#endif
  b->maxarea = -1.0;

  for (i = 0; options[i] != '\0'; i++) {
    if (options[i] == 'p') {
      b->poly = 1;
    }
#ifndef CDT_ONLY
    if (options[i] == 'r') {
      b->refine = 1;
    }
    if (options[i] == 'q') {
      b->quality = 1;
      if (((options[i + 1] >= '0') && (options[i + 1] <= '9')) ||
          (options[i + 1] == '.')) {
        k = 0;
        while (((options[i + 1] >= '0') && (options[i + 1] <= '9')) ||
               (options[i + 1] == '.')) {
          i++;
          workstring[k] = options[i];
          k++;
        }
        workstring[k] = '\0';
        b->minangle = (REAL) strtod(workstring, (char **) NULL);
      } else {
        b->minangle = 20.0;
      }
    }
#ifndef NO_ACUTE
    if (options[i] == 'U') {
      b->quality = 1;
      if (((options[i + 1] >= '0') && (options[i + 1] <= '9')) ||
          (options[i + 1] == '.')) {
        k = 0;
        while (((options[i + 1] >= '0') && (options[i + 1] <= '9')) ||
            (options[i + 1] == '.')) {
          i++;
          workstring[k] = options[i];
          k++;
        }
        workstring[k] = '\0';
        b->maxangle = (REAL) strtod(workstring, (char **) NULL);
      } else {
        b->maxangle = 140.0;
      }
    }
#endif
    if (options[i] == 'a') {
      b->quality = 1;
      if (((options[i + 1] >= '0') && (options[i + 1] <= '9')) ||
          (options[i + 1] == '.')) {
        b->fixedarea = 1;
        k = 0;
        while (((options[i + 1] >= '0') && (options[i + 1] <= '9')) ||
               (options[i + 1] == '.')) {
          i++;
          workstring[k] = options[i];
          k++;
        }
        workstring[k] = '\0';
        b->maxarea = (REAL) strtod(workstring, (char **) NULL);
      } else {
        b->vararea = 1;
      }
    }
    if (options[i] == 'u') {
      b->quality = 1;
      b->usertest = 1;
    }
#endif /* not CDT_ONLY */
    if (options[i] == 'A') {
      b->regionattrib = 1;
    }
    if (options[i] == 'c') {
      b->convex = 1;
    }
    if (options[i] == 'w') {
      b->weighted = 1;
    }
    if (options[i] == 'W') {
      b->weighted = 2;
    }
    if (options[i] == 'j') {
      b->jettison = 1;
    }
    if (options[i] == 'z') {
      b->firstnumber = 0;
    }
    if (options[i] == 'n') {
      b->neighbors = 1;
    }
    if (options[i] == 'B') {
      b->nobound = 1;
    }
    if (options[i] == 'O') {
      b->noholes = 1;
    }
    if (options[i] == 'X') {
      b->noexact = 1;
    }
    if (options[i] == 'o') {
      if (options[i + 1] == '2') {
        i++;
        b->order = 2;
      }
    }
#ifndef CDT_ONLY
    if (options[i] == 'Y') {
      b->nobisect++;
    }
    if (options[i] == 'S') {
      b->steiner = 0;
      while ((options[i + 1] >= '0') && (options[i + 1] <= '9')) {
        i++;
        b->steiner = b->steiner * 10 + (int) (options[i] - '0');
      }
    }
#endif /* not CDT_ONLY */
#ifndef REDUCED
    if (options[i] == 'i') {
      b->incremental = 1;
    }
    if (options[i] == 'F') {
      b->sweepline = 1;
    }
#endif /* not REDUCED */
    if (options[i] == 'l') {
      b->dwyer = 0;
    }
#ifndef REDUCED
#ifndef CDT_ONLY
    if (options[i] == 's') {
      b->splitseg = 1;
    }
    if ((options[i] == 'D') || (options[i] == 'L')) {
      b->quality = 1;
      b->conformdel = 1;
    }
#endif /* not CDT_ONLY */
#endif /* not REDUCED */
  }
  b->usesegments = b->poly || b->refine || b->quality || b->convex;
  b->goodangle = cos(b->minangle * PI / 180.0);
#ifndef NO_ACUTE
  b->maxgoodangle = cos(b->maxangle * PI / 180.0);
#endif
  if (b->goodangle == 1.0) {
    b->offconstant = 0.0;
  } else {
    b->offconstant = 0.475 * sqrt((1.0 + b->goodangle) / (1.0 - b->goodangle));
  }
  b->goodangle *= b->goodangle;
  /* Be careful not to allocate space for element area constraints that */
  /*   will never be assigned any value (other than the default -1.0).  */
  if (!b->refine && !b->poly) {
    b->vararea = 0;
  }
  /* Be careful not to add an extra attribute to each element unless the */
  /*   input supports it (PSLG in, but not refining a preexisting mesh). */
  if (b->refine || !b->poly) {
    b->regionattrib = 0;
  }
  /* Regular/weighted triangulations are incompatible with PSLGs */
  /*   and meshing.                                              */
  if (b->weighted && (b->poly || b->quality)) {
    b->weighted = 0;
  }
}

/**                                                                         **/
/**                                                                         **/
/********* User interaction routines begin here                      *********/

/********* Debugging routines begin here                             *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  printtriangle()   Print out the details of an oriented triangle.         */
/*                                                                           */
/*  I originally wrote this procedure to simplify debugging; it can be       */
/*  called directly from the debugger, and presents information about an     */
/*  oriented triangle in digestible form.  It's also used when the           */
/*  highest level of verbosity (`-VVV') is specified.                        */
/*                                                                           */
/*****************************************************************************/

void printtriangle(mesh *m, behavior *b, struct otri *t)
{
  struct otri printtri;
  struct osub printsh;
  vertex printvertex;

  printf("triangle x"LX" with orientation %d:\n", (ULONG_PTR) t->tri,
         t->orient);
  decode(t->tri[0], printtri);
  if (printtri.tri == m->dummytri) {
    printf("    [0] = Outer space\n");
  } else {
    printf("    [0] = x"LX"  %d\n", (ULONG_PTR) printtri.tri,
           printtri.orient);
  }
  decode(t->tri[1], printtri);
  if (printtri.tri == m->dummytri) {
    printf("    [1] = Outer space\n");
  } else {
    printf("    [1] = x"LX"  %d\n", (ULONG_PTR) printtri.tri,
           printtri.orient);
  }
  decode(t->tri[2], printtri);
  if (printtri.tri == m->dummytri) {
    printf("    [2] = Outer space\n");
  } else {
    printf("    [2] = x"LX"  %d\n", (ULONG_PTR) printtri.tri,
           printtri.orient);
  }

  org(*t, printvertex);
  if (printvertex == (vertex) NULL)
    printf("    Origin[%d] = NULL\n", (t->orient + 1) % 3 + 3);
  else
    printf("    Origin[%d] = x"LX"  (%.12g, %.12g)\n",
           (t->orient + 1) % 3 + 3, (ULONG_PTR) printvertex,
           printvertex[0], printvertex[1]);
  dest(*t, printvertex);
  if (printvertex == (vertex) NULL)
    printf("    Dest  [%d] = NULL\n", (t->orient + 2) % 3 + 3);
  else
    printf("    Dest  [%d] = x"LX"  (%.12g, %.12g)\n",
           (t->orient + 2) % 3 + 3, (ULONG_PTR) printvertex,
           printvertex[0], printvertex[1]);
  apex(*t, printvertex);
  if (printvertex == (vertex) NULL)
    printf("    Apex  [%d] = NULL\n", t->orient + 3);
  else
    printf("    Apex  [%d] = x"LX"  (%.12g, %.12g)\n",
           t->orient + 3, (ULONG_PTR) printvertex,
           printvertex[0], printvertex[1]);

  if (b->usesegments) {
    sdecode(t->tri[6], printsh);
    if (printsh.ss != m->dummysub) {
      printf("    [6] = x"LX"  %d\n", (ULONG_PTR) printsh.ss,
             printsh.ssorient);
    }
    sdecode(t->tri[7], printsh);
    if (printsh.ss != m->dummysub) {
      printf("    [7] = x"LX"  %d\n", (ULONG_PTR) printsh.ss,
             printsh.ssorient);
    }
    sdecode(t->tri[8], printsh);
    if (printsh.ss != m->dummysub) {
      printf("    [8] = x"LX"  %d\n", (ULONG_PTR) printsh.ss,
             printsh.ssorient);
    }
  }

  if (b->vararea) {
    printf("    Area constraint:  %.4g\n", areabound(*t));
  }
}

/*****************************************************************************/
/*                                                                           */
/*  printsubseg()   Print out the details of an oriented subsegment.         */
/*                                                                           */
/*  I originally wrote this procedure to simplify debugging; it can be       */
/*  called directly from the debugger, and presents information about an     */
/*  oriented subsegment in digestible form.  It's also used when the highest */
/*  level of verbosity (`-VVV') is specified.                                */
/*                                                                           */
/*****************************************************************************/

void printsubseg(mesh *m, behavior *b, struct osub *s)
{
  struct osub printsh;
  struct otri printtri;
  vertex printvertex;

  printf("subsegment x"LX" with orientation %d and mark %d:\n",
         (ULONG_PTR) s->ss, s->ssorient, mark(*s));
  sdecode(s->ss[0], printsh);
  if (printsh.ss == m->dummysub) {
    printf("    [0] = No subsegment\n");
  } else {
    printf("    [0] = x"LX"  %d\n", (ULONG_PTR) printsh.ss,
           printsh.ssorient);
  }
  sdecode(s->ss[1], printsh);
  if (printsh.ss == m->dummysub) {
    printf("    [1] = No subsegment\n");
  } else {
    printf("    [1] = x"LX"  %d\n", (ULONG_PTR) printsh.ss,
           printsh.ssorient);
  }

  sorg(*s, printvertex);
  if (printvertex == (vertex) NULL)
    printf("    Origin[%d] = NULL\n", 2 + s->ssorient);
  else
    printf("    Origin[%d] = x"LX"  (%.12g, %.12g)\n",
           2 + s->ssorient, (ULONG_PTR) printvertex,
           printvertex[0], printvertex[1]);
  sdest(*s, printvertex);
  if (printvertex == (vertex) NULL)
    printf("    Dest  [%d] = NULL\n", 3 - s->ssorient);
  else
    printf("    Dest  [%d] = x"LX"  (%.12g, %.12g)\n",
           3 - s->ssorient, (ULONG_PTR) printvertex,
           printvertex[0], printvertex[1]);

  decode(s->ss[6], printtri);
  if (printtri.tri == m->dummytri) {
    printf("    [6] = Outer space\n");
  } else {
    printf("    [6] = x"LX"  %d\n", (ULONG_PTR) printtri.tri,
           printtri.orient);
  }
  decode(s->ss[7], printtri);
  if (printtri.tri == m->dummytri) {
    printf("    [7] = Outer space\n");
  } else {
    printf("    [7] = x"LX"  %d\n", (ULONG_PTR) printtri.tri,
           printtri.orient);
  }

  segorg(*s, printvertex);
  if (printvertex == (vertex) NULL)
    printf("    Segment origin[%d] = NULL\n", 4 + s->ssorient);
  else
    printf("    Segment origin[%d] = x"LX"  (%.12g, %.12g)\n",
           4 + s->ssorient, (ULONG_PTR) printvertex,
           printvertex[0], printvertex[1]);
  segdest(*s, printvertex);
  if (printvertex == (vertex) NULL)
    printf("    Segment dest  [%d] = NULL\n", 5 - s->ssorient);
  else
    printf("    Segment dest  [%d] = x"LX"  (%.12g, %.12g)\n",
           5 - s->ssorient, (ULONG_PTR) printvertex,
           printvertex[0], printvertex[1]);
}

/**                                                                         **/
/**                                                                         **/
/********* Debugging routines end here                               *********/

/********* Memory management routines begin here                     *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  poolzero()   Set all of a pool's fields to zero.                         */
/*                                                                           */
/*  This procedure should never be called on a pool that has any memory      */
/*  allocated to it, as that memory would leak.                              */
/*                                                                           */
/*****************************************************************************/

void poolzero(struct memorypool *pool)
{
  pool->firstblock = (VOID **) NULL;
  pool->nowblock = (VOID **) NULL;
  pool->nextitem = (VOID *) NULL;
  pool->deaditemstack = (VOID *) NULL;
  pool->pathblock = (VOID **) NULL;
  pool->pathitem = (VOID *) NULL;
  pool->alignbytes = 0;
  pool->itembytes = 0;
  pool->itemsperblock = 0;
  pool->itemsfirstblock = 0;
  pool->items = 0;
  pool->maxitems = 0;
  pool->unallocateditems = 0;
  pool->pathitemsleft = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  poolrestart()   Deallocate all items in a pool.                          */
/*                                                                           */
/*  The pool is returned to its starting state, except that no memory is     */
/*  freed to the operating system.  Rather, the previously allocated blocks  */
/*  are ready to be reused.                                                  */
/*                                                                           */
/*****************************************************************************/

void poolrestart(struct memorypool *pool)
{
  ULONG_PTR alignptr;

  pool->items = 0;
  pool->maxitems = 0;

  /* Set the currently active block. */
  pool->nowblock = pool->firstblock;
  /* Find the first item in the pool.  Increment by the size of (VOID *). */
  alignptr = (ULONG_PTR) (pool->nowblock + 1);
  /* Align the item on an `alignbytes'-byte boundary. */
  pool->nextitem = (VOID *)
    (alignptr + (ULONG_PTR) pool->alignbytes -
     (alignptr % (ULONG_PTR) pool->alignbytes));
  /* There are lots of unallocated items left in this block. */
  pool->unallocateditems = pool->itemsfirstblock;
  /* The stack of deallocated items is empty. */
  pool->deaditemstack = (VOID *) NULL;
}

/*****************************************************************************/
/*                                                                           */
/*  poolinit()   Initialize a pool of memory for allocation of items.        */
/*                                                                           */
/*  This routine initializes the machinery for allocating items.  A `pool'   */
/*  is created whose records have size at least `bytecount'.  Items will be  */
/*  allocated in `itemcount'-item blocks.  Each item is assumed to be a      */
/*  collection of words, and either pointers or floating-point values are    */
/*  assumed to be the "primary" word type.  (The "primary" word type is used */
/*  to determine alignment of items.)  If `alignment' isn't zero, all items  */
/*  will be `alignment'-byte aligned in memory.  `alignment' must be either  */
/*  a multiple or a factor of the primary word size; powers of two are safe. */
/*  `alignment' is normally used to create a few unused bits at the bottom   */
/*  of each item's pointer, in which information may be stored.              */
/*                                                                           */
/*  Don't change this routine unless you understand it.                      */
/*                                                                           */
/*****************************************************************************/

void poolinit(struct memorypool *pool, int bytecount, int itemcount,
              int firstitemcount, int alignment)
{
  /* Find the proper alignment, which must be at least as large as:   */
  /*   - The parameter `alignment'.                                   */
  /*   - sizeof(VOID *), so the stack of dead items can be maintained */
  /*       without unaligned accesses.                                */
  if (alignment > sizeof(VOID *)) {
    pool->alignbytes = alignment;
  } else {
    pool->alignbytes = sizeof(VOID *);
  }
  pool->itembytes = ((bytecount - 1) / pool->alignbytes + 1) *
                    pool->alignbytes;
  pool->itemsperblock = itemcount;
  if (firstitemcount == 0) {
    pool->itemsfirstblock = itemcount;
  } else {
    pool->itemsfirstblock = firstitemcount;
  }

  /* Allocate a block of items.  Space for `itemsfirstblock' items and one  */
  /*   pointer (to point to the next block) are allocated, as well as space */
  /*   to ensure alignment of the items.                                    */
  pool->firstblock = (VOID **)
    trimalloc(pool->itemsfirstblock * pool->itembytes + (int) sizeof(VOID *) +
              pool->alignbytes);
  /* Set the next block pointer to NULL. */
  *(pool->firstblock) = (VOID *) NULL;
  poolrestart(pool);
}

/*****************************************************************************/
/*                                                                           */
/*  pooldeinit()   Free to the operating system all memory taken by a pool.  */
/*                                                                           */
/*****************************************************************************/

void pooldeinit(struct memorypool *pool)
{
  while (pool->firstblock != (VOID **) NULL) {
    pool->nowblock = (VOID **) *(pool->firstblock);
    trifree((VOID *) pool->firstblock);
    pool->firstblock = pool->nowblock;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  poolalloc()   Allocate space for an item.                                */
/*                                                                           */
/*****************************************************************************/

VOID *poolalloc(struct memorypool *pool)
{
  VOID *newitem;
  VOID **newblock;
  ULONG_PTR alignptr;

  /* First check the linked list of dead items.  If the list is not   */
  /*   empty, allocate an item from the list rather than a fresh one. */
  if (pool->deaditemstack != (VOID *) NULL) {
    newitem = pool->deaditemstack;               /* Take first item in list. */
    pool->deaditemstack = * (VOID **) pool->deaditemstack;
  } else {
    /* Check if there are any free items left in the current block. */
    if (pool->unallocateditems == 0) {
      /* Check if another block must be allocated. */
      if (*(pool->nowblock) == (VOID *) NULL) {
        /* Allocate a new block of items, pointed to by the previous block. */
        newblock = (VOID **) trimalloc(pool->itemsperblock * pool->itembytes +
                                       (int) sizeof(VOID *) +
                                       pool->alignbytes);
        *(pool->nowblock) = (VOID *) newblock;
        /* The next block pointer is NULL. */
        *newblock = (VOID *) NULL;
      }

      /* Move to the new block. */
      pool->nowblock = (VOID **) *(pool->nowblock);
      /* Find the first item in the block.    */
      /*   Increment by the size of (VOID *). */
      alignptr = (ULONG_PTR) (pool->nowblock + 1);
      /* Align the item on an `alignbytes'-byte boundary. */
      pool->nextitem = (VOID *)
        (alignptr + (ULONG_PTR) pool->alignbytes -
         (alignptr % (ULONG_PTR) pool->alignbytes));
      /* There are lots of unallocated items left in this block. */
      pool->unallocateditems = pool->itemsperblock;
    }

    /* Allocate a new item. */
    newitem = pool->nextitem;
    /* Advance `nextitem' pointer to next free item in block. */
    pool->nextitem = (VOID *) ((char *) pool->nextitem + pool->itembytes);
    pool->unallocateditems--;
    pool->maxitems++;
  }
  pool->items++;
  return newitem;
}

/*****************************************************************************/
/*                                                                           */
/*  pooldealloc()   Deallocate space for an item.                            */
/*                                                                           */
/*  The deallocated space is stored in a queue for later reuse.              */
/*                                                                           */
/*****************************************************************************/

void pooldealloc(struct memorypool *pool, VOID *dyingitem)
{
  /* Push freshly killed item onto stack. */
  *((VOID **) dyingitem) = pool->deaditemstack;
  pool->deaditemstack = dyingitem;
  pool->items--;
}

/*****************************************************************************/
/*                                                                           */
/*  traversalinit()   Prepare to traverse the entire list of items.          */
/*                                                                           */
/*  This routine is used in conjunction with traverse().                     */
/*                                                                           */
/*****************************************************************************/

void traversalinit(struct memorypool *pool)
{
  ULONG_PTR alignptr;

  /* Begin the traversal in the first block. */
  pool->pathblock = pool->firstblock;
  /* Find the first item in the block.  Increment by the size of (VOID *). */
  alignptr = (ULONG_PTR) (pool->pathblock + 1);
  /* Align with item on an `alignbytes'-byte boundary. */
  pool->pathitem = (VOID *)
    (alignptr + (ULONG_PTR) pool->alignbytes -
     (alignptr % (ULONG_PTR) pool->alignbytes));
  /* Set the number of items left in the current block. */
  pool->pathitemsleft = pool->itemsfirstblock;
}

/*****************************************************************************/
/*                                                                           */
/*  traverse()   Find the next item in the list.                             */
/*                                                                           */
/*  This routine is used in conjunction with traversalinit().  Be forewarned */
/*  that this routine successively returns all items in the list, including  */
/*  deallocated ones on the deaditemqueue.  It's up to you to figure out     */
/*  which ones are actually dead.  Why?  I don't want to allocate extra      */
/*  space just to demarcate dead items.  It can usually be done more         */
/*  space-efficiently by a routine that knows something about the structure  */
/*  of the item.                                                             */
/*                                                                           */
/*****************************************************************************/

VOID *traverse(struct memorypool *pool)
{
  VOID *newitem;
  ULONG_PTR alignptr;

  /* Stop upon exhausting the list of items. */
  if (pool->pathitem == pool->nextitem) {
    return (VOID *) NULL;
  }

  /* Check whether any untraversed items remain in the current block. */
  if (pool->pathitemsleft == 0) {
    /* Find the next block. */
    pool->pathblock = (VOID **) *(pool->pathblock);
    /* Find the first item in the block.  Increment by the size of (VOID *). */
    alignptr = (ULONG_PTR) (pool->pathblock + 1);
    /* Align with item on an `alignbytes'-byte boundary. */
    pool->pathitem = (VOID *)
      (alignptr + (ULONG_PTR) pool->alignbytes -
       (alignptr % (ULONG_PTR) pool->alignbytes));
    /* Set the number of items left in the current block. */
    pool->pathitemsleft = pool->itemsperblock;
  }

  newitem = pool->pathitem;
  /* Find the next item in the block. */
  pool->pathitem = (VOID *) ((char *) pool->pathitem + pool->itembytes);
  pool->pathitemsleft--;
  return newitem;
}

/*****************************************************************************/
/*                                                                           */
/*  dummyinit()   Initialize the triangle that fills "outer space" and the   */
/*                omnipresent subsegment.                                    */
/*                                                                           */
/*  The triangle that fills "outer space," called `dummytri', is pointed to  */
/*  by every triangle and subsegment on a boundary (be it outer or inner) of */
/*  the triangulation.  Also, `dummytri' points to one of the triangles on   */
/*  the convex hull (until the holes and concavities are carved), making it  */
/*  possible to find a starting triangle for point location.                 */
/*                                                                           */
/*  The omnipresent subsegment, `dummysub', is pointed to by every triangle  */
/*  or subsegment that doesn't have a full complement of real subsegments    */
/*  to point to.                                                             */
/*                                                                           */
/*  `dummytri' and `dummysub' are generally required to fulfill only a few   */
/*  invariants:  their vertices must remain NULL and `dummytri' must always  */
/*  be bonded (at offset zero) to some triangle on the convex hull of the    */
/*  mesh, via a boundary edge.  Otherwise, the connections of `dummytri' and */
/*  `dummysub' may change willy-nilly.  This makes it possible to avoid      */
/*  writing a good deal of special-case code (in the edge flip, for example) */
/*  for dealing with the boundary of the mesh, places where no subsegment is */
/*  present, and so forth.  Other entities are frequently bonded to          */
/*  `dummytri' and `dummysub' as if they were real mesh entities, with no    */
/*  harm done.                                                               */
/*                                                                           */
/*****************************************************************************/

void dummyinit(mesh *m, behavior *b, int trianglebytes,
               int subsegbytes)
{
  ULONG_PTR alignptr;

  /* Set up `dummytri', the `triangle' that occupies "outer space." */
  m->dummytribase = (triangle *) trimalloc(trianglebytes +
                                           m->triangles.alignbytes);
  /* Align `dummytri' on a `triangles.alignbytes'-byte boundary. */
  alignptr = (ULONG_PTR) m->dummytribase;
  m->dummytri = (triangle *)
    (alignptr + (ULONG_PTR) m->triangles.alignbytes -
     (alignptr % (ULONG_PTR) m->triangles.alignbytes));
  /* Initialize the three adjoining triangles to be "outer space."  These  */
  /*   will eventually be changed by various bonding operations, but their */
  /*   values don't really matter, as long as they can legally be          */
  /*   dereferenced.                                                       */
  m->dummytri[0] = (triangle) m->dummytri;
  m->dummytri[1] = (triangle) m->dummytri;
  m->dummytri[2] = (triangle) m->dummytri;
  /* Three NULL vertices. */
  m->dummytri[3] = (triangle) NULL;
  m->dummytri[4] = (triangle) NULL;
  m->dummytri[5] = (triangle) NULL;

  if (b->usesegments) {
    /* Set up `dummysub', the omnipresent subsegment pointed to by any */
    /*   triangle side or subsegment end that isn't attached to a real */
    /*   subsegment.                                                   */
    m->dummysubbase = (subseg *) trimalloc(subsegbytes +
                                           m->subsegs.alignbytes);
    /* Align `dummysub' on a `subsegs.alignbytes'-byte boundary. */
    alignptr = (ULONG_PTR) m->dummysubbase;
    m->dummysub = (subseg *)
      (alignptr + (ULONG_PTR) m->subsegs.alignbytes -
       (alignptr % (ULONG_PTR) m->subsegs.alignbytes));
    /* Initialize the two adjoining subsegments to be the omnipresent      */
    /*   subsegment.  These will eventually be changed by various bonding  */
    /*   operations, but their values don't really matter, as long as they */
    /*   can legally be dereferenced.                                      */
    m->dummysub[0] = (subseg) m->dummysub;
    m->dummysub[1] = (subseg) m->dummysub;
    /* Four NULL vertices. */
    m->dummysub[2] = (subseg) NULL;
    m->dummysub[3] = (subseg) NULL;
    m->dummysub[4] = (subseg) NULL;
    m->dummysub[5] = (subseg) NULL;
    /* Initialize the two adjoining triangles to be "outer space." */
    m->dummysub[6] = (subseg) m->dummytri;
    m->dummysub[7] = (subseg) m->dummytri;
    /* Set the boundary marker to zero. */
    * (int *) (m->dummysub + 8) = 0;

    /* Initialize the three adjoining subsegments of `dummytri' to be */
    /*   the omnipresent subsegment.                                  */
    m->dummytri[6] = (triangle) m->dummysub;
    m->dummytri[7] = (triangle) m->dummysub;
    m->dummytri[8] = (triangle) m->dummysub;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  initializevertexpool()   Calculate the size of the vertex data structure */
/*                           and initialize its memory pool.                 */
/*                                                                           */
/*  This routine also computes the `vertexmarkindex' and `vertex2triindex'   */
/*  indices used to find values within each vertex.                          */
/*                                                                           */
/*****************************************************************************/

void initializevertexpool(mesh *m, behavior *b)
{
  int vertexsize;

  /* The index within each vertex at which the boundary marker is found,    */
  /*   followed by the vertex type.  Ensure the vertex marker is aligned to */
  /*   a sizeof(int)-byte address.                                          */
  m->vertexmarkindex = ((m->mesh_dim + m->nextras) * sizeof(REAL) +
                        sizeof(int) - 1) /
                       sizeof(int);
  vertexsize = (m->vertexmarkindex + 2) * sizeof(int);
  if (b->poly) {
    /* The index within each vertex at which a triangle pointer is found.  */
    /*   Ensure the pointer is aligned to a sizeof(triangle)-byte address. */
    m->vertex2triindex = (vertexsize + sizeof(triangle) - 1) /
                         sizeof(triangle);
    vertexsize = (m->vertex2triindex + 1) * sizeof(triangle);
  }

  /* Initialize the pool of vertices. */
  poolinit(&m->vertices, vertexsize, VERTEXPERBLOCK,
           m->invertices > VERTEXPERBLOCK ? m->invertices : VERTEXPERBLOCK,
           sizeof(REAL));
}

/*****************************************************************************/
/*                                                                           */
/*  initializetrisubpools()   Calculate the sizes of the triangle and        */
/*                            subsegment data structures and initialize      */
/*                            their memory pools.                            */
/*                                                                           */
/*  This routine also computes the `highorderindex', `elemattribindex', and  */
/*  `areaboundindex' indices used to find values within each triangle.       */
/*                                                                           */
/*****************************************************************************/

void initializetrisubpools(mesh *m, behavior *b)
{
  int trisize;

  /* The index within each triangle at which the extra nodes (above three)  */
  /*   associated with high order elements are found.  There are three      */
  /*   pointers to other triangles, three pointers to corners, and possibly */
  /*   three pointers to subsegments before the extra nodes.                */
  m->highorderindex = 6 + (b->usesegments * 3);
  /* The number of bytes occupied by a triangle. */
  trisize = ((b->order + 1) * (b->order + 2) / 2 + (m->highorderindex - 3)) *
            sizeof(triangle);
  /* The index within each triangle at which its attributes are found, */
  /*   where the index is measured in REALs.                           */
  m->elemattribindex = (trisize + sizeof(REAL) - 1) / sizeof(REAL);
  /* The index within each triangle at which the maximum area constraint  */
  /*   is found, where the index is measured in REALs.  Note that if the  */
  /*   `regionattrib' flag is set, an additional attribute will be added. */
  m->areaboundindex = m->elemattribindex + m->eextras + b->regionattrib;
  /* If triangle attributes or an area bound are needed, increase the number */
  /*   of bytes occupied by a triangle.                                      */
  if (b->vararea) {
    trisize = (m->areaboundindex + 1) * sizeof(REAL);
  } else if (m->eextras + b->regionattrib > 0) {
    trisize = m->areaboundindex * sizeof(REAL);
  }
  /* If a Voronoi diagram or triangle neighbor graph is requested, make    */
  /*   sure there's room to store an integer index in each triangle.  This */
  /*   integer index can occupy the same space as the subsegment pointers  */
  /*   or attributes or area constraint or extra nodes.                    */
  if (b->neighbors &&
      (trisize < 6 * sizeof(triangle) + sizeof(int))) {
    trisize = 6 * sizeof(triangle) + sizeof(int);
  }

  /* Having determined the memory size of a triangle, initialize the pool. */
  poolinit(&m->triangles, trisize, TRIPERBLOCK,
           (2 * m->invertices - 2) > TRIPERBLOCK ? (2 * m->invertices - 2) :
           TRIPERBLOCK, 4);

  if (b->usesegments) {
    /* Initialize the pool of subsegments.  Take into account all eight */
    /*   pointers and one boundary marker.                              */
    poolinit(&m->subsegs, 8 * sizeof(triangle) + sizeof(int),
             SUBSEGPERBLOCK, SUBSEGPERBLOCK, 4);

    /* Initialize the "outer space" triangle and omnipresent subsegment. */
    dummyinit(m, b, m->triangles.itembytes, m->subsegs.itembytes);
  } else {
    /* Initialize the "outer space" triangle. */
    dummyinit(m, b, m->triangles.itembytes, 0);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  triangledealloc()   Deallocate space for a triangle, marking it dead.    */
/*                                                                           */
/*****************************************************************************/

void triangledealloc(mesh *m, triangle *dyingtriangle)
{
  /* Mark the triangle as dead.  This makes it possible to detect dead */
  /*   triangles when traversing the list of all triangles.            */
  killtri(dyingtriangle);
  pooldealloc(&m->triangles, (VOID *) dyingtriangle);
}

/*****************************************************************************/
/*                                                                           */
/*  triangletraverse()   Traverse the triangles, skipping dead ones.         */
/*                                                                           */
/*****************************************************************************/

triangle *triangletraverse(mesh *m)
{
  triangle *newtriangle;

  do {
    newtriangle = (triangle *) traverse(&m->triangles);
    if (newtriangle == (triangle *) NULL) {
      return (triangle *) NULL;
    }
  } while (deadtri(newtriangle));                         /* Skip dead ones. */
  return newtriangle;
}

/*****************************************************************************/
/*                                                                           */
/*  subsegdealloc()   Deallocate space for a subsegment, marking it dead.    */
/*                                                                           */
/*****************************************************************************/

void subsegdealloc(mesh *m, subseg *dyingsubseg)
{
  /* Mark the subsegment as dead.  This makes it possible to detect dead */
  /*   subsegments when traversing the list of all subsegments.          */
  killsubseg(dyingsubseg);
  pooldealloc(&m->subsegs, (VOID *) dyingsubseg);
}

/*****************************************************************************/
/*                                                                           */
/*  subsegtraverse()   Traverse the subsegments, skipping dead ones.         */
/*                                                                           */
/*****************************************************************************/

subseg *subsegtraverse(mesh *m)
{
  subseg *newsubseg;

  do {
    newsubseg = (subseg *) traverse(&m->subsegs);
    if (newsubseg == (subseg *) NULL) {
      return (subseg *) NULL;
    }
  } while (deadsubseg(newsubseg));                        /* Skip dead ones. */
  return newsubseg;
}

/*****************************************************************************/
/*                                                                           */
/*  vertexdealloc()   Deallocate space for a vertex, marking it dead.        */
/*                                                                           */
/*****************************************************************************/

void vertexdealloc(mesh *m, vertex dyingvertex)
{
  /* Mark the vertex as dead.  This makes it possible to detect dead */
  /*   vertices when traversing the list of all vertices.            */
  setvertextype(dyingvertex, DEADVERTEX);
  pooldealloc(&m->vertices, (VOID *) dyingvertex);
}

/*****************************************************************************/
/*                                                                           */
/*  vertextraverse()   Traverse the vertices, skipping dead ones.            */
/*                                                                           */
/*****************************************************************************/

vertex vertextraverse(mesh *m)
{
  vertex newvertex;

  do {
    newvertex = (vertex) traverse(&m->vertices);
    if (newvertex == (vertex) NULL) {
      return (vertex) NULL;
    }
  } while (vertextype(newvertex) == DEADVERTEX);          /* Skip dead ones. */
  return newvertex;
}

/*****************************************************************************/
/*                                                                           */
/*  badsubsegdealloc()   Deallocate space for a bad subsegment, marking it   */
/*                       dead.                                               */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void badsubsegdealloc(mesh *m, struct badsubseg *dyingseg)
{
  /* Set subsegment's origin to NULL.  This makes it possible to detect dead */
  /*   badsubsegs when traversing the list of all badsubsegs             .   */
  dyingseg->subsegorg = (vertex) NULL;
  pooldealloc(&m->badsubsegs, (VOID *) dyingseg);
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  badsubsegtraverse()   Traverse the bad subsegments, skipping dead ones.  */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

struct badsubseg *badsubsegtraverse(mesh *m)
{
  struct badsubseg *newseg;

  do {
    newseg = (struct badsubseg *) traverse(&m->badsubsegs);
    if (newseg == (struct badsubseg *) NULL) {
      return (struct badsubseg *) NULL;
    }
  } while (newseg->subsegorg == (vertex) NULL);           /* Skip dead ones. */
  return newseg;
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  getvertex()   Get a specific vertex, by number, from the list.           */
/*                                                                           */
/*  The first vertex is number 'firstnumber'.                                */
/*                                                                           */
/*  Note that this takes O(n) time (with a small constant, if VERTEXPERBLOCK */
/*  is large).  I don't care to take the trouble to make it work in constant */
/*  time.                                                                    */
/*                                                                           */
/*****************************************************************************/

vertex getvertex(mesh *m, behavior *b, int number)
{
  VOID **getblock;
  char *foundvertex;
  ULONG_PTR alignptr;
  int current;

  getblock = m->vertices.firstblock;
  current = b->firstnumber;

  /* Find the right block. */
  if (current + m->vertices.itemsfirstblock <= number) {
    getblock = (VOID **) *getblock;
    current += m->vertices.itemsfirstblock;
    while (current + m->vertices.itemsperblock <= number) {
      getblock = (VOID **) *getblock;
      current += m->vertices.itemsperblock;
    }
  }

  /* Now find the right vertex. */
  alignptr = (ULONG_PTR) (getblock + 1);
  foundvertex = (char *) (alignptr + (ULONG_PTR) m->vertices.alignbytes -
                          (alignptr % (ULONG_PTR) m->vertices.alignbytes));
  return (vertex) (foundvertex + m->vertices.itembytes * (number - current));
}

/*****************************************************************************/
/*                                                                           */
/*  triangledeinit()   Free all remaining allocated memory.                  */
/*                                                                           */
/*****************************************************************************/

void triangledeinit(mesh *m, behavior *b)
{
  pooldeinit(&m->triangles);
  trifree((VOID *) m->dummytribase);
  if (b->usesegments) {
    pooldeinit(&m->subsegs);
    trifree((VOID *) m->dummysubbase);
  }
  pooldeinit(&m->vertices);
#ifndef CDT_ONLY
  if (b->quality) {
    pooldeinit(&m->badsubsegs);
    if ((b->minangle > 0.0) || b->vararea || b->fixedarea || b->usertest) {
      pooldeinit(&m->badtriangles);
      pooldeinit(&m->flipstackers);
    }
  }
#endif /* not CDT_ONLY */
#ifndef NO_ACUTE
  acutepool_deinit(m->acute_mem);
  trifree(m->acute_mem);
#endif
}

/**                                                                         **/
/**                                                                         **/
/********* Memory management routines end here                       *********/

/********* Constructors begin here                                   *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  maketriangle()   Create a new triangle with orientation zero.            */
/*                                                                           */
/*****************************************************************************/

void maketriangle(mesh *m, behavior *b, struct otri *newotri)
{
  int i;

  newotri->tri = (triangle *) poolalloc(&m->triangles);
  /* Initialize the three adjoining triangles to be "outer space". */
  newotri->tri[0] = (triangle) m->dummytri;
  newotri->tri[1] = (triangle) m->dummytri;
  newotri->tri[2] = (triangle) m->dummytri;
  /* Three NULL vertices. */
  newotri->tri[3] = (triangle) NULL;
  newotri->tri[4] = (triangle) NULL;
  newotri->tri[5] = (triangle) NULL;
  if (b->usesegments) {
    /* Initialize the three adjoining subsegments to be the omnipresent */
    /*   subsegment.                                                    */
    newotri->tri[6] = (triangle) m->dummysub;
    newotri->tri[7] = (triangle) m->dummysub;
    newotri->tri[8] = (triangle) m->dummysub;
  }
  for (i = 0; i < m->eextras; i++) {
    setelemattribute(*newotri, i, 0.0);
  }
  if (b->vararea) {
    setareabound(*newotri, -1.0);
  }

  newotri->orient = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  makesubseg()   Create a new subsegment with orientation zero.            */
/*                                                                           */
/*****************************************************************************/

void makesubseg(mesh *m, struct osub *newsubseg)
{
  newsubseg->ss = (subseg *) poolalloc(&m->subsegs);
  /* Initialize the two adjoining subsegments to be the omnipresent */
  /*   subsegment.                                                  */
  newsubseg->ss[0] = (subseg) m->dummysub;
  newsubseg->ss[1] = (subseg) m->dummysub;
  /* Four NULL vertices. */
  newsubseg->ss[2] = (subseg) NULL;
  newsubseg->ss[3] = (subseg) NULL;
  newsubseg->ss[4] = (subseg) NULL;
  newsubseg->ss[5] = (subseg) NULL;
  /* Initialize the two adjoining triangles to be "outer space." */
  newsubseg->ss[6] = (subseg) m->dummytri;
  newsubseg->ss[7] = (subseg) m->dummytri;
  /* Set the boundary marker to zero. */
  setmark(*newsubseg, 0);

  newsubseg->ssorient = 0;
}

/**                                                                         **/
/**                                                                         **/
/********* Constructors end here                                     *********/


/*****************************************************************************/
/*                                                                           */
/*  triangleinit()   Initialize some variables.                              */
/*                                                                           */
/*****************************************************************************/

void triangleinit(mesh *m)
{
  poolzero(&m->vertices);
  poolzero(&m->triangles);
  poolzero(&m->subsegs);
  poolzero(&m->viri);
  poolzero(&m->badsubsegs);
  poolzero(&m->badtriangles);
  poolzero(&m->flipstackers);
  poolzero(&m->splaynodes);

  m->recenttri.tri = (triangle *) NULL; /* No triangle has been visited yet. */
  m->undeads = 0;                       /* No eliminated input vertices yet. */
  m->samples = 1;         /* Point location should take at least one sample. */
  m->checksegments = 0;   /* There are no segments in the triangulation yet. */
  m->checkquality = 0;     /* The quality triangulation stage has not begun. */
  m->incirclecount = m->counterclockcount = m->orient3dcount = 0;
  m->hyperbolacount = m->circletopcount = m->circumcentercount = 0;
  randomseed = 1;

  exactinit();                     /* Initialize exact arithmetic constants. */

#ifndef NO_ACUTE
  m->acute_mem = (acutepool *) NULL;
#endif
}

/*****************************************************************************/
/*                                                                           */
/*  randomnation()   Generate a random number between 0 and `choices' - 1.   */
/*                                                                           */
/*  This is a simple linear congruential random number generator.  Hence, it */
/*  is a bad random number generator, but good enough for most randomized    */
/*  geometric algorithms.                                                    */
/*                                                                           */
/*****************************************************************************/

unsigned long randomnation(unsigned int choices)
{
  randomseed = (randomseed * 1366l + 150889l) % 714025l;
  return randomseed / (714025l / choices + 1);
}

/********* Mesh quality testing routines begin here                  *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  checkmesh()   Test the mesh for topological consistency.                 */
/*                                                                           */
/*****************************************************************************/

int checkmesh(mesh *m, behavior *b)
{
  struct otri triangleloop;
  struct otri oppotri, oppooppotri;
  vertex triorg, tridest, triapex;
  vertex oppoorg, oppodest;
  int horrors;
  int saveexact;
  triangle ptr;                         /* Temporary variable used by sym(). */

  /* Temporarily turn on exact arithmetic if it's off. */
  saveexact = b->noexact;
  b->noexact = 0;
  horrors = 0;
  /* Run through the list of triangles, checking each one. */
  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  while (triangleloop.tri != (triangle *) NULL) {
    /* Check all three edges of the triangle. */
    for (triangleloop.orient = 0; triangleloop.orient < 3;
         triangleloop.orient++) {
      org(triangleloop, triorg);
      dest(triangleloop, tridest);
      if (triangleloop.orient == 0) {       /* Only test for inversion once. */
        /* Test if the triangle is flat or inverted. */
        apex(triangleloop, triapex);
        if (counterclockwise(m, b, triorg, tridest, triapex) <= 0.0) {
#ifdef _DEBUG
          printf("  !! !! Inverted ");
          printtriangle(m, b, &triangleloop);
#endif
          horrors++;
        }
      }
      /* Find the neighboring triangle on this edge. */
      sym(triangleloop, oppotri);
      if (oppotri.tri != m->dummytri) {
        /* Check that the triangle's neighbor knows it's a neighbor. */
        sym(oppotri, oppooppotri);
        if ((triangleloop.tri != oppooppotri.tri)
            || (triangleloop.orient != oppooppotri.orient)) {
#ifdef _DEBUG
          printf("  !! !! Asymmetric triangle-triangle bond:\n");
          if (triangleloop.tri == oppooppotri.tri) {
            printf("   (Right triangle, wrong orientation)\n");
          }
          printf("    First ");
          printtriangle(m, b, &triangleloop);
          printf("    Second (nonreciprocating) ");
          printtriangle(m, b, &oppotri);
#endif
          horrors++;
        }
        /* Check that both triangles agree on the identities */
        /*   of their shared vertices.                       */
        org(oppotri, oppoorg);
        dest(oppotri, oppodest);
        if ((triorg != oppodest) || (tridest != oppoorg)) {
#ifdef _DEBUG
          printf("  !! !! Mismatched edge coordinates between two triangles:\n"
                 );
          printf("    First mismatched ");
          printtriangle(m, b, &triangleloop);
          printf("    Second mismatched ");
          printtriangle(m, b, &oppotri);
#endif
          horrors++;
        }
      }
    }
    triangleloop.tri = triangletraverse(m);
  }

  /* Restore the status of exact arithmetic. */
  b->noexact = saveexact;

  return horrors;
}

/*****************************************************************************/
/*                                                                           */
/*  checkdelaunay()   Ensure that the mesh is (constrained) Delaunay.        */
/*                                                                           */
/*****************************************************************************/

int checkdelaunay(mesh *m, behavior *b)
{
  struct otri triangleloop;
  struct otri oppotri;
  struct osub opposubseg;
  vertex triorg, tridest, triapex;
  vertex oppoapex;
  int shouldbedelaunay;
  int horrors;
  int saveexact;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Temporarily turn on exact arithmetic if it's off. */
  saveexact = b->noexact;
  b->noexact = 0;
  horrors = 0;
  /* Run through the list of triangles, checking each one. */
  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  while (triangleloop.tri != (triangle *) NULL) {
    /* Check all three edges of the triangle. */
    for (triangleloop.orient = 0; triangleloop.orient < 3;
         triangleloop.orient++) {
      org(triangleloop, triorg);
      dest(triangleloop, tridest);
      apex(triangleloop, triapex);
      sym(triangleloop, oppotri);
      apex(oppotri, oppoapex);
      /* Only test that the edge is locally Delaunay if there is an   */
      /*   adjoining triangle whose pointer is larger (to ensure that */
      /*   each pair isn't tested twice).                             */
      shouldbedelaunay = (oppotri.tri != m->dummytri) &&
            !deadtri(oppotri.tri) && (triangleloop.tri < oppotri.tri) &&
            (triorg != m->infvertex1) && (triorg != m->infvertex2) &&
            (triorg != m->infvertex3) &&
            (tridest != m->infvertex1) && (tridest != m->infvertex2) &&
            (tridest != m->infvertex3) &&
            (triapex != m->infvertex1) && (triapex != m->infvertex2) &&
            (triapex != m->infvertex3) &&
            (oppoapex != m->infvertex1) && (oppoapex != m->infvertex2) &&
            (oppoapex != m->infvertex3);
      if (m->checksegments && shouldbedelaunay) {
        /* If a subsegment separates the triangles, then the edge is */
        /*   constrained, so no local Delaunay test should be done.  */
        tspivot(triangleloop, opposubseg);
        if (opposubseg.ss != m->dummysub){
          shouldbedelaunay = 0;
        }
      }
      if (shouldbedelaunay) {
        if (nonregular(m, b, triorg, tridest, triapex, oppoapex) > 0.0) {
#ifdef _DEBUG
          if (!b->weighted) {
            printf("  !! !! Non-Delaunay pair of triangles:\n");
            printf("    First non-Delaunay ");
            printtriangle(m, b, &triangleloop);
            printf("    Second non-Delaunay ");
          } else {
            printf("  !! !! Non-regular pair of triangles:\n");
            printf("    First non-regular ");
            printtriangle(m, b, &triangleloop);
            printf("    Second non-regular ");
          }
          printtriangle(m, b, &oppotri);
#endif
          horrors++;
        }
      }
    }
    triangleloop.tri = triangletraverse(m);
  }

  /* Restore the status of exact arithmetic. */
  b->noexact = saveexact;

  return horrors;
}

/*****************************************************************************/
/*                                                                           */
/*  enqueuebadtriang()   Add a bad triangle data structure to the end of a   */
/*                       queue.                                              */
/*                                                                           */
/*  The queue is actually a set of 4096 queues.  I use multiple queues to    */
/*  give priority to smaller angles.  I originally implemented a heap, but   */
/*  the queues are faster by a larger margin than I'd suspected.             */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void enqueuebadtriang(mesh *m, behavior *b,
                      struct badtriang *badtri)
{
  REAL length, multiplier;
  int exponent, expincrement;
  int queuenumber;
  int posexponent;
  int i;

  /* Determine the appropriate queue to put the bad triangle into.    */
  /*   Recall that the key is the square of its shortest edge length. */
  if (badtri->key >= 1.0) {
    length = badtri->key;
    posexponent = 1;
  } else {
    /* `badtri->key' is 2.0 to a negative exponent, so we'll record that */
    /*   fact and use the reciprocal of `badtri->key', which is > 1.0.   */
    length = 1.0 / badtri->key;
    posexponent = 0;
  }
  /* `length' is approximately 2.0 to what exponent?  The following code */
  /*   determines the answer in time logarithmic in the exponent.        */
  exponent = 0;
  while (length > 2.0) {
    /* Find an approximation by repeated squaring of two. */
    expincrement = 1;
    multiplier = 0.5;
    while (length * multiplier * multiplier > 1.0) {
      expincrement *= 2;
      multiplier *= multiplier;
    }
    /* Reduce the value of `length', then iterate if necessary. */
    exponent += expincrement;
    length *= multiplier;
  }
  /* `length' is approximately squareroot(2.0) to what exponent? */
  exponent = 2 * exponent + (length > SQUAREROOTTWO);
  /* `exponent' is now in the range 0...2047 for IEEE double precision.   */
  /*   Choose a queue in the range 0...4095.  The shortest edges have the */
  /*   highest priority (queue 4095).                                     */
  if (posexponent) {
    queuenumber = 2047 - exponent;
  } else {
    queuenumber = 2048 + exponent;
  }

  /* Are we inserting into an empty queue? */
  if (m->queuefront[queuenumber] == (struct badtriang *) NULL) {
    /* Yes, we are inserting into an empty queue.     */
    /*   Will this become the highest-priority queue? */
    if (queuenumber > m->firstnonemptyq) {
      /* Yes, this is the highest-priority queue. */
      m->nextnonemptyq[queuenumber] = m->firstnonemptyq;
      m->firstnonemptyq = queuenumber;
    } else {
      /* No, this is not the highest-priority queue. */
      /*   Find the queue with next higher priority. */
      i = queuenumber + 1;
      while (m->queuefront[i] == (struct badtriang *) NULL) {
        i++;
      }
      /* Mark the newly nonempty queue as following a higher-priority queue. */
      m->nextnonemptyq[queuenumber] = m->nextnonemptyq[i];
      m->nextnonemptyq[i] = queuenumber;
    }
    /* Put the bad triangle at the beginning of the (empty) queue. */
    m->queuefront[queuenumber] = badtri;
  } else {
    /* Add the bad triangle to the end of an already nonempty queue. */
    m->queuetail[queuenumber]->nexttriang = badtri;
  }
  /* Maintain a pointer to the last triangle of the queue. */
  m->queuetail[queuenumber] = badtri;
  /* Newly enqueued bad triangle has no successor in the queue. */
  badtri->nexttriang = (struct badtriang *) NULL;
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  enqueuebadtri()   Add a bad triangle to the end of a queue.              */
/*                                                                           */
/*  Allocates a badtriang data structure for the triangle, then passes it to */
/*  enqueuebadtriang().                                                      */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void enqueuebadtri(mesh *m, behavior *b, struct otri *enqtri,
                   REAL minedge, vertex enqapex, vertex enqorg, vertex enqdest)
{
  struct badtriang *newbad;

  /* Allocate space for the bad triangle. */
  newbad = (struct badtriang *) poolalloc(&m->badtriangles);
  newbad->poortri = encode(*enqtri);
  newbad->key = minedge;
  newbad->triangapex = enqapex;
  newbad->triangorg = enqorg;
  newbad->triangdest = enqdest;
  enqueuebadtriang(m, b, newbad);
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  dequeuebadtriang()   Remove a triangle from the front of the queue.      */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

struct badtriang *dequeuebadtriang(mesh *m)
{
  struct badtriang *result;

  /* If no queues are nonempty, return NULL. */
  if (m->firstnonemptyq < 0) {
    return (struct badtriang *) NULL;
  }
  /* Find the first triangle of the highest-priority queue. */
  result = m->queuefront[m->firstnonemptyq];
  /* Remove the triangle from the queue. */
  m->queuefront[m->firstnonemptyq] = result->nexttriang;
  /* If this queue is now empty, note the new highest-priority */
  /*   nonempty queue.                                         */
  if (result == m->queuetail[m->firstnonemptyq]) {
    m->firstnonemptyq = m->nextnonemptyq[m->firstnonemptyq];
  }
  return result;
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  checkseg4encroach()   Check a subsegment to see if it is encroached; add */
/*                        it to the list if it is.                           */
/*                                                                           */
/*  A subsegment is encroached if there is a vertex in its diametral lens.   */
/*  For Ruppert's algorithm (-D switch), the "diametral lens" is the         */
/*  diametral circle.  For Chew's algorithm (default), the diametral lens is */
/*  just big enough to enclose two isosceles triangles whose bases are the   */
/*  subsegment.  Each of the two isosceles triangles has two angles equal    */
/*  to `b->minangle'.                                                        */
/*                                                                           */
/*  Chew's algorithm does not require diametral lenses at all--but they save */
/*  time.  Any vertex inside a subsegment's diametral lens implies that the  */
/*  triangle adjoining the subsegment will be too skinny, so it's only a     */
/*  matter of time before the encroaching vertex is deleted by Chew's        */
/*  algorithm.  It's faster to simply not insert the doomed vertex in the    */
/*  first place, which is why I use diametral lenses with Chew's algorithm.  */
/*                                                                           */
/*  Returns a nonzero value if the subsegment is encroached.                 */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

int checkseg4encroach(mesh *m, behavior *b,
                      struct osub *testsubseg)
{
  struct otri neighbortri;
  struct osub testsym;
  struct badsubseg *encroachedseg;
  REAL dotproduct;
  int encroached;
  int sides;
  vertex eorg, edest, eapex;
  triangle ptr;                     /* Temporary variable used by stpivot(). */

  encroached = 0;
  sides = 0;

  sorg(*testsubseg, eorg);
  sdest(*testsubseg, edest);
  /* Check one neighbor of the subsegment. */
  stpivot(*testsubseg, neighbortri);
  /* Does the neighbor exist, or is this a boundary edge? */
  if (neighbortri.tri != m->dummytri) {
    sides++;
    /* Find a vertex opposite this subsegment. */
    apex(neighbortri, eapex);
    /* Check whether the apex is in the diametral lens of the subsegment */
    /*   (the diametral circle if `conformdel' is set).  A dot product   */
    /*   of two sides of the triangle is used to check whether the angle */
    /*   at the apex is greater than (180 - 2 `minangle') degrees (for   */
    /*   lenses; 90 degrees for diametral circles).                      */
    dotproduct = (eorg[0] - eapex[0]) * (edest[0] - eapex[0]) +
                 (eorg[1] - eapex[1]) * (edest[1] - eapex[1]);
    if (dotproduct < 0.0) {
      if (b->conformdel ||
          (dotproduct * dotproduct >=
           (2.0 * b->goodangle - 1.0) * (2.0 * b->goodangle - 1.0) *
           ((eorg[0] - eapex[0]) * (eorg[0] - eapex[0]) +
            (eorg[1] - eapex[1]) * (eorg[1] - eapex[1])) *
           ((edest[0] - eapex[0]) * (edest[0] - eapex[0]) +
            (edest[1] - eapex[1]) * (edest[1] - eapex[1])))) {
        encroached = 1;
      }
    }
  }
  /* Check the other neighbor of the subsegment. */
  ssym(*testsubseg, testsym);
  stpivot(testsym, neighbortri);
  /* Does the neighbor exist, or is this a boundary edge? */
  if (neighbortri.tri != m->dummytri) {
    sides++;
    /* Find the other vertex opposite this subsegment. */
    apex(neighbortri, eapex);
    /* Check whether the apex is in the diametral lens of the subsegment */
    /*   (or the diametral circle, if `conformdel' is set).              */
    dotproduct = (eorg[0] - eapex[0]) * (edest[0] - eapex[0]) +
                 (eorg[1] - eapex[1]) * (edest[1] - eapex[1]);
    if (dotproduct < 0.0) {
      if (b->conformdel ||
          (dotproduct * dotproduct >=
           (2.0 * b->goodangle - 1.0) * (2.0 * b->goodangle - 1.0) *
           ((eorg[0] - eapex[0]) * (eorg[0] - eapex[0]) +
            (eorg[1] - eapex[1]) * (eorg[1] - eapex[1])) *
           ((edest[0] - eapex[0]) * (edest[0] - eapex[0]) +
            (edest[1] - eapex[1]) * (edest[1] - eapex[1])))) {
        encroached += 2;
      }
    }
  }

  if (encroached && (!b->nobisect || ((b->nobisect == 1) && (sides == 2)))) {
    /* Add the subsegment to the list of encroached subsegments. */
    /*   Be sure to get the orientation right.                   */
    encroachedseg = (struct badsubseg *) poolalloc(&m->badsubsegs);
    if (encroached == 1) {
      encroachedseg->encsubseg = sencode(*testsubseg);
      encroachedseg->subsegorg = eorg;
      encroachedseg->subsegdest = edest;
    } else {
      encroachedseg->encsubseg = sencode(testsym);
      encroachedseg->subsegorg = edest;
      encroachedseg->subsegdest = eorg;
    }
  }

  return encroached;
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  testtriangle()   Test a triangle for quality and size.                   */
/*                                                                           */
/*  Tests a triangle to see if it satisfies the minimum angle condition and  */
/*  the maximum area condition.  Triangles that aren't up to spec are added  */
/*  to the bad triangle queue.                                               */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void testtriangle(mesh *m, behavior *b, struct otri *testtri)
{
  struct otri tri1, tri2;
  struct osub testsub;
  vertex torg, tdest, tapex;
  vertex base1, base2;
  vertex org1, dest1, org2, dest2;
  vertex joinvertex;
  REAL dxod, dyod, dxda, dyda, dxao, dyao;
  REAL dxod2, dyod2, dxda2, dyda2, dxao2, dyao2;
  REAL apexlen, orglen, destlen, minedge;
  REAL angle;
  REAL area;
  REAL dist1, dist2;
#ifndef NO_ACUTE
  REAL maxedge, maxangle;
#endif
  subseg sptr;                      /* Temporary variable used by tspivot(). */
  triangle ptr;           /* Temporary variable used by oprev() and dnext(). */

  org(*testtri, torg);
  dest(*testtri, tdest);
  apex(*testtri, tapex);
  dxod = torg[0] - tdest[0];
  dyod = torg[1] - tdest[1];
  dxda = tdest[0] - tapex[0];
  dyda = tdest[1] - tapex[1];
  dxao = tapex[0] - torg[0];
  dyao = tapex[1] - torg[1];
  dxod2 = dxod * dxod;
  dyod2 = dyod * dyod;
  dxda2 = dxda * dxda;
  dyda2 = dyda * dyda;
  dxao2 = dxao * dxao;
  dyao2 = dyao * dyao;
  /* Find the lengths of the triangle's three edges. */
  apexlen = dxod2 + dyod2;
  orglen = dxda2 + dyda2;
  destlen = dxao2 + dyao2;

  if ((apexlen < orglen) && (apexlen < destlen)) {
    /* The edge opposite the apex is shortest. */
    minedge = apexlen;
    /* Find the square of the cosine of the angle at the apex. */
    angle = dxda * dxao + dyda * dyao;
    angle = angle * angle / (orglen * destlen);
    base1 = torg;
    base2 = tdest;
    otricopy(*testtri, tri1);
  } else if (orglen < destlen) {
    /* The edge opposite the origin is shortest. */
    minedge = orglen;
    /* Find the square of the cosine of the angle at the origin. */
    angle = dxod * dxao + dyod * dyao;
    angle = angle * angle / (apexlen * destlen);
    base1 = tdest;
    base2 = tapex;
    lnext(*testtri, tri1);
  } else {
    /* The edge opposite the destination is shortest. */
    minedge = destlen;
    /* Find the square of the cosine of the angle at the destination. */
    angle = dxod * dxda + dyod * dyda;
    angle = angle * angle / (apexlen * orglen);
    base1 = tapex;
    base2 = torg;
    lprev(*testtri, tri1);
  }

  if (b->vararea || b->fixedarea || b->usertest) {
    /* Check whether the area is larger than permitted. */
    area = 0.5 * (dxod * dyda - dyod * dxda);
    if (b->fixedarea && (area > b->maxarea)) {
      /* Add this triangle to the list of bad triangles. */
      enqueuebadtri(m, b, testtri, minedge, tapex, torg, tdest);
      return;
    }

    /* Nonpositive area constraints are treated as unconstrained. */
    if ((b->vararea) && (area > areabound(*testtri)) &&
        (areabound(*testtri) > 0.0)) {
      /* Add this triangle to the list of bad triangles. */
      enqueuebadtri(m, b, testtri, minedge, tapex, torg, tdest);
      return;
    }

    if (b->usertest) {
      /* Check whether the user thinks this triangle is too large. */
      if (b->triunsuitable_user_func && b->triunsuitable_user_func(torg, tdest, tapex, area)) {
        enqueuebadtri(m, b, testtri, minedge, tapex, torg, tdest);
        return;
      }
    }
  }

#ifndef NO_ACUTE
	// find the maximum edge and accordingly the pqr orientation
	if ((apexlen > orglen) && (apexlen > destlen)) {
		/* The edge opposite the apex is longest. */
		maxedge = apexlen;
		/* Find the cosine of the angle at the apex. */
		maxangle = (orglen + destlen - apexlen)/ (2*sqrt(orglen*destlen));	
	} else if (orglen > destlen) {
		/* The edge opposite the origin is longest. */
		maxedge = orglen;
		/* Find the cosine of the angle at the origin. */
		maxangle = (apexlen + destlen - orglen)/(2*sqrt(apexlen*destlen));
	} else {
		/* The edge opposite the destination is longest. */
		maxedge = destlen;
		/* Find the cosine of the angle at the destination. */
		maxangle = (apexlen + orglen -destlen)/(2*sqrt(apexlen*orglen));
	}
#endif

  /* Check whether the angle is smaller than permitted. */
#ifndef NO_ACUTE
  if ((angle > b->goodangle)  ||  (maxangle < b->maxgoodangle && b->maxangle != 0.0)) {
#else
  if (angle > b->goodangle) {
#endif
    /* Use the rules of Miller, Pav, and Walkington to decide that certain */
    /*   triangles should not be split, even if they have bad angles.      */
    /*   A skinny triangle is not split if its shortest edge subtends a    */
    /*   small input angle, and both endpoints of the edge lie on a        */
    /*   concentric circular shell.  For convenience, I make a small       */
    /*   adjustment to that rule:  I check if the endpoints of the edge    */
    /*   both lie in segment interiors, equidistant from the apex where    */
    /*   the two segments meet.                                            */
    /* First, check if both points lie in segment interiors.               */
    if ((vertextype(base1) == SEGMENTVERTEX) &&
        (vertextype(base2) == SEGMENTVERTEX)) {
      /* Check if both points lie in a common segment.  If they do, the */
      /*   skinny triangle is enqueued to be split as usual.            */
      tspivot(tri1, testsub);
      if (testsub.ss == m->dummysub) {
        /* No common segment.  Find a subsegment that contains `torg'. */
        otricopy(tri1, tri2);
        do {
          oprevself(tri1);
          tspivot(tri1, testsub);
        } while (testsub.ss == m->dummysub);
        /* Find the endpoints of the containing segment. */
        segorg(testsub, org1);
        segdest(testsub, dest1);
        /* Find a subsegment that contains `tdest'. */
        do {
          dnextself(tri2);
          tspivot(tri2, testsub);
        } while (testsub.ss == m->dummysub);
        /* Find the endpoints of the containing segment. */
        segorg(testsub, org2);
        segdest(testsub, dest2);
        /* Check if the two containing segments have an endpoint in common. */
        joinvertex = (vertex) NULL;
        if ((dest1[0] == org2[0]) && (dest1[1] == org2[1])) {
          joinvertex = dest1;
        } else if ((org1[0] == dest2[0]) && (org1[1] == dest2[1])) {
          joinvertex = org1;
        }
        if (joinvertex != (vertex) NULL) {
          /* Compute the distance from the common endpoint (of the two  */
          /*   segments) to each of the endpoints of the shortest edge. */
          dist1 = ((base1[0] - joinvertex[0]) * (base1[0] - joinvertex[0]) +
                   (base1[1] - joinvertex[1]) * (base1[1] - joinvertex[1]));
          dist2 = ((base2[0] - joinvertex[0]) * (base2[0] - joinvertex[0]) +
                   (base2[1] - joinvertex[1]) * (base2[1] - joinvertex[1]));
          /* If the two distances are equal, don't split the triangle. */
          if ((dist1 < 1.001 * dist2) && (dist1 > 0.999 * dist2)) {
            /* Return now to avoid enqueueing the bad triangle. */
            return;
          }
        }
      }
    }

    /* Add this triangle to the list of bad triangles. */
    enqueuebadtri(m, b, testtri, minedge, tapex, torg, tdest);
  }
}

#endif /* not CDT_ONLY */

/**                                                                         **/
/**                                                                         **/
/********* Mesh quality testing routines end here                    *********/

/********* Point location routines begin here                        *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  makevertexmap()   Construct a mapping from vertices to triangles to      */
/*                    improve the speed of point location for segment        */
/*                    insertion.                                             */
/*                                                                           */
/*  Traverses all the triangles, and provides each corner of each triangle   */
/*  with a pointer to that triangle.  Of course, pointers will be            */
/*  overwritten by other pointers because (almost) each vertex is a corner   */
/*  of several triangles, but in the end every vertex will point to some     */
/*  triangle that contains it.                                               */
/*                                                                           */
/*****************************************************************************/

void makevertexmap(mesh *m, behavior *b)
{
  struct otri triangleloop;
  vertex triorg;

  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  while (triangleloop.tri != (triangle *) NULL) {
    /* Check all three vertices of the triangle. */
    for (triangleloop.orient = 0; triangleloop.orient < 3;
         triangleloop.orient++) {
      org(triangleloop, triorg);
      setvertex2tri(triorg, encode(triangleloop));
    }
    triangleloop.tri = triangletraverse(m);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  preciselocate()   Find a triangle or edge containing a given point.      */
/*                                                                           */
/*  Begins its search from `searchtri'.  It is important that `searchtri'    */
/*  be a handle with the property that `searchpoint' is strictly to the left */
/*  of the edge denoted by `searchtri', or is collinear with that edge and   */
/*  does not intersect that edge.  (In particular, `searchpoint' should not  */
/*  be the origin or destination of that edge.)                              */
/*                                                                           */
/*  These conditions are imposed because preciselocate() is normally used in */
/*  one of two situations:                                                   */
/*                                                                           */
/*  (1)  To try to find the location to insert a new point.  Normally, we    */
/*       know an edge that the point is strictly to the left of.  In the     */
/*       incremental Delaunay algorithm, that edge is a bounding box edge.   */
/*       In Ruppert's Delaunay refinement algorithm for quality meshing,     */
/*       that edge is the shortest edge of the triangle whose circumcenter   */
/*       is being inserted.                                                  */
/*                                                                           */
/*  (2)  To try to find an existing point.  In this case, any edge on the    */
/*       convex hull is a good starting edge.  You must screen out the       */
/*       possibility that the vertex sought is an endpoint of the starting   */
/*       edge before you call preciselocate().                               */
/*                                                                           */
/*  On completion, `searchtri' is a triangle that contains `searchpoint'.    */
/*                                                                           */
/*  This implementation differs from that given by Guibas and Stolfi.  It    */
/*  walks from triangle to triangle, crossing an edge only if `searchpoint'  */
/*  is on the other side of the line containing that edge.  After entering   */
/*  a triangle, there are two edges by which one can leave that triangle.    */
/*  If both edges are valid (`searchpoint' is on the other side of both      */
/*  edges), one of the two is chosen by drawing a line perpendicular to      */
/*  the entry edge (whose endpoints are `forg' and `fdest') passing through  */
/*  `fapex'.  Depending on which side of this perpendicular `searchpoint'    */
/*  falls on, an exit edge is chosen.                                        */
/*                                                                           */
/*  This implementation is empirically faster than the Guibas and Stolfi     */
/*  point location routine (which I originally used), which tends to spiral  */
/*  in toward its target.                                                    */
/*                                                                           */
/*  Returns ONVERTEX if the point lies on an existing vertex.  `searchtri'   */
/*  is a handle whose origin is the existing vertex.                         */
/*                                                                           */
/*  Returns ONEDGE if the point lies on a mesh edge.  `searchtri' is a       */
/*  handle whose primary edge is the edge on which the point lies.           */
/*                                                                           */
/*  Returns INTRIANGLE if the point lies strictly within a triangle.         */
/*  `searchtri' is a handle on the triangle that contains the point.         */
/*                                                                           */
/*  Returns OUTSIDE if the point lies outside the mesh.  `searchtri' is a    */
/*  handle whose primary edge the point is to the right of.  This might      */
/*  occur when the circumcenter of a triangle falls just slightly outside    */
/*  the mesh due to floating-point roundoff error.  It also occurs when      */
/*  seeking a hole or region point that a foolish user has placed outside    */
/*  the mesh.                                                                */
/*                                                                           */
/*  If `stopatsubsegment' is nonzero, the search will stop if it tries to    */
/*  walk through a subsegment, and will return OUTSIDE.                      */
/*                                                                           */
/*  WARNING:  This routine is designed for convex triangulations, and will   */
/*  not generally work after the holes and concavities have been carved.     */
/*  However, it can still be used to find the circumcenter of a triangle, as */
/*  long as the search is begun from the triangle in question.               */
/*                                                                           */
/*****************************************************************************/

enum locateresult preciselocate(mesh *m, behavior *b,
                                vertex searchpoint, struct otri *searchtri,
                                int stopatsubsegment)
{
  struct otri backtracktri;
  struct osub checkedge;
  vertex forg, fdest, fapex;
  REAL orgorient, destorient;
  int moveleft;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Where are we? */
  org(*searchtri, forg);
  dest(*searchtri, fdest);
  apex(*searchtri, fapex);
  while (1) {
    /* Check whether the apex is the point we seek. */
    if ((fapex[0] == searchpoint[0]) && (fapex[1] == searchpoint[1])) {
      lprevself(*searchtri);
      return ONVERTEX;
    }
    /* Does the point lie on the other side of the line defined by the */
    /*   triangle edge opposite the triangle's destination?            */
    destorient = counterclockwise(m, b, forg, fapex, searchpoint);
    /* Does the point lie on the other side of the line defined by the */
    /*   triangle edge opposite the triangle's origin?                 */
    orgorient = counterclockwise(m, b, fapex, fdest, searchpoint);
    if (destorient > 0.0) {
      if (orgorient > 0.0) {
        /* Move left if the inner product of (fapex - searchpoint) and  */
        /*   (fdest - forg) is positive.  This is equivalent to drawing */
        /*   a line perpendicular to the line (forg, fdest) and passing */
        /*   through `fapex', and determining which side of this line   */
        /*   `searchpoint' falls on.                                    */
        moveleft = (fapex[0] - searchpoint[0]) * (fdest[0] - forg[0]) +
                   (fapex[1] - searchpoint[1]) * (fdest[1] - forg[1]) > 0.0;
      } else {
        moveleft = 1;
      }
    } else {
      if (orgorient > 0.0) {
        moveleft = 0;
      } else {
        /* The point we seek must be on the boundary of or inside this */
        /*   triangle.                                                 */
        if (destorient == 0.0) {
          lprevself(*searchtri);
          return ONEDGE;
        }
        if (orgorient == 0.0) {
          lnextself(*searchtri);
          return ONEDGE;
        }
        return INTRIANGLE;
      }
    }

    /* Move to another triangle.  Leave a trace `backtracktri' in case */
    /*   floating-point roundoff or some such bogey causes us to walk  */
    /*   off a boundary of the triangulation.                          */
    if (moveleft) {
      lprev(*searchtri, backtracktri);
      fdest = fapex;
    } else {
      lnext(*searchtri, backtracktri);
      forg = fapex;
    }
    sym(backtracktri, *searchtri);

    if (m->checksegments && stopatsubsegment) {
      /* Check for walking through a subsegment. */
      tspivot(backtracktri, checkedge);
      if (checkedge.ss != m->dummysub) {
        /* Go back to the last triangle. */
        otricopy(backtracktri, *searchtri);
        return OUTSIDE;
      }
    }
    /* Check for walking right out of the triangulation. */
    if (searchtri->tri == m->dummytri) {
      /* Go back to the last triangle. */
      otricopy(backtracktri, *searchtri);
      return OUTSIDE;
    }

    apex(*searchtri, fapex);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  locate()   Find a triangle or edge containing a given point.             */
/*                                                                           */
/*  Searching begins from one of:  the input `searchtri', a recently         */
/*  encountered triangle `recenttri', or from a triangle chosen from a       */
/*  random sample.  The choice is made by determining which triangle's       */
/*  origin is closest to the point we are searching for.  Normally,          */
/*  `searchtri' should be a handle on the convex hull of the triangulation.  */
/*                                                                           */
/*  Details on the random sampling method can be found in the Mucke, Saias,  */
/*  and Zhu paper cited in the header of this code.                          */
/*                                                                           */
/*  On completion, `searchtri' is a triangle that contains `searchpoint'.    */
/*                                                                           */
/*  Returns ONVERTEX if the point lies on an existing vertex.  `searchtri'   */
/*  is a handle whose origin is the existing vertex.                         */
/*                                                                           */
/*  Returns ONEDGE if the point lies on a mesh edge.  `searchtri' is a       */
/*  handle whose primary edge is the edge on which the point lies.           */
/*                                                                           */
/*  Returns INTRIANGLE if the point lies strictly within a triangle.         */
/*  `searchtri' is a handle on the triangle that contains the point.         */
/*                                                                           */
/*  Returns OUTSIDE if the point lies outside the mesh.  `searchtri' is a    */
/*  handle whose primary edge the point is to the right of.  This might      */
/*  occur when the circumcenter of a triangle falls just slightly outside    */
/*  the mesh due to floating-point roundoff error.  It also occurs when      */
/*  seeking a hole or region point that a foolish user has placed outside    */
/*  the mesh.                                                                */
/*                                                                           */
/*  WARNING:  This routine is designed for convex triangulations, and will   */
/*  not generally work after the holes and concavities have been carved.     */
/*                                                                           */
/*****************************************************************************/

enum locateresult locate(mesh *m, behavior *b,
                         vertex searchpoint, struct otri *searchtri)
{
  VOID **sampleblock;
  char *firsttri;
  struct otri sampletri;
  vertex torg, tdest;
  ULONG_PTR alignptr;
  REAL searchdist, dist;
  REAL ahead;
  long samplesperblock, totalsamplesleft, samplesleft;
  long population, totalpopulation;
  triangle ptr;                         /* Temporary variable used by sym(). */

  /* Record the distance from the suggested starting triangle to the */
  /*   point we seek.                                                */
  org(*searchtri, torg);
  searchdist = (searchpoint[0] - torg[0]) * (searchpoint[0] - torg[0]) +
               (searchpoint[1] - torg[1]) * (searchpoint[1] - torg[1]);

  /* If a recently encountered triangle has been recorded and has not been */
  /*   deallocated, test it as a good starting point.                      */
  if (m->recenttri.tri != (triangle *) NULL) {
    if (!deadtri(m->recenttri.tri)) {
      org(m->recenttri, torg);
      if ((torg[0] == searchpoint[0]) && (torg[1] == searchpoint[1])) {
        otricopy(m->recenttri, *searchtri);
        return ONVERTEX;
      }
      dist = (searchpoint[0] - torg[0]) * (searchpoint[0] - torg[0]) +
             (searchpoint[1] - torg[1]) * (searchpoint[1] - torg[1]);
      if (dist < searchdist) {
        otricopy(m->recenttri, *searchtri);
        searchdist = dist;
      }
    }
  }

  /* The number of random samples taken is proportional to the cube root of */
  /*   the number of triangles in the mesh.  The next bit of code assumes   */
  /*   that the number of triangles increases monotonically (or at least    */
  /*   doesn't decrease enough to matter).                                  */
  while (SAMPLEFACTOR * m->samples * m->samples * m->samples <
         m->triangles.items) {
    m->samples++;
  }

  /* We'll draw ceiling(samples * TRIPERBLOCK / maxitems) random samples  */
  /*   from each block of triangles (except the first)--until we meet the */
  /*   sample quota.  The ceiling means that blocks at the end might be   */
  /*   neglected, but I don't care.                                       */
  samplesperblock = (m->samples * TRIPERBLOCK - 1) / m->triangles.maxitems + 1;
  /* We'll draw ceiling(samples * itemsfirstblock / maxitems) random samples */
  /*   from the first block of triangles.                                    */
  samplesleft = (m->samples * m->triangles.itemsfirstblock - 1) /
                m->triangles.maxitems + 1;
  totalsamplesleft = m->samples;
  population = m->triangles.itemsfirstblock;
  totalpopulation = m->triangles.maxitems;
  sampleblock = m->triangles.firstblock;
  sampletri.orient = 0;
  while (totalsamplesleft > 0) {
    /* If we're in the last block, `population' needs to be corrected. */
    if (population > totalpopulation) {
      population = totalpopulation;
    }
    /* Find a pointer to the first triangle in the block. */
    alignptr = (ULONG_PTR) (sampleblock + 1);
    firsttri = (char *) (alignptr +
                         (ULONG_PTR) m->triangles.alignbytes -
                         (alignptr %
                          (ULONG_PTR) m->triangles.alignbytes));

    /* Choose `samplesleft' randomly sampled triangles in this block. */
    do {
      sampletri.tri = (triangle *) (firsttri +
                                    (randomnation((unsigned int) population) *
                                     m->triangles.itembytes));
      if (!deadtri(sampletri.tri)) {
        org(sampletri, torg);
        dist = (searchpoint[0] - torg[0]) * (searchpoint[0] - torg[0]) +
               (searchpoint[1] - torg[1]) * (searchpoint[1] - torg[1]);
        if (dist < searchdist) {
          otricopy(sampletri, *searchtri);
          searchdist = dist;
        }
      }

      samplesleft--;
      totalsamplesleft--;
    } while ((samplesleft > 0) && (totalsamplesleft > 0));

    if (totalsamplesleft > 0) {
      sampleblock = (VOID **) *sampleblock;
      samplesleft = samplesperblock;
      totalpopulation -= population;
      population = TRIPERBLOCK;
    }
  }

  /* Where are we? */
  org(*searchtri, torg);
  dest(*searchtri, tdest);
  /* Check the starting triangle's vertices. */
  if ((torg[0] == searchpoint[0]) && (torg[1] == searchpoint[1])) {
    return ONVERTEX;
  }
  if ((tdest[0] == searchpoint[0]) && (tdest[1] == searchpoint[1])) {
    lnextself(*searchtri);
    return ONVERTEX;
  }
  /* Orient `searchtri' to fit the preconditions of calling preciselocate(). */
  ahead = counterclockwise(m, b, torg, tdest, searchpoint);
  if (ahead < 0.0) {
    /* Turn around so that `searchpoint' is to the left of the */
    /*   edge specified by `searchtri'.                        */
    symself(*searchtri);
  } else if (ahead == 0.0) {
    /* Check if `searchpoint' is between `torg' and `tdest'. */
    if (((torg[0] < searchpoint[0]) == (searchpoint[0] < tdest[0])) &&
        ((torg[1] < searchpoint[1]) == (searchpoint[1] < tdest[1]))) {
      return ONEDGE;
    }
  }
  return preciselocate(m, b, searchpoint, searchtri, 0);
}

/**                                                                         **/
/**                                                                         **/
/********* Point location routines end here                          *********/

/********* Mesh transformation routines begin here                   *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  insertsubseg()   Create a new subsegment and insert it between two       */
/*                   triangles.                                              */
/*                                                                           */
/*  The new subsegment is inserted at the edge described by the handle       */
/*  `tri'.  Its vertices are properly initialized.  The marker `subsegmark'  */
/*  is applied to the subsegment and, if appropriate, its vertices.          */
/*                                                                           */
/*****************************************************************************/

void insertsubseg(mesh *m, behavior *b, struct otri *tri,
                  int subsegmark)
{
  struct otri oppotri;
  struct osub newsubseg;
  vertex triorg, tridest;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  org(*tri, triorg);
  dest(*tri, tridest);
  /* Mark vertices if possible. */
  if (vertexmark(triorg) == 0) {
    setvertexmark(triorg, subsegmark);
  }
  if (vertexmark(tridest) == 0) {
    setvertexmark(tridest, subsegmark);
  }
  /* Check if there's already a subsegment here. */
  tspivot(*tri, newsubseg);
  if (newsubseg.ss == m->dummysub) {
    /* Make new subsegment and initialize its vertices. */
    makesubseg(m, &newsubseg);
    setsorg(newsubseg, tridest);
    setsdest(newsubseg, triorg);
    setsegorg(newsubseg, tridest);
    setsegdest(newsubseg, triorg);
    /* Bond new subsegment to the two triangles it is sandwiched between. */
    /*   Note that the facing triangle `oppotri' might be equal to        */
    /*   `dummytri' (outer space), but the new subsegment is bonded to it */
    /*   all the same.                                                    */
    tsbond(*tri, newsubseg);
    sym(*tri, oppotri);
    ssymself(newsubseg);
    tsbond(oppotri, newsubseg);
    setmark(newsubseg, subsegmark);
  } else {
    if (mark(newsubseg) == 0) {
      setmark(newsubseg, subsegmark);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  Terminology                                                              */
/*                                                                           */
/*  A "local transformation" replaces a small set of triangles with another  */
/*  set of triangles.  This may or may not involve inserting or deleting a   */
/*  vertex.                                                                  */
/*                                                                           */
/*  The term "casing" is used to describe the set of triangles that are      */
/*  attached to the triangles being transformed, but are not transformed     */
/*  themselves.  Think of the casing as a fixed hollow structure inside      */
/*  which all the action happens.  A "casing" is only defined relative to    */
/*  a single transformation; each occurrence of a transformation will        */
/*  involve a different casing.                                              */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  flip()   Transform two triangles to two different triangles by flipping  */
/*           an edge counterclockwise within a quadrilateral.                */
/*                                                                           */
/*  Imagine the original triangles, abc and bad, oriented so that the        */
/*  shared edge ab lies in a horizontal plane, with the vertex b on the left */
/*  and the vertex a on the right.  The vertex c lies below the edge, and    */
/*  the vertex d lies above the edge.  The `flipedge' handle holds the edge  */
/*  ab of triangle abc, and is directed left, from vertex a to vertex b.     */
/*                                                                           */
/*  The triangles abc and bad are deleted and replaced by the triangles cdb  */
/*  and dca.  The triangles that represent abc and bad are NOT deallocated;  */
/*  they are reused for dca and cdb, respectively.  Hence, any handles that  */
/*  may have held the original triangles are still valid, although not       */
/*  directed as they were before.                                            */
/*                                                                           */
/*  Upon completion of this routine, the `flipedge' handle holds the edge    */
/*  dc of triangle dca, and is directed down, from vertex d to vertex c.     */
/*  (Hence, the two triangles have rotated counterclockwise.)                */
/*                                                                           */
/*  WARNING:  This transformation is geometrically valid only if the         */
/*  quadrilateral adbc is convex.  Furthermore, this transformation is       */
/*  valid only if there is not a subsegment between the triangles abc and    */
/*  bad.  This routine does not check either of these preconditions, and     */
/*  it is the responsibility of the calling routine to ensure that they are  */
/*  met.  If they are not, the streets shall be filled with wailing and      */
/*  gnashing of teeth.                                                       */
/*                                                                           */
/*****************************************************************************/

void flip(mesh *m, behavior *b, struct otri *flipedge)
{
  struct otri botleft, botright;
  struct otri topleft, topright;
  struct otri top;
  struct otri botlcasing, botrcasing;
  struct otri toplcasing, toprcasing;
  struct osub botlsubseg, botrsubseg;
  struct osub toplsubseg, toprsubseg;
  vertex leftvertex, rightvertex, botvertex;
  vertex farvertex;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Identify the vertices of the quadrilateral. */
  org(*flipedge, rightvertex);
  dest(*flipedge, leftvertex);
  apex(*flipedge, botvertex);
  sym(*flipedge, top);
#ifdef SELF_CHECK
  if (top.tri == m->dummytri) {
    printf("Internal error in flip():  Attempt to flip on boundary.\n");
    lnextself(*flipedge);
    return;
  }
  if (m->checksegments) {
    tspivot(*flipedge, toplsubseg);
    if (toplsubseg.ss != m->dummysub) {
      printf("Internal error in flip():  Attempt to flip a segment.\n");
      lnextself(*flipedge);
      return;
    }
  }
#endif /* SELF_CHECK */
  apex(top, farvertex);

  /* Identify the casing of the quadrilateral. */
  lprev(top, topleft);
  sym(topleft, toplcasing);
  lnext(top, topright);
  sym(topright, toprcasing);
  lnext(*flipedge, botleft);
  sym(botleft, botlcasing);
  lprev(*flipedge, botright);
  sym(botright, botrcasing);
  /* Rotate the quadrilateral one-quarter turn counterclockwise. */
  bond(topleft, botlcasing);
  bond(botleft, botrcasing);
  bond(botright, toprcasing);
  bond(topright, toplcasing);

  if (m->checksegments) {
    /* Check for subsegments and rebond them to the quadrilateral. */
    tspivot(topleft, toplsubseg);
    tspivot(botleft, botlsubseg);
    tspivot(botright, botrsubseg);
    tspivot(topright, toprsubseg);
    if (toplsubseg.ss == m->dummysub) {
      tsdissolve(topright);
    } else {
      tsbond(topright, toplsubseg);
    }
    if (botlsubseg.ss == m->dummysub) {
      tsdissolve(topleft);
    } else {
      tsbond(topleft, botlsubseg);
    }
    if (botrsubseg.ss == m->dummysub) {
      tsdissolve(botleft);
    } else {
      tsbond(botleft, botrsubseg);
    }
    if (toprsubseg.ss == m->dummysub) {
      tsdissolve(botright);
    } else {
      tsbond(botright, toprsubseg);
    }
  }

  /* New vertex assignments for the rotated quadrilateral. */
  setorg(*flipedge, farvertex);
  setdest(*flipedge, botvertex);
  setapex(*flipedge, rightvertex);
  setorg(top, botvertex);
  setdest(top, farvertex);
  setapex(top, leftvertex);
}

/*****************************************************************************/
/*                                                                           */
/*  unflip()   Transform two triangles to two different triangles by         */
/*             flipping an edge clockwise within a quadrilateral.  Reverses  */
/*             the flip() operation so that the data structures representing */
/*             the triangles are back where they were before the flip().     */
/*                                                                           */
/*  Imagine the original triangles, abc and bad, oriented so that the        */
/*  shared edge ab lies in a horizontal plane, with the vertex b on the left */
/*  and the vertex a on the right.  The vertex c lies below the edge, and    */
/*  the vertex d lies above the edge.  The `flipedge' handle holds the edge  */
/*  ab of triangle abc, and is directed left, from vertex a to vertex b.     */
/*                                                                           */
/*  The triangles abc and bad are deleted and replaced by the triangles cdb  */
/*  and dca.  The triangles that represent abc and bad are NOT deallocated;  */
/*  they are reused for cdb and dca, respectively.  Hence, any handles that  */
/*  may have held the original triangles are still valid, although not       */
/*  directed as they were before.                                            */
/*                                                                           */
/*  Upon completion of this routine, the `flipedge' handle holds the edge    */
/*  cd of triangle cdb, and is directed up, from vertex c to vertex d.       */
/*  (Hence, the two triangles have rotated clockwise.)                       */
/*                                                                           */
/*  WARNING:  This transformation is geometrically valid only if the         */
/*  quadrilateral adbc is convex.  Furthermore, this transformation is       */
/*  valid only if there is not a subsegment between the triangles abc and    */
/*  bad.  This routine does not check either of these preconditions, and     */
/*  it is the responsibility of the calling routine to ensure that they are  */
/*  met.  If they are not, the streets shall be filled with wailing and      */
/*  gnashing of teeth.                                                       */
/*                                                                           */
/*****************************************************************************/

void unflip(mesh *m, behavior *b, struct otri *flipedge)
{
  struct otri botleft, botright;
  struct otri topleft, topright;
  struct otri top;
  struct otri botlcasing, botrcasing;
  struct otri toplcasing, toprcasing;
  struct osub botlsubseg, botrsubseg;
  struct osub toplsubseg, toprsubseg;
  vertex leftvertex, rightvertex, botvertex;
  vertex farvertex;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Identify the vertices of the quadrilateral. */
  org(*flipedge, rightvertex);
  dest(*flipedge, leftvertex);
  apex(*flipedge, botvertex);
  sym(*flipedge, top);
#ifdef SELF_CHECK
  if (top.tri == m->dummytri) {
    printf("Internal error in unflip():  Attempt to flip on boundary.\n");
    lnextself(*flipedge);
    return;
  }
  if (m->checksegments) {
    tspivot(*flipedge, toplsubseg);
    if (toplsubseg.ss != m->dummysub) {
      printf("Internal error in unflip():  Attempt to flip a subsegment.\n");
      lnextself(*flipedge);
      return;
    }
  }
#endif /* SELF_CHECK */
  apex(top, farvertex);

  /* Identify the casing of the quadrilateral. */
  lprev(top, topleft);
  sym(topleft, toplcasing);
  lnext(top, topright);
  sym(topright, toprcasing);
  lnext(*flipedge, botleft);
  sym(botleft, botlcasing);
  lprev(*flipedge, botright);
  sym(botright, botrcasing);
  /* Rotate the quadrilateral one-quarter turn clockwise. */
  bond(topleft, toprcasing);
  bond(botleft, toplcasing);
  bond(botright, botlcasing);
  bond(topright, botrcasing);

  if (m->checksegments) {
    /* Check for subsegments and rebond them to the quadrilateral. */
    tspivot(topleft, toplsubseg);
    tspivot(botleft, botlsubseg);
    tspivot(botright, botrsubseg);
    tspivot(topright, toprsubseg);
    if (toplsubseg.ss == m->dummysub) {
      tsdissolve(botleft);
    } else {
      tsbond(botleft, toplsubseg);
    }
    if (botlsubseg.ss == m->dummysub) {
      tsdissolve(botright);
    } else {
      tsbond(botright, botlsubseg);
    }
    if (botrsubseg.ss == m->dummysub) {
      tsdissolve(topright);
    } else {
      tsbond(topright, botrsubseg);
    }
    if (toprsubseg.ss == m->dummysub) {
      tsdissolve(topleft);
    } else {
      tsbond(topleft, toprsubseg);
    }
  }

  /* New vertex assignments for the rotated quadrilateral. */
  setorg(*flipedge, botvertex);
  setdest(*flipedge, farvertex);
  setapex(*flipedge, leftvertex);
  setorg(top, farvertex);
  setdest(top, botvertex);
  setapex(top, rightvertex);
}

/*****************************************************************************/
/*                                                                           */
/*  insertvertex()   Insert a vertex into a Delaunay triangulation,          */
/*                   performing flips as necessary to maintain the Delaunay  */
/*                   property.                                               */
/*                                                                           */
/*  The point `insertvertex' is located.  If `searchtri.tri' is not NULL,    */
/*  the search for the containing triangle begins from `searchtri'.  If      */
/*  `searchtri.tri' is NULL, a full point location procedure is called.      */
/*  If `insertvertex' is found inside a triangle, the triangle is split into */
/*  three; if `insertvertex' lies on an edge, the edge is split in two,      */
/*  thereby splitting the two adjacent triangles into four.  Edge flips are  */
/*  used to restore the Delaunay property.  If `insertvertex' lies on an     */
/*  existing vertex, no action is taken, and the value DUPLICATEVERTEX is    */
/*  returned.  On return, `searchtri' is set to a handle whose origin is the */
/*  existing vertex.                                                         */
/*                                                                           */
/*  Normally, the parameter `splitseg' is set to NULL, implying that no      */
/*  subsegment should be split.  In this case, if `insertvertex' is found to */
/*  lie on a segment, no action is taken, and the value VIOLATINGVERTEX is   */
/*  returned.  On return, `searchtri' is set to a handle whose primary edge  */
/*  is the violated subsegment.                                              */
/*                                                                           */
/*  If the calling routine wishes to split a subsegment by inserting a       */
/*  vertex in it, the parameter `splitseg' should be that subsegment.  In    */
/*  this case, `searchtri' MUST be the triangle handle reached by pivoting   */
/*  from that subsegment; no point location is done.                         */
/*                                                                           */
/*  `segmentflaws' and `triflaws' are flags that indicate whether or not     */
/*  there should be checks for the creation of encroached subsegments or bad */
/*  quality triangles.  If a newly inserted vertex encroaches upon           */
/*  subsegments, these subsegments are added to the list of subsegments to   */
/*  be split if `segmentflaws' is set.  If bad triangles are created, these  */
/*  are added to the queue if `triflaws' is set.                             */
/*                                                                           */
/*  If a duplicate vertex or violated segment does not prevent the vertex    */
/*  from being inserted, the return value will be ENCROACHINGVERTEX if the   */
/*  vertex encroaches upon a subsegment (and checking is enabled), or        */
/*  SUCCESSFULVERTEX otherwise.  In either case, `searchtri' is set to a     */
/*  handle whose origin is the newly inserted vertex.                        */
/*                                                                           */
/*  insertvertex() does not use flip() for reasons of speed; some            */
/*  information can be reused from edge flip to edge flip, like the          */
/*  locations of subsegments.                                                */
/*                                                                           */
/*****************************************************************************/

enum insertvertexresult insertvertex(mesh *m, behavior *b,
                                     vertex newvertex, struct otri *searchtri,
                                     struct osub *splitseg,
                                     int segmentflaws, int triflaws,
                                     int attribs)
{
  struct otri horiz;
  struct otri top;
  struct otri botleft, botright;
  struct otri topleft, topright;
  struct otri newbotleft, newbotright;
  struct otri newtopright;
  struct otri botlcasing, botrcasing;
  struct otri toplcasing, toprcasing;
  struct otri testtri;
  struct osub botlsubseg, botrsubseg;
  struct osub toplsubseg, toprsubseg;
  struct osub brokensubseg;
  struct osub checksubseg;
  struct osub rightsubseg;
  struct osub newsubseg;
  struct badsubseg *encroached;
  struct flipstacker *newflip;
  vertex first;
  vertex leftvertex, rightvertex, botvertex, topvertex, farvertex;
  vertex segmentorg, segmentdest;
  REAL attrib;
  REAL area;
  enum insertvertexresult success;
  enum locateresult intersect;
  int doflip;
  int mirrorflag;
  int enq;
  int i;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;         /* Temporary variable used by spivot() and tspivot(). */

  if (splitseg == (struct osub *) NULL) {
    /* Find the location of the vertex to be inserted.  Check if a good */
    /*   starting triangle has already been provided by the caller.     */
    if (searchtri->tri == m->dummytri) {
      /* Find a boundary triangle. */
      horiz.tri = m->dummytri;
      horiz.orient = 0;
      symself(horiz);
      /* Search for a triangle containing `newvertex'. */
      intersect = locate(m, b, newvertex, &horiz);
    } else {
      /* Start searching from the triangle provided by the caller. */
      otricopy(*searchtri, horiz);
      intersect = preciselocate(m, b, newvertex, &horiz, 1);
    }
  } else {
    /* The calling routine provides the subsegment in which */
    /*   the vertex is inserted.                             */
    otricopy(*searchtri, horiz);
    intersect = ONEDGE;
  }

  if (intersect == ONVERTEX) {
    /* There's already a vertex there.  Return in `searchtri' a triangle */
    /*   whose origin is the existing vertex.                            */
    otricopy(horiz, *searchtri);
    otricopy(horiz, m->recenttri);
    return DUPLICATEVERTEX;
  }
  if ((intersect == ONEDGE) || (intersect == OUTSIDE)) {
    /* The vertex falls on an edge or boundary. */
    if (m->checksegments && (splitseg == (struct osub *) NULL)) {
      /* Check whether the vertex falls on a subsegment. */
      tspivot(horiz, brokensubseg);
      if (brokensubseg.ss != m->dummysub) {
        /* The vertex falls on a subsegment, and hence will not be inserted. */
        if (segmentflaws) {
          enq = b->nobisect != 2;
          if (enq && (b->nobisect == 1)) {
            /* This subsegment may be split only if it is an */
            /*   internal boundary.                          */
            sym(horiz, testtri);
            enq = testtri.tri != m->dummytri;
          }
          if (enq) {
            /* Add the subsegment to the list of encroached subsegments. */
            encroached = (struct badsubseg *) poolalloc(&m->badsubsegs);
            encroached->encsubseg = sencode(brokensubseg);
            sorg(brokensubseg, encroached->subsegorg);
            sdest(brokensubseg, encroached->subsegdest);
          }
        }
        /* Return a handle whose primary edge contains the vertex, */
        /*   which has not been inserted.                          */
        otricopy(horiz, *searchtri);
        otricopy(horiz, m->recenttri);
        return VIOLATINGVERTEX;
      }
    }

    /* Insert the vertex on an edge, dividing one triangle into two (if */
    /*   the edge lies on a boundary) or two triangles into four.       */
    lprev(horiz, botright);
    sym(botright, botrcasing);
    sym(horiz, topright);
    /* Is there a second triangle?  (Or does this edge lie on a boundary?) */
    mirrorflag = topright.tri != m->dummytri;
    if (mirrorflag) {
      lnextself(topright);
      sym(topright, toprcasing);
      maketriangle(m, b, &newtopright);
    } else {
      /* Splitting a boundary edge increases the number of boundary edges. */
      m->hullsize++;
    }
    maketriangle(m, b, &newbotright);

    /* Set the vertices of changed and new triangles. */
    org(horiz, rightvertex);
    dest(horiz, leftvertex);
    apex(horiz, botvertex);
    setorg(newbotright, botvertex);
    setdest(newbotright, rightvertex);
    setapex(newbotright, newvertex);
    setorg(horiz, newvertex);

    /* Interpolate attributes of new vertex. */
    if (attribs > 0 && m->nextras > 0) {
      /* TODO: vertex attribute interpolation */
      /*   The new vertex lies on the segment (rightvertex, leftvertex), */
      /*   so we could use line interpolation. */
      interpolate(newvertex, rightvertex, leftvertex, botvertex, m->nextras);
    }

    for (i = 0; i < m->eextras; i++) {
      /* Set the element attributes of a new triangle. */
      setelemattribute(newbotright, i, elemattribute(botright, i));
    }
    if (b->vararea) {
      /* Set the area constraint of a new triangle. */
      setareabound(newbotright, areabound(botright));
    }
    if (mirrorflag) {
      dest(topright, topvertex);
      setorg(newtopright, rightvertex);
      setdest(newtopright, topvertex);
      setapex(newtopright, newvertex);
      setorg(topright, newvertex);
      for (i = 0; i < m->eextras; i++) {
        /* Set the element attributes of another new triangle. */
        setelemattribute(newtopright, i, elemattribute(topright, i));
      }
      if (b->vararea) {
        /* Set the area constraint of another new triangle. */
        setareabound(newtopright, areabound(topright));
      }
    }

    /* There may be subsegments that need to be bonded */
    /*   to the new triangle(s).                       */
    if (m->checksegments) {
      tspivot(botright, botrsubseg);
      if (botrsubseg.ss != m->dummysub) {
        tsdissolve(botright);
        tsbond(newbotright, botrsubseg);
      }
      if (mirrorflag) {
        tspivot(topright, toprsubseg);
        if (toprsubseg.ss != m->dummysub) {
          tsdissolve(topright);
          tsbond(newtopright, toprsubseg);
        }
      }
    }

    /* Bond the new triangle(s) to the surrounding triangles. */
    bond(newbotright, botrcasing);
    lprevself(newbotright);
    bond(newbotright, botright);
    lprevself(newbotright);
    if (mirrorflag) {
      bond(newtopright, toprcasing);
      lnextself(newtopright);
      bond(newtopright, topright);
      lnextself(newtopright);
      bond(newtopright, newbotright);
    }

    if (splitseg != (struct osub *) NULL) {
      /* Split the subsegment into two. */
      setsdest(*splitseg, newvertex);
      segorg(*splitseg, segmentorg);
      segdest(*splitseg, segmentdest);
      ssymself(*splitseg);
      spivot(*splitseg, rightsubseg);
      insertsubseg(m, b, &newbotright, mark(*splitseg));
      tspivot(newbotright, newsubseg);
      setsegorg(newsubseg, segmentorg);
      setsegdest(newsubseg, segmentdest);
      sbond(*splitseg, newsubseg);
      ssymself(newsubseg);
      sbond(newsubseg, rightsubseg);
      ssymself(*splitseg);
      /* Transfer the subsegment's boundary marker to the vertex */
      /*   if required.                                          */
      if (vertexmark(newvertex) == 0) {
        setvertexmark(newvertex, mark(*splitseg));
      }
    }

    if (m->checkquality) {
      poolrestart(&m->flipstackers);
      m->lastflip = (struct flipstacker *) poolalloc(&m->flipstackers);
      m->lastflip->flippedtri = encode(horiz);
      m->lastflip->prevflip = (struct flipstacker *) &insertvertex;
    }

#ifdef SELF_CHECK
    if (counterclockwise(m, b, rightvertex, leftvertex, botvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf(
            "  Clockwise triangle prior to edge vertex insertion (bottom).\n");
    }
    if (mirrorflag) {
      if (counterclockwise(m, b, leftvertex, rightvertex, topvertex) < 0.0) {
        printf("Internal error in insertvertex():\n");
        printf("  Clockwise triangle prior to edge vertex insertion (top).\n");
      }
      if (counterclockwise(m, b, rightvertex, topvertex, newvertex) < 0.0) {
        printf("Internal error in insertvertex():\n");
        printf(
            "  Clockwise triangle after edge vertex insertion (top right).\n");
      }
      if (counterclockwise(m, b, topvertex, leftvertex, newvertex) < 0.0) {
        printf("Internal error in insertvertex():\n");
        printf(
            "  Clockwise triangle after edge vertex insertion (top left).\n");
      }
    }
    if (counterclockwise(m, b, leftvertex, botvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf(
          "  Clockwise triangle after edge vertex insertion (bottom left).\n");
    }
    if (counterclockwise(m, b, botvertex, rightvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf(
        "  Clockwise triangle after edge vertex insertion (bottom right).\n");
    }
#endif /* SELF_CHECK */

    /* Position `horiz' on the first edge to check for */
    /*   the Delaunay property.                        */
    lnextself(horiz);
  } else {
    /* Insert the vertex in a triangle, splitting it into three. */
    lnext(horiz, botleft);
    lprev(horiz, botright);
    sym(botleft, botlcasing);
    sym(botright, botrcasing);
    maketriangle(m, b, &newbotleft);
    maketriangle(m, b, &newbotright);

    /* Set the vertices of changed and new triangles. */
    org(horiz, rightvertex);
    dest(horiz, leftvertex);
    apex(horiz, botvertex);

    /* Interpolate attributes of new vertex. */
    if (attribs > 0 && m->nextras > 0) {
      interpolate(newvertex, rightvertex, leftvertex, botvertex, m->nextras);
    }

    setorg(newbotleft, leftvertex);
    setdest(newbotleft, botvertex);
    setapex(newbotleft, newvertex);
    setorg(newbotright, botvertex);
    setdest(newbotright, rightvertex);
    setapex(newbotright, newvertex);
    setapex(horiz, newvertex);
    for (i = 0; i < m->eextras; i++) {
      /* Set the element attributes of the new triangles. */
      attrib = elemattribute(horiz, i);
      setelemattribute(newbotleft, i, attrib);
      setelemattribute(newbotright, i, attrib);
    }
    if (b->vararea) {
      /* Set the area constraint of the new triangles. */
      area = areabound(horiz);
      setareabound(newbotleft, area);
      setareabound(newbotright, area);
    }

    /* There may be subsegments that need to be bonded */
    /*   to the new triangles.                         */
    if (m->checksegments) {
      tspivot(botleft, botlsubseg);
      if (botlsubseg.ss != m->dummysub) {
        tsdissolve(botleft);
        tsbond(newbotleft, botlsubseg);
      }
      tspivot(botright, botrsubseg);
      if (botrsubseg.ss != m->dummysub) {
        tsdissolve(botright);
        tsbond(newbotright, botrsubseg);
      }
    }

    /* Bond the new triangles to the surrounding triangles. */
    bond(newbotleft, botlcasing);
    bond(newbotright, botrcasing);
    lnextself(newbotleft);
    lprevself(newbotright);
    bond(newbotleft, newbotright);
    lnextself(newbotleft);
    bond(botleft, newbotleft);
    lprevself(newbotright);
    bond(botright, newbotright);

    if (m->checkquality) {
      poolrestart(&m->flipstackers);
      m->lastflip = (struct flipstacker *) poolalloc(&m->flipstackers);
      m->lastflip->flippedtri = encode(horiz);
      m->lastflip->prevflip = (struct flipstacker *) NULL;
    }

#ifdef SELF_CHECK
    if (counterclockwise(m, b, rightvertex, leftvertex, botvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise triangle prior to vertex insertion.\n");
    }
    if (counterclockwise(m, b, rightvertex, leftvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise triangle after vertex insertion (top).\n");
    }
    if (counterclockwise(m, b, leftvertex, botvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise triangle after vertex insertion (left).\n");
    }
    if (counterclockwise(m, b, botvertex, rightvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise triangle after vertex insertion (right).\n");
    }
#endif /* SELF_CHECK */
  }

  /* The insertion is successful by default, unless an encroached */
  /*   subsegment is found.                                       */
  success = SUCCESSFULVERTEX;
  /* Circle around the newly inserted vertex, checking each edge opposite */
  /*   it for the Delaunay property.  Non-Delaunay edges are flipped.     */
  /*   `horiz' is always the edge being checked.  `first' marks where to  */
  /*   stop circling.                                                     */
  org(horiz, first);
  rightvertex = first;
  dest(horiz, leftvertex);
  /* Circle until finished. */
  while (1) {
    /* By default, the edge will be flipped. */
    doflip = 1;

    if (m->checksegments) {
      /* Check for a subsegment, which cannot be flipped. */
      tspivot(horiz, checksubseg);
      if (checksubseg.ss != m->dummysub) {
        /* The edge is a subsegment and cannot be flipped. */
        doflip = 0;
#ifndef CDT_ONLY
        if (segmentflaws) {
          /* Does the new vertex encroach upon this subsegment? */
          if (checkseg4encroach(m, b, &checksubseg)) {
            success = ENCROACHINGVERTEX;
          }
        }
#endif /* not CDT_ONLY */
      }
    }

    if (doflip) {
      /* Check if the edge is a boundary edge. */
      sym(horiz, top);
      if (top.tri == m->dummytri) {
        /* The edge is a boundary edge and cannot be flipped. */
        doflip = 0;
      } else {
        /* Find the vertex on the other side of the edge. */
        apex(top, farvertex);
        /* In the incremental Delaunay triangulation algorithm, any of      */
        /*   `leftvertex', `rightvertex', and `farvertex' could be vertices */
        /*   of the triangular bounding box.  These vertices must be        */
        /*   treated as if they are infinitely distant, even though their   */
        /*   "coordinates" are not.                                         */
        if ((leftvertex == m->infvertex1) || (leftvertex == m->infvertex2) ||
            (leftvertex == m->infvertex3)) {
          /* `leftvertex' is infinitely distant.  Check the convexity of  */
          /*   the boundary of the triangulation.  'farvertex' might be   */
          /*   infinite as well, but trust me, this same condition should */
          /*   be applied.                                                */
          doflip = counterclockwise(m, b, newvertex, rightvertex, farvertex)
                   > 0.0;
        } else if ((rightvertex == m->infvertex1) ||
                   (rightvertex == m->infvertex2) ||
                   (rightvertex == m->infvertex3)) {
          /* `rightvertex' is infinitely distant.  Check the convexity of */
          /*   the boundary of the triangulation.  'farvertex' might be   */
          /*   infinite as well, but trust me, this same condition should */
          /*   be applied.                                                */
          doflip = counterclockwise(m, b, farvertex, leftvertex, newvertex)
                   > 0.0;
        } else if ((farvertex == m->infvertex1) ||
                   (farvertex == m->infvertex2) ||
                   (farvertex == m->infvertex3)) {
          /* `farvertex' is infinitely distant and cannot be inside */
          /*   the circumcircle of the triangle `horiz'.            */
          doflip = 0;
        } else {
          /* Test whether the edge is locally Delaunay. */
          doflip = incircle(m, b, leftvertex, newvertex, rightvertex,
                            farvertex) > 0.0;
        }
        if (doflip) {
          /* We made it!  Flip the edge `horiz' by rotating its containing */
          /*   quadrilateral (the two triangles adjacent to `horiz').      */
          /* Identify the casing of the quadrilateral. */
          lprev(top, topleft);
          sym(topleft, toplcasing);
          lnext(top, topright);
          sym(topright, toprcasing);
          lnext(horiz, botleft);
          sym(botleft, botlcasing);
          lprev(horiz, botright);
          sym(botright, botrcasing);
          /* Rotate the quadrilateral one-quarter turn counterclockwise. */
          bond(topleft, botlcasing);
          bond(botleft, botrcasing);
          bond(botright, toprcasing);
          bond(topright, toplcasing);
          if (m->checksegments) {
            /* Check for subsegments and rebond them to the quadrilateral. */
            tspivot(topleft, toplsubseg);
            tspivot(botleft, botlsubseg);
            tspivot(botright, botrsubseg);
            tspivot(topright, toprsubseg);
            if (toplsubseg.ss == m->dummysub) {
              tsdissolve(topright);
            } else {
              tsbond(topright, toplsubseg);
            }
            if (botlsubseg.ss == m->dummysub) {
              tsdissolve(topleft);
            } else {
              tsbond(topleft, botlsubseg);
            }
            if (botrsubseg.ss == m->dummysub) {
              tsdissolve(botleft);
            } else {
              tsbond(botleft, botrsubseg);
            }
            if (toprsubseg.ss == m->dummysub) {
              tsdissolve(botright);
            } else {
              tsbond(botright, toprsubseg);
            }
          }
          /* New vertex assignments for the rotated quadrilateral. */
          setorg(horiz, farvertex);
          setdest(horiz, newvertex);
          setapex(horiz, rightvertex);
          setorg(top, newvertex);
          setdest(top, farvertex);
          setapex(top, leftvertex);
          for (i = 0; i < m->eextras; i++) {
            /* Take the average of the two triangles' attributes. */
            attrib = 0.5 * (elemattribute(top, i) + elemattribute(horiz, i));
            setelemattribute(top, i, attrib);
            setelemattribute(horiz, i, attrib);
          }
          if (b->vararea) {
            if ((areabound(top) <= 0.0) || (areabound(horiz) <= 0.0)) {
              area = -1.0;
            } else {
              /* Take the average of the two triangles' area constraints.    */
              /*   This prevents small area constraints from migrating a     */
              /*   long, long way from their original location due to flips. */
              area = 0.5 * (areabound(top) + areabound(horiz));
            }
            setareabound(top, area);
            setareabound(horiz, area);
          }

          if (m->checkquality) {
            newflip = (struct flipstacker *) poolalloc(&m->flipstackers);
            newflip->flippedtri = encode(horiz);
            newflip->prevflip = m->lastflip;
            m->lastflip = newflip;
          }

#ifdef SELF_CHECK
          if (newvertex != (vertex) NULL) {
            if (counterclockwise(m, b, leftvertex, newvertex, rightvertex) <
                0.0) {
              printf("Internal error in insertvertex():\n");
              printf("  Clockwise triangle prior to edge flip (bottom).\n");
            }
            /* The following test has been removed because constrainededge() */
            /*   sometimes generates inverted triangles that insertvertex()  */
            /*   removes.                                                    */
/*
            if (counterclockwise(m, b, rightvertex, farvertex, leftvertex) <
                0.0) {
              printf("Internal error in insertvertex():\n");
              printf("  Clockwise triangle prior to edge flip (top).\n");
            }
*/
            if (counterclockwise(m, b, farvertex, leftvertex, newvertex) <
                0.0) {
              printf("Internal error in insertvertex():\n");
              printf("  Clockwise triangle after edge flip (left).\n");
            }
            if (counterclockwise(m, b, newvertex, rightvertex, farvertex) <
                0.0) {
              printf("Internal error in insertvertex():\n");
              printf("  Clockwise triangle after edge flip (right).\n");
            }
          }
#endif /* SELF_CHECK */
          /* On the next iterations, consider the two edges that were  */
          /*   exposed (this is, are now visible to the newly inserted */
          /*   vertex) by the edge flip.                               */
          lprevself(horiz);
          leftvertex = farvertex;
        }
      }
    }
    if (!doflip) {
      /* The handle `horiz' is accepted as locally Delaunay. */
#ifndef CDT_ONLY
      if (triflaws) {
        /* Check the triangle `horiz' for quality. */
        testtriangle(m, b, &horiz);
      }
#endif /* not CDT_ONLY */
      /* Look for the next edge around the newly inserted vertex. */
      lnextself(horiz);
      sym(horiz, testtri);
      /* Check for finishing a complete revolution about the new vertex, or */
      /*   falling outside  of the triangulation.  The latter will happen   */
      /*   when a vertex is inserted at a boundary.                         */
      if ((leftvertex == first) || (testtri.tri == m->dummytri)) {
        /* We're done.  Return a triangle whose origin is the new vertex. */
        lnext(horiz, *searchtri);
        lnext(horiz, m->recenttri);
        return success;
      }
      /* Finish finding the next edge around the newly inserted vertex. */
      lnext(testtri, horiz);
      rightvertex = leftvertex;
      dest(horiz, leftvertex);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  triangulatepolygon()   Find the Delaunay triangulation of a polygon that */
/*                         has a certain "nice" shape.  This includes the    */
/*                         polygons that result from deletion of a vertex or */
/*                         insertion of a segment.                           */
/*                                                                           */
/*  This is a conceptually difficult routine.  The starting assumption is    */
/*  that we have a polygon with n sides.  n - 1 of these sides are currently */
/*  represented as edges in the mesh.  One side, called the "base", need not */
/*  be.                                                                      */
/*                                                                           */
/*  Inside the polygon is a structure I call a "fan", consisting of n - 1    */
/*  triangles that share a common origin.  For each of these triangles, the  */
/*  edge opposite the origin is one of the sides of the polygon.  The        */
/*  primary edge of each triangle is the edge directed from the origin to    */
/*  the destination; note that this is not the same edge that is a side of   */
/*  the polygon.  `firstedge' is the primary edge of the first triangle.     */
/*  From there, the triangles follow in counterclockwise order about the     */
/*  polygon, until `lastedge', the primary edge of the last triangle.        */
/*  `firstedge' and `lastedge' are probably connected to other triangles     */
/*  beyond the extremes of the fan, but their identity is not important, as  */
/*  long as the fan remains connected to them.                               */
/*                                                                           */
/*  Imagine the polygon oriented so that its base is at the bottom.  This    */
/*  puts `firstedge' on the far right, and `lastedge' on the far left.       */
/*  The right vertex of the base is the destination of `firstedge', and the  */
/*  left vertex of the base is the apex of `lastedge'.                       */
/*                                                                           */
/*  The challenge now is to find the right sequence of edge flips to         */
/*  transform the fan into a Delaunay triangulation of the polygon.  Each    */
/*  edge flip effectively removes one triangle from the fan, committing it   */
/*  to the polygon.  The resulting polygon has one fewer edge.  If `doflip'  */
/*  is set, the final flip will be performed, resulting in a fan of one      */
/*  (useless?) triangle.  If `doflip' is not set, the final flip is not      */
/*  performed, resulting in a fan of two triangles, and an unfinished        */
/*  triangular polygon that is not yet filled out with a single triangle.    */
/*  On completion of the routine, `lastedge' is the last remaining triangle, */
/*  or the leftmost of the last two.                                         */
/*                                                                           */
/*  Although the flips are performed in the order described above, the       */
/*  decisions about what flips to perform are made in precisely the reverse  */
/*  order.  The recursive triangulatepolygon() procedure makes a decision,   */
/*  uses up to two recursive calls to triangulate the "subproblems"          */
/*  (polygons with fewer edges), and then performs an edge flip.             */
/*                                                                           */
/*  The "decision" it makes is which vertex of the polygon should be         */
/*  connected to the base.  This decision is made by testing every possible  */
/*  vertex.  Once the best vertex is found, the two edges that connect this  */
/*  vertex to the base become the bases for two smaller polygons.  These     */
/*  are triangulated recursively.  Unfortunately, this approach can take     */
/*  O(n^2) time not only in the worst case, but in many common cases.  It's  */
/*  rarely a big deal for vertex deletion, where n is rarely larger than     */
/*  ten, but it could be a big deal for segment insertion, especially if     */
/*  there's a lot of long segments that each cut many triangles.  I ought to */
/*  code a faster algorithm some day.                                        */
/*                                                                           */
/*  The `edgecount' parameter is the number of sides of the polygon,         */
/*  including its base.  `triflaws' is a flag that determines whether the    */
/*  new triangles should be tested for quality, and enqueued if they are     */
/*  bad.                                                                     */
/*                                                                           */
/*****************************************************************************/

void triangulatepolygon(mesh *m, behavior *b,
                        struct otri *firstedge, struct otri *lastedge,
                        int edgecount, int doflip, int triflaws)
{
  struct otri testtri;
  struct otri besttri;
  struct otri tempedge;
  vertex leftbasevertex, rightbasevertex;
  vertex testvertex;
  vertex bestvertex;
  int bestnumber;
  int i;
  triangle ptr;   /* Temporary variable used by sym(), onext(), and oprev(). */

  /* Identify the base vertices. */
  apex(*lastedge, leftbasevertex);
  dest(*firstedge, rightbasevertex);

  /* Find the best vertex to connect the base to. */
  onext(*firstedge, besttri);
  dest(besttri, bestvertex);
  otricopy(besttri, testtri);
  bestnumber = 1;
  for (i = 2; i <= edgecount - 2; i++) {
    onextself(testtri);
    dest(testtri, testvertex);
    /* Is this a better vertex? */
    if (incircle(m, b, leftbasevertex, rightbasevertex, bestvertex,
                 testvertex) > 0.0) {
      otricopy(testtri, besttri);
      bestvertex = testvertex;
      bestnumber = i;
    }
  }
  if (bestnumber > 1) {
    /* Recursively triangulate the smaller polygon on the right. */
    oprev(besttri, tempedge);
    triangulatepolygon(m, b, firstedge, &tempedge, bestnumber + 1, 1,
                       triflaws);
  }
  if (bestnumber < edgecount - 2) {
    /* Recursively triangulate the smaller polygon on the left. */
    sym(besttri, tempedge);
    triangulatepolygon(m, b, &besttri, lastedge, edgecount - bestnumber, 1,
                       triflaws);
    /* Find `besttri' again; it may have been lost to edge flips. */
    sym(tempedge, besttri);
  }
  if (doflip) {
    /* Do one final edge flip. */
    flip(m, b, &besttri);
#ifndef CDT_ONLY
    if (triflaws) {
      /* Check the quality of the newly committed triangle. */
      sym(besttri, testtri);
      testtriangle(m, b, &testtri);
    }
#endif /* not CDT_ONLY */
  }
  /* Return the base triangle. */
  otricopy(besttri, *lastedge);
}

/*****************************************************************************/
/*                                                                           */
/*  deletevertex()   Delete a vertex from a Delaunay triangulation, ensuring */
/*                   that the triangulation remains Delaunay.                */
/*                                                                           */
/*  The origin of `deltri' is deleted.  The union of the triangles adjacent  */
/*  to this vertex is a polygon, for which the Delaunay triangulation is     */
/*  found.  Two triangles are removed from the mesh.                         */
/*                                                                           */
/*  Only interior vertices that do not lie on segments or boundaries may be  */
/*  deleted.                                                                 */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void deletevertex(mesh *m, behavior *b, struct otri *deltri)
{
  struct otri countingtri;
  struct otri firstedge, lastedge;
  struct otri deltriright;
  struct otri lefttri, righttri;
  struct otri leftcasing, rightcasing;
  struct osub leftsubseg, rightsubseg;
  vertex delvertex;
  vertex neworg;
  int edgecount;
  triangle ptr;   /* Temporary variable used by sym(), onext(), and oprev(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  org(*deltri, delvertex);
  vertexdealloc(m, delvertex);

  /* Count the degree of the vertex being deleted. */
  onext(*deltri, countingtri);
  edgecount = 1;
  while (!otriequal(*deltri, countingtri)) {
#ifdef SELF_CHECK
    if (countingtri.tri == m->dummytri) {
      printf("Internal error in deletevertex():\n");
      printf("  Attempt to delete boundary vertex.\n");
      internalerror();
    }
#endif /* SELF_CHECK */
    edgecount++;
    onextself(countingtri);
  }

#ifdef SELF_CHECK
  if (edgecount < 3) {
    printf("Internal error in deletevertex():\n  Vertex has degree %d.\n",
           edgecount);
    internalerror();
  }
#endif /* SELF_CHECK */
  if (edgecount > 3) {
    /* Triangulate the polygon defined by the union of all triangles */
    /*   adjacent to the vertex being deleted.  Check the quality of */
    /*   the resulting triangles.                                    */
    onext(*deltri, firstedge);
    oprev(*deltri, lastedge);
    triangulatepolygon(m, b, &firstedge, &lastedge, edgecount, 0,
                       !b->nobisect);
  }
  /* Splice out two triangles. */
  lprev(*deltri, deltriright);
  dnext(*deltri, lefttri);
  sym(lefttri, leftcasing);
  oprev(deltriright, righttri);
  sym(righttri, rightcasing);
  bond(*deltri, leftcasing);
  bond(deltriright, rightcasing);
  tspivot(lefttri, leftsubseg);
  if (leftsubseg.ss != m->dummysub) {
    tsbond(*deltri, leftsubseg);
  }
  tspivot(righttri, rightsubseg);
  if (rightsubseg.ss != m->dummysub) {
    tsbond(deltriright, rightsubseg);
  }

  /* Set the new origin of `deltri' and check its quality. */
  org(lefttri, neworg);
  setorg(*deltri, neworg);
  if (!b->nobisect) {
    testtriangle(m, b, deltri);
  }

  /* Delete the two spliced-out triangles. */
  triangledealloc(m, lefttri.tri);
  triangledealloc(m, righttri.tri);
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  undovertex()   Undo the most recent vertex insertion.                    */
/*                                                                           */
/*  Walks through the list of transformations (flips and a vertex insertion) */
/*  in the reverse of the order in which they were done, and undoes them.    */
/*  The inserted vertex is removed from the triangulation and deallocated.   */
/*  Two triangles (possibly just one) are also deallocated.                  */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void undovertex(mesh *m, behavior *b)
{
  struct otri fliptri;
  struct otri botleft, botright, topright;
  struct otri botlcasing, botrcasing, toprcasing;
  struct otri gluetri;
  struct osub botlsubseg, botrsubseg, toprsubseg;
  vertex botvertex, rightvertex;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Walk through the list of transformations (flips and a vertex insertion) */
  /*   in the reverse of the order in which they were done, and undo them.   */
  while (m->lastflip != (struct flipstacker *) NULL) {
    /* Find a triangle involved in the last unreversed transformation. */
    decode(m->lastflip->flippedtri, fliptri);

    /* We are reversing one of three transformations:  a trisection of one */
    /*   triangle into three (by inserting a vertex in the triangle), a    */
    /*   bisection of two triangles into four (by inserting a vertex in an */
    /*   edge), or an edge flip.                                           */
    if (m->lastflip->prevflip == (struct flipstacker *) NULL) {
      /* Restore a triangle that was split into three triangles, */
      /*   so it is again one triangle.                          */
      dprev(fliptri, botleft);
      lnextself(botleft);
      onext(fliptri, botright);
      lprevself(botright);
      sym(botleft, botlcasing);
      sym(botright, botrcasing);
      dest(botleft, botvertex);

      setapex(fliptri, botvertex);
      lnextself(fliptri);
      bond(fliptri, botlcasing);
      tspivot(botleft, botlsubseg);
      tsbond(fliptri, botlsubseg);
      lnextself(fliptri);
      bond(fliptri, botrcasing);
      tspivot(botright, botrsubseg);
      tsbond(fliptri, botrsubseg);

      /* Delete the two spliced-out triangles. */
      triangledealloc(m, botleft.tri);
      triangledealloc(m, botright.tri);
    } else if (m->lastflip->prevflip == (struct flipstacker *) &insertvertex) {
      /* Restore two triangles that were split into four triangles, */
      /*   so they are again two triangles.                         */
      lprev(fliptri, gluetri);
      sym(gluetri, botright);
      lnextself(botright);
      sym(botright, botrcasing);
      dest(botright, rightvertex);

      setorg(fliptri, rightvertex);
      bond(gluetri, botrcasing);
      tspivot(botright, botrsubseg);
      tsbond(gluetri, botrsubseg);

      /* Delete the spliced-out triangle. */
      triangledealloc(m, botright.tri);

      sym(fliptri, gluetri);
      if (gluetri.tri != m->dummytri) {
        lnextself(gluetri);
        dnext(gluetri, topright);
        sym(topright, toprcasing);

        setorg(gluetri, rightvertex);
        bond(gluetri, toprcasing);
        tspivot(topright, toprsubseg);
        tsbond(gluetri, toprsubseg);

        /* Delete the spliced-out triangle. */
        triangledealloc(m, topright.tri);
      }

      /* This is the end of the list, sneakily encoded. */
      m->lastflip->prevflip = (struct flipstacker *) NULL;
    } else {
      /* Undo an edge flip. */
      unflip(m, b, &fliptri);
    }

    /* Go on and process the next transformation. */
    m->lastflip = m->lastflip->prevflip;
  }
}

#endif /* not CDT_ONLY */

/**                                                                         **/
/**                                                                         **/
/********* Mesh transformation routines end here                     *********/

/********* Divide-and-conquer Delaunay triangulation begins here     *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  The divide-and-conquer bounding box                                      */
/*                                                                           */
/*  I originally implemented the divide-and-conquer and incremental Delaunay */
/*  triangulations using the edge-based data structure presented by Guibas   */
/*  and Stolfi.  Switching to a triangle-based data structure doubled the    */
/*  speed.  However, I had to think of a few extra tricks to maintain the    */
/*  elegance of the original algorithms.                                     */
/*                                                                           */
/*  The "bounding box" used by my variant of the divide-and-conquer          */
/*  algorithm uses one triangle for each edge of the convex hull of the      */
/*  triangulation.  These bounding triangles all share a common apical       */
/*  vertex, which is represented by NULL and which represents nothing.       */
/*  The bounding triangles are linked in a circular fan about this NULL      */
/*  vertex, and the edges on the convex hull of the triangulation appear     */
/*  opposite the NULL vertex.  You might find it easiest to imagine that     */
/*  the NULL vertex is a point in 3D space behind the center of the          */
/*  triangulation, and that the bounding triangles form a sort of cone.      */
/*                                                                           */
/*  This bounding box makes it easy to represent degenerate cases.  For      */
/*  instance, the triangulation of two vertices is a single edge.  This edge */
/*  is represented by two bounding box triangles, one on each "side" of the  */
/*  edge.  These triangles are also linked together in a fan about the NULL  */
/*  vertex.                                                                  */
/*                                                                           */
/*  The bounding box also makes it easy to traverse the convex hull, as the  */
/*  divide-and-conquer algorithm needs to do.                                */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  vertexsort()   Sort an array of vertices by x-coordinate, using the      */
/*                 y-coordinate as a secondary key.                          */
/*                                                                           */
/*  Uses quicksort.  Randomized O(n log n) time.  No, I did not make any of  */
/*  the usual quicksort mistakes.                                            */
/*                                                                           */
/*****************************************************************************/

void vertexsort(vertex *sortarray, int arraysize)
{
  int left, right;
  int pivot;
  REAL pivotx, pivoty;
  vertex temp;

  if (arraysize == 2) {
    /* Recursive base case. */
    if ((sortarray[0][0] > sortarray[1][0]) ||
        ((sortarray[0][0] == sortarray[1][0]) &&
         (sortarray[0][1] > sortarray[1][1]))) {
      temp = sortarray[1];
      sortarray[1] = sortarray[0];
      sortarray[0] = temp;
    }
    return;
  }
  /* Choose a random pivot to split the array. */
  pivot = (int) randomnation((unsigned int) arraysize);
  pivotx = sortarray[pivot][0];
  pivoty = sortarray[pivot][1];
  /* Split the array. */
  left = -1;
  right = arraysize;
  while (left < right) {
    /* Search for a vertex whose x-coordinate is too large for the left. */
    do {
      left++;
    } while ((left <= right) && ((sortarray[left][0] < pivotx) ||
                                 ((sortarray[left][0] == pivotx) &&
                                  (sortarray[left][1] < pivoty))));
    /* Search for a vertex whose x-coordinate is too small for the right. */
    do {
      right--;
    } while ((left <= right) && ((sortarray[right][0] > pivotx) ||
                                 ((sortarray[right][0] == pivotx) &&
                                  (sortarray[right][1] > pivoty))));
    if (left < right) {
      /* Swap the left and right vertices. */
      temp = sortarray[left];
      sortarray[left] = sortarray[right];
      sortarray[right] = temp;
    }
  }
  if (left > 1) {
    /* Recursively sort the left subset. */
    vertexsort(sortarray, left);
  }
  if (right < arraysize - 2) {
    /* Recursively sort the right subset. */
    vertexsort(&sortarray[right + 1], arraysize - right - 1);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  vertexmedian()   An order statistic algorithm, almost.  Shuffles an      */
/*                   array of vertices so that the first `median' vertices   */
/*                   occur lexicographically before the remaining vertices.  */
/*                                                                           */
/*  Uses the x-coordinate as the primary key if axis == 0; the y-coordinate  */
/*  if axis == 1.  Very similar to the vertexsort() procedure, but runs in   */
/*  randomized linear time.                                                  */
/*                                                                           */
/*****************************************************************************/

void vertexmedian(vertex *sortarray, int arraysize, int median, int axis)
{
  int left, right;
  int pivot;
  REAL pivot1, pivot2;
  vertex temp;

  if (arraysize == 2) {
    /* Recursive base case. */
    if ((sortarray[0][axis] > sortarray[1][axis]) ||
        ((sortarray[0][axis] == sortarray[1][axis]) &&
         (sortarray[0][1 - axis] > sortarray[1][1 - axis]))) {
      temp = sortarray[1];
      sortarray[1] = sortarray[0];
      sortarray[0] = temp;
    }
    return;
  }
  /* Choose a random pivot to split the array. */
  pivot = (int) randomnation((unsigned int) arraysize);
  pivot1 = sortarray[pivot][axis];
  pivot2 = sortarray[pivot][1 - axis];
  /* Split the array. */
  left = -1;
  right = arraysize;
  while (left < right) {
    /* Search for a vertex whose x-coordinate is too large for the left. */
    do {
      left++;
    } while ((left <= right) && ((sortarray[left][axis] < pivot1) ||
                                 ((sortarray[left][axis] == pivot1) &&
                                  (sortarray[left][1 - axis] < pivot2))));
    /* Search for a vertex whose x-coordinate is too small for the right. */
    do {
      right--;
    } while ((left <= right) && ((sortarray[right][axis] > pivot1) ||
                                 ((sortarray[right][axis] == pivot1) &&
                                  (sortarray[right][1 - axis] > pivot2))));
    if (left < right) {
      /* Swap the left and right vertices. */
      temp = sortarray[left];
      sortarray[left] = sortarray[right];
      sortarray[right] = temp;
    }
  }
  /* Unlike in vertexsort(), at most one of the following */
  /*   conditionals is true.                             */
  if (left > median) {
    /* Recursively shuffle the left subset. */
    vertexmedian(sortarray, left, median, axis);
  }
  if (right < median - 1) {
    /* Recursively shuffle the right subset. */
    vertexmedian(&sortarray[right + 1], arraysize - right - 1,
                 median - right - 1, axis);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  alternateaxes()   Sorts the vertices as appropriate for the divide-and-  */
/*                    conquer algorithm with alternating cuts.               */
/*                                                                           */
/*  Partitions by x-coordinate if axis == 0; by y-coordinate if axis == 1.   */
/*  For the base case, subsets containing only two or three vertices are     */
/*  always sorted by x-coordinate.                                           */
/*                                                                           */
/*****************************************************************************/

void alternateaxes(vertex *sortarray, int arraysize, int axis)
{
  int divider;

  divider = arraysize >> 1;
  if (arraysize <= 3) {
    /* Recursive base case:  subsets of two or three vertices will be    */
    /*   handled specially, and should always be sorted by x-coordinate. */
    axis = 0;
  }
  /* Partition with a horizontal or vertical cut. */
  vertexmedian(sortarray, arraysize, divider, axis);
  /* Recursively partition the subsets with a cross cut. */
  if (arraysize - divider >= 2) {
    if (divider >= 2) {
      alternateaxes(sortarray, divider, 1 - axis);
    }
    alternateaxes(&sortarray[divider], arraysize - divider, 1 - axis);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  mergehulls()   Merge two adjacent Delaunay triangulations into a         */
/*                 single Delaunay triangulation.                            */
/*                                                                           */
/*  This is similar to the algorithm given by Guibas and Stolfi, but uses    */
/*  a triangle-based, rather than edge-based, data structure.                */
/*                                                                           */
/*  The algorithm walks up the gap between the two triangulations, knitting  */
/*  them together.  As they are merged, some of their bounding triangles     */
/*  are converted into real triangles of the triangulation.  The procedure   */
/*  pulls each hull's bounding triangles apart, then knits them together     */
/*  like the teeth of two gears.  The Delaunay property determines, at each  */
/*  step, whether the next "tooth" is a bounding triangle of the left hull   */
/*  or the right.  When a bounding triangle becomes real, its apex is        */
/*  changed from NULL to a real vertex.                                      */
/*                                                                           */
/*  Only two new triangles need to be allocated.  These become new bounding  */
/*  triangles at the top and bottom of the seam.  They are used to connect   */
/*  the remaining bounding triangles (those that have not been converted     */
/*  into real triangles) into a single fan.                                  */
/*                                                                           */
/*  On entry, `farleft' and `innerleft' are bounding triangles of the left   */
/*  triangulation.  The origin of `farleft' is the leftmost vertex, and      */
/*  the destination of `innerleft' is the rightmost vertex of the            */
/*  triangulation.  Similarly, `innerright' and `farright' are bounding      */
/*  triangles of the right triangulation.  The origin of `innerright' and    */
/*  destination of `farright' are the leftmost and rightmost vertices.       */
/*                                                                           */
/*  On completion, the origin of `farleft' is the leftmost vertex of the     */
/*  merged triangulation, and the destination of `farright' is the rightmost */
/*  vertex.                                                                  */
/*                                                                           */
/*****************************************************************************/

void mergehulls(mesh *m, behavior *b, struct otri *farleft,
                struct otri *innerleft, struct otri *innerright,
                struct otri *farright, int axis)
{
  struct otri leftcand, rightcand;
  struct otri baseedge;
  struct otri nextedge;
  struct otri sidecasing, topcasing, outercasing;
  struct otri checkedge;
  vertex innerleftdest;
  vertex innerrightorg;
  vertex innerleftapex, innerrightapex;
  vertex farleftpt, farrightpt;
  vertex farleftapex, farrightapex;
  vertex lowerleft, lowerright;
  vertex upperleft, upperright;
  vertex nextapex;
  vertex checkvertex;
  int changemade;
  int badedge;
  int leftfinished, rightfinished;
  triangle ptr;                         /* Temporary variable used by sym(). */

  dest(*innerleft, innerleftdest);
  apex(*innerleft, innerleftapex);
  org(*innerright, innerrightorg);
  apex(*innerright, innerrightapex);
  /* Special treatment for horizontal cuts. */
  if (b->dwyer && (axis == 1)) {
    org(*farleft, farleftpt);
    apex(*farleft, farleftapex);
    dest(*farright, farrightpt);
    apex(*farright, farrightapex);
    /* The pointers to the extremal vertices are shifted to point to the */
    /*   topmost and bottommost vertex of each hull, rather than the     */
    /*   leftmost and rightmost vertices.                                */
    while (farleftapex[1] < farleftpt[1]) {
      lnextself(*farleft);
      symself(*farleft);
      farleftpt = farleftapex;
      apex(*farleft, farleftapex);
    }
    sym(*innerleft, checkedge);
    apex(checkedge, checkvertex);
    while (checkvertex[1] > innerleftdest[1]) {
      lnext(checkedge, *innerleft);
      innerleftapex = innerleftdest;
      innerleftdest = checkvertex;
      sym(*innerleft, checkedge);
      apex(checkedge, checkvertex);
    }
    while (innerrightapex[1] < innerrightorg[1]) {
      lnextself(*innerright);
      symself(*innerright);
      innerrightorg = innerrightapex;
      apex(*innerright, innerrightapex);
    }
    sym(*farright, checkedge);
    apex(checkedge, checkvertex);
    while (checkvertex[1] > farrightpt[1]) {
      lnext(checkedge, *farright);
      farrightapex = farrightpt;
      farrightpt = checkvertex;
      sym(*farright, checkedge);
      apex(checkedge, checkvertex);
    }
  }
  /* Find a line tangent to and below both hulls. */
  do {
    changemade = 0;
    /* Make innerleftdest the "bottommost" vertex of the left hull. */
    if (counterclockwise(m, b, innerleftdest, innerleftapex, innerrightorg) >
        0.0) {
      lprevself(*innerleft);
      symself(*innerleft);
      innerleftdest = innerleftapex;
      apex(*innerleft, innerleftapex);
      changemade = 1;
    }
    /* Make innerrightorg the "bottommost" vertex of the right hull. */
    if (counterclockwise(m, b, innerrightapex, innerrightorg, innerleftdest) >
        0.0) {
      lnextself(*innerright);
      symself(*innerright);
      innerrightorg = innerrightapex;
      apex(*innerright, innerrightapex);
      changemade = 1;
    }
  } while (changemade);
  /* Find the two candidates to be the next "gear tooth." */
  sym(*innerleft, leftcand);
  sym(*innerright, rightcand);
  /* Create the bottom new bounding triangle. */
  maketriangle(m, b, &baseedge);
  /* Connect it to the bounding boxes of the left and right triangulations. */
  bond(baseedge, *innerleft);
  lnextself(baseedge);
  bond(baseedge, *innerright);
  lnextself(baseedge);
  setorg(baseedge, innerrightorg);
  setdest(baseedge, innerleftdest);
  /* Apex is intentionally left NULL. */

  /* Fix the extreme triangles if necessary. */
  org(*farleft, farleftpt);
  if (innerleftdest == farleftpt) {
    lnext(baseedge, *farleft);
  }
  dest(*farright, farrightpt);
  if (innerrightorg == farrightpt) {
    lprev(baseedge, *farright);
  }
  /* The vertices of the current knitting edge. */
  lowerleft = innerleftdest;
  lowerright = innerrightorg;
  /* The candidate vertices for knitting. */
  apex(leftcand, upperleft);
  apex(rightcand, upperright);
  /* Walk up the gap between the two triangulations, knitting them together. */
  while (1) {
    /* Have we reached the top?  (This isn't quite the right question,       */
    /*   because even though the left triangulation might seem finished now, */
    /*   moving up on the right triangulation might reveal a new vertex of   */
    /*   the left triangulation.  And vice-versa.)                           */
    leftfinished = counterclockwise(m, b, upperleft, lowerleft, lowerright) <=
                   0.0;
    rightfinished = counterclockwise(m, b, upperright, lowerleft, lowerright)
                 <= 0.0;
    if (leftfinished && rightfinished) {
      /* Create the top new bounding triangle. */
      maketriangle(m, b, &nextedge);
      setorg(nextedge, lowerleft);
      setdest(nextedge, lowerright);
      /* Apex is intentionally left NULL. */
      /* Connect it to the bounding boxes of the two triangulations. */
      bond(nextedge, baseedge);
      lnextself(nextedge);
      bond(nextedge, rightcand);
      lnextself(nextedge);
      bond(nextedge, leftcand);
      /* Special treatment for horizontal cuts. */
      if (b->dwyer && (axis == 1)) {
        org(*farleft, farleftpt);
        apex(*farleft, farleftapex);
        dest(*farright, farrightpt);
        apex(*farright, farrightapex);
        sym(*farleft, checkedge);
        apex(checkedge, checkvertex);
        /* The pointers to the extremal vertices are restored to the  */
        /*   leftmost and rightmost vertices (rather than topmost and */
        /*   bottommost).                                             */
        while (checkvertex[0] < farleftpt[0]) {
          lprev(checkedge, *farleft);
          farleftapex = farleftpt;
          farleftpt = checkvertex;
          sym(*farleft, checkedge);
          apex(checkedge, checkvertex);
        }
        while (farrightapex[0] > farrightpt[0]) {
          lprevself(*farright);
          symself(*farright);
          farrightpt = farrightapex;
          apex(*farright, farrightapex);
        }
      }
      return;
    }
    /* Consider eliminating edges from the left triangulation. */
    if (!leftfinished) {
      /* What vertex would be exposed if an edge were deleted? */
      lprev(leftcand, nextedge);
      symself(nextedge);
      apex(nextedge, nextapex);
      /* If nextapex is NULL, then no vertex would be exposed; the */
      /*   triangulation would have been eaten right through.      */
      if (nextapex != (vertex) NULL) {
        /* Check whether the edge is Delaunay. */
        badedge = incircle(m, b, lowerleft, lowerright, upperleft, nextapex) >
                  0.0;
        while (badedge) {
          /* Eliminate the edge with an edge flip.  As a result, the    */
          /*   left triangulation will have one more boundary triangle. */
          lnextself(nextedge);
          sym(nextedge, topcasing);
          lnextself(nextedge);
          sym(nextedge, sidecasing);
          bond(nextedge, topcasing);
          bond(leftcand, sidecasing);
          lnextself(leftcand);
          sym(leftcand, outercasing);
          lprevself(nextedge);
          bond(nextedge, outercasing);
          /* Correct the vertices to reflect the edge flip. */
          setorg(leftcand, lowerleft);
          setdest(leftcand, NULL);
          setapex(leftcand, nextapex);
          setorg(nextedge, NULL);
          setdest(nextedge, upperleft);
          setapex(nextedge, nextapex);
          /* Consider the newly exposed vertex. */
          upperleft = nextapex;
          /* What vertex would be exposed if another edge were deleted? */
          otricopy(sidecasing, nextedge);
          apex(nextedge, nextapex);
          if (nextapex != (vertex) NULL) {
            /* Check whether the edge is Delaunay. */
            badedge = incircle(m, b, lowerleft, lowerright, upperleft,
                               nextapex) > 0.0;
          } else {
            /* Avoid eating right through the triangulation. */
            badedge = 0;
          }
        }
      }
    }
    /* Consider eliminating edges from the right triangulation. */
    if (!rightfinished) {
      /* What vertex would be exposed if an edge were deleted? */
      lnext(rightcand, nextedge);
      symself(nextedge);
      apex(nextedge, nextapex);
      /* If nextapex is NULL, then no vertex would be exposed; the */
      /*   triangulation would have been eaten right through.      */
      if (nextapex != (vertex) NULL) {
        /* Check whether the edge is Delaunay. */
        badedge = incircle(m, b, lowerleft, lowerright, upperright, nextapex) >
                  0.0;
        while (badedge) {
          /* Eliminate the edge with an edge flip.  As a result, the     */
          /*   right triangulation will have one more boundary triangle. */
          lprevself(nextedge);
          sym(nextedge, topcasing);
          lprevself(nextedge);
          sym(nextedge, sidecasing);
          bond(nextedge, topcasing);
          bond(rightcand, sidecasing);
          lprevself(rightcand);
          sym(rightcand, outercasing);
          lnextself(nextedge);
          bond(nextedge, outercasing);
          /* Correct the vertices to reflect the edge flip. */
          setorg(rightcand, NULL);
          setdest(rightcand, lowerright);
          setapex(rightcand, nextapex);
          setorg(nextedge, upperright);
          setdest(nextedge, NULL);
          setapex(nextedge, nextapex);
          /* Consider the newly exposed vertex. */
          upperright = nextapex;
          /* What vertex would be exposed if another edge were deleted? */
          otricopy(sidecasing, nextedge);
          apex(nextedge, nextapex);
          if (nextapex != (vertex) NULL) {
            /* Check whether the edge is Delaunay. */
            badedge = incircle(m, b, lowerleft, lowerright, upperright,
                               nextapex) > 0.0;
          } else {
            /* Avoid eating right through the triangulation. */
            badedge = 0;
          }
        }
      }
    }
    if (leftfinished || (!rightfinished &&
           (incircle(m, b, upperleft, lowerleft, lowerright, upperright) >
            0.0))) {
      /* Knit the triangulations, adding an edge from `lowerleft' */
      /*   to `upperright'.                                       */
      bond(baseedge, rightcand);
      lprev(rightcand, baseedge);
      setdest(baseedge, lowerleft);
      lowerright = upperright;
      sym(baseedge, rightcand);
      apex(rightcand, upperright);
    } else {
      /* Knit the triangulations, adding an edge from `upperleft' */
      /*   to `lowerright'.                                       */
      bond(baseedge, leftcand);
      lnext(leftcand, baseedge);
      setorg(baseedge, lowerright);
      lowerleft = upperleft;
      sym(baseedge, leftcand);
      apex(leftcand, upperleft);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  divconqrecurse()   Recursively form a Delaunay triangulation by the      */
/*                     divide-and-conquer method.                            */
/*                                                                           */
/*  Recursively breaks down the problem into smaller pieces, which are       */
/*  knitted together by mergehulls().  The base cases (problems of two or    */
/*  three vertices) are handled specially here.                              */
/*                                                                           */
/*  On completion, `farleft' and `farright' are bounding triangles such that */
/*  the origin of `farleft' is the leftmost vertex (breaking ties by         */
/*  choosing the highest leftmost vertex), and the destination of            */
/*  `farright' is the rightmost vertex (breaking ties by choosing the        */
/*  lowest rightmost vertex).                                                */
/*                                                                           */
/*****************************************************************************/

void divconqrecurse(mesh *m, behavior *b, vertex *sortarray,
                    int vertices, int axis,
                    struct otri *farleft, struct otri *farright)
{
  struct otri midtri, tri1, tri2, tri3;
  struct otri innerleft, innerright;
  REAL area;
  int divider;

  if (vertices == 2) {
    /* The triangulation of two vertices is an edge.  An edge is */
    /*   represented by two bounding triangles.                  */
    maketriangle(m, b, farleft);
    setorg(*farleft, sortarray[0]);
    setdest(*farleft, sortarray[1]);
    /* The apex is intentionally left NULL. */
    maketriangle(m, b, farright);
    setorg(*farright, sortarray[1]);
    setdest(*farright, sortarray[0]);
    /* The apex is intentionally left NULL. */
    bond(*farleft, *farright);
    lprevself(*farleft);
    lnextself(*farright);
    bond(*farleft, *farright);
    lprevself(*farleft);
    lnextself(*farright);
    bond(*farleft, *farright);
    /* Ensure that the origin of `farleft' is sortarray[0]. */
    lprev(*farright, *farleft);
    return;
  } else if (vertices == 3) {
    /* The triangulation of three vertices is either a triangle (with */
    /*   three bounding triangles) or two edges (with four bounding   */
    /*   triangles).  In either case, four triangles are created.     */
    maketriangle(m, b, &midtri);
    maketriangle(m, b, &tri1);
    maketriangle(m, b, &tri2);
    maketriangle(m, b, &tri3);
    area = counterclockwise(m, b, sortarray[0], sortarray[1], sortarray[2]);
    if (area == 0.0) {
      /* Three collinear vertices; the triangulation is two edges. */
      setorg(midtri, sortarray[0]);
      setdest(midtri, sortarray[1]);
      setorg(tri1, sortarray[1]);
      setdest(tri1, sortarray[0]);
      setorg(tri2, sortarray[2]);
      setdest(tri2, sortarray[1]);
      setorg(tri3, sortarray[1]);
      setdest(tri3, sortarray[2]);
      /* All apices are intentionally left NULL. */
      bond(midtri, tri1);
      bond(tri2, tri3);
      lnextself(midtri);
      lprevself(tri1);
      lnextself(tri2);
      lprevself(tri3);
      bond(midtri, tri3);
      bond(tri1, tri2);
      lnextself(midtri);
      lprevself(tri1);
      lnextself(tri2);
      lprevself(tri3);
      bond(midtri, tri1);
      bond(tri2, tri3);
      /* Ensure that the origin of `farleft' is sortarray[0]. */
      otricopy(tri1, *farleft);
      /* Ensure that the destination of `farright' is sortarray[2]. */
      otricopy(tri2, *farright);
    } else {
      /* The three vertices are not collinear; the triangulation is one */
      /*   triangle, namely `midtri'.                                   */
      setorg(midtri, sortarray[0]);
      setdest(tri1, sortarray[0]);
      setorg(tri3, sortarray[0]);
      /* Apices of tri1, tri2, and tri3 are left NULL. */
      if (area > 0.0) {
        /* The vertices are in counterclockwise order. */
        setdest(midtri, sortarray[1]);
        setorg(tri1, sortarray[1]);
        setdest(tri2, sortarray[1]);
        setapex(midtri, sortarray[2]);
        setorg(tri2, sortarray[2]);
        setdest(tri3, sortarray[2]);
      } else {
        /* The vertices are in clockwise order. */
        setdest(midtri, sortarray[2]);
        setorg(tri1, sortarray[2]);
        setdest(tri2, sortarray[2]);
        setapex(midtri, sortarray[1]);
        setorg(tri2, sortarray[1]);
        setdest(tri3, sortarray[1]);
      }
      /* The topology does not depend on how the vertices are ordered. */
      bond(midtri, tri1);
      lnextself(midtri);
      bond(midtri, tri2);
      lnextself(midtri);
      bond(midtri, tri3);
      lprevself(tri1);
      lnextself(tri2);
      bond(tri1, tri2);
      lprevself(tri1);
      lprevself(tri3);
      bond(tri1, tri3);
      lnextself(tri2);
      lprevself(tri3);
      bond(tri2, tri3);
      /* Ensure that the origin of `farleft' is sortarray[0]. */
      otricopy(tri1, *farleft);
      /* Ensure that the destination of `farright' is sortarray[2]. */
      if (area > 0.0) {
        otricopy(tri2, *farright);
      } else {
        lnext(*farleft, *farright);
      }
    }
    return;
  } else {
    /* Split the vertices in half. */
    divider = vertices >> 1;
    /* Recursively triangulate each half. */
    divconqrecurse(m, b, sortarray, divider, 1 - axis, farleft, &innerleft);
    divconqrecurse(m, b, &sortarray[divider], vertices - divider, 1 - axis,
                   &innerright, farright);
    /* Merge the two triangulations into one. */
    mergehulls(m, b, farleft, &innerleft, &innerright, farright, axis);
  }
}

long removeghosts(mesh *m, behavior *b, struct otri *startghost)
{
  struct otri searchedge;
  struct otri dissolveedge;
  struct otri deadtriangle;
  vertex markorg;
  long hullsize;
  triangle ptr;                         /* Temporary variable used by sym(). */

  /* Find an edge on the convex hull to start point location from. */
  lprev(*startghost, searchedge);
  symself(searchedge);
  m->dummytri[0] = encode(searchedge);
  /* Remove the bounding box and count the convex hull edges. */
  otricopy(*startghost, dissolveedge);
  hullsize = 0;
  do {
    hullsize++;
    lnext(dissolveedge, deadtriangle);
    lprevself(dissolveedge);
    symself(dissolveedge);
    /* If no PSLG is involved, set the boundary markers of all the vertices */
    /*   on the convex hull.  If a PSLG is used, this step is done later.   */
    if (!b->poly) {
      /* Watch out for the case where all the input vertices are collinear. */
      if (dissolveedge.tri != m->dummytri) {
        org(dissolveedge, markorg);
        if (vertexmark(markorg) == 0) {
          setvertexmark(markorg, 1);
        }
      }
    }
    /* Remove a bounding triangle from a convex hull triangle. */
    dissolve(dissolveedge);
    /* Find the next bounding triangle. */
    sym(deadtriangle, dissolveedge);
    /* Delete the bounding triangle. */
    triangledealloc(m, deadtriangle.tri);
  } while (!otriequal(dissolveedge, *startghost));
  return hullsize;
}

/*****************************************************************************/
/*                                                                           */
/*  divconqdelaunay()   Form a Delaunay triangulation by the divide-and-     */
/*                      conquer method.                                      */
/*                                                                           */
/*  Sorts the vertices, calls a recursive procedure to triangulate them, and */
/*  removes the bounding box, setting boundary markers as appropriate.       */
/*                                                                           */
/*****************************************************************************/

long divconqdelaunay(mesh *m, behavior *b)
{
  vertex *sortarray;
  struct otri hullleft, hullright;
  int divider;
  int i, j;

  /* Allocate an array of pointers to vertices for sorting. */
  sortarray = (vertex *) trimalloc(m->invertices * (int) sizeof(vertex));
  traversalinit(&m->vertices);
  for (i = 0; i < m->invertices; i++) {
    sortarray[i] = vertextraverse(m);
  }
  /* Sort the vertices. */
  vertexsort(sortarray, m->invertices);
  /* Discard duplicate vertices, which can really mess up the algorithm. */
  i = 0;
  for (j = 1; j < m->invertices; j++) {
    if ((sortarray[i][0] == sortarray[j][0])
        && (sortarray[i][1] == sortarray[j][1])) {
#ifdef _DEBUG
        printf(
"Warning:  A duplicate vertex at (%.12g, %.12g) appeared and was ignored.\n",
               sortarray[j][0], sortarray[j][1]);
#endif
      // TODO: warning UNDEADVERTEX
      setvertextype(sortarray[j], UNDEADVERTEX);
      m->undeads++;
    } else {
      i++;
      sortarray[i] = sortarray[j];
    }
  }
  i++;
  if (b->dwyer) {
    /* Re-sort the array of vertices to accommodate alternating cuts. */
    divider = i >> 1;
    if (i - divider >= 2) {
      if (divider >= 2) {
        alternateaxes(sortarray, divider, 1);
      }
      alternateaxes(&sortarray[divider], i - divider, 1);
    }
  }

  /* Form the Delaunay triangulation. */
  divconqrecurse(m, b, sortarray, i, 0, &hullleft, &hullright);
  trifree((VOID *) sortarray);

  return removeghosts(m, b, &hullleft);
}

/**                                                                         **/
/**                                                                         **/
/********* Divide-and-conquer Delaunay triangulation ends here       *********/

/********* Incremental Delaunay triangulation begins here            *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  boundingbox()   Form an "infinite" bounding triangle to insert vertices  */
/*                  into.                                                    */
/*                                                                           */
/*  The vertices at "infinity" are assigned finite coordinates, which are    */
/*  used by the point location routines, but (mostly) ignored by the         */
/*  Delaunay edge flip routines.                                             */
/*                                                                           */
/*****************************************************************************/

#ifndef REDUCED

void boundingbox(mesh *m, behavior *b)
{
  struct otri inftri;          /* Handle for the triangular bounding box. */
  REAL width;

  /* Find the width (or height, whichever is larger) of the triangulation. */
  width = m->xmax - m->xmin;
  if (m->ymax - m->ymin > width) {
    width = m->ymax - m->ymin;
  }
  if (width == 0.0) {
    width = 1.0;
  }
  /* Create the vertices of the bounding box. */
  m->infvertex1 = (vertex) trimalloc(m->vertices.itembytes);
  m->infvertex2 = (vertex) trimalloc(m->vertices.itembytes);
  m->infvertex3 = (vertex) trimalloc(m->vertices.itembytes);
  m->infvertex1[0] = m->xmin - 50.0 * width;
  m->infvertex1[1] = m->ymin - 40.0 * width;
  m->infvertex2[0] = m->xmax + 50.0 * width;
  m->infvertex2[1] = m->ymin - 40.0 * width;
  m->infvertex3[0] = 0.5 * (m->xmin + m->xmax);
  m->infvertex3[1] = m->ymax + 60.0 * width;

  /* Create the bounding box. */
  maketriangle(m, b, &inftri);
  setorg(inftri, m->infvertex1);
  setdest(inftri, m->infvertex2);
  setapex(inftri, m->infvertex3);
  /* Link dummytri to the bounding box so we can always find an */
  /*   edge to begin searching (point location) from.           */
  m->dummytri[0] = (triangle) inftri.tri;
}

#endif /* not REDUCED */

/*****************************************************************************/
/*                                                                           */
/*  removebox()   Remove the "infinite" bounding triangle, setting boundary  */
/*                markers as appropriate.                                    */
/*                                                                           */
/*  The triangular bounding box has three boundary triangles (one for each   */
/*  side of the bounding box), and a bunch of triangles fanning out from     */
/*  the three bounding box vertices (one triangle for each edge of the       */
/*  convex hull of the inner mesh).  This routine removes these triangles.   */
/*                                                                           */
/*  Returns the number of edges on the convex hull of the triangulation.     */
/*                                                                           */
/*****************************************************************************/

#ifndef REDUCED

long removebox(mesh *m, behavior *b)
{
  struct otri deadtriangle;
  struct otri searchedge;
  struct otri checkedge;
  struct otri nextedge, finaledge, dissolveedge;
  vertex markorg;
  long hullsize;
  triangle ptr;                         /* Temporary variable used by sym(). */

  /* Find a boundary triangle. */
  nextedge.tri = m->dummytri;
  nextedge.orient = 0;
  symself(nextedge);
  /* Mark a place to stop. */
  lprev(nextedge, finaledge);
  lnextself(nextedge);
  symself(nextedge);
  /* Find a triangle (on the boundary of the vertex set) that isn't */
  /*   a bounding box triangle.                                     */
  lprev(nextedge, searchedge);
  symself(searchedge);
  /* Check whether nextedge is another boundary triangle */
  /*   adjacent to the first one.                        */
  lnext(nextedge, checkedge);
  symself(checkedge);
  if (checkedge.tri == m->dummytri) {
    /* Go on to the next triangle.  There are only three boundary   */
    /*   triangles, and this next triangle cannot be the third one, */
    /*   so it's safe to stop here.                                 */
    lprevself(searchedge);
    symself(searchedge);
  }
  /* Find a new boundary edge to search from, as the current search */
  /*   edge lies on a bounding box triangle and will be deleted.    */
  m->dummytri[0] = encode(searchedge);
  hullsize = -2l;
  while (!otriequal(nextedge, finaledge)) {
    hullsize++;
    lprev(nextedge, dissolveedge);
    symself(dissolveedge);
    /* If not using a PSLG, the vertices should be marked now. */
    /*   (If using a PSLG, markhull() will do the job.)        */
    if (!b->poly) {
      /* Be careful!  One must check for the case where all the input     */
      /*   vertices are collinear, and thus all the triangles are part of */
      /*   the bounding box.  Otherwise, the setvertexmark() call below   */
      /*   will cause a bad pointer reference.                            */
      if (dissolveedge.tri != m->dummytri) {
        org(dissolveedge, markorg);
        if (vertexmark(markorg) == 0) {
          setvertexmark(markorg, 1);
        }
      }
    }
    /* Disconnect the bounding box triangle from the mesh triangle. */
    dissolve(dissolveedge);
    lnext(nextedge, deadtriangle);
    sym(deadtriangle, nextedge);
    /* Get rid of the bounding box triangle. */
    triangledealloc(m, deadtriangle.tri);
    /* Do we need to turn the corner? */
    if (nextedge.tri == m->dummytri) {
      /* Turn the corner. */
      otricopy(dissolveedge, nextedge);
    }
  }
  triangledealloc(m, finaledge.tri);

  trifree((VOID *) m->infvertex1);  /* Deallocate the bounding box vertices. */
  trifree((VOID *) m->infvertex2);
  trifree((VOID *) m->infvertex3);

  return hullsize;
}

#endif /* not REDUCED */

/*****************************************************************************/
/*                                                                           */
/*  incrementaldelaunay()   Form a Delaunay triangulation by incrementally   */
/*                          inserting vertices.                              */
/*                                                                           */
/*  Returns the number of edges on the convex hull of the triangulation.     */
/*                                                                           */
/*****************************************************************************/

#ifndef REDUCED

long incrementaldelaunay(mesh *m, behavior *b)
{
  struct otri starttri;
  vertex vertexloop;

  /* Create a triangular bounding box. */
  boundingbox(m, b);
  traversalinit(&m->vertices);
  vertexloop = vertextraverse(m);
  while (vertexloop != (vertex) NULL) {
    starttri.tri = m->dummytri;
    if (insertvertex(m, b, vertexloop, &starttri, (struct osub *) NULL, 0, 0, 0)
        == DUPLICATEVERTEX) {
#ifdef _DEBUG
      printf("Warning:  A duplicate vertex at (%.12g, %.12g) appeared and was ignored.\n",
             vertexloop[0], vertexloop[1]);
#endif
      // TODO: warning UNDEADVERTEX
      setvertextype(vertexloop, UNDEADVERTEX);
      m->undeads++;
    }
    vertexloop = vertextraverse(m);
  }
  /* Remove the bounding box. */
  return removebox(m, b);
}

#endif /* not REDUCED */

/**                                                                         **/
/**                                                                         **/
/********* Incremental Delaunay triangulation ends here              *********/

/********* Sweepline Delaunay triangulation begins here              *********/
/**                                                                         **/
/**                                                                         **/

#ifndef REDUCED

void eventheapinsert(struct event **heap, int heapsize, struct event *newevent)
{
  REAL eventx, eventy;
  int eventnum;
  int parent;
  int notdone;

  eventx = newevent->xkey;
  eventy = newevent->ykey;
  eventnum = heapsize;
  notdone = eventnum > 0;
  while (notdone) {
    parent = (eventnum - 1) >> 1;
    if ((heap[parent]->ykey < eventy) ||
        ((heap[parent]->ykey == eventy)
         && (heap[parent]->xkey <= eventx))) {
      notdone = 0;
    } else {
      heap[eventnum] = heap[parent];
      heap[eventnum]->heapposition = eventnum;

      eventnum = parent;
      notdone = eventnum > 0;
    }
  }
  heap[eventnum] = newevent;
  newevent->heapposition = eventnum;
}

#endif /* not REDUCED */

#ifndef REDUCED

void eventheapify(struct event **heap, int heapsize, int eventnum)
{
  struct event *thisevent;
  REAL eventx, eventy;
  int leftchild, rightchild;
  int smallest;
  int notdone;

  thisevent = heap[eventnum];
  eventx = thisevent->xkey;
  eventy = thisevent->ykey;
  leftchild = 2 * eventnum + 1;
  notdone = leftchild < heapsize;
  while (notdone) {
    if ((heap[leftchild]->ykey < eventy) ||
        ((heap[leftchild]->ykey == eventy)
         && (heap[leftchild]->xkey < eventx))) {
      smallest = leftchild;
    } else {
      smallest = eventnum;
    }
    rightchild = leftchild + 1;
    if (rightchild < heapsize) {
      if ((heap[rightchild]->ykey < heap[smallest]->ykey) ||
          ((heap[rightchild]->ykey == heap[smallest]->ykey)
           && (heap[rightchild]->xkey < heap[smallest]->xkey))) {
        smallest = rightchild;
      }
    }
    if (smallest == eventnum) {
      notdone = 0;
    } else {
      heap[eventnum] = heap[smallest];
      heap[eventnum]->heapposition = eventnum;
      heap[smallest] = thisevent;
      thisevent->heapposition = smallest;

      eventnum = smallest;
      leftchild = 2 * eventnum + 1;
      notdone = leftchild < heapsize;
    }
  }
}

#endif /* not REDUCED */

#ifndef REDUCED

void eventheapdelete(struct event **heap, int heapsize, int eventnum)
{
  struct event *moveevent;
  REAL eventx, eventy;
  int parent;
  int notdone;

  moveevent = heap[heapsize - 1];
  if (eventnum > 0) {
    eventx = moveevent->xkey;
    eventy = moveevent->ykey;
    do {
      parent = (eventnum - 1) >> 1;
      if ((heap[parent]->ykey < eventy) ||
          ((heap[parent]->ykey == eventy)
           && (heap[parent]->xkey <= eventx))) {
        notdone = 0;
      } else {
        heap[eventnum] = heap[parent];
        heap[eventnum]->heapposition = eventnum;

        eventnum = parent;
        notdone = eventnum > 0;
      }
    } while (notdone);
  }
  heap[eventnum] = moveevent;
  moveevent->heapposition = eventnum;
  eventheapify(heap, heapsize - 1, eventnum);
}

#endif /* not REDUCED */

#ifndef REDUCED

void createeventheap(mesh *m, struct event ***eventheap,
                     struct event **events, struct event **freeevents)
{
  vertex thisvertex;
  int maxevents;
  int i;

  maxevents = (3 * m->invertices) / 2;
  *eventheap = (struct event **) trimalloc(maxevents *
                                           (int) sizeof(struct event *));
  *events = (struct event *) trimalloc(maxevents * (int) sizeof(struct event));
  traversalinit(&m->vertices);
  for (i = 0; i < m->invertices; i++) {
    thisvertex = vertextraverse(m);
    (*events)[i].eventptr = (VOID *) thisvertex;
    (*events)[i].xkey = thisvertex[0];
    (*events)[i].ykey = thisvertex[1];
    eventheapinsert(*eventheap, i, *events + i);
  }
  *freeevents = (struct event *) NULL;
  for (i = maxevents - 1; i >= m->invertices; i--) {
    (*events)[i].eventptr = (VOID *) *freeevents;
    *freeevents = *events + i;
  }
}

#endif /* not REDUCED */

#ifndef REDUCED

int rightofhyperbola(mesh *m, struct otri *fronttri, vertex newsite)
{
  vertex leftvertex, rightvertex;
  REAL dxa, dya, dxb, dyb;

  m->hyperbolacount++;

  dest(*fronttri, leftvertex);
  apex(*fronttri, rightvertex);
  if ((leftvertex[1] < rightvertex[1]) ||
      ((leftvertex[1] == rightvertex[1]) &&
       (leftvertex[0] < rightvertex[0]))) {
    if (newsite[0] >= rightvertex[0]) {
      return 1;
    }
  } else {
    if (newsite[0] <= leftvertex[0]) {
      return 0;
    }
  }
  dxa = leftvertex[0] - newsite[0];
  dya = leftvertex[1] - newsite[1];
  dxb = rightvertex[0] - newsite[0];
  dyb = rightvertex[1] - newsite[1];
  return dya * (dxb * dxb + dyb * dyb) > dyb * (dxa * dxa + dya * dya);
}

#endif /* not REDUCED */

#ifndef REDUCED

REAL circletop(mesh *m, vertex pa, vertex pb, vertex pc, REAL ccwabc)
{
  REAL xac, yac, xbc, ybc, xab, yab;
  REAL aclen2, bclen2, ablen2;

  m->circletopcount++;

  xac = pa[0] - pc[0];
  yac = pa[1] - pc[1];
  xbc = pb[0] - pc[0];
  ybc = pb[1] - pc[1];
  xab = pa[0] - pb[0];
  yab = pa[1] - pb[1];
  aclen2 = xac * xac + yac * yac;
  bclen2 = xbc * xbc + ybc * ybc;
  ablen2 = xab * xab + yab * yab;
  return pc[1] + (xac * bclen2 - xbc * aclen2 + sqrt(aclen2 * bclen2 * ablen2))
               / (2.0 * ccwabc);
}

#endif /* not REDUCED */

#ifndef REDUCED

void check4deadevent(struct otri *checktri, struct event **freeevents,
                     struct event **eventheap, int *heapsize)
{
  struct event *deadevent;
  vertex eventvertex;
  int eventnum;

  org(*checktri, eventvertex);
  if (eventvertex != (vertex) NULL) {
    deadevent = (struct event *) eventvertex;
    eventnum = deadevent->heapposition;
    deadevent->eventptr = (VOID *) *freeevents;
    *freeevents = deadevent;
    eventheapdelete(eventheap, *heapsize, eventnum);
    (*heapsize)--;
    setorg(*checktri, NULL);
  }
}

#endif /* not REDUCED */

#ifndef REDUCED

struct splaynode *splay(mesh *m, struct splaynode *splaytree,
                        vertex searchpoint, struct otri *searchtri)
{
  struct splaynode *child, *grandchild;
  struct splaynode *lefttree, *righttree;
  struct splaynode *leftright;
  vertex checkvertex;
  int rightofroot, rightofchild;

  if (splaytree == (struct splaynode *) NULL) {
    return (struct splaynode *) NULL;
  }
  dest(splaytree->keyedge, checkvertex);
  if (checkvertex == splaytree->keydest) {
    rightofroot = rightofhyperbola(m, &splaytree->keyedge, searchpoint);
    if (rightofroot) {
      otricopy(splaytree->keyedge, *searchtri);
      child = splaytree->rchild;
    } else {
      child = splaytree->lchild;
    }
    if (child == (struct splaynode *) NULL) {
      return splaytree;
    }
    dest(child->keyedge, checkvertex);
    if (checkvertex != child->keydest) {
      child = splay(m, child, searchpoint, searchtri);
      if (child == (struct splaynode *) NULL) {
        if (rightofroot) {
          splaytree->rchild = (struct splaynode *) NULL;
        } else {
          splaytree->lchild = (struct splaynode *) NULL;
        }
        return splaytree;
      }
    }
    rightofchild = rightofhyperbola(m, &child->keyedge, searchpoint);
    if (rightofchild) {
      otricopy(child->keyedge, *searchtri);
      grandchild = splay(m, child->rchild, searchpoint, searchtri);
      child->rchild = grandchild;
    } else {
      grandchild = splay(m, child->lchild, searchpoint, searchtri);
      child->lchild = grandchild;
    }
    if (grandchild == (struct splaynode *) NULL) {
      if (rightofroot) {
        splaytree->rchild = child->lchild;
        child->lchild = splaytree;
      } else {
        splaytree->lchild = child->rchild;
        child->rchild = splaytree;
      }
      return child;
    }
    if (rightofchild) {
      if (rightofroot) {
        splaytree->rchild = child->lchild;
        child->lchild = splaytree;
      } else {
        splaytree->lchild = grandchild->rchild;
        grandchild->rchild = splaytree;
      }
      child->rchild = grandchild->lchild;
      grandchild->lchild = child;
    } else {
      if (rightofroot) {
        splaytree->rchild = grandchild->lchild;
        grandchild->lchild = splaytree;
      } else {
        splaytree->lchild = child->rchild;
        child->rchild = splaytree;
      }
      child->lchild = grandchild->rchild;
      grandchild->rchild = child;
    }
    return grandchild;
  } else {
    lefttree = splay(m, splaytree->lchild, searchpoint, searchtri);
    righttree = splay(m, splaytree->rchild, searchpoint, searchtri);

    pooldealloc(&m->splaynodes, (VOID *) splaytree);
    if (lefttree == (struct splaynode *) NULL) {
      return righttree;
    } else if (righttree == (struct splaynode *) NULL) {
      return lefttree;
    } else if (lefttree->rchild == (struct splaynode *) NULL) {
      lefttree->rchild = righttree->lchild;
      righttree->lchild = lefttree;
      return righttree;
    } else if (righttree->lchild == (struct splaynode *) NULL) {
      righttree->lchild = lefttree->rchild;
      lefttree->rchild = righttree;
      return lefttree;
    } else {
/*      printf("Holy Toledo!!!\n"); */
      leftright = lefttree->rchild;
      while (leftright->rchild != (struct splaynode *) NULL) {
        leftright = leftright->rchild;
      }
      leftright->rchild = righttree;
      return lefttree;
    }
  }
}

#endif /* not REDUCED */

#ifndef REDUCED

struct splaynode *splayinsert(mesh *m, struct splaynode *splayroot,
                              struct otri *newkey, vertex searchpoint)
{
  struct splaynode *newsplaynode;

  newsplaynode = (struct splaynode *) poolalloc(&m->splaynodes);
  otricopy(*newkey, newsplaynode->keyedge);
  dest(*newkey, newsplaynode->keydest);
  if (splayroot == (struct splaynode *) NULL) {
    newsplaynode->lchild = (struct splaynode *) NULL;
    newsplaynode->rchild = (struct splaynode *) NULL;
  } else if (rightofhyperbola(m, &splayroot->keyedge, searchpoint)) {
    newsplaynode->lchild = splayroot;
    newsplaynode->rchild = splayroot->rchild;
    splayroot->rchild = (struct splaynode *) NULL;
  } else {
    newsplaynode->lchild = splayroot->lchild;
    newsplaynode->rchild = splayroot;
    splayroot->lchild = (struct splaynode *) NULL;
  }
  return newsplaynode;
}

#endif /* not REDUCED */

#ifndef REDUCED

struct splaynode *circletopinsert(mesh *m, behavior *b,
                                  struct splaynode *splayroot,
                                  struct otri *newkey,
                                  vertex pa, vertex pb, vertex pc, REAL topy)
{
  REAL ccwabc;
  REAL xac, yac, xbc, ybc;
  REAL aclen2, bclen2;
  REAL searchpoint[2];
  struct otri dummytri;

  ccwabc = counterclockwise(m, b, pa, pb, pc);
  xac = pa[0] - pc[0];
  yac = pa[1] - pc[1];
  xbc = pb[0] - pc[0];
  ybc = pb[1] - pc[1];
  aclen2 = xac * xac + yac * yac;
  bclen2 = xbc * xbc + ybc * ybc;
  searchpoint[0] = pc[0] - (yac * bclen2 - ybc * aclen2) / (2.0 * ccwabc);
  searchpoint[1] = topy;
  return splayinsert(m, splay(m, splayroot, (vertex) searchpoint, &dummytri),
                     newkey, (vertex) searchpoint);
}

#endif /* not REDUCED */

#ifndef REDUCED

struct splaynode *frontlocate(mesh *m, struct splaynode *splayroot,
                              struct otri *bottommost, vertex searchvertex,
                              struct otri *searchtri, int *farright)
{
  int farrightflag;
  triangle ptr;                       /* Temporary variable used by onext(). */

  otricopy(*bottommost, *searchtri);
  splayroot = splay(m, splayroot, searchvertex, searchtri);

  farrightflag = 0;
  while (!farrightflag && rightofhyperbola(m, searchtri, searchvertex)) {
    onextself(*searchtri);
    farrightflag = otriequal(*searchtri, *bottommost);
  }
  *farright = farrightflag;
  return splayroot;
}

#endif /* not REDUCED */

#ifndef REDUCED

long sweeplinedelaunay(mesh *m, behavior *b)
{
  struct event **eventheap;
  struct event *events;
  struct event *freeevents;
  struct event *nextevent;
  struct event *newevent;
  struct splaynode *splayroot;
  struct otri bottommost;
  struct otri searchtri;
  struct otri fliptri;
  struct otri lefttri, righttri, farlefttri, farrighttri;
  struct otri inserttri;
  vertex firstvertex, secondvertex;
  vertex nextvertex, lastvertex;
  vertex connectvertex;
  vertex leftvertex, midvertex, rightvertex;
  REAL lefttest, righttest;
  int heapsize;
  int check4events, farrightflag;
  triangle ptr;   /* Temporary variable used by sym(), onext(), and oprev(). */

  poolinit(&m->splaynodes, sizeof(struct splaynode), SPLAYNODEPERBLOCK,
           SPLAYNODEPERBLOCK, 0);
  splayroot = (struct splaynode *) NULL;

  createeventheap(m, &eventheap, &events, &freeevents);
  heapsize = m->invertices;

  maketriangle(m, b, &lefttri);
  maketriangle(m, b, &righttri);
  bond(lefttri, righttri);
  lnextself(lefttri);
  lprevself(righttri);
  bond(lefttri, righttri);
  lnextself(lefttri);
  lprevself(righttri);
  bond(lefttri, righttri);
  firstvertex = (vertex) eventheap[0]->eventptr;
  eventheap[0]->eventptr = (VOID *) freeevents;
  freeevents = eventheap[0];
  eventheapdelete(eventheap, heapsize, 0);
  heapsize--;
  do {
    if (heapsize == 0) {
      /* Error:  Input vertices are all identical. */
      return TRI_INPUT;
    }
    secondvertex = (vertex) eventheap[0]->eventptr;
    eventheap[0]->eventptr = (VOID *) freeevents;
    freeevents = eventheap[0];
    eventheapdelete(eventheap, heapsize, 0);
    heapsize--;
    if ((firstvertex[0] == secondvertex[0]) &&
        (firstvertex[1] == secondvertex[1])) {
#ifdef _DEBUG
        printf(
"Warning:  A duplicate vertex at (%.12g, %.12g) appeared and was ignored.\n",
               secondvertex[0], secondvertex[1]);
#endif
      // TODO: warning UNDEADVERTEX
      setvertextype(secondvertex, UNDEADVERTEX);
      m->undeads++;
    }
  } while ((firstvertex[0] == secondvertex[0]) &&
           (firstvertex[1] == secondvertex[1]));
  setorg(lefttri, firstvertex);
  setdest(lefttri, secondvertex);
  setorg(righttri, secondvertex);
  setdest(righttri, firstvertex);
  lprev(lefttri, bottommost);
  lastvertex = secondvertex;
  while (heapsize > 0) {
    nextevent = eventheap[0];
    eventheapdelete(eventheap, heapsize, 0);
    heapsize--;
    check4events = 1;
    if (nextevent->xkey < m->xmin) {
      decode(nextevent->eventptr, fliptri);
      oprev(fliptri, farlefttri);
      check4deadevent(&farlefttri, &freeevents, eventheap, &heapsize);
      onext(fliptri, farrighttri);
      check4deadevent(&farrighttri, &freeevents, eventheap, &heapsize);

      if (otriequal(farlefttri, bottommost)) {
        lprev(fliptri, bottommost);
      }
      flip(m, b, &fliptri);
      setapex(fliptri, NULL);
      lprev(fliptri, lefttri);
      lnext(fliptri, righttri);
      sym(lefttri, farlefttri);

      if (randomnation(SAMPLERATE) == 0) {
        symself(fliptri);
        dest(fliptri, leftvertex);
        apex(fliptri, midvertex);
        org(fliptri, rightvertex);
        splayroot = circletopinsert(m, b, splayroot, &lefttri, leftvertex,
                                    midvertex, rightvertex, nextevent->ykey);
      }
    } else {
      nextvertex = (vertex) nextevent->eventptr;
      if ((nextvertex[0] == lastvertex[0]) &&
          (nextvertex[1] == lastvertex[1])) {
#ifdef _DEBUG
          printf(
"Warning:  A duplicate vertex at (%.12g, %.12g) appeared and was ignored.\n",
                 nextvertex[0], nextvertex[1]);
#endif
        // TODO: warning UNDEADVERTEX
        setvertextype(nextvertex, UNDEADVERTEX);
        m->undeads++;
        check4events = 0;
      } else {
        lastvertex = nextvertex;

        splayroot = frontlocate(m, splayroot, &bottommost, nextvertex,
                                &searchtri, &farrightflag);
/*
        otricopy(bottommost, searchtri);
        farrightflag = 0;
        while (!farrightflag && rightofhyperbola(m, &searchtri, nextvertex)) {
          onextself(searchtri);
          farrightflag = otriequal(searchtri, bottommost);
        }
*/

        check4deadevent(&searchtri, &freeevents, eventheap, &heapsize);

        otricopy(searchtri, farrighttri);
        sym(searchtri, farlefttri);
        maketriangle(m, b, &lefttri);
        maketriangle(m, b, &righttri);
        dest(farrighttri, connectvertex);
        setorg(lefttri, connectvertex);
        setdest(lefttri, nextvertex);
        setorg(righttri, nextvertex);
        setdest(righttri, connectvertex);
        bond(lefttri, righttri);
        lnextself(lefttri);
        lprevself(righttri);
        bond(lefttri, righttri);
        lnextself(lefttri);
        lprevself(righttri);
        bond(lefttri, farlefttri);
        bond(righttri, farrighttri);
        if (!farrightflag && otriequal(farrighttri, bottommost)) {
          otricopy(lefttri, bottommost);
        }

        if (randomnation(SAMPLERATE) == 0) {
          splayroot = splayinsert(m, splayroot, &lefttri, nextvertex);
        } else if (randomnation(SAMPLERATE) == 0) {
          lnext(righttri, inserttri);
          splayroot = splayinsert(m, splayroot, &inserttri, nextvertex);
        }
      }
    }
    nextevent->eventptr = (VOID *) freeevents;
    freeevents = nextevent;

    if (check4events) {
      apex(farlefttri, leftvertex);
      dest(lefttri, midvertex);
      apex(lefttri, rightvertex);
      lefttest = counterclockwise(m, b, leftvertex, midvertex, rightvertex);
      if (lefttest > 0.0) {
        newevent = freeevents;
        freeevents = (struct event *) freeevents->eventptr;
        newevent->xkey = m->xminextreme;
        newevent->ykey = circletop(m, leftvertex, midvertex, rightvertex,
                                   lefttest);
        newevent->eventptr = (VOID *) encode(lefttri);
        eventheapinsert(eventheap, heapsize, newevent);
        heapsize++;
        setorg(lefttri, newevent);
      }
      apex(righttri, leftvertex);
      org(righttri, midvertex);
      apex(farrighttri, rightvertex);
      righttest = counterclockwise(m, b, leftvertex, midvertex, rightvertex);
      if (righttest > 0.0) {
        newevent = freeevents;
        freeevents = (struct event *) freeevents->eventptr;
        newevent->xkey = m->xminextreme;
        newevent->ykey = circletop(m, leftvertex, midvertex, rightvertex,
                                   righttest);
        newevent->eventptr = (VOID *) encode(farrighttri);
        eventheapinsert(eventheap, heapsize, newevent);
        heapsize++;
        setorg(farrighttri, newevent);
      }
    }
  }

  pooldeinit(&m->splaynodes);
  lprevself(bottommost);
  return removeghosts(m, b, &bottommost);
}

#endif /* not REDUCED */

/**                                                                         **/
/**                                                                         **/
/********* Sweepline Delaunay triangulation ends here                *********/

/********* General mesh construction routines begin here             *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  delaunay()   Form a Delaunay triangulation.                              */
/*                                                                           */
/*****************************************************************************/

long delaunay(mesh *m, behavior *b)
{
  long hulledges;

  m->eextras = 0;
  initializetrisubpools(m, b);

#ifdef REDUCED
  hulledges = divconqdelaunay(m, b);
#else /* not REDUCED */
  if (b->incremental) {
    hulledges = incrementaldelaunay(m, b);
  } else if (b->sweepline) {
    hulledges = sweeplinedelaunay(m, b);
  } else {
    hulledges = divconqdelaunay(m, b);
  }
#endif /* not REDUCED */

  if (m->triangles.items == 0) {
    /* The input vertices were all collinear, so there are no triangles. */
    return 0l;
  } else {
    return hulledges;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  reconstruct()   Reconstruct a triangulation from its .ele (and possibly  */
/*                  .poly) file.  Used when the -r switch is used.           */
/*                                                                           */
/*  Reads an .ele file and reconstructs the original mesh.  If the -p switch */
/*  is used, this procedure will also read a .poly file and reconstruct the  */
/*  subsegments of the original mesh.  If the -a switch is used, this        */
/*  procedure will also read an .area file and set a maximum area constraint */
/*  on each triangle.                                                        */
/*                                                                           */
/*  Vertices that are not corners of triangles, such as nodes on edges of    */
/*  subparametric elements, are discarded.                                   */
/*                                                                           */
/*  This routine finds the adjacencies between triangles (and subsegments)   */
/*  by forming one stack of triangles for each vertex.  Each triangle is on  */
/*  three different stacks simultaneously.  Each triangle's subsegment       */
/*  pointers are used to link the items in each stack.  This memory-saving   */
/*  feature makes the code harder to read.  The most important thing to keep */
/*  in mind is that each triangle is removed from a stack precisely when     */
/*  the corresponding pointer is adjusted to refer to a subsegment rather    */
/*  than the next triangle of the stack.                                     */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

int reconstruct(mesh *m, behavior *b, int *trianglelist,
                REAL *triangleattriblist, REAL *trianglearealist,
                int elements, int corners, int attribs,
                int *segmentlist,int *segmentmarkerlist, int numberofsegments)
{
  int vertexindex;
  int attribindex;
  struct otri triangleloop;
  struct otri triangleleft;
  struct otri checktri;
  struct otri checkleft;
  struct otri checkneighbor;
  struct osub subsegloop;
  triangle *vertexarray;
  triangle *prevlink;
  triangle nexttri;
  vertex tdest, tapex;
  vertex checkdest, checkapex;
  vertex shorg;
  vertex killvertex;
  vertex segmentorg, segmentdest;
  REAL area;
  int corner[3];
  int end[2];
  int killvertexindex;
  int incorners;
  int segmentmarkers;
  int boundmarker;
  int aroundvertex;
  long hullsize;
  int notfound;
  long elementnumber, segmentnumber;
  int i, j;
  triangle ptr;                         /* Temporary variable used by sym(). */

  m->inelements = elements;
  incorners = corners;
  if (incorners < 3) {
    /* Error:  Triangles must have at least 3 vertices. */
    return TRI_INPUT;
  }
  m->eextras = attribs;

  initializetrisubpools(m, b);

  /* Create the triangles. */
  for (elementnumber = 1; elementnumber <= m->inelements; elementnumber++) {
    maketriangle(m, b, &triangleloop);
    /* Mark the triangle as living. */
    triangleloop.tri[3] = (triangle) triangleloop.tri;
  }

  segmentmarkers = 0;
  if (b->poly) {
    m->insegments = numberofsegments;
    segmentmarkers = segmentmarkerlist != (int *) NULL;

    /* Create the subsegments. */
    for (segmentnumber = 1; segmentnumber <= m->insegments; segmentnumber++) {
      makesubseg(m, &subsegloop);
      /* Mark the subsegment as living. */
      subsegloop.ss[2] = (subseg) subsegloop.ss;
    }
  }

  vertexindex = 0;
  attribindex = 0;

  /* Allocate a temporary array that maps each vertex to some adjacent */
  /*   triangle.  I took care to allocate all the permanent memory for */
  /*   triangles and subsegments first.                                */
  vertexarray = (triangle *) trimalloc(m->vertices.items *
                                       (int) sizeof(triangle));
  /* Each vertex is initially unrepresented. */
  for (i = 0; i < m->vertices.items; i++) {
    vertexarray[i] = (triangle) m->dummytri;
  }

  /* Read the triangles from the .ele file, and link */
  /*   together those that share an edge.            */
  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  elementnumber = b->firstnumber;
  while (triangleloop.tri != (triangle *) NULL) {
    /* Copy the triangle's three corners. */
    for (j = 0; j < 3; j++) {
      corner[j] = trianglelist[vertexindex++];
      if ((corner[j] < b->firstnumber) ||
          (corner[j] >= b->firstnumber + m->invertices)) {
        // TODO: Error:  Triangle (elementnumber) has an invalid vertex index.
        return TRI_INPUT;
      }
    }

    /* Find out about (and throw away) extra nodes. */
    for (j = 3; j < incorners; j++) {
      killvertexindex = trianglelist[vertexindex++];
        if ((killvertexindex >= b->firstnumber) &&
            (killvertexindex < b->firstnumber + m->invertices)) {
          /* Delete the non-corner vertex if it's not already deleted. */
          killvertex = getvertex(m, b, killvertexindex);
          if (vertextype(killvertex) != DEADVERTEX) {
            vertexdealloc(m, killvertex);
          }
        }
    }

    /* Read the triangle's attributes. */
    for (j = 0; j < m->eextras; j++) {
      setelemattribute(triangleloop, j, triangleattriblist[attribindex++]);
    }

    if (b->vararea) {
      area = trianglearealist[elementnumber - b->firstnumber];
      setareabound(triangleloop, area);
    }

    /* Set the triangle's vertices. */
    triangleloop.orient = 0;
    setorg(triangleloop, getvertex(m, b, corner[0]));
    setdest(triangleloop, getvertex(m, b, corner[1]));
    setapex(triangleloop, getvertex(m, b, corner[2]));
    /* Try linking the triangle to others that share these vertices. */
    for (triangleloop.orient = 0; triangleloop.orient < 3;
         triangleloop.orient++) {
      /* Take the number for the origin of triangleloop. */
      aroundvertex = corner[triangleloop.orient];
      /* Look for other triangles having this vertex. */
      nexttri = vertexarray[aroundvertex - b->firstnumber];
      /* Link the current triangle to the next one in the stack. */
      triangleloop.tri[6 + triangleloop.orient] = nexttri;
      /* Push the current triangle onto the stack. */
      vertexarray[aroundvertex - b->firstnumber] = encode(triangleloop);
      decode(nexttri, checktri);
      if (checktri.tri != m->dummytri) {
        dest(triangleloop, tdest);
        apex(triangleloop, tapex);
        /* Look for other triangles that share an edge. */
        do {
          dest(checktri, checkdest);
          apex(checktri, checkapex);
          if (tapex == checkdest) {
            /* The two triangles share an edge; bond them together. */
            lprev(triangleloop, triangleleft);
            bond(triangleleft, checktri);
          }
          if (tdest == checkapex) {
            /* The two triangles share an edge; bond them together. */
            lprev(checktri, checkleft);
            bond(triangleloop, checkleft);
          }
          /* Find the next triangle in the stack. */
          nexttri = checktri.tri[6 + checktri.orient];
          decode(nexttri, checktri);
        } while (checktri.tri != m->dummytri);
      }
    }
    triangleloop.tri = triangletraverse(m);
    elementnumber++;
  }

  vertexindex = 0;
  hullsize = 0;                      /* Prepare to count the boundary edges. */
  if (b->poly) {
    /* Read the segments from the .poly file, and link them */
    /*   to their neighboring triangles.                    */
    boundmarker = 0;
    traversalinit(&m->subsegs);
    subsegloop.ss = subsegtraverse(m);
    segmentnumber = b->firstnumber;
    while (subsegloop.ss != (subseg *) NULL) {
      end[0] = segmentlist[vertexindex++];
      end[1] = segmentlist[vertexindex++];
      if (segmentmarkers) {
        boundmarker = segmentmarkerlist[segmentnumber - b->firstnumber];
      }

      for (j = 0; j < 2; j++) {
        if ((end[j] < b->firstnumber) ||
            (end[j] >= b->firstnumber + m->invertices)) {
          // TODO: Error:  Segment (segmentnumber) has an invalid vertex index.
          return TRI_INPUT;
        }
      }

      /* set the subsegment's vertices. */
      subsegloop.ssorient = 0;
      segmentorg = getvertex(m, b, end[0]);
      segmentdest = getvertex(m, b, end[1]);
      setsorg(subsegloop, segmentorg);
      setsdest(subsegloop, segmentdest);
      setsegorg(subsegloop, segmentorg);
      setsegdest(subsegloop, segmentdest);
      setmark(subsegloop, boundmarker);
      /* Try linking the subsegment to triangles that share these vertices. */
      for (subsegloop.ssorient = 0; subsegloop.ssorient < 2;
           subsegloop.ssorient++) {
        /* Take the number for the destination of subsegloop. */
        aroundvertex = end[1 - subsegloop.ssorient];
        /* Look for triangles having this vertex. */
        prevlink = &vertexarray[aroundvertex - b->firstnumber];
        nexttri = vertexarray[aroundvertex - b->firstnumber];
        decode(nexttri, checktri);
        sorg(subsegloop, shorg);
        notfound = 1;
        /* Look for triangles having this edge.  Note that I'm only       */
        /*   comparing each triangle's destination with the subsegment;   */
        /*   each triangle's apex is handled through a different vertex.  */
        /*   Because each triangle appears on three vertices' lists, each */
        /*   occurrence of a triangle on a list can (and does) represent  */
        /*   an edge.  In this way, most edges are represented twice, and */
        /*   every triangle-subsegment bond is represented once.          */
        while (notfound && (checktri.tri != m->dummytri)) {
          dest(checktri, checkdest);
          if (shorg == checkdest) {
            /* We have a match.  Remove this triangle from the list. */
            *prevlink = checktri.tri[6 + checktri.orient];
            /* Bond the subsegment to the triangle. */
            tsbond(checktri, subsegloop);
            /* Check if this is a boundary edge. */
            sym(checktri, checkneighbor);
            if (checkneighbor.tri == m->dummytri) {
              /* The next line doesn't insert a subsegment (because there's */
              /*   already one there), but it sets the boundary markers of  */
              /*   the existing subsegment and its vertices.                */
              insertsubseg(m, b, &checktri, 1);
              hullsize++;
            }
            notfound = 0;
          }
          /* Find the next triangle in the stack. */
          prevlink = &checktri.tri[6 + checktri.orient];
          nexttri = checktri.tri[6 + checktri.orient];
          decode(nexttri, checktri);
        }
      }
      subsegloop.ss = subsegtraverse(m);
      segmentnumber++;
    }
  }

  /* Mark the remaining edges as not being attached to any subsegment. */
  /* Also, count the (yet uncounted) boundary edges.                   */
  for (i = 0; i < m->vertices.items; i++) {
    /* Search the stack of triangles adjacent to a vertex. */
    nexttri = vertexarray[i];
    decode(nexttri, checktri);
    while (checktri.tri != m->dummytri) {
      /* Find the next triangle in the stack before this */
      /*   information gets overwritten.                 */
      nexttri = checktri.tri[6 + checktri.orient];
      /* No adjacent subsegment.  (This overwrites the stack info.) */
      tsdissolve(checktri);
      sym(checktri, checkneighbor);
      if (checkneighbor.tri == m->dummytri) {
        insertsubseg(m, b, &checktri, 1);
        hullsize++;
      }
      decode(nexttri, checktri);
    }
  }

  trifree((VOID *) vertexarray);
  return hullsize;
}

#endif /* not CDT_ONLY */

/**                                                                         **/
/**                                                                         **/
/********* General mesh construction routines end here               *********/

/********* Segment insertion begins here                             *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  finddirection()   Find the first triangle on the path from one point     */
/*                    to another.                                            */
/*                                                                           */
/*  Finds the triangle that intersects a line segment drawn from the         */
/*  origin of `searchtri' to the point `searchpoint', and returns the result */
/*  in `searchtri'.  The origin of `searchtri' does not change, even though  */
/*  the triangle returned may differ from the one passed in.  This routine   */
/*  is used to find the direction to move in to get from one point to        */
/*  another.                                                                 */
/*                                                                           */
/*  The return value notes whether the destination or apex of the found      */
/*  triangle is collinear with the two points in question.                   */
/*                                                                           */
/*****************************************************************************/

enum finddirectionresult finddirection(mesh *m, behavior *b,
                                       struct otri *searchtri,
                                       vertex searchpoint, int *status)
{
  struct otri checktri;
  vertex startvertex;
  vertex leftvertex, rightvertex;
  REAL leftccw, rightccw;
  int leftflag, rightflag;
  triangle ptr;           /* Temporary variable used by onext() and oprev(). */

  org(*searchtri, startvertex);
  dest(*searchtri, rightvertex);
  apex(*searchtri, leftvertex);
  /* Is `searchpoint' to the left? */
  leftccw = counterclockwise(m, b, searchpoint, startvertex, leftvertex);
  leftflag = leftccw > 0.0;
  /* Is `searchpoint' to the right? */
  rightccw = counterclockwise(m, b, startvertex, searchpoint, rightvertex);
  rightflag = rightccw > 0.0;
  if (leftflag && rightflag) {
    /* `searchtri' faces directly away from `searchpoint'.  We could go left */
    /*   or right.  Ask whether it's a triangle or a boundary on the left.   */
    onext(*searchtri, checktri);
    if (checktri.tri == m->dummytri) {
      leftflag = 0;
    } else {
      rightflag = 0;
    }
  }
  while (leftflag) {
    /* Turn left until satisfied. */
    onextself(*searchtri);
    if (searchtri->tri == m->dummytri) {
      // TODO: Internal error: finddirection()
      // Unable to find a triangle leading from (startvertex) to (searchpoint).
      *status = TRI_FIND_DIRECTION;
	  return WITHIN;
    }
    apex(*searchtri, leftvertex);
    rightccw = leftccw;
    leftccw = counterclockwise(m, b, searchpoint, startvertex, leftvertex);
    leftflag = leftccw > 0.0;
  }
  while (rightflag) {
    /* Turn right until satisfied. */
    oprevself(*searchtri);
    if (searchtri->tri == m->dummytri) {
      // TODO: Internal error: finddirection()
      // Unable to find a triangle leading from (startvertex) to (searchpoint).
      *status = TRI_FIND_DIRECTION;
	  return WITHIN;
    }
    dest(*searchtri, rightvertex);
    leftccw = rightccw;
    rightccw = counterclockwise(m, b, startvertex, searchpoint, rightvertex);
    rightflag = rightccw > 0.0;
  }
  if (leftccw == 0.0) {
    return LEFTCOLLINEAR;
  } else if (rightccw == 0.0) {
    return RIGHTCOLLINEAR;
  } else {
    return WITHIN;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  segmentintersection()   Find the intersection of an existing segment     */
/*                          and a segment that is being inserted.  Insert    */
/*                          a vertex at the intersection, splitting an       */
/*                          existing subsegment.                             */
/*                                                                           */
/*  The segment being inserted connects the apex of splittri to endpoint2.   */
/*  splitsubseg is the subsegment being split, and MUST adjoin splittri.     */
/*  Hence, endpoints of the subsegment being split are the origin and        */
/*  destination of splittri.                                                 */
/*                                                                           */
/*  On completion, splittri is a handle having the newly inserted            */
/*  intersection point as its origin, and endpoint1 as its destination.      */
/*                                                                           */
/*****************************************************************************/

void segmentintersection(mesh *m, behavior *b,
                         struct otri *splittri, struct osub *splitsubseg,
                         vertex endpoint2, int *status)
{
  struct osub opposubseg;
  vertex endpoint1;
  vertex torg, tdest;
  vertex leftvertex, rightvertex;
  vertex newvertex;
  enum insertvertexresult success;
  enum finddirectionresult collinear;
  REAL ex, ey;
  REAL tx, ty;
  REAL etx, ety;
  REAL split, denom;
  int i;
  triangle ptr;                       /* Temporary variable used by onext(). */
  subseg sptr;                        /* Temporary variable used by snext(). */

  /* Find the other three segment endpoints. */
  apex(*splittri, endpoint1);
  org(*splittri, torg);
  dest(*splittri, tdest);
  /* Segment intersection formulae; see the Antonio reference. */
  tx = tdest[0] - torg[0];
  ty = tdest[1] - torg[1];
  ex = endpoint2[0] - endpoint1[0];
  ey = endpoint2[1] - endpoint1[1];
  etx = torg[0] - endpoint2[0];
  ety = torg[1] - endpoint2[1];
  denom = ty * ex - tx * ey;
  if (denom == 0.0) {
      // TODO: Internal error: segmentintersection()
      // Attempt to find intersection of parallel segments.
      *status = TRI_SEG_INTERSECT;
	  return;
  }
  split = (ey * etx - ex * ety) / denom;
  /* Create the new vertex. */
  newvertex = (vertex) poolalloc(&m->vertices);
  /* Interpolate its coordinate and attributes. */
  for (i = 0; i < 2 + m->nextras; i++) {
    newvertex[i] = torg[i] + split * (tdest[i] - torg[i]);
  }
  setvertexmark(newvertex, mark(*splitsubseg));
  setvertextype(newvertex, INPUTVERTEX);
  /* Insert the intersection vertex.  This should always succeed. */
  success = insertvertex(m, b, newvertex, splittri, splitsubseg, 0, 0, 0);
  if (success != SUCCESSFULVERTEX) {
      // TODO: Internal error: segmentintersection()
      // Failure to split a segment.
      *status = TRI_SEG_INTERSECT;
	  return;
  }
  /* Record a triangle whose origin is the new vertex. */
  setvertex2tri(newvertex, encode(*splittri));
  if (m->steinerleft > 0) {
    m->steinerleft--;
  }

  /* Divide the segment into two, and correct the segment endpoints. */
  ssymself(*splitsubseg);
  spivot(*splitsubseg, opposubseg);
  sdissolve(*splitsubseg);
  sdissolve(opposubseg);
  do {
    setsegorg(*splitsubseg, newvertex);
    snextself(*splitsubseg);
  } while (splitsubseg->ss != m->dummysub);
  do {
    setsegorg(opposubseg, newvertex);
    snextself(opposubseg);
  } while (opposubseg.ss != m->dummysub);

  /* Inserting the vertex may have caused edge flips.  We wish to rediscover */
  /*   the edge connecting endpoint1 to the new intersection vertex.         */
  collinear = finddirection(m, b, splittri, endpoint1, status);
  if (*status < 0) return;

  dest(*splittri, rightvertex);
  apex(*splittri, leftvertex);
  if ((leftvertex[0] == endpoint1[0]) && (leftvertex[1] == endpoint1[1])) {
    onextself(*splittri);
  } else if ((rightvertex[0] != endpoint1[0]) ||
             (rightvertex[1] != endpoint1[1])) {
      // TODO: Internal error: segmentintersection()
      // Topological inconsistency after splitting a segment.
      *status = TRI_SEG_INTERSECT;
	  return;
  }
  /* `splittri' should have destination endpoint1. */
}

/*****************************************************************************/
/*                                                                           */
/*  scoutsegment()   Scout the first triangle on the path from one endpoint  */
/*                   to another, and check for completion (reaching the      */
/*                   second endpoint), a collinear vertex, or the            */
/*                   intersection of two segments.                           */
/*                                                                           */
/*  Returns one if the entire segment is successfully inserted, and zero if  */
/*  the job must be finished by conformingedge() or constrainededge().       */
/*                                                                           */
/*  If the first triangle on the path has the second endpoint as its         */
/*  destination or apex, a subsegment is inserted and the job is done.       */
/*                                                                           */
/*  If the first triangle on the path has a destination or apex that lies on */
/*  the segment, a subsegment is inserted connecting the first endpoint to   */
/*  the collinear vertex, and the search is continued from the collinear     */
/*  vertex.                                                                  */
/*                                                                           */
/*  If the first triangle on the path has a subsegment opposite its origin,  */
/*  then there is a segment that intersects the segment being inserted.      */
/*  Their intersection vertex is inserted, splitting the subsegment.         */
/*                                                                           */
/*****************************************************************************/

int scoutsegment(mesh *m, behavior *b, struct otri *searchtri,
                 vertex endpoint2, int newmark, int *status)
{
  struct otri crosstri;
  struct osub crosssubseg;
  vertex leftvertex, rightvertex;
  enum finddirectionresult collinear;
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  collinear = finddirection(m, b, searchtri, endpoint2, status);
  if (*status < 0) return TRI_SEG_SCOUT;

  dest(*searchtri, rightvertex);
  apex(*searchtri, leftvertex);
  if (((leftvertex[0] == endpoint2[0]) && (leftvertex[1] == endpoint2[1])) ||
      ((rightvertex[0] == endpoint2[0]) && (rightvertex[1] == endpoint2[1]))) {
    /* The segment is already an edge in the mesh. */
    if ((leftvertex[0] == endpoint2[0]) && (leftvertex[1] == endpoint2[1])) {
      lprevself(*searchtri);
    }
    /* Insert a subsegment, if there isn't already one there. */
    insertsubseg(m, b, searchtri, newmark);
    return 1;
  } else if (collinear == LEFTCOLLINEAR) {
    /* We've collided with a vertex between the segment's endpoints. */
    /* Make the collinear vertex be the triangle's origin. */
    lprevself(*searchtri);
    insertsubseg(m, b, searchtri, newmark);
    /* Insert the remainder of the segment. */
    return scoutsegment(m, b, searchtri, endpoint2, newmark, status);
  } else if (collinear == RIGHTCOLLINEAR) {
    /* We've collided with a vertex between the segment's endpoints. */
    insertsubseg(m, b, searchtri, newmark);
    /* Make the collinear vertex be the triangle's origin. */
    lnextself(*searchtri);
    /* Insert the remainder of the segment. */
    return scoutsegment(m, b, searchtri, endpoint2, newmark, status);
  } else {
    lnext(*searchtri, crosstri);
    tspivot(crosstri, crosssubseg);
    /* Check for a crossing segment. */
    if (crosssubseg.ss == m->dummysub) {
      return 0;
    } else {
      /* Insert a vertex at the intersection. */
      segmentintersection(m, b, &crosstri, &crosssubseg, endpoint2, status);
      if (*status < 0) return TRI_SEG_SCOUT;

      otricopy(crosstri, *searchtri);
      insertsubseg(m, b, searchtri, newmark);
      /* Insert the remainder of the segment. */
      return scoutsegment(m, b, searchtri, endpoint2, newmark, status);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  conformingedge()   Force a segment into a conforming Delaunay            */
/*                     triangulation by inserting a vertex at its midpoint,  */
/*                     and recursively forcing in the two half-segments if   */
/*                     necessary.                                            */
/*                                                                           */
/*  Generates a sequence of subsegments connecting `endpoint1' to            */
/*  `endpoint2'.  `newmark' is the boundary marker of the segment, assigned  */
/*  to each new splitting vertex and subsegment.                             */
/*                                                                           */
/*  Note that conformingedge() does not always maintain the conforming       */
/*  Delaunay property.  Once inserted, segments are locked into place;       */
/*  vertices inserted later (to force other segments in) may render these    */
/*  fixed segments non-Delaunay.  The conforming Delaunay property will be   */
/*  restored by enforcequality() by splitting encroached subsegments.        */
/*                                                                           */
/*****************************************************************************/

#ifndef REDUCED
#ifndef CDT_ONLY

void conformingedge(mesh *m, behavior *b, vertex endpoint1, vertex endpoint2,
                    int newmark, int *status)
{
  struct otri searchtri1, searchtri2;
  struct osub brokensubseg;
  vertex newvertex;
  vertex midvertex1, midvertex2;
  enum insertvertexresult success;
  int i;
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Create a new vertex to insert in the middle of the segment. */
  newvertex = (vertex) poolalloc(&m->vertices);
  /* Interpolate coordinates and attributes. */
  for (i = 0; i < 2 + m->nextras; i++) {
    newvertex[i] = 0.5 * (endpoint1[i] + endpoint2[i]);
  }
  setvertexmark(newvertex, newmark);
  setvertextype(newvertex, SEGMENTVERTEX);
  /* No known triangle to search from. */
  searchtri1.tri = m->dummytri;
  /* Attempt to insert the new vertex. */
  success = insertvertex(m, b, newvertex, &searchtri1, (struct osub *) NULL,
                         0, 0, 0);
  if (success == DUPLICATEVERTEX) {
    /* Use the vertex that's already there. */
    vertexdealloc(m, newvertex);
    org(searchtri1, newvertex);
  } else {
    if (success == VIOLATINGVERTEX) {
      /* By fluke, we've landed right on another segment.  Split it. */
      tspivot(searchtri1, brokensubseg);
      success = insertvertex(m, b, newvertex, &searchtri1, &brokensubseg,
                             0, 0, 0);
      if (success != SUCCESSFULVERTEX) {
        printf("Internal error in conformingedge():\n");
        printf("  Failure to split a segment.\n");
        internalerror();
      }
    }
    /* The vertex has been inserted successfully. */
    if (m->steinerleft > 0) {
      m->steinerleft--;
    }
  }
  otricopy(searchtri1, searchtri2);
  /* `searchtri1' and `searchtri2' are fastened at their origins to         */
  /*   `newvertex', and will be directed toward `endpoint1' and `endpoint2' */
  /*   respectively.  First, we must get `searchtri2' out of the way so it  */
  /*   won't be invalidated during the insertion of the first half of the   */
  /*   segment.                                                             */
  finddirection(m, b, &searchtri2, endpoint2, status);
  if (*status < 0) {
    return;
  }
  if (!scoutsegment(m, b, &searchtri1, endpoint1, newmark, status)) {
    if (*status < 0) return;
    /* The origin of searchtri1 may have changed if a collision with an */
    /*   intervening vertex on the segment occurred.                    */
    org(searchtri1, midvertex1);
    conformingedge(m, b, midvertex1, endpoint1, newmark, status);
  }
  if (!scoutsegment(m, b, &searchtri2, endpoint2, newmark, status)) {
    if (*status < 0) return;
    /* The origin of searchtri2 may have changed if a collision with an */
    /*   intervening vertex on the segment occurred.                    */
    org(searchtri2, midvertex2);
    conformingedge(m, b, midvertex2, endpoint2, newmark, status);
  }
}

#endif /* not CDT_ONLY */
#endif /* not REDUCED */

/*****************************************************************************/
/*                                                                           */
/*  delaunayfixup()   Enforce the Delaunay condition at an edge, fanning out */
/*                    recursively from an existing vertex.  Pay special      */
/*                    attention to stacking inverted triangles.              */
/*                                                                           */
/*  This is a support routine for inserting segments into a constrained      */
/*  Delaunay triangulation.                                                  */
/*                                                                           */
/*  The origin of fixuptri is treated as if it has just been inserted, and   */
/*  the local Delaunay condition needs to be enforced.  It is only enforced  */
/*  in one sector, however, that being the angular range defined by          */
/*  fixuptri.                                                                */
/*                                                                           */
/*  This routine also needs to make decisions regarding the "stacking" of    */
/*  triangles.  (Read the description of constrainededge() below before      */
/*  reading on here, so you understand the algorithm.)  If the position of   */
/*  the new vertex (the origin of fixuptri) indicates that the vertex before */
/*  it on the polygon is a reflex vertex, then "stack" the triangle by       */
/*  doing nothing.  (fixuptri is an inverted triangle, which is how stacked  */
/*  triangles are identified.)                                               */
/*                                                                           */
/*  Otherwise, check whether the vertex before that was a reflex vertex.     */
/*  If so, perform an edge flip, thereby eliminating an inverted triangle    */
/*  (popping it off the stack).  The edge flip may result in the creation    */
/*  of a new inverted triangle, depending on whether or not the new vertex   */
/*  is visible to the vertex three edges behind on the polygon.              */
/*                                                                           */
/*  If neither of the two vertices behind the new vertex are reflex          */
/*  vertices, fixuptri and fartri, the triangle opposite it, are not         */
/*  inverted; hence, ensure that the edge between them is locally Delaunay.  */
/*                                                                           */
/*  `leftside' indicates whether or not fixuptri is to the left of the       */
/*  segment being inserted.  (Imagine that the segment is pointing up from   */
/*  endpoint1 to endpoint2.)                                                 */
/*                                                                           */
/*****************************************************************************/

void delaunayfixup(mesh *m, behavior *b,
                   struct otri *fixuptri, int leftside)
{
  struct otri neartri;
  struct otri fartri;
  struct osub faredge;
  vertex nearvertex, leftvertex, rightvertex, farvertex;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  lnext(*fixuptri, neartri);
  sym(neartri, fartri);
  /* Check if the edge opposite the origin of fixuptri can be flipped. */
  if (fartri.tri == m->dummytri) {
    return;
  }
  tspivot(neartri, faredge);
  if (faredge.ss != m->dummysub) {
    return;
  }
  /* Find all the relevant vertices. */
  apex(neartri, nearvertex);
  org(neartri, leftvertex);
  dest(neartri, rightvertex);
  apex(fartri, farvertex);
  /* Check whether the previous polygon vertex is a reflex vertex. */
  if (leftside) {
    if (counterclockwise(m, b, nearvertex, leftvertex, farvertex) <= 0.0) {
      /* leftvertex is a reflex vertex too.  Nothing can */
      /*   be done until a convex section is found.      */
      return;
    }
  } else {
    if (counterclockwise(m, b, farvertex, rightvertex, nearvertex) <= 0.0) {
      /* rightvertex is a reflex vertex too.  Nothing can */
      /*   be done until a convex section is found.       */
      return;
    }
  }
  if (counterclockwise(m, b, rightvertex, leftvertex, farvertex) > 0.0) {
    /* fartri is not an inverted triangle, and farvertex is not a reflex */
    /*   vertex.  As there are no reflex vertices, fixuptri isn't an     */
    /*   inverted triangle, either.  Hence, test the edge between the    */
    /*   triangles to ensure it is locally Delaunay.                     */
    if (incircle(m, b, leftvertex, farvertex, rightvertex, nearvertex) <=
        0.0) {
      return;
    }
    /* Not locally Delaunay; go on to an edge flip. */
  }        /* else fartri is inverted; remove it from the stack by flipping. */
  flip(m, b, &neartri);
  lprevself(*fixuptri);    /* Restore the origin of fixuptri after the flip. */
  /* Recursively process the two triangles that result from the flip. */
  delaunayfixup(m, b, fixuptri, leftside);
  delaunayfixup(m, b, &fartri, leftside);
}

/*****************************************************************************/
/*                                                                           */
/*  constrainededge()   Force a segment into a constrained Delaunay          */
/*                      triangulation by deleting the triangles it           */
/*                      intersects, and triangulating the polygons that      */
/*                      form on each side of it.                             */
/*                                                                           */
/*  Generates a single subsegment connecting `endpoint1' to `endpoint2'.     */
/*  The triangle `starttri' has `endpoint1' as its origin.  `newmark' is the */
/*  boundary marker of the segment.                                          */
/*                                                                           */
/*  To insert a segment, every triangle whose interior intersects the        */
/*  segment is deleted.  The union of these deleted triangles is a polygon   */
/*  (which is not necessarily monotone, but is close enough), which is       */
/*  divided into two polygons by the new segment.  This routine's task is    */
/*  to generate the Delaunay triangulation of these two polygons.            */
/*                                                                           */
/*  You might think of this routine's behavior as a two-step process.  The   */
/*  first step is to walk from endpoint1 to endpoint2, flipping each edge    */
/*  encountered.  This step creates a fan of edges connected to endpoint1,   */
/*  including the desired edge to endpoint2.  The second step enforces the   */
/*  Delaunay condition on each side of the segment in an incremental manner: */
/*  proceeding along the polygon from endpoint1 to endpoint2 (this is done   */
/*  independently on each side of the segment), each vertex is "enforced"    */
/*  as if it had just been inserted, but affecting only the previous         */
/*  vertices.  The result is the same as if the vertices had been inserted   */
/*  in the order they appear on the polygon, so the result is Delaunay.      */
/*                                                                           */
/*  In truth, constrainededge() interleaves these two steps.  The procedure  */
/*  walks from endpoint1 to endpoint2, and each time an edge is encountered  */
/*  and flipped, the newly exposed vertex (at the far end of the flipped     */
/*  edge) is "enforced" upon the previously flipped edges, usually affecting */
/*  only one side of the polygon (depending upon which side of the segment   */
/*  the vertex falls on).                                                    */
/*                                                                           */
/*  The algorithm is complicated by the need to handle polygons that are not */
/*  convex.  Although the polygon is not necessarily monotone, it can be     */
/*  triangulated in a manner similar to the stack-based algorithms for       */
/*  monotone polygons.  For each reflex vertex (local concavity) of the      */
/*  polygon, there will be an inverted triangle formed by one of the edge    */
/*  flips.  (An inverted triangle is one with negative area - that is, its   */
/*  vertices are arranged in clockwise order - and is best thought of as a   */
/*  wrinkle in the fabric of the mesh.)  Each inverted triangle can be       */
/*  thought of as a reflex vertex pushed on the stack, waiting to be fixed   */
/*  later.                                                                   */
/*                                                                           */
/*  A reflex vertex is popped from the stack when a vertex is inserted that  */
/*  is visible to the reflex vertex.  (However, if the vertex behind the     */
/*  reflex vertex is not visible to the reflex vertex, a new inverted        */
/*  triangle will take its place on the stack.)  These details are handled   */
/*  by the delaunayfixup() routine above.                                    */
/*                                                                           */
/*****************************************************************************/

void constrainededge(mesh *m, behavior *b,
                     struct otri *starttri, vertex endpoint2, int newmark, int *status)
{
  struct otri fixuptri, fixuptri2;
  struct osub crosssubseg;
  vertex endpoint1;
  vertex farvertex;
  REAL area;
  int collision;
  int done;
  triangle ptr;             /* Temporary variable used by sym() and oprev(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  org(*starttri, endpoint1);
  lnext(*starttri, fixuptri);
  flip(m, b, &fixuptri);
  /* `collision' indicates whether we have found a vertex directly */
  /*   between endpoint1 and endpoint2.                            */
  collision = 0;
  done = 0;
  do {
    org(fixuptri, farvertex);
    /* `farvertex' is the extreme point of the polygon we are "digging" */
    /*   to get from endpoint1 to endpoint2.                           */
    if ((farvertex[0] == endpoint2[0]) && (farvertex[1] == endpoint2[1])) {
      oprev(fixuptri, fixuptri2);
      /* Enforce the Delaunay condition around endpoint2. */
      delaunayfixup(m, b, &fixuptri, 0);
      delaunayfixup(m, b, &fixuptri2, 1);
      done = 1;
    } else {
      /* Check whether farvertex is to the left or right of the segment */
      /*   being inserted, to decide which edge of fixuptri to dig      */
      /*   through next.                                                */
      area = counterclockwise(m, b, endpoint1, endpoint2, farvertex);
      if (area == 0.0) {
        /* We've collided with a vertex between endpoint1 and endpoint2. */
        collision = 1;
        oprev(fixuptri, fixuptri2);
        /* Enforce the Delaunay condition around farvertex. */
        delaunayfixup(m, b, &fixuptri, 0);
        delaunayfixup(m, b, &fixuptri2, 1);
        done = 1;
      } else {
        if (area > 0.0) {        /* farvertex is to the left of the segment. */
          oprev(fixuptri, fixuptri2);
          /* Enforce the Delaunay condition around farvertex, on the */
          /*   left side of the segment only.                        */
          delaunayfixup(m, b, &fixuptri2, 1);
          /* Flip the edge that crosses the segment.  After the edge is */
          /*   flipped, one of its endpoints is the fan vertex, and the */
          /*   destination of fixuptri is the fan vertex.               */
          lprevself(fixuptri);
        } else {                /* farvertex is to the right of the segment. */
          delaunayfixup(m, b, &fixuptri, 0);
          /* Flip the edge that crosses the segment.  After the edge is */
          /*   flipped, one of its endpoints is the fan vertex, and the */
          /*   destination of fixuptri is the fan vertex.               */
          oprevself(fixuptri);
        }
        /* Check for two intersecting segments. */
        tspivot(fixuptri, crosssubseg);
        if (crosssubseg.ss == m->dummysub) {
          flip(m, b, &fixuptri);    /* May create inverted triangle at left. */
        } else {
          /* We've collided with a segment between endpoint1 and endpoint2. */
          collision = 1;
          /* Insert a vertex at the intersection. */
          segmentintersection(m, b, &fixuptri, &crosssubseg, endpoint2, status);
          done = 1;
        }
      }
    }
  } while (!done);
  /* Insert a subsegment to make the segment permanent. */
  insertsubseg(m, b, &fixuptri, newmark);
  /* If there was a collision with an interceding vertex, install another */
  /*   segment connecting that vertex with endpoint2.                     */
  if (collision) {
    /* Insert the remainder of the segment. */
    if (!scoutsegment(m, b, &fixuptri, endpoint2, newmark, status)) {
      constrainededge(m, b, &fixuptri, endpoint2, newmark, status);
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  insertsegment()   Insert a PSLG segment into a triangulation.            */
/*                                                                           */
/*****************************************************************************/

void insertsegment(mesh *m, behavior *b,
                   vertex endpoint1, vertex endpoint2, int newmark, int *status)
{
  struct otri searchtri1, searchtri2;
  triangle encodedtri;
  vertex checkvertex;
  triangle ptr;                         /* Temporary variable used by sym(). */

  /* Find a triangle whose origin is the segment's first endpoint. */
  checkvertex = (vertex) NULL;
  encodedtri = vertex2tri(endpoint1);
  if (encodedtri != (triangle) NULL) {
    decode(encodedtri, searchtri1);
    org(searchtri1, checkvertex);
  }
  if (checkvertex != endpoint1) {
    /* Find a boundary triangle to search from. */
    searchtri1.tri = m->dummytri;
    searchtri1.orient = 0;
    symself(searchtri1);
    /* Search for the segment's first endpoint by point location. */
    if (locate(m, b, endpoint1, &searchtri1) != ONVERTEX) {
      // TODO: Internal error: insertsegment()
      // Unable to locate PSLG vertex (endpoint1) in triangulation.
      *status = TRI_SEG_INSERT;
	  return;
    }
  }
  /* Remember this triangle to improve subsequent point location. */
  otricopy(searchtri1, m->recenttri);
  /* Scout the beginnings of a path from the first endpoint */
  /*   toward the second.                                   */
  if (scoutsegment(m, b, &searchtri1, endpoint2, newmark, status)) {
    /* The segment was easily inserted. */
    return;
  }
  /* The first endpoint may have changed if a collision with an intervening */
  /*   vertex on the segment occurred.                                      */
  org(searchtri1, endpoint1);

  /* Find a triangle whose origin is the segment's second endpoint. */
  checkvertex = (vertex) NULL;
  encodedtri = vertex2tri(endpoint2);
  if (encodedtri != (triangle) NULL) {
    decode(encodedtri, searchtri2);
    org(searchtri2, checkvertex);
  }
  if (checkvertex != endpoint2) {
    /* Find a boundary triangle to search from. */
    searchtri2.tri = m->dummytri;
    searchtri2.orient = 0;
    symself(searchtri2);
    /* Search for the segment's second endpoint by point location. */
    if (locate(m, b, endpoint2, &searchtri2) != ONVERTEX) {
      // TODO: Internal error: insertsegment()
      // Unable to locate PSLG vertex (endpoint2) in triangulation.
      *status = TRI_SEG_INSERT;
	  return;
    }
  }
  /* Remember this triangle to improve subsequent point location. */
  otricopy(searchtri2, m->recenttri);
  /* Scout the beginnings of a path from the second endpoint */
  /*   toward the first.                                     */
  if (scoutsegment(m, b, &searchtri2, endpoint1, newmark, status)) {
    /* The segment was easily inserted. */
    return;
  }
  /* The second endpoint may have changed if a collision with an intervening */
  /*   vertex on the segment occurred.                                       */
  org(searchtri2, endpoint2);

#ifndef REDUCED
#ifndef CDT_ONLY
  if (b->splitseg) {
    /* Insert vertices to force the segment into the triangulation. */
    conformingedge(m, b, endpoint1, endpoint2, newmark, status);
  } else {
#endif /* not CDT_ONLY */
#endif /* not REDUCED */
    /* Insert the segment directly into the triangulation. */
    constrainededge(m, b, &searchtri1, endpoint2, newmark, status);
#ifndef REDUCED
#ifndef CDT_ONLY
  }
#endif /* not CDT_ONLY */
#endif /* not REDUCED */
}

/*****************************************************************************/
/*                                                                           */
/*  markhull()   Cover the convex hull of a triangulation with subsegments.  */
/*                                                                           */
/*****************************************************************************/

void markhull(mesh *m, behavior *b)
{
  struct otri hulltri;
  struct otri nexttri;
  struct otri starttri;
  triangle ptr;             /* Temporary variable used by sym() and oprev(). */

  /* Find a triangle handle on the hull. */
  hulltri.tri = m->dummytri;
  hulltri.orient = 0;
  symself(hulltri);
  /* Remember where we started so we know when to stop. */
  otricopy(hulltri, starttri);
  /* Go once counterclockwise around the convex hull. */
  do {
    /* Create a subsegment if there isn't already one here. */
    insertsubseg(m, b, &hulltri, 1);
    /* To find the next hull edge, go clockwise around the next vertex. */
    lnextself(hulltri);
    oprev(hulltri, nexttri);
    while (nexttri.tri != m->dummytri) {
      otricopy(nexttri, hulltri);
      oprev(hulltri, nexttri);
    }
  } while (!otriequal(hulltri, starttri));
}

/*****************************************************************************/
/*                                                                           */
/*  formskeleton()   Create the segments of a triangulation, including PSLG  */
/*                   segments and edges on the convex hull.                  */
/*                                                                           */
/*  The PSLG segments are read from a .poly file.  The return value is the   */
/*  number of segments in the file.                                          */
/*                                                                           */
/*****************************************************************************/

void formskeleton(mesh *m, behavior *b, int *segmentlist,
                  int *segmentmarkerlist, int numberofsegments, int *status)
{
  int index;
  vertex endpoint1, endpoint2;
  int segmentmarkers;
  int end1, end2;
  int boundmarker;
  int i;

  if (b->poly) {
    m->insegments = numberofsegments;
    segmentmarkers = segmentmarkerlist != (int *) NULL;
    index = 0;

    /* If the input vertices are collinear, there is no triangulation, */
    /*   so don't try to insert segments.                              */
    if (m->triangles.items == 0) {
      return;
    }

    /* If segments are to be inserted, compute a mapping */
    /*   from vertices to triangles.                     */
    if (m->insegments > 0) {
      makevertexmap(m, b);
    }

    boundmarker = 0;
    /* Read and insert the segments. */
    for (i = 0; i < m->insegments; i++) {
      end1 = segmentlist[index++];
      end2 = segmentlist[index++];
      if (segmentmarkers) {
        boundmarker = segmentmarkerlist[i];
      }

      if ((end1 < b->firstnumber) ||
          (end1 >= b->firstnumber + m->invertices)) {
#ifdef _DEBUG
          printf("Warning:  Invalid first endpoint of segment %d.\n",
                 b->firstnumber + i);
#endif
      } else if ((end2 < b->firstnumber) ||
                 (end2 >= b->firstnumber + m->invertices)) {
#ifdef _DEBUG
          printf("Warning:  Invalid second endpoint of segment %d.\n",
                 b->firstnumber + i);
#endif
      } else {
        /* Find the vertices numbered `end1' and `end2'. */
        endpoint1 = getvertex(m, b, end1);
        endpoint2 = getvertex(m, b, end2);
        if ((endpoint1[0] == endpoint2[0]) && (endpoint1[1] == endpoint2[1])) {
#ifdef _DEBUG
            printf("Warning:  Endpoints of segment %d are coincident.\n",
                   b->firstnumber + i);
#endif
        } else {
          insertsegment(m, b, endpoint1, endpoint2, boundmarker, status);
          if (*status < 0) return;
        }
      }
    }
  } else {
    m->insegments = 0;
  }
  if (b->convex || !b->poly) {
    /* Enclose the convex hull with subsegments. */
    markhull(m, b);
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Segment insertion ends here                               *********/

/********* Carving out holes and concavities begins here             *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  infecthull()   Virally infect all of the triangles of the convex hull    */
/*                 that are not protected by subsegments.  Where there are   */
/*                 subsegments, set boundary markers as appropriate.         */
/*                                                                           */
/*****************************************************************************/

void infecthull(mesh *m, behavior *b)
{
  struct otri hulltri;
  struct otri nexttri;
  struct otri starttri;
  struct osub hullsubseg;
  triangle **deadtriangle;
  vertex horg, hdest;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Find a triangle handle on the hull. */
  hulltri.tri = m->dummytri;
  hulltri.orient = 0;
  symself(hulltri);
  /* Remember where we started so we know when to stop. */
  otricopy(hulltri, starttri);
  /* Go once counterclockwise around the convex hull. */
  do {
    /* Ignore triangles that are already infected. */
    if (!infected(hulltri)) {
      /* Is the triangle protected by a subsegment? */
      tspivot(hulltri, hullsubseg);
      if (hullsubseg.ss == m->dummysub) {
        /* The triangle is not protected; infect it. */
        if (!infected(hulltri)) {
          infect(hulltri);
          deadtriangle = (triangle **) poolalloc(&m->viri);
          *deadtriangle = hulltri.tri;
        }
      } else {
        /* The triangle is protected; set boundary markers if appropriate. */
        if (mark(hullsubseg) == 0) {
          setmark(hullsubseg, 1);
          org(hulltri, horg);
          dest(hulltri, hdest);
          if (vertexmark(horg) == 0) {
            setvertexmark(horg, 1);
          }
          if (vertexmark(hdest) == 0) {
            setvertexmark(hdest, 1);
          }
        }
      }
    }
    /* To find the next hull edge, go clockwise around the next vertex. */
    lnextself(hulltri);
    oprev(hulltri, nexttri);
    while (nexttri.tri != m->dummytri) {
      otricopy(nexttri, hulltri);
      oprev(hulltri, nexttri);
    }
  } while (!otriequal(hulltri, starttri));
}

/*****************************************************************************/
/*                                                                           */
/*  plague()   Spread the virus from all infected triangles to any neighbors */
/*             not protected by subsegments.  Delete all infected triangles. */
/*                                                                           */
/*  This is the procedure that actually creates holes and concavities.       */
/*                                                                           */
/*  This procedure operates in two phases.  The first phase identifies all   */
/*  the triangles that will die, and marks them as infected.  They are       */
/*  marked to ensure that each triangle is added to the virus pool only      */
/*  once, so the procedure will terminate.                                   */
/*                                                                           */
/*  The second phase actually eliminates the infected triangles.  It also    */
/*  eliminates orphaned vertices.                                            */
/*                                                                           */
/*****************************************************************************/

void plague(mesh *m, behavior *b)
{
  struct otri testtri;
  struct otri neighbor;
  triangle **virusloop;
  triangle **deadtriangle;
  struct osub neighborsubseg;
  vertex testvertex;
  vertex norg, ndest;
  int killorg;
  triangle ptr;             /* Temporary variable used by sym() and onext(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Loop through all the infected triangles, spreading the virus to */
  /*   their neighbors, then to their neighbors' neighbors.          */
  traversalinit(&m->viri);
  virusloop = (triangle **) traverse(&m->viri);
  while (virusloop != (triangle **) NULL) {
    testtri.tri = *virusloop;
    /* A triangle is marked as infected by messing with one of its pointers */
    /*   to subsegments, setting it to an illegal value.  Hence, we have to */
    /*   temporarily uninfect this triangle so that we can examine its      */
    /*   adjacent subsegments.                                              */
    uninfect(testtri);

    /* Check each of the triangle's three neighbors. */
    for (testtri.orient = 0; testtri.orient < 3; testtri.orient++) {
      /* Find the neighbor. */
      sym(testtri, neighbor);
      /* Check for a subsegment between the triangle and its neighbor. */
      tspivot(testtri, neighborsubseg);
      /* Check if the neighbor is nonexistent or already infected. */
      if ((neighbor.tri == m->dummytri) || infected(neighbor)) {
        if (neighborsubseg.ss != m->dummysub) {
          /* There is a subsegment separating the triangle from its      */
          /*   neighbor, but both triangles are dying, so the subsegment */
          /*   dies too.                                                 */
          subsegdealloc(m, neighborsubseg.ss);
          if (neighbor.tri != m->dummytri) {
            /* Make sure the subsegment doesn't get deallocated again */
            /*   later when the infected neighbor is visited.         */
            uninfect(neighbor);
            tsdissolve(neighbor);
            infect(neighbor);
          }
        }
      } else {                   /* The neighbor exists and is not infected. */
        if (neighborsubseg.ss == m->dummysub) {
          /* There is no subsegment protecting the neighbor, so */
          /*   the neighbor becomes infected.                   */
          infect(neighbor);
          /* Ensure that the neighbor's neighbors will be infected. */
          deadtriangle = (triangle **) poolalloc(&m->viri);
          *deadtriangle = neighbor.tri;
        } else {               /* The neighbor is protected by a subsegment. */
          /* Remove this triangle from the subsegment. */
          stdissolve(neighborsubseg);
          /* The subsegment becomes a boundary.  Set markers accordingly. */
          if (mark(neighborsubseg) == 0) {
            setmark(neighborsubseg, 1);
          }
          org(neighbor, norg);
          dest(neighbor, ndest);
          if (vertexmark(norg) == 0) {
            setvertexmark(norg, 1);
          }
          if (vertexmark(ndest) == 0) {
            setvertexmark(ndest, 1);
          }
        }
      }
    }
    /* Remark the triangle as infected, so it doesn't get added to the */
    /*   virus pool again.                                             */
    infect(testtri);
    virusloop = (triangle **) traverse(&m->viri);
  }

  traversalinit(&m->viri);
  virusloop = (triangle **) traverse(&m->viri);
  while (virusloop != (triangle **) NULL) {
    testtri.tri = *virusloop;

    /* Check each of the three corners of the triangle for elimination. */
    /*   This is done by walking around each vertex, checking if it is  */
    /*   still connected to at least one live triangle.                 */
    for (testtri.orient = 0; testtri.orient < 3; testtri.orient++) {
      org(testtri, testvertex);
      /* Check if the vertex has already been tested. */
      if (testvertex != (vertex) NULL) {
        killorg = 1;
        /* Mark the corner of the triangle as having been tested. */
        setorg(testtri, NULL);
        /* Walk counterclockwise about the vertex. */
        onext(testtri, neighbor);
        /* Stop upon reaching a boundary or the starting triangle. */
        while ((neighbor.tri != m->dummytri) &&
               (!otriequal(neighbor, testtri))) {
          if (infected(neighbor)) {
            /* Mark the corner of this triangle as having been tested. */
            setorg(neighbor, NULL);
          } else {
            /* A live triangle.  The vertex survives. */
            killorg = 0;
          }
          /* Walk counterclockwise about the vertex. */
          onextself(neighbor);
        }
        /* If we reached a boundary, we must walk clockwise as well. */
        if (neighbor.tri == m->dummytri) {
          /* Walk clockwise about the vertex. */
          oprev(testtri, neighbor);
          /* Stop upon reaching a boundary. */
          while (neighbor.tri != m->dummytri) {
            if (infected(neighbor)) {
            /* Mark the corner of this triangle as having been tested. */
              setorg(neighbor, NULL);
            } else {
              /* A live triangle.  The vertex survives. */
              killorg = 0;
            }
            /* Walk clockwise about the vertex. */
            oprevself(neighbor);
          }
        }
        if (killorg) {
          setvertextype(testvertex, UNDEADVERTEX);
          m->undeads++;
        }
      }
    }

    /* Record changes in the number of boundary edges, and disconnect */
    /*   dead triangles from their neighbors.                         */
    for (testtri.orient = 0; testtri.orient < 3; testtri.orient++) {
      sym(testtri, neighbor);
      if (neighbor.tri == m->dummytri) {
        /* There is no neighboring triangle on this edge, so this edge    */
        /*   is a boundary edge.  This triangle is being deleted, so this */
        /*   boundary edge is deleted.                                    */
        m->hullsize--;
      } else {
        /* Disconnect the triangle from its neighbor. */
        dissolve(neighbor);
        /* There is a neighboring triangle on this edge, so this edge */
        /*   becomes a boundary edge when this triangle is deleted.   */
        m->hullsize++;
      }
    }
    /* Return the dead triangle to the pool of triangles. */
    triangledealloc(m, testtri.tri);
    virusloop = (triangle **) traverse(&m->viri);
  }
  /* Empty the virus pool. */
  poolrestart(&m->viri);
}

/*****************************************************************************/
/*                                                                           */
/*  regionplague()   Spread regional attributes and/or area constraints      */
/*                   (from a .poly file) throughout the mesh.                */
/*                                                                           */
/*  This procedure operates in two phases.  The first phase spreads an       */
/*  attribute and/or an area constraint through a (segment-bounded) region.  */
/*  The triangles are marked to ensure that each triangle is added to the    */
/*  virus pool only once, so the procedure will terminate.                   */
/*                                                                           */
/*  The second phase uninfects all infected triangles, returning them to     */
/*  normal.                                                                  */
/*                                                                           */
/*****************************************************************************/

void regionplague(mesh *m, behavior *b,
                  REAL attribute, REAL area)
{
  struct otri testtri;
  struct otri neighbor;
  triangle **virusloop;
  triangle **regiontri;
  struct osub neighborsubseg;
  triangle ptr;             /* Temporary variable used by sym() and onext(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Loop through all the infected triangles, spreading the attribute      */
  /*   and/or area constraint to their neighbors, then to their neighbors' */
  /*   neighbors.                                                          */
  traversalinit(&m->viri);
  virusloop = (triangle **) traverse(&m->viri);
  while (virusloop != (triangle **) NULL) {
    testtri.tri = *virusloop;
    /* A triangle is marked as infected by messing with one of its pointers */
    /*   to subsegments, setting it to an illegal value.  Hence, we have to */
    /*   temporarily uninfect this triangle so that we can examine its      */
    /*   adjacent subsegments.                                              */
    uninfect(testtri);
    if (b->regionattrib) {
      /* Set an attribute. */
      setelemattribute(testtri, m->eextras, attribute);
    }
    if (b->vararea) {
      /* Set an area constraint. */
      setareabound(testtri, area);
    }
    /* Check each of the triangle's three neighbors. */
    for (testtri.orient = 0; testtri.orient < 3; testtri.orient++) {
      /* Find the neighbor. */
      sym(testtri, neighbor);
      /* Check for a subsegment between the triangle and its neighbor. */
      tspivot(testtri, neighborsubseg);
      /* Make sure the neighbor exists, is not already infected, and */
      /*   isn't protected by a subsegment.                          */
      if ((neighbor.tri != m->dummytri) && !infected(neighbor)
          && (neighborsubseg.ss == m->dummysub)) {
        /* Infect the neighbor. */
        infect(neighbor);
        /* Ensure that the neighbor's neighbors will be infected. */
        regiontri = (triangle **) poolalloc(&m->viri);
        *regiontri = neighbor.tri;
      }
    }
    /* Remark the triangle as infected, so it doesn't get added to the */
    /*   virus pool again.                                             */
    infect(testtri);
    virusloop = (triangle **) traverse(&m->viri);
  }

  /* Uninfect all triangles. */
  traversalinit(&m->viri);
  virusloop = (triangle **) traverse(&m->viri);
  while (virusloop != (triangle **) NULL) {
    testtri.tri = *virusloop;
    uninfect(testtri);
    virusloop = (triangle **) traverse(&m->viri);
  }
  /* Empty the virus pool. */
  poolrestart(&m->viri);
}

/*****************************************************************************/
/*                                                                           */
/*  carveholes()   Find the holes and infect them.  Find the area            */
/*                 constraints and infect them.  Infect the convex hull.     */
/*                 Spread the infection and kill triangles.  Spread the      */
/*                 area constraints.                                         */
/*                                                                           */
/*  This routine mainly calls other routines to carry out all these          */
/*  functions.                                                               */
/*                                                                           */
/*****************************************************************************/

void carveholes(mesh *m, behavior *b, REAL *holelist, int holes,
                REAL *regionlist, int regions)
{
  struct otri searchtri;
  struct otri triangleloop;
  struct otri *regiontris;
  triangle **holetri;
  triangle **regiontri;
  vertex searchorg, searchdest;
  enum locateresult intersect;
  int i;
  triangle ptr;                         /* Temporary variable used by sym(). */

  if (regions > 0) {
    /* Allocate storage for the triangles in which region points fall. */
    regiontris = (struct otri *) trimalloc(regions *
                                           (int) sizeof(struct otri));
  } else {
    regiontris = (struct otri *) NULL;
  }

  if (((holes > 0) && !b->noholes) || !b->convex || (regions > 0)) {
    /* Initialize a pool of viri to be used for holes, concavities, */
    /*   regional attributes, and/or regional area constraints.     */
    poolinit(&m->viri, sizeof(triangle *), VIRUSPERBLOCK, VIRUSPERBLOCK, 0);
  }

  if (!b->convex) {
    /* Mark as infected any unprotected triangles on the boundary. */
    /*   This is one way by which concavities are created.         */
    infecthull(m, b);
  }

  if ((holes > 0) && !b->noholes) {
    /* Infect each triangle in which a hole lies. */
    for (i = 0; i < 2 * holes; i += 2) {
      /* Ignore holes that aren't within the bounds of the mesh. */
      if ((holelist[i] >= m->xmin) && (holelist[i] <= m->xmax)
          && (holelist[i + 1] >= m->ymin) && (holelist[i + 1] <= m->ymax)) {
        /* Start searching from some triangle on the outer boundary. */
        searchtri.tri = m->dummytri;
        searchtri.orient = 0;
        symself(searchtri);
        /* Ensure that the hole is to the left of this boundary edge; */
        /*   otherwise, locate() will falsely report that the hole    */
        /*   falls within the starting triangle.                      */
        org(searchtri, searchorg);
        dest(searchtri, searchdest);
        if (counterclockwise(m, b, searchorg, searchdest, &holelist[i]) >
            0.0) {
          /* Find a triangle that contains the hole. */
          intersect = locate(m, b, &holelist[i], &searchtri);
          if ((intersect != OUTSIDE) && (!infected(searchtri))) {
            /* Infect the triangle.  This is done by marking the triangle  */
            /*   as infected and including the triangle in the virus pool. */
            infect(searchtri);
            holetri = (triangle **) poolalloc(&m->viri);
            *holetri = searchtri.tri;
          }
        }
      }
    }
  }

  /* Now, we have to find all the regions BEFORE we carve the holes, because */
  /*   locate() won't work when the triangulation is no longer convex.       */
  /*   (Incidentally, this is the reason why regional attributes and area    */
  /*   constraints can't be used when refining a preexisting mesh, which     */
  /*   might not be convex; they can only be used with a freshly             */
  /*   triangulated PSLG.)                                                   */
  if (regions > 0) {
    /* Find the starting triangle for each region. */
    for (i = 0; i < regions; i++) {
      regiontris[i].tri = m->dummytri;
      /* Ignore region points that aren't within the bounds of the mesh. */
      if ((regionlist[4 * i] >= m->xmin) && (regionlist[4 * i] <= m->xmax) &&
          (regionlist[4 * i + 1] >= m->ymin) &&
          (regionlist[4 * i + 1] <= m->ymax)) {
        /* Start searching from some triangle on the outer boundary. */
        searchtri.tri = m->dummytri;
        searchtri.orient = 0;
        symself(searchtri);
        /* Ensure that the region point is to the left of this boundary */
        /*   edge; otherwise, locate() will falsely report that the     */
        /*   region point falls within the starting triangle.           */
        org(searchtri, searchorg);
        dest(searchtri, searchdest);
        if (counterclockwise(m, b, searchorg, searchdest, &regionlist[4 * i]) >
            0.0) {
          /* Find a triangle that contains the region point. */
          intersect = locate(m, b, &regionlist[4 * i], &searchtri);
          if ((intersect != OUTSIDE) && (!infected(searchtri))) {
            /* Record the triangle for processing after the */
            /*   holes have been carved.                    */
            otricopy(searchtri, regiontris[i]);
          }
        }
      }
    }
  }

  if (m->viri.items > 0) {
    /* Carve the holes and concavities. */
    plague(m, b);
  }
  /* The virus pool should be empty now. */

  if (regions > 0) {
    if (b->regionattrib && !b->refine) {
      /* Assign every triangle a regional attribute of zero. */
      traversalinit(&m->triangles);
      triangleloop.orient = 0;
      triangleloop.tri = triangletraverse(m);
      while (triangleloop.tri != (triangle *) NULL) {
        setelemattribute(triangleloop, m->eextras, 0.0);
        triangleloop.tri = triangletraverse(m);
      }
    }
    for (i = 0; i < regions; i++) {
      if (regiontris[i].tri != m->dummytri) {
        /* Make sure the triangle under consideration still exists. */
        /*   It may have been eaten by the virus.                   */
        if (!deadtri(regiontris[i].tri)) {
          /* Put one triangle in the virus pool. */
          infect(regiontris[i]);
          regiontri = (triangle **) poolalloc(&m->viri);
          *regiontri = regiontris[i].tri;
          /* Apply one region's attribute and/or area constraint. */
          regionplague(m, b, regionlist[4 * i + 2], regionlist[4 * i + 3]);
          /* The virus pool should be empty now. */
        }
      }
    }
    if (b->regionattrib && !b->refine) {
      /* Note the fact that each triangle has an additional attribute. */
      m->eextras++;
    }
  }

  /* Free up memory. */
  if (((holes > 0) && !b->noholes) || !b->convex || (regions > 0)) {
    pooldeinit(&m->viri);
  }
  if (regions > 0) {
    trifree((VOID *) regiontris);
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Carving out holes and concavities ends here               *********/

/********* Mesh quality maintenance begins here                      *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  tallyencs()   Traverse the entire list of subsegments, and check each    */
/*                to see if it is encroached.  If so, add it to the list.    */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void tallyencs(mesh *m, behavior *b)
{
  struct osub subsegloop;
  int dummy;

  traversalinit(&m->subsegs);
  subsegloop.ssorient = 0;
  subsegloop.ss = subsegtraverse(m);
  while (subsegloop.ss != (subseg *) NULL) {
    /* If the segment is encroached, add it to the list. */
    dummy = checkseg4encroach(m, b, &subsegloop);
    subsegloop.ss = subsegtraverse(m);
  }
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  precisionerror()  Print an error message for precision problems.         */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void precisionerror()
{
  printf("Try increasing the area criterion and/or reducing the minimum\n");
  printf("  allowable angle so that tiny triangles are not created.\n");
#ifdef SINGLE
  printf("Alternatively, try recompiling me with double precision\n");
  printf("  arithmetic (by removing \"#define SINGLE\" from the\n");
  printf("  source file or \"-DSINGLE\" from the makefile).\n");
#endif /* SINGLE */
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  splitencsegs()   Split all the encroached subsegments.                   */
/*                                                                           */
/*  Each encroached subsegment is repaired by splitting it - inserting a     */
/*  vertex at or near its midpoint.  Newly inserted vertices may encroach    */
/*  upon other subsegments; these are also repaired.                         */
/*                                                                           */
/*  `triflaws' is a flag that specifies whether one should take note of new  */
/*  bad triangles that result from inserting vertices to repair encroached   */
/*  subsegments.                                                             */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void splitencsegs(mesh *m, behavior *b, int triflaws, int *status)
{
  struct otri enctri;
  struct otri testtri;
  struct osub testsh;
  struct osub currentenc;
  struct badsubseg *encloop;
  vertex eorg, edest, eapex;
  vertex newvertex;
  enum insertvertexresult success;
  REAL segmentlength, nearestpoweroftwo;
  REAL split;
  REAL multiplier, divisor;
  int acuteorg, acuteorg2, acutedest, acutedest2;
  int dummy;
  int i;
  triangle ptr;                     /* Temporary variable used by stpivot(). */
  subseg sptr;                        /* Temporary variable used by snext(). */

  /* Note that steinerleft == -1 if an unlimited number */
  /*   of Steiner points is allowed.                    */
  while ((m->badsubsegs.items > 0) && (m->steinerleft != 0)) {
    traversalinit(&m->badsubsegs);
    encloop = badsubsegtraverse(m);
    while ((encloop != (struct badsubseg *) NULL) && (m->steinerleft != 0)) {
      sdecode(encloop->encsubseg, currentenc);
      sorg(currentenc, eorg);
      sdest(currentenc, edest);
      /* Make sure that this segment is still the same segment it was   */
      /*   when it was determined to be encroached.  If the segment was */
      /*   enqueued multiple times (because several newly inserted      */
      /*   vertices encroached it), it may have already been split.     */
      if (!deadsubseg(currentenc.ss) &&
          (eorg == encloop->subsegorg) && (edest == encloop->subsegdest)) {
        /* To decide where to split a segment, we need to know if the   */
        /*   segment shares an endpoint with an adjacent segment.       */
        /*   The concern is that, if we simply split every encroached   */
        /*   segment in its center, two adjacent segments with a small  */
        /*   angle between them might lead to an infinite loop; each    */
        /*   vertex added to split one segment will encroach upon the   */
        /*   other segment, which must then be split with a vertex that */
        /*   will encroach upon the first segment, and so on forever.   */
        /* To avoid this, imagine a set of concentric circles, whose    */
        /*   radii are powers of two, about each segment endpoint.      */
        /*   These concentric circles determine where the segment is    */
        /*   split.  (If both endpoints are shared with adjacent        */
        /*   segments, split the segment in the middle, and apply the   */
        /*   concentric circles for later splittings.)                  */

        /* Is the origin shared with another segment? */
        stpivot(currentenc, enctri);
        lnext(enctri, testtri);
        tspivot(testtri, testsh);
        acuteorg = testsh.ss != m->dummysub;
        /* Is the destination shared with another segment? */
        lnextself(testtri);
        tspivot(testtri, testsh);
        acutedest = testsh.ss != m->dummysub;

        /* If we're using Chew's algorithm (rather than Ruppert's) */
        /*   to define encroachment, delete free vertices from the */
        /*   subsegment's diametral circle.                        */
        if (!b->conformdel && !acuteorg && !acutedest) {
          apex(enctri, eapex);
          while ((vertextype(eapex) == FREEVERTEX) &&
                 ((eorg[0] - eapex[0]) * (edest[0] - eapex[0]) +
                  (eorg[1] - eapex[1]) * (edest[1] - eapex[1]) < 0.0)) {
            deletevertex(m, b, &testtri);
            stpivot(currentenc, enctri);
            apex(enctri, eapex);
            lprev(enctri, testtri);
          }
        }

        /* Now, check the other side of the segment, if there's a triangle */
        /*   there.                                                        */
        sym(enctri, testtri);
        if (testtri.tri != m->dummytri) {
          /* Is the destination shared with another segment? */
          lnextself(testtri);
          tspivot(testtri, testsh);
          acutedest2 = testsh.ss != m->dummysub;
          acutedest = acutedest || acutedest2;
          /* Is the origin shared with another segment? */
          lnextself(testtri);
          tspivot(testtri, testsh);
          acuteorg2 = testsh.ss != m->dummysub;
          acuteorg = acuteorg || acuteorg2;

          /* Delete free vertices from the subsegment's diametral circle. */
          if (!b->conformdel && !acuteorg2 && !acutedest2) {
            org(testtri, eapex);
            while ((vertextype(eapex) == FREEVERTEX) &&
                   ((eorg[0] - eapex[0]) * (edest[0] - eapex[0]) +
                    (eorg[1] - eapex[1]) * (edest[1] - eapex[1]) < 0.0)) {
              deletevertex(m, b, &testtri);
              sym(enctri, testtri);
              apex(testtri, eapex);
              lprevself(testtri);
            }
          }
        }

        /* Use the concentric circles if exactly one endpoint is shared */
        /*   with another adjacent segment.                             */
        if (acuteorg || acutedest) {
          segmentlength = sqrt((edest[0] - eorg[0]) * (edest[0] - eorg[0]) +
                               (edest[1] - eorg[1]) * (edest[1] - eorg[1]));
          /* Find the power of two that most evenly splits the segment.  */
          /*   The worst case is a 2:1 ratio between subsegment lengths. */
          nearestpoweroftwo = 1.0;
          while (segmentlength > 3.0 * nearestpoweroftwo) {
            nearestpoweroftwo *= 2.0;
          }
          while (segmentlength < 1.5 * nearestpoweroftwo) {
            nearestpoweroftwo *= 0.5;
          }
          /* Where do we split the segment? */
          split = nearestpoweroftwo / segmentlength;
          if (acutedest) {
            split = 1.0 - split;
          }
        } else {
          /* If we're not worried about adjacent segments, split */
          /*   this segment in the middle.                       */
          split = 0.5;
        }

        /* Create the new vertex. */
        newvertex = (vertex) poolalloc(&m->vertices);
        /* Interpolate its coordinate and attributes. */
        for (i = 0; i < 2 + m->nextras; i++) {
          newvertex[i] = eorg[i] + split * (edest[i] - eorg[i]);
        }

        if (!b->noexact) {
          /* Roundoff in the above calculation may yield a `newvertex'   */
          /*   that is not precisely collinear with `eorg' and `edest'.  */
          /*   Improve collinearity by one step of iterative refinement. */
          multiplier = counterclockwise(m, b, eorg, edest, newvertex);
          divisor = ((eorg[0] - edest[0]) * (eorg[0] - edest[0]) +
                     (eorg[1] - edest[1]) * (eorg[1] - edest[1]));
          if ((multiplier != 0.0) && (divisor != 0.0)) {
            multiplier = multiplier / divisor;
            /* Watch out for NANs. */
            if (multiplier == multiplier) {
              newvertex[0] += multiplier * (edest[1] - eorg[1]);
              newvertex[1] += multiplier * (eorg[0] - edest[0]);
            }
          }
        }

        setvertexmark(newvertex, mark(currentenc));
        setvertextype(newvertex, SEGMENTVERTEX);
        /* Check whether the new vertex lies on an endpoint. */
        if (((newvertex[0] == eorg[0]) && (newvertex[1] == eorg[1])) ||
            ((newvertex[0] == edest[0]) && (newvertex[1] == edest[1]))) {
          // TODO: Precision error: splitencsegs()
          // Ran out of precision at (newvertex).
          *status = TRI_SEG_SPLIT;
		  return;
        }
        /* Insert the splitting vertex.  This should always succeed. */
        success = insertvertex(m, b, newvertex, &enctri, &currentenc,
                               1, triflaws, 0);
        if ((success != SUCCESSFULVERTEX) && (success != ENCROACHINGVERTEX)) {
          // TODO: Internal error: splitencsegs()
          // Failure to split a segment.
          *status = TRI_SEG_SPLIT;
		  return;
        }
        if (m->steinerleft > 0) {
          m->steinerleft--;
        }
        /* Check the two new subsegments to see if they're encroached. */
        dummy = checkseg4encroach(m, b, &currentenc);
        snextself(currentenc);
        dummy = checkseg4encroach(m, b, &currentenc);
      }

      badsubsegdealloc(m, encloop);
      encloop = badsubsegtraverse(m);
    }
  }
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  tallyfaces()   Test every triangle in the mesh for quality measures.     */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void tallyfaces(mesh *m, behavior *b)
{
  struct otri triangleloop;

  traversalinit(&m->triangles);
  triangleloop.orient = 0;
  triangleloop.tri = triangletraverse(m);
  while (triangleloop.tri != (triangle *) NULL) {
    /* If the triangle is bad, enqueue it. */
    testtriangle(m, b, &triangleloop);
    triangleloop.tri = triangletraverse(m);
  }
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  splittriangle()   Inserts a vertex at the circumcenter of a triangle.    */
/*                    Deletes the newly inserted vertex if it encroaches     */
/*                    upon a segment.                                        */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void splittriangle(mesh *m, behavior *b,
                   struct badtriang *badtri)
{
  struct otri badotri;
  vertex borg, bdest, bapex;
  vertex newvertex;
  REAL xi, eta;
  enum insertvertexresult success;
  int errorflag;

  decode(badtri->poortri, badotri);
  org(badotri, borg);
  dest(badotri, bdest);
  apex(badotri, bapex);
  /* Make sure that this triangle is still the same triangle it was      */
  /*   when it was tested and determined to be of bad quality.           */
  /*   Subsequent transformations may have made it a different triangle. */
  if (!deadtri(badotri.tri) && (borg == badtri->triangorg) &&
      (bdest == badtri->triangdest) && (bapex == badtri->triangapex)) {

    errorflag = 0;
    /* Create a new vertex at the triangle's circumcenter. */
    newvertex = (vertex) poolalloc(&m->vertices);
#ifndef NO_ACUTE
    if (b->fixedarea || b->vararea || b->usertest) {
      // TODO: Acute fails for certain area constraints.
      findcircumcenter(m, b, borg, bdest, bapex, newvertex, &xi, &eta, 1);
    } else {
      findNewSPLocation(m, b, borg, bdest, bapex, newvertex, &xi, &eta, 1, badotri);
    }
#else
    findcircumcenter(m, b, borg, bdest, bapex, newvertex, &xi, &eta, 1);
#endif

    /* Check whether the new vertex lies on a triangle vertex. */
    if (((newvertex[0] == borg[0]) && (newvertex[1] == borg[1])) ||
        ((newvertex[0] == bdest[0]) && (newvertex[1] == bdest[1])) ||
        ((newvertex[0] == bapex[0]) && (newvertex[1] == bapex[1]))) {
#ifdef _DEBUG
      printf(
           "Warning:  New vertex (%.12g, %.12g) falls on existing vertex.\n",
             newvertex[0], newvertex[1]);
#endif
      errorflag = 1;
      vertexdealloc(m, newvertex);
    } else {
      /* Interpolation of vertex attributes is done in insertvertex method. */

      /* The new vertex must be in the interior, and therefore is a */
      /*   free vertex with a marker of zero.                       */
      setvertexmark(newvertex, 0);
      setvertextype(newvertex, FREEVERTEX);

      /* Ensure that the handle `badotri' does not represent the longest  */
      /*   edge of the triangle.  This ensures that the circumcenter must */
      /*   fall to the left of this edge, so point location will work.    */
      /*   (If the angle org-apex-dest exceeds 90 degrees, then the       */
      /*   circumcenter lies outside the org-dest edge, and eta is        */
      /*   negative.  Roundoff error might prevent eta from being         */
      /*   negative when it should be, so I test eta against xi.)         */
      if (eta < xi) {
        lprevself(badotri);
      }

      /* Insert the circumcenter, searching from the edge of the triangle, */
      /*   and maintain the Delaunay property of the triangulation.        */
      success = insertvertex(m, b, newvertex, &badotri, (struct osub *) NULL,
                             1, 1, 1);
      if (success == SUCCESSFULVERTEX) {
        if (m->steinerleft > 0) {
          m->steinerleft--;
        }
      } else if (success == ENCROACHINGVERTEX) {
        /* If the newly inserted vertex encroaches upon a subsegment, */
        /*   delete the new vertex.                                   */
        undovertex(m, b);
        vertexdealloc(m, newvertex);
      } else if (success == VIOLATINGVERTEX) {
        /* Failed to insert the new vertex, but some subsegment was */
        /*   marked as being encroached.                            */
        vertexdealloc(m, newvertex);
      } else {                                 /* success == DUPLICATEVERTEX */
        /* Couldn't insert the new vertex because a vertex is already there. */
#ifdef _DEBUG
        printf(
          "Warning:  New vertex (%.12g, %.12g) falls on existing vertex.\n",
               newvertex[0], newvertex[1]);
#endif
        errorflag = 1;
        vertexdealloc(m, newvertex);
      }
    }
    if (errorflag) {
#ifdef _DEBUG
      printf("This probably means that I am trying to refine triangles\n");
      printf("  to a smaller size than can be accommodated by the finite\n");
      printf("  precision of floating point arithmetic.  (You can be\n");
      printf("  sure of this if I fail to terminate.)\n");
      precisionerror();
#endif
    }
  }
}

#endif /* not CDT_ONLY */

/*****************************************************************************/
/*                                                                           */
/*  enforcequality()   Remove all the encroached subsegments and bad         */
/*                     triangles from the triangulation.                     */
/*                                                                           */
/*****************************************************************************/

#ifndef CDT_ONLY

void enforcequality(mesh *m, behavior *b, int *status)
{
  struct badtriang *badtri;
  int i;

  /* Initialize the pool of encroached subsegments. */
  poolinit(&m->badsubsegs, sizeof(struct badsubseg), BADSUBSEGPERBLOCK,
           BADSUBSEGPERBLOCK, 0);
  
#ifndef NO_ACUTE
  if (m->acute_mem == (acutepool *)NULL) {
    acutepool_init(20, &m->acute_mem);
  }
#endif

  /* Test all segments to see if they're encroached. */
  tallyencs(m, b);

  /* Fix encroached subsegments without noting bad triangles. */
  splitencsegs(m, b, 0, status);
  if (*status < 0) return;

  /* At this point, if we haven't run out of Steiner points, the */
  /*   triangulation should be (conforming) Delaunay.            */

  /* Next, we worry about enforcing triangle quality. */
  if ((b->minangle > 0.0) || b->vararea || b->fixedarea || b->usertest) {
    /* Initialize the pool of bad triangles. */
    poolinit(&m->badtriangles, sizeof(struct badtriang), BADTRIPERBLOCK,
             BADTRIPERBLOCK, 0);
    /* Initialize the queues of bad triangles. */
    for (i = 0; i < 4096; i++) {
      m->queuefront[i] = (struct badtriang *) NULL;
    }
    m->firstnonemptyq = -1;
    /* Test all triangles to see if they're bad. */
    tallyfaces(m, b);
    /* Initialize the pool of recently flipped triangles. */
    poolinit(&m->flipstackers, sizeof(struct flipstacker), FLIPSTACKERPERBLOCK,
             FLIPSTACKERPERBLOCK, 0);
    m->checkquality = 1;

    while ((m->badtriangles.items > 0) && (m->steinerleft != 0)) {
      /* Fix one bad triangle by inserting a vertex at its circumcenter. */
      badtri = dequeuebadtriang(m);
      splittriangle(m, b, badtri);
      if (m->badsubsegs.items > 0) {
        /* Put bad triangle back in queue for another try later. */
        enqueuebadtriang(m, b, badtri);
        /* Fix any encroached subsegments that resulted. */
        /*   Record any new bad triangles that result.   */
        splitencsegs(m, b, 1, status);
        if (*status < 0) return;
      } else {
        /* Return the bad triangle to the pool. */
        pooldealloc(&m->badtriangles, (VOID *) badtri);
      }
    }
  }
  /* At this point, if the "-D" switch was selected and we haven't run out  */
  /*   of Steiner points, the triangulation should be (conforming) Delaunay */
  /*   and have no low-quality triangles.                                   */

  /* Might we have run out of Steiner points too soon? */
#ifdef _DEBUG
  if (b->conformdel && (m->badsubsegs.items > 0) && (m->steinerleft == 0)) {
    printf("\nWarning:  I ran out of Steiner points, but the mesh has\n");
    if (m->badsubsegs.items == 1) {
      printf("  one encroached subsegment, and therefore might not be truly\n"
             );
    } else {
      printf("  %ld encroached subsegments, and therefore might not be truly\n"
             , m->badsubsegs.items);
    }
    printf("  Delaunay.  If the Delaunay property is important to you,\n");
    printf("  try increasing the number of Steiner points (controlled by\n");
    printf("  the -S switch) slightly and try again.\n\n");
  }
#endif
}

#endif /* not CDT_ONLY */

/**                                                                         **/
/**                                                                         **/
/********* Mesh quality maintenance ends here                        *********/

/*****************************************************************************/
/*                                                                           */
/*  highorder()   Create extra nodes for quadratic subparametric elements.   */
/*                                                                           */
/*****************************************************************************/

void highorder(mesh *m, behavior *b)
{
  struct otri triangleloop, trisym;
  struct osub checkmark;
  vertex newvertex;
  vertex torg, tdest;
  int i;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* The following line ensures that dead items in the pool of nodes    */
  /*   cannot be allocated for the extra nodes associated with high     */
  /*   order elements.  This ensures that the primary nodes (at the     */
  /*   corners of elements) will occur earlier in the output files, and */
  /*   have lower indices, than the extra nodes.                        */
  m->vertices.deaditemstack = (VOID *) NULL;

  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  /* To loop over the set of edges, loop over all triangles, and look at   */
  /*   the three edges of each triangle.  If there isn't another triangle  */
  /*   adjacent to the edge, operate on the edge.  If there is another     */
  /*   adjacent triangle, operate on the edge only if the current triangle */
  /*   has a smaller pointer than its neighbor.  This way, each edge is    */
  /*   considered only once.                                               */
  while (triangleloop.tri != (triangle *) NULL) {
    for (triangleloop.orient = 0; triangleloop.orient < 3;
         triangleloop.orient++) {
      sym(triangleloop, trisym);
      if ((triangleloop.tri < trisym.tri) || (trisym.tri == m->dummytri)) {
        org(triangleloop, torg);
        dest(triangleloop, tdest);
        /* Create a new node in the middle of the edge.  Interpolate */
        /*   its attributes.                                         */
        newvertex = (vertex) poolalloc(&m->vertices);
        for (i = 0; i < 2 + m->nextras; i++) {
          newvertex[i] = 0.5 * (torg[i] + tdest[i]);
        }
        /* Set the new node's marker to zero or one, depending on */
        /*   whether it lies on a boundary.                       */
        setvertexmark(newvertex, trisym.tri == m->dummytri);
        setvertextype(newvertex,
                      trisym.tri == m->dummytri ? FREEVERTEX : SEGMENTVERTEX);
        if (b->usesegments) {
          tspivot(triangleloop, checkmark);
          /* If this edge is a segment, transfer the marker to the new node. */
          if (checkmark.ss != m->dummysub) {
            setvertexmark(newvertex, mark(checkmark));
            setvertextype(newvertex, SEGMENTVERTEX);
          }
        }
        /* Record the new node in the (one or two) adjacent elements. */
        triangleloop.tri[m->highorderindex + triangleloop.orient] =
                (triangle) newvertex;
        if (trisym.tri != m->dummytri) {
          trisym.tri[m->highorderindex + trisym.orient] = (triangle) newvertex;
        }
      }
    }
    triangleloop.tri = triangletraverse(m);
  }
}

/********* Array I/O routines begin here                             *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  transfernodes()   Read the vertices from memory.                         */
/*                                                                           */
/*****************************************************************************/

int transfernodes(mesh *m, behavior *b, REAL *pointlist,
                   REAL *pointattriblist, int *pointmarkerlist,
                   int numberofpoints, int numberofpointattribs)
{
  vertex vertexloop;
  REAL x, y;
  int i, j;
  int coordindex;
  int attribindex;

  m->invertices = numberofpoints;
  m->mesh_dim = 2;
  m->nextras = numberofpointattribs;
  if (m->invertices < 3) {
    /* Error:  Input must have at least three input vertices. */
    return TRI_INPUT;
  }
  if (m->nextras == 0) {
    b->weighted = 0;
  }

  initializevertexpool(m, b);

  /* Read the vertices. */
  coordindex = 0;
  attribindex = 0;
  for (i = 0; i < m->invertices; i++) {
    vertexloop = (vertex) poolalloc(&m->vertices);
    /* Read the vertex coordinates. */
    x = vertexloop[0] = pointlist[coordindex++];
    y = vertexloop[1] = pointlist[coordindex++];
    /* Read the vertex attributes. */
    for (j = 0; j < numberofpointattribs; j++) {
      vertexloop[2 + j] = pointattriblist[attribindex++];
    }
    if (pointmarkerlist != (int *) NULL) {
      /* Read a vertex marker. */
      setvertexmark(vertexloop, pointmarkerlist[i]);
    } else {
      /* If no markers are specified, they default to zero. */
      setvertexmark(vertexloop, 0);
    }
    setvertextype(vertexloop, INPUTVERTEX);
    /* Determine the smallest and largest x and y coordinates. */
    if (i == 0) {
      m->xmin = m->xmax = x;
      m->ymin = m->ymax = y;
    } else {
      m->xmin = (x < m->xmin) ? x : m->xmin;
      m->xmax = (x > m->xmax) ? x : m->xmax;
      m->ymin = (y < m->ymin) ? y : m->ymin;
      m->ymax = (y > m->ymax) ? y : m->ymax;
    }
  }

  /* Nonexistent x value used as a flag to mark circle events in sweepline */
  /*   Delaunay algorithm.                                                 */
  m->xminextreme = 10 * m->xmin - 9 * m->xmax;

  return TRI_OK;
}

/*****************************************************************************/
/*                                                                           */
/*  writenodes()   Number the vertices and write them to arrays.             */
/*                                                                           */
/*  To save memory, the vertex numbers are written over the boundary markers */
/*  after the vertices are written to a file.                                */
/*                                                                           */
/*****************************************************************************/

void writenodes(mesh *m, behavior *b, REAL **pointlist,
                REAL **pointattriblist, int **pointmarkerlist)
{
  REAL *plist;
  REAL *palist;
  int *pmlist;
  int coordindex;
  int attribindex;
  vertex vertexloop;
  long outvertices;
  int vertexnumber;
  int i;

  if (b->jettison) {
    outvertices = m->vertices.items - m->undeads;
  } else {
    outvertices = m->vertices.items;
  }

  /* Allocate memory for output vertices if necessary. */
  if (*pointlist == (REAL *) NULL) {
    *pointlist = (REAL *) trimalloc((int) (outvertices * 2 * sizeof(REAL)));
  }
  /* Allocate memory for output vertex attributes if necessary. */
  if ((m->nextras > 0) && (*pointattriblist == (REAL *) NULL)) {
    *pointattriblist = (REAL *) trimalloc((int) (outvertices * m->nextras *
                                                 sizeof(REAL)));
  }
  /* Allocate memory for output vertex markers if necessary. */
  if (!b->nobound && (*pointmarkerlist == (int *) NULL)) {
    *pointmarkerlist = (int *) trimalloc((int) (outvertices * sizeof(int)));
  }
  plist = *pointlist;
  palist = *pointattriblist;
  pmlist = *pointmarkerlist;
  coordindex = 0;
  attribindex = 0;

  traversalinit(&m->vertices);
  vertexnumber = b->firstnumber;
  vertexloop = vertextraverse(m);
  while (vertexloop != (vertex) NULL) {
    if (!b->jettison || (vertextype(vertexloop) != UNDEADVERTEX)) {
      /* X and y coordinates. */
      plist[coordindex++] = vertexloop[0];
      plist[coordindex++] = vertexloop[1];
      /* Vertex attributes. */
      for (i = 0; i < m->nextras; i++) {
        palist[attribindex++] = vertexloop[2 + i];
      }
      if (!b->nobound) {
        /* Copy the boundary marker. */
        pmlist[vertexnumber - b->firstnumber] = vertexmark(vertexloop);
      }

      setvertexmark(vertexloop, vertexnumber);
      vertexnumber++;
    }
    vertexloop = vertextraverse(m);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  numbernodes()   Number the vertices.                                     */
/*                                                                           */
/*  Each vertex is assigned a marker equal to its number.                    */
/*                                                                           */
/*  Used when writenodes() is not called because no .node file is written.   */
/*                                                                           */
/*****************************************************************************/

void numbernodes(mesh *m, behavior *b)
{
  vertex vertexloop;
  int vertexnumber;

  traversalinit(&m->vertices);
  vertexnumber = b->firstnumber;
  vertexloop = vertextraverse(m);
  while (vertexloop != (vertex) NULL) {
    setvertexmark(vertexloop, vertexnumber);
    if (!b->jettison || (vertextype(vertexloop) != UNDEADVERTEX)) {
      vertexnumber++;
    }
    vertexloop = vertextraverse(m);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  writeelements()   Write the triangles to arrays.                         */
/*                                                                           */
/*****************************************************************************/

void writeelements(mesh *m, behavior *b,
                   int **trianglelist, REAL **triangleattriblist)
{
  int *tlist;
  REAL *talist;
  int vertexindex;
  int attribindex;
  struct otri triangleloop;
  vertex p1, p2, p3;
  vertex mid1, mid2, mid3;
  long elementnumber;
  int i;

  /* Allocate memory for output triangles if necessary. */
  if (*trianglelist == (int *) NULL) {
    *trianglelist = (int *) trimalloc((int) (m->triangles.items *
                                             ((b->order + 1) * (b->order + 2) /
                                              2) * sizeof(int)));
  }
  /* Allocate memory for output triangle attributes if necessary. */
  if ((m->eextras > 0) && (*triangleattriblist == (REAL *) NULL)) {
    *triangleattriblist = (REAL *) trimalloc((int) (m->triangles.items *
                                                    m->eextras *
                                                    sizeof(REAL)));
  }
  tlist = *trianglelist;
  talist = *triangleattriblist;
  vertexindex = 0;
  attribindex = 0;

  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  triangleloop.orient = 0;
  elementnumber = b->firstnumber;
  while (triangleloop.tri != (triangle *) NULL) {
    org(triangleloop, p1);
    dest(triangleloop, p2);
    apex(triangleloop, p3);
    if (b->order == 1) {
      tlist[vertexindex++] = vertexmark(p1);
      tlist[vertexindex++] = vertexmark(p2);
      tlist[vertexindex++] = vertexmark(p3);
    } else {
      mid1 = (vertex) triangleloop.tri[m->highorderindex + 1];
      mid2 = (vertex) triangleloop.tri[m->highorderindex + 2];
      mid3 = (vertex) triangleloop.tri[m->highorderindex];

      tlist[vertexindex++] = vertexmark(p1);
      tlist[vertexindex++] = vertexmark(p2);
      tlist[vertexindex++] = vertexmark(p3);
      tlist[vertexindex++] = vertexmark(mid1);
      tlist[vertexindex++] = vertexmark(mid2);
      tlist[vertexindex++] = vertexmark(mid3);
    }

    for (i = 0; i < m->eextras; i++) {
      talist[attribindex++] = elemattribute(triangleloop, i);
    }

    triangleloop.tri = triangletraverse(m);
    elementnumber++;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  writepoly()   Write the segments and holes to arrays.                    */
/*                                                                           */
/*****************************************************************************/

void writepoly(mesh *m, behavior *b,
               int **segmentlist, int **segmentmarkerlist)
{
  int *slist;
  int *smlist;
  int index;
  struct osub subsegloop;
  vertex endpoint1, endpoint2;
  long subsegnumber;

  /* Allocate memory for output segments if necessary. */
  if (*segmentlist == (int *) NULL) {
    *segmentlist = (int *) trimalloc((int) (m->subsegs.items * 2 *
                                            sizeof(int)));
  }
  /* Allocate memory for output segment markers if necessary. */
  if (!b->nobound && (*segmentmarkerlist == (int *) NULL)) {
    *segmentmarkerlist = (int *) trimalloc((int) (m->subsegs.items *
                                                  sizeof(int)));
  }
  slist = *segmentlist;
  smlist = *segmentmarkerlist;
  index = 0;

  traversalinit(&m->subsegs);
  subsegloop.ss = subsegtraverse(m);
  subsegloop.ssorient = 0;
  subsegnumber = b->firstnumber;
  while (subsegloop.ss != (subseg *) NULL) {
    sorg(subsegloop, endpoint1);
    sdest(subsegloop, endpoint2);
    /* Copy indices of the segment's two endpoints. */
    slist[index++] = vertexmark(endpoint1);
    slist[index++] = vertexmark(endpoint2);
    if (!b->nobound) {
      /* Copy the boundary marker. */
      smlist[subsegnumber - b->firstnumber] = mark(subsegloop);
    }

    subsegloop.ss = subsegtraverse(m);
    subsegnumber++;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  writeedges()   Write the edges to arrays.                                */
/*                                                                           */
/*****************************************************************************/

void writeedges(mesh *m, behavior *b,
                int **edgelist, int **edgemarkerlist)
{
  int *elist;
  int *emlist;
  int index;
  struct otri triangleloop, trisym;
  struct osub checkmark;
  vertex p1, p2;
  long edgenumber;
  triangle ptr;                         /* Temporary variable used by sym(). */
  subseg sptr;                      /* Temporary variable used by tspivot(). */

  /* Allocate memory for edges if necessary. */
  if (*edgelist == (int *) NULL) {
    *edgelist = (int *) trimalloc((int) (m->edges * 2 * sizeof(int)));
  }
  /* Allocate memory for edge markers if necessary. */
  if (!b->nobound && (*edgemarkerlist == (int *) NULL)) {
    *edgemarkerlist = (int *) trimalloc((int) (m->edges * sizeof(int)));
  }
  elist = *edgelist;
  emlist = *edgemarkerlist;
  index = 0;

  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  edgenumber = b->firstnumber;
  /* To loop over the set of edges, loop over all triangles, and look at   */
  /*   the three edges of each triangle.  If there isn't another triangle  */
  /*   adjacent to the edge, operate on the edge.  If there is another     */
  /*   adjacent triangle, operate on the edge only if the current triangle */
  /*   has a smaller pointer than its neighbor.  This way, each edge is    */
  /*   considered only once.                                               */
  while (triangleloop.tri != (triangle *) NULL) {
    for (triangleloop.orient = 0; triangleloop.orient < 3;
         triangleloop.orient++) {
      sym(triangleloop, trisym);
      if ((triangleloop.tri < trisym.tri) || (trisym.tri == m->dummytri)) {
        org(triangleloop, p1);
        dest(triangleloop, p2);
        elist[index++] = vertexmark(p1);
        elist[index++] = vertexmark(p2);
        if (b->nobound) {
        } else {
          /* Edge number, indices of two endpoints, and a boundary marker. */
          /*   If there's no subsegment, the boundary marker is zero.      */
          if (b->usesegments) {
            tspivot(triangleloop, checkmark);
            if (checkmark.ss == m->dummysub) {
              emlist[edgenumber - b->firstnumber] = 0;
            } else {
              emlist[edgenumber - b->firstnumber] = mark(checkmark);
            }
          } else {
            emlist[edgenumber - b->firstnumber] = trisym.tri == m->dummytri;
          }
        }
        edgenumber++;
      }
    }
    triangleloop.tri = triangletraverse(m);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  writeneighbors()   Write the list of neighbours to an array.             */
/*                                                                           */
/*****************************************************************************/

void writeneighbors(mesh *m, behavior *b, int **neighborlist)
{
  int *subsegs;

  int *nlist;
  int index;
  struct otri triangleloop, trisym;
  long elementnumber;
  int neighbor1, neighbor2, neighbor3;
  triangle ptr;                         /* Temporary variable used by sym(). */

  /* Allocate memory for neighbors if necessary. */
  if (*neighborlist == (int *) NULL) {
    *neighborlist = (int *) trimalloc((int) (m->triangles.items * 3 *
                                             sizeof(int)));
  }
  nlist = *neighborlist;
  index = 0;
  
  /* TODO: check writeneighbors subseg pointers. */

  /* This needs some checking: (triangleloop.tri + 6) actually is a
   * pointer to a subseg and not an int. */

  subsegs = (int *) trimalloc((int) (m->triangles.items * sizeof(int)));

  /* Number the triangles by overwriting the first subseg pointer
   * with an integer id. */
  
  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  triangleloop.orient = 0;
  elementnumber = b->firstnumber;
  while (triangleloop.tri != (triangle *) NULL) {
	subsegs[index] = * (int *) (triangleloop.tri + 6);
    * (int *) (triangleloop.tri + 6) = (int) elementnumber;
    triangleloop.tri = triangletraverse(m);
    elementnumber++;
	index++;
  }
  * (int *) (m->dummytri + 6) = -1;
  
  index = 0;
  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  elementnumber = b->firstnumber;
  while (triangleloop.tri != (triangle *) NULL) {
    triangleloop.orient = 1;
    sym(triangleloop, trisym);
    neighbor1 = * (int *) (trisym.tri + 6);
    triangleloop.orient = 2;
    sym(triangleloop, trisym);
    neighbor2 = * (int *) (trisym.tri + 6);
    triangleloop.orient = 0;
    sym(triangleloop, trisym);
    neighbor3 = * (int *) (trisym.tri + 6);

    nlist[index++] = neighbor1;
    nlist[index++] = neighbor2;
    nlist[index++] = neighbor3;

    triangleloop.tri = triangletraverse(m);
    elementnumber++;
  }
  
  /* Restore original subseg values. */

  index = 0;
  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  triangleloop.orient = 0;
  while (triangleloop.tri != (triangle *) NULL) {
    * (int *) (triangleloop.tri + 6) = subsegs[index];
    triangleloop.tri = triangletraverse(m);
	index++;
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Array I/O routines end here                               *********/


/*****************************************************************************/
/*                                                                           */
/*  quality_statistics()   Compute statistics about the quality of the mesh.   */
/*                                                                           */
/*****************************************************************************/

int quality_statistics(mesh *m, behavior *b, quality *q)
{
  struct otri triangleloop;
  vertex p[3];
  REAL cossquaretable[8];
  REAL ratiotable[16];
  REAL dx[3], dy[3];
  REAL edgelength[3];
  REAL dotproduct;
  REAL cossquare;
  REAL triarea;
  REAL trilongest2;
  REAL triminaltitude2;
  REAL triaspect2;
  REAL radconst, degconst;
  REAL shortest, longest;
  REAL smallestarea, biggestarea;
  REAL smallestangle, biggestangle;
  REAL minaltitude;
  REAL worstaspect;
  int aspectindex;
  int tendegree;
  int acutebiggest;
  int i, ii, j, k;

  radconst = PI / 18.0;
  degconst = 180.0 / PI;
  for (i = 0; i < 8; i++) {
    cossquaretable[i] = cos(radconst * (REAL) (i + 1));
    cossquaretable[i] = cossquaretable[i] * cossquaretable[i];
  }
  for (i = 0; i < 18; i++) {
    q->angletable[i] = 0;
  }

  ratiotable[0]  =      1.5;      ratiotable[1]  =     2.0;
  ratiotable[2]  =      2.5;      ratiotable[3]  =     3.0;
  ratiotable[4]  =      4.0;      ratiotable[5]  =     6.0;
  ratiotable[6]  =     10.0;      ratiotable[7]  =    15.0;
  ratiotable[8]  =     25.0;      ratiotable[9]  =    50.0;
  ratiotable[10] =    100.0;      ratiotable[11] =   300.0;
  ratiotable[12] =   1000.0;      ratiotable[13] = 10000.0;
  ratiotable[14] = 100000.0;      ratiotable[15] =     0.0;
  for (i = 0; i < 16; i++) {
    q->aspecttable[i] = 0;
  }

  worstaspect = 0.0;
  minaltitude = m->xmax - m->xmin + m->ymax - m->ymin;
  minaltitude = minaltitude * minaltitude;
  shortest = minaltitude;
  longest = 0.0;
  smallestarea = minaltitude;
  biggestarea = 0.0;
  worstaspect = 0.0;
  smallestangle = 0.0;
  biggestangle = 2.0;
  acutebiggest = 1;

  traversalinit(&m->triangles);
  triangleloop.tri = triangletraverse(m);
  triangleloop.orient = 0;
  while (triangleloop.tri != (triangle *) NULL) {
    org(triangleloop, p[0]);
    dest(triangleloop, p[1]);
    apex(triangleloop, p[2]);
    trilongest2 = 0.0;

    for (i = 0; i < 3; i++) {
      j = plus1mod3[i];
      k = minus1mod3[i];
      dx[i] = p[j][0] - p[k][0];
      dy[i] = p[j][1] - p[k][1];
      edgelength[i] = dx[i] * dx[i] + dy[i] * dy[i];
      if (edgelength[i] > trilongest2) {
        trilongest2 = edgelength[i];
      }
      if (edgelength[i] > longest) {
        longest = edgelength[i];
      }
      if (edgelength[i] < shortest) {
        shortest = edgelength[i];
      }
    }

    triarea = counterclockwise(m, b, p[0], p[1], p[2]);
    if (triarea < smallestarea) {
      smallestarea = triarea;
    }
    if (triarea > biggestarea) {
      biggestarea = triarea;
    }
    triminaltitude2 = triarea * triarea / trilongest2;
    if (triminaltitude2 < minaltitude) {
      minaltitude = triminaltitude2;
    }
    triaspect2 = trilongest2 / triminaltitude2;
    if (triaspect2 > worstaspect) {
      worstaspect = triaspect2;
    }
    aspectindex = 0;
    while ((triaspect2 > ratiotable[aspectindex] * ratiotable[aspectindex])
      && (aspectindex < 15)) {
        aspectindex++;
    }
    q->aspecttable[aspectindex]++;

    for (i = 0; i < 3; i++) {
      j = plus1mod3[i];
      k = minus1mod3[i];
      dotproduct = dx[j] * dx[k] + dy[j] * dy[k];
      cossquare = dotproduct * dotproduct / (edgelength[j] * edgelength[k]);
      tendegree = 8;
      for (ii = 7; ii >= 0; ii--) {
        if (cossquare > cossquaretable[ii]) {
          tendegree = ii;
        }
      }
      if (dotproduct <= 0.0) {
        q->angletable[tendegree]++;
        if (cossquare > smallestangle) {
          smallestangle = cossquare;
        }
        if (acutebiggest && (cossquare < biggestangle)) {
          biggestangle = cossquare;
        }
      } else {
        q->angletable[17 - tendegree]++;
        if (acutebiggest || (cossquare > biggestangle)) {
          biggestangle = cossquare;
          acutebiggest = 0;
        }
      }
    }
    triangleloop.tri = triangletraverse(m);
  }

  shortest = sqrt(shortest);
  longest = sqrt(longest);
  minaltitude = sqrt(minaltitude);
  worstaspect = sqrt(worstaspect);
  smallestarea *= 0.5;
  biggestarea *= 0.5;
  if (smallestangle >= 1.0) {
    smallestangle = 0.0;
  } else {
    smallestangle = degconst * acos(sqrt(smallestangle));
  }
  if (biggestangle >= 1.0) {
    biggestangle = 180.0;
  } else {
    if (acutebiggest) {
      biggestangle = degconst * acos(sqrt(biggestangle));
    } else {
      biggestangle = 180.0 - degconst * acos(sqrt(biggestangle));
    }
  }

  q->worstaspect = worstaspect;
  q->minaltitude = minaltitude;
  q->shortest = shortest;
  q->longest = longest;
  q->smallestarea = smallestarea;
  q->biggestarea = biggestarea;
  q->worstaspect = worstaspect;
  q->smallestangle = smallestangle;
  q->biggestangle = biggestangle;

  return 0;
}

