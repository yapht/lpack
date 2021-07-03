#include "resource.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <Windows.h>

typedef struct _bytecode_info {
    void* data;
    uint32_t size;
} bytecode_info;

int find_bytecode_resource(void** data, uint32_t* size)
{
    HRSRC hBytecodeResource = FindResource(NULL, MAKEINTRESOURCE(IDR_CODE1), RT_RCDATA);
    if (hBytecodeResource == NULL)
    {
        printf("Failed to find Lua bytecode resource. Error code: %i\n", GetLastError());
        return 0;
    }

    HGLOBAL hGlobResource = LoadResource(NULL, hBytecodeResource);
    if (hGlobResource == NULL)
    {
        printf("Failed to load Lua bytecode resource. Error code: %i\n", GetLastError());
        return 0;
    }

    PVOID pResource = LockResource(hGlobResource);
    DWORD dwResourceSize = SizeofResource(NULL, hBytecodeResource);

    *data = pResource;
    *size = dwResourceSize;

    return 1;
}

const char* bc_reader(lua_State* L, void* data, int* size)
{
    assert(data);
    bytecode_info* bc = (bytecode_info*)data;
    *size = bc->size;
    return (const char*)bc->data;
}

void print_status(lua_State* L, int status)
{
    switch (status)
    {
    case LUA_ERRMEM:
        printf("Lua ran out of memory while executing program.\n");
        break;
    case LUA_ERRSYNTAX:
        luaL_traceback(L, L, lua_tostring(L, -1), 0);
        printf("Syntax error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        break;
    case LUA_ERRRUN:
        luaL_traceback(L, L, lua_tostring(L, -1), 0);
        printf("Runtime error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        break;
    }
}

int main(int argc, char** argv)
{
    void* bc;
    uint32_t bc_size;

    if (!find_bytecode_resource(&bc, &bc_size))
        return 1;

    bytecode_info info = { .data = bc, .size = bc_size };

    lua_State* L = luaL_newstate();
    if (!L)
    {
        printf("Failed to create Lua state.\n");
        return 1;
    }

    luaL_openlibs(L);

    // Load the bytecode in 'bt' mode
    int status = lua_load(L, (lua_Reader)bc_reader, (void*)&info, "program", 0);

    if (status != LUA_OK)
    {
        print_status(L, status);
        return 1;
    }

    // Push command-line arguments
    for (int i = 0; i < argc; i++)
        lua_pushstring(L, argv[i]);

    status = lua_pcall(L, argc, 0, 0);

    if (status != LUA_OK) {
        print_status(L, status);
        return 1;
    }

    lua_close(L);
    return 0;
}