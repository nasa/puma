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

#ifndef SHINY_MACROS_H
#define SHINY_MACROS_H

#include "ShinyManager.h"

#if SHINY_PROFILER == TRUE


//-----------------------------------------------------------------------------
// public preprocessors

#define PROFILER_UPDATE														\
	Shiny::ProfileManager::instance.update

#define PROFILER_OUTPUT														\
	Shiny::ProfileManager::instance.output

#define PROFILER_OUTPUT_TREE_STRING()										\
	Shiny::ProfileManager::instance.outputNodesAsString()

#define PROFILER_OUTPUT_FLAT_STRING()										\
	Shiny::ProfileManager::instance.outputZonesAsString()

#define PROFILER_DESTROY()													\
	Shiny::ProfileManager::instance.destroy()


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_ROOT_DATA()													\
	Shiny::ProfileManager::instance.rootZone.data


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_END()														\
	Shiny::ProfileManager::instance._endCurNode()


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_BEGIN( name )												\
																			\
	static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE(name), #name);				\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_BLOCK( name )												\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	PROFILE_BEGIN(name);


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_FUNC()														\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_FUNC(), __FUNCTION__);		\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_FUNC());


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_CODE( code )												\
	{																		\
		static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_CODE(), #code);		\
		_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_CODE());						\
		{ code; }															\
		PROFILE_END();														\
	}


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_EXTERN( name )										\
																			\
	_PROFILE_ZONE_DECLARE(extern, _PROFILE_ID_ZONE_SHARED(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_STATIC( name )										\
																			\
	_PROFILE_ZONE_DECLARE(static, _PROFILE_ID_ZONE_SHARED(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_DEFINE( name )										\
																			\
	_PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_SHARED(name), #name);


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_BEGIN( name )										\
																			\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_SHARED(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_BLOCK( name )										\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_SHARED(name));


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_DATA( name )											\
																			\
	_PROFILE_ID_ZONE_SHARED(name).data										\


//-----------------------------------------------------------------------------
// internal preprocessors

#define _PROFILE_ID_ZONE( name )			__ShinyZone_##name
#define _PROFILE_ID_ZONE_FUNC()				__ShinyZoneFunc
#define _PROFILE_ID_ZONE_CODE()				__ShinyZoneCode
#define _PROFILE_ID_ZONE_SHARED( name )		name##__ShinyZoneShared
#define _PROFILE_ID_BLOCK()					__ShinyBlock


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_DEFINE( id, string )									\
																			\
	Shiny::ProfileZone id = {												\
		NULL, Shiny::ProfileZone::STATE_HIDDEN, string,						\
		{ { 0, 0 }, { 0, 0 }, { 0, 0 } }									\
	};


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_DECLARE( prefix, id )									\
																			\
	prefix Shiny::ProfileZone id;


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_BLOCK_DEFINE( id )											\
																			\
	Shiny::ProfileAutoEndNode SHINY_UNUSED id;


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_BEGIN( id )											\
	{																		\
		static Shiny::ProfileNodeCache cache =								\
			&Shiny::ProfileNode::_dummy;									\
																			\
		Shiny::ProfileManager::instance._beginNode(&cache, &id);			\
	}

//-----------------------------------------------------------------------------

#else // #if SHINY_PROFILER == TRUE

namespace Shiny {

	SHINY_INLINE ProfileData GetEmptyData() {
		ProfileData a = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
		return a;
	}
	
	SHINY_INLINE void DummyUpdate(float a = 0.0f) { /* meditate */ }
	SHINY_INLINE bool DummyOutput(const char *a = NULL) { return false; }
	SHINY_INLINE bool DummyOutput(std::ostream &a) { return false; }
	
}

#define PROFILER_UPDATE					DummyUpdate
#define PROFILER_OUTPUT					DummyOutput
#define PROFILER_OUTPUT_TREE_STRING()	std::string()
#define PROFILER_OUTPUT_FLAT_STRING()	std::string()
#define PROFILER_DESTROY()
#define PROFILE_BEGIN(name)
#define PROFILE_BLOCK(name)
#define PROFILE_FUNC()
#define PROFILE_CODE(code)				{ code; }
#define PROFILE_SHARED_EXTERN(name)
#define PROFILE_SHARED_STATIC(name)
#define PROFILE_SHARED_DEFINE(name)
#define PROFILE_SHARED_BEGIN(name)
#define PROFILE_SHARED_BLOCK(name)
#define PROFILE_SHARED_DATA(name)		Shiny::GetEmptyData()
#define PROFILE_ROOT_DATA()				Shiny::GetEmptyData()

#endif

#endif // ifndef SHINY_*_H
