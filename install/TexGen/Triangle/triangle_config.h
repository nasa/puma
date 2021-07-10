#ifndef TRIANGLE_CONFIG_H
#define TRIANGLE_CONFIG_H

/* For single precision (which will save some memory and reduce paging),     */
/*   define the symbol SINGLE by using the -DSINGLE compiler switch or by    */
/*   writing "#define SINGLE" below.                                         */
/*                                                                           */
/* For double precision (which will allow you to refine meshes to a smaller  */
/*   edge length), leave SINGLE undefined.                                   */
/*                                                                           */
/* Double precision uses more memory, but improves the resolution of the     */
/*   meshes you can generate with Triangle.  It also reduces the likelihood  */
/*   of a floating exception due to overflow.  Finally, it is much faster    */
/*   than single precision on 64-bit architectures like the DEC Alpha.  I    */
/*   recommend double precision unless you want to generate a mesh for which */
/*   you do not have enough memory.                                          */

/* Define as "float" for single precision. */
#define REAL double

/* The next line is used to outsmart some very stupid compilers.  If your    */
/*   compiler is smarter, feel free to replace the "int" with "void".        */
/*   Not that it matters.                                                    */
#define VOID void

/* Triangle status codes. */

#define TRI_OK 0
#define TRI_FAILURE (-1)
#define TRI_OPTIONS (-2)
#define TRI_INPUT (-3)
#define TRI_FIND_DIRECTION (-4)
#define TRI_SEG_SPLIT (-5)
#define TRI_SEG_INTERSECT (-6)
#define TRI_SEG_INSERT (-7)
#define TRI_SEG_SCOUT (-8)
#define TRI_FILE_OPEN (-9)
#define TRI_FILE_READ (-10)
#define TRI_NULL (-20)

/* Set correct size for pointer alignment calculations */
#if defined(_M_X64) || defined(__amd64__)
  #define ULONG_PTR unsigned long long
  #define LX "%llx"
#else
  #define ULONG_PTR unsigned long
  #define LX "%lx"
#endif

/* If yours is not a Unix system, define the NO_TIMER compiler switch to     */
/*   remove the Unix-specific timing code.                                   */

/* #define NO_TIMER */

/* To insert lots of self-checks for internal errors, define the SELF_CHECK  */
/*   symbol.  This will slow down the program significantly.  It is best to  */
/*   define the symbol using the -DSELF_CHECK compiler switch, but you could */
/*   write "#define SELF_CHECK" below.  If you are modifying this code, I    */
/*   recommend you turn self-checks on until your work is debugged.          */

/* #define SELF_CHECK */

/* To compile Triangle as a callable object library (triangle.o), define the */
/*   TRILIBRARY symbol.  Read the file triangle.h for details on how to call */
/*   the procedure triangulate() that results.                               */

/* #define TRILIBRARY */

/* It is possible to generate a smaller version of Triangle using one or     */
/*   both of the following symbols.  Define the REDUCED symbol to eliminate  */
/*   all features that are primarily of research interest; specifically, the */
/*   -i, -F, -s, and -C switches.  Define the CDT_ONLY symbol to eliminate   */
/*   all meshing algorithms above and beyond constrained Delaunay            */
/*   triangulation; specifically, the -r, -q, -a, -u, -D, -S, and -s         */
/*   switches.  These reductions are most likely to be useful when           */
/*   generating an object library (triangle.o) by defining the TRILIBRARY    */
/*   symbol.                                                                 */

/* #define REDUCED */
/* #define CDT_ONLY */

/* On some machines, my exact arithmetic routines might be defeated by the   */
/*   use of internal extended precision floating-point registers.  The best  */
/*   way to solve this problem is to set the floating-point registers to use */
/*   single or double precision internally.  On 80x86 processors, this may   */
/*   be accomplished by setting the CPU86 symbol for the Microsoft C         */
/*   compiler, or the LINUX symbol for the gcc compiler running on Linux.    */
/*                                                                           */
/* An inferior solution is to declare certain values as `volatile', thus     */
/*   forcing them to be stored to memory and rounded off.  Unfortunately,    */
/*   this solution might slow Triangle down quite a bit.  To use volatile    */
/*   values, write "#define INEXACT volatile" below.  Normally, however,     */
/*   INEXACT should be defined to be nothing.  ("#define INEXACT".)          */
/*                                                                           */
/* For more discussion, see http://www.cs.cmu.edu/~quake/robust.pc.html .    */
/*   For yet more discussion, see Section 5 of my paper, "Adaptive Precision */
/*   Floating-Point Arithmetic and Fast Robust Geometric Predicates" (also   */
/*   available as Section 6.6 of my dissertation).                           */

/* #define CPU86 */
/* #define LINUX */

#define INEXACT /* Nothing */
/* #define INEXACT volatile */

/* Maximum number of characters in a file name (including the null).         */

#define FILENAMESIZE 2048

/* Maximum number of characters in a line read from a file (including the    */
/*   null).                                                                  */

#define INPUTLINESIZE 1024

/* For efficiency, a variety of data structures are allocated in bulk.  The  */
/*   following constants determine how many of each structure is allocated   */
/*   at once.                                                                */

#define TRIPERBLOCK 4092           /* Number of triangles allocated at once. */
#define SUBSEGPERBLOCK 508       /* Number of subsegments allocated at once. */
#define VERTEXPERBLOCK 4092         /* Number of vertices allocated at once. */
#define VIRUSPERBLOCK 1020   /* Number of virus triangles allocated at once. */
/* Number of encroached subsegments allocated at once. */
#define BADSUBSEGPERBLOCK 252
/* Number of skinny triangles allocated at once. */
#define BADTRIPERBLOCK 4092
/* Number of flipped triangles allocated at once. */
#define FLIPSTACKERPERBLOCK 252
/* Number of splay tree nodes allocated at once. */
#define SPLAYNODEPERBLOCK 508

/* The vertex types.   A DEADVERTEX has been deleted entirely.  An           */
/*   UNDEADVERTEX is not part of the mesh, but is written to the output      */
/*   .node file and affects the node indexing in the other output files.     */

#define INPUTVERTEX 0
#define SEGMENTVERTEX 1
#define FREEVERTEX 2
#define DEADVERTEX -32768
#define UNDEADVERTEX -32767

/* Two constants for algorithms based on random sampling.  Both constants    */
/*   have been chosen empirically to optimize their respective algorithms.   */

/* Used for the point location scheme of Mucke, Saias, and Zhu, to decide    */
/*   how large a random sample of triangles to inspect.                      */

#define SAMPLEFACTOR 11

/* Used in Fortune's sweepline Delaunay algorithm to determine what fraction */
/*   of boundary edges should be maintained in the splay tree for point      */
/*   location on the front.                                                  */

#define SAMPLERATE 10

/* A number that speaks for itself, every kissable digit.                    */

#define PI 3.141592653589793238462643383279502884197169399375105820974944592308

/* Another fave.                                                             */

#define SQUAREROOTTWO 1.4142135623730950488016887242096980785696718753769480732

/* And here's one for those of you who are intimidated by math.              */

#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333

#endif /* TRIANGLE_CONFIG_H */