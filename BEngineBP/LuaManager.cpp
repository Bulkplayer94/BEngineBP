#include "LuaManager.h"
#include "EntityManager.h"
#include <iostream> 
#include "ErrorReporter.h"

extern "C"
{
	namespace BLua {
		static int print(lua_State* L) {

			int nargs = lua_gettop(L);
			for (int i = 1; i <= nargs; ++i)
			{
				if (lua_isstring(L, i)) {
					size_t lStrSize = 0;
					std::cout << lua_tolstring(L, i, &lStrSize) << std::endl;
				}
			}

			return 0;
		}
	}

	static const struct luaL_Reg lua_BLib[]{
		{"print", &BLua::print},
		{NULL, NULL}
	};
}

BEngine::LuaManager BEngine::luaManager = {};

void luaL_openBLib(lua_State* L) {

	luaL_openlibs(L);

	lua_getglobal(L, "_G");
	// lua_register(L, NULL, lua_BLib);
	luaL_setfuncs(L, lua_BLib, 0);
	lua_pop(L, 1);

}

void BEngine::LuaManager::Init() {
	lState = luaL_newstate();
	luaL_openBLib(lState);

	luaL_dostring(lState, "print(\"Test from Lua\")");

	lua_close(lState);
	lState = nullptr;
}
