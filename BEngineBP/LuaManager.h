#pragma once
#include <lua.hpp>

namespace BEngine {
	struct LuaManager {
		
		lua_State* lState;

		void Init();
		void Proc();

	} extern luaManager;
}