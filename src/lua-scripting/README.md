# LuaLunaFive
C++ bindings for Lua from the Lua Users forum.

Code available from:

[http://lua-users.org/wiki/LuaLunaFive](http://lua-users.org/wiki/LuaLunaFive).

# Example

```cpp
LuaClass::LuaClass(lua_State* L)
{
	ref_property = LUA_REFNIL;
}

const char LuaClass::className[] = "luaclass";
const Luna<LuaClass>::PropertyType LuaClass::properties[] =
{
	{ "property", &LuaClass::lua_get_property, &LuaClass::lua_set_property },
	{ 0 }
};

int LuaClass::lua_set_property(lua_State* L)
{
	luaL_unref(L, LUA_REGISTRYINDEX, ref_format);
	ref_format = luaL_ref(L, LUA_REGISTRYINDEX);
	return 0;
}

int LuaClass::lua_get_property(lua_State* L)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, ref_format);
	return 1;
}

const Luna<LuaClass>::FunctionType LuaClass::methods[] =
{
	{ "func", &LuaClass::lua_func },
	{ 0 }
};

int LuaClass::lua_func(lua_State* L)
{
	const char* str = luaL_checkstring(L, -1);
	// do something
	lua_pushinteger(L, 1);
	return 1;
}
```
