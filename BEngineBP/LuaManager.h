#pragma once
#include "lua/lua.hpp"

namespace BEngine {
	struct LuaManager {
		
		lua_State* lState;

		void Init();

	} extern luaManager;
}