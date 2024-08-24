#include "pch.h"
#include "LuaManager.h"
#include "EntityManager.h"
#include <iostream> 
#include "ErrorReporter.h"
#include <string> 
#include <format>

extern "C"
{
	namespace BLua {
		static int print(lua_State* L) {

			int nargs = lua_gettop(L);
			for (int i = 1; i <= nargs; ++i)
			{
				if (lua_isstring(L, i)) {
					size_t lStrSize = 0;
					std::cout << lua_tolstring(L, i, &lStrSize) << "\t";
				}

				std::cout << std::endl;
			}

			return 0;
		}

		static int include(lua_State* L) {

			std::string fileName = luaL_checkstring(L, 1);

			if (luaL_dofile(L, ("lua\\" + fileName).c_str()) != LUA_OK) {
				std::cout << lua_tostring(L, -1) << std::endl;
				lua_pushboolean(L, false);
			}
			else {
				lua_pushboolean(L, true);
			}

			return 1;

		}
	}

	static const struct luaL_Reg lua_BLib[]{
		{"print", &BLua::print},
		{"include", &BLua::include},
		{NULL, NULL}
	};
}

class LuaMyObject {
private:
	static const char* LUA_MYOBJECT;

public:
	// Constructor
	static void Register(lua_State* L) {
		lua_register(L, LUA_MYOBJECT, LuaMyObject::New);
		luaL_newmetatable(L, LUA_MYOBJECT);
		lua_pushcfunction(L, LuaMyObject::Delete);
		lua_setfield(L, -2, "__gc");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, LuaMyObject::Set);
		lua_setfield(L, -2, "set");
		lua_pushcfunction(L, LuaMyObject::Get);
		lua_setfield(L, -2, "get");
		lua_pop(L, 1);
	}

private:
	double x;

	// Private constructor
	LuaMyObject(double x) : x(x) {}

	// Lua binding for MyObject creation
	static int New(lua_State* L) {
		double x = luaL_checknumber(L, 1);
		*reinterpret_cast<LuaMyObject**>(lua_newuserdata(L, sizeof(LuaMyObject*))) = new LuaMyObject(x);
		luaL_setmetatable(L, LUA_MYOBJECT);
		return 1;
	}

	// Lua binding for MyObject deletion
	static int Delete(lua_State* L) {
		delete* reinterpret_cast<LuaMyObject**>(lua_touserdata(L, 1));
		return 0;
	}

	// Lua binding for MyObject::set method
	static int Set(lua_State* L) {
		LuaMyObject* obj = *reinterpret_cast<LuaMyObject**>(luaL_checkudata(L, 1, LUA_MYOBJECT));
		obj->x = luaL_checknumber(L, 2);
		return 0;
	}

	// Lua binding for MyObject::get method
	static int Get(lua_State* L) {
		LuaMyObject* obj = *reinterpret_cast<LuaMyObject**>(luaL_checkudata(L, 1, LUA_MYOBJECT));
		lua_pushnumber(L, obj->x);
		return 1;
	}
};

// Define the metatable name
const char* LuaMyObject::LUA_MYOBJECT = "MyObject";

class LuaVector {
public:
	static const char* LUA_VECTOR;

	XMVECTOR data;

	LuaVector(float x, float y, float z, float w) {
		XMFLOAT4 dataFloat(x, y, z, w);
		data = XMLoadFloat4(&dataFloat);
	}

	LuaVector(const XMVECTOR& vec) {
		this->data = vec;
	}

	static void Register(lua_State* L) {
		lua_register(L, LUA_VECTOR, LuaVector::New);
		luaL_newmetatable(L, LUA_VECTOR);

		// Setze __index auf die Funktion LuaVector::Index
		lua_pushcfunction(L, LuaVector::Index);
		lua_setfield(L, -2, "__index");

		lua_pushcfunction(L, LuaVector::Delete);
		lua_setfield(L, -2, "__gc");
		lua_pushcfunction(L, LuaVector::Add);
		lua_setfield(L, -2, "add");
		lua_pushcfunction(L, LuaVector::Subtract);
		lua_setfield(L, -2, "subtract");
		lua_pushcfunction(L, LuaVector::Scale);
		lua_setfield(L, -2, "scale");
		lua_pushcfunction(L, LuaVector::GetComponents);
		lua_setfield(L, -2, "getComponents");
		lua_pushcfunction(L, LuaVector::ToString);
		lua_setfield(L, -2, "toString");
		lua_pushcfunction(L, LuaVector::ToString);
		lua_setfield(L, -2, "__tostring");
		lua_pop(L, 1);
	}

private:
	static int New(lua_State* L) {
		float x = (float)luaL_checknumber(L, 1);
		float y = (float)luaL_checknumber(L, 2);
		float z = (float)luaL_checknumber(L, 3);
		float w = (float)luaL_optnumber(L, 4, 1.0);

		*reinterpret_cast<LuaVector**>(lua_newuserdata(L, sizeof(LuaVector*))) = new LuaVector(x, y, z, w);
		luaL_setmetatable(L, LUA_VECTOR);
		return 1;
	}

	static int Delete(lua_State* L) {
		delete* reinterpret_cast<LuaVector**>(lua_touserdata(L, 1));
		return 0;
	}

	// __index-Funktion zur Zugriffssteuerung auf die Komponenten
	static int Index(lua_State* L) {
		LuaVector* vec = *reinterpret_cast<LuaVector**>(luaL_checkudata(L, 1, LUA_VECTOR));
		const char* key = luaL_checkstring(L, 2);

		if (strcmp(key, "x") == 0) {
			lua_pushnumber(L, XMVectorGetX(vec->data));
		}
		else if (strcmp(key, "y") == 0) {
			lua_pushnumber(L, XMVectorGetY(vec->data));
		}
		else if (strcmp(key, "z") == 0) {
			lua_pushnumber(L, XMVectorGetZ(vec->data));
		}
		else if (strcmp(key, "w") == 0) {
			lua_pushnumber(L, XMVectorGetW(vec->data));
		}
		else {
			lua_getmetatable(L, 1);
			lua_getfield(L, -1, key);
			if (lua_isnil(L, -1)) {
				luaL_error(L, "Ungültiges Attribut '%s' für Vector", key);
			}
			return 1;
		}
		return 1;
	}

	static int Add(lua_State* L) {
		LuaVector* vec1 = *reinterpret_cast<LuaVector**>(luaL_checkudata(L, 1, LUA_VECTOR));
		LuaVector* vec2 = *reinterpret_cast<LuaVector**>(luaL_checkudata(L, 2, LUA_VECTOR));
		LuaVector* result = new LuaVector(XMVectorAdd(vec1->data, vec2->data));
		*reinterpret_cast<LuaVector**>(lua_newuserdata(L, sizeof(LuaVector*))) = result;
		luaL_setmetatable(L, LUA_VECTOR);
		return 1;
	}

	static int Subtract(lua_State* L) {
		LuaVector* vec1 = *reinterpret_cast<LuaVector**>(luaL_checkudata(L, 1, LUA_VECTOR));
		LuaVector* vec2 = *reinterpret_cast<LuaVector**>(luaL_checkudata(L, 2, LUA_VECTOR));
		LuaVector* result = new LuaVector(XMVectorSubtract(vec1->data, vec2->data));
		*reinterpret_cast<LuaVector**>(lua_newuserdata(L, sizeof(LuaVector*))) = result;
		luaL_setmetatable(L, LUA_VECTOR);
		return 1;
	}

	static int Scale(lua_State* L) {
		LuaVector* vec = *reinterpret_cast<LuaVector**>(luaL_checkudata(L, 1, LUA_VECTOR));
		float scalar = luaL_checknumber(L, 2);
		LuaVector* result = new LuaVector(XMVectorScale(vec->data, scalar));
		*reinterpret_cast<LuaVector**>(lua_newuserdata(L, sizeof(LuaVector*))) = result;
		luaL_setmetatable(L, LUA_VECTOR);
		return 1;
	}

	static int GetComponents(lua_State* L) {
		LuaVector* vec = *reinterpret_cast<LuaVector**>(luaL_checkudata(L, 1, LUA_VECTOR));
		lua_pushnumber(L, XMVectorGetX(vec->data));
		lua_pushnumber(L, XMVectorGetY(vec->data));
		lua_pushnumber(L, XMVectorGetZ(vec->data));
		lua_pushnumber(L, XMVectorGetW(vec->data));
		return 4;
	}

	static int ToString(lua_State* L) {
		LuaVector* vec = *reinterpret_cast<LuaVector**>(luaL_checkudata(L, 1, LUA_VECTOR));
		XMFLOAT4 storedVec;
		XMStoreFloat4(&storedVec, vec->data);

		std::string str = std::format("Vector({}, {}, {}, {})", storedVec.x, storedVec.y, storedVec.z, storedVec.w);
		lua_pushstring(L, str.c_str());

		return 1;
	}
};

const char* LuaVector::LUA_VECTOR = "Vector";

BEngine::LuaManager BEngine::luaManager = {};

void luaL_openBLib(lua_State* L) {

	luaL_openlibs(L);

	lua_getglobal(L, "_G");
	luaL_setfuncs(L, lua_BLib, 0);
	lua_pop(L, 1);

	LuaMyObject::Register(L);
	LuaVector::Register(L);

}

void BEngine::LuaManager::Init() {
	lState = luaL_newstate();
	luaL_openBLib(lState);

	if (luaL_dofile(lState, "lua\\test_script.lua") != LUA_OK) {
		std::cout << lua_tostring(lState, -1) << std::endl;
	}

	lua_close(lState);
	lState = nullptr;
}

void BEngine::LuaManager::Proc() {
	
}
