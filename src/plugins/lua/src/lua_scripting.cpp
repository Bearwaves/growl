#include "lua_scripting.h"
#include "growl/core/api/scripting_api.h"
#include "growl/core/api/system_api.h"
#include "growl/core/error.h"
#include "growl/core/scripting/class.h"
#include "growl/core/scripting/script.h"
#include "lauxlib.h"
#include "lua.h"
#include "lua_class.h"
#include "lua_error.h"
#include "lua_object.h"
#include "lua_script.h"
#include <cstring>
#include <memory>
#include <string>
#include <string_view>

using Growl::Class;
using Growl::ClassSelf;
using Growl::Error;
using Growl::LuaError;
using Growl::LuaScript;
using Growl::LuaScriptingAPI;
using Growl::LuaSelf;
using Growl::Result;
using Growl::Script;
using Growl::ScriptingAPI;
using Growl::ScriptingParam;
using Growl::ScriptingType;
using Growl::SystemAPI;

struct ScriptingSigLua {
	ScriptingType return_type;
	ScriptingType* args;
	int n_args;
};

// return not actually used
int checkArgMetatable(lua_State* state, int arg, const char* metatable) {
	luaL_checktype(state, arg, LUA_TTABLE);
	luaL_getmetatable(state, metatable);
	lua_getmetatable(state, arg);
	int cmp = lua_compare(state, -1, -2, LUA_OPEQ);
	lua_pop(state, 2);
	if (!cmp) {
		return luaL_argerror(
			state, 1,
			(std::string("Must have metatable ") + std::string(metatable))
				.c_str());
	}
	return 0;
}

void luaPushArgs(std::vector<ScriptingParam>& args, lua_State* state) {
	for (size_t i = 0; i < args.size(); i++) {
		switch (static_cast<Growl::ScriptingType>(args[i].index())) {
		case Growl::ScriptingType::Void:
			continue;
		case Growl::ScriptingType::Ptr:
			// TODO checks!
			lua_pushlightuserdata(
				state, const_cast<void*>(std::get<const void*>(args[i])));
			break;
		case Growl::ScriptingType::Int:
			lua_pushinteger(state, std::get<int>(args[i]));
			break;
		case Growl::ScriptingType::String:
			lua_pushstring(
				state,
				std::string(std::get<std::string_view>(args[i])).c_str());
			break;
		case Growl::ScriptingType::Float:
			lua_pushnumber(state, std::get<float>(args[i]));
			break;
		case Growl::ScriptingType::Object:
			auto ptr = std::get<std::unique_ptr<Growl::Object>>(args[i]).get();
			lua_rawgeti(
				state, LUA_REGISTRYINDEX,
				static_cast<Growl::LuaObject*>(ptr)->getRef());
			break;
		}
	}
}

ScriptingParam
luaPullReturn(Growl::ScriptingType return_type, lua_State* state) {
	ScriptingParam ret;
	switch (return_type) {
	case Growl::ScriptingType::Ptr:
		ret = lua_topointer(state, -1);
		break;
	case Growl::ScriptingType::Int:
		ret = static_cast<int>(lua_tointeger(state, -1));
		break;
	case Growl::ScriptingType::String:
		ret = std::string_view(lua_tostring(state, -1));
		break;
	case Growl::ScriptingType::Float:
		ret = static_cast<float>(lua_tonumber(state, -1));
		break;
	case Growl::ScriptingType::Object:
		ret = std::make_unique<Growl::LuaObject>(
			state, luaL_ref(state, LUA_REGISTRYINDEX));
		break;
	default:
		ret = ScriptingParam();
		break;
	}
	lua_pop(state, 1);
	return ret;
}

Error LuaScriptingAPI::init() {
	this->state = luaL_newstate();
	luaL_openlibs(state);

	if (auto err = mountGrowlScripts(api)) {
		return err;
	}

	api.system().log("LuaScriptingAPI", "Created Lua state");

	return nullptr;
}

void LuaScriptingAPI::dispose() {
	api.system().log("LuaScriptingAPI", "Closing Lua state");
	lua_close(this->state);
}

Result<std::unique_ptr<Script>> LuaScriptingAPI::createScript(
	std::string&& source, ScriptingSignature signature) {
	return std::unique_ptr<Script>(
		std::make_unique<LuaScript>(std::move(source), signature));
}

Result<ScriptingParam> LuaScriptingAPI::execute(Script& script) {
	int n_returns =
		script.getSignature().return_type == ScriptingType::Void ? 0 : 1;
	if (luaL_loadstring(this->state, script.getSource().c_str()) ||
		lua_pcall(this->state, 0, n_returns, 0)) {
		lua_tostring(this->state, -1);
		auto err = std::make_unique<LuaError>(
			std::string("Failed to execute script: ") +
			std::string(lua_tostring(this->state, -1)));
		lua_pop(this->state, 1);
		return Error(std::move(err));
	}
	return luaPullReturn(script.getSignature().return_type, this->state);
}

Result<std::unique_ptr<Class>>
LuaScriptingAPI::createClass(std::string&& name, bool is_static) {
	if (is_static) {
		lua_newtable(this->state);
	} else {
		luaL_newmetatable(this->state, ("Growl::meta::" + name).c_str());
		lua_pushvalue(state, -1);
		lua_setfield(state, -2, "__index");
	}
	lua_setglobal(this->state, name.c_str());
	return std::make_unique<Class>(std::move(name), this, is_static);
}

Error LuaScriptingAPI::setField(
	Object& obj, const std::string& name, ScriptingParam value) {
	auto ref = static_cast<LuaObject&>(obj).getRef();
	lua_rawgeti(this->state, LUA_REGISTRYINDEX, ref);
	if (!lua_istable(this->state, -1)) {
		return std::make_unique<LuaError>(
			"Object must be a table to set fields");
	}
	lua_pushstring(this->state, name.c_str());
	switch (static_cast<ScriptingType>(value.index())) {
	case ScriptingType::Ptr:
		lua_pushlightuserdata(
			this->state, const_cast<void*>(std::get<const void*>(value)));
		break;
	default:
		return std::make_unique<LuaError>("Unsupported field type");
	}
	lua_settable(this->state, -3);
	lua_rawseti(this->state, LUA_REGISTRYINDEX, ref);
	return nullptr;
}

Error LuaScriptingAPI::setClass(Object& obj, const std::string& class_name) {
	auto ref = static_cast<LuaObject&>(obj).getRef();
	lua_rawgeti(this->state, LUA_REGISTRYINDEX, ref);
	if (!lua_istable(this->state, -1)) {
		return std::make_unique<LuaError>(
			"Object must be a table to set fields");
	}
	luaL_getmetatable(
		this->state, (std::string("Growl::meta::") + class_name).c_str());
	lua_pushvalue(this->state, -1);
	lua_setfield(this->state, -3, "__index");
	lua_setmetatable(this->state, -2);
	lua_rawseti(this->state, LUA_REGISTRYINDEX, ref);
	return nullptr;
}

Error LuaScriptingAPI::addConstructorToClass(
	Class* cls, const ScriptingSignature& signature, ScriptingFn fn,
	void* context) {
	std::string metatable_name = std::string("Growl::meta::" + cls->getName());
	luaL_getmetatable(this->state, metatable_name.c_str());
	lua_pushstring(this->state, "new");

	lua_pushlightuserdata(this->state, reinterpret_cast<void*>(fn));
	lua_pushlightuserdata(this->state, context);
	void* buf = lua_newuserdata(
		this->state, sizeof(ScriptingSigLua) +
						 sizeof(ScriptingParam) * signature.arg_types.size());
	ScriptingSigLua* sig = new (buf) ScriptingSigLua();
	sig->return_type = signature.return_type;
	sig->n_args = signature.arg_types.size();
	sig->args = reinterpret_cast<ScriptingType*>(
		static_cast<ScriptingSigLua*>(buf) + 1);
	std::memcpy(
		sig->args, signature.arg_types.data(),
		signature.arg_types.size() * sizeof(ScriptingType));
	lua_pushstring(this->state, metatable_name.c_str());
	lua_pushcclosure(
		this->state,
		[](lua_State* state) -> int {
			auto fn = reinterpret_cast<ScriptingFn>(
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(1))));
			void* ctx =
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(2)));
			ScriptingSigLua* signature = static_cast<ScriptingSigLua*>(
				lua_touserdata(state, lua_upvalueindex(3)));
			const char* metatable_name =
				lua_tostring(state, lua_upvalueindex(4));
			std::vector<ScriptingParam> args(signature->n_args);
			for (int i = 0; i < signature->n_args; i++) {
				switch (signature->args[i]) {
				case ScriptingType::String:
					luaL_checktype(state, i + 1, LUA_TSTRING);
					args[i] = std::string{lua_tostring(state, i + 1)};
					break;
				default:
					continue;
				}
			}
			lua_newtable(state);
			fn(std::make_unique<LuaSelf>(state).get(), ctx, args);
			luaL_setmetatable(state, metatable_name);
			return 1;
		},
		4);
	lua_settable(this->state, -3);
	return nullptr;
}

Error LuaScriptingAPI::addDestructorToClass(
	Class* cls, ScriptingFn fn, void* context) {
	std::string metatable_name = std::string("Growl::meta::" + cls->getName());
	luaL_getmetatable(this->state, metatable_name.c_str());
	lua_pushstring(this->state, "__gc");

	lua_pushlightuserdata(this->state, reinterpret_cast<void*>(fn));
	lua_pushlightuserdata(this->state, context);
	lua_pushstring(this->state, metatable_name.c_str());
	lua_pushcclosure(
		this->state,
		[](lua_State* state) -> int {
			auto fn = reinterpret_cast<ScriptingFn>(
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(1))));
			void* ctx =
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(2)));
			const char* metatable_name =
				lua_tostring(state, lua_upvalueindex(3));
			lua_newtable(state);
			fn(std::make_unique<LuaSelf>(state).get(), ctx,
			   std::vector<ScriptingParam>());
			luaL_setmetatable(state, metatable_name);
			return 1;
		},
		3);
	lua_settable(this->state, -3);
	return nullptr;
}

Error LuaScriptingAPI::addMethodToClass(
	Class* cls, const std::string& method_name,
	const ScriptingSignature& signature, ScriptingFn fn, void* context) {
	int stack_offset = 1;
	if (cls->isStatic()) {
		lua_getglobal(this->state, cls->getName().c_str()); // TODO err
	} else {
		auto metatable_name = std::string("Growl::meta::" + cls->getName());
		luaL_getmetatable(this->state, metatable_name.c_str());
		stack_offset = 2;
	}
	lua_pushstring(this->state, method_name.c_str());
	lua_pushlightuserdata(this->state, reinterpret_cast<void*>(fn));
	lua_pushlightuserdata(this->state, context);
	void* buf = lua_newuserdata(
		this->state, sizeof(ScriptingSigLua) +
						 sizeof(ScriptingParam) * signature.arg_types.size());
	ScriptingSigLua* sig = new (buf) ScriptingSigLua();
	sig->return_type = signature.return_type;
	sig->n_args = signature.arg_types.size();
	sig->args = reinterpret_cast<ScriptingType*>(
		static_cast<ScriptingSigLua*>(buf) + 1);
	std::memcpy(
		sig->args, signature.arg_types.data(),
		signature.arg_types.size() * sizeof(ScriptingType));
	lua_pushinteger(this->state, stack_offset);
	lua_pushcclosure(
		this->state,
		[](lua_State* state) -> int {
			auto fn = reinterpret_cast<ScriptingFn>(
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(1))));
			void* ctx =
				const_cast<void*>(lua_topointer(state, lua_upvalueindex(2)));
			ScriptingSigLua* signature = static_cast<ScriptingSigLua*>(
				lua_touserdata(state, lua_upvalueindex(3)));
			int stack_offset = lua_tointeger(state, lua_upvalueindex(4));
			std::vector<ScriptingParam> args(signature->n_args);
			for (int i = 0; i < signature->n_args; i++) {
				switch (signature->args[i]) {
				case ScriptingType::String:
					luaL_checktype(state, i + stack_offset, LUA_TSTRING);
					args[i] =
						std::string_view{lua_tostring(state, i + stack_offset)};
					break;
				case ScriptingType::Float: {
					luaL_checktype(state, i + stack_offset, LUA_TNUMBER);
					args[i] = static_cast<float>(
						lua_tonumber(state, i + stack_offset));
					break;
				}
				default:
					continue;
				}
			}
			auto res = fn(std::make_unique<LuaSelf>(state).get(), ctx, args);
			// TODO check error
			switch (signature->return_type) {
			case ScriptingType::Void:
			case ScriptingType::Object:
				return 0;
			case ScriptingType::Ptr:
				lua_pushlightuserdata(
					state, const_cast<void*>(std::get<const void*>(*res)));
				return 1;
			case ScriptingType::Int:
				lua_pushnumber(state, std::get<int>(*res));
				return 1;
			case ScriptingType::String:
				lua_pushstring(
					state,
					std::string(std::get<std::string_view>(*res)).c_str());
				return 1;
			case ScriptingType::Float:
				lua_pushnumber(state, std::get<float>(*res));
				return 1;
			}
			return 0;
		},
		4);
	lua_settable(this->state, -3);
	return nullptr;
}

Result<ScriptingParam> LuaScriptingAPI::executeMethod(
	Object& obj, const std::string& method_name,
	std::vector<ScriptingParam>& args, ScriptingSignature signature) {
	lua_rawgeti(
		this->state, LUA_REGISTRYINDEX, static_cast<LuaObject&>(obj).getRef());
	lua_getfield(this->state, -1, method_name.c_str());
	lua_pushvalue(this->state, -2);
	luaPushArgs(args, this->state);
	if (lua_pcall(
			this->state, args.size() + 1,
			signature.return_type == ScriptingType::Void ? 0 : 1, 0)) {
		lua_tostring(this->state, -1);
		auto err = std::make_unique<LuaError>(
			std::string("Failed to execute method: ") +
			std::string(lua_tostring(this->state, -1)));
		lua_pop(this->state, 1);
		return Error(std::move(err));
	}
	return luaPullReturn(signature.return_type, this->state);
}

void LuaSelf::setField(const std::string& name, void* val) {
	lua_pushstring(this->state, name.c_str());
	lua_pushlightuserdata(this->state, val);
	lua_settable(this->state, -3);
}

const void* LuaSelf::getField(const std::string& name) {
	lua_getfield(this->state, 1, name.c_str());
	return lua_topointer(this->state, -1);
}
