#ifndef TRIANGLE_INTERNAL_H
#define TRIANGLE_INTERNAL_H

#include "../triangle.h"

#include <stdio.h>
#include <stdlib.h>

void interpolate(vertex newvertex, vertex org, vertex dest, vertex apex, int nextras);

void behavior_update(behavior *b);

/********* Memory allocation and program exit wrappers begin here    *********/
/**                                                                         **/

void triexit(int status);

VOID *trimalloc(int size);

void trifree(VOID *memptr);


/**                                                                         **/
/********* Memory allocation and program exit wrappers end here      *********/

/********* User interaction routines begin here                      *********/
/**                                                                         **/

void internalerror();

/**
 * Read the command line, identify switches and set up options.
 */
void parsecommandline(char *options, behavior *b);

/**                                                                         **/
/********* User interaction routines begin here                      *********/

/********* Debugging routines begin here                             *********/
/**                                                                         **/

/**
 * Print out the details of an oriented triangle.
 */
void printtriangle(mesh *m, behavior *b, struct otri *t);

/**
 * Print out the details of an oriented subsegment.
 */
void printsubseg(mesh *m, behavior *b, struct osub *s);

/**                                                                         **/
/********* Debugging routines end here                               *********/

/********* Memory management routines begin here                     *********/
/**                                                                         **/

/**
 * Set all of a pool's fields to zero.
 */
void poolzero(struct memorypool *pool);

/**
 * Deallocate all items in a pool.
 */
void poolrestart(struct memorypool *pool);

/**
 * Initialize a pool of memory for allocation of items.
 */
void poolinit(struct memorypool *pool, int bytecount, int itemcount,
              int firstitemcount, int alignment);

/**
 * Free to the operating system all memory taken by a pool.
 */
void pooldeinit(struct memorypool *pool);

/**
 * Allocate space for an item.
 */
VOID *poolalloc(struct memorypool *pool);

/**
 * Deallocate space for an item.
 */
void pooldealloc(struct memorypool *pool, VOID *dyingitem);

/**
 * Prepare to traverse the entire list of items.
 */
void traversalinit(struct memorypool *pool);

/**
 * Find the next item in the list.
 */
VOID *traverse(struct memorypool *pool);

/**
 * Initialize the triangle that fills "outer space" and the
 * omnipresent subsegment.
 */
void dummyinit(mesh *m, behavior *b, int trianglebytes,
               int subsegbytes);

void initializevertexpool(mesh *m, behavior *b);

void initializetrisubpools(mesh *m, behavior *b);

void triangledealloc(mesh *m, triangle *dyingtriangle);

triangle *triangletraverse(mesh *m);

void subsegdealloc(mesh *m, subseg *dyingsubseg);

subseg *subsegtraverse(mesh *m);

void vertexdealloc(mesh *m, vertex dyingvertex);

vertex vertextraverse(mesh *m);

void badsubsegdealloc(mesh *m, struct badsubseg *dyingseg);

struct badsubseg *badsubsegtraverse(mesh *m);

vertex getvertex(mesh *m, behavior *b, int number);

void triangledeinit(mesh *m, behavior *b);

/**                                                                         **/
/********* Memory management routines end here                       *********/

/********* Constructors begin here                                   *********/
/**                                                                         **/

void maketriangle(mesh *m, behavior *b, struct otri *newotri);

void makesubseg(mesh *m, struct osub *newsubseg);

/**                                                                         **/
/********* Constructors end here                                     *********/

void triangleinit(mesh *m);

unsigned long randomnation(unsigned int choices);

/********* Mesh quality testing routines begin here                  *********/
/**                                                                         **/

int checkmesh(mesh *m, behavior *b);

int checkdelaunay(mesh *m, behavior *b);

#ifndef CDT_ONLY

void enqueuebadtriang(mesh *m, behavior *b,
                      struct badtriang *badtri);

void enqueuebadtri(mesh *m, behavior *b, struct otri *enqtri,
                   REAL minedge, vertex enqapex, vertex enqorg, vertex enqdest);

struct badtriang *dequeuebadtriang(mesh *m);

int checkseg4encroach(mesh *m, behavior *b,
                      struct osub *testsubseg);

void testtriangle(mesh *m, behavior *b, struct otri *testtri);

#endif /* not CDT_ONLY */

/**                                                                         **/
/********* Mesh quality testing routines end here                    *********/

/********* Point location routines begin here                        *********/
/**                                                                         **/

void makevertexmap(mesh *m, behavior *b);

enum locateresult preciselocate(mesh *m, behavior *b,
                                vertex searchpoint, struct otri *searchtri,
                                int stopatsubsegment);

enum locateresult locate(mesh *m, behavior *b,
                         vertex searchpoint, struct otri *searchtri);

/**                                                                         **/
/********* Point location routines end here                          *********/

/********* Mesh transformation routines begin here                   *********/
/**                                                                         **/

void insertsubseg(mesh *m, behavior *b, struct otri *tri,
                  int subsegmark);

void flip(mesh *m, behavior *b, struct otri *flipedge);

void unflip(mesh *m, behavior *b, struct otri *flipedge);

enum insertvertexresult insertvertex(mesh *m, behavior *b,
                                     vertex newvertex, struct otri *searchtri,
                                     struct osub *splitseg,
                                     int segmentflaws, int triflaws,
                                     int attribs);

void triangulatepolygon(mesh *m, behavior *b,
                        struct otri *firstedge, struct otri *lastedge,
                        int edgecount, int doflip, int triflaws);

#ifndef CDT_ONLY

void deletevertex(mesh *m, behavior *b, struct otri *deltri);

void undovertex(mesh *m, behavior *b);

#endif /* not CDT_ONLY */

/**                                                                         **/
/********* Mesh transformation routines end here                     *********/

/********* Divide-and-conquer Delaunay triangulation begins here     *********/
/**                                                                         **/

void vertexsort(vertex *sortarray, int arraysize);

void vertexmedian(vertex *sortarray, int arraysize, int median, int axis);

void alternateaxes(vertex *sortarray, int arraysize, int axis);

void mergehulls(mesh *m, behavior *b, struct otri *farleft,
                struct otri *innerleft, struct otri *innerright,
                struct otri *farright, int axis);
                
void divconqrecurse(mesh *m, behavior *b, vertex *sortarray,
                    int vertices, int axis,
                    struct otri *farleft, struct otri *farright);

long removeghosts(mesh *m, behavior *b, struct otri *startghost);

long divconqdelaunay(mesh *m, behavior *b);

/**                                                                         **/
/********* Divide-and-conquer Delaunay triangulation ends here       *********/

/********* Incremental Delaunay triangulation begins here            *********/
/**                                                                         **/

#ifndef REDUCED

void boundingbox(mesh *m, behavior *b);

long removebox(mesh *m, behavior *b);

long incrementaldelaunay(mesh *m, behavior *b);

#endif /* not REDUCED */

/**                                                                         **/
/********* Incremental Delaunay triangulation ends here              *********/

/********* Sweepline Delaunay triangulation begins here              *********/
/**                                                                         **/

#ifndef REDUCED

void eventheapinsert(struct event **heap, int heapsize, struct event *newevent);

void eventheapify(struct event **heap, int heapsize, int eventnum);

void eventheapdelete(struct event **heap, int heapsize, int eventnum);

void createeventheap(mesh *m, struct event ***eventheap,
                     struct event **events, struct event **freeevents);

int rightofhyperbola(mesh *m, struct otri *fronttri, vertex newsite);

REAL circletop(mesh *m, vertex pa, vertex pb, vertex pc, REAL ccwabc);

void check4deadevent(struct otri *checktri, struct event **freeevents,
                     struct event **eventheap, int *heapsize);

struct splaynode *splay(mesh *m, struct splaynode *splaytree,
                        vertex searchpoint, struct otri *searchtri);

struct splaynode *splayinsert(mesh *m, struct splaynode *splayroot,
                              struct otri *newkey, vertex searchpoint);

struct splaynode *circletopinsert(mesh *m, behavior *b,
                                  struct splaynode *splayroot,
                                  struct otri *newkey,
                                  vertex pa, vertex pb, vertex pc, REAL topy);

struct splaynode *frontlocate(mesh *m, struct splaynode *splayroot,
                              struct otri *bottommost, vertex searchvertex,
                              struct otri *searchtri, int *farright);

long sweeplinedelaunay(mesh *m, behavior *b);

#endif /* not REDUCED */

/**                                                                         **/
/********* Sweepline Delaunay triangulation ends here                *********/

/********* General mesh construction routines begin here             *********/
/**                                                                         **/

long delaunay(mesh *m, behavior *b);

#ifndef CDT_ONLY

int reconstruct(mesh *m, behavior *b, int *trianglelist,
                REAL *triangleattriblist, REAL *trianglearealist,
                int elements, int corners, int attribs,
                int *segmentlist,int *segmentmarkerlist, int numberofsegments);

#endif /* not CDT_ONLY */

/**                                                                         **/
/********* General mesh construction routines end here               *********/

/********* Segment insertion begins here                             *********/
/**                                                                         **/

enum finddirectionresult finddirection(mesh *m, behavior *b,
                                       struct otri *searchtri,
                                       vertex searchpoint, int *status);

void segmentintersection(mesh *m, behavior *b,
                         struct otri *splittri, struct osub *splitsubseg,
                         vertex endpoint2, int *status);

int scoutsegment(mesh *m, behavior *b, struct otri *searchtri,
                 vertex endpoint2, int newmark, int *status);

#ifndef REDUCED
#ifndef CDT_ONLY

void conformingedge(mesh *m, behavior *b, vertex endpoint1, vertex endpoint2,
                    int newmark, int *status);

#endif /* not CDT_ONLY */
#endif /* not REDUCED */

void delaunayfixup(mesh *m, behavior *b,
                   struct otri *fixuptri, int leftside);

void constrainededge(mesh *m, behavior *b,
                     struct otri *starttri, vertex endpoint2, int newmark, int *status);

void insertsegment(mesh *m, behavior *b,
                   vertex endpoint1, vertex endpoint2, int newmark, int *status);

void markhull(mesh *m, behavior *b);

void formskeleton(mesh *m, behavior *b, int *segmentlist,
                  int *segmentmarkerlist, int numberofsegments, int *status);

/**                                                                         **/
/********* Segment insertion ends here                               *********/

/********* Carving out holes and concavities begins here             *********/
/**                                                                         **/

void infecthull(mesh *m, behavior *b);

void plague(mesh *m, behavior *b);

void regionplague(mesh *m, behavior *b,
                  REAL attribute, REAL area);

void carveholes(mesh *m, behavior *b, REAL *holelist, int holes,
                REAL *regionlist, int regions);

/**                                                                         **/
/********* Carving out holes and concavities ends here               *********/

/********* Mesh quality maintenance begins here                      *********/
/**                                                                         **/

#ifndef CDT_ONLY

void tallyencs(mesh *m, behavior *b);

void precisionerror();

void splitencsegs(mesh *m, behavior *b, int triflaws, int *status);

void tallyfaces(mesh *m, behavior *b);

void splittriangle(mesh *m, behavior *b,
                   struct badtriang *badtri);

void enforcequality(mesh *m, behavior *b, int *status);

#endif /* not CDT_ONLY */

/**                                                                         **/
/********* Mesh quality maintenance ends here                        *********/

void highorder(mesh *m, behavior *b);

/********* Array I/O routines begin here                              *********/
/**                                                                         **/

int transfernodes(mesh *m, behavior *b, REAL *pointlist,
                   REAL *pointattriblist, int *pointmarkerlist,
                   int numberofpoints, int numberofpointattribs);

void writenodes(mesh *m, behavior *b, REAL **pointlist,
                REAL **pointattriblist, int **pointmarkerlist);

void numbernodes(mesh *m, behavior *b);

void writeelements(mesh *m, behavior *b,
                   int **trianglelist, REAL **triangleattriblist);

void writepoly(mesh *m, behavior *b,
               int **segmentlist, int **segmentmarkerlist);

void writeedges(mesh *m, behavior *b,
                int **edgelist, int **edgemarkerlist);

void writevoronoi(mesh *m, behavior *b, REAL **vpointlist,
                  REAL **vpointattriblist, int **vpointmarkerlist,
                  int **vedgelist, int **vedgemarkerlist, REAL **vnormlist);

void writeneighbors(mesh *m, behavior *b, int **neighborlist);

/**                                                                         **/
/********* Array I/O routines end here                                *********/

/********* File I/O routines begin here                              *********/
/**                                                                         **/

int file_writenodes(mesh *m, behavior *b, FILE *nodefile);

int file_writeelements(mesh *m, behavior *b, FILE *elefile);

int file_writepoly(mesh *m, behavior *b, FILE *polyfile,
				   REAL *holelist, int holes, REAL *regionlist, int regions);

int file_writeedges(mesh *m, behavior *b, FILE *edgefile);

int file_writeneighbors(mesh *m, behavior *b, FILE *neighborfile);

int file_write_eps(mesh* m, behavior *b, FILE *file);

int file_readnodes(FILE *nodefile, triangleio *io, int *firstnode);
				   
int file_readpoly(FILE *nodefile, triangleio *io, int *firstnode);
				   
int file_readelements(FILE *nodefile, triangleio *io);

int file_readelementsarea(FILE *file, triangleio *io);

/**                                                                         **/
/********* File I/O routines end here                                *********/

int quality_statistics(mesh *m, behavior *b, quality *q);

#endif /* TRIANGLE_INTERNAL_H */
