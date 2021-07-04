# lpack
Pack Lua scripts into standalone Windows executables


## Compiling
Install the Lua/LuaJIT vcpkg package: `lua{version}:arch-windows-static`  
You must not have both the Lua and LuaJIT package installed at the same time.  

### Usage
`lpack.exe script.lua out.exe`

Example script:
```lua
local args = {...}
for k,v in pairs(args) do
    print(k,v)
end
```
Output: https://imgur.com/a/dy4hzcF
