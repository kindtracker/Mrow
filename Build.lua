local lfs = require("lfs")

local Sources = { "Source" }

local CCompiler = "cc"
local CXXCompiler = "cc"
local Linker = "cc"
local CFlags = "-O0"
local LDFlags = "-O0 -lunicorn"

local Output = "mrow"
local OutputFolder = "Output"
local BuildFolder = "Build"

local function ListAllFiles(Directory, Extension)
	local Files = {}

	for File in lfs.dir(Directory) do
		if File ~= "." and File ~= ".." then
			local FullPath = Directory .. "/" .. File
			local Attributes = lfs.attributes(FullPath)

			if Attributes.mode == "directory" then
				local SubFiles = ListAllFiles(FullPath, Extension)

				for _, SubFile in ipairs(SubFiles) do
					table.insert(Files, SubFile)
				end
			elseif File:match("%." .. Extension .. "$") then
				table.insert(Files, FullPath)
			end
		end
	end

	return Files
end

local function Execute(Command)
	print(Command)

	local Success, Type, Code = os.execute(Command)

	if not Success then
		error(string.format("Command failed (exit code: %s): %s", tostring(Code), Command))
	end

	return Success
end

local function MakeDirectory(Directory)
	Execute('mkdir -p "' .. Directory .. '"')
end

local function CopyFile(Source, Destination)
	local InputPtr = assert(io.open(Source, "rb"))
	local OutputPtr = assert(io.open(Destination, "wb"))

	OutputPtr:write(InputPtr:read("*a"))

	InputPtr:close()
	OutputPtr:close()
end

MakeDirectory(OutputFolder)
MakeDirectory(BuildFolder)

local CFiles = {}
for _, SourceFolder in ipairs(Sources) do
	local CFilesInSource = ListAllFiles(SourceFolder, "c")
	local CXXFilesInSource = ListAllFiles(SourceFolder, "cpp")
	for _, File in ipairs(CXXFilesInSource) do
		table.insert(CFilesInSource, File)
	end

	for _, File in ipairs(CFilesInSource) do
		table.insert(CFiles, File)
	end
end

local ObjectFiles = {}
for _, File in ipairs(CFiles) do
	local ObjectFile = BuildFolder .. "/" .. File:sub(#BuildFolder + 3) .. ".o"

	local Parent = ObjectFile:match("(.+)/[^/]+$")
	if Parent then
		MakeDirectory(Parent)
	end

	if File:find(".cpp") then
		Execute(string.format("%s -c %q -o %q %s", CXXCompiler, File, ObjectFile, CFlags))
	else
		Execute(string.format("%s -c %q -o %q %s", CCompiler, File, ObjectFile, CFlags))
	end

	ObjectFile = string.format("%q", ObjectFile)
	table.insert(ObjectFiles, ObjectFile)
end

Execute(string.format("%s %s -o %q %s", Linker, table.concat(ObjectFiles, " "), OutputFolder .. "/" .. Output, LDFlags))
