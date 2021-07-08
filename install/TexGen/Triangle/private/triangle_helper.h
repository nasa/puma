#ifndef TRIANGLE_HELPER_H
#define TRIANGLE_HELPER_H

#include "../triangle_api.h"

int triangle_check_context(context* c);
int triangle_check_behavior(behavior* b);
int triangle_check_triangleio(triangleio* io, int firstnumber);

void triangle_restore_pointmarkers(context* ctx, int *pointmarkers);

#endif /* TRIANGLE_HELPER_H */
