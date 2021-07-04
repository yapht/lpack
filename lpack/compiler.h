#pragma once

#ifndef LUAJIT
#include <lua.hpp>
#else
#include <luajit/lua.hpp>
#define LUA_OK 0
#endif
#include <string>

namespace compiler
{
	struct bcbuffer
	{
		uint8_t* buf;
		uint32_t capacity;
		uint32_t size;
	};

	bool compile_bytecode(const std::string& path, bcbuffer* buf, bool strip_debug_info);
}