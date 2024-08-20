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

		namespace LuaVector {
			const char* LuaVector_metaTable = "LuaVector_metaTable";

#define getLuaVector(L, idx) \
				LuaVector_t* vec; \
				{ \
					LuaVector_t** luaVec = (LuaVector_t**)luaL_checkudata(L, idx, LuaVector_metaTable); \
					if (!luaVec || !*luaVec) { \
						lua_pushstring(L, "Invalid Datatype! Expected \"LuaVector\""); \
						return lua_error(L); \
					} else { \
						vec = *luaVec; \
					} \
				}

			struct LuaVector_t {

				float x, y, z;

				static int __gc(lua_State* L) {

					getLuaVector(L, 1);
					//delete vec;
					return 0;

				}

				static int __tostring(lua_State* L) {

					getLuaVector(L, 1);
					lua_pushstring(L, std::format("LuaVector<{}, {}, {}>", vec->x, vec->y, vec->z).c_str());
					return 1;
				}

				static int __index(lua_State* L) {

					getLuaVector(L, 1);
					const char* indexStr = luaL_checkstring(L, 2);

					switch (indexStr[0]) {
					case 'x':
					case 'X':
					case 1:
						lua_pushinteger(L, (lua_Integer)vec->x);
						break;

					case 'y':
					case 'Y':
					case 2:
						lua_pushinteger(L, (lua_Integer)vec->y);
						break;

					case 'z':
					case 'Z':
					case 3:
						lua_pushinteger(L, (lua_Integer)vec->z);
						break;

					default:
						lua_pushnil(L);
					}

					return 1;
				}
			};

			static int LuaVector_new(lua_State* L) {

				LuaVector_t* luaVector = (LuaVector_t*)lua_newuserdata(L, sizeof(LuaVector_t));

				float x = (float)luaL_checkinteger(L, 1);
				float y = (float)luaL_checkinteger(L, 2);
				float z = (float)luaL_checkinteger(L, 3);

				luaVector->x = x;
				luaVector->y = y;
				luaVector->z = z;

				luaL_getmetatable(L, LuaVector_metaTable);
				lua_setmetatable(L, -1);

				return 1;

			}

			static void register_LuaVector(lua_State* L) {

				luaL_newmetatable(L, LuaVector_metaTable);

				lua_pushstring(L, "__gc");
				lua_pushcfunction(L, LuaVector_t::__gc);
				lua_settable(L, -3);

				lua_pushstring(L, "__tostring");
				lua_pushcfunction(L, LuaVector_t::__tostring);
				lua_settable(L, -3);

				lua_pushstring(L, "__index");
				lua_pushcfunction(L, LuaVector_t::__index);
				//lua_newtable(L);


				//lua_settable(L, -3);

				lua_pop(L, 1);  // Metatable vom Stack entfernen

				lua_register(L, "LuaVector", LuaVector_new);

			}
		}
	}
//		namespace LuaEntity {
//			const char* LuaEntity_metaTable = "LuaEntity_metaTable";
//
//#define getLuaEntity(L, idx) \
//				LuaEntity_t* ent; \
//				{ \
//					LuaEntity_t** luaEnt = (LuaEntity_t**)luaL_checkudata(L, idx, LuaEntity_metaTable); \
//					if (!luaEnt || !*luaEnt) { \
//						lua_pushstring(L, "Invalid Datatype! Expected \"LuaEntity\""); \
//						return lua_error(L); \
//					} else { \
//						ent = *luaEnt; \
//					} \
//				} 
//
//			struct LuaEntity_t {
//				Entity* ent;
//				int entityID;
//
//				static int __gc(lua_State* L) {
//
//					getLuaEntity(L, 1);
//					std::cout << std::hex << (uintptr_t)ent << std::endl;;
//					return 0;
//
//				}
//
//				static int __tostring(lua_State* L) {
//
//					getLuaEntity(L, 1);
//					lua_pushstring(L, std::format("LuaEntity<{}>", ent->entityID).c_str());
//
//					return 1;
//
//				}
//
//				static int GetID(lua_State* L) {
//
//					getLuaEntity(L, 1);
//					lua_pushinteger(L, ent->entityID);
//
//					return 1;
//
//				}
//
//			};
//
//			static int LuaEntity_new(lua_State* L) {
//
//				LuaEntity_t* createdEnt = (LuaEntity_t*)lua_newuserdata(L, sizeof(LuaEntity_t));
//				int entityID = luaL_checkint(L, 1);
//
//				createdEnt->entityID = entityID;
//				createdEnt->ent = entityManager.GetEntity(entityID);
//
//				luaL_getmetatable(L, LuaEntity_metaTable);
//				lua_setmetatable(L, -2);
//
//				return 1;
//
//			}
//
//			static void register_LuaEntity(lua_State* L) {
//
//				luaL_newmetatable(L, LuaEntity_metaTable);
//
//				lua_pushstring(L, "__gc");
//				lua_pushcfunction(L, LuaEntity_t::__gc);
//				lua_settable(L, -3);
//
//				lua_pushstring(L, "__tostring");
//				lua_pushcfunction(L, LuaEntity_t::__tostring);
//				lua_settable(L, -3);
//
//				lua_pushstring(L, "__index");
//				lua_newtable(L);
//
//				lua_pushstring(L, "GetID");
//				lua_pushcfunction(L, LuaEntity_t::GetID);
//				lua_settable(L, -3);
//
//				lua_settable(L, -3);
//
//				lua_pop(L, 1);  // Metatable vom Stack entfernen
//
//				lua_register(L, "LuaEntity", LuaEntity_new);
//
//			}
//		}
//	}

	static const struct luaL_Reg lua_BLib[]{
		{"print", &BLua::print},
		{"include", &BLua::include},
		{NULL, NULL}
	};
}

BEngine::LuaManager BEngine::luaManager = {};

void luaL_openBLib(lua_State* L) {

	luaL_openlibs(L);

	lua_getglobal(L, "_G");
	luaL_setfuncs(L, lua_BLib, 0);
	lua_pop(L, 1);

	BLua::LuaVector::register_LuaVector(L);
	//BLua::LuaEntity::register_LuaEntity(L);

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
