#include "lua_scripting.h"
#include "growl/core/api/scripting_api.h"
#include "growl/core/api/system_api.h"
#include "growl/core/error.h"
#include "growl/core/scripting/class.h"
#include "growl/core/scripting/object.h"
#include "growl/core/scripting/script.h"
#include "lauxlib.h"
#include "lua.h"
#include "lua_class.h"
#include "lua_error.h"
#include "lua_object.h"
#include "lua_script.h"
#include <cstring>
#include <iostream>
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
using Growl::ScriptingSignature;
using Growl::ScriptingType;
using Growl::SystemAPI;

struct LuaStack {
	int stack_count;
	lua_State* state;

	LuaStack(lua_State* state)
		: stack_count{0}
		, state{state} {}

	~LuaStack() {
		lua_pop(state, stack_count);
	}
};

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

bool luaPushArg(lua_State* state, ScriptingParam& param) {
	// TODO checks!
	switch (static_cast<Growl::ScriptingType>(param.index())) {
	case Growl::ScriptingType::Void:
		return false;
	case Growl::ScriptingType::Bool:
		lua_pushboolean(state, std::get<bool>(param));
		break;
	case Growl::ScriptingType::Float:
		lua_pushnumber(state, std::get<float>(param));
		break;
	case Growl::ScriptingType::Int:
		lua_pushinteger(state, std::get<int>(param));
		break;
	case Growl::ScriptingType::String:
		lua_pushstring(
			state, std::string(std::get<std::string_view>(param)).c_str());
		break;
	case Growl::ScriptingType::Ptr:
		lua_pushlightuserdata(
			state, const_cast<void*>(std::get<const void*>(param)));
		break;
	case Growl::ScriptingType::Object: {
		auto ptr = std::get<std::unique_ptr<Growl::Object>>(param).get();
		lua_rawgeti(
			state, LUA_REGISTRYINDEX,
			static_cast<Growl::LuaObject*>(ptr)->getRef());
		break;
	}
	case Growl::ScriptingType::Ref: {
		lua_rawgeti(
			state, LUA_REGISTRYINDEX,
			static_cast<Growl::LuaObject*>(std::get<Growl::Object*>(param))
				->getRef());
		break;
	}
	}
	return true;
}

int luaPushArgs(std::vector<ScriptingParam>& args, lua_State* state) {
	int args_count = 0;
	for (size_t i = 0; i < args.size(); i++) {
		if (luaPushArg(state, args[i])) {
			args_count++;
		}
	}
	return args_count;
}

ScriptingParam luaPull(lua_State* state, ScriptingType type) {
	switch (type) {
	case ScriptingType::Ptr:
		return lua_topointer(state, -1);
	case ScriptingType::Int:
		return static_cast<int>(lua_tointeger(state, -1));
	case ScriptingType::String:
		return std::string_view(lua_tostring(state, -1));
	case ScriptingType::Float:
		return static_cast<float>(lua_tonumber(state, -1));
	case ScriptingType::Bool:
		return static_cast<bool>(lua_toboolean(state, -1));
	case ScriptingType::Object:
		return std::make_unique<Growl::LuaObject>(
			state, luaL_ref(state, LUA_REGISTRYINDEX));
	default:
		return ScriptingParam();
	}
}

std::vector<ScriptingParam>
luaPullArgs(lua_State* state, ScriptingSigLua* signature) {
	std::vector<ScriptingParam> args(signature->n_args);
	for (int i = 0; i < signature->n_args; i++) {
		args[i] = luaPull(state, signature->args[i]);
	}
	return args;
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
		auto err = std::make_unique<LuaError>(
			std::string("Failed to execute script: ") +
			std::string(lua_tostring(this->state, -1)));
		lua_pop(this->state, 1);
		return Error(std::move(err));
	}
	return luaPull(this->state, script.getSignature().return_type);
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
			auto args = luaPullArgs(state, signature);
			lua_newtable(state);
			fn(std::make_unique<LuaSelf>(state).get(), ctx, args);
			luaL_setmetatable(state, metatable_name);
			return 1;
		},
		4);
	lua_settable(this->state, -3);
	lua_pop(this->state, 1);
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
			fn(std::make_unique<LuaSelf>(state).get(), ctx,
			   std::vector<ScriptingParam>());
			return 1;
		},
		3);
	lua_settable(this->state, -3);
	lua_pop(this->state, 1);
	return nullptr;
}

Error LuaScriptingAPI::addMethodToClass(
	Class* cls, const std::string& method_name,
	const ScriptingSignature& signature, ScriptingFn fn, void* context) {
	LuaStack stack{state};
	stack.stack_count++;
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
				case ScriptingType::Ptr: {
					luaL_checktype(state, i + stack_offset, LUA_TLIGHTUSERDATA);
					args[i] = static_cast<const void*>(
						lua_topointer(state, i + stack_offset));
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
				return 0;
			case ScriptingType::Object:
				lua_rawgeti(
					state, LUA_REGISTRYINDEX,
					static_cast<LuaObject*>(
						std::get<std::unique_ptr<Object>>(*res).get())
						->getRef());
				return 1;
			case ScriptingType::Ref:
				lua_rawgeti(
					state, LUA_REGISTRYINDEX,
					static_cast<Growl::LuaObject*>(
						std::get<Growl::Object*>(*res))
						->getRef());
				return 1;
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
			case ScriptingType::Bool:
				lua_pushboolean(state, std::get<bool>(*res));
				return 1;
			}
			return 0;
		},
		4);
	lua_settable(this->state, -3);
	return nullptr;
}

Result<std::unique_ptr<Growl::Object>> LuaScriptingAPI::executeConstructor(
	const std::string& class_name, std::vector<ScriptingParam>& args,
	ScriptingSignature signature) {
	LuaStack stack{this->state};
	std::string metatable_name = std::string("Growl::meta::" + class_name);
	luaL_getmetatable(this->state, metatable_name.c_str());
	lua_getfield(this->state, -1, "new");
	lua_pushvalue(this->state, -2);
	int n_args = luaPushArgs(args, this->state);
	stack.stack_count += 1;
	if (lua_pcall(this->state, n_args + 1, 1, 0)) {
		auto err = std::make_unique<LuaError>(
			std::string("Failed to execute method: ") +
			std::string(lua_tostring(this->state, -1)));
		return Error(std::move(err));
	}
	return std::unique_ptr<Growl::Object>(std::make_unique<Growl::LuaObject>(
		state, luaL_ref(state, LUA_REGISTRYINDEX)));
}

Result<ScriptingParam> LuaScriptingAPI::executeMethod(
	Object& obj, const std::string& method_name,
	std::vector<ScriptingParam>& args, ScriptingSignature signature) {
	LuaStack stack{this->state};
	lua_rawgeti(
		this->state, LUA_REGISTRYINDEX, static_cast<LuaObject&>(obj).getRef());
	lua_getfield(this->state, -1, method_name.c_str());
	lua_pushvalue(this->state, -2);
	int n_args = luaPushArgs(args, this->state);
	int n_returns = signature.return_type == ScriptingType::Void ? 0 : 1;
	stack.stack_count += n_returns + 1;
	if (lua_pcall(this->state, n_args + 1, n_returns, 0)) {
		auto err = std::make_unique<LuaError>(
			std::string("Failed to execute method: ") +
			std::string(lua_tostring(this->state, -1)));
		return Error(std::move(err));
	}
	return luaPull(this->state, signature.return_type);
}

void LuaSelf::setField(const std::string& name, ScriptingParam val) {
	lua_pushstring(this->state, name.c_str());
	luaPushArg(this->state, val);
	lua_settable(this->state, -3);
}

const ScriptingParam
LuaSelf::getField(const std::string& name, ScriptingType type) {
	LuaStack stack{this->state};
	lua_getfield(this->state, 1, name.c_str());
	stack.stack_count += 2;
	return luaPull(state, type);
}
