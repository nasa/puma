
#ifndef TRIANGLE_EXPORT_H
#define TRIANGLE_EXPORT_H

#ifdef TRIANGLE_STATIC_DEFINE
#  define TRIANGLE_EXPORT
#  define TRIANGLE_NO_EXPORT
#else
#  ifndef TRIANGLE_EXPORT
#    ifdef triangle_api_EXPORTS
        /* We are building this library */
#      define TRIANGLE_EXPORT 
#    else
        /* We are using this library */
#      define TRIANGLE_EXPORT 
#    endif
#  endif

#  ifndef TRIANGLE_NO_EXPORT
#    define TRIANGLE_NO_EXPORT 
#  endif
#endif

#ifndef TRIANGLE_DEPRECATED
#  define TRIANGLE_DEPRECATED __declspec(deprecated)
#endif

#ifndef TRIANGLE_DEPRECATED_EXPORT
#  define TRIANGLE_DEPRECATED_EXPORT TRIANGLE_EXPORT TRIANGLE_DEPRECATED
#endif

#ifndef TRIANGLE_DEPRECATED_NO_EXPORT
#  define TRIANGLE_DEPRECATED_NO_EXPORT TRIANGLE_NO_EXPORT TRIANGLE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef TRIANGLE_NO_DEPRECATED
#    define TRIANGLE_NO_DEPRECATED
#  endif
#endif

#endif /* TRIANGLE_EXPORT_H */
