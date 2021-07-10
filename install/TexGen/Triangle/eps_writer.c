
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "triangle_internal.h"
#include "eps_writer.h"

// Defined in triangle.c
extern int plus1mod3[3];
extern int minus1mod3[3];

void eps_write_header(FILE *file, const char* title, rect *ps)
{
	fprintf(file, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(file, "%%%%Creator: libtriangle\n");
	fprintf(file, "%%%%Title: %s\n", title);
	fprintf(file, "%%%%Pages: 1\n");
	fprintf(file, "%%%%BoundingBox:  %d  %d  %d  %d\n",
		(int)ps->xmin, (int)ps->ymin,
		(int)ps->xmax, (int)ps->ymax);
	fprintf(file, "%%%%Document-Fonts: Times-Roman\n");
	fprintf(file, "%%%%LanguageLevel: 1\n");
	fprintf(file, "%%%%EndComments\n");
	fprintf(file, "%%%%BeginProlog\n");
	fprintf(file, "/inch {72 mul} def\n");
	fprintf(file, "%%%%EndProlog\n");
	fprintf(file, "%%%%Page: 1 1\n");
}

void eps_write_trailer(FILE *file)
{
	fprintf(file, "%%\n");
	fprintf(file, "restore  showpage\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%  End of page.\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%%%Trailer\n");
	fprintf(file, "%%%%EOF\n");
}

void eps_draw_clip(FILE *file, rect *ps, rect *clip)
{
	fprintf(file, "save\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%  Set the RGB color to very light gray.\n");
	fprintf(file, "%%\n");
	fprintf(file, "0.900  0.900  0.900 setrgbcolor\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%  Draw a gray border around the page.\n");
	fprintf(file, "%%\n");
	fprintf(file, "newpath\n");
	fprintf(file, "  %d  %d  moveto\n", (int)ps->xmin, (int)ps->ymin);
	fprintf(file, "  %d  %d  lineto\n", (int)ps->xmax, (int)ps->ymin);
	fprintf(file, "  %d  %d  lineto\n", (int)ps->xmax, (int)ps->ymax);
	fprintf(file, "  %d  %d  lineto\n", (int)ps->xmin, (int)ps->ymax);
	fprintf(file, "  %d  %d  lineto\n", (int)ps->xmin, (int)ps->ymin);
	fprintf(file, "stroke\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%  Set the RGB color to black.\n");
	fprintf(file, "%%\n");
	fprintf(file, "0.000  0.000  0.000 setrgbcolor\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%  Set the font and its size.\n");
	fprintf(file, "%%\n");
	fprintf(file, "/Times-Roman findfont\n");
	fprintf(file, "0.50 inch scalefont\n");
	fprintf(file, "setfont\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%  Print a title.\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%210  702  moveto\n");
	fprintf(file, "%%(Triangulation)  show\n");
	fprintf(file, "%%\n");
	fprintf(file, "%%  Define a clipping polygon.\n");
	fprintf(file, "%%\n");
	fprintf(file, "newpath\n");
	fprintf(file, "  %d  %d  moveto\n", (int)clip->xmin, (int)clip->ymin);
	fprintf(file, "  %d  %d  lineto\n", (int)clip->xmax, (int)clip->ymin);
	fprintf(file, "  %d  %d  lineto\n", (int)clip->xmax, (int)clip->ymax);
	fprintf(file, "  %d  %d  lineto\n", (int)clip->xmin, (int)clip->ymax);
	fprintf(file, "  %d  %d  lineto\n", (int)clip->xmin, (int)clip->ymin);
	fprintf(file, "clip newpath\n");
}

void eps_draw_edges(FILE *file, mesh *m, rect *ps, rect *ms)
{
	struct otri e, trisym;
	vertex p, q;
	triangle ptr; /* Temporary variable used by sym(). */

	fprintf(file, "%%\n");
	fprintf(file, "%%  Draw the triangles (mesh edges).\n");
	fprintf(file, "%%\n");

	eps_set_stroke(file, 0.6f, 0.6f, 0.6f, 0.4f);

	fprintf(file, "/L {\n"
		"2 dict begin\n"
		"/y2 exch def\n"
		"/x2 exch def\n"
		"/y1 exch def\n"
		"/x1 exch def\n"
		"gsave\n"
		"newpath x1 y1 moveto x2 y2 lineto stroke\n"
		"grestore\n"
		"end\n"
		"} def\n");

	traversalinit(&m->triangles);
	e.tri = triangletraverse(m);

	while (e.tri != (triangle *) NULL) {
		for (e.orient = 0; e.orient < 3; e.orient++) {
			sym(e, trisym);
			if ((e.tri < trisym.tri) || (trisym.tri == m->dummytri)) {
				org(e, p);
				dest(e, q);

				fprintf(file, "%d %d %d %d L\n",
					TX(p[0], ps, ms),
					TY(p[1], ps, ms),
					TX(q[0], ps, ms),
					TY(q[1], ps, ms));
			}
		}
		e.tri = triangletraverse(m);
	}
}

void eps_draw_segments(FILE *file, mesh *m, rect *ps, rect *ms)
{
	struct osub e;
	vertex p, q;

	fprintf(file, "%%\n");
	fprintf(file, "%%  Draw the segments.\n");
	fprintf(file, "%%\n");

	eps_set_stroke(file, 0.27f, 0.5f, 0.7f, 0.8f);

	traversalinit(&m->subsegs);
	e.ss = subsegtraverse(m);
	e.ssorient = 0;

	while (e.ss != (subseg *) NULL) {
		sorg(e, p);
		sdest(e, q);

		fprintf(file, "%d %d %d %d L\n",
			TX(p[0], ps, ms),
			TY(p[1], ps, ms),
			TX(q[0], ps, ms),
			TY(q[1], ps, ms));

		e.ss = subsegtraverse(m);
	}
}

void eps_draw_points(FILE *file, mesh *m, rect *ps, rect *ms)
{
	vertex e;

	fprintf(file, "%%\n");
	fprintf(file, "%%  Draw the vertices.\n");
	fprintf(file, "%%\n");

	eps_set_color(file, 0.0f, 0.4f, 0.0f);

	fprintf(file, "/P {\n"
				  "2 dict begin\n"
				  "/y exch def\n"
				  "/x exch def\n"
				  "gsave\n"
				  "newpath x y 1 0 360 arc fill\n"
				  "grestore\n"
				  "end\n"
				  "} def\n");

	traversalinit(&m->vertices);
	e = vertextraverse(m);

	while (e != (vertex) NULL) {
		if (vertextype(e) != UNDEADVERTEX) {
			fprintf(file, "%d %d P\n", TX(e[0], ps, ms), TY(e[1], ps, ms));
		}
		e = vertextraverse(m);
	}
}

void eps_set_color(FILE *file, float r, float g, float b)
{
	fprintf(file, "%.3g %.3g %.3g setrgbcolor\n", r, g, b);
	fprintf(file, "%%\n");
}

void eps_set_stroke(FILE *file, float r, float g, float b, float width)
{
	fprintf(file, "%.3g %.3g %.3g setrgbcolor\n", r, g, b);
	fprintf(file, "%.3g setlinewidth\n", width);
	fprintf(file, "%%\n");
}

void eps_update_metrics(rect *ps, rect *clip, rect *ms)
{
	double x_scale, y_scale;

	// Enlarge width 5% on each side
	x_scale = ms->xmax - ms->xmin;
	ms->xmax = ms->xmax + 0.05 * x_scale;
	ms->xmin = ms->xmin - 0.05 * x_scale;
	x_scale = ms->xmax - ms->xmin;

	// Enlarge height 5% on each side
	y_scale = ms->ymax - ms->ymin;
	ms->ymax = ms->ymax + 0.05 * y_scale;
	ms->ymin = ms->ymin - 0.05 * y_scale;
	y_scale = ms->ymax - ms->ymin;

	if (x_scale < y_scale) {
		int delta = rint((ps->xmax - ps->xmin) * (y_scale - x_scale) / (2.0 * y_scale));

		ps->xmax = ps->xmax - delta;
		ps->xmin = ps->xmin + delta;

		clip->xmax = clip->xmax - delta;
		clip->xmin = clip->xmin + delta;

		x_scale = y_scale;
	} else {
		int delta = rint((ps->ymax - ps->ymin) * (x_scale - y_scale) / (2.0 * x_scale));

		ps->ymax = ps->ymax - delta;
		ps->ymin = ps->ymin + delta;

		clip->ymax = clip->ymax - delta;
		clip->ymin = clip->ymin + delta;

		y_scale = x_scale;
	}
}