#ifndef EPS_WRITER_H
#define EPS_WRITER_H

#include "../triangle.h"

#define rint(x) ((int)((x)+0.5))  /* MSC does not have rint() function */

/* Transform points from mesh to page coordinates. */
#define TX(x, ps, ms) (int)floor(((ms->xmax - x) * ps->xmin + (x - ms->xmin) * ps->xmax) / (ms->xmax - ms->xmin))
#define TY(y, ps, ms) (int)floor(((ms->ymax - y) * ps->ymin + (y - ms->ymin) * ps->ymax) / (ms->ymax - ms->ymin))

void eps_write_header(FILE *file, const char* filename, rect *ps);
void eps_write_trailer(FILE *file);

void eps_draw_clip(FILE *file, rect *ps, rect *clip);
void eps_draw_edges(FILE *file, mesh *m, rect *ps, rect *ms);
void eps_draw_segments(FILE *file, mesh *m, rect *ps, rect *ms);
void eps_draw_points(FILE *file, mesh *m, rect *ps, rect *ms);

void eps_set_stroke(FILE *file, float r, float g, float b, float width);
void eps_set_color(FILE *file, float r, float g, float b);

void eps_update_metrics(rect *ps, rect *ps_clip, rect *ms);

#endif /* EPS_WRITER_H */
