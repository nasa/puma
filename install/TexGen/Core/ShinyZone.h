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

#ifndef SHINY_ZONE_H
#define SHINY_ZONE_H

#include "ShinyData.h"

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	struct ProfileZone {

		enum STATE {
			STATE_HIDDEN = 0,
			STATE_INITIALIZED,
			STATE_UPDATING
		};

		//NOTE: data-members are intentionally public because the
		//		class needs to fulfil the definition of an aggregate

		ProfileZone* next;

		STATE _state;

		mutable const char* name;

		ProfileData data;

		//

		bool isInited(void) const { return _state != 0; }

		void init(ProfileZone* a_prev) {
			_state = STATE_INITIALIZED;

			a_prev->next = this;
		}

		void uninit(void) {
			_state = STATE_HIDDEN;
		}

		void preUpdateChain(void) {
			data.clearCurrent();
			if (next) next->preUpdateChain();
		}

		void updateChain(float a_damping) {
			data.computeAverage(a_damping);
			if (next) next->updateChain(a_damping);
		}

		bool isUpdating(void) const { return _state == STATE_UPDATING; }

		void enableUpdating(void) { _state = STATE_UPDATING; }
		void disableUpdating(void) { _state = STATE_INITIALIZED; }
	};

} // namespace Shiny
#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
