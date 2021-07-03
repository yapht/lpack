# lpack
Pack Lua scripts into standalone Windows executables


## Compiling
Make sure you have vcpkg and `lua:x86-windows-static` or `lua:x64-windows-static` depending on which you want to compile for

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
