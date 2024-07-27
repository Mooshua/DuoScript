
export type FolderType = "inherit" | "push"

export type Node = {
    name: string
}

export type Folder = Node | {

    children: {[string]: Folder},

    contents: {[string]: File},
    --  The Whether this folder begins a new luau environment
    --  or inherits the parent's environment
    env: FolderType,
};

export type File = Node | {
    --  The name of this file
    name: string,

    contents: buffer,
}

local Module = {}
local Root: Folder = { name = "", children = {}, contents = {} }

do
    local function QueryFileTree(segments: {string}, start: Folder)
        local step: Folder = start;
        local environments: {string} = { "root" };
        for idx, key in ipairs(segments) do

            --  Are we the last step?
            if idx == #segments then
                --  Check contents
                if step.contents[key] ~= nil then
                    return true, step.contents[key], environments
                else
                    return false, string.format("Failed to find file '%s' in folder '%s' (%s)", key, step.name, table.concat(segments, '.')), {}
                end
            end

            -- Otherwise, step down some more
            if step.children[key] ~= nil then
                step = step.children[key]
                
                if step.env == "push" then
                    table.insert(environments, step.name)
                end
            else
                return false, string.format("Failed to find file '%s' in folder '%s' (%s)", key, step.name, table.concat(segments, '.')), {}
            end

        end
        return false, "Unknown error", {};
    end

    --  @brief Find the specified path in the FileTree
    function Module:Query(path: string)
        local segments = string.split(path, '.')
        return QueryFileTree(segments, Root);
    end

end

do
    --  Determine if we should ignore a file based on it's extension
    local function ShouldIgnoreFile(name: string)
        local ignored_exts = {
            "exe", "so", "dyn", "dynlib", "ps1", "bat",
            "duo", "zip", "tar",
        }

        for _, ext in ipairs(ignored_exts) do
            if string.match(name, "(.+)%."..ext.."$") then
                return true
            end
        end

        return false
    end

    local function RemoveExtension(path: string)
        local segments = string.split(path, '.')

        if #segments ~= 2 then
            return false, string.format("File '%s' does not have exactly one extension!", path)
        else
            return true, segments[1]
        end
    end

    local function CollectLocalTree(path: string, self: Folder)
        --  Folder is our parent folder.
        --  Walk the path and add in all children and files, then walk children

        local files = { File:GetFiles(path) }
        for _, filePath in ipairs(files) do
            if ShouldIgnoreFile(filePath) then
                Log:Debug("Ignoring file " .. path .. filePath, "FileTree");
                continue;
            end;

            local success, extlessName = RemoveExtension(filePath)
            if not success then
                Log:Warn(extlessName, "FileTree")
            end

            if self.contents[extlessName] ~= nil then
                error("Duplicate filenames: " .. extlessName);
            end

            local success, file = File:Open(path .. filePath, false);

            if not success then
                Log:Warn("Error opening file " .. path .. filePath .. ": " .. file, "FileTree");
                continue;
            else
                Log:Debug("+ " .. path .. extlessName, "FileTree");
            end

            local success, length = file:Length();
            if not success then
                Log:Warn("Error reading stat of file: " .. length, "FileTree");
                continue;
            end

            local success, contents = file:Read(length);
            if not success then
                Log:Warn("Error reading contents of file: " .. contents, "FileTree");
                continue;
            end

            local object: File = {
                name = extlessName,
                contents = contents,
            };

            self.contents[extlessName] = object;
            --table.insert(self.contents, object);
        end

        local folders = { File:GetDirs(path) }
        for _, folderPath in ipairs(folders) do
            --  Dive baby
            local child: Folder = {
                name = folderPath,
                contents = {},
                children = {},
                env = "inherit",
            }

            CollectLocalTree(path .. folderPath .. "/", child, self);
            self.children[folderPath] = child;
            --table.insert(self.children, child);
        end
    end

    --  Collect all files in the local path
    CollectLocalTree("./", Root);

    --  @brief Insert a folder object
    function Module:Insert(path: string, insert: Folder)

    end
end

return Module;