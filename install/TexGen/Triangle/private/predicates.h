#ifndef PREDICATES_H
#define PREDICATES_H

#include "../triangle.h"

void exactinit();

REAL counterclockwise(mesh *m, behavior *b,
                      vertex pa, vertex pb, vertex pc);

REAL incircle(mesh *m, behavior *b,
              vertex pa, vertex pb, vertex pc, vertex pd);

REAL nonregular(mesh *m, behavior *b,
                vertex pa, vertex pb, vertex pc, vertex pd);

void findcircumcenter(mesh *m, behavior *b,
                      vertex torg, vertex tdest, vertex tapex,
                      vertex circumcenter, REAL *xi, REAL *eta, int offcenter);

/********* Private methods *********/

int fast_expansion_sum_zeroelim(int elen, REAL *e, int flen, REAL *f, REAL *h);

int scale_expansion_zeroelim(int elen, REAL *e, REAL b, REAL *h);

REAL estimate(int elen, REAL *e);

REAL counterclockwiseadapt(vertex pa, vertex pb, vertex pc, REAL detsum);

REAL incircleadapt(vertex pa, vertex pb, vertex pc, vertex pd, REAL permanent);

REAL orient3dadapt(vertex pa, vertex pb, vertex pc, vertex pd,
                   REAL aheight, REAL bheight, REAL cheight, REAL dheight,
                   REAL permanent);

REAL orient3d(mesh *m, behavior *b,
              vertex pa, vertex pb, vertex pc, vertex pd,
              REAL aheight, REAL bheight, REAL cheight, REAL dheight);


#endif /* PREDICATES_H */
