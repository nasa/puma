/* This file is automatically created by the build system.
 * Please make changes only in its source file triangle_version.h.in.
 */

#ifndef TRIANGLE_VERSION_H
#define TRIANGLE_VERSION_H

/* #undef TRIANGLE_ENABLE_ACUTE */

#ifdef TRIANGLE_ENABLE_ACUTE
#define TRIANGLE_VERSION "_acute"
#else
#define TRIANGLE_VERSION ""
#endif
#define TRIANGLE_VERSION_MAJOR 1
#define TRIANGLE_VERSION_MINOR 7
#define TRIANGLE_VERSION_PATCH 0

#define TRIANGLE_VERSION_INT ( ( TRIANGLE_VERSION_MAJOR * 100 + TRIANGLE_VERSION_MINOR ) * 100 + TRIANGLE_VERSION_PATCH )

#endif
