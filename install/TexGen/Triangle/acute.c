
//=======================================//
//       ACUTE SOFTWARE VERSION 1.0      //
//=======================================//
// DATE: 06/15/2009
// GENERATES PREMIUM QUALITY TRIANGULATIONS; LARGE MINIMUM ANGLE VALUE OR LARGE MINIMUM ANGLE VALUE WHILE HAVING SMALL MAXIMUM ANGLE VALUE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "triangle.h"
#include "triangle_internal.h"
#include "predicates.h"
#include "acute.h"
#include "acute_internal.h"

#ifndef NO_ACUTE

// ACUTE MEMORY POOL

void acutepool_init(int n, acutepool **mp) {
    acutepool *p = (acutepool *) trimalloc(sizeof(acutepool));

    p->size = n;
    
    p->initialpoly = (REAL *)malloc(sizeof(REAL)* 500);
    p->petalx = (REAL *)malloc(sizeof(REAL)* 2 * n);
    p->petaly = (REAL *)malloc(sizeof(REAL)* 2 * n);
    p->petalr = (REAL *)malloc(sizeof(REAL)* 2 * n);

    // If maxangle is 0.0 we'd only need (2 * n * 16 + 36) REALs, but since we
    // do not know if maxangle gets set later on, let's allocate enough memory
    // right away.

    p->wedges = (REAL *)malloc(sizeof(REAL)* 2 * n * 20 + 40);

    p->points_p = (REAL *)malloc(sizeof(REAL)* 500);
    p->points_q = (REAL *)malloc(sizeof(REAL)* 500);
    p->points_r = (REAL *)malloc(sizeof(REAL)* 500);

	*mp = p;
}

void acutepool_resize(int n, acutepool *p) {
    if (p->size < n) {
        p->size = n;

        // Free old memory
        free(p->petalx);
        free(p->petaly);
        free(p->petalr);
        free(p->wedges);

        // Allocate new memory
        p->petalx = (REAL *)malloc(sizeof(REAL)* 2 * n);
        p->petaly = (REAL *)malloc(sizeof(REAL)* 2 * n);
        p->petalr = (REAL *)malloc(sizeof(REAL)* 2 * n);

        p->wedges = (REAL *)malloc(sizeof(REAL)* 2 * n * 20 + 40);
    }
}

void acutepool_deinit(acutepool *p) {
  if (p != (acutepool *)NULL) {
    free(p->initialpoly);
    free(p->petalx);
    free(p->petaly);
    free(p->petalr);
    free(p->wedges);

    free(p->points_p);
    free(p->points_q);
    free(p->points_r);
  }
}

// END ACUTE MEMORY POOL

// for comparing real numbers
const double compConst = 1.0e-80;

// Defined in triangle.c
extern int plus1mod3[3];
extern int minus1mod3[3];

/*=====================NEW STEINER POINT FUNCTION============================*/
/*****************************************************************************/
/*  Steiner point insertion routine                                          */
/*  findNewSPLocation()    Find a new location for a Steiner point	     */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

void findNewSPLocation(mesh *m, behavior *b,
                      vertex torg, vertex tdest, vertex tapex,
                      vertex circumcenter, REAL *xi, REAL *eta, int offcenter, struct otri badotri) {
	// Based on using -U switch, call the corresponding function
	if(b->maxangle == 0.00000){
		 findNewSPLocationWithoutMaxAngle(m, b, torg, tdest, tapex, circumcenter, xi, eta, 1, badotri);
	}else{
		 findNewSPLocationWithMaxAngle(m, b, torg, tdest, tapex, circumcenter, xi, eta, 1, badotri);
	}

}// end of findNewSPLocation()

/*********************************************************************************/
/*  Steiner point insertion routine                                              */
/*  findNewSPLocationWithoutMaxAngle() Find a new location for a Steiner point   */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
void findNewSPLocationWithoutMaxAngle(mesh *m, behavior *b,
                      vertex torg, vertex tdest, vertex tapex,
                      vertex circumcenter, REAL *xi, REAL *eta, int offcenter, struct otri badotri){

	// for calculating the distances of the edges
	REAL xdo, ydo, xao, yao, xda, yda;
	REAL dodist, aodist, dadist;
	// for exact calculation
	REAL denominator;
	REAL dx, dy, dxoff, dyoff;
	
	////////////////////////////// HALE'S VARIABLES //////////////////////////////
	// keeps the difference of coordinates edge 
	REAL xShortestEdge, yShortestEdge, xMiddleEdge, yMiddleEdge, xLongestEdge, yLongestEdge;
	
	// keeps the square of edge lengths
	REAL shortestEdgeDist, middleEdgeDist, longestEdgeDist;
	
	// keeps the vertices according to the angle incident to that vertex in a triangle
	vertex smallestAngleCorner = NULL, middleAngleCorner = NULL, largestAngleCorner = NULL;
	
	// keeps the type of orientation if the triangle
	int orientation = 0;
	// keeps the coordinates of circumcenter of itself and neighbor triangle circumcenter	
	REAL myCircumcenter[2], neighborCircumcenter[2];	

	// keeps if bad triangle is almost good or not
	int almostGood = 0;
	// keeps the cosine of the largest angle
	REAL cosMaxAngle;
	int isObtuse = -1; // 1: obtuse 0: nonobtuse
	// keeps the radius of petal
	REAL petalRadius;
	// for calculating petal center
	REAL xPetalCtr_1, yPetalCtr_1, xPetalCtr_2, yPetalCtr_2, xPetalCtr, yPetalCtr, xMidOfShortestEdge, yMidOfShortestEdge;
	REAL dxcenter1, dycenter1, dxcenter2, dycenter2;
	// for finding neighbor
	struct otri neighborotri;
	REAL thirdPoint[2];
	int neighborNotFound = -1;
	// for keeping the vertices of the neighbor triangle
	vertex neighborvertex_1;
	vertex neighborvertex_2;
	vertex neighborvertex_3;
	// dummy variables 
  	REAL xi_tmp, eta_tmp;
	// for petal intersection
	REAL vector_x, vector_y, xMidOfLongestEdge, yMidOfLongestEdge, inter_x, inter_y, p[5], voronoiOrInter[4];
	int isCorrect = -1; 

	// for vector calculations in perturbation
	REAL ax, ay, d;	
	REAL pertConst = 0.06; // perturbation constant

	REAL lengthConst = 1; // used at comparing circumcenter's distance to proposed point's distance
	REAL justAcute = 1; // used for making the program working for one direction only
	// for smoothing
	int relocated = 0;// used to differentiate between calling the deletevertex and just proposing a steiner point
	REAL newloc[2];   // new location suggested by smoothing
	REAL origin_x, origin_y; // for keeping torg safe
	struct otri delotri; // keeping the original orientation for relocation process
	// keeps the first and second direction suggested points
	REAL dxFirstSuggestion, dyFirstSuggestion, dxSecondSuggestion, dySecondSuggestion;
	// second direction variables
	REAL xMidOfMiddleEdge, yMidOfMiddleEdge;
	////////////////////////////// END OF HALE'S VARIABLES //////////////////////////////
	
	m->circumcentercount++; 

	/* Compute the circumcenter of the triangle. */
	xdo = tdest[0] - torg[0];
	ydo = tdest[1] - torg[1];
	xao = tapex[0] - torg[0];
	yao = tapex[1] - torg[1];
	xda = tapex[0] - tdest[0];
	yda = tapex[1] - tdest[1];
	// keeps the square of the distances
	dodist = xdo * xdo + ydo * ydo;
	aodist = xao * xao + yao * yao;
	dadist = (tdest[0] - tapex[0]) * (tdest[0] - tapex[0]) +
		(tdest[1] - tapex[1]) * (tdest[1] - tapex[1]);
	// checking if the user wanted exact arithmetic or not
	if (b->noexact) {
		denominator = 0.5 / (xdo * yao - xao * ydo);
	} else {
		/* Use the counterclockwise() routine to ensure a positive (and */
		/*   reasonably accurate) result, avoiding any possibility of   */
		/*   division by zero.                                          */
		denominator = 0.5 / counterclockwise(m, b, tdest, tapex, torg);
		/* Don't count the above as an orientation test. */
		m->counterclockcount--;
	}
	// calculate the circumcenter in terms of distance to origin point 
	dx = (yao * dodist - ydo * aodist) * denominator;
	dy = (xdo * aodist - xao * dodist) * denominator;
	// for debugging and for keeping circumcenter to use later
	// coordinate value of the circumcenter
	myCircumcenter[0] = torg[0] + dx;  
	myCircumcenter[1] = torg[1] + dy;		
	
	delotri = badotri; // save for later
	///////////////// FINDING THE ORIENTATION OF TRIANGLE //////////////////
	/* Find the (squared) length of the triangle's shortest edge.  This   */
	/*   serves as a conservative estimate of the insertion radius of the */
	/*   circumcenter's parent.  The estimate is used to ensure that      */
	/*   the algorithm terminates even if very small angles appear in     */
	/*   the input PSLG. 						      */
	// find the orientation of the triangle, basically shortest and longest edges
	orientation = longestShortestEdge(aodist, dadist, dodist);
	//printf("org: (%f,%f), dest: (%f,%f), apex: (%f,%f)\n",torg[0],torg[1],tdest[0],tdest[1],tapex[0],tapex[1]);
	/////////////////////////////////////////////////////////////////////////////////////////////
	// 123: shortest: aodist	// 213: shortest: dadist	// 312: shortest: dodist   //	
	//	middle: dadist 		//	middle: aodist 		//	middle: aodist     //
	//	longest: dodist		//	longest: dodist		//	longest: dadist    //
	// 132: shortest: aodist 	// 231: shortest: dadist 	// 321: shortest: dodist   //
	//	middle: dodist 		//	middle: dodist 		//	middle: dadist     //
	//	longest: dadist		//	longest: aodist		//	longest: aodist    //
	/////////////////////////////////////////////////////////////////////////////////////////////

	switch(orientation){
		case 123: 	// assign necessary information
				/// smallest angle corner: dest
				/// largest angle corner: apex
				xShortestEdge = xao;	yShortestEdge = yao;
				xMiddleEdge = xda;	yMiddleEdge = yda;
				xLongestEdge = xdo;	yLongestEdge = ydo;
				
				shortestEdgeDist = aodist;
				middleEdgeDist = dadist;
				longestEdgeDist = dodist;

				smallestAngleCorner = tdest;
				middleAngleCorner = torg;
				largestAngleCorner = tapex;
				break;

		case 132: 	// assign necessary information
				/// smallest angle corner: dest
				/// largest angle corner: org
				xShortestEdge = xao;	yShortestEdge = yao;
				xMiddleEdge = xdo;	yMiddleEdge = ydo;
				xLongestEdge = xda;	yLongestEdge = yda;
				
				shortestEdgeDist = aodist;
				middleEdgeDist = dodist;
				longestEdgeDist = dadist;

				smallestAngleCorner = tdest;
				middleAngleCorner = tapex;
				largestAngleCorner = torg;
				
				break;
		case 213: 	// assign necessary information
				/// smallest angle corner: org
				/// largest angle corner: apex
				xShortestEdge = xda;	yShortestEdge = yda;
				xMiddleEdge = xao;	yMiddleEdge = yao;
				xLongestEdge = xdo;	yLongestEdge = ydo;
				
				shortestEdgeDist = dadist;
				middleEdgeDist = aodist;
				longestEdgeDist = dodist;

				smallestAngleCorner = torg;
				middleAngleCorner = tdest;
				largestAngleCorner = tapex;
				break;
		case 231: 	// assign necessary information
				/// smallest angle corner: org
				/// largest angle corner: dest
				xShortestEdge = xda;	yShortestEdge = yda;
				xMiddleEdge = xdo;	yMiddleEdge = ydo;
				xLongestEdge = xao;	yLongestEdge = yao;
					
				shortestEdgeDist = dadist;
				middleEdgeDist = dodist;
				longestEdgeDist = aodist;

				smallestAngleCorner = torg;	
				middleAngleCorner = tapex;	
				largestAngleCorner = tdest;		
				break;
		case 312: 	// assign necessary information
				/// smallest angle corner: apex
				/// largest angle corner: org
				xShortestEdge = xdo;	yShortestEdge = ydo;
				xMiddleEdge = xao;	yMiddleEdge = yao;
				xLongestEdge = xda;	yLongestEdge = yda;
				
				shortestEdgeDist = dodist;
				middleEdgeDist = aodist;
				longestEdgeDist = dadist;

				smallestAngleCorner = tapex;
				middleAngleCorner = tdest;
				largestAngleCorner = torg;
				break;
		case 321: 	// assign necessary information
				/// smallest angle corner: apex
				/// largest angle corner: dest
				xShortestEdge = xdo;	yShortestEdge = ydo;
				xMiddleEdge = xda;	yMiddleEdge = yda;
				xLongestEdge = xao;	yLongestEdge = yao;
				
				shortestEdgeDist = dodist;
				middleEdgeDist = dadist;
				longestEdgeDist = aodist;

				smallestAngleCorner = tapex;
				middleAngleCorner = torg;
				largestAngleCorner = tdest;
				break;

	}// end of switch	
	// check for offcenter condition
	if (offcenter && (b->offconstant > 0.0)) {
		// origin has the smallest angle
		if(orientation == 213 || orientation == 231){
			/* Find the position of the off-center, as described by Alper Ungor. */
			dxoff = 0.5 * xShortestEdge - b->offconstant * yShortestEdge;
			dyoff = 0.5 * yShortestEdge + b->offconstant * xShortestEdge;
			/* If the off-center is closer to destination than the */
			/*   circumcenter, use the off-center instead.        */
			/// REALLY BAD CASE ///			
			if (dxoff * dxoff + dyoff * dyoff <
			    (dx - xdo) * (dx - xdo) + (dy - ydo) * (dy - ydo)) {
				dx = xdo + dxoff;
				dy = ydo + dyoff;							
			}
			/// ALMOST GOOD CASE ///
			else{
				almostGood = 1;
			}
		// destination has the smallest angle	
		}else if(orientation == 123 || orientation == 132){
			/* Find the position of the off-center, as described by Alper Ungor. */
			dxoff = 0.5 * xShortestEdge + b->offconstant * yShortestEdge;
			dyoff = 0.5 * yShortestEdge - b->offconstant * xShortestEdge;
			/* If the off-center is closer to the origin than the */
			/*   circumcenter, use the off-center instead.        */
			/// REALLY BAD CASE ///
			if (dxoff * dxoff + dyoff * dyoff < dx * dx + dy * dy) {
				dx = dxoff;
				dy = dyoff;							
			}		
			/// ALMOST GOOD CASE ///		
			else{	
				almostGood = 1;
			}
		// apex has the smallest angle	
		}else{//orientation == 312 || orientation == 321 
			/* Find the position of the off-center, as described by Alper Ungor. */
			dxoff = 0.5 * xShortestEdge - b->offconstant * yShortestEdge;
			dyoff = 0.5 * yShortestEdge + b->offconstant * xShortestEdge;
			/* If the off-center is closer to the origin than the */
			/*   circumcenter, use the off-center instead.        */
			/// REALLY BAD CASE ///
			if (dxoff * dxoff + dyoff * dyoff < dx * dx + dy * dy) {
				dx = dxoff;
				dy = dyoff;						
			}		
			/// ALMOST GOOD CASE ///		
			else{	
				almostGood = 1;
			}
		}
	}	
	// if the bad triangle is almost good, apply our approach
	if(almostGood == 1){
		
		/// calculate cosine of largest angle	///	
		cosMaxAngle = (middleEdgeDist + shortestEdgeDist - longestEdgeDist)/(2*sqrt(middleEdgeDist)*sqrt(shortestEdgeDist));	
		if(cosMaxAngle < 0.0){
			// obtuse
			isObtuse = 1;
		}else if(fabs(cosMaxAngle - 0.0) <= compConst){
			// right triangle (largest angle is 90 degrees)
			isObtuse = 1;
		}else{
			// nonobtuse
			isObtuse = 0;
		}
		/// RELOCATION	(LOCAL SMOOTHING) ///
		/// check for possible relocation of one of triangle's points ///				
		relocated = doSmoothing(m,b,delotri,torg,tdest,tapex,newloc);		
		/// if relocation is possible, delete that vertex and insert a vertex at the new location ///		
		if(relocated > 0){							
			dx = newloc[0] - torg[0];
			dy = newloc[1] - torg[1];
			origin_x = torg[0];	// keep for later use
			origin_y = torg[1];									
			switch(relocated){
				case 1:
					//printf("Relocate: (%f,%f)\n", torg[0],torg[1]);			
					deletevertex(m,b,&delotri);					
					break;
				case 2:	
					//printf("Relocate: (%f,%f)\n", tdest[0],tdest[1]);			
					lnextself(delotri);
					deletevertex(m,b,&delotri);					
					break;
				case 3:
					//printf("Relocate: (%f,%f)\n", tapex[0],tapex[1]);						
					lprevself(delotri);
					deletevertex(m,b,&delotri);							
					break;				
					
			}					
		}else{				
			// calculate radius of the petal according to angle constraint
			// first find the visible region, PETAL
			// find the center of the circle and radius
			petalRadius = sqrt(shortestEdgeDist)/(2*sin(b->minangle* PI / 180.0));				
			/// compute two possible centers of the petal ///
			// finding the center
			// first find the middle point of smallest edge
			xMidOfShortestEdge = (middleAngleCorner[0] + largestAngleCorner[0])/2.0;
			yMidOfShortestEdge = (middleAngleCorner[1] + largestAngleCorner[1])/2.0;
			// two possible centers
			xPetalCtr_1 = xMidOfShortestEdge + sqrt(petalRadius*petalRadius-(shortestEdgeDist/4))*(middleAngleCorner[1] -
				largestAngleCorner[1])/sqrt(shortestEdgeDist);
			yPetalCtr_1 = yMidOfShortestEdge + sqrt(petalRadius*petalRadius-(shortestEdgeDist/4))*(largestAngleCorner[0] - 
				middleAngleCorner[0])/sqrt(shortestEdgeDist); 
			
			xPetalCtr_2 = xMidOfShortestEdge - sqrt(petalRadius*petalRadius-(shortestEdgeDist/4))*(middleAngleCorner[1] -
				largestAngleCorner[1])/sqrt(shortestEdgeDist);
			yPetalCtr_2 = yMidOfShortestEdge - sqrt(petalRadius*petalRadius-(shortestEdgeDist/4))*(largestAngleCorner[0] - 
				middleAngleCorner[0])/sqrt(shortestEdgeDist); 		
			// find the correct circle since there will be two possible circles
			// calculate the distance to smallest angle corner
			dxcenter1 = (xPetalCtr_1 - smallestAngleCorner[0]) * (xPetalCtr_1 - smallestAngleCorner[0]);
			dycenter1 = (yPetalCtr_1 - smallestAngleCorner[1]) * (yPetalCtr_1 - smallestAngleCorner[1]);
			dxcenter2 = (xPetalCtr_2 - smallestAngleCorner[0]) * (xPetalCtr_2 - smallestAngleCorner[0]);
			dycenter2 = (yPetalCtr_2 - smallestAngleCorner[1]) * (yPetalCtr_2 - smallestAngleCorner[1]);
	
			// whichever is closer to smallest angle corner, it must be the center
			if (dxcenter1 + dycenter1 <= dxcenter2 + dycenter2){
				xPetalCtr = xPetalCtr_1;	yPetalCtr = yPetalCtr_1;
			}else{
				xPetalCtr = xPetalCtr_2;	yPetalCtr = yPetalCtr_2;
			}

			/// find the third point of the neighbor triangle  ///
			neighborNotFound = getNeighborsVertex(m, badotri, middleAngleCorner[0], middleAngleCorner[1], 
						smallestAngleCorner[0], smallestAngleCorner[1] ,thirdPoint, &neighborotri);		
			/// find the circumcenter of the neighbor triangle ///
			dxFirstSuggestion = dx;	// if we cannot find any appropriate suggestion, we use circumcenter
			dyFirstSuggestion = dy; 			
			// if there is a neighbor triangle
			if(neighborNotFound == 0){
				org(neighborotri, neighborvertex_1);
				dest(neighborotri, neighborvertex_2);	
				apex(neighborotri, neighborvertex_3);		
				// now calculate neighbor's circumcenter which is the voronoi site
				findcircumcenter(m, b, neighborvertex_1, neighborvertex_2,neighborvertex_3,neighborCircumcenter, &xi_tmp, &eta_tmp, 0);
				/// compute petal and Voronoi edge intersection ///
				// in order to avoid degenerate cases, we need to do a vector based calculation for line		
				vector_x = (middleAngleCorner[1] - smallestAngleCorner[1]);//(-y, x)
				vector_y = smallestAngleCorner[0] - middleAngleCorner[0];
				vector_x = myCircumcenter[0] + vector_x;
				vector_y = myCircumcenter[1] + vector_y;
				
				
				// by intersecting bisectors you will end up with the one you want to walk on
				// then this line and circle should be intersected
				circleLineIntersection(myCircumcenter[0], myCircumcenter[1], vector_x, vector_y, 
						xPetalCtr, yPetalCtr, petalRadius, p);			
				/// choose the correct intersection point ///
				// calculate middle point of the longest edge(bisector)
				xMidOfLongestEdge = (middleAngleCorner[0]+smallestAngleCorner[0])/2.0;	
				yMidOfLongestEdge = (middleAngleCorner[1]+smallestAngleCorner[1])/2.0;
				// we need to find correct intersection point, since line intersects circle twice
				isCorrect = chooseCorrectPoint(xMidOfLongestEdge, yMidOfLongestEdge, p[3], p[4], 
							myCircumcenter[0], myCircumcenter[1],isObtuse);			
				// make sure which point is the correct one to be considered
				if(isCorrect == 1){			
					inter_x = p[3];
					inter_y = p[4];
				}else{
					inter_x = p[1];
					inter_y = p[2];
				}
				/// check if there is a Voronoi vertex between before intersection ///
				// check if the voronoi vertex is between the intersection and circumcenter
				pointBetweenPoints(inter_x, inter_y, myCircumcenter[0], myCircumcenter[1], 
						neighborCircumcenter[0], neighborCircumcenter[1], voronoiOrInter);
				
				/// determine the point to be suggested ///
				if(p[0] > 0.0){ // there is at least one intersection point
					// if it is between circumcenter and intersection	
					// if it returns 1.0 this means we have a voronoi vertex within feasible region
					if(fabs(voronoiOrInter[0] - 1.0) <= compConst){
						if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &neighborCircumcenter[0], &neighborCircumcenter[1]) != 0){
							// go back to circumcenter
							dxFirstSuggestion = dx;
							dyFirstSuggestion = dy;							
													
						}else{ // we are not creating a bad triangle
							// neighbor's circumcenter is suggested
							dxFirstSuggestion = voronoiOrInter[2] - torg[0];
							dyFirstSuggestion = voronoiOrInter[3] - torg[1];						
						}
												
					}else{ // there is no voronoi vertex between intersection point and circumcenter
						if(testTriangleAngle(b, &largestAngleCorner[0], &largestAngleCorner[1], &middleAngleCorner[0], &middleAngleCorner[1], &inter_x, &inter_y) != 0){
						// if it is inside feasible region, then insert v2				
							// apply perturbation
							// find the distance between circumcenter and intersection point
							d = sqrt((inter_x - myCircumcenter[0]) * (inter_x - myCircumcenter[0]) + 
								(inter_y - myCircumcenter[1]) * (inter_y - myCircumcenter[1]));
							// then find the vector going from intersection point to circumcenter
							ax = myCircumcenter[0] - inter_x;
							ay = myCircumcenter[1] - inter_y;
							
							ax = ax / d;
							ay = ay / d;
							// now calculate the new intersection point which is perturbated towards the circumcenter
							inter_x = inter_x + ax * pertConst * sqrt(shortestEdgeDist);
							inter_y = inter_y + ay * pertConst * sqrt(shortestEdgeDist);
							if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &inter_x, &inter_y) != 0){							
								// go back to circumcenter
								dxFirstSuggestion = dx;
								dyFirstSuggestion = dy; 	 						
											
							}else{
								// intersection point is suggested
								dxFirstSuggestion = inter_x - torg[0];
								dyFirstSuggestion = inter_y - torg[1]; 
								
							}
						}else{	
							// intersection point is suggested
							dxFirstSuggestion = inter_x - torg[0];
							dyFirstSuggestion = inter_y - torg[1]; 								
						}
					}
					/// if it is an acute triangle, check if it is a good enough location ///
					// for acute triangle case, we need to check if it is ok to use either of them
					if( (smallestAngleCorner[0]-myCircumcenter[0])*(smallestAngleCorner[0]-myCircumcenter[0]) +
						(smallestAngleCorner[1]-myCircumcenter[1])*(smallestAngleCorner[1]-myCircumcenter[1]) > 
						lengthConst * ( (smallestAngleCorner[0]-(dxFirstSuggestion + torg[0])) *
								(smallestAngleCorner[0]-(dxFirstSuggestion + torg[0])) + 
								(smallestAngleCorner[1]-(dyFirstSuggestion + torg[1])) *
								(smallestAngleCorner[1]-(dyFirstSuggestion + torg[1])) ) ){
						// use circumcenter
						dxFirstSuggestion = dx;
						dyFirstSuggestion = dy; 						
					}// else we stick to what we have found	
				}// intersection point
				
			}// if it is on the boundary, meaning no neighbor triangle in this direction, try other direction	
	
			/// DO THE SAME THING FOR THE OTHER DIRECTION ///
			/// find the third point of the neighbor triangle  ///
			neighborNotFound = getNeighborsVertex(m, badotri, largestAngleCorner[0], largestAngleCorner[1], 
						smallestAngleCorner[0], smallestAngleCorner[1] , thirdPoint, &neighborotri);
			/// find the circumcenter of the neighbor triangle ///
			dxSecondSuggestion = dx;	// if we cannot find any appropriate suggestion, we use circumcenter
			dySecondSuggestion = dy; 				
			// if there is a neighbor triangle
			if(neighborNotFound == 0){
				org(neighborotri, neighborvertex_1);
				dest(neighborotri, neighborvertex_2);	
				apex(neighborotri, neighborvertex_3);		
				// now calculate neighbor's circumcenter which is the voronoi site
				findcircumcenter(m, b, neighborvertex_1, neighborvertex_2,neighborvertex_3,neighborCircumcenter, &xi_tmp, &eta_tmp, 0);	
	
				/// compute petal and Voronoi edge intersection ///
				// in order to avoid degenerate cases, we need to do a vector based calculation for line		
				vector_x = (largestAngleCorner[1] - smallestAngleCorner[1]);//(-y, x)
				vector_y = smallestAngleCorner[0] - largestAngleCorner[0];
				vector_x = myCircumcenter[0] + vector_x;
				vector_y = myCircumcenter[1] + vector_y;
				
				
				// by intersecting bisectors you will end up with the one you want to walk on
				// then this line and circle should be intersected
				circleLineIntersection(myCircumcenter[0], myCircumcenter[1], vector_x, vector_y, 
						xPetalCtr, yPetalCtr, petalRadius, p);
				
				/// choose the correct intersection point ///
				// calcuwedgeslate middle point of the longest edge(bisector)
				xMidOfMiddleEdge = (largestAngleCorner[0]+smallestAngleCorner[0])/2.0;	
				yMidOfMiddleEdge = (largestAngleCorner[1]+smallestAngleCorner[1])/2.0;
				// we need to find correct intersection point, since line intersects circle twice
				// this direction is always ACUTE
				isCorrect = chooseCorrectPoint(xMidOfMiddleEdge, yMidOfMiddleEdge, p[3], p[4], 
							myCircumcenter[0], myCircumcenter[1],0/*(isObtuse+1)%2*/);			
				// make sure which point is the correct one to be considered
				if(isCorrect == 1){			
					inter_x = p[3];
					inter_y = p[4];
				}else{
					inter_x = p[1];
					inter_y = p[2];
				}
				
				/// check if there is a Voronoi vertex between before intersection ///
				// check if the voronoi vertex is between the intersection and circumcenter
				pointBetweenPoints(inter_x, inter_y, myCircumcenter[0], myCircumcenter[1], 
						neighborCircumcenter[0], neighborCircumcenter[1], voronoiOrInter);
				
				/// determine the point to be suggested ///
				if(p[0] > 0.0){ // there is at least one intersection point
					// if it is between circumcenter and intersection	
					// if it returns 1.0 this means we have a voronoi vertex within feasible region
					if(fabs(voronoiOrInter[0] - 1.0) <= compConst){
						if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &neighborCircumcenter[0], &neighborCircumcenter[1]) != 0){
							// go back to circumcenter
							dxSecondSuggestion = dx;
							dySecondSuggestion = dy;
													
						}else{ // we are not creating a bad triangle
							// neighbor's circumcenter is suggested
							dxSecondSuggestion = voronoiOrInter[2] - torg[0];
							dySecondSuggestion = voronoiOrInter[3] - torg[1];
						
						}
												
					}else{ // there is no voronoi vertex between intersection point and circumcenter
						if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &inter_x, &inter_y) != 0){
						// if it is inside feasible region, then insert v2				
							// apply perturbation
							// find the distance between circumcenter and intersection point
							d = sqrt((inter_x - myCircumcenter[0]) * (inter_x - myCircumcenter[0]) + 
								(inter_y - myCircumcenter[1]) * (inter_y - myCircumcenter[1]));
							// then find the vector going from intersection point to circumcenter
							ax = myCircumcenter[0] - inter_x;
							ay = myCircumcenter[1] - inter_y;
							
							ax = ax / d;
							ay = ay / d;
							// now calculate the new intersection point which is perturbated towards the circumcenter
							inter_x = inter_x + ax * pertConst * sqrt(shortestEdgeDist);
							inter_y = inter_y + ay * pertConst * sqrt(shortestEdgeDist);
							if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &inter_x, &inter_y) != 0){							
								// go back to circumcenter
								dxSecondSuggestion = dx;
								dySecondSuggestion = dy; 		
											
							}else{
								// intersection point is suggested
								dxSecondSuggestion = inter_x - torg[0];
								dySecondSuggestion = inter_y - torg[1]; 			
							}
						}else{
						
							// intersection point is suggested
							dxSecondSuggestion = inter_x - torg[0];
							dySecondSuggestion = inter_y - torg[1]; 							
						}
					}
					/// if it is an acute triangle, check if it is a good enough location ///
					// for acute triangle case, we need to check if it is ok to use either of them
					if( (smallestAngleCorner[0]-myCircumcenter[0])*(smallestAngleCorner[0]-myCircumcenter[0]) +
						(smallestAngleCorner[1]-myCircumcenter[1])*(smallestAngleCorner[1]-myCircumcenter[1]) > 
						lengthConst * ( (smallestAngleCorner[0]-(dxSecondSuggestion + torg[0])) *
								(smallestAngleCorner[0]-(dxSecondSuggestion + torg[0])) + 
								(smallestAngleCorner[1]-(dySecondSuggestion + torg[1])) *
								(smallestAngleCorner[1]-(dySecondSuggestion + torg[1])) ) ){
						// use circumcenter
						dxSecondSuggestion = dx;
						dySecondSuggestion = dy; 						
					}// else we stick on what we have found	
				}
			}// if it is on the boundary, meaning no neighbor triangle in this direction, the other direction might be ok		
			if(isObtuse == 1){				
					//obtuse: do nothing					
					dx = dxFirstSuggestion;
					dy = dyFirstSuggestion;					
			}else{ // acute : consider other direction				
				if(justAcute*( (smallestAngleCorner[0]-(dxSecondSuggestion+torg[0]))*
						(smallestAngleCorner[0]-(dxSecondSuggestion+torg[0])) +
				   		(smallestAngleCorner[1]-(dySecondSuggestion+torg[1]))*
						(smallestAngleCorner[1]-(dySecondSuggestion+torg[1])) ) >
				   		(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0]))*
						(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0])) +
				   		(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1]))*
						(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1])) ){
					dx = dxSecondSuggestion;
					dy = dySecondSuggestion;								
				}else{					
					dx = dxFirstSuggestion;
					dy = dyFirstSuggestion;						
				}
				
			}// end if obtuse
		}// end of relocation				 
	}// end of almostGood	

	if(relocated <= 0){
		circumcenter[0] = torg[0] + dx;
		circumcenter[1] = torg[1] + dy;
	}else{
		circumcenter[0] = origin_x + dx;
		circumcenter[1] = origin_y + dy;
	}	

	*xi = (yao * dx - xao * dy) * (2.0 * denominator);
	*eta = (xdo * dy - ydo * dx) * (2.0 * denominator);	

}// end of findNewSPLocationWithoutMaxAngle()

/*********************************************************************************/
/*  Steiner point insertion routine                                              */
/*  findNewSPLocationWithMaxAngle() Find a new location for a Steiner point      */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
void findNewSPLocationWithMaxAngle(mesh *m, behavior *b,
                      vertex torg, vertex tdest, vertex tapex,
                      vertex circumcenter, REAL *xi, REAL *eta, int offcenter, struct otri badotri){

	// for calculating the distances of the edges
	REAL xdo, ydo, xao, yao, xda, yda;
	REAL dodist, aodist, dadist;
	// for exact calculation
	REAL denominator;
	REAL dx, dy, dxoff, dyoff;
	
	////////////////////////////// HALE'S VARIABLES //////////////////////////////
	// keeps the difference of coordinates edge 
	REAL xShortestEdge, yShortestEdge, xMiddleEdge, yMiddleEdge, xLongestEdge, yLongestEdge;
	
	// keeps the square of edge lengths
	REAL shortestEdgeDist, middleEdgeDist, longestEdgeDist;
	
	// keeps the vertices according to the angle incident to that vertex in a triangle
	vertex smallestAngleCorner = NULL, middleAngleCorner = NULL, largestAngleCorner = NULL;
	
	// keeps the type of orientation if the triangle
	int orientation = 0;
	// keeps the coordinates of circumcenter of itself and neighbor triangle circumcenter	
	REAL myCircumcenter[2], neighborCircumcenter[2];	

	// keeps if bad triangle is almost good or not
	int almostGood = 0;
	// keeps the cosine of the largest angle
	REAL cosMaxAngle;
	int isObtuse = -1; // 1: obtuse 0: nonobtuse
	// keeps the radius of petal
	REAL petalRadius;
	// for calculating petal center
	REAL xPetalCtr_1, yPetalCtr_1, xPetalCtr_2, yPetalCtr_2, xPetalCtr, yPetalCtr, xMidOfShortestEdge, yMidOfShortestEdge;
	REAL dxcenter1, dycenter1, dxcenter2, dycenter2;
	// for finding neighbor
	struct otri neighborotri;
	REAL thirdPoint[2];
	// for keeping the vertices of the neighbor triangle
	vertex neighborvertex_1;
	vertex neighborvertex_2;
	vertex neighborvertex_3;
	// dummy variables 
  	REAL xi_tmp, eta_tmp;
	// for petal intersection
	REAL vector_x, vector_y, xMidOfLongestEdge, yMidOfLongestEdge, inter_x, inter_y, p[5], voronoiOrInter[4];
	int isCorrect = -1; 

	// for vector calculations in perturbation
	REAL ax, ay, d;	
	REAL pertConst = 0.06; // perturbation constant

	REAL lengthConst = 1; // used at comparing circumcenter's distance to proposed point's distance
	REAL justAcute = 1; // used for making the program working for one direction only
	// for smoothing
	int relocated = 0;// used to differentiate between calling the deletevertex and just proposing a steiner point
	REAL newloc[2];   // new location suggested by smoothing
	REAL origin_x, origin_y; // for keeping torg safe
	struct otri delotri; // keeping the original orientation for relocation process
	// keeps the first and second direction suggested points
	REAL dxFirstSuggestion, dyFirstSuggestion, dxSecondSuggestion, dySecondSuggestion;
	// second direction variables
	REAL xMidOfMiddleEdge, yMidOfMiddleEdge;

	REAL minangle;	// in order to make sure that the circumcircle of the bad triangle is greater than petal
	// for calculating the slab
	REAL linepnt1_x,linepnt1_y,linepnt2_x,linepnt2_y;	// two points of the line
  	REAL line_inter_x, line_inter_y;
  	REAL line_vector_x, line_vector_y;
  	REAL line_p[3]; // used for getting the return values of functions related to line intersection
  	REAL line_result[4];
	// intersection of slab and the petal
	REAL petal_slab_inter_x_first, petal_slab_inter_y_first,petal_slab_inter_x_second, petal_slab_inter_y_second, x_1, y_1, x_2, y_2;
	REAL petal_bisector_x, petal_bisector_y, dist;
	REAL alpha;
	int neighborNotFound_first = -1;
	int neighborNotFound_second = -1;
	////////////////////////////// END OF HALE'S VARIABLES //////////////////////////////
	
	m->circumcentercount++; 

	/* Compute the circumcenter of the triangle. */
	xdo = tdest[0] - torg[0];
	ydo = tdest[1] - torg[1];
	xao = tapex[0] - torg[0];
	yao = tapex[1] - torg[1];
	xda = tapex[0] - tdest[0];
	yda = tapex[1] - tdest[1];
	// keeps the square of the distances
	dodist = xdo * xdo + ydo * ydo;
	aodist = xao * xao + yao * yao;
	dadist = (tdest[0] - tapex[0]) * (tdest[0] - tapex[0]) +
		(tdest[1] - tapex[1]) * (tdest[1] - tapex[1]);
	// checking if the user wanted exact arithmetic or not
	if (b->noexact) {
		denominator = 0.5 / (xdo * yao - xao * ydo);
	} else {
		/* Use the counterclockwise() routine to ensure a positive (and */
		/*   reasonably accurate) result, avoiding any possibility of   */
		/*   division by zero.                                          */
		denominator = 0.5 / counterclockwise(m, b, tdest, tapex, torg);
		/* Don't count the above as an orientation test. */
		m->counterclockcount--;
	}
	// calculate the circumcenter in terms of distance to origin point 
	dx = (yao * dodist - ydo * aodist) * denominator;
	dy = (xdo * aodist - xao * dodist) * denominator;
	// for debugging and for keeping circumcenter to use later
	// coordinate value of the circumcenter
	myCircumcenter[0] = torg[0] + dx;  
	myCircumcenter[1] = torg[1] + dy;		
	
	delotri = badotri; // save for later
	///////////////// FINDING THE ORIENTATION OF TRIANGLE //////////////////
	/* Find the (squared) length of the triangle's shortest edge.  This   */
	/*   serves as a conservative estimate of the insertion radius of the */
	/*   circumcenter's parent.  The estimate is used to ensure that      */
	/*   the algorithm terminates even if very small angles appear in     */
	/*   the input PSLG. 						      */
	// find the orientation of the triangle, basically shortest and longest edges
	orientation = longestShortestEdge(aodist, dadist, dodist);
	//printf("org: (%f,%f), dest: (%f,%f), apex: (%f,%f)\n",torg[0],torg[1],tdest[0],tdest[1],tapex[0],tapex[1]);
	/////////////////////////////////////////////////////////////////////////////////////////////
	// 123: shortest: aodist	// 213: shortest: dadist	// 312: shortest: dodist   //	
	//	middle: dadist 		//	middle: aodist 		//	middle: aodist     //
	//	longest: dodist		//	longest: dodist		//	longest: dadist    //
	// 132: shortest: aodist 	// 231: shortest: dadist 	// 321: shortest: dodist   //
	//	middle: dodist 		//	middle: dodist 		//	middle: dadist     //
	//	longest: dadist		//	longest: aodist		//	longest: aodist    //
	/////////////////////////////////////////////////////////////////////////////////////////////

	switch(orientation){
		case 123: 	// assign necessary information
				/// smallest angle corner: dest
				/// largest angle corner: apex
				xShortestEdge = xao;	yShortestEdge = yao;
				xMiddleEdge = xda;	yMiddleEdge = yda;
				xLongestEdge = xdo;	yLongestEdge = ydo;
				
				shortestEdgeDist = aodist;
				middleEdgeDist = dadist;
				longestEdgeDist = dodist;

				smallestAngleCorner = tdest;
				middleAngleCorner = torg;
				largestAngleCorner = tapex;
				break;

		case 132: 	// assign necessary information
				/// smallest angle corner: dest
				/// largest angle corner: org
				xShortestEdge = xao;	yShortestEdge = yao;
				xMiddleEdge = xdo;	yMiddleEdge = ydo;
				xLongestEdge = xda;	yLongestEdge = yda;
				
				shortestEdgeDist = aodist;
				middleEdgeDist = dodist;
				longestEdgeDist = dadist;

				smallestAngleCorner = tdest;
				middleAngleCorner = tapex;
				largestAngleCorner = torg;
				
				break;
		case 213: 	// assign necessary information
				/// smallest angle corner: org
				/// largest angle corner: apex
				xShortestEdge = xda;	yShortestEdge = yda;
				xMiddleEdge = xao;	yMiddleEdge = yao;
				xLongestEdge = xdo;	yLongestEdge = ydo;
				
				shortestEdgeDist = dadist;
				middleEdgeDist = aodist;
				longestEdgeDist = dodist;

				smallestAngleCorner = torg;
				middleAngleCorner = tdest;
				largestAngleCorner = tapex;
				break;
		case 231: 	// assign necessary information
				/// smallest angle corner: org
				/// largest angle corner: dest
				xShortestEdge = xda;	yShortestEdge = yda;
				xMiddleEdge = xdo;	yMiddleEdge = ydo;
				xLongestEdge = xao;	yLongestEdge = yao;
					
				shortestEdgeDist = dadist;
				middleEdgeDist = dodist;
				longestEdgeDist = aodist;

				smallestAngleCorner = torg;	
				middleAngleCorner = tapex;	
				largestAngleCorner = tdest;		
				break;
		case 312: 	// assign necessary information
				/// smallest angle corner: apex
				/// largest angle corner: org
				xShortestEdge = xdo;	yShortestEdge = ydo;
				xMiddleEdge = xao;	yMiddleEdge = yao;
				xLongestEdge = xda;	yLongestEdge = yda;
				
				shortestEdgeDist = dodist;
				middleEdgeDist = aodist;
				longestEdgeDist = dadist;

				smallestAngleCorner = tapex;
				middleAngleCorner = tdest;
				largestAngleCorner = torg;
				break;
		case 321: 	// assign necessary information
				/// smallest angle corner: apex
				/// largest angle corner: dest
				xShortestEdge = xdo;	yShortestEdge = ydo;
				xMiddleEdge = xda;	yMiddleEdge = yda;
				xLongestEdge = xao;	yLongestEdge = yao;
				
				shortestEdgeDist = dodist;
				middleEdgeDist = dadist;
				longestEdgeDist = aodist;

				smallestAngleCorner = tapex;
				middleAngleCorner = torg;
				largestAngleCorner = tdest;
				break;

	}// end of switch	
	// check for offcenter condition
	if (offcenter && (b->offconstant > 0.0)) {
		// origin has the smallest angle
		if(orientation == 213 || orientation == 231){
			/* Find the position of the off-center, as described by Alper Ungor. */
			dxoff = 0.5 * xShortestEdge - b->offconstant * yShortestEdge;
			dyoff = 0.5 * yShortestEdge + b->offconstant * xShortestEdge;
			/* If the off-center is closer to destination than the */
			/*   circumcenter, use the off-center instead.        */
			/// REALLY BAD CASE ///			
			if (dxoff * dxoff + dyoff * dyoff <
			    (dx - xdo) * (dx - xdo) + (dy - ydo) * (dy - ydo)) {
				dx = xdo + dxoff;
				dy = ydo + dyoff;							
			}
			/// ALMOST GOOD CASE ///
			else{
				almostGood = 1;
			}
		// destination has the smallest angle	
		}else if(orientation == 123 || orientation == 132){
			/* Find the position of the off-center, as described by Alper Ungor. */
			dxoff = 0.5 * xShortestEdge + b->offconstant * yShortestEdge;
			dyoff = 0.5 * yShortestEdge - b->offconstant * xShortestEdge;
			/* If the off-center is closer to the origin than the */
			/*   circumcenter, use the off-center instead.        */
			/// REALLY BAD CASE ///
			if (dxoff * dxoff + dyoff * dyoff < dx * dx + dy * dy) {
				dx = dxoff;
				dy = dyoff;							
			}		
			/// ALMOST GOOD CASE ///		
			else{	
				almostGood = 1;
			}
		// apex has the smallest angle	
		}else{//orientation == 312 || orientation == 321 
			/* Find the position of the off-center, as described by Alper Ungor. */
			dxoff = 0.5 * xShortestEdge - b->offconstant * yShortestEdge;
			dyoff = 0.5 * yShortestEdge + b->offconstant * xShortestEdge;
			/* If the off-center is closer to the origin than the */
			/*   circumcenter, use the off-center instead.        */
			/// REALLY BAD CASE ///
			if (dxoff * dxoff + dyoff * dyoff < dx * dx + dy * dy) {
				dx = dxoff;
				dy = dyoff;						
			}		
			/// ALMOST GOOD CASE ///		
			else{	
				almostGood = 1;
			}
		}
	}	
	// if the bad triangle is almost good, apply our approach
	if(almostGood == 1){
		
		/// calculate cosine of largest angle	///	
		cosMaxAngle = (middleEdgeDist + shortestEdgeDist - longestEdgeDist)/(2*sqrt(middleEdgeDist)*sqrt(shortestEdgeDist));
		if(cosMaxAngle < 0.0){
			// obtuse
			isObtuse = 1;
		}else if(fabs(cosMaxAngle - 0.0) <= compConst){
			// right triangle (largest angle is 90 degrees)
			isObtuse = 1;
		}else{
			// nonobtuse
			isObtuse = 0;
		}
		/// RELOCATION	(LOCAL SMOOTHING) ///
		/// check for possible relocation of one of triangle's points ///				
		relocated = doSmoothing(m,b,delotri,torg,tdest,tapex,newloc);		
		/// if relocation is possible, delete that vertex and insert a vertex at the new location ///		
		if(relocated > 0){							
			dx = newloc[0] - torg[0];
			dy = newloc[1] - torg[1];
			origin_x = torg[0];	// keep for later use
			origin_y = torg[1];									
			switch(relocated){
				case 1:
					//printf("Relocate: (%f,%f)\n", torg[0],torg[1]);			
					deletevertex(m,b,&delotri);					
					break;
				case 2:	
					//printf("Relocate: (%f,%f)\n", tdest[0],tdest[1]);			
					lnextself(delotri);
					deletevertex(m,b,&delotri);					
					break;
				case 3:
					//printf("Relocate: (%f,%f)\n", tapex[0],tapex[1]);						
					lprevself(delotri);
					deletevertex(m,b,&delotri);							
					break;				
					
			}					
		}else{	
			// calculate radius of the petal according to angle constraint
			// first find the visible region, PETAL
			// find the center of the circle and radius
			// choose minimum angle as the maximum of quality angle and the minimum angle of the bad triangle
			minangle = acos((middleEdgeDist + longestEdgeDist - shortestEdgeDist)/(2*sqrt(middleEdgeDist)*sqrt(longestEdgeDist)))*180.0/PI;	
			if(b->minangle > minangle){
				minangle = b->minangle;
			}else{
				minangle = minangle+0.5;
			}							
			petalRadius = sqrt(shortestEdgeDist)/(2*sin(minangle* PI / 180.0));					
			/// compute two possible centers of the petal ///
			// finding the center
			// first find the middle point of smallest edge
			xMidOfShortestEdge = (middleAngleCorner[0] + largestAngleCorner[0])/2.0;
			yMidOfShortestEdge = (middleAngleCorner[1] + largestAngleCorner[1])/2.0;
			// two possible centers
			xPetalCtr_1 = xMidOfShortestEdge + sqrt(petalRadius*petalRadius-(shortestEdgeDist/4))*(middleAngleCorner[1] -
				largestAngleCorner[1])/sqrt(shortestEdgeDist);
			yPetalCtr_1 = yMidOfShortestEdge + sqrt(petalRadius*petalRadius-(shortestEdgeDist/4))*(largestAngleCorner[0] - 
				middleAngleCorner[0])/sqrt(shortestEdgeDist); 
			
			xPetalCtr_2 = xMidOfShortestEdge - sqrt(petalRadius*petalRadius-(shortestEdgeDist/4))*(middleAngleCorner[1] -
				largestAngleCorner[1])/sqrt(shortestEdgeDist);
			yPetalCtr_2 = yMidOfShortestEdge - sqrt(petalRadius*petalRadius-(shortestEdgeDist/4))*(largestAngleCorner[0] - 
				middleAngleCorner[0])/sqrt(shortestEdgeDist); 		
			// find the correct circle since there will be two possible circles
			// calculate the distance to smallest angle corner
			dxcenter1 = (xPetalCtr_1 - smallestAngleCorner[0]) * (xPetalCtr_1 - smallestAngleCorner[0]);
			dycenter1 = (yPetalCtr_1 - smallestAngleCorner[1]) * (yPetalCtr_1 - smallestAngleCorner[1]);
			dxcenter2 = (xPetalCtr_2 - smallestAngleCorner[0]) * (xPetalCtr_2 - smallestAngleCorner[0]);
			dycenter2 = (yPetalCtr_2 - smallestAngleCorner[1]) * (yPetalCtr_2 - smallestAngleCorner[1]);
	
			// whichever is closer to smallest angle corner, it must be the center
			if (dxcenter1 + dycenter1 <= dxcenter2 + dycenter2){
				xPetalCtr = xPetalCtr_1;	yPetalCtr = yPetalCtr_1;
			}else{
				xPetalCtr = xPetalCtr_2;	yPetalCtr = yPetalCtr_2;
			}
			/// find the third point of the neighbor triangle  ///
			neighborNotFound_first = getNeighborsVertex(m, badotri, middleAngleCorner[0], middleAngleCorner[1], 
						smallestAngleCorner[0], smallestAngleCorner[1] ,thirdPoint, &neighborotri);
			/// find the circumcenter of the neighbor triangle ///
			dxFirstSuggestion = dx;	// if we cannot find any appropriate suggestion, we use circumcenter
			dyFirstSuggestion = dy; 
			/// before checking the neighbor, find the petal and slab intersections ///
			// calculate the intersection point of the petal and the slab lines
			// first find the vector			
			// distance between xmid and petal center			
			dist = sqrt((xPetalCtr - xMidOfShortestEdge)*(xPetalCtr - xMidOfShortestEdge) + (yPetalCtr - yMidOfShortestEdge)*(yPetalCtr - yMidOfShortestEdge));
			// find the unit vector goes from mid point to petal center			
			line_vector_x = (xPetalCtr - xMidOfShortestEdge)/dist;
			line_vector_y = (yPetalCtr - yMidOfShortestEdge)/dist;
			// find the third point other than p and q
			petal_bisector_x = xPetalCtr + line_vector_x * petalRadius;
			petal_bisector_y = yPetalCtr + line_vector_y * petalRadius;
			alpha = (2.0 * b->maxangle + minangle - 180.0) * PI/180.0; 
			// rotate the vector cw around the petal center			
			x_1 = petal_bisector_x*cos(alpha) + petal_bisector_y*sin(alpha) + xPetalCtr  - xPetalCtr *cos(alpha) - yPetalCtr*sin(alpha);
			y_1 = -petal_bisector_x*sin(alpha) + petal_bisector_y*cos(alpha) + yPetalCtr + xPetalCtr *sin(alpha) - yPetalCtr*cos(alpha);
			// rotate the vector ccw around the petal center			
			x_2 = petal_bisector_x*cos(alpha) - petal_bisector_y*sin(alpha) + xPetalCtr - xPetalCtr*cos(alpha) + yPetalCtr*sin(alpha);
			y_2 = petal_bisector_x*sin(alpha) + petal_bisector_y*cos(alpha) + yPetalCtr - xPetalCtr*sin(alpha) - yPetalCtr*cos(alpha);	
			// we need to find correct intersection point, since there are two possibilities
			// weather it is obtuse/acute the one closer to the minimum angle corner is the first direction
			isCorrect = chooseCorrectPoint(x_2, y_2,middleAngleCorner[0], middleAngleCorner[1],x_1, y_1,1);			
			// make sure which point is the correct one to be considered				
			if(isCorrect == 1){			
				petal_slab_inter_x_first = x_1;
				petal_slab_inter_y_first = y_1;
				petal_slab_inter_x_second = x_2;
				petal_slab_inter_y_second = y_2;
			}else{
				petal_slab_inter_x_first = x_2;
				petal_slab_inter_y_first = y_2;
				petal_slab_inter_x_second = x_1;
				petal_slab_inter_y_second = y_1;
			}		
			/// choose the correct intersection point ///
			// calculate middle point of the longest edge(bisector)
			xMidOfLongestEdge = (middleAngleCorner[0]+smallestAngleCorner[0])/2.0;	
			yMidOfLongestEdge = (middleAngleCorner[1]+smallestAngleCorner[1])/2.0;
			// if there is a neighbor triangle
			if(neighborNotFound_first == 0){
				org(neighborotri, neighborvertex_1);
				dest(neighborotri, neighborvertex_2);	
				apex(neighborotri, neighborvertex_3);		
				// now calculate neighbor's circumcenter which is the voronoi site
				findcircumcenter(m, b, neighborvertex_1, neighborvertex_2,neighborvertex_3,neighborCircumcenter, &xi_tmp, &eta_tmp, 0);
				/// compute petal and Voronoi edge intersection ///						
				// in order to avoid degenerate cases, we need to do a vector based calculation for line		
				vector_x = (middleAngleCorner[1] - smallestAngleCorner[1]);//(-y, x)
				vector_y = smallestAngleCorner[0] - middleAngleCorner[0];
				vector_x = myCircumcenter[0] + vector_x;
				vector_y = myCircumcenter[1] + vector_y;
				// by intersecting bisectors you will end up with the one you want to walk on
				// then this line and circle should be intersected
				circleLineIntersection(myCircumcenter[0], myCircumcenter[1], vector_x, vector_y, 
						xPetalCtr, yPetalCtr, petalRadius, p);		
				// we need to find correct intersection point, since line intersects circle twice
				isCorrect = chooseCorrectPoint(xMidOfLongestEdge, yMidOfLongestEdge, p[3], p[4], 
							myCircumcenter[0], myCircumcenter[1],isObtuse);			
				// make sure which point is the correct one to be considered
				if(isCorrect == 1){			
					inter_x = p[3];
					inter_y = p[4];
				}else{
					inter_x = p[1];
					inter_y = p[2];
				}
				//----------------------hale new first direction: for slab calculation---------------//
				// calculate the intersection of angle lines and Voronoi
				linepnt1_x = middleAngleCorner[0];
				linepnt1_y = middleAngleCorner[1];
				// vector from middleAngleCorner to largestAngleCorner
				line_vector_x = largestAngleCorner[0] - middleAngleCorner[0];
				line_vector_y = largestAngleCorner[1] - middleAngleCorner[1];
				// rotate the vector around middleAngleCorner in cw by maxangle degrees				
				linepnt2_x = petal_slab_inter_x_first;
				linepnt2_y = petal_slab_inter_y_first;
				// now calculate the intersection of two lines
				lineLineIntersection (myCircumcenter[0],myCircumcenter[1],vector_x,vector_y,linepnt1_x,linepnt1_y,linepnt2_x,linepnt2_y,line_p);
				// check if there is a suitable intersection
				if(line_p[0] > 0.0){
					line_inter_x = line_p[1];
					line_inter_y = line_p[2];
				}else{
#ifdef _DEBUG
					// for debugging (to make sure)
					printf("1) No intersection between two lines!\n");
					printf("(%.8f,%.8f) (%.8f,%.8f) (%.8f,%.8f) (%.8f,%.8f)\n",myCircumcenter[0],myCircumcenter[1],vector_x,vector_y,linepnt1_x,linepnt1_y,linepnt2_x,linepnt2_y);
#endif
				}
			
				//---------------------------------------------------------------------//
				/// check if there is a Voronoi vertex between before intersection ///
				// check if the voronoi vertex is between the intersection and circumcenter
				pointBetweenPoints(inter_x, inter_y, myCircumcenter[0], myCircumcenter[1], 
						neighborCircumcenter[0], neighborCircumcenter[1], voronoiOrInter);
				
				/// determine the point to be suggested ///
				if(p[0] > 0.0){ // there is at least one intersection point
					// if it is between circumcenter and intersection	
					// if it returns 1.0 this means we have a voronoi vertex within feasible region
					if(fabs(voronoiOrInter[0] - 1.0) <= compConst){
						//-----------------hale new continues 1------------------//
						// now check if the line intersection is between cc and voronoi
						pointBetweenPoints(voronoiOrInter[2], voronoiOrInter[3],myCircumcenter[0], myCircumcenter[1],line_inter_x, line_inter_y, line_result);
						if(fabs(line_result[0] - 1.0) <= compConst && line_p[0] > 0.0){
							// check if we can go further by picking the slab line and petal intersection
							// calculate the distance to the smallest angle corner
							// check if we create a bad triangle or not
							if(((smallestAngleCorner[0]-petal_slab_inter_x_first)*(smallestAngleCorner[0]-petal_slab_inter_x_first) +
							(smallestAngleCorner[1]-petal_slab_inter_y_first)*(smallestAngleCorner[1]-petal_slab_inter_y_first) > 
						lengthConst * ( (smallestAngleCorner[0]-line_inter_x) *
								(smallestAngleCorner[0]-line_inter_x) + 
								(smallestAngleCorner[1]-line_inter_y) *
								(smallestAngleCorner[1]-line_inter_y) ))  
								&& (testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &petal_slab_inter_x_first, &petal_slab_inter_y_first) != 0) 
								&& minDistanceToNeigbor(m, b, petal_slab_inter_x_first, petal_slab_inter_y_first,&neighborotri) > minDistanceToNeigbor(m, b, line_inter_x, line_inter_y,&neighborotri)){
	// 							
								/// check the neighbor's vertices also, which one if better
								
								//slab and petal intersection is advised
								dxFirstSuggestion = petal_slab_inter_x_first - torg[0];
								dyFirstSuggestion = petal_slab_inter_y_first - torg[1];	
							}else{ // slab intersection point is further away
								if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &line_inter_x, &line_inter_y) != 0){											
									// apply perturbation
									// find the distance between circumcenter and intersection point
									d = sqrt((line_inter_x - myCircumcenter[0]) * (line_inter_x - myCircumcenter[0]) + 
										(line_inter_y - myCircumcenter[1]) * (line_inter_y - myCircumcenter[1]));
									// then find the vector going from intersection point to circumcenter
									ax = myCircumcenter[0] - line_inter_x;
									ay = myCircumcenter[1] - line_inter_y;
									
									ax = ax / d;
									ay = ay / d;
									// now calculate the new intersection point which is perturbated towards the circumcenter
									line_inter_x = line_inter_x + ax * pertConst * sqrt(shortestEdgeDist);
									line_inter_y = line_inter_y + ay * pertConst * sqrt(shortestEdgeDist);
									if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &line_inter_x, &line_inter_y) != 0){							
										// go back to circumcenter
										dxFirstSuggestion = dx;
										dyFirstSuggestion = dy; 
											
													
									}else{
										// intersection point is suggested
										dxFirstSuggestion = line_inter_x - torg[0];
										dyFirstSuggestion = line_inter_y - torg[1]; 
										
										
									}
																		
					
								}else{// we are not creating a bad triangle
									// slab intersection is advised
									dxFirstSuggestion  = line_result[2] - torg[0];
									dyFirstSuggestion = line_result[3] - torg[1];					
									
								}
							}
						//------------------------------------------------------//
						}else{
							/// NOW APPLY A BREADTH-FIRST SEARCH ON THE VORONOI
							if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &neighborCircumcenter[0], &neighborCircumcenter[1]) != 0){
								// go back to circumcenter
								dxFirstSuggestion = dx;
								dyFirstSuggestion = dy;
								
														
							}else{ 							
								// we are not creating a bad triangle
								// neighbor's circumcenter is suggested
								dxFirstSuggestion = voronoiOrInter[2] - torg[0];
								dyFirstSuggestion = voronoiOrInter[3] - torg[1];
													
							
							}
						}
												
					}else{ // there is no voronoi vertex between intersection point and circumcenter
						//-----------------hale new continues 2-----------------//
						// now check if the line intersection is between cc and intersection point
						pointBetweenPoints(inter_x, inter_y,myCircumcenter[0], myCircumcenter[1],line_inter_x, line_inter_y, line_result);
						if(fabs(line_result[0] - 1.0) <= compConst && line_p[0] > 0.0){	
							// check if we can go further by picking the slab line and petal intersection
							// calculate the distance to the smallest angle corner
							if(((smallestAngleCorner[0]-petal_slab_inter_x_first)*(smallestAngleCorner[0]-petal_slab_inter_x_first) +
						(smallestAngleCorner[1]-petal_slab_inter_y_first)*(smallestAngleCorner[1]-petal_slab_inter_y_first) > 
						lengthConst * ( (smallestAngleCorner[0]-line_inter_x) *
								(smallestAngleCorner[0]-line_inter_x) + 
								(smallestAngleCorner[1]-line_inter_y) *
								(smallestAngleCorner[1]-line_inter_y) ))  
								&& (testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &petal_slab_inter_x_first, &petal_slab_inter_y_first) != 0)
								&& minDistanceToNeigbor(m, b, petal_slab_inter_x_first, petal_slab_inter_y_first,&neighborotri) > minDistanceToNeigbor(m, b, line_inter_x, line_inter_y,&neighborotri) ){
								//slab and petal intersection is advised
								dxFirstSuggestion  = petal_slab_inter_x_first - torg[0];
								dyFirstSuggestion = petal_slab_inter_y_first - torg[1];			
								
							}else{ // slab intersection point is further away							
								if(testTriangleAngle(b, &largestAngleCorner[0], &largestAngleCorner[1], &middleAngleCorner[0], &middleAngleCorner[1], &line_inter_x, &line_inter_y) != 0){			
									// apply perturbation
									// find the distance between circumcenter and intersection point
									d = sqrt((line_inter_x - myCircumcenter[0]) * (line_inter_x - myCircumcenter[0]) + 
										(line_inter_y - myCircumcenter[1]) * (line_inter_y - myCircumcenter[1]));
									// then find the vector going from intersection point to circumcenter
									ax = myCircumcenter[0] - line_inter_x;
									ay = myCircumcenter[1] - line_inter_y;
									
									ax = ax / d;
									ay = ay / d;
									// now calculate the new intersection point which is perturbated towards the circumcenter
									line_inter_x = line_inter_x + ax * pertConst * sqrt(shortestEdgeDist);
									line_inter_y = line_inter_y + ay * pertConst * sqrt(shortestEdgeDist);
									if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &line_inter_x, &line_inter_y) != 0){							
										// go back to circumcenter
										dxFirstSuggestion = dx;
										dyFirstSuggestion = dy; 
										
									}else{
										// intersection point is suggested
										dxFirstSuggestion = line_inter_x - torg[0];
										dyFirstSuggestion = line_inter_y - torg[1]; 
										
									}		
										
		
								}else{// we are not creating a bad triangle
									// slab intersection is advised
									dxFirstSuggestion = line_result[2] - torg[0];
									dyFirstSuggestion = line_result[3] - torg[1];					
									
								}
							}
						//------------------------------------------------------//
						
						}else{						
							
							if(testTriangleAngle(b, &largestAngleCorner[0], &largestAngleCorner[1], &middleAngleCorner[0], &middleAngleCorner[1], &inter_x, &inter_y) != 0){
								//printf("testtriangle returned false! bad triangle\n");	
								// if it is inside feasible region, then insert v2				
								// apply perturbation
								// find the distance between circumcenter and intersection point
								d = sqrt((inter_x - myCircumcenter[0]) * (inter_x - myCircumcenter[0]) + 
									(inter_y - myCircumcenter[1]) * (inter_y - myCircumcenter[1]));
								// then find the vector going from intersection point to circumcenter
								ax = myCircumcenter[0] - inter_x;
								ay = myCircumcenter[1] - inter_y;
								
								ax = ax / d;
								ay = ay / d;
								// now calculate the new intersection point which is perturbated towards the circumcenter
								inter_x = inter_x + ax * pertConst * sqrt(shortestEdgeDist);
								inter_y = inter_y + ay * pertConst * sqrt(shortestEdgeDist);
								if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &inter_x, &inter_y) != 0){							
									// go back to circumcenter
									dxFirstSuggestion = dx;
									dyFirstSuggestion = dy; 
									
								
												
								}else{
									// intersection point is suggested
									dxFirstSuggestion = inter_x - torg[0];
									dyFirstSuggestion = inter_y - torg[1]; 	
									
								}
							}else{								
								// intersection point is suggested
								dxFirstSuggestion = inter_x - torg[0];
								dyFirstSuggestion = inter_y - torg[1]; 	
								
							}
						}			
					}
					/// if it is an acute triangle, check if it is a good enough location ///
					// for acute triangle case, we need to check if it is ok to use either of them
					if( (smallestAngleCorner[0]-myCircumcenter[0])*(smallestAngleCorner[0]-myCircumcenter[0]) +
						(smallestAngleCorner[1]-myCircumcenter[1])*(smallestAngleCorner[1]-myCircumcenter[1]) > 
						lengthConst * ( (smallestAngleCorner[0]-(dxFirstSuggestion + torg[0])) *
								(smallestAngleCorner[0]-(dxFirstSuggestion + torg[0])) + 
								(smallestAngleCorner[1]-(dyFirstSuggestion + torg[1])) *
								(smallestAngleCorner[1]-(dyFirstSuggestion + torg[1])) ) ){
						// use circumcenter
						dxFirstSuggestion = dx;
						dyFirstSuggestion = dy; 
						
					}// else we stick to what we have found	
				}// intersection point
				
			}// if it is on the boundary, meaning no neighbor triangle in this direction, try other direction	
	
			/// DO THE SAME THING FOR THE OTHER DIRECTION ///
			/// find the third point of the neighbor triangle  ///
			neighborNotFound_second = getNeighborsVertex(m, badotri, largestAngleCorner[0], largestAngleCorner[1], 
						smallestAngleCorner[0], smallestAngleCorner[1] , thirdPoint, &neighborotri);
			/// find the circumcenter of the neighbor triangle ///
			dxSecondSuggestion = dx;	// if we cannot find any appropriate suggestion, we use circumcenter
			dySecondSuggestion = dy; 	
			
			/// choose the correct intersection point ///
			// calculate middle point of the longest edge(bisector)
			xMidOfMiddleEdge = (largestAngleCorner[0]+smallestAngleCorner[0])/2.0;	
			yMidOfMiddleEdge = (largestAngleCorner[1]+smallestAngleCorner[1])/2.0;
			// if there is a neighbor triangle
			if(neighborNotFound_second == 0){
				org(neighborotri, neighborvertex_1);
				dest(neighborotri, neighborvertex_2);	
				apex(neighborotri, neighborvertex_3);		
				// now calculate neighbor's circumcenter which is the voronoi site
				findcircumcenter(m, b, neighborvertex_1, neighborvertex_2,neighborvertex_3,neighborCircumcenter, &xi_tmp, &eta_tmp, 0);	
	
				/// compute petal and Voronoi edge intersection ///
				// in order to avoid degenerate cases, we need to do a vector based calculation for line		
				vector_x = (largestAngleCorner[1] - smallestAngleCorner[1]);//(-y, x)
				vector_y = smallestAngleCorner[0] - largestAngleCorner[0];
				vector_x = myCircumcenter[0] + vector_x;
				vector_y = myCircumcenter[1] + vector_y;
				
				
				// by intersecting bisectors you will end up with the one you want to walk on
				// then this line and circle should be intersected
				circleLineIntersection(myCircumcenter[0], myCircumcenter[1], vector_x, vector_y, 
						xPetalCtr, yPetalCtr, petalRadius, p);
							
				// we need to find correct intersection point, since line intersects circle twice
				// this direction is always ACUTE
				isCorrect = chooseCorrectPoint(xMidOfMiddleEdge, yMidOfMiddleEdge, p[3], p[4], 
							myCircumcenter[0], myCircumcenter[1],0/*(isObtuse+1)%2*/);				
				// make sure which point is the correct one to be considered
				if(isCorrect == 1){			
					inter_x = p[3];
					inter_y = p[4];
				}else{
					inter_x = p[1];
					inter_y = p[2];
				}
				//----------------------hale new second direction:for slab calculation---------------//			
				// calculate the intersection of angle lines and Voronoi
				linepnt1_x = largestAngleCorner[0];
				linepnt1_y = largestAngleCorner[1];
				// vector from largestAngleCorner to middleAngleCorner 
				line_vector_x = middleAngleCorner[0] - largestAngleCorner[0];
				line_vector_y = middleAngleCorner[1] - largestAngleCorner[1];
				// rotate the vector around largestAngleCorner in ccw by maxangle degrees				
				linepnt2_x = petal_slab_inter_x_second;
				linepnt2_y = petal_slab_inter_y_second;
				// now calculate the intersection of two lines
				lineLineIntersection (myCircumcenter[0],myCircumcenter[1],vector_x,vector_y,linepnt1_x,linepnt1_y,linepnt2_x,linepnt2_y,line_p);
				// check if there is a suitable intersection
				if(line_p[0] > 0.0){
					line_inter_x = line_p[1];
					line_inter_y = line_p[2];
				}else{
#ifdef _DEBUG
					// for debugging (to make sure)
					printf("2) No intersection between two lines!\n");
					printf("(%.8f,%.8f) (%.8f,%.8f) (%.8f,%.8f) (%.8f,%.8f)\n",myCircumcenter[0],myCircumcenter[1],vector_x,vector_y,linepnt1_x,linepnt1_y,linepnt2_x,linepnt2_y);
#endif
				}		
				//---------------------------------------------------------------------//
				/// check if there is a Voronoi vertex between before intersection ///
				// check if the voronoi vertex is between the intersection and circumcenter
				pointBetweenPoints(inter_x, inter_y, myCircumcenter[0], myCircumcenter[1], 
						neighborCircumcenter[0], neighborCircumcenter[1], voronoiOrInter);			
				/// determine the point to be suggested ///
				if(p[0] > 0.0){ // there is at least one intersection point				
					// if it is between circumcenter and intersection	
					// if it returns 1.0 this means we have a voronoi vertex within feasible region
					if(fabs(voronoiOrInter[0] - 1.0) <= compConst){										
						//-----------------hale new continues 1------------------//
						// now check if the line intersection is between cc and voronoi
						pointBetweenPoints(voronoiOrInter[2], voronoiOrInter[3],myCircumcenter[0], myCircumcenter[1],line_inter_x, line_inter_y, line_result);
						if(fabs(line_result[0] - 1.0) <= compConst && line_p[0] > 0.0){						
							// check if we can go further by picking the slab line and petal intersection
							// calculate the distance to the smallest angle corner
	// 						
							if(((smallestAngleCorner[0]-petal_slab_inter_x_second)*(smallestAngleCorner[0]-petal_slab_inter_x_second) +
						(smallestAngleCorner[1]-petal_slab_inter_y_second)*(smallestAngleCorner[1]-petal_slab_inter_y_second) > 
						lengthConst * ( (smallestAngleCorner[0]-line_inter_x) *
								(smallestAngleCorner[0]-line_inter_x) + 
								(smallestAngleCorner[1]-line_inter_y) *
								(smallestAngleCorner[1]-line_inter_y) ))  
								&& (testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &petal_slab_inter_x_second, &petal_slab_inter_y_second) != 0)
								&& minDistanceToNeigbor(m, b, petal_slab_inter_x_second, petal_slab_inter_y_second,&neighborotri) > minDistanceToNeigbor(m, b, line_inter_x, line_inter_y,&neighborotri)){							
								// slab and petal intersection is advised
								dxSecondSuggestion = petal_slab_inter_x_second - torg[0];
								dySecondSuggestion = petal_slab_inter_y_second - torg[1];
								
								
							}else{ // slab intersection point is further away	
								if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &line_inter_x, &line_inter_y) != 0){											
									// apply perturbation
									// find the distance between circumcenter and intersection point
									d = sqrt((line_inter_x - myCircumcenter[0]) * (line_inter_x - myCircumcenter[0]) + 
										(line_inter_y - myCircumcenter[1]) * (line_inter_y - myCircumcenter[1]));
									// then find the vector going from intersection point to circumcenter
									ax = myCircumcenter[0] - line_inter_x;
									ay = myCircumcenter[1] - line_inter_y;
									
									ax = ax / d;
									ay = ay / d;
									// now calculate the new intersection point which is perturbated towards the circumcenter
									line_inter_x = line_inter_x + ax * pertConst * sqrt(shortestEdgeDist);
									line_inter_y = line_inter_y + ay * pertConst * sqrt(shortestEdgeDist);
									if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &line_inter_x, &line_inter_y) != 0){							
										// go back to circumcenter
										dxSecondSuggestion = dx;
										dySecondSuggestion = dy; 
												
													
									}else{
										// intersection point is suggested
										dxSecondSuggestion = line_inter_x - torg[0];
										dySecondSuggestion = line_inter_y - torg[1]; 
										
									}
																		
					
								}else{// we are not creating a bad triangle
									// slab intersection is advised
									dxSecondSuggestion  = line_result[2] - torg[0];
									dySecondSuggestion = line_result[3] - torg[1];					
									
								}
							}
						//------------------------------------------------------//
						}else{
							if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &neighborCircumcenter[0], &neighborCircumcenter[1]) != 0){
								// go back to circumcenter
								dxSecondSuggestion = dx;
								dySecondSuggestion = dy;
								
														
							}else{ // we are not creating a bad triangle
								// neighbor's circumcenter is suggested
								dxSecondSuggestion = voronoiOrInter[2] - torg[0];
								dySecondSuggestion = voronoiOrInter[3] - torg[1];
								
							
							}
						}					
					}else{ // there is no voronoi vertex between intersection point and circumcenter
						//-----------------hale new continues 2-----------------//
						// now check if the line intersection is between cc and intersection point
						pointBetweenPoints(inter_x, inter_y,myCircumcenter[0], myCircumcenter[1],line_inter_x, line_inter_y, line_result);
						if(fabs(line_result[0] - 1.0) <= compConst && line_p[0] > 0.0){
							// check if we can go further by picking the slab line and petal intersection
							// calculate the distance to the smallest angle corner
							if(((smallestAngleCorner[0]-petal_slab_inter_x_second)*(smallestAngleCorner[0]-petal_slab_inter_x_second) +
						(smallestAngleCorner[1]-petal_slab_inter_y_second)*(smallestAngleCorner[1]-petal_slab_inter_y_second) > 
						lengthConst * ( (smallestAngleCorner[0]-line_inter_x) *
								(smallestAngleCorner[0]-line_inter_x) + 
								(smallestAngleCorner[1]-line_inter_y) *
								(smallestAngleCorner[1]-line_inter_y) ))  
								&& (testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &petal_slab_inter_x_second, &petal_slab_inter_y_second) != 0)
								&& minDistanceToNeigbor(m, b, petal_slab_inter_x_second, petal_slab_inter_y_second,&neighborotri) > minDistanceToNeigbor(m, b, line_inter_x, line_inter_y,&neighborotri)){
								// slab and petal intersection is advised
								dxSecondSuggestion  = petal_slab_inter_x_second - torg[0];
								dySecondSuggestion = petal_slab_inter_y_second - torg[1];	
																
							}else{ // slab intersection point is further away							;
								if(testTriangleAngle(b, &largestAngleCorner[0], &largestAngleCorner[1], &middleAngleCorner[0], &middleAngleCorner[1], &line_inter_x, &line_inter_y) != 0){				
									// apply perturbation
									// find the distance between circumcenter and intersection point
									d = sqrt((line_inter_x - myCircumcenter[0]) * (line_inter_x - myCircumcenter[0]) + 
										(line_inter_y - myCircumcenter[1]) * (line_inter_y - myCircumcenter[1]));
									// then find the vector going from intersection point to circumcenter
									ax = myCircumcenter[0] - line_inter_x;
									ay = myCircumcenter[1] - line_inter_y;
									
									ax = ax / d;
									ay = ay / d;
									// now calculate the new intersection point which is perturbated towards the circumcenter
									line_inter_x = line_inter_x + ax * pertConst * sqrt(shortestEdgeDist);
									line_inter_y = line_inter_y + ay * pertConst * sqrt(shortestEdgeDist);
									if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &line_inter_x, &line_inter_y) != 0){							
										// go back to circumcenter
										dxSecondSuggestion = dx;
										dySecondSuggestion = dy; 
										
									}else{
										// intersection point is suggested
										dxSecondSuggestion = line_inter_x - torg[0];
										dySecondSuggestion = line_inter_y - torg[1]; 
										
									}
										
											
								}else{// we are not creating a bad triangle
									// slab intersection is advised
									dxSecondSuggestion = line_result[2] - torg[0];
									dySecondSuggestion = line_result[3] - torg[1];					
									
								}
							}
						//------------------------------------------------------//
						
						}else{						
							if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &inter_x, &inter_y) != 0){
							// if it is inside feasible region, then insert v2				
								// apply perturbation
								// find the distance between circumcenter and intersection point
								d = sqrt((inter_x - myCircumcenter[0]) * (inter_x - myCircumcenter[0]) + 
									(inter_y - myCircumcenter[1]) * (inter_y - myCircumcenter[1]));
								// then find the vector going from intersection point to circumcenter
								ax = myCircumcenter[0] - inter_x;
								ay = myCircumcenter[1] - inter_y;
								
								ax = ax / d;
								ay = ay / d;
								// now calculate the new intersection point which is perturbated towards the circumcenter
								inter_x = inter_x + ax * pertConst * sqrt(shortestEdgeDist);
								inter_y = inter_y + ay * pertConst * sqrt(shortestEdgeDist);
								if(testTriangleAngle(b, &middleAngleCorner[0], &middleAngleCorner[1], &largestAngleCorner[0], &largestAngleCorner[1], &inter_x, &inter_y) != 0){							
									// go back to circumcenter
									dxSecondSuggestion = dx;
									dySecondSuggestion = dy; 
											
												
								}else{
									// intersection point is suggested
									dxSecondSuggestion = inter_x - torg[0];
									dySecondSuggestion = inter_y - torg[1]; 
									
								}
							}else{
							
								// intersection point is suggested
								dxSecondSuggestion = inter_x - torg[0];
								dySecondSuggestion = inter_y - torg[1]; 
								
							}
						}
					}
						
					/// if it is an acute triangle, check if it is a good enough location ///
					// for acute triangle case, we need to check if it is ok to use either of them
					if( (smallestAngleCorner[0]-myCircumcenter[0])*(smallestAngleCorner[0]-myCircumcenter[0]) +
						(smallestAngleCorner[1]-myCircumcenter[1])*(smallestAngleCorner[1]-myCircumcenter[1]) > 
						lengthConst * ( (smallestAngleCorner[0]-(dxSecondSuggestion + torg[0])) *
								(smallestAngleCorner[0]-(dxSecondSuggestion + torg[0])) + 
								(smallestAngleCorner[1]-(dySecondSuggestion + torg[1])) *
								(smallestAngleCorner[1]-(dySecondSuggestion + torg[1])) ) ){
						// use circumcenter
						dxSecondSuggestion = dx;
						dySecondSuggestion = dy; 
						
					}// else we stick on what we have found	
				}
			}// if it is on the boundary, meaning no neighbor triangle in this direction, the other direction might be ok	
			if(isObtuse == 1){									
				if(neighborNotFound_first == 1 && neighborNotFound_second == 1){
					//obtuse: check if the other direction works	
					if(justAcute*( (smallestAngleCorner[0]-(xMidOfMiddleEdge))*
						(smallestAngleCorner[0]-(xMidOfMiddleEdge)) +
				   		(smallestAngleCorner[1]-(yMidOfMiddleEdge))*
						(smallestAngleCorner[1]-(yMidOfMiddleEdge)) ) >
				   		(smallestAngleCorner[0]-(xMidOfLongestEdge))*
						(smallestAngleCorner[0]-(xMidOfLongestEdge)) +
				   		(smallestAngleCorner[1]-(yMidOfLongestEdge))*
						(smallestAngleCorner[1]-(yMidOfLongestEdge)) ){
						dx = dxSecondSuggestion;
						dy = dySecondSuggestion;
													
					}else{					
						dx = dxFirstSuggestion;
						dy = dyFirstSuggestion;	
												
					}	
				}else if(neighborNotFound_first == 1){
					//obtuse: check if the other direction works	
					if(justAcute*( (smallestAngleCorner[0]-(dxSecondSuggestion+torg[0]))*
							(smallestAngleCorner[0]-(dxSecondSuggestion+torg[0])) +
							(smallestAngleCorner[1]-(dySecondSuggestion+torg[1]))*
							(smallestAngleCorner[1]-(dySecondSuggestion+torg[1])) ) >
							(smallestAngleCorner[0]-(xMidOfLongestEdge))*
							(smallestAngleCorner[0]-(xMidOfLongestEdge)) +
							(smallestAngleCorner[1]-(yMidOfLongestEdge))*
							(smallestAngleCorner[1]-(yMidOfLongestEdge)) ){
						dx = dxSecondSuggestion;
						dy = dySecondSuggestion;
														
					}else{					
						dx = dxFirstSuggestion;
						dy = dyFirstSuggestion;	
													
					}
				}else if(neighborNotFound_second == 1){
					//obtuse: check if the other direction works	
					if(justAcute*( (smallestAngleCorner[0]-(xMidOfMiddleEdge))*
							(smallestAngleCorner[0]-(xMidOfMiddleEdge)) +
							(smallestAngleCorner[1]-(yMidOfMiddleEdge))*
							(smallestAngleCorner[1]-(yMidOfMiddleEdge)) ) >
							(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0]))*
							(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0])) +
							(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1]))*
							(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1])) ){
						dx = dxSecondSuggestion;
						dy = dySecondSuggestion;
														
					}else{					
						dx = dxFirstSuggestion;
						dy = dyFirstSuggestion;	
												
					}
				}else{
					//obtuse: check if the other direction works	
					if(justAcute*( (smallestAngleCorner[0]-(dxSecondSuggestion+torg[0]))*
						(smallestAngleCorner[0]-(dxSecondSuggestion+torg[0])) +
				   		(smallestAngleCorner[1]-(dySecondSuggestion+torg[1]))*
						(smallestAngleCorner[1]-(dySecondSuggestion+torg[1])) ) >
				   		(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0]))*
						(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0])) +
				   		(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1]))*
						(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1])) ){
						dx = dxSecondSuggestion;
						dy = dySecondSuggestion;
															
					}else{					
						dx = dxFirstSuggestion;
						dy = dyFirstSuggestion;	
													
					}
				}
								
			}else{ // acute : consider other direction
				if(neighborNotFound_first == 1 && neighborNotFound_second == 1){
					//obtuse: check if the other direction works	
					if(justAcute*( (smallestAngleCorner[0]-(xMidOfMiddleEdge))*
						(smallestAngleCorner[0]-(xMidOfMiddleEdge)) +
				   		(smallestAngleCorner[1]-(yMidOfMiddleEdge))*
						(smallestAngleCorner[1]-(yMidOfMiddleEdge)) ) >
				   		(smallestAngleCorner[0]-(xMidOfLongestEdge))*
						(smallestAngleCorner[0]-(xMidOfLongestEdge)) +
				   		(smallestAngleCorner[1]-(yMidOfLongestEdge))*
						(smallestAngleCorner[1]-(yMidOfLongestEdge)) ){
						dx = dxSecondSuggestion;
						dy = dySecondSuggestion;
															
					}else{					
						dx = dxFirstSuggestion;
						dy = dyFirstSuggestion;	
													
					}	
				}else if(neighborNotFound_first == 1){
					//obtuse: check if the other direction works	
					if(justAcute*( (smallestAngleCorner[0]-(dxSecondSuggestion+torg[0]))*
							(smallestAngleCorner[0]-(dxSecondSuggestion+torg[0])) +
							(smallestAngleCorner[1]-(dySecondSuggestion+torg[1]))*
							(smallestAngleCorner[1]-(dySecondSuggestion+torg[1])) ) >
							(smallestAngleCorner[0]-(xMidOfLongestEdge))*
							(smallestAngleCorner[0]-(xMidOfLongestEdge)) +
							(smallestAngleCorner[1]-(yMidOfLongestEdge))*
							(smallestAngleCorner[1]-(yMidOfLongestEdge)) ){
						dx = dxSecondSuggestion;
						dy = dySecondSuggestion;
														
					}else{					
						dx = dxFirstSuggestion;
						dy = dyFirstSuggestion;	
													
					}
				}else if(neighborNotFound_second == 1){
					//obtuse: check if the other direction works	
					if(justAcute*( (smallestAngleCorner[0]-(xMidOfMiddleEdge))*
							(smallestAngleCorner[0]-(xMidOfMiddleEdge)) +
							(smallestAngleCorner[1]-(yMidOfMiddleEdge))*
							(smallestAngleCorner[1]-(yMidOfMiddleEdge)) ) >
							(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0]))*
							(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0])) +
							(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1]))*
							(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1])) ){
						dx = dxSecondSuggestion;
						dy = dySecondSuggestion;
															
					}else{					
						dx = dxFirstSuggestion;
						dy = dyFirstSuggestion;	
													
					}
				}else{
					//obtuse: check if the other direction works	
					if(justAcute*( (smallestAngleCorner[0]-(dxSecondSuggestion+torg[0]))*
						(smallestAngleCorner[0]-(dxSecondSuggestion+torg[0])) +
				   		(smallestAngleCorner[1]-(dySecondSuggestion+torg[1]))*
						(smallestAngleCorner[1]-(dySecondSuggestion+torg[1])) ) >
				   		(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0]))*
						(smallestAngleCorner[0]-(dxFirstSuggestion+torg[0])) +
				   		(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1]))*
						(smallestAngleCorner[1]-(dyFirstSuggestion+torg[1])) ){
						dx = dxSecondSuggestion;
						dy = dySecondSuggestion;
														
					}else{					
						dx = dxFirstSuggestion;
						dy = dyFirstSuggestion;	
													
					}
				}								
				
			}// end if obtuse
		}// end of relocation				 
	}// end of almostGood	

	if(relocated <= 0){
		circumcenter[0] = torg[0] + dx;
		circumcenter[1] = torg[1] + dy;
	}else{
		circumcenter[0] = origin_x + dx;
		circumcenter[1] = origin_y + dy;
	}	
	*xi = (yao * dx - xao * dy) * (2.0 * denominator);
	*eta = (xdo * dy - ydo * dx) * (2.0 * denominator);	

}// end of findNewSPLocationWithMaxAngle()
/*============================HELPER FUNCTIONS==============================*/
//---------------------------------------------------------------------------------//
// longestShortestEdge() 
// Given square of edge lengths of a triangle,
// determine its orientation
// Returns a number indicating an orientation
// 123: shortest: aodist	// 213: shortest: dadist	// 312: shortest: dodist	
//	middle: dadist 		//	middle: aodist 		//	middle: aodist 
//	longest: dodist		//	longest: dodist		//	longest: dadist
// 132: shortest: aodist 	// 231: shortest: dadist 	// 321: shortest: dodist 
//	middle: dodist 		//	middle: dodist 		//	middle: dadist 
//	longest: dadist		//	longest: aodist		//	longest: aodist
// 
//---------------------------------------------------------------------------------//
int longestShortestEdge(REAL aodist, REAL dadist, REAL dodist){
	
	int max = 0, min = 0, mid = 0,minMidMax;
	if (dodist < aodist && dodist < dadist){
		min = 3; // apex is the smallest angle, dodist is the longest edge
		if(aodist < dadist){
			max = 2; // dadist is the longest edge 
			mid = 1; // aodist is the middle longest edge
		}else{
			max = 1; // aodist is the longest edge 
			mid = 2; // dadist is the middle longest edge
		}
	}else if (aodist < dadist){
		min = 1; // dest is the smallest angle, aodist is the biggest edge
		if(dodist < dadist){
			max = 2; // dadist is the longest edge 
			mid = 3; // dodist is the middle longest edge
		}else{
			max = 3; // dodist is the longest edge 
			mid = 2; // dadist is the middle longest edge
		}
	}else{
		min = 2; // origin is the smallest angle, dadist is the biggest edge
		if(aodist < dodist){
			max = 3; // dodist is the longest edge 
			mid = 1; // aodist is the middle longest edge
		}else{
			max = 1; // aodist is the longest edge 
			mid = 3; // dodist is the middle longest edge
		}
	}
	minMidMax = min * 100 + mid * 10 + max;
	// HANDLE ISOSCELES TRIANGLE CASE
	return minMidMax;
}// end of longestShortestEdge(

//---------------------------------------------------------------------------------//
// doSmoothing()
// Given the triangulation, a bad traingle and its vertices
// returns 1,2 or 3 if smoothing will work, 0 otherwise
// also returns the new location for the point, if somothing is possible
//---------------------------------------------------------------------------------//
int doSmoothing(mesh *m, behavior *b, 
		struct otri badotri,
		vertex torg,
		vertex tdest,
		vertex tapex,
		REAL *newloc){

    int numpoints_p = 0;// keeps the number of points in a star of point p, q, r
	int numpoints_q = 0;
	int numpoints_r = 0;
	REAL possibilities[6];//there can be more than one possibilities
	int num_pos = 0; // number of possibilities
	int flag1 = 0, flag2 = 0, flag3 = 0;
	int newLocFound = 0;
	
	REAL *points_p;// keeps the points in a star of point p, q, r
	REAL *points_q;
	REAL *points_r;

	points_p = m->acute_mem->points_p;
	points_q = m->acute_mem->points_q;
	points_r = m->acute_mem->points_r;

	/*********************** TRY TO RELOCATE POINT "p" *************************/

	// get the surrounding points of p, so this gives us the triangles
	numpoints_p = getStarPoints(m,badotri,torg,tdest,tapex,1,points_p);		
	// check if the points in counterclockwise order
// 	p1[0] = points_p[0];  p1[1] = points_p[1];
// 	p2[0] = points_p[2];  p2[1] = points_p[3];
// 	p3[0] = points_p[4];  p3[1] = points_p[5];
// 	v1 = (vertex)p1; v2 = (vertex)p2; v3 = (vertex)p3; 
// 	if(counterclockwise(m,b,v1,v2,v3) < 0){
// 		// reverse the order to ccw
// 		for(i = 0; i < numpoints_p/2; i++){
// 			temp[0] = points_p[2*i];	
// 			temp[1] = points_p[2*i+1];
// 			points_p[2*i] = points_p[2*(numpoints_p-1)-2*i];
// 			points_p[2*i+1] = points_p[2*(numpoints_p-1)+1-2*i];
// 			points_p[2*(numpoints_p-1)-2*i] = temp[0];
// 			points_p[2*(numpoints_p-1)+1-2*i] = temp[1];
// 		}
// 	}
// 	m->counterclockcount--;
	// INTERSECTION OF PETALS
	// first check whether the star angles are appropriate for relocation
	if(numpoints_p != 0 && polygonAngles(b,numpoints_p,points_p)== 0){
		//newLocFound = getPetalIntersection(m, b, numpoints_p, points_p, newloc);
		//newLocFound = getPetalIntersectionBruteForce(m, b,numpoints_p, points_p, newloc,torg[0],torg[1]);
		if(b->maxangle == 0.0){
			newLocFound = getWedgeIntersectionWithoutMaxAngle(m, b, numpoints_p, points_p, newloc);
		}else{
			newLocFound = getWedgeIntersectionWithMaxAngle(m, b, numpoints_p, points_p, newloc);
		}
		//printf("call petal intersection for p\n");
		// make sure the relocated point is a free vertex	
		if(newLocFound && vertextype(torg) == 2){					
			possibilities[0] = newloc[0];// something found
			possibilities[1] = newloc[1];
			num_pos++;// increase the number of possibilities
			flag1 = 1;
		}
	}
		
	/*********************** TRY TO RELOCATE POINT "q" *************************/		

	// get the surrounding points of q, so this gives us the triangles
	numpoints_q = getStarPoints(m,badotri,torg,tdest,tapex,2,points_q);	
// 	// check if the points in counterclockwise order
// 	v1[0] = points_q[0];  v1[1] = points_q[1];
// 	v2[0] = points_q[2];  v2[1] = points_q[3];
// 	v3[0] = points_q[4];  v3[1] = points_q[5];
// 	if(counterclockwise(m,b,v1,v2,v3) < 0){
// 		// reverse the order to ccw
// 		for(i = 0; i < numpoints_q/2; i++){
// 			temp[0] = points_q[2*i];	
// 			temp[1] = points_q[2*i+1];
// 			points_q[2*i] = points_q[2*(numpoints_q-1)-2*i];
// 			points_q[2*i+1] = points_q[2*(numpoints_q-1)+1-2*i];
// 			points_q[2*(numpoints_q-1)-2*i] = temp[0];
// 			points_q[2*(numpoints_q-1)+1-2*i] = temp[1];
// 		}
// 	}
// 	m->counterclockcount--;
	// INTERSECTION OF PETALS
	// first check whether the star angles are appropriate for relocation
	if(numpoints_q != 0 && polygonAngles(b,numpoints_q,points_q)== 0){
		//newLocFound = getPetalIntersection(m, b,numpoints_q, points_q, newloc);
		//newLocFound = getPetalIntersectionBruteForce(m, b,numpoints_q, points_q, newloc,tapex[0],tapex[1]);
		if(b->maxangle == 0.0){
			newLocFound = getWedgeIntersectionWithoutMaxAngle(m, b, numpoints_q, points_q, newloc);
		}else{
			newLocFound = getWedgeIntersectionWithMaxAngle(m, b, numpoints_q, points_q, newloc);
		}	
		//printf("call petal intersection for q\n");	
	
		// make sure the relocated point is a free vertex	
		if(newLocFound && vertextype(tdest)==2 ){						
			possibilities[2] = newloc[0];// something found
			possibilities[3] = newloc[1];
			num_pos++;// increase the number of possibilities
			flag2 = 2;
		}
	}

	
	/*********************** TRY TO RELOCATE POINT "q" *************************/		
	// get the surrounding points of r, so this gives us the triangles
	numpoints_r = getStarPoints(m,badotri,torg,tdest,tapex,3,points_r);		
	// check if the points in counterclockwise order
// 	v1[0] = points_r[0];  v1[1] = points_r[1];
// 	v2[0] = points_r[2];  v2[1] = points_r[3];
// 	v3[0] = points_r[4];  v3[1] = points_r[5];
// 	if(counterclockwise(m,b,v1,v2,v3) < 0){
// 		// reverse the order to ccw
// 		for(i = 0; i < numpoints_r/2; i++){
// 			temp[0] = points_r[2*i];	
// 			temp[1] = points_r[2*i+1];
// 			points_r[2*i] = points_r[2*(numpoints_r-1)-2*i];
// 			points_r[2*i+1] = points_r[2*(numpoints_r-1)+1-2*i];
// 			points_r[2*(numpoints_r-1)-2*i] = temp[0];
// 			points_r[2*(numpoints_r-1)+1-2*i] = temp[1];
// 		}
// 	}
// 	m->counterclockcount--;
	// INTERSECTION OF PETALS
	// first check whether the star angles are appropriate for relocation
	if(numpoints_r != 0 && polygonAngles(b,numpoints_r,points_r)== 0){
		//newLocFound = getPetalIntersection(m, b,numpoints_r, points_r, newloc);
		//newLocFound = getPetalIntersectionBruteForce(m, b,numpoints_r, points_r, newloc,tdest[0],tdest[1]);
		if(b->maxangle == 0.0){
			newLocFound = getWedgeIntersectionWithoutMaxAngle(m, b, numpoints_r, points_r, newloc);
		}else{
			newLocFound = getWedgeIntersectionWithMaxAngle(m, b, numpoints_r, points_r, newloc);
		}
		
		//printf("call petal intersection for r\n");
	
	
		// make sure the relocated point is a free vertex	
		if(newLocFound && vertextype(tapex) == 2){						
			possibilities[4] = newloc[0];// something found
			possibilities[5] = newloc[1];
			num_pos++;// increase the number of possibilities
			flag3 = 3;
		}
	}
	//printf("numpossibilities %d\n",num_pos);
	//////////// AFTER FINISH CHECKING EVERY POSSIBILITY, CHOOSE ANY OF THE AVAILABLE ONE //////////////////////	
	if(num_pos > 0){
		if(flag1 > 0){ // suggest to relocate origin
			newloc[0] = possibilities[0];
			newloc[1] = possibilities[1];				
			return flag1;
			
		}else{
			if(flag2 > 0){ // suggest to relocate apex
				newloc[0] = possibilities[2];
				newloc[1] = possibilities[3];
				return flag2;
				
			}else{// suggest to relocate destination
				if(flag3 > 0){
					newloc[0] = possibilities[4];
					newloc[1] = possibilities[5];
					return flag3;
					
				}
			}
		}
	}
	else{
		return 0;// could not find any good relocation
	}
	return 0;
}// end of function doSmoothing()
//---------------------------------------------------------------------------------//
// getStarPoints() 
// Given the triangulation, a triangle, three vertices of the triangle, 
// and a specific point choice 
// returns list of points and the number of points on the star of the given point
//---------------------------------------------------------------------------------//
int getStarPoints(mesh *m, struct otri badotri,
			vertex p,
			vertex q,
			vertex r,
			int whichPoint,
			REAL *points){

	struct otri neighotri;  // for return value of the function
	struct otri tempotri;   // for temporary usage
	REAL first_x, first_y;	  // keeps the first point to be considered
	REAL second_x, second_y;  // for determining the edge we will begin
	REAL third_x, third_y;	  // termination
	REAL returnPoint[2];	  // for keeping the returned point	
	int numvertices = 0;	  // for keeping number of surrounding vertices

	// first determine which point to be used to find its neighbor triangles
	switch(whichPoint){
		case 1:
			first_x = p[0];	// point at the center
			first_y = p[1];
			second_x = r[0]; // second vertex of first edge to consider
			second_y = r[1];
			third_x = q[0];  // for terminating the search
			third_y = q[1];
			break;
		case 2:
			first_x = q[0];  // point at the center
			first_y = q[1];
			second_x = p[0]; // second vertex of first edge to consider
			second_y = p[1];
			third_x = r[0];	// for terminating the search
			third_y = r[1];			
			break;
		case 3:
			first_x = r[0];	// point at the center
			first_y = r[1];
			second_x = q[0]; // second vertex of first edge to consider
			second_y = q[1];
			third_x = p[0];	// for terminating the search
			third_y = p[1];			
			break;
	} 
 	tempotri = badotri;
	// add first point as the end of first edge
	points[numvertices] = second_x;
	numvertices++;
	points[numvertices] = second_y;
	numvertices++;
	// assign as dummy value
	returnPoint[0] = second_x;	returnPoint[1] = second_y;
	// until we reach the third point of the beginning triangle	
  	do{	
		// find the neighbor's third point where it is incident to given edge
 		if(getNeighborsVertex(m, tempotri, first_x, first_y, second_x, second_y, returnPoint, &neighotri) == 0){
			// go to next triangle
			tempotri = neighotri;
			// now the second point is the neighbor's third vertex			
			second_x = returnPoint[0];
			second_y = returnPoint[1];
			// add a new point to the list of surrounding points
			points[numvertices] = returnPoint[0];
			numvertices++;
			points[numvertices] = returnPoint[1];
			numvertices++;
		}else{
			numvertices = 0;
			break;
		}
  		
  	}while( !( (fabs(returnPoint[0] - third_x) <= compConst)  && 
		     (fabs(returnPoint[1] - third_y) <= compConst) ) );
	return numvertices/2;

}// end of getStarPoints()
//---------------------------------------------------------------------------------//
// getNeighborsVertex() 
// Given the triangulation, a triangle, an edge represented by two points,  
// returns the neighbor's third vertex incident to given edge
// also returns the pointer for this neighbor triangle
// returns 1, if not found, 0 otherwise
//---------------------------------------------------------------------------------//
int getNeighborsVertex(mesh *m, struct otri badotri,
				REAL first_x, REAL first_y,
				REAL second_x, REAL second_y, 
				REAL *thirdpoint, struct otri *neighotri){
 
	struct otri neighbor; // keeps the neighbor triangles
	int notFound = 0;	// boolean variable if we can find that neighbor or not
	
	// for keeping the vertices of the neighbor triangle
	vertex neighborvertex_1 = NULL;
	vertex neighborvertex_2 = NULL;
	vertex neighborvertex_3 = NULL;
	
	// used for finding neighbor triangle
	int firstVertexMatched = 0, secondVertexMatched = 0;	// to find the correct neighbor
	triangle ptr;             /* Temporary variable used by sym() */
	/* find neighbors */
	/* Check each of the triangle's three neighbors to find the correct one */
	for (badotri.orient = 0; badotri.orient < 3; badotri.orient++) {			
		/* Find the neighbor. */
		sym(badotri, neighbor);
		// check if it is the one we are looking for by checking the corners			
		/* first check if the neighbor is nonexistent, since it can be on the border */
		if ((neighbor.tri != m->dummytri)) {
			// then check if two wanted corners are also in this triangle
			/* take the vertices of the candidate neighbor */		
			org(neighbor, neighborvertex_1);	
			dest(neighbor, neighborvertex_2);	
			apex(neighbor, neighborvertex_3);
		
			// check if it is really a triangle
			if((neighborvertex_1[0] == neighborvertex_2[0] && neighborvertex_1[1] == neighborvertex_2[1]) 
			 || (neighborvertex_2[0] == neighborvertex_3[0] && neighborvertex_2[1] == neighborvertex_3[1]) 
			 || (neighborvertex_1[0] == neighborvertex_3[0] && neighborvertex_1[1] == neighborvertex_3[1])){
				//printf("Two vertices are the same!!!!!!!\n");
			}else{
				// begin searching for the correct neighbor triangle
				firstVertexMatched = 0;				
				if( (fabs(first_x - neighborvertex_1[0]) < compConst) && 
				     (fabs(first_y - neighborvertex_1[1]) < compConst) ){
					firstVertexMatched = 11; // neighbor's 1st vertex is matched to first vertex

				}else if( (fabs(first_x - neighborvertex_2[0]) < compConst) && 
					    (fabs(first_y - neighborvertex_2[1]) < compConst) ){
					firstVertexMatched = 12; // neighbor's 2nd vertex is matched to first vertex

				}else if( (fabs(first_x - neighborvertex_3[0]) < compConst) && 
				            (fabs(first_y - neighborvertex_3[1]) < compConst) ){
					firstVertexMatched = 13; // neighbor's 3rd vertex is matched to first vertex

				}/*else{	
					 // none of them matched
				} // end of first vertex matching*/
											
				secondVertexMatched = 0;					
				if( (fabs(second_x - neighborvertex_1[0]) < compConst) && 
					(fabs(second_y - neighborvertex_1[1]) < compConst) ){
					secondVertexMatched = 21; // neighbor's 1st vertex is matched to second vertex
				}else if( (fabs(second_x - neighborvertex_2[0]) < compConst) && 
					(fabs(second_y - neighborvertex_2[1]) < compConst) ){
					secondVertexMatched = 22; // neighbor's 2nd vertex is matched to second vertex
				}else if( (fabs(second_x - neighborvertex_3[0]) < compConst) && 
						(fabs(second_y - neighborvertex_3[1]) < compConst) ){
					secondVertexMatched = 23; // neighbor's 3rd vertex is matched to second vertex
				}/*else{	
					// none of them matched
				} // end of second vertex matching*/
								
			}
						
		}// if neighbor exists or not
		
		if( ( (firstVertexMatched == 11) && (secondVertexMatched == 22 || secondVertexMatched == 23) ) 
		 ||  ( (firstVertexMatched == 12) && (secondVertexMatched == 21 || secondVertexMatched == 23) ) 
		 ||  ( (firstVertexMatched == 13) && (secondVertexMatched == 21 || secondVertexMatched == 22) ) )
			break;
	}// end of for loop over all orientations
	
	switch(firstVertexMatched){
		case 0: 
			notFound = 1; 
			break;
		case 11:
			if(secondVertexMatched == 22){
				thirdpoint[0]= neighborvertex_3[0];
				thirdpoint[1]= neighborvertex_3[1];				
			}
			else if(secondVertexMatched == 23){
				thirdpoint[0]= neighborvertex_2[0];
				thirdpoint[1]= neighborvertex_2[1];
			}else{notFound = 1; }
			break;
		case 12:
			if(secondVertexMatched == 21){
				thirdpoint[0]= neighborvertex_3[0];
				thirdpoint[1]= neighborvertex_3[1];
			}
			else if(secondVertexMatched == 23){
				thirdpoint[0]= neighborvertex_1[0];
				thirdpoint[1]= neighborvertex_1[1];
			}else{notFound = 1; }
			break;
		case 13:
			if(secondVertexMatched == 21){
				thirdpoint[0]= neighborvertex_2[0];
				thirdpoint[1]= neighborvertex_2[1];
			}
			else if(secondVertexMatched == 22){
				thirdpoint[0]= neighborvertex_1[0];
				thirdpoint[1]= neighborvertex_1[1];
			}else{notFound = 1; }
			break;
		default:
			if(secondVertexMatched == 0){notFound = 1;}
			break;
	}	
	// pointer of the neighbor triangle
	*neighotri = neighbor;
	return notFound;
	
}// end of getNeighborsVertex()

//---------------------------------------------------------------------------------//
// getWedgeIntersectionWithoutMaxAngle()
// Given the triangulation, list of points and number of points on the star,
// returns a new location for the point according to surrounding points
// returns 1 if new location found, 0 otherwise
// this version uses intersection of wedges to find the new location
//---------------------------------------------------------------------------------//
int getWedgeIntersectionWithoutMaxAngle(mesh *m, behavior *b, 
			    int numpoints, REAL *points, REAL *newloc){
    REAL x0, y0, x1, y1, x2, y2;
    //REAL compConst = 0.01; // for comparing real numbers
    
    REAL x01, y01;
    REAL d01;
    
    REAL *petalx;
    REAL *petaly;
    REAL *petalr;
    REAL *wedges;
    REAL *initialConvexPoly;

    REAL xmid, ymid, dist, x3, y3;
    REAL x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4, tempx, tempy; 
    REAL ux, uy;
    REAL alpha;
    REAL p1[3];
    int numpolypoints;
    
    int i, j;
	int s, flag, count, num;

    REAL petalcenterconstant, petalradiusconstant;
	
	acutepool_resize(numpoints, m->acute_mem);
    
	petalx = m->acute_mem->petalx;
    petaly = m->acute_mem->petaly;
    petalr = m->acute_mem->petalr;
    wedges = m->acute_mem->wedges;
	initialConvexPoly = m->acute_mem->initialpoly;

    x0 = points[2*numpoints-4];
    y0 = points[2*numpoints-3];
    x1 = points[2*numpoints-2];
    y1 = points[2*numpoints-1];
    
    // minimum angle
    alpha = b->minangle*PI/180.0;
    // initialize the constants
    if (b->goodangle == 1.0) {
     	petalcenterconstant = 0;
    	petalradiusconstant = 0;	
    }else { 
        petalcenterconstant = 0.5 / tan(b->minangle * PI /180.0);
        petalradiusconstant = 0.5 / sin(b->minangle * PI / 180.0);
    }
    for(i = 0; i < numpoints*2; i= i+2){
	x2 = points[i];
	y2 = points[i+1];
	
	//printf("POLYGON POINTS (p,q) #%d (%.12f, %.12f) (%.12f, %.12f)\n", i/2, x0, y0,x1, y1);
	
	x01 = x1 - x0; 
	y01 = y1 - y0;
	d01 = sqrt(x01*x01 + y01*y01);
	// find the petal of each edge 01;
	
//	    printf("PETAL CONSTANT (%.12f, %.12f)\n", 
	//	   b->petalcenterconstant,  b->petalradiusconstant );
//	    printf("PETAL DIFFS (%.6f, %.6f, %.4f)\n", x01, y01, d01);
	
	petalx[i/2] = x0 + 0.5 * x01 - petalcenterconstant * y01;
	petaly[i/2] = y0 + 0.5 * y01 + petalcenterconstant * x01;
	petalr[i/2] =  petalradiusconstant * d01;
	petalx[numpoints+i/2]= petalx[i/2];
	petaly[numpoints+i/2]= petaly[i/2];
	petalr[numpoints+i/2]= petalr[i/2];
	//printf("PETAL POINTS #%d (%.12f, %.12f) R= %.12f\n", i/2, petalx[i/2],petaly[i/2], petalr[i/2]);

	/// FIRST FIND THE HALF-PLANE POINTS FOR EACH PETAL
	xmid = (x0 + x1)/2.0;	// mid point of pq
	ymid = (y0 + y1)/2.0;

	// distance between xmid and petal center	
	dist = sqrt((petalx[i/2] - xmid)*(petalx[i/2] - xmid) + (petaly[i/2] - ymid)*(petaly[i/2] - ymid));
	// find the unit vector goes from mid point to petal center
	ux = (petalx[i/2] - xmid)/dist;
	uy = (petaly[i/2] - ymid)/dist;
	// find the third point other than p and q
	x3 = petalx[i/2] + ux * petalr[i/2];
	y3 = petaly[i/2] + uy * petalr[i/2];
	/// FIND THE LINE POINTS BY THE ROTATION MATRIX
	// cw rotation matrix [cosX sinX; -sinX cosX]
	// cw rotation about (x,y) [ux*cosX + uy*sinX + x - x*cosX - y*sinX; -ux*sinX + uy*cosX + y + x*sinX - y*cosX]
	// ccw rotation matrix [cosX -sinX; sinX cosX]
	// ccw rotation about (x,y) [ux*cosX - uy*sinX + x - x*cosX + y*sinX; ux*sinX + uy*cosX + y - x*sinX - y*cosX]
	/// LINE #1: (x1,y1) & (x_1,y_1) 
	// vector from p to q
	ux = x1 - x0;
	uy = y1 - y0;
	// rotate the vector around p = (x0,y0) in ccw by alpha degrees
	x_1 = x1*cos(alpha) - y1*sin(alpha) + x0 - x0*cos(alpha) + y0*sin(alpha);
	y_1 = x1*sin(alpha) + y1*cos(alpha) + y0 - x0*sin(alpha) - y0*cos(alpha);
	// add these to wedges list as lines in order	
	wedges[i*16] = x0; wedges[i*16+1] = y0;
	wedges[i*16+2] = x_1; wedges[i*16+3] = y_1;
	//printf("LINE #1 (%.12f, %.12f) (%.12f, %.12f)\n", x0,y0,x_1,y_1);
	/// LINE #2: (x2,y2) & (x_2,y_2) 
	// vector from p to q
	ux = x0 - x1;
	uy = y0 - y1;	
	// rotate the vector around q = (x1,y1) in cw by alpha degrees
	x_2 = x0*cos(alpha) + y0*sin(alpha) + x1 - x1*cos(alpha) - y1*sin(alpha);
	y_2 = -x0*sin(alpha) + y0*cos(alpha) + y1 + x1*sin(alpha) - y1*cos(alpha);
	// add these to wedges list as lines in order	
	wedges[i*16+4] = x_2; wedges[i*16+5] = y_2;
	wedges[i*16+6] = x1; wedges[i*16+7] = y1;
	//printf("LINE #2 (%.12f, %.12f) (%.12f, %.12f)\n", x_2,y_2,x1,y1);
	// vector from (petalx, petaly) to (x3,y3)
	ux = x3 - petalx[i/2];
	uy = y3 - petaly[i/2];
	tempx = x3; tempy = y3;
	/// LINE #3, #4, #5: (x3,y3) & (x_3,y_3) 
	for(j = 1; j < 4; j++){		
		// rotate the vector around (petalx,petaly) in cw by (60 - alpha)*j degrees			
		x_3 = x3*cos((PI/3.0 - alpha)*j) + y3*sin((PI/3.0 - alpha)*j) + petalx[i/2] - petalx[i/2]*cos((PI/3.0 - alpha)*j) - petaly[i/2]*sin((PI/3.0 - alpha)*j);
		y_3 = -x3*sin((PI/3.0 - alpha)*j) + y3*cos((PI/3.0 - alpha)*j) + petaly[i/2] + petalx[i/2]*sin((PI/3.0 - alpha)*j) - petaly[i/2]*cos((PI/3.0 - alpha)*j);
		// add these to wedges list as lines in order	
		wedges[i*16+8+4*(j-1)] = x_3; wedges[i*16+9+4*(j-1)] = y_3;
		wedges[i*16+10+4*(j-1)] = tempx; wedges[i*16+11+4*(j-1)] = tempy;
		tempx = x_3; tempy = y_3;
	}
	tempx = x3; tempy = y3;
	/// LINE #6, #7, #8: (x3,y3) & (x_4,y_4) 
	for(j = 1; j < 4; j++){		
		// rotate the vector around (petalx,petaly) in ccw by (60 - alpha)*j degrees
		x_4 = x3*cos((PI/3.0 - alpha)*j) - y3*sin((PI/3.0 - alpha)*j) + petalx[i/2] - petalx[i/2]*cos((PI/3.0 - alpha)*j) + petaly[i/2]*sin((PI/3.0 - alpha)*j);
		y_4 = x3*sin((PI/3.0 - alpha)*j) + y3*cos((PI/3.0 - alpha)*j) + petaly[i/2] - petalx[i/2]*sin((PI/3.0 - alpha)*j) - petaly[i/2]*cos((PI/3.0 - alpha)*j);

		// add these to wedges list as lines in order	
		wedges[i*16+20+4*(j-1)] = tempx; wedges[i*16+21+4*(j-1)] = tempy;
		wedges[i*16+22+4*(j-1)] = x_4; wedges[i*16+23+4*(j-1)] = y_4;
		tempx = x_4; tempy = y_4;
	}			
	//printf("LINE #3 (%.12f, %.12f) (%.12f, %.12f)\n", x_3,y_3,x3,y3);			
	//printf("LINE #4 (%.12f, %.12f) (%.12f, %.12f)\n", x3,y3,x_4,y_4);

	/// IF IT IS THE FIRST ONE, FIND THE CONVEX POLYGON
	if(i == 0){
		// line1 & line2: p1
		lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_2, y_2, p1);		
		if(p1[0] == 1.0){
			// #0
			initialConvexPoly[0] = p1[1]; initialConvexPoly[1] = p1[2];
			// #1
			initialConvexPoly[2] = wedges[i*16+16]; initialConvexPoly[3] = wedges[i*16+17];
			// #2
			initialConvexPoly[4] = wedges[i*16+12]; initialConvexPoly[5] = wedges[i*16+13];
			// #3
			initialConvexPoly[6] = wedges[i*16+8]; initialConvexPoly[7] = wedges[i*16+9];
			// #4
			initialConvexPoly[8] = x3; initialConvexPoly[9] = y3;
			// #5
			initialConvexPoly[10] = wedges[i*16+22]; initialConvexPoly[11] = wedges[i*16+23];
			// #6
			initialConvexPoly[12] = wedges[i*16+26]; initialConvexPoly[13] = wedges[i*16+27];	
			// #7
			initialConvexPoly[14] = wedges[i*16+30]; initialConvexPoly[15] = wedges[i*16+31];
			//printf("INITIAL POLY [%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f]\n", initialConvexPoly[0],initialConvexPoly[1],initialConvexPoly[2],initialConvexPoly[3],initialConvexPoly[4],initialConvexPoly[5],initialConvexPoly[6],initialConvexPoly[7],initialConvexPoly[8],initialConvexPoly[9],initialConvexPoly[10],initialConvexPoly[11],initialConvexPoly[12],initialConvexPoly[13],initialConvexPoly[14],initialConvexPoly[15]);
		}
	}	
	
	x0 = x1;	    y0 = y1;
	x1 = x2;	    y1 = y2;
    }  

    /// HALF PLANE INTERSECTION: START SPLITTING THE INITIAL POLYGON TO FIND FEASIBLE REGION    
    if(numpoints != 0){
	// first intersect the opposite located ones
	s = (numpoints-1)/2+1;
	flag = 0; 
	count = 0;
	i = 1;
	num = 8;	
	for(j = 0; j < 32; j = j+4){
			numpolypoints = halfPlaneIntersection(num, initialConvexPoly, wedges[32*s+j],wedges[32*s+1+j], wedges[32*s+2+j], wedges[32*s+3+j]);
			if(numpolypoints == 0)
				return 0;
			else
				num = numpolypoints;
	}
	count++;
	while(count < numpoints-1){
		for(j = 0; j < 32; j = j+4){
			numpolypoints = halfPlaneIntersection(num, initialConvexPoly, wedges[32*(i+s*flag)+j],wedges[32*(i+s*flag)+1+j], wedges[32*(i+s*flag)+2+j], wedges[32*(i+s*flag)+3+j]);				
			if(numpolypoints == 0)
				return 0;
			else
				num = numpolypoints;
		}
		i = i+flag;
		flag = (flag + 1)%2;
		count++;
	}        
	/// IF THERE IS A FEASIBLE INTERSECTION POLYGON, FIND ITS CENTROID AS THE NEW LOCATION
	findPolyCentroid(numpolypoints, initialConvexPoly, newloc);
	
	if(b->fixedarea){
// 		numBadTriangle = 0;
// 		for(j= 0; j < numpoints *2-2; j = j+2){
// 			if(testTriangleAngleArea(m,b,&newloc[0],&newloc[1], &points[j], &points[j+1], &points[j+2], &points[j+3] )){
// 				numBadTriangle++; 
// 			}
// 		}
// 		if(testTriangleAngleArea(m,b, &newloc[0],&newloc[1], &points[0], &points[1], &points[numpoints*2-2], &points[numpoints*2-1] )){
// 			numBadTriangle++;
// 		}
// 		
// 		if (numBadTriangle == 0)  {
// 			
// 			return 1;
// 		}
	}else{	
	//printf("yes, we found a feasible region num: %d newloc (%.12f,%.12f)\n", numpolypoints, newloc[0], newloc[1]);
// 	for(i = 0; i < 2*numpolypoints; i = i+2){
// 		printf("point %d) (%.12f,%.12f)\n", i/2, initialConvexPoly[i], initialConvexPoly[i+1]);
// 	}	
// 	printf("numpoints %d\n",numpoints);
		return 1;
	}
   }
   
   // freeing done by acute memory pool
	//free(petalx);
	//free(petaly);
	//free(petalr);
	//free(wedges);

    return 0;
}// end of getWedgeIntersectionWithoutMaxAngle()
//---------------------------------------------------------------------------------//
// getWedgeIntersectionWithMaxAngle()
// Given the triangulation, list of points and number of points on the star,
// returns a new location for the point according to surrounding points
// returns 1 if new location found, 0 otherwise
// this version uses intersection of wedges to find the new location
//---------------------------------------------------------------------------------//
int getWedgeIntersectionWithMaxAngle(mesh *m, behavior *b, 
			    int numpoints, REAL *points, REAL *newloc){
    REAL x0, y0, x1, y1, x2, y2;
    //REAL compConst = 0.01; // for comparing real numbers
    
    REAL x01, y01;
    
    REAL d01;
    
    REAL *petalx;
    REAL *petaly;
    REAL *petalr;
    REAL *wedges;
    REAL *initialConvexPoly;

    REAL xmid, ymid, dist, x3, y3;
    REAL x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4, tempx, tempy,x_5,y_5,x_6,y_6; 
    REAL ux, uy;
    REAL alpha;
    REAL p1[3], p2[3], p3[3], p4[3];
    int numpolypoints;
    int howManyPoints;	// keeps the number of points used for representing the wedge
    REAL line345 = 4.0, line789 = 4.0; // flag keeping which line to skip or construct
    
    int numBadTriangle;
    int i, j, k;
	int s, flag, count, num;
	int e, n;
	REAL weight;

    REAL petalcenterconstant, petalradiusconstant;
	
	acutepool_resize(numpoints, m->acute_mem);
    
	petalx = m->acute_mem->petalx;
    petaly = m->acute_mem->petaly;
    petalr = m->acute_mem->petalr;
    wedges = m->acute_mem->wedges;
	initialConvexPoly = m->acute_mem->initialpoly;

    x0 = points[2*numpoints-4];
    y0 = points[2*numpoints-3];
    x1 = points[2*numpoints-2];
    y1 = points[2*numpoints-1];
    
    // minimum angle
    alpha = b->minangle*PI/180.0;
    // initialize the constants
    if (b->goodangle == 1.0) {
     	petalcenterconstant = 0;
    	petalradiusconstant = 0;	
    }else { 
        petalcenterconstant = 0.5 / tan(b->minangle * PI /180.0);
        petalradiusconstant = 0.5 / sin(b->minangle * PI / 180.0);
    }
    for(i = 0; i < numpoints*2; i= i+2){
	// minimum angle
    	alpha = b->minangle*PI/180.0;   
	// go to the next point
	x2 = points[i];
	y2 = points[i+1];
	
//   	printf("POLYGON POINTS (p,q) #%d (%.12f, %.12f) (%.12f, %.12f)\n", i/2, x0, y0,x1, y1);
	
	x01 = x1 - x0; 
	y01 = y1 - y0;
	d01 = sqrt(x01*x01 + y01*y01);
	// find the petal of each edge 01;
	
//	    printf("PETAL CONSTANT (%.12f, %.12f)\n", 
	//	   b->petalcenterconstant,  b->petalradiusconstant );
//	    printf("PETAL DIFFS (%.6f, %.6f, %.4f)\n", x01, y01, d01);
	//printf("i:%d numpoints:%d\n", i, numpoints);
	petalx[i/2] = x0 + 0.5 * x01 - petalcenterconstant * y01;
	petaly[i/2] = y0 + 0.5 * y01 + petalcenterconstant * x01;
	petalr[i/2] =  petalradiusconstant * d01;
	petalx[numpoints+i/2]= petalx[i/2];
	petaly[numpoints+i/2]= petaly[i/2];
	petalr[numpoints+i/2]= petalr[i/2];
	//printf("PETAL POINTS #%d (%.12f, %.12f) R= %.12f\n", i/2, petalx[i/2],petaly[i/2], petalr[i/2]);

	/// FIRST FIND THE HALF-PLANE POINTS FOR EACH PETAL
	xmid = (x0 + x1)/2.0;	// mid point of pq
	ymid = (y0 + y1)/2.0;

	// distance between xmid and petal center	
	dist = sqrt((petalx[i/2] - xmid)*(petalx[i/2] - xmid) + (petaly[i/2] - ymid)*(petaly[i/2] - ymid));
	// find the unit vector goes from mid point to petal center
	ux = (petalx[i/2] - xmid)/dist;
	uy = (petaly[i/2] - ymid)/dist;
	// find the third point other than p and q
	x3 = petalx[i/2] + ux * petalr[i/2];
	y3 = petaly[i/2] + uy * petalr[i/2];
	/// FIND THE LINE POINTS BY THE ROTATION MATRIX
	// cw rotation matrix [cosX sinX; -sinX cosX]
	// cw rotation about (x,y) [ux*cosX + uy*sinX + x - x*cosX - y*sinX; -ux*sinX + uy*cosX + y + x*sinX - y*cosX]
	// ccw rotation matrix [cosX -sinX; sinX cosX]
	// ccw rotation about (x,y) [ux*cosX - uy*sinX + x - x*cosX + y*sinX; ux*sinX + uy*cosX + y - x*sinX - y*cosX]
	/// LINE #1: (x1,y1) & (x_1,y_1) 
	// vector from p to q
	ux = x1 - x0;
	uy = y1 - y0;
	// rotate the vector around p = (x0,y0) in ccw by alpha degrees
	x_1 = x1*cos(alpha) - y1*sin(alpha) + x0 - x0*cos(alpha) + y0*sin(alpha);
	y_1 = x1*sin(alpha) + y1*cos(alpha) + y0 - x0*sin(alpha) - y0*cos(alpha);
	// add these to wedges list as lines in order	
	wedges[i*20] = x0; wedges[i*20+1] = y0;
	wedges[i*20+2] = x_1; wedges[i*20+3] = y_1;
	//printf("LINE #1 (%.12f, %.12f) (%.12f, %.12f)\n", x0,y0,x_1,y_1);
	/// LINE #2: (x2,y2) & (x_2,y_2) 
	// vector from q to p
	ux = x0 - x1;
	uy = y0 - y1;	
	// rotate the vector around q = (x1,y1) in cw by alpha degrees
	x_2 = x0*cos(alpha) + y0*sin(alpha) + x1 - x1*cos(alpha) - y1*sin(alpha);
	y_2 = -x0*sin(alpha) + y0*cos(alpha) + y1 + x1*sin(alpha) - y1*cos(alpha);
	// add these to wedges list as lines in order	
	wedges[i*20+4] = x_2; wedges[i*20+5] = y_2;
	wedges[i*20+6] = x1; wedges[i*20+7] = y1;
	//printf("LINE #2 (%.12f, %.12f) (%.12f, %.12f)\n", x_2,y_2,x1,y1);
	// vector from (petalx, petaly) to (x3,y3)
	ux = x3 - petalx[i/2];
	uy = y3 - petaly[i/2];
	tempx = x3; tempy = y3;

	/// DETERMINE HOW MANY POINTS TO USE ACCORDING TO THE MINANGLE-MAXANGLE COMBINATION
	// petal center angle
    	alpha = (2.0*b->maxangle + b->minangle - 180.0);    
	if(alpha <= 0.0){// when only angle lines needed
		// 4 point case
		howManyPoints = 4;
		//printf("4 point case\n");
		line345 = 1.0;
		line789 = 1.0;
	}else if(alpha <= 5.0){// when only angle lines plus two other lines are needed
		// 6 point case
		howManyPoints = 6;
		//printf("6 point case\n");
		line345 = 2.0;
		line789 = 2.0;
	}else if(alpha <= 10.0){// when we need more lines
		// 8 point case
		howManyPoints = 8;
		line345 = 3.0;
		line789 = 3.0;
		//printf("8 point case\n");
	}else{// when we have a big wedge
		// 10 point case
		howManyPoints = 10;
		//printf("10 point case\n");
		line345 = 4.0;
		line789 = 4.0;
	}	
	alpha = alpha*PI/180.0; 	
	/// LINE #3, #4, #5: (x3,y3) & (x_3,y_3) 
	for(j = 1; j < line345; j++){	
		if(line345 == 1)
			continue;	
		// rotate the vector around (petalx,petaly) in cw by (alpha/3.0)*j degrees			
		x_3 = x3*cos((alpha/(line345-1.0))*j) + y3*sin(((alpha/(line345-1.0))*j)) + petalx[i/2] - petalx[i/2]*cos(((alpha/(line345-1.0))*j)) - petaly[i/2]*sin(((alpha/(line345-1.0))*j));
		y_3 = -x3*sin(((alpha/(line345-1.0))*j)) + y3*cos(((alpha/(line345-1.0))*j)) + petaly[i/2] + petalx[i/2]*sin(((alpha/(line345-1.0))*j)) - petaly[i/2]*cos(((alpha/(line345-1.0))*j));
		// add these to wedges list as lines in order	
		wedges[i*20+8+4*(j-1)] = x_3; wedges[i*20+9+4*(j-1)] = y_3;
		wedges[i*20+10+4*(j-1)] = tempx; wedges[i*20+11+4*(j-1)] = tempy;
		tempx = x_3; tempy = y_3;
	}
	/// LINE #6: (x2,y2) & (x_3,y_3) 
	// vector from q to p
	ux = x0 - x1;
	uy = y0 - y1;	
	// rotate the vector around q = (x1,y1) in cw by alpha degrees
	x_5 = x0*cos(b->maxangle*PI/180.0) + y0*sin(b->maxangle*PI/180.0) + x1 - x1*cos(b->maxangle*PI/180.0) - y1*sin(b->maxangle*PI/180.0);
	y_5 = -x0*sin(b->maxangle*PI/180.0) + y0*cos(b->maxangle*PI/180.0) + y1 + x1*sin(b->maxangle*PI/180.0) - y1*cos(b->maxangle*PI/180.0);	
	wedges[i*20+20] = x1; wedges[i*20+21] = y1;
	wedges[i*20+22] = x_5; wedges[i*20+23] = y_5;			

	tempx = x3; tempy = y3;
	/// LINE #7, #8, #9: (x3,y3) & (x_4,y_4) 
	for(j = 1; j < line789; j++){	
		if(line789 == 1)
			continue;	
		// rotate the vector around (petalx,petaly) in ccw by (alpha/3.0)*j degrees
		x_4 = x3*cos((alpha/(line789-1.0))*j) - y3*sin((alpha/(line789-1.0))*j) + petalx[i/2] - petalx[i/2]*cos((alpha/(line789-1.0))*j) + petaly[i/2]*sin((alpha/(line789-1.0))*j);
		y_4 = x3*sin((alpha/(line789-1.0))*j) + y3*cos((alpha/(line789-1.0))*j) + petaly[i/2] - petalx[i/2]*sin((alpha/(line789-1.0))*j) - petaly[i/2]*cos((alpha/(line789-1.0))*j);

		// add these to wedges list as lines in order	
		wedges[i*20+24+4*(j-1)] = tempx; wedges[i*20+25+4*(j-1)] = tempy;
		wedges[i*20+26+4*(j-1)] = x_4; wedges[i*20+27+4*(j-1)] = y_4;
		tempx = x_4; tempy = y_4;
	}
	/// LINE #10: (x1,y1) & (x_3,y_3) 
	// vector from p to q
	ux = x1 - x0;
	uy = y1 - y0;
	// rotate the vector around p = (x0,y0) in ccw by alpha degrees
	x_6 = x1*cos(b->maxangle*PI/180.0) - y1*sin(b->maxangle*PI/180.0) + x0 - x0*cos(b->maxangle*PI/180.0) + y0*sin(b->maxangle*PI/180.0);
	y_6 = x1*sin(b->maxangle*PI/180.0) + y1*cos(b->maxangle*PI/180.0) + y0 - x0*sin(b->maxangle*PI/180.0) - y0*cos(b->maxangle*PI/180.0);
	wedges[i*20+36] = x_6; wedges[i*20+37] = y_6;
	wedges[i*20+38] = x0; wedges[i*20+39] = y0;		

	//printf("LINE #1 (%.12f, %.12f) (%.12f, %.12f)\n", x0,y0,x_1,y_1);
	/// IF IT IS THE FIRST ONE, FIND THE CONVEX POLYGON
	if(i == 0){
		switch(howManyPoints){		
			case 4: 
				// line1 & line2 & line3 & line4
				lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_2, y_2, p1);
				lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_5, y_5, p2);
				lineLineIntersection(x0, y0, x_6, y_6, x1, y1, x_5, y_5, p3);
				lineLineIntersection(x0, y0, x_6, y_6, x1, y1, x_2, y_2, p4);		
				if((p1[0] == 1.0) && (p2[0] == 1.0) && (p3[0] == 1.0) && (p4[0] == 1.0)){			
					// #0
					initialConvexPoly[0] = p1[1]; initialConvexPoly[1] = p1[2];
					// #1
					initialConvexPoly[2] = p2[1]; initialConvexPoly[3] = p2[2];
					// #2
					initialConvexPoly[4] = p3[1]; initialConvexPoly[5] = p3[2];
					// #3
					initialConvexPoly[6] = p4[1]; initialConvexPoly[7] = p4[2];
				}	
				break;
			case 6: 			
				// line1 & line2 & line3
				lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_2, y_2, p1);
				lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_5, y_5, p2);
				lineLineIntersection(x0, y0, x_6, y_6, x1, y1, x_2, y_2, p3);
				if((p1[0] == 1.0) && (p2[0] == 1.0) && (p3[0] == 1.0)){
					// #0
					initialConvexPoly[0] = p1[1]; initialConvexPoly[1] = p1[2];
					// #1
					initialConvexPoly[2] =  p2[1]; initialConvexPoly[3] =  p2[2];
					// #2
					initialConvexPoly[4] = wedges[i*20+8]; initialConvexPoly[5] = wedges[i*20+9];
					// #3
					initialConvexPoly[6] = x3; initialConvexPoly[7] = y3;
					// #4
					initialConvexPoly[8] =  wedges[i*20+26]; initialConvexPoly[9] =  wedges[i*20+27];
					// #5
					initialConvexPoly[10] = p3[1]; initialConvexPoly[11] = p3[2];	
				}
				break;
			case 8: 
				// line1 & line2: p1
				lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_2, y_2, p1);
				lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_5, y_5, p2);
				lineLineIntersection(x0, y0, x_6, y_6, x1, y1, x_2, y_2, p3);				
				if((p1[0] == 1.0) && (p2[0] == 1.0) && (p3[0] == 1.0)){			
					// #0
					initialConvexPoly[0] = p1[1]; initialConvexPoly[1] = p1[2];
					// #1
					initialConvexPoly[2] =  p2[1]; initialConvexPoly[3] =  p2[2];
					// #2
					initialConvexPoly[4] = wedges[i*20+12]; initialConvexPoly[5] = wedges[i*20+13];
					// #3
					initialConvexPoly[6] = wedges[i*20+8]; initialConvexPoly[7] = wedges[i*20+9];
					// #4
					initialConvexPoly[8] =  x3; initialConvexPoly[9] =  y3;
					// #5
					initialConvexPoly[10] = wedges[i*20+26]; initialConvexPoly[11] = wedges[i*20+27];
					// #6
					initialConvexPoly[12] = wedges[i*20+30]; initialConvexPoly[13] = wedges[i*20+31];	
					// #7
					initialConvexPoly[14] = p3[1]; initialConvexPoly[15] = p3[2];
				}
				break;
			case 10: 
				// line1 & line2: p1
				lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_2, y_2, p1);
				lineLineIntersection(x0, y0, x_1, y_1, x1, y1, x_5, y_5, p2);
				lineLineIntersection(x0, y0, x_6, y_6, x1, y1, x_2, y_2, p3);
				//printf("p3 %f %f %f (%f %f) (%f %f) (%f %f) (%f %f)\n",p3[0],p3[1],p3[2], x0, y0, x_6, x_6, x1, y1, x_2, y_2);
				if((p1[0] == 1.0) && (p2[0] == 1.0) && (p3[0] == 1.0)){			
					// #0
					initialConvexPoly[0] = p1[1]; initialConvexPoly[1] = p1[2];
					// #1
					initialConvexPoly[2] =  p2[1]; initialConvexPoly[3] =  p2[2];
					// #2
					initialConvexPoly[4] = wedges[i*20+16]; initialConvexPoly[5] = wedges[i*20+17];
					// #3
					initialConvexPoly[6] = wedges[i*20+12]; initialConvexPoly[7] = wedges[i*20+13];
					// #4
					initialConvexPoly[8] =  wedges[i*20+8]; initialConvexPoly[9] =  wedges[i*20+9];
					// #5
					initialConvexPoly[10] = x3; initialConvexPoly[11] = y3;
					// #6
					initialConvexPoly[12] = wedges[i*20+28]; initialConvexPoly[13] = wedges[i*20+29];	
					// #7
					initialConvexPoly[14] = wedges[i*20+32]; initialConvexPoly[15] = wedges[i*20+33];
					// #8
					initialConvexPoly[16] = wedges[i*20+34]; initialConvexPoly[17] = wedges[i*20+35];	
					// #9
					initialConvexPoly[18] = p3[1]; initialConvexPoly[19] = p3[2]; 
				}
				break;	
		}
// 		printf("smallest edge (%f,%f) (%f,%f)\n", x0,y0, x1,y1);
// 			printf("real INITIAL POLY [%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;]\n", initialConvexPoly[0],initialConvexPoly[1],initialConvexPoly[2],initialConvexPoly[3],initialConvexPoly[4],initialConvexPoly[5],initialConvexPoly[6],initialConvexPoly[7],initialConvexPoly[8],initialConvexPoly[9],initialConvexPoly[10],initialConvexPoly[11],initialConvexPoly[12],initialConvexPoly[13],initialConvexPoly[14],initialConvexPoly[15],initialConvexPoly[16],initialConvexPoly[17],initialConvexPoly[18],initialConvexPoly[19]);
	}	
	
	x0 = x1;	    y0 = y1;
	x1 = x2;	    y1 = y2;
    }  	
    /// HALF PLANE INTERSECTION: START SPLITTING THE INITIAL POLYGON TO FIND FEASIBLE REGION    
    if(numpoints != 0){
	// first intersect the opposite located ones
	s = (numpoints-1)/2+1;
	flag = 0; 
	count = 0;
	i = 1;
	num = howManyPoints;			
	for(j = 0; j < 40; j = j+4){			
			// in order to skip non-existent lines
			if(howManyPoints == 4 && (j == 8 || j == 12 || j == 16 || j == 24 || j == 28 || j == 32)){
				continue;
			}else if(howManyPoints == 6 && (j == 12 || j == 16 || j == 28 || j == 32)){
				continue;
			}else if(howManyPoints == 8 && (j == 16 || j == 32)){
				continue;
			}
// 			printf("%d 1 INITIAL POLY [%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;]\n",num, initialConvexPoly[0],initialConvexPoly[1],initialConvexPoly[2],initialConvexPoly[3],initialConvexPoly[4],initialConvexPoly[5],initialConvexPoly[6],initialConvexPoly[7],initialConvexPoly[8],initialConvexPoly[9],initialConvexPoly[10],initialConvexPoly[11],initialConvexPoly[12],initialConvexPoly[13],initialConvexPoly[14],initialConvexPoly[15],initialConvexPoly[16],initialConvexPoly[17],initialConvexPoly[18],initialConvexPoly[19]);	
// 			printf("line (%f, %f) (%f, %f)\n",wedges[40*s+j],wedges[40*s+1+j], wedges[40*s+2+j], wedges[40*s+3+j]);	
			numpolypoints = halfPlaneIntersection(num, initialConvexPoly, wedges[40*s+j],wedges[40*s+1+j], wedges[40*s+2+j], wedges[40*s+3+j]);	
			
			if(numpolypoints == 0)
				return 0;
			else
				num = numpolypoints;
	}
	count++;	
	//printf("yes here\n");	
	while(count < numpoints-1){
		for(j = 0; j < 40; j = j+4){
			// in order to skip non-existent lines
			if(howManyPoints == 4 && (j == 8 || j == 12 || j == 16 || j == 24 || j == 28 || j == 32)){
				continue;
			}else if(howManyPoints == 6 && (j == 12 || j == 16 || j == 28 || j == 32)){
				continue;
			}else if(howManyPoints == 8 && (j == 16 || j == 32)){
				continue;
			}
			/*printf("%d 2 INITIAL POLY [%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;%.12f, %.12f;]\n",numpolypoints, initialConvexPoly[0],initialConvexPoly[1],initialConvexPoly[2],initialConvexPoly[3],initialConvexPoly[4],initialConvexPoly[5],initialConvexPoly[6],initialConvexPoly[7],initialConvexPoly[8],initialConvexPoly[9],initialConvexPoly[10],initialConvexPoly[11],initialConvexPoly[12],initialConvexPoly[13],initialConvexPoly[14],initialConvexPoly[15],initialConvexPoly[16],initialConvexPoly[17],initialConvexPoly[18],initialConvexPoly[19]);	
			printf("line (%.20f, %.20f) (%.20f, %.20f)\n",wedges[40*(i+s*flag)+j],wedges[40*(i+s*flag)+1+j],wedges[40*(i+s*flag)+2+j], wedges[40*(i+s*flag)+3+j]);*/	
			numpolypoints = halfPlaneIntersection(num, initialConvexPoly, wedges[40*(i+s*flag)+j],wedges[40*(i+s*flag)+1+j], wedges[40*(i+s*flag)+2+j], wedges[40*(i+s*flag)+3+j]);	
						
			if(numpolypoints == 0)
				return 0;
			else
				num = numpolypoints;
		}
		i = i+flag;
		flag = (flag + 1)%2;
		count++;
	} 		
	/// IF THERE IS A FEASIBLE INTERSECTION POLYGON, FIND ITS CENTROID AS THE NEW LOCATION
	findPolyCentroid(numpolypoints, initialConvexPoly, newloc);
	
	if(b->maxangle != 0.0){
		numBadTriangle = 0;
		for(j= 0; j < numpoints *2-2; j = j+2){
			if(testTriangleAngle(b,&newloc[0],&newloc[1], &points[j], &points[j+1], &points[j+2], &points[j+3] )){
				numBadTriangle++; 
			}
		}
		if(testTriangleAngle(b, &newloc[0],&newloc[1], &points[0], &points[1], &points[numpoints*2-2], &points[numpoints*2-1] )){
			numBadTriangle++;
		}
		
		if (numBadTriangle == 0)  {
			
			return 1;
		}

		n = (numpoints <= 2) ? 20 : 30;
		// try points other than centroid
		for(k = 0; k < 2*numpoints; k = k+2){
			for(e = 1; e < n; e = e + 1){				
				newloc[0] = 0.0;	newloc[1] = 0.0;			
				for(i = 0; i < 2*numpoints; i = i+2){	
					weight = 1.0/numpoints;			
					if(i == k){
						newloc[0] = newloc[0] + 0.1*e*weight*points[i];
						newloc[1] = newloc[1] + 0.1*e*weight*points[i+1];
					}else{
						weight = (1.0 - 0.1*e*weight)/(double)(numpoints - 1.0);
						newloc[0] = newloc[0] + weight*points[i];
						newloc[1] = newloc[1] + weight*points[i+1];			
					}
					
				}			
				numBadTriangle = 0;
				for(j= 0; j < numpoints *2-2; j = j+2){
					if(testTriangleAngle(b,&newloc[0],&newloc[1], &points[j], &points[j+1], &points[j+2], &points[j+3] )){
						numBadTriangle++; 
					}
				}
				if(testTriangleAngle(b, &newloc[0],&newloc[1], &points[0], &points[1], &points[numpoints*2-2], &points[numpoints*2-1] )){
					numBadTriangle++;
				}
				
				if (numBadTriangle == 0)  {
					
					return 1;
				}
			}
		}
	}else{	
	//printf("yes, we found a feasible region num: %d newloc (%.12f,%.12f)\n", numpolypoints, newloc[0], newloc[1]);
// 	for(i = 0; i < 2*numpolypoints; i = i+2){
// 		printf("point %d) (%.12f,%.12f)\n", i/2, initialConvexPoly[i], initialConvexPoly[i+1]);
// 	}	
// 	printf("numpoints %d\n",numpoints);
		return 1;
	}
   }

   // freeing done by acute memory pool
	//free(petalx);
	//free(petaly);
	//free(petalr);
	//free(wedges);

    return 0;
}// end of getWedgeIntersectionWithMaxAngle()

//---------------------------------------------------------------------------------//
// polygonAngles()
// Return 0 if the polygon has angles greater than 2*minangle
//---------------------------------------------------------------------------------//
int polygonAngles(behavior *b,int numpoints, REAL *points){
	int i;
	for(i = 0; i < numpoints; i++){
		if(i == numpoints-1){
			if( testPolygonAngle(b, &points[i*2], &points[i*2+1], &points[0], &points[1], &points[2], &points[3]) ){
				return 1;	// one of the inner angles is less than required
			}
		}else if(i == numpoints-2){
			if( testPolygonAngle(b, &points[i*2], &points[i*2+1], &points[(i+1)*2], &points[(i+1)*2+1], &points[0], &points[1]) ){
				return 1;	// one of the inner angles is less than required
			}
		}else{
			if( testPolygonAngle(b, &points[i*2], &points[i*2+1], &points[(i+1)*2], &points[(i+1)*2+1], &points[(i+2)*2], &points[(i+2)*2+1]) ){
				return 1;	// one of the inner angles is less than required
			}
		}
	}
	return 0;	// all angles are valid
}// end of polygonAngles()

//---------------------------------------------------------------------------------//
// testPolygonAngle() 
// Given three coordinates of a polygon,  
// tests to see if it satisfies the minimum angle condition for relocation 
// Returns 1, if it is a BAD polygon corner, returns 0 if it is a GOOD polygon corner
//---------------------------------------------------------------------------------//
int testPolygonAngle(behavior *b, 
				REAL *x1, REAL *y1,
				REAL *x2, REAL *y2,
				REAL *x3, REAL *y3 ){
	// variables keeping the distance values for the edges
	REAL dx12, dy12, dx23, dy23, dx31, dy31;
	REAL dist12, dist23, dist31;
	
	REAL cosAngle;    // in order to check minimum angle condition
	
	// calculate the side lengths
	
	dx12 = *x1 - *x2;
	dy12 = *y1 - *y2;
	dx23 = *x2 - *x3;
	dy23 = *y2 - *y3;
	dx31 = *x3 - *x1;
	dy31 = *y3 - *y1;
	// calculate the squares of the side lentghs
	dist12 = dx12 * dx12 + dy12 * dy12;
	dist23 = dx23 * dx23 + dy23 * dy23;
	dist31 = dx31 * dx31 + dy31 * dy31;
	
	/// calculate cosine of largest angle	///	
	cosAngle = (dist12 + dist23 - dist31)/(2*sqrt(dist12)*sqrt(dist23));
	/* Check whether the angle is smaller than permitted which is 2*minangle!!! */  
	//printf("angle: %f 2*minangle = %f\n",acos(cosAngle)*180/PI, 2*acos(sqrt(b->goodangle))*180/PI);
	if (acos(cosAngle) < 2*acos(sqrt(b->goodangle))) {
		return 1;// it is a BAD triangle
	}
	return 0;// it is a GOOD triangle

}// end of testPolygonAngle()

//---------------------------------------------------------------------------------//
// lineLineIntersection() 
// Given four points representing two lines, returns the intersection point
// referenced to: http://local.wasp.uwa.edu.au/~pbourke/geometry/
//---------------------------------------------------------------------------------//
void lineLineIntersection (
    REAL x1, REAL y1 ,
    REAL x2, REAL y2 ,
    REAL x3, REAL y3 , 
    REAL x4, REAL y4 , REAL *p)
{
	// x1,y1  P1 coordinates (point of line 1)
	// x2,y2  P2 coordinates (point of line 1)	
	// x3,y3  P3 coordinates (point of line 2)
	// x4,y4  P4 coordinates (point of line 2)
	// p[1],p[2]   intersection coordinates
	//
	// This function returns a pointer array which first index indicates
	// weather they intersect on one point or not, followed by coordinate pairs.
		
	REAL u_a, u_b, denom;
	
	// calculate denominator first
	denom = (y4-y3)*(x2-x1)-(x4-x3)*(y2-y1);
	u_a = (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3);
	u_b = (x2-x1)*(y1-y3) - (y2-y1)*(x1-x3);
	// if denominator and numerator equal to zero, lines are coincident
	if(fabs(denom-0.0) < compConst && (fabs(u_b-0.0) < compConst && fabs(u_a-0.0) < compConst)){
		p[0] = 0.0;	
	}
	// if denominator equals to zero, lines are parallel
	else if(fabs(denom-0.0) < compConst){
		p[0] = 0.0;
	}
	else{
		p[0] = 1.0;
		u_a = u_a/denom;
		u_b = u_b/denom;
		p[1] = x1 + u_a*(x2-x1); // not the intersection point
		p[2] = y1 + u_a*(y2-y1);
	}
	
	
	
}// end of lineLineIntersection()


//---------------------------------------------------------------------------------//
// halfPlaneIntersection()
// Returns the convex polygon which is the intersection of the
// given convex polygon with the halfplane on the left side
// (regarding the directional vector) of the given line.
// www.mathematik.uni-ulm.de/stochastik/lehre/ws03_04/rt/Geometry2D.ps
//---------------------------------------------------------------------------------//
int halfPlaneIntersection(int numvertices, REAL *convexPoly, REAL x1, REAL y1, REAL x2, REAL y2){

	REAL dx, dy;	// direction of the line
	REAL z, min, max;
	int i, j;
	REAL *polys[2];
	REAL *res = NULL;
	int count = 0;
 	int intFound = 0;
	int numpolys;

	polys[0] = (REAL *)malloc(sizeof(REAL) * 100);
	polys[1] = (REAL *)malloc(sizeof(REAL) * 100);

	dx = x2 - x1;
	dy = y2 - y1;
	numpolys = splitConvexPolygon(numvertices,convexPoly, x1, y1, x2, y2, polys);	
	if(numpolys == 3){
		count = numvertices;
	}else{
		for (i = 0; i < numpolys; i++) {
			min = 1E+37;
			max = -1E+37;
			// compute the minimum and maximum of the
			// third coordinate of the cross product		
			for (j = 1; j <= 2*polys[i][0]-1; j = j+2) {				
				z = dx * (polys[i][j+1] - y1) - dy * (polys[i][j] - x1);
				min = (z < min ? z : min);
				max = (z > max ? z : max);			
			}
			// ... and choose the (absolute) greater of both
			z = (fabs(min) > fabs(max) ? min : max);
			// and if it is positive, the polygon polys[i]
			// is on the left side of line
			if (z > 0.0) {
				res = polys[i];
				intFound = 1;
				break;
			}
		}	
		if(intFound == 1){
			while(count < res[0]){
				convexPoly[2*count] = res[2*count+1];
				convexPoly[2*count+1] = res[2*count+2];
				count++;		
				
			}
		}
	}

	free(polys[0]);
	free(polys[1]);

	// update convexPoly
	return count;
}// end of halfPlaneIntersection()

//---------------------------------------------------------------------------------//
// splitConvexPolygon()
// Splits a convex polygons into one or to polygons
// through the intersection with the given line.
// (regarding the directional vector) of the given line.
// www.mathematik.uni-ulm.de/stochastik/lehre/ws03_04/rt/Geometry2D.ps
//---------------------------------------------------------------------------------//
int splitConvexPolygon(int numvertices,REAL *convexPoly, REAL x1, REAL y1, REAL x2, REAL y2, REAL *polys[]) {
	/*
	* state = 0: before the first intersection (with the line)
	* state = 1: after the first intersection (with the line)
	* state = 2: after the second intersection (with the line)
	*/
	int state = 0;
	REAL p[3];
	// poly1 is constructed in states 0 and 2
	REAL *poly1 = polys[0];
	int poly1counter = 0;
	// poly2 is constructed in state 1
	REAL *poly2 = polys[1];
	int poly2counter = 0;
	int numpolys;
	int i;
	REAL compConst = 0.000000000001;
	// for debugging 
	int case1 = 0, case2 = 0, case3 = 0, case31 = 0, case32 = 0, case33 = 0, case311 = 0, case3111 = 0;
	// intersect all edges of poly with line
	for (i = 0; i < 2*numvertices; i=i+2) {
		int j = (i+2 >= 2*numvertices) ? 0 : i+2;
		lineLineSegmentIntersection(x1, y1, x2, y2, convexPoly[i], convexPoly[i+1], convexPoly[j],convexPoly[j+1],p);
		// if this edge does not intersect with line
		if (fabs(p[0]-0.0) <= compConst) {
			//System.out.println("null");
			// add p[j] to the proper polygon
			if (state == 1){
				poly2counter++;
				poly2[2*poly2counter-1] = convexPoly[j];
				poly2[2*poly2counter] = convexPoly[j+1];
			}else{
				poly1counter++;
				poly1[2*poly1counter-1] = convexPoly[j];
				poly1[2*poly1counter] = convexPoly[j+1];
			}
			// debug
			case1++;
		}
		// ... or if the intersection is the whole edge
		else if (fabs(p[0]-2.0) <= compConst) {
			//System.out.println(o);
			// then we can not reach state 1 and 2
			poly1counter++;
			poly1[2*poly1counter-1] = convexPoly[j];
			poly1[2*poly1counter] = convexPoly[j+1];
			// debug
			case2++;
		}
		// ... or if the intersection is a point
		else{	
			// debug
			case3++;		
			// if the point is the second vertex of the edge
			if (fabs(p[1] - convexPoly[j]) <= compConst && fabs(p[2] - convexPoly[j+1]) <= compConst) {
				// debug
				case31++;
				if (state == 1) {
					poly2counter++;
					poly2[2*poly2counter-1] = convexPoly[j];
					poly2[2*poly2counter] = convexPoly[j+1];
					poly1counter++;
					poly1[2*poly1counter-1] = convexPoly[j];
					poly1[2*poly1counter] = convexPoly[j+1];					
					state++;
				}
				else if (state == 0) {
					// debug
					case311++;
					poly1counter++;
					poly1[2*poly1counter-1] = convexPoly[j];
					poly1[2*poly1counter] = convexPoly[j+1];
					// test whether the polygon is splitted
					// or the line only touches the polygon
					if (i+4 < 2*numvertices) {
						int s1 = linePointLocation(x1, y1, x2, y2, convexPoly[i], convexPoly[i+1]);
						int s2 = linePointLocation(x1, y1, x2, y2, convexPoly[i+4], convexPoly[i+5]);
						// the line only splits the polygon
						// when the previous and next vertex lie
						// on different sides of the line
						if (s1 != s2 && s1 != 0 && s2 != 0) {
							// debug
							case3111++;
							poly2counter++;
							poly2[2*poly2counter-1] = convexPoly[j];
							poly2[2*poly2counter] = convexPoly[j+1]; 
							state++;
						}
					}
				}
			}
			// ... if the point is not the other vertex of the edge
			else if (!(fabs(p[1] - convexPoly[i]) <= compConst && fabs(p[2] - convexPoly[i+1]) <= compConst)) {
				// debug
				case32++;
				poly1counter++;
				poly1[2*poly1counter-1] = p[1];
				poly1[2*poly1counter] = p[2];
				poly2counter++;
				poly2[2*poly2counter-1] = p[1];
				poly2[2*poly2counter] = p[2];				
				if (state == 1){
					poly1counter++;
					poly1[2*poly1counter-1] = convexPoly[j];
					poly1[2*poly1counter] = convexPoly[j+1];
				}else if (state == 0){
					poly2counter++;
					poly2[2*poly2counter-1] = convexPoly[j];
					poly2[2*poly2counter] = convexPoly[j+1];
				}				
				state++;
			}
			// ... else if the point is the second vertex of the edge
			else {
				// debug
				case33++;
				if (state == 1){
					poly2counter++;
					poly2[2*poly2counter-1] = convexPoly[j];
					poly2[2*poly2counter] = convexPoly[j+1];
				}else{
					poly1counter++;
					poly1[2*poly1counter-1] = convexPoly[j];
					poly1[2*poly1counter] = convexPoly[j+1];
				}
			}
		}
	}
	// after splitting the state must be 0 or 2
	// (depending whether the polygon was splitted or not)
	if (state != 0 && state != 2) {
// 		printf("there is something wrong state: %d\n", state);
// 		printf("polygon might not be convex!!\n");
// 		printf("case1: %d\ncase2: %d\ncase3: %d\ncase31: %d case311: %d case3111: %d\ncase32: %d\ncase33: %d\n", case1, case2, case3, case31, case311, case3111, case32, case33);
// 		printf("numvertices %d\n=============\n", numvertices);
		// if there is something wrong with the intersection, just ignore this one				
		numpolys = 3;
	}else{
		// finally convert the vertex lists into convex polygons
		numpolys = (state == 0) ? 1 : 2;
		poly1[0] = poly1counter;
		poly2[0] = poly2counter;
		// convert the first convex polygon		
		polys[0] = poly1;
		// convert the second convex polygon
		if (state == 2) {
			polys[1] = poly2;
		}	
	}		
	return numpolys;
}// end of splitConvexPoly()

//---------------------------------------------------------------------------------//
// linePointLocation()
// Determines on which side (relative to the direction)
// of the given line and the point lies
// (regarding the directional vector) of the given line.
// www.mathematik.uni-ulm.de/stochastik/lehre/ws03_04/rt/Geometry2D.ps
//---------------------------------------------------------------------------------//
int linePointLocation(REAL x1, REAL y1, REAL x2, REAL y2, REAL x, REAL y){
	REAL z;
	if(atan((y2-y1)/(x2-x1))*180.0/PI == 90.0){
		if(fabs(x1-x) <= 0.00000000001)
			return 0;
	}else{
		if(fabs(y1 + (((y2 - y1)*(x - x1))/(x2 - x1)) - y) <= compConst)
			return 0;
	}
	// third component of the 3 dimensional product
	z = (x2 - x1) * (y - y1) - (y2 -y1) * (x - x1);
	if(fabs(z - 0.0) <= 0.00000000001){
		return 0;
	}else if(z > 0){
		return 1;
	}else{
		return 2;
	}	
}// end of linePointLocation()
//---------------------------------------------------------------------------------//
// lineLineSegmentIntersection() 
// Given four points representing one line and a line segment, returns the intersection point
// referenced to: http://local.wasp.uwa.edu.au/~pbourke/geometry/
//---------------------------------------------------------------------------------//
void lineLineSegmentIntersection(
    REAL x1, REAL y1 ,
    REAL x2, REAL y2 ,
    REAL x3, REAL y3 , 
    REAL x4, REAL y4 , REAL *p)
{
	// x1,y1  P1 coordinates (point of line)
	// x2,y2  P2 coordinates (point of line)	
	// x3,y3  P3 coordinates (point of line segment)
	// x4,y4  P4 coordinates (point of line segment)
	// p[1],p[2]   intersection coordinates
	//
	// This function returns a pointer array which first index indicates
	// weather they intersect on one point or not, followed by coordinate pairs.
		
	REAL u_a, u_b, denom;	
	REAL compConst = 0.0000000000001;	
	// calculate denominator first
	denom = (y4-y3)*(x2-x1)-(x4-x3)*(y2-y1);
	u_a = (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3);
	u_b = (x2-x1)*(y1-y3) - (y2-y1)*(x1-x3);
	
	
	//if(fabs(denom-0.0) < compConst && (fabs(u_b-0.0) < compConst && fabs(u_a-0.0) < compConst)){
 	//printf("denom %.20f u_b  %.20f u_a  %.20f\n",denom, u_b, u_a);
	if(fabs(denom-0.0) < compConst){
	 	if(fabs(u_b-0.0) < compConst && fabs(u_a-0.0) < compConst){
			p[0] = 2.0;	// if denominator and numerator equal to zero, lines are coincident
		}else{
			p[0] = 0.0;// if denominator equals to zero, lines are parallel
		}
		
	}else{ 
	    u_b = u_b/denom;
	    u_a = u_a/denom;
// 	    printf("u_b %.20f\n", u_b);
	    if( u_b < -compConst  || u_b > 1.0 + compConst ){	// check if it is on the line segment		
// 		printf("line (%.20f, %.20f) (%.20f, %.20f) line seg (%.20f, %.20f) (%.20f, %.20f) \n",x1, y1 ,x2, y2 ,x3, y3 , x4, y4);		
		p[0] = 0.0;
	    }else{
		p[0] = 1.0;				
		p[1] = x1 + u_a*(x2-x1); // intersection point
		p[2] = y1 + u_a*(y2-y1);
	    }
	}
	
}// end of lineLineSegmentIntersection()
//---------------------------------------------------------------------------------//
// findPolyCentroid()
// Returns the centroid of a given polygon 
//---------------------------------------------------------------------------------//
void findPolyCentroid(int numpoints, REAL *points, REAL *centroid){
	int i;
	centroid[0] = 0.0;	centroid[1] = 0.0;

	for(i = 0; i < 2*numpoints; i = i+2){
		
		centroid[0] = centroid[0] + points[i];
		centroid[1] = centroid[1] + points[i+1];
		
	}
	centroid[0] = centroid[0]/numpoints;
	centroid[1] = centroid[1]/numpoints;
	
}// end of findPolyCentroid()


//---------------------------------------------------------------------------------//
// circleLineIntersection() 
// Given two points representing a line and 
// a radius together with a center point representing a circle,
// returns the intersection points
// referenced to: http://local.wasp.uwa.edu.au/~pbourke/geometry/sphereline/
//---------------------------------------------------------------------------------//
// returns a pointer to list of intersection points
void circleLineIntersection (
    REAL x1, REAL y1 ,
    REAL x2, REAL y2 ,
    REAL x3, REAL y3 , REAL r , REAL *p)
{
	// x1,y1  P1 coordinates [point of line]
	// x2,y2  P2 coordinates [point of line]
	// x3,y3, r  P3 coordinates(circle center) and radius [circle]	
	// p[1],p[2]; p[3],p[4]   intersection coordinates
	//
	// This function returns a pointer array which first index indicates
	// the number of intersection points, followed by coordinate pairs.
	
	REAL a, b, c, mu, i ;	
	
	a =  (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
	b =  2* ( (x2 - x1)*(x1 - x3) + (y2 - y1)*(y1 - y3)) ;
	c =  x3*x3 + y3*y3 + x1*x1 + y1*y1  - 2* ( x3*x1 + y3*y1 ) - r*r ;
	i =   b * b - 4 * a * c ;
	
	if ( i < 0.0 )	{
		// no intersection
		p[0] = 0.0;	
	}
	else if ( fabs(i-0.0) < compConst ){
		// one intersection
		p[0] = 1.0;
		
		mu = -b/(2*a) ;
		p[1] = x1 + mu*(x2-x1);
		p[2] = y1 + mu*(y2-y1);
		
	}
	else if ( i > 0.0 && !(fabs(a-0.0) < compConst)){
		// two intersections
		p[0] = 2.0;		
		// first intersection
		mu = (-b + sqrt( i )) / (2*a);
		p[1] = x1 + mu*(x2-x1);
		p[2] = y1 + mu*(y2-y1);
		// second intersection
		mu = (-b - sqrt( i )) / (2*a);
		p[3] = x1 + mu*(x2-x1);
		p[4] = y1 + mu*(y2-y1);
		
		
	}else{ 
		p[0] = 0.0;
	}
}// end of circleLineIntersection()


//---------------------------------------------------------------------------------//
// chooseCorrectPoint() 
// Given three points,  
// check if the point is the correct point that we are looking for
//---------------------------------------------------------------------------------//
int chooseCorrectPoint (
    REAL x1, REAL y1 ,
    REAL x2, REAL y2 ,
    REAL x3, REAL y3, int isObtuse )
{

	// x1,y1  P1 coordinates (bisector point of dual edge on triangle)
	// x2,y2  P2 coordinates (intersection point)
	// x3,y3  P3 coordinates (circumcenter point)
	// returns 1.0, if given point is the correct one
	// otherwise return 0.0
	
	
	REAL d1, d2 ;
	int p;
	
	// squared distance between circumcenter and intersection point
	d1 = (x2 - x3)*(x2 - x3) +  (y2 - y3)*(y2 - y3);
	// squared distance between bisector point and intersection point
	d2 = (x2 - x1)*(x2 - x1) +  (y2 - y1)*(y2 - y1); 
	
	if(isObtuse == 1){
		// obtuse case
		if(d2 >= d1){
			p = 1; // means we have found the right point
		}else{
			p = 0; // means take the other point
		}	
	}else{
		// non-obtuse case
		if(d2 < d1){
			p = 1; // means we have found the right point
		}else{
			p = 0; // means take the other point
		}	
	}
	/// HANDLE RIGHT TRIANGLE CASE!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	return(p);
 
}// end of chooseCorrectPoint()

//---------------------------------------------------------------------------------//
// pointBetweenPoints() 
// Given three points on a line,  
// returns 1 if point is between given points
//---------------------------------------------------------------------------------//

void pointBetweenPoints(REAL x1, REAL y1, REAL x2, REAL y2, REAL x, REAL y, REAL *p){
	
	// x1,y1  P1 coordinates [point of line] (point on Voronoi edge - intersection)
	// x2,y2  P2 coordinates [point of line] (circumcenter)
	// x,y    P3 coordinates [point to be compared]	(neighbor's circumcenter)
	//
	// This function returns a pointer array which first index indicates
	// the whether the point is in between the other points, followed by coordinate pairs.
	
	// now check whether the point is close to circumcenter than intersection point
	// BETWEEN THE POINTS
	if((x2 - x)*(x2 - x)+(y2 - y)*(y2 - y) < (x2 - x1)*(x2 - x1)+(y2 - y1)*(y2 - y1)){
		p[0] = 1.0;
		// calculate the squared distance to circumcenter
		p[1] = (x -x2)*(x -x2) + (y -y2)*(y -y2);
		p[2] = x;
		p[3] = y;
	}// *NOT* BETWEEN THE POINTS
	else{
		p[0] = 0.0;
		p[1] = 0.0;
		p[2] = 0.0;
		p[3] = 0.0;
	}
	

	
}// end of pointBetweenPoints()

//---------------------------------------------------------------------------------//
// testTriangleAngle() 
// Given three coordinates of a triangle,  
// tests a triangle to see if it satisfies the minimum and/or maximum angle condition 
// Returns 1, if it is a BAD triangle, returns 0 if it is a GOOD triangle
//---------------------------------------------------------------------------------//
int testTriangleAngle(behavior *b, 
				REAL *x1, REAL *y1,
				REAL *x2, REAL *y2,
				REAL *x3, REAL *y3 )
{
    // variables keeping the distance values for the edges
  REAL dxod, dyod, dxda, dyda, dxao, dyao;
  REAL dxod2, dyod2, dxda2, dyda2, dxao2, dyao2;

  REAL apexlen, orglen, destlen, minedge;
  REAL angle;    // in order to check minimum angle condition 
  
  REAL maxangle, maxedge;    // in order to check minimum angle condition
 // calculate the side lengths
  
  dxod = *x1 - *x2;
  dyod = *y1 - *y2;
  dxda = *x2 - *x3;
  dyda = *y2 - *y3;
  dxao = *x3 - *x1;
  dyao = *y3 - *y1;
  // calculate the squares of the side lentghs
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

  // try to find the minimum edge and accordingly the pqr orientation
  if ((apexlen < orglen) && (apexlen < destlen)) {
    /* The edge opposite the apex is shortest. */
    minedge = apexlen;
    /* Find the square of the cosine of the angle at the apex. */
    angle = dxda * dxao + dyda * dyao;
    angle = angle * angle / (orglen * destlen);
    
    
  } else if (orglen < destlen) {
    /* The edge opposite the origin is shortest. */
    minedge = orglen;
    /* Find the square of the cosine of the angle at the origin. */
    angle = dxod * dxao + dyod * dyao;
    angle = angle * angle / (apexlen * destlen);
   
    
  } else {
    /* The edge opposite the destination is shortest. */
    minedge = destlen;
    /* Find the square of the cosine of the angle at the destination. */
    angle = dxod * dxda + dyod * dyda;
    angle = angle * angle / (apexlen * orglen);
   
  }
  // try to find the maximum edge and accordingly the pqr orientation
  if ((apexlen > orglen) && (apexlen > destlen)) {
    /* The edge opposite the apex is longest. */
    maxedge = apexlen;
    /* Find the cosine of the angle at the apex. */
    maxangle = (orglen + destlen - apexlen)/ (2*sqrt(orglen)*sqrt(destlen));     
  } else if (orglen > destlen) {
    /* The edge opposite the origin is longest. */
    maxedge = orglen;
    /* Find the cosine of the angle at the origin. */
    maxangle = (apexlen + destlen - orglen)/(2*sqrt(apexlen)*sqrt(destlen));
  } else {
    /* The edge opposite the destination is longest. */
    maxedge = destlen;
    /* Find the cosine of the angle at the destination. */
    maxangle = (apexlen + orglen -destlen)/(2*sqrt(apexlen)*sqrt(orglen));
  }

 
  /* Check whether the angle is smaller than permitted. */
  if ((angle > b->goodangle)  ||  (b->maxangle != 0.00 && maxangle < b->maxgoodangle)) {
   	return 1;// it is a bad triangle
  }
  return 0;// it is a good triangle

}// end of testTriangleAngle()

//---------------------------------------------------------------------------------//
// minDistanceToNeigbor()
// Given the triangulation, and a vertex
// returns the minimum distance to the vertices of the triangle where the given vertex located
//---------------------------------------------------------------------------------//
REAL minDistanceToNeigbor(mesh *m, behavior *b, REAL newlocX, REAL newlocY, struct otri *searchtri){
	struct otri horiz;	// for search operation
	enum locateresult intersect;
	vertex v1, v2, v3, newvertex, torg, tdest;
	REAL d1, d2, d3, ahead;
	triangle ptr;                         /* Temporary variable used by sym(). */
		
	newvertex = (vertex) poolalloc(&m->vertices);
	newvertex[0] = newlocX;
	newvertex[1] = newlocY;
// 	printf("newvertex %f,%f\n", newvertex[0], newvertex[1]);
	/* Find the location of the vertex to be inserted.  Check if a good */
	/*   starting triangle has already been provided by the caller.     */	
	/* Find a boundary triangle. */
	/*horiz.tri = m->dummytri;
	horiz.orient = 0;
	symself(horiz);	*/
	/* Search for a triangle containing `newvertex'. */
	/* Start searching from the triangle provided by the caller. */
	/* Where are we? */
	org(*searchtri, torg);
	dest(*searchtri, tdest);
	/* Check the starting triangle's vertices. */
	if ((torg[0] == newvertex[0]) && (torg[1] == newvertex[1])) {
		intersect = ONVERTEX;
		otricopy(*searchtri, horiz);

	}else if ((tdest[0] == newvertex[0]) && (tdest[1] == newvertex[1])) {
		lnextself(*searchtri);
		intersect =  ONVERTEX;
		otricopy(*searchtri, horiz);
	}else{
		/* Orient `searchtri' to fit the preconditions of calling preciselocate(). */
		ahead = counterclockwise(m, b, torg, tdest, newvertex);
		if (ahead < 0.0) {
			/* Turn around so that `searchpoint' is to the left of the */
			/*   edge specified by `searchtri'.                        */
			symself(*searchtri);						
			otricopy(*searchtri, horiz);
			intersect = preciselocate(m, b, newvertex, &horiz, 0);	
		} else if (ahead == 0.0) {
			/* Check if `searchpoint' is between `torg' and `tdest'. */
			if (((torg[0] < newvertex[0]) == (newvertex[0] < tdest[0])) &&
				((torg[1] < newvertex[1]) == (newvertex[1] < tdest[1]))) {
				intersect = ONEDGE;
				otricopy(*searchtri, horiz);
				
			}			
		}else{
			otricopy(*searchtri, horiz);
			intersect = preciselocate(m, b, newvertex, &horiz, 0);	
		}			
	}		
	if(intersect == ONVERTEX || intersect == OUTSIDE){		
		// set distance to 0
		vertexdealloc(m,newvertex);
		return 0.0;
	}else{ // intersect == ONEDGE || intersect == INTRIANGLE
		// find the triangle vertices
		org(horiz, v1);
		dest(horiz, v2);
		apex(horiz, v3);
		d1 = (v1[0] - newvertex[0]) * (v1[0] - newvertex[0]) + (v1[1] - newvertex[1]) * (v1[1] - newvertex[1]);
		d2 = (v2[0] - newvertex[0]) * (v2[0] - newvertex[0]) + (v2[1] - newvertex[1]) * (v2[1] - newvertex[1]);
		d3 = (v3[0] - newvertex[0]) * (v3[0] - newvertex[0]) + (v3[1] - newvertex[1]) * (v3[1] - newvertex[1]);
		vertexdealloc(m,newvertex);		
		// find minimum of the distance
		if(d1 <= d2 && d1 <= d3){			
			return d1;
		}else if(d2 <= d3){
			return d2;
		}else{
			return d3;
		}
	}	
	
}// end of minDistanceToNeighbor()

/*============================NEW CODE ENDS==============================*/
#endif


