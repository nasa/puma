#ifndef ACUTE_INTERNAL_H
#define ACUTE_INTERNAL_H

#include "../triangle.h"

void findNewSPLocationWithoutMaxAngle(mesh *m, behavior *b,
                      vertex torg, vertex tdest, vertex tapex,
                      vertex circumcenter, REAL *xi, REAL *eta, int offcenter, struct otri badotri);
void findNewSPLocationWithMaxAngle(mesh *m, behavior *b,
                      vertex torg, vertex tdest, vertex tapex,
                      vertex circumcenter, REAL *xi, REAL *eta, int offcenter, struct otri badotri);
int longestShortestEdge(REAL aodist, REAL dadist, REAL dodist);
int doSmoothing(mesh *m, behavior *b, struct otri badotri,
		vertex torg, vertex tdest, vertex tapex, REAL *newloc);
int getStarPoints(mesh *m, struct otri badotri,
			vertex p, vertex q, vertex r, int whichPoint, REAL *points);
int getNeighborsVertex(mesh *m, struct otri badotri,
				REAL first_x, REAL first_y, REAL second_x, REAL second_y, 
				REAL *thirdpoint, struct otri *neighotri);
int getWedgeIntersectionWithoutMaxAngle(mesh *m, behavior *b, 
			                int numpoints, REAL *points, REAL *newloc);
int getWedgeIntersectionWithMaxAngle(mesh *m, behavior *b, 
			             int numpoints, REAL *points, REAL *newloc);
int polygonAngles(behavior *b,int numpoints, REAL *points);
int testPolygonAngle(behavior *b, REAL *x1, REAL *y1, REAL *x2, REAL *y2, REAL *x3, REAL *y3 );
void lineLineIntersection(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4 , REAL *p);
int halfPlaneIntersection(int numvertices, REAL *convexPoly, REAL x1, REAL y1, REAL x2, REAL y2);
int splitConvexPolygon(int numvertices,REAL *convexPoly, REAL x1, REAL y1, REAL x2, REAL y2, REAL *polys[]);
int linePointLocation(REAL x1, REAL y1, REAL x2, REAL y2, REAL x, REAL y);
void lineLineSegmentIntersection(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4 , REAL *p);
void findPolyCentroid(int numpoints, REAL *points, REAL *centroid);
void circleLineIntersection (REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL r , REAL *p);
int chooseCorrectPoint (REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, int isObtuse );
void pointBetweenPoints(REAL x1, REAL y1, REAL x2, REAL y2, REAL x, REAL y, REAL *p);
int testTriangleAngle(behavior *b, REAL *x1, REAL *y1, REAL *x2, REAL *y2, REAL *x3, REAL *y3 );
REAL minDistanceToNeigbor(mesh *m, behavior *b, REAL newlocX, REAL newlocY, struct otri *searchtri);

#endif /* ACUTE_INTERNAL_H */
