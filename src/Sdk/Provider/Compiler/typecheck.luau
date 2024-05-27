
local Module = {}
local Compiler = {}
Compiler.__index = Compiler;

function Module.New()
    return setmetatable({}, Compiler);
end

return Module
