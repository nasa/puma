
#include "triangle_helper.h"
#include "triangle_internal.h"

int triangle_check_context(context* ctx)
{
	if (ctx == NULL) {
		return TRI_FAILURE;
	}
	if (ctx->b == NULL || ctx->m == NULL) {
		return TRI_FAILURE;
	}

	return TRI_OK;
}

int triangle_check_behavior(behavior* b)
{
	if (b->fixedarea && b->maxarea <= 0.0) {
		return TRI_OPTIONS;
	}
	return TRI_OK;
}

int triangle_check_triangleio(triangleio* io, int firstnumber)
{
	int i;
	int a, b, c;
	REAL x1, y1, x2, y2;

	int invertices = io->numberofpoints;
	int insegments = io->numberofsegments;
	int inelements = io->numberoftriangles;
	int corners = io->numberofcorners;

	for (i = 0; i < insegments; i++) {
		a = io->segmentlist[2 * i];
		b = io->segmentlist[2 * i + 1];
		if ((a < firstnumber) || (a >= firstnumber + invertices)) {
			// TODO: Warning: Invalid first endpoint of segment (firstnumber + i).
			return TRI_FAILURE;
		} else if ((b < firstnumber) || (b >= firstnumber + invertices)) {
			// TODO: Warning: Invalid second endpoint of segment (firstnumber + i).
			return TRI_FAILURE;
		} else if (a == b) {
			// TODO: Warning: Endpoints of segment (firstnumber + i) are coincident.
			return TRI_FAILURE;
		} else {
			x1 = io->pointlist[2 * a];
			x2 = io->pointlist[2 * a + 1];
			y1 = io->pointlist[2 * b];
			y2 = io->pointlist[2 * b + 1];

			if ((x1 == x2) && (y1 == y2)) {
				// TODO: Warning:  Endpoints of segment (firstnumber + i) are coincident.
				return TRI_FAILURE;
			}
		}
	}

	for (i = 0; i < inelements; i++) {
		a = io->trianglelist[corners * i];
		b = io->trianglelist[corners * i + 1];
		c = io->trianglelist[corners * i + 2];

		if ((a < firstnumber) || (a >= firstnumber + invertices) ||
			(b < firstnumber) || (b >= firstnumber + invertices) ||
			(c < firstnumber) || (c >= firstnumber + invertices)) {
				// TODO: Error: Triangle (i) has an invalid vertex index.
				return TRI_FAILURE;
		}
	}

	return TRI_OK;
}

void triangle_restore_pointmarkers(context* ctx, int *pointmarkers)
{
	mesh *m;
	behavior *b;
	vertex vertexloop;
	int vertexnumber;

	m = ctx->m;
	b = ctx->b;

	traversalinit(&m->vertices);
	vertexnumber = 0;
	vertexloop = vertextraverse(m);
	while (vertexloop != (vertex)NULL) {
		if (!b->jettison || (vertextype(vertexloop) != UNDEADVERTEX)) {
			setvertexmark(vertexloop, pointmarkers[vertexnumber]);
			vertexnumber++;
		}
		vertexloop = vertextraverse(m);
	}
}
