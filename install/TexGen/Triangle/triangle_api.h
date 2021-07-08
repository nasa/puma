#ifndef TRIANGLE_API_H
#define TRIANGLE_API_H

#include "triangle.h"
#include "triangle_export.h"
#include "triangle_version.h"

#include <stdio.h>
#include <stdlib.h>

//#define EXPORT __declspec(dllexport)
#define EXPORT TRIANGLE_EXPORT

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct context_t {
		mesh *m;
		behavior *b;
	} context;

	typedef struct statistics_t {
		int vertices;
		int undeads;
		int triangles;
		int hullsize;
		int subsegs;
		int edges;
		rect bounds;
	} statistics;

	/**
	 * Gets the Triangle version.
	 * @param version Integer array of length 4 [major, minor, patch, has_acute].
	 */
	EXPORT void triangle_version(int version[4]);
	
	/**
	 * Create a context struct.
	 * @return Pointer to context struct.
	 */
	EXPORT context* triangle_context_create();
	
	/**
	 * Destroy a context struct.
	 * @param ctx Pointer to context struct.
	 */
	EXPORT void triangle_context_destroy(context* ctx);
	
	/**
	 * Parse Triangle options string.
	 * @param ctx Pointer to context struct.
	 * @param options String containing Triangle options.
	 * @return Integer status code.
	 */
	EXPORT int triangle_context_options(context* ctx, char *options);
	
	/**
	 * Copy the context behavior to given behavior struct.
	 * @param ctx Pointer to context struct.
	 * @param out Pointer to input behavior struct.
	 */
	EXPORT void triangle_context_get_behavior(context* ctx, behavior *out);
	
	/**
	 * Copy given behavior to the context behavior struct.
	 * @param ctx Pointer to context struct.
	 * @param in Pointer to output behavior struct.
	 * @return Integer status code.
	 */
	EXPORT int triangle_context_set_behavior(context* ctx, behavior *in);
	
	/**
	 * Triangulate an input polygon.
	 * @param ctx Pointer to context struct.
	 * @param in Pointer to triangleio struct, containing the input polygon.
	 * @return Integer status code.
	 */
	EXPORT int triangle_mesh_create(context* ctx, triangleio *in);
	
	/**
	 * Load a previously computed mesh.
	 * @param ctx Pointer to context struct.
	 * @param in Pointer to triangleio struct, containing a previously computed mesh.
	 * @return Integer status code.
	 */
	EXPORT int triangle_mesh_load(context* ctx, triangleio *in);
	
	/**
	 * Refine the current mesh.
	 * @param ctx Pointer to context struct.
	 * @return Integer status code.
	 */
	EXPORT int triangle_mesh_refine(context* ctx);
	
	/**
	 * Copy mesh to triangleio struct.
	 * @param ctx Pointer to context struct.
	 * @param out Pointer to triangleio struct, containing the mesh.
	 * @param edges If non-zero, write edges to triangleio.
	 * @param neighbors If non-zero, write neighbors to triangleio.
	 * @return Integer status code.
	 */
	EXPORT int triangle_mesh_copy(context* ctx, triangleio *out,
		int edges, int neighbors);
	
	/**
	 * Compute mesh statistics.
	 * @param ctx Pointer to context struct.
	 * @param s Pointer to statistics struct.
	 * @return Integer status code.
	 */
	EXPORT int triangle_mesh_statistics(context *ctx, statistics *s);
	
	/**
	 * Get approximate value of allocated bytes.
	 * @param ctx Pointer to context struct.
	 * @return Number of bytes.
	 */
	EXPORT int triangle_memory(context* ctx);

	/**
	 * Compute mesh quality statistics.
	 * @param ctx Pointer to context struct.
	 * @param q Pointer to quality struct.
	 * @return Integer status code.
	 */
	EXPORT int triangle_mesh_quality(context *ctx, quality *q);
	
	/**
	 * Check the mesh for topological consistency.
	 * @param ctx Pointer to context struct.
	 * @return Number of elements that failed the test.
	 */
	EXPORT int triangle_check_mesh(context *ctx);
	
	/**
	 * Check if the mesh is (constrained) Delaunay.
	 * @param ctx Pointer to context struct.
	 * @return Number of elements that failed the test.
	 */
	EXPORT int triangle_check_delaunay(context *ctx);
	
	/**
	 * Free memory that was allocated by triangle.
	 * @param memptr Pointer to native memory.
	 */
	EXPORT void triangle_free(VOID *memptr);

#ifndef NO_FILE_IO
	/**
	 * Write node file.
	 * @param ctx Pointer to context struct.
	 * @param file File pointer (to .node file).
	 * @return Integer status code.
	 */
	EXPORT int triangle_write_nodes(context *ctx, FILE *file);
	
	/**
	 * Write ele file.
	 * @param ctx Pointer to context struct.
	 * @param file File pointer (to .ele file).
	 * @return Integer status code.
	 */
	EXPORT int triangle_write_elements(context *ctx, FILE *file);
	
	/**
	 * Write poly file.
	 * @param ctx Pointer to context struct.
	 * @param file File pointer (to .poly file).
	 * @param holelist Pointer to an array of REALs.
	 * @param holes Number of holes.
	 * @param regionlist Pointer to an array of REALs.
	 * @param regions Number of regions.
	 * @return Integer status code.
	 */
	EXPORT int triangle_write_poly(context *ctx, FILE *file,
		REAL *holelist, int holes, REAL *regionlist, int regions);
	
	/**
	 * Write edge file.
	 * @param ctx Pointer to context struct.
	 * @param file File pointer (to .edge file).
	 * @return Integer status code.
	 */
	EXPORT int triangle_write_edges(context *ctx, FILE *file);
	
	/**
	 * Write neighbor file.
	 * @param ctx Pointer to context struct.
	 * @param file File pointer (to .neigh file).
	 * @return Integer status code.
	 */
	EXPORT int triangle_write_neighbors(context *ctx, FILE *file);
	
	/**
	 * Write mesh to encapsulated postscript file.
	 * @param ctx Pointer to context struct.
	 * @param file File pointer (to .eps file).
	 * @return Integer status code.
	 */
	EXPORT int triangle_write_eps(context *ctx, FILE *file);
	
	/**
	 * Read nodes file.
	 * @param filename File name (path to .node file).
	 * @param io Pointer to triangleio struct.
	 * @param firstnode Number of the first node (output).
	 * @return Integer status code.
	 */
	EXPORT int triangle_read_nodes(const char* filename, triangleio *io, int *firstnode);
	
	/**
	 * Read poly file.
	 * @param filename File name (path to .poly file).
	 * @param io Pointer to triangleio struct.
	 * @param firstnode Number of the first node (output).
	 * @return Integer status code.
	 */
	EXPORT int triangle_read_poly(const char* filename, triangleio *io, int *firstnode);
	
	/**
	 * Read elements file.
	 * @param filename File name (path to .ele file).
	 * @param io Pointer to triangleio struct.
	 * @return Integer status code.
	 */
	EXPORT int triangle_read_elements(const char* filename, triangleio *io);
	
	/**
	 * Read area file.
	 * @param filename File name (path to .area file).
	 * @param io Pointer to triangleio struct.
	 * @return Integer status code.
	 */
	EXPORT int triangle_read_area(const char* filename, triangleio *io);

#endif /* NO_FILE_IO */

#ifdef __cplusplus
}
#endif

#endif /* TRIANGLE_API_H */
