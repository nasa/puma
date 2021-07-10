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

#ifndef SHINY_MANAGER_H
#define SHINY_MANAGER_H

#include "ShinyZone.h"
#include "ShinyNode.h"
#include "ShinyNodePool.h"
#include "ShinyTools.h"
#include "ShinyOutput.h"

#include <iostream>


#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	struct ProfileManager {
		//NOTE: data-members are intentionally public because the
		//		class needs to fulfil the definition of an aggregate

		enum TABLE_SIZE {
			TABLE_SIZE_INIT = 256
		};

		tick_t _lastTick;

		ProfileNode* _curNode;

		uint32_t _tableMask; // = _tableSize - 1

		ProfileNodeTable* _nodeTable;

#if SHINY_PROFILER_LOOKUPRATE == TRUE
		uint64_t _lookupCount;
		uint64_t _lookupSuccessCount;
#endif

		uint32_t _tableSize;

		uint32_t nodeCount;
		uint32_t zoneCount;

		ProfileZone* _lastZone;

		ProfileNodePool* _lastNodePool;
		ProfileNodePool* _firstNodePool;

		ProfileNode rootNode;
		ProfileZone rootZone;

		bool _initialized;
		bool _firstUpdate;

		static ProfileNode* _dummyNodeTable[];

		static ProfileManager instance;

		//

		SHINY_INLINE void _appendTicksToCurNode(void) {
			register tick_t curTick;
			GetTicks(&curTick);

			_curNode->appendTicks(curTick - _lastTick);
			_lastTick = curTick;
		}

		ProfileNode* _lookupNode(ProfileNodeCache* a_cache, ProfileZone* a_zone);

		void _createNodeTable(uint32_t a_count);
		void _resizeNodeTable(uint32_t a_count);

		void _createNodePool(uint32_t a_count);
		void _resizeNodePool(uint32_t a_count);

		ProfileNode* _createNode(ProfileNodeCache* a_cache, ProfileZone* a_pZone);
		void _insertNode(ProfileNode* a_pNode);

		void _init(void) {
			_initialized = true;

			rootNode.beginEntry();
			GetTicks(&_lastTick);
		}

		void _uninit(void) {
			_initialized = false;

			rootNode.clear();
			rootNode.parent = &rootNode;
			rootNode.zone = &rootZone;
		}

#if SHINY_PROFILER_LOOKUPRATE == TRUE
		SHINY_INLINE void _incLookup(void) { _lookupCount++; }
		SHINY_INLINE void _incLookupSuccess(void) { _lookupSuccessCount++; }
		SHINY_INLINE float lookupSuccessRate(void) const { return ((float) _lookupSuccessCount) / ((float) _lookupCount); }

#else
		SHINY_INLINE void _incLookup(void) {}
		SHINY_INLINE void _incLookupSuccess(void) {}
		SHINY_INLINE float lookupSuccessRate(void) const { return -1; }
#endif

		void _resetZones(void);
		void _destroyNodes(void);

		SHINY_INLINE float tableUsage(void) const { return ((float) nodeCount) / ((float) _tableSize); }

		uint32_t staticMemInBytes(void) {
			// ASSUME: zones and cache are used as intended; throught the macros

			return sizeof(instance) + sizeof(_dummyNodeTable[0]) + sizeof(ProfileNode::_dummy)
				 + (zoneCount - 1) * (sizeof(ProfileZone) + sizeof(ProfileNodeCache));
		}

		uint32_t allocMemInBytes(void) {
			return _tableSize * sizeof(ProfileNode*)
				 + (_firstNodePool)? _firstNodePool->memoryUsageChain() : 0;
		}

		SHINY_INLINE void _beginNode(ProfileNodeCache* a_cache, ProfileZone* a_zone) {
			if (_curNode != (*a_cache)->parent)
				*a_cache = _lookupNode(a_cache, a_zone);

			_beginNode(*a_cache);
		}

		SHINY_INLINE void _beginNode(ProfileNode* a_node) {
			a_node->beginEntry();

			_appendTicksToCurNode();
			_curNode = a_node;
		}

		SHINY_INLINE void _endCurNode(void) {
			_appendTicksToCurNode();
			_curNode = _curNode->parent;
		}

		//

		void preLoad(void);

		void updateClean(void);
		void update(float a_damping = 0.9f);

		void clear(void);
		void destroy(void);

		bool output(const char *a_filename);
		bool output(std::ostream &a_ostream = std::cout);

		SHINY_INLINE std::string outputNodesAsString(void) { return OutputNodesAsString(&rootNode, nodeCount); }
		SHINY_INLINE std::string outputZonesAsString(void) { return OutputZonesAsString(&rootZone, zoneCount); }

		//

		static void enumerateNodes(void (*a_func)(const ProfileNode*),
			const ProfileNode* a_node = &instance.rootNode)
		{
			a_func(a_node);

			if (a_node->firstChild) enumerateNodes(a_func, a_node->firstChild);
			if (a_node->nextSibling) enumerateNodes(a_func, a_node->nextSibling);
		}

		template <class T>
		static void enumerateNodes(T* a_this, void (T::*a_func)(const ProfileNode*),
			const ProfileNode* a_node = &instance.rootNode)
		{
			(a_this->*a_func)(a_node);

			if (a_node->firstChild) enumerateNodes(a_this, a_func, a_node->firstChild);
			if (a_node->nextSibling) enumerateNodes(a_this, a_func, a_node->nextSibling);
		}

		static void enumerateZones(void (*a_func)(const ProfileZone*),
			const ProfileZone* a_zone = &instance.rootZone)
		{
			a_func(a_zone);

			if (a_zone->next) enumerateZones(a_func, a_zone->next);
		}

		template <class T>
		static void enumerateZones(T* a_this, void (T::*a_func)(const ProfileZone*),
			const ProfileZone* a_zone = &instance.rootZone)
		{
			(a_this->*a_func)(a_zone);

			if (a_zone->next) enumerateZones(a_this, a_func, a_zone->next);
		}
	};


//-----------------------------------------------------------------------------

	class ProfileAutoEndNode {
	public:

		SHINY_INLINE ~ProfileAutoEndNode() {
			ProfileManager::instance._endCurNode();
		}
	};

} // namespace Shiny

#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
