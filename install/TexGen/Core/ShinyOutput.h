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

#ifndef SHINY_OUTPUT_H
#define SHINY_OUTPUT_H

#include "ShinyNode.h"
#include "ShinyZone.h"

#include <string>

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	enum OUTPUT_WIDTH {
		OUTPUT_WIDTH_HIT = 6,
		OUTPUT_WIDTH_TIME = 6,
		OUTPUT_WIDTH_PERC = 4,
		OUTPUT_WIDTH_SUM = 79,

		OUTPUT_WIDTH_DATA = 1+OUTPUT_WIDTH_HIT + 1 + 2*(OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1) + 1,
		OUTPUT_WIDTH_NAME = OUTPUT_WIDTH_SUM - OUTPUT_WIDTH_DATA
	};


//-----------------------------------------------------------------------------

	std::string OutputNodesAsString(const ProfileNode *a_root, uint32_t a_count);
	std::string OutputZonesAsString(const ProfileZone *a_root, uint32_t a_count);

} // namespace Shiny
#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
