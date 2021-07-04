#include "compiler.h"
#include <assert.h>
#include <iostream>

int bc_writer(lua_State*, const void* data, size_t size, void* ud)
{
    assert(ud);
    compiler::bcbuffer* buffer = (compiler::bcbuffer*)ud;

    if (buffer->size + size > buffer->capacity) {
        std::cout << "Failed to write bytecode. Buffer too small.\n";
        return 1;
    }

    std::memcpy(buffer->buf + buffer->size, data, size);
    buffer->size += size;

    // No errors
    return 0;
}

void print_status(lua_State* L, int status)
{
    switch (status)
    {
    case LUA_ERRMEM:
        std::cout << "Lua ran out of memory!\n";
        break;
    case LUA_ERRFILE:
        std::cout << "Cannot find file.";
        break;
    case LUA_ERRSYNTAX:
        std::cout << "Syntax error: " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1);
        break;
    }
}

bool compiler::compile_bytecode(const std::string& path, bcbuffer* buf, bool strip_debug_info)
{
    assert(buf);

    lua_State* L = luaL_newstate();
    assert(L && "Failed to create Lua state");

    // Compile the chunk and push it onto the stack
    int status = luaL_loadfile(L, path.c_str());
    if (status != LUA_OK)
    {
        print_status(L, status);
        return false;
    }

    // Write the bytecode to our buffer
#ifndef LUAJIT
    status = lua_dump(L, bc_writer, buf, strip_debug_info);
#else
    status = lua_dump(L, bc_writer, buf);
#endif

    if (status != 0)
    {
        std::cout << "Failed to write bytecode.\n";
        return false;
    }

    // Pop the compiled chunk off the stack
    lua_pop(L, 1);

    lua_close(L);
	return true;
}