#ifndef ACUTE_H
#define ACUTE_H

#include "../triangle.h"

typedef struct acutepool_t {
    int size;
    // getWedgeIntersection (fixed size)
    REAL *initialpoly;
    // getWedgeIntersection (dynamic size)
    REAL *petalx;
    REAL *petaly;
    REAL *petalr;
    REAL *wedges;
    // doSmoothing (fixed size [500])
    REAL *points_p;
    REAL *points_q;
    REAL *points_r;
} acutepool;

void findNewSPLocation(mesh *m, behavior *b,
                      vertex torg, vertex tdest, vertex tapex,
                      vertex circumcenter, REAL *xi, REAL *eta, int offcenter, struct otri badotri);

void acutepool_init(int n, acutepool **mp);

void acutepool_resize(int n, acutepool *p);

void acutepool_deinit(acutepool *p);

#endif /* ACUTE_H */
