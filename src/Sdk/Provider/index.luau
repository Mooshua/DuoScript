
--  ======================
--  The DuoScript Compiler
--
--  This is a tool that transforms a directory
--  into a plug-and-play .duo package (a zip file),
--  written as a duo plugin itself for dogfooding reasons.
--
--  Our goal is to take a set of source files and transform
--  them into a simple filesystem layout within a ZIP file.
--  In other words, we're making it as easy as possible for the
--  C++ plugin manager to interact with this plugin.

local _, FileTree = require "Tree.FileTree"

function FileResolver(path: string)
    Log:Debug(path, "FileResolver")

    if path == "mod" then
        return [[if b < 0 then local a = b + 1 print(a, b) end return true]]
    end

    return [[

    local a = require "mod"

    if type(v) == "String" then
        print("v is a string")
    end

    ]]
end

function ModuleResolver(name: any)
    Log:Debug(name, "ModuleResolver")

    return name;
end

function EnvironmentResolver(name: string)
    Log:Debug(name, "EnvironmentResolver")
    local success, _, environment = FileTree:Query(name);

    if not success then
        Log:Error("Failed to fetch environment for ")
        return nil
    end

    return name;
end

local frontend = LuauFrontend:New(FileResolver, ModuleResolver, EnvironmentResolver);

--local results = frontend:Check("hi")
--[[
for i = 1, results.ErrorCount do
    local code, error, module, location = results:GetError(i);
    Log:Error(code .. ": " .. error, module .. " " .. location)
end]]

local s, f, e = FileTree:Query("index");
print(s,f, e[#e]);

while true do
    Fiber:Sleep(math.random(1,20))
end