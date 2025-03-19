#pragma once
#include <lua.hpp>

namespace BEngine {
	struct LuaManager {
		static LuaManager& GetInstance() {
			static LuaManager instance;
			return instance;
		}
		
		lua_State* lState;

		void Init();
		void Proc();
	};
}