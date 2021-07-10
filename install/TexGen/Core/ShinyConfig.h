/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (www.*)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
       you wrote the original software. If you use this software in a product, 
       an acknowledgment in the product documentation would be appreciated but is 
       not required.

    2. Altered source versions must be plainly marked as such, and must not be 
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SHINY_CONFIG_H
#define SHINY_CONFIG_H


//-----------------------------------------------------------------------------

#ifndef SHINY_PROFILER
#define SHINY_PROFILER				TRUE
#endif

#ifndef SHINY_PROFILER_LOOKUPRATE
#define SHINY_PROFILER_LOOKUPRATE	TRUE
#endif


//-----------------------------------------------------------------------------

#ifndef FALSE
#define FALSE	0x0
#endif

#ifndef TRUE
#define TRUE	0x1
#endif

#ifndef NULL
#define NULL	0
#endif


//-----------------------------------------------------------------------------

#define SHINY_PLATFORM_WIN32	0x1
#define SHINY_PLATFORM_POSIX	0x2

#if defined (_WIN32)
#   define SHINY_PLATFORM	SHINY_PLATFORM_WIN32

#else
//	ASSUME: POSIX-compliant OS
#   define SHINY_PLATFORM	SHINY_PLATFORM_POSIX
#endif


//-----------------------------------------------------------------------------

#define SHINY_COMPILER_MSVC		0x1
#define SHINY_COMPILER_GNUC		0x2
#define SHINY_COMPILER_OTHER	0x3

#if defined (_MSC_VER)
#   define SHINY_COMPILER	SHINY_COMPILER_MSVC

#elif defined (__GNUG__)
#   define SHINY_COMPILER	SHINY_COMPILER_GNUC

#else
#   define SHINY_COMPILER	SHINY_COMPILER_OTHER
#endif

#endif // ifndef SHINY_*_H
