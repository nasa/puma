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

#ifndef SHINY_NODE_H
#define SHINY_NODE_H

#include "ShinyData.h"
#include "ShinyTools.h"

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	struct ProfileNode {

		//NOTE: data-members are intentionally public because the
		//		class needs to fulfil the definition of an aggregate


		ProfileLastData _last;

		ProfileZone* zone;
		ProfileNode* parent;
		ProfileNode* nextSibling;

		ProfileNode* firstChild;
		ProfileNode* lastChild;

		uint32_t childCount;
		uint32_t entryLevel;

		ProfileNodeCache* _cache;

		ProfileData data;

		static ProfileNode _dummy;

		//

		void init(ProfileNode* a_parent, ProfileZone* a_zone, ProfileNodeCache* a_cache) {
			// NOTE: all member variables are assumed to be zero when allocated

			zone = a_zone;
			parent = a_parent;

			entryLevel = a_parent->entryLevel + 1;
			a_parent->addChild(this);

			_cache = a_cache;
		}

		void addChild(ProfileNode* a_child) {
			if (childCount++) {
				lastChild->nextSibling = a_child;
				lastChild = a_child;

			} else {
				lastChild = a_child;
				firstChild = a_child;
			}
		}

		void updateTree(float a_damping);

		void destroy(void) { *_cache = &_dummy; }

		SHINY_INLINE void appendTicks(tick_t a_elapsedTicks) { _last.selfTicks += a_elapsedTicks; }
		SHINY_INLINE void beginEntry(void) { _last.entryCount++; }

		bool isRoot(void) const { return (entryLevel == 0); }
		bool isDummy(void) const { return (this == &_dummy); }

		bool isEqual(const ProfileNode* a_parent, const ProfileZone* a_zone) const {
			return (parent == a_parent && zone == a_zone);
		}

		const ProfileNode* findNextInTree(void) const;

		void clear(void);
	};

} // namespace Shiny
#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
