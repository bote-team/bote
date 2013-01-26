require "lfs"
local currentdir = lfs.currentdir()
local allowed_types = {"hpp","h","cpp"}

local function files(path, allowed_types)
	assert(type(path) == "string")
	local length = string.len(path)
	if string.sub(path, length) == dirsep then
		if unix then path = string.sub(path, 1, length - 1)
		else path = string.sub(path, 1, length - 2)
		end
	end
	local result = {}
	local function sub_files(path)
		local attr = assert(lfs.attributes(path))
		if attr.mode == "directory" then
			for name, test in lfs.dir(path) do
				if string.sub(name, 1, 1) ~= "." then
					sub_files(string.format("%s%s%s", path, "\\", name))
				end
			end
		elseif attr.mode == "file" then
			local insert = true
			if allowed_types then
				local dot_index = string.find(string.reverse(path), "%.")
				insert = false
				if dot_index then
					local ending = string.sub(path, string.len(path) - dot_index + 2)
					for i, v in ipairs(allowed_types) do
						if v == ending then insert = true; break end
					end
				end
			end
			if insert then
				table.insert(result, path)
			end
		else assert(false)
		end
	end
	sub_files(path)
	return result
end

for index, filenamestring in ipairs(files(currentdir, allowed_types)) do
	local file = io.open(filenamestring, "rb")

	--check for CString variables passed to their own .Format calls
	local line_number = 1
	local var = ""
	local statement = ""
	for line in file:lines() do
		statement = statement .. line
		local s, e = string.find(statement, "%.Format%(")
		if s then
			local var_start = string.find(statement,"[^%s]")
			var = string.sub(statement, var_start, s - 1)
			local args_s, args_e = string.find(statement, "%b()", e)
			if args_s and args_e and args_s == e then
				local args = string.sub(statement, args_s, args_e)
				local evil = string.find(statement, "[%,]*[%s]+" .. var .. ",")
				evil = evil or string.find(statement, "," .. var .. ",")
				if evil then
					print(filenamestring .. ": " .. line_number .. ": CString passed to its own .Format call; this is undefined behavior, use += instead")
				end
				statement = ""
			end
		else
			statement = ""
		end
		line_number = line_number + 1
	end
	assert(statement == "")
	--reset file pointer to beginning
	file:seek("set")

	--look for calls to CString::AllocSysString which are likely to cause a memory leak
	local line_number = 1
	for line in file:lines() do
		if string.find(line, "%.AllocSysString") then
			print(filenamestring .. ": " .. line_number .. ": call to CString::AllocSysString; this allocates memory which needs to be freed, use CComBSTR(...) instead")
		end
		line_number = line_number + 1
	end

	file:close()
end
